/**
* @file SelfLocator.cpp
* Implements a class that performs self-localization using a particle filter.
*
* - May choose different implementations for pose calculation
*
* @author <a href="mailto:Tim.Laue@dfki.de">Tim Laue</a>
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
*/

#include <limits>
#include "SelfLocator.h"
#include "SelfLocatorParameter.h"
#include "PoseCalculator/PoseCalculatorParticleHistory.h"
#include "PoseCalculator/PoseCalculator2DBinning.h"
#include "PoseCalculator/PoseCalculatorBestParticle.h"
#include "PoseCalculator/PoseCalculatorOverallAverage.h"
#include "PoseCalculator/PoseCalculatorKMeansClustering.h"
#include "Tools/Math/GaussianDistribution3D.h"
#include "Tools/Settings.h"
#include "Tools/Streams/InStreams.h"
#include "Tools/Streams/OutStreams.h"
#include "Tools/Team.h"
#include "SensorModels/CenterCircleSensorModel.h"
#include "SensorModels/GoalPostsSensorModel.h"
#include "SensorModels/LineSensorModel.h"
#include "SensorModels/CornersSensorModel.h"
#include <algorithm>


SelfLocator::SelfLocator() : 
  parameter(new SelfLocatorParameter),
  samples(0), slowWeighting(0.0), fastWeighting(0.0), updatedBySensors(false),
  poseCalculator(0), perceptValidityChecker(0), lastPoseComputationTimeStamp(0),
  sampleTemplateGenerator(theGoalPercept, theFrameInfo, theFieldDimensions, theOdometryData,
  parameter->standardDeviationGoalpostSampleBearingDistance,
  parameter->standardDeviationGoalpostSampleSizeDistance,
  parameter->clipTemplateGeneration,
  parameter->clipTemplateGenerationRangeX,
  parameter->clipTemplateGenerationRangeY),
  gameInfoPenaltyLastFrame(PENALTY_NONE),
  gameInfoGameStateLastFrame(STATE_INITIAL)
{
  observations.reserve(100);
  selectedObservations.reserve(100);
  selectedIndices.reserve(100);
  lines.reserve(100);
}

void SelfLocator::setNumberOfSamples(const int num)
{
  if(samples)
    delete samples;
  samples = new SampleSet<Sample>(num);
  sensorModelWeightings.resize(num);

  if(poseCalculator)
    delete poseCalculator;
  poseCalculator = new PoseCalculatorParticleHistory< Sample, SampleSet<Sample> >(*samples);
  poseCalculatorType = POSE_CALCULATOR_PARTICLE_HISTORY;
}

SelfLocator::~SelfLocator()
{
  delete poseCalculator;
  delete samples;
  for(unsigned int i=0; i<sensorModels.size(); ++i)
    delete sensorModels[i];
  if(perceptValidityChecker)
    delete perceptValidityChecker;
  delete parameter;
}

void SelfLocator::init()
{
  fieldModel.init(theFieldDimensions, parameter->maxCrossingLength);
  for(unsigned int i=0; i<sensorModels.size(); ++i)
    delete sensorModels[i];
  sensorModels.clear();
  if(perceptValidityChecker)
    delete perceptValidityChecker;
  perceptValidityChecker = new PerceptValidityChecker(theFieldDimensions);
  sensorModels.push_back(new GoalPostsSensorModel(*parameter,
    theGoalPercept, theFrameInfo, theFieldDimensions, theCameraMatrix, *perceptValidityChecker));
  sensorModels.push_back(new CenterCircleSensorModel(*parameter,
    theLinePercept, theFrameInfo, theFieldDimensions, theCameraMatrix, *perceptValidityChecker));
  sensorModels.push_back(new LineSensorModel(*parameter,
    lines, theFrameInfo, theFieldDimensions, theCameraMatrix, *perceptValidityChecker,
    fieldModel));
  sensorModels.push_back(new CornersSensorModel(*parameter,
    theLinePercept, theFrameInfo, theFieldDimensions, theCameraMatrix, *perceptValidityChecker,
    fieldModel));

  setNumberOfSamples(parameter->numberOfSamples);
  sampleTemplateGenerator.init();
  std::vector<Pose2D> poses;    
  std::vector<Pose2D> standardDeviations;
  if(parameter->knownStartPose)
  {
    poses.push_back(parameter->startPose);
    standardDeviations.push_back(parameter->startPoseStandardDeviation);
  }
  // If no start position is known, spread samples uniformly across state space
  initSamplesAtGivenPositions(poses, standardDeviations);
}

void SelfLocator::initSamplesAtGivenPositions(const std::vector<Pose2D>& poses,
    const std::vector< Pose2D >& standardDeviations)
{
  for(int i = 0; i < samples->size(); ++i)
  {
    Sample& sample(samples->at(i));
    if(poses.size())
    {
      // Select one of the poses from the list:
      unsigned int index = random(poses.size());
      Pose2D pose = poses[index];
      Pose2D stdDev = standardDeviations[index];
      // Create sample:
      sample.translation.x = static_cast<int>(pose.translation.x);
      sample.translation.x += sampleTriangularDistribution(static_cast<int>(stdDev.translation.x));
      sample.translation.y = static_cast<int>(pose.translation.y);
      sample.translation.y += sampleTriangularDistribution(static_cast<int>(stdDev.translation.y));
      double rotation = pose.rotation;
      rotation += sampleTriangularDistribution(stdDev.rotation);
      normalize(rotation);
      sample.angle = rotation;
      sample.rotation = Vector2<int>(int(cos(rotation) * 1024), int(sin(rotation) * 1024));
    }
    else //No given positions, spread uniformly:
    {
      Pose2D pose(theFieldDimensions.randomPoseOnField());
      sample.translation = Vector2<int>(int(pose.translation.x), int(pose.translation.y));
      sample.rotation = Vector2<int>(int(cos(pose.rotation) * 1024), int(sin(pose.rotation) * 1024));
      sample.angle = sample.rotation.angle();
    }
  }
  lastOdometry = theOdometryData;
  poseCalculator->init();
}

void SelfLocator::update(SelfLocatorSampleSet& sampleSet)
{
  //sample set needs to have been updated within this frame:
  if(lastPoseComputationTimeStamp != theFrameInfo.time)
  {
    RobotPose dummyPose;
    update(dummyPose);
  }
  sampleSet.weightingsSum = totalWeighting;
  samples->link(sampleSet.sampleSetProxy);
}

void SelfLocator::update(RobotPose& robotPose)
{
  MODIFY("module:SelfLocator:parameter", *parameter);

  // recreate sampleset if number of samples was changed
  if (parameter->numberOfSamples != samples->size())
    init();

  // Maybe pose has already been computed by call to other update function (for clusters)
  if(lastPoseComputationTimeStamp == theFrameInfo.time)
  {
    robotPose = lastComputedPose;
    return;
  }
  // Normal computation:
  preExecution(robotPose);
  bool templatesOnly(false);
  bool odometryOnly(false);
  DEBUG_RESPONSE("module:SelfLocator:templates only", templatesOnly = true; );
  DEBUG_RESPONSE("module:SelfLocator:odometry only", odometryOnly = true; );

  if(templatesOnly) //debug
  {
    if(sampleTemplateGenerator.templatesAvailable())
      for(int i = 0; i < this->samples->size(); ++i)
        generateTemplate(samples->at(i));
    poseCalculator->calcPose(robotPose);
  }
  else if(odometryOnly) //debug
  {
    robotPose += theOdometryData - lastOdometry;
    lastOdometry = theOdometryData;
  }
  else //normal case
  {
    motionUpdate(updatedBySensors);
    updatedBySensors = applySensorModels();
    if(updatedBySensors)
    {
      adaptWeightings();
      resampling();
    }
    poseCalculator->calcPose(robotPose);
  }

  lastComputedPose = robotPose;
  lastPoseComputationTimeStamp = theFrameInfo.time;

  // send own position to the team mates
  TEAM_OUTPUT(idTeamMateRobotPose, bin, robotPose);	

  // drawings
  DECLARE_DEBUG_DRAWING("module:SelfLocator:poseCalculator", "drawingOnField"); // Draws the internal representations for pose computation
  COMPLEX_DRAWING("module:SelfLocator:poseCalculator", poseCalculator->draw());

  DECLARE_DEBUG_DRAWING("module:SelfLocator:perceptValidityChecker", "drawingOnField"); // Draws the internal representations for percept validity checking
  COMPLEX_DRAWING("module:SelfLocator:perceptValidityChecker", perceptValidityChecker->draw());

  DECLARE_DEBUG_DRAWING("module:SelfLocator:samples", "drawingOnField"); // Draws the internal representations of the goal locator
  COMPLEX_DRAWING("module:SelfLocator:samples", drawSamples());

  DECLARE_DEBUG_DRAWING("module:SelfLocator:templates", "drawingOnField"); // Draws all available templates
  COMPLEX_DRAWING("module:SelfLocator:templates", sampleTemplateGenerator.draw(); );

  DECLARE_DEBUG_DRAWING("module:SelfLocator:selectedPoints", "drawingOnField");

  fieldModel.draw();
}

bool cmpClusterPairs(std::pair<int,int> a, std::pair<int,int> b)
{
  return a.second > b.second;
}

void SelfLocator::update(RobotPoseHypotheses& robotPoseHypotheses)
{
  robotPoseHypotheses.hypotheses.clear();
  //update only available for two types of pose calculation:
  if(poseCalculatorType != POSE_CALCULATOR_PARTICLE_HISTORY && poseCalculatorType != POSE_CALCULATOR_K_MEANS_CLUSTERING)  
    return;
  //sample set needs to have been updated within this frame:
  if(lastPoseComputationTimeStamp != theFrameInfo.time)
  {
    RobotPose dummyPose;
    update(dummyPose);
  }

  if(poseCalculatorType == POSE_CALCULATOR_PARTICLE_HISTORY)  
  {
    //get hypotheses:
    PoseCalculatorParticleHistory< Sample, SampleSet<Sample> >* poseHistoryCalc 
      = (PoseCalculatorParticleHistory< Sample, SampleSet<Sample> >*)poseCalculator;
    std::vector< std::pair<int,int> > clusters = poseHistoryCalc->getClusters();
    std::sort(clusters.begin(),clusters.end(),cmpClusterPairs);
    const unsigned int MAX_HYPOTHESES = std::min<unsigned int>(robotPoseHypotheses.MAX_HYPOTHESES, clusters.size());
    for(unsigned int i=0; i<MAX_HYPOTHESES; ++i)
    {
      // No mini clusters, please:
      if(clusters[i].second <= 3)  
        break;
      // Compute average position:
      RobotPoseHypothesis newHypothesis;
      poseHistoryCalc->calcPoseOfCluster(newHypothesis, clusters[i].first);
      Vector2<int> newTrans(static_cast<int>(newHypothesis.translation.x), static_cast<int>(newHypothesis.translation.y));
      // Compute variance of position:
      int varianceX(0);
      int varianceY(0);
      for(int j = 0; j < samples->size(); ++j)
      {
        Sample& s(samples->at(j));
        if(s.cluster == clusters[i].first)
        {
          varianceX += (s.translation.x - newTrans.x) * (s.translation.x - newTrans.x);
          varianceY += (s.translation.y - newTrans.y) * (s.translation.y - newTrans.y);
        }
      }
      varianceX /= (clusters[i].second - 1);
      varianceY /= (clusters[i].second - 1);
      newHypothesis.positionCovariance[0][0] = static_cast<double>(varianceX);
      newHypothesis.positionCovariance[1][1] = static_cast<double>(varianceY);
      // Compute covariance:
      int cov_xy(0);
      for(int j = 0; j < samples->size(); ++j)
      {
        Sample& s(samples->at(j));
        if(s.cluster == clusters[i].first)
          cov_xy += (s.translation.x - newTrans.x) * (s.translation.y - newTrans.y);
      }
      cov_xy /= (clusters[i].second - 1);
      newHypothesis.positionCovariance[0][1] = newHypothesis.positionCovariance[1][0] = static_cast<double>(cov_xy);
      // Finally add to list:
      robotPoseHypotheses.hypotheses.push_back(newHypothesis);
    }
  }

  if(poseCalculatorType == POSE_CALCULATOR_K_MEANS_CLUSTERING)  
  {
    //get hypotheses:
    PoseCalculatorKMeansClustering< Sample, SampleSet<Sample>, 5, 1000 >* poseKMeansCalc 
      = (PoseCalculatorKMeansClustering< Sample, SampleSet<Sample>, 5, 1000 >*)poseCalculator;
    for(unsigned int i=0; i<5; ++i)
    {
      RobotPoseHypothesis newHypothesis;
      newHypothesis.validity = poseKMeansCalc->getClusterValidity(i);
      if(newHypothesis.validity > 0)
      {
        poseKMeansCalc->getClusterPose(newHypothesis, i);
        newHypothesis.positionCovariance[0][0] = 0.1;   //TODO: calculate covariances
        newHypothesis.positionCovariance[0][1] = 0.1;
        newHypothesis.positionCovariance[1][0] = 0.1;
        newHypothesis.positionCovariance[1][1] = 0.1;
        robotPoseHypotheses.hypotheses.push_back(newHypothesis);
      }
    }
  }
}

void SelfLocator::preExecution(RobotPose& robotPose)
{
  sampleTemplateGenerator.bufferNewPerceptions();

  // Reset all weightings to 1.0
  for(int i = 0; i < samples->size(); ++i)
    samples->at(i).weighting = 1.0;

  DEBUG_RESPONSE("module:SelfLocator:createFieldModel", fieldModel.create(); );

  // Table for checking point / line validity:
  DEBUG_RESPONSE("module:SelfLocator:PerceptValidityChecker:saveGoalNetTable", perceptValidityChecker->saveGoalNetTable(););
  DEBUG_RESPONSE("module:SelfLocator:PerceptValidityChecker:recomputeGoalNetTable", perceptValidityChecker->computeGoalNetTable(););

  // change module for pose calculation or use debug request to reload the pose calculator
  PoseCalculatorType newPoseCalculatorType(poseCalculatorType);
  MODIFY_ENUM("module:SelfLocator:poseCalculatorType", 
              newPoseCalculatorType, NUMBER_OF_POSE_CALCULATORS, getPoseCalculatorTypeName);

  bool reloadPoseCalculator = (newPoseCalculatorType != poseCalculatorType);
  DEBUG_RESPONSE("module:SelfLocator:reloadPoseCalculator", 
                 reloadPoseCalculator=true; );

  if(reloadPoseCalculator)
  {
    delete poseCalculator;
    switch(newPoseCalculatorType)
    {
    case POSE_CALCULATOR_2D_BINNING: 
      poseCalculator = new PoseCalculator2DBinning< Sample, SampleSet<Sample>, 10 >
        (*samples, theFieldDimensions);
      break;
    case POSE_CALCULATOR_PARTICLE_HISTORY: 
      poseCalculator = new PoseCalculatorParticleHistory< Sample, SampleSet<Sample> >(*samples);
      poseCalculator->init();
      break;
    case POSE_CALCULATOR_BEST_PARTICLE: 
      poseCalculator = new PoseCalculatorBestParticle< Sample, SampleSet<Sample> >(*samples);
      break;
    case POSE_CALCULATOR_OVERALL_AVERAGE:
      poseCalculator = new PoseCalculatorOverallAverage< Sample, SampleSet<Sample> >(*samples);
      break;
    case POSE_CALCULATOR_K_MEANS_CLUSTERING:
      poseCalculator = new PoseCalculatorKMeansClustering< Sample, SampleSet<Sample>, 5, 1000 >
        (*samples, theFieldDimensions);
      break;
    default: ASSERT(false);
    }
    poseCalculatorType = newPoseCalculatorType;
  }

  // if the considerGameState flag is set, reset samples when necessary
  if(parameter->considerGameState)
  {
    // penalty shoot: if game state switched to playing reset samples to start pos
    if(theGameInfo.secondaryState == STATE2_PENALTYSHOOT || theBehaviorConfiguration.agent.compare("penalty") == 0)
    {
      if(gameInfoGameStateLastFrame == STATE_SET && theGameInfo.state == STATE_PLAYING)
      {
        init();
      }
    }
    // normal game: if penalty is over reset samples to reenter positions
    if(theGameInfo.secondaryState == STATE2_NORMAL || theBehaviorConfiguration.agent.compare("soccer") == 0)
    {
      if(gameInfoPenaltyLastFrame != PENALTY_NONE && theRobotInfo.penalty == PENALTY_NONE)
      {
        std::vector<Pose2D> poses;
        std::vector<Pose2D> stdDevs;
        poses.push_back(Pose2D(-pi/2.0, 0.0, theFieldDimensions.yPosLeftSideline));
        poses.push_back(Pose2D(pi/2.0, 0.0, theFieldDimensions.yPosRightSideline));
        stdDevs.push_back(Pose2D(0.2, 200, 200));
        stdDevs.push_back(Pose2D(0.2, 200, 200));
        initSamplesAtGivenPositions(poses, stdDevs);
      }
    }
    gameInfoPenaltyLastFrame = theRobotInfo.penalty;
    gameInfoGameStateLastFrame = theGameInfo.state;
  }

  // Initialize sample set again:
  DEBUG_RESPONSE("module:SelfLocator:resetSamples", init(););
  // Set team color in robot pose
  robotPose.ownTeamColorForDrawing = theOwnTeamInfo.teamColor == TEAM_BLUE ? ColorRGBA(0,0,255) : ColorRGBA(255,0,0);
}

void SelfLocator::motionUpdate(bool noise)
{
  Pose2D odometryOffset = theOdometryData - lastOdometry;

  const double transNoise = noise ? parameter->translationNoise : 0;
  const double rotNoise = noise ? parameter->rotationNoise : 0;
  const int transX = (int) odometryOffset.translation.x;
  const int transY = (int) odometryOffset.translation.y;
  const double dist = odometryOffset.translation.abs();
  const double angle = fabs(odometryOffset.rotation);

  lastOdometry += Pose2D(odometryOffset.rotation, transX, transY);

  // precalculate rotational error that has to be adapted to all samples
  const double rotError = std::max(rotNoise,
                             std::max(dist * parameter->movedDistWeight,
                                      angle * parameter->movedAngleWeight));

  // precalculate translational error that has to be adapted to all samples
  const int transXError = (int) std::max(transNoise,
                                   std::max(fabs(transX * parameter->majorDirTransWeight),
                                            fabs(transY * parameter->minorDirTransWeight)));
  const int transYError = (int) std::max(transNoise,
                                   std::max(fabs(transY * parameter->majorDirTransWeight),
                                            fabs(transX * parameter->minorDirTransWeight)));
  for(int i = 0; i < samples->size(); i++)
  {
    Sample& s(samples->at(i));

    // the translational error vector
    const Vector2<int> transOffset((((transX - transXError) << 10) + 512 + ((transXError << 1) + 1) * (rand() & 0x3ff)) >> 10,
                                   (((transY - transYError) << 10) + 512 + ((transYError << 1) + 1) * (rand() & 0x3ff)) >> 10);

    // update the sample
    s.translation = Vector2<int>(((s.translation.x << 10) + transOffset.x * s.rotation.x - transOffset.y * s.rotation.y + 512) >> 10,
                                 ((s.translation.y << 10) + transOffset.x * s.rotation.y + transOffset.y * s.rotation.x + 512) >> 10);
    s.angle += odometryOffset.rotation + (randomDouble() * 2 - 1) * rotError;
    s.angle = normalize(s.angle);
    s.rotation = Vector2<int>(int(cosf((float)s.angle) * 1024), int(sinf((float)s.angle) * 1024));

    // clip to field boundary
    Vector2<double> translationAsDouble(s.translation.x, s.translation.y);
    theFieldDimensions.clipToCarpet(translationAsDouble);
    s.translation.x = static_cast<int>(translationAsDouble.x);
    s.translation.y = static_cast<int>(translationAsDouble.y);
  }	
}

bool SelfLocator::applySensorModels()
{
  if(!theCameraMatrix.isValid)
    return false;

  // collect all observations
  // goals and the center circle will always be selected
  selectedObservations.clear();
  for(int i = 0; i < GoalPercept::NUMBER_OF_GOAL_POSTS; ++i)
    if(theGoalPercept.posts[i].timeWhenLastSeen == theFrameInfo.time)
      selectedObservations.push_back(SensorModel::Observation(SensorModel::Observation::GOAL_POST, i));
  if(selectedObservations.empty()) // no known posts
    for(int i = 0; i < GoalPercept::NUMBER_OF_UNKNOWN_GOAL_POSTS; ++i)
      if(theGoalPercept.unknownPosts[i].timeWhenLastSeen == theFrameInfo.time)
        selectedObservations.push_back(SensorModel::Observation(SensorModel::Observation::GOAL_POST, i + GoalPercept::NUMBER_OF_GOAL_POSTS));
  if(theLinePercept.circle.found)
    selectedObservations.push_back(SensorModel::Observation(SensorModel::Observation::CENTER_CIRCLE, 0));

  // points and corners are optional
  observations.clear();
  lines.clear();
  int j = 0;
  for(std::list<LinePercept::Line>::const_iterator i = theLinePercept.lines.begin();
      i != theLinePercept.lines.end(); ++i)
  {
    lines.push_back(&*i);
    observations.push_back(SensorModel::Observation(SensorModel::Observation::POINT, j++));
    observations.push_back(SensorModel::Observation(SensorModel::Observation::POINT, j++));
  }
  for(int i = 0; i < (int) theLinePercept.intersections.size(); ++i)
    observations.push_back(SensorModel::Observation(SensorModel::Observation::CORNER, i));

  if(selectedObservations.empty() && observations.empty())
    return false;

  // select observations
  while((int) selectedObservations.size() < parameter->numberOfObservations)
    if(observations.empty())
      selectedObservations.push_back(selectedObservations[rand() % selectedObservations.size()]);
    else
      selectedObservations.push_back(observations[rand() % observations.size()]);

  // apply sensor models
  bool sensorModelApplied(false);
  std::vector<SensorModel*>::iterator sensorModel = sensorModels.begin();
  for(; sensorModel != sensorModels.end(); ++sensorModel)
  {
    selectedIndices.clear();
    for(std::vector<SensorModel::Observation>::const_iterator i = selectedObservations.begin(); i != selectedObservations.end(); ++i)
      if(i->type == (*sensorModel)->type)
        selectedIndices.push_back(i->index);

    SensorModel::SensorModelResult result;
    if(selectedIndices.empty())
      result = SensorModel::NO_SENSOR_UPDATE;
    else
      result = (*sensorModel)->computeWeightings(*samples, selectedIndices, sensorModelWeightings);
    if(result == SensorModel::FULL_SENSOR_UPDATE)
    {
      for(int i = 0; i < samples->size(); ++i)
      {
        samples->at(i).weighting *= sensorModelWeightings[static_cast<unsigned int>(i)];
        sensorModelApplied = true;
      }
    }
    else if(result == SensorModel::PARTIAL_SENSOR_UPDATE)
    {
      // Compute average of all valid weightings, use this average for all invalid ones
      double sum(0.0);
      int numOfValidSamples(0);
      for(unsigned int i=0; i<sensorModelWeightings.size(); ++i)
      {
        if(sensorModelWeightings[i] != -1)
        {
          sum += sensorModelWeightings[i];
          ++numOfValidSamples;
        }
      }
      if(numOfValidSamples == 0) //Should not happen, but you never know...
      {
        OUTPUT(idText, text, "Meeeeek! SensorModel claimed to have made a partial update but did not provide any weighting!");
        continue;
      }
      const double averageWeighting(sum / numOfValidSamples);
      for(unsigned int i=0; i<sensorModelWeightings.size(); ++i)
      {
        if(sensorModelWeightings[i] != -1)
          samples->at(i).weighting *= sensorModelWeightings[i];
        else
          samples->at(i).weighting *= averageWeighting;
      }
      sensorModelApplied = true;
    }
  }
  return sensorModelApplied;
}

void SelfLocator::resampling()
{
  // swap sample arrays
  Sample* oldSet = samples->swap();
  const int numberOfSamples(samples->size());
  const double weightingsSum(totalWeighting);
  const double resamplingPercentage(sampleTemplateGenerator.templatesAvailable() ? std::max(0.0, 1.0 - fastWeighting / slowWeighting) : 0.0);
  const double numberOfResampledSamples = parameter->disableSensorResetting ? numberOfSamples : numberOfSamples * (1.0 - resamplingPercentage);
  const double threshold = parameter->resamplingThreshold * weightingsSum / numberOfSamples;
  const double weightingBetweenTwoDrawnSamples((weightingsSum + threshold * numberOfSamples) / numberOfResampledSamples);
  double nextPos(randomDouble() * weightingBetweenTwoDrawnSamples); 
  double currentSum(0);

  // resample:
  int j(0);
  for(int i = 0; i < numberOfSamples; ++i)
  {
    currentSum += oldSet[i].weighting + threshold;
    while(currentSum > nextPos && j < numberOfSamples)
    {
      samples->at(j++) = oldSet[i];
      nextPos += weightingBetweenTwoDrawnSamples;
    }
  }

  // fill up remaining samples with new poses:
  if(sampleTemplateGenerator.templatesAvailable())
    for(; j < numberOfSamples; ++j)
      generateTemplate(samples->at(j));
  else if(j) // in rare cases, a sample is missing, so add one (or more...)
    for(; j < numberOfSamples; ++j)
      samples->at(j) = samples->at(rand() % j);
  else // resampling was not possible (for unknown reasons), so create a new sample set (fail safe)
#ifdef NDEBUG
  {
    for(int i = 0; i < numberOfSamples; ++i)
    {
      Sample& sample = samples->at(i);
      Pose2D pose(theFieldDimensions.randomPoseOnField());
      sample.translation = Vector2<int>(int(pose.translation.x), int(pose.translation.y));
      sample.rotation = Vector2<int>(int(cos(pose.rotation) * 1024), int(sin(pose.rotation) * 1024));
    }
    poseCalculator->init();
  }
#else
    ASSERT(false);
#endif
}

void SelfLocator::adaptWeightings()
{
  totalWeighting = 0;
  int numberOfSamples(samples->size());
  for(int i=0; i<numberOfSamples; ++i)
     totalWeighting += samples->at(i).weighting;
  if(totalWeighting == 0)
  {
    ASSERT(totalWeighting); // For debugging
    return;                 // For "real" operation
  }
  const double averageWeighting = totalWeighting / numberOfSamples;
  if(slowWeighting)
  {
    slowWeighting = slowWeighting + parameter->alphaSlow * (averageWeighting - slowWeighting);
    fastWeighting = fastWeighting + parameter->alphaFast * (averageWeighting - fastWeighting);
  }
  else
  {
    slowWeighting = averageWeighting;
    if(parameter->knownStartPose)
      fastWeighting = averageWeighting;   // Must be done to avoid complete re-init in first cycle
  }
  PLOT("module:SelfLocator:averageWeighting", averageWeighting * 1e3);
  PLOT("module:SelfLocator:slowWeighting", slowWeighting * 1e3);
  PLOT("module:SelfLocator:fastWeighting", fastWeighting * 1e3);
}

void SelfLocator::generateTemplate(Sample& sample)
{
  if(sampleTemplateGenerator.templatesAvailable())
  {
    const SampleTemplate st = sampleTemplateGenerator.getNewTemplate();
    sample.translation = Vector2<int>(int(st.translation.x), int(st.translation.y));
    sample.rotation = Vector2<int>(int(cos(st.rotation) * (1 << 10)),
                                   int(sin(st.rotation) * (1 << 10)));
    sample.angle = st.rotation;
    sample.weighting = 0;
    sample.cluster = poseCalculator->getNewClusterIndex();
  }
  return;
}

void SelfLocator::drawSamples()
{
  const int numberOfSamples(samples->size());
  const double maxWeighting = 2*totalWeighting / numberOfSamples;
  for(int i = 0; i < numberOfSamples; ++i)
  {
    const Sample& s(samples->at(i));
    const Pose2D pose(s.angle, s.translation.x, s.translation.y);
    unsigned char weighting = (unsigned char) (s.weighting / maxWeighting * 255);
    Vector2<double> bodyPoints[4] = {Vector2<double>(55,90),
                                     Vector2<double>(-55,90),
                                     Vector2<double>(-55,-90),
                                     Vector2<double>(55,-90)};
    for(int j = 0; j < 4; ++j)
    {
      bodyPoints[j] = Geometry::rotate(bodyPoints[j], pose.rotation);
      bodyPoints[j] += pose.translation;
    }
    Vector2<double> headPos(30,0);
    headPos = Geometry::rotate(headPos, pose.rotation);
    headPos += pose.translation;
    ColorRGBA color = s.weighting ? ColorRGBA(weighting,weighting,weighting) : ColorRGBA(255,0,0);
    if(s.cluster == poseCalculator->getIndexOfBestCluster())
      color = ColorRGBA(200,0,200);
    POLYGON("module:SelfLocator:samples", 4, bodyPoints, 
        1, // pen width 
        Drawings::ps_solid, 
        ColorRGBA(180,180,180),
        Drawings::bs_solid, 
        color);
    CIRCLE("module:SelfLocator:samples", 
        headPos.x, 
        headPos.y, 
        30, 
        1, // pen width 
        Drawings::ps_solid, 
        ColorRGBA(180,180,180),
        Drawings::bs_solid, 
        ColorRGBA(180,180,180));
  }
}


MAKE_MODULE(SelfLocator, Modeling)
