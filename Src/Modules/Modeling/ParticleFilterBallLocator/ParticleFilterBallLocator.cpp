/**
* @file ParticleFilterBallLocator.cpp
* Contains a BallLocator implementation using a particle filter
*
* @author <a href="mailto:timlaue@informatik.uni-bremen.de">Tim Laue</a>
*/

#include "ParticleFilterBallLocator.h"
#include "Tools/Math/AngleTables.h"
#include "Tools/Math/Matrix2x2.h"
#include "Tools/Team.h"


ParticleFilterBallLocator::ParticleFilterBallLocator()
 : slowWeighting(0.0), fastWeighting(0.0), lastExecution(0), lastSeenBallTime(0), lastCheatingTime(0), 
   calibrateUncertainty(false), currentCalibrationMeasurement(0), gaussian(5000, 100, 50, 400, 10),
   gaussianAngles(3200, 320, static_cast<int> (parameters.angleStandardDeviation),
   static_cast<int> (parameters.angleStandardDeviation), 1),newSamplesNumberForDrawing(0)
{
  ballSamples = new BallSampleSet(40); //40 samples
  for(int i = 0; i < ballSamples->size(); ++i)
  {
    Pose2D pose(theFieldDimensions.randomPoseOnField());
    ballSamples->at(i).pos.x = static_cast<int>(pose.translation.x);
    ballSamples->at(i).pos.y = static_cast<int>(pose.translation.y);
    if(randomDouble() <= parameters.ballMotionProbability)
    {
      ballSamples->at(i).setRandomVelocity(parameters.maxVelocity);
      ballSamples->at(i).state = BallSample::ROLLING;
    }
    else
    {
      ballSamples->at(1).state = BallSample::LYING;
    }
  }
}


void ParticleFilterBallLocator::init()
{
  lastOdometry = theOdometryData;
  numberOfGeneratedSamples = 0;
  int dist = static_cast<int>(sqrt(static_cast<double>(parameters.robotBoxSizeX*parameters.robotBoxSizeX 
    + parameters.robotBoxSizeY*parameters.robotBoxSizeY)) + 0.5);
  dist += theFieldDimensions.ballRadius;
  parameters.ballCollisionCheckSquareDistance = dist*dist;
}

void ParticleFilterBallLocator::update(BallLocatorSampleSet& sampleSet)
{
  ballSamples->link(sampleSet.sampleSetProxy);
}

void ParticleFilterBallLocator::update(BallModel& ballModel)
{
  preExecution();
  motionUpdate();
  if(theBallPercept.ballWasSeen)
  {
    sensorUpdate();
    resampling();
  }
  modelGeneration(ballModel);
  postExecution(ballModel);

  //Send BallModel to TeamMate
  TEAM_OUTPUT(idTeamMateBallModel, bin, ballModel);	

  DECLARE_DEBUG_DRAWING("module:ParticleFilterBallLocator:ball samples", "drawingOnField"); // Draws the internal representations of the ball locator
  COMPLEX_DRAWING("module:ParticleFilterBallLocator:ball samples", drawSamples());
  DECLARE_DEBUG_DRAWING("module:ParticleFilterBallLocator:ball samples in image", "drawingOnImage"); // Draws the internal representations of the ball locator to the image view
  COMPLEX_DRAWING("module:ParticleFilterBallLocator:ball samples in image", drawSamplesToImage());
}

void ParticleFilterBallLocator::preExecution()
{
  // Remove old elements from percept buffer and try to add a new one (if it exists):
  while(ballPerceptBuffer.size() && theFrameInfo.getTimeSince(ballPerceptBuffer.front().timeStamp) > 1000)
  {
    ballPerceptBuffer.pop_front();
  }
  if(theBallPercept.ballWasSeen)
  {
    ballPerceptBuffer.push_back(BufferedPercept(theBallPercept, theFrameInfo.time, theOdometryData));
  }

  // Compute odometry offset applied in this frame:
  accumulatedOdometry += (theOdometryData - lastOdometry);
  appliedOdometryOffset = Pose2D();
  if(fabs(accumulatedOdometry.rotation) > parameters.minOdoRotation)
  {
    appliedOdometryOffset = accumulatedOdometry.rotation;
    accumulatedOdometry.rotation = 0;
  }
  if(fabs(accumulatedOdometry.translation.x) > parameters.minOdoTranslation)
  {
    appliedOdometryOffset.translation.x = static_cast<int>(accumulatedOdometry.translation.x);
    accumulatedOdometry.translation.x -= appliedOdometryOffset.translation.x;
  }
  if(fabs(accumulatedOdometry.translation.y) > parameters.minOdoTranslation)
  {
    appliedOdometryOffset.translation.y = static_cast<int>(accumulatedOdometry.translation.y);
    accumulatedOdometry.translation.y -= appliedOdometryOffset.translation.y;
  }

  // Debug stuff:
  MODIFY("module:ParticleFilterBallLocator:calibrate", calibrateUncertainty);
  DEBUG_RESPONSE("module:ParticleFilterBallLocator:reset calibration measurements", {currentCalibrationMeasurement = 0;});
  if(theBallPercept.ballWasSeen && calibrateUncertainty && 
     currentCalibrationMeasurement < NUM_OF_CALIBRATION_MEASUREMENTS)
  {
    uncertaintyCalibration();
  }
  MODIFY("parameters:ParticleFilterBallLocator", parameters);
}


void ParticleFilterBallLocator::motionUpdate()
{
  //Rotation matrix for odometry:
  const int cosor = static_cast<int>(cos(appliedOdometryOffset.rotation) * 1024);
  const int sinor = static_cast<int>(sin(appliedOdometryOffset.rotation) * 1024);
  const Matrix2x2<int> R(Vector2<int>(cosor,-sinor), Vector2<int>(sinor,cosor));
  //Some values:
  const Vector2<int> odoTrans(static_cast<int>(appliedOdometryOffset.translation.x), 
                              static_cast<int>(appliedOdometryOffset.translation.y));
  const int odometryLength = odoTrans.abs();
  const int t(theFrameInfo.getTimeSince(lastExecution));
  int minusSpeedPercentage(100-parameters.speedPercentageAfterOneSecond);
  minusSpeedPercentage *= 1024;
  minusSpeedPercentage *= t;
  minusSpeedPercentage /= 1000;
  int realSpeedPercentage = (100*1024) - minusSpeedPercentage;
  int numberOfRollingBalls(0);
  //Update samples:
  for(int i = 0; i < ballSamples->size(); ++i)
  {
    BallSample& s(ballSamples->at(i)); //pick each sample
    const int velocity = s.velocity.abs();
    //Ball motion update:
    if(s.state == BallSample::ROLLING)
    {
      s.pos.x += (s.velocity.x + sampleTriangularDistribution(static_cast<int>(velocity*parameters.scaleFactorVelocityVariance)))* t / 1000;
      s.pos.y += (s.velocity.y + sampleTriangularDistribution(static_cast<int>(velocity*parameters.scaleFactorVelocityVariance)))* t / 1000;
      ++numberOfRollingBalls;
    }
    //Robot odometry update:
    s.pos = (R * s.pos) / 1024;
    s.pos -= odoTrans;
    s.velocity = (R * s.velocity) / 1024;
    //Add uncertainty to position (depending on walked distance):
    s.pos.x += sampleTriangularDistribution(static_cast<int>(odometryLength*parameters.scaleFactorOdometryVariance));
    s.pos.y += sampleTriangularDistribution(static_cast<int>(odometryLength*parameters.scaleFactorOdometryVariance));
    if((s.pos * s.pos) < parameters.ballCollisionCheckSquareDistance)
      clipSampleWithFeet(s);
    //Add uncertainty to velocity:
    if(s.state == BallSample::ROLLING)
    {
      s.velocity.x += sampleTriangularDistribution(static_cast<int>(velocity*parameters.scaleFactorVelocityVariance))* t / 1000;
      s.velocity.y += sampleTriangularDistribution(static_cast<int>(velocity*parameters.scaleFactorVelocityVariance))* t / 1000;
      // Apply some friction:
      s.velocity *= realSpeedPercentage;
      s.velocity /= (100*1024);
    }
  }
  int rollingBallsPercentage = (numberOfRollingBalls*100)/ballSamples->size();
  PLOT("module:ParticleFilterBallLocator:rollingBallsPercentage", rollingBallsPercentage);
}


inline void ParticleFilterBallLocator::clipSampleWithFeet(BallSample& s) 
{
  // Check, if sample is really in relevant area (assume quadratic ball :-):
  Vector2<int>& b(s.pos);
  const int& maxX = parameters.robotBoxSizeX;
  const int& maxY = parameters.robotBoxSizeY;
  if((b.x > maxX + theFieldDimensions.ballRadius) ||
     (b.x < -maxX - theFieldDimensions.ballRadius) ||
     (b.y > maxY + theFieldDimensions.ballRadius) ||
     (b.y < -maxY - theFieldDimensions.ballRadius))
    return;

  const int xDiff = b.x >= 0 ? std::abs(maxX - b.x) : std::abs(-maxX - b.x);
  const int yDiff = b.y >= 0 ? std::abs(maxY - b.y) : std::abs(-maxY - b.y);
  // Move ball to valid position:
  if(xDiff <= yDiff) 
  {
    if(b.x >= 0)
      b.x = maxX + theFieldDimensions.ballRadius;
    else
      b.x = -maxX - theFieldDimensions.ballRadius;
  }
  else
  {   
    if(b.y >= 0)
      b.y = maxY + theFieldDimensions.ballRadius;
    else
      b.y = -maxY - theFieldDimensions.ballRadius;
  }
  // ROLLING ball bounces off: angle in == angle out, velocity becomes decreased by 50%
  // Robot motion is not relevant in this case
  if(s.state == BallSample::ROLLING)
  {
    if(xDiff < yDiff) // Bounce at front of box
    {
      s.velocity.x = -s.velocity.x;
      s.velocity /= 2;
    }
    else              // Bounce at side of box
    {
      s.velocity.y = -s.velocity.y;
      s.velocity /= 2;
    }
  }
  // In addition, the ball is moved in direction of robot movement. Ball speed is higher than robot speed:
  s.velocity.x += static_cast<int>(theWalkingEngineOutput.speed.translation.x)*2;
  s.velocity.y += static_cast<int>(theWalkingEngineOutput.speed.translation.y)*2;
  s.state = BallSample::ROLLING;
}


void ParticleFilterBallLocator::sensorUpdate()
{
  averageWeighting = 0;
  const int observationDistance(static_cast<int>(theBallPercept.relativePositionOnField.abs()));
  const double observationAngle(atan2(theBallPercept.relativePositionOnField.y,theBallPercept.relativePositionOnField.x));
  double distStdDev;
  if(observationDistance <= parameters.closeThreshold)
    distStdDev = parameters.closeBallDistanceStandardDeviation;
  else if(observationDistance >= parameters.farThreshold)
    distStdDev = parameters.farBallDistanceStandardDeviation;
  else
    distStdDev = parameters.closeBallDistanceStandardDeviation + 
    ((observationDistance - parameters.closeThreshold)/(parameters.farThreshold - parameters.closeThreshold))*(parameters.farBallDistanceStandardDeviation - parameters.closeBallDistanceStandardDeviation);
  for(int i = 0; i < ballSamples->size(); ++i)
  {
    BallSample& s(ballSamples->at(i));
    const int modelDistance(s.pos.abs());
    const double modelAngle(Atan2Approximation::atan2(s.pos.y, s.pos.x));
    const double weighting = gaussian.value(abs(observationDistance - modelDistance), (int)distStdDev) *
      gaussianAngles.value(int(1000*fabs(observationAngle - modelAngle)), (int)parameters.angleStandardDeviation);
    s.weighting = weighting;
    averageWeighting += weighting;
  }
  averageWeighting /= ballSamples->size();
  slowWeighting = slowWeighting + parameters.alphaSlow * (averageWeighting - slowWeighting);
  fastWeighting = fastWeighting + parameters.alphaFast * (averageWeighting - fastWeighting);
}


void ParticleFilterBallLocator::resampling()
{
  // swap sample arrays
  BallSample* oldSet = ballSamples->swap();

  const double weightingsSum(averageWeighting*ballSamples->size());
  const double resamplingPercentage(std::max(0.0, 1.0 - fastWeighting / slowWeighting));
  const double RESAMPLED_SAMPLES = ballSamples->size() * (1.0 - resamplingPercentage);
  const double weightingBetweenTwoDrawnSamples(weightingsSum  / double(RESAMPLED_SAMPLES));
  double nextPos(randomDouble()*weightingBetweenTwoDrawnSamples); 
  double currentSum(0);

  // resample:
  int j(0);
  for(int i = 0; i < ballSamples->size(); ++i)
  {
    currentSum += oldSet[i].weighting;
    while(currentSum > nextPos)
    {
      ballSamples->at(j++) = oldSet[i];
      nextPos += weightingBetweenTwoDrawnSamples;
      if(j == ballSamples->size())  // This might be necessary because of floating point imprecision
        return;
    }
  }
  // Store number of generated samples
  numberOfGeneratedSamples = ballSamples->size() - j;
  // fill up remaining samples with new poses:
  for(; j < ballSamples->size(); ++j)
  {
    ballSamples->at(j) = generateNewSample();
  }
  newSamplesNumberForDrawing = numberOfGeneratedSamples;
}


BallSample ParticleFilterBallLocator::generateNewSample()
{
  BallSample newSample;
  // If there have been some percepts, use two of them to create a new sample
  if(ballPerceptBuffer.size() > 1)
  {
    int a = random(ballPerceptBuffer.size());
    int b = random(ballPerceptBuffer.size());
    // a and b must be different, of course:
    if(a == b)
    {
      if(a != 0)
        a--;
      else
        a++;
    }
    // a must be smaller than b:
    if(a > b)
    {
      int temp = b;
      b = a;
      a = temp;
    }
    Vector2<double> p1 = ballPerceptBuffer[a].getOdometryCorrectedPosition(theOdometryData);
    unsigned timeP1 = ballPerceptBuffer[a].timeStamp;
    Vector2<double> p2 = ballPerceptBuffer[b].getOdometryCorrectedPosition(theOdometryData);
    unsigned timeP2 = ballPerceptBuffer[b].timeStamp;
    double timeInSeconds((double(timeP2) - double(timeP1))/1000.0);
    Vector2<double> speed(p2-p1);
    speed /= timeInSeconds;
    newSample.pos.x = static_cast<int>(p2.x);
    newSample.pos.y = static_cast<int>(p2.y);
    newSample.velocity.x = static_cast<int>(speed.x);
    newSample.velocity.y = static_cast<int>(speed.y);
    newSample.clipVelocity(parameters.maxVelocity);
    // Compute current position of sample
    double timeToNow = theFrameInfo.getTimeSince(timeP2);
    timeToNow /= 1000;
    Vector2<double> offset(newSample.velocity.x, newSample.velocity.y);
    offset *= timeToNow;
    newSample.pos.x += static_cast<int>(offset.x);
    newSample.pos.y += static_cast<int>(offset.y);
    //If sample is close, decrease velocity.
    if(newSample.pos.abs() < parameters.templateSpeedDecreaseDistance)
    {
      newSample.velocity /= static_cast<int>(parameters.templateSpeedDecreaseFactor);
    }
  }
  // Use the current percept and a random speed
  else
  {
    newSample.pos.x = static_cast<int>(theBallPercept.relativePositionOnField.x);
    newSample.pos.y = static_cast<int>(theBallPercept.relativePositionOnField.y);
    newSample.setRandomVelocity(parameters.maxVelocity);
    //If sample is close, decrease velocity.
    if(newSample.pos.abs() < parameters.templateSpeedDecreaseDistance)
    {
      newSample.velocity /= static_cast<int>(parameters.templateSpeedDecreaseFactor);
    }
  }
  //This must be integrated a bit better in the previous lines, but anyway:
  if(randomDouble() > parameters.ballMotionProbability)
    newSample.state = BallSample::LYING;
  else
    newSample.state = BallSample::ROLLING;
  return newSample;
}


void ParticleFilterBallLocator::modelGeneration(BallModel& ballModel)
{
  //The estimated ball position:
  Vector2<double> averagePosition;
  Vector2<double> averageVelocity;
  double sumOfAllWeights(0.0);
  // This is a bad case that might happen:
  if(numberOfGeneratedSamples == ballSamples->size())
  {
    for(int i = 0; i < ballSamples->size(); ++i)
    {
      BallSample& s(ballSamples->at(i));
      averagePosition.x += s.pos.x;
      averagePosition.y += s.pos.y;
      if(s.state == BallSample::ROLLING)
      {
        averageVelocity.x += s.velocity.x;
        averageVelocity.y += s.velocity.y;
      }
    }
    averagePosition /= ballSamples->size();
    averageVelocity /= ballSamples->size();
  }
  // This is the standard case:
  else
  {
    for(int i = 0; i < ballSamples->size(); ++i)
    {
      BallSample& s(ballSamples->at(i));
      if(s.weighting != -1.0)
      {
        averagePosition.x += (s.pos.x * s.weighting);
        averagePosition.y += (s.pos.y * s.weighting);
        if(s.state == BallSample::ROLLING)
        {
          averageVelocity.x += (s.velocity.x * s.weighting);
          averageVelocity.y += (s.velocity.y * s.weighting);
        }
        sumOfAllWeights += s.weighting;
      }
    }
    averagePosition /= sumOfAllWeights;
    averageVelocity /= sumOfAllWeights;
  }
  ballModel.estimate.position = averagePosition;
  ballModel.estimate.velocity = averageVelocity;

  //Information about what has actually been seen:
  if(theBallPercept.ballWasSeen)
  {
    ballModel.timeWhenLastSeen = theFrameInfo.time;
    ballModel.lastPerception.position = theBallPercept.relativePositionOnField;
    ballModel.lastSeenEstimate = ballModel.estimate;
  }
  else
  {
    ballModel.lastPerception.position.rotate(-appliedOdometryOffset.rotation);
    ballModel.lastPerception.position -= appliedOdometryOffset.translation;
    ballModel.lastSeenEstimate.position.rotate(-appliedOdometryOffset.rotation);
    ballModel.lastSeenEstimate.position -= appliedOdometryOffset.translation;
  }
}


void ParticleFilterBallLocator::postExecution(BallModel& ballModel)
{
  PLOT("module:ParticleFilterBallLocator:numberOfGeneratedSamples", newSamplesNumberForDrawing);
  lastExecution = theFrameInfo.time;
  if(theBallPercept.ballWasSeen)
  {
    // There is currently one thing that cannot be handled properly:
    // Seeing a ball for the first time after a longer period. Then the model
    // does not adjust fast enough. Therefore we will deliver the percept for one
    // second ;-)

    //Currently cheating:
    if(theFrameInfo.getTimeSince(lastCheatingTime) < 1000)
    {
      ballModel.estimate.position = theBallPercept.relativePositionOnField;
      ballModel.lastSeenEstimate.position = theBallPercept.relativePositionOnField;
    }
    //Detect cheating situation:
    else if(theFrameInfo.getTimeSince(lastSeenBallTime) > 2000)
    {
      lastCheatingTime = theFrameInfo.time;
      ballModel.estimate.position = theBallPercept.relativePositionOnField;
      ballModel.lastSeenEstimate.position = theBallPercept.relativePositionOnField;
    }
    //Standard stuff:
    lastSeenBallTime = lastExecution;
  }
  lastOdometry = theOdometryData;
}


void ParticleFilterBallLocator::drawSamples()
{
  //Default drawing color
  ColorRGBA sampleColor(255,255,255);
  //Determine alpha offset for nicer drawing ;-)
  double alphaOffset(1.0);
  for(int i = 0; i < ballSamples->size(); ++i)
  {
    const double offset(1.0 - ballSamples->at(i).weighting);
    if(offset < alphaOffset)
      alphaOffset = offset;
  }
  for(int i = 0; i < ballSamples->size(); ++i)
  {
    const BallSample& s(ballSamples->at(i));
    Vector2<int> p(s.pos);
    Vector2<int> v(s.velocity);
    if(s.weighting != -1.0)
      sampleColor.a = (unsigned char)((s.weighting + alphaOffset)*255.0);
    else //Draw new samples in a different color:
      sampleColor = ColorRGBA(255,255,0);
    LARGE_DOT("module:ParticleFilterBallLocator:ball samples", p.x, p.y, sampleColor, sampleColor);
    if(s.state == BallSample::ROLLING)
    {
      ARROW("module:ParticleFilterBallLocator:ball samples", p.x, p.y, p.x+v.x, p.y+v.y,5,Drawings::ps_solid,sampleColor);
    }
  }
  // Draw collision box:
  RECTANGLE("module:ParticleFilterBallLocator:ball samples", 
    parameters.robotBoxSizeX, parameters.robotBoxSizeY, -parameters.robotBoxSizeX, -parameters.robotBoxSizeY,
    10, Drawings::ps_solid, ColorRGBA(255,0,0));
  CIRCLE("module:ParticleFilterBallLocator:ball samples", 0, 0, sqrt(double(parameters.ballCollisionCheckSquareDistance)),
    10, Drawings::ps_solid, ColorRGBA(130,130,255), Drawings::bs_null, ColorClasses::blue);
}


void ParticleFilterBallLocator::drawSamplesToImage()
{
  ColorRGBA fillColor(255,128,64,100);
  ColorRGBA borderColor(255,128,64);
  for(int i = 0; i < ballSamples->size(); ++i)
  {
    const BallSample& s(ballSamples->at(i));
    Geometry::Circle c;
    Geometry::calculateBallInImage(Vector2<double>(s.pos.x, s.pos.y), theCameraMatrix, theCameraInfo, theFieldDimensions.ballRadius, c);
    CIRCLE("module:ParticleFilterBallLocator:ball samples in image", 
      c.center.x, 
      c.center.y, 
      c.radius, 
      1, // pen width 
      Drawings::ps_solid, 
      ColorClasses::orange,
      Drawings::bs_solid, 
      ColorRGBA(255,128,64,100));
  }
}


void ParticleFilterBallLocator::uncertaintyCalibration()
{
  if(currentCalibrationMeasurement < NUM_OF_CALIBRATION_MEASUREMENTS)
  {
    ballXMeasurements[currentCalibrationMeasurement] = theBallPercept.positionInImage.x;
    ballYMeasurements[currentCalibrationMeasurement] = theBallPercept.positionInImage.y;
    currentCalibrationMeasurement++;
  }
  if(currentCalibrationMeasurement == NUM_OF_CALIBRATION_MEASUREMENTS)
  {
    ballPosUncertainty.generateDistributionFromMeasurements(ballXMeasurements, NUM_OF_CALIBRATION_MEASUREMENTS,
      ballYMeasurements, NUM_OF_CALIBRATION_MEASUREMENTS);
    OUTPUT(idText, text, ballPosUncertainty.covariance[0][0] << " " << ballPosUncertainty.covariance[1][0] << " "
                      << ballPosUncertainty.covariance[0][1] << " " << ballPosUncertainty.covariance[1][1]);
  }
}


MAKE_MODULE(ParticleFilterBallLocator, Modeling)
