/**
* @file GoalPostsSensorModel.h
*
* Sensor model for updating samples by perceived goal posts
*
* @author <a href="mailto:Tim.Laue@dfki.de">Tim Laue</a>
*/

#ifndef _GoalPostsSensorModel_h_
#define _GoalPostsSensorModel_h_


/**
* @class GoalPostsSensorModel
*/
class GoalPostsSensorModel: public SensorModel
{
private:
  /** Reference to goal percept which contains goal post information */
  const GoalPercept& theGoalPercept;

public:
  /** Constructor. */
  GoalPostsSensorModel(const SelfLocatorParameter& selfLocatorParameter, 
    const GoalPercept& goalPercept, const FrameInfo& frameInfo,
    const FieldDimensions& fieldDimensions, const CameraMatrix& cameraMatrix, 
    const PerceptValidityChecker& perceptValidityChecker):
  SensorModel(selfLocatorParameter, frameInfo, fieldDimensions, cameraMatrix, 
              perceptValidityChecker, Observation::GOAL_POST), 
    theGoalPercept(goalPercept)
  {}

  /** Function for computing weightings for a sample set.
  * @param samples The samples (not changed by this function
  * @param selectedIndices The indices of the selected observations.
  * @param weightings List of weightings. -1 means: no update
  * @return An overall result of the computation
  */
  SensorModelResult computeWeightings(const SampleSet<SelfLocatorSample>& samples,
    const std::vector<int>& selectedIndices, std::vector<double>& weightings)
  {
    bool updated(false);
    const double& camZ = theCameraMatrix.translation.z;
    if(selectedIndices[0] < GoalPercept::NUMBER_OF_GOAL_POSTS)
    { // Identified (left or right) goal posts
      for(std::vector<int>::const_iterator i = selectedIndices.begin(); i != selectedIndices.end(); ++i)
      {
        int p = *i;
        const GoalPost& post = theGoalPercept.posts[p];
        // Precompute stuff:
        const double distanceStdDev = (post.distanceType == GoalPost::HEIGHT_BASED) ?
          theSelfLocatorParameter.standardDeviationGoalpostSizeDistance : theSelfLocatorParameter.standardDeviationGoalpostBearingDistance;
        const double bestPossibleAngleWeighting    = gaussianProbability(0.0, theSelfLocatorParameter.standardDeviationGoalpostAngle);
        const double bestPossibleDistanceWeighting = gaussianProbability(0.0, distanceStdDev);
        const Vector2<double> pField(post.positionOnField.x, post.positionOnField.y);
        const double angleObserved = pField.angle();
        const double distanceAsAngleObserved = (pi_2 - atan2(camZ, pField.abs()));
        Vector2<double> uniquePosition;
        if(p == GoalPercept::LEFT_OPPONENT)
          uniquePosition = Vector2<double>(theFieldDimensions.xPosOpponentGoalpost, theFieldDimensions.yPosLeftGoal);
        else if(p == GoalPercept::RIGHT_OPPONENT)
          uniquePosition = Vector2<double>(theFieldDimensions.xPosOpponentGoalpost, theFieldDimensions.yPosRightGoal);
        else if(p == GoalPercept::LEFT_OWN)
          uniquePosition = Vector2<double>(theFieldDimensions.xPosOwnGoalpost, theFieldDimensions.yPosRightGoal);
        else if(p == GoalPercept::RIGHT_OWN)
          uniquePosition = Vector2<double>(theFieldDimensions.xPosOwnGoalpost, theFieldDimensions.yPosLeftGoal);
        // Iterate over all samples and compute weightings:
        for(int i = 0; i < samples.size(); ++i)
        {
          const SelfLocatorSample& s(samples.at(i));
          const Pose2D sPose(s.angle, s.translation.x, s.translation.y);
          if(updated)
            weightings[i] *= computeAngleWeighting(angleObserved, uniquePosition, sPose, 
              theSelfLocatorParameter.standardDeviationGoalpostAngle, bestPossibleAngleWeighting);
          else
            weightings[i] = computeAngleWeighting(angleObserved, uniquePosition, sPose, 
              theSelfLocatorParameter.standardDeviationGoalpostAngle, bestPossibleAngleWeighting);
          if(post.distanceType != GoalPost::IS_CLOSER)
          {
            weightings[i] *= computeDistanceWeighting(distanceAsAngleObserved, uniquePosition, 
              sPose, camZ, distanceStdDev, bestPossibleDistanceWeighting);
          }
        }
        updated = true;
      }
    }
    else
    { // Use unknown posts only, if there has not been an update by a "normal" post
      for(std::vector<int>::const_iterator i = selectedIndices.begin(); i != selectedIndices.end(); ++i)
      {
        int p = *i - GoalPercept::NUMBER_OF_GOAL_POSTS;
        const GoalPost& post = theGoalPercept.unknownPosts[p];
        const double distanceStdDev = (post.distanceType == GoalPost::HEIGHT_BASED) ?
          theSelfLocatorParameter.standardDeviationGoalpostSizeDistance : theSelfLocatorParameter.standardDeviationGoalpostBearingDistance;
        const double bestPossibleAngleWeighting    = gaussianProbability(0.0, theSelfLocatorParameter.standardDeviationGoalpostAngle);
        const double bestPossibleDistanceWeighting = gaussianProbability(0.0, distanceStdDev);
        
        const Vector2<double> pField(post.positionOnField.x, post.positionOnField.y);
        const double distanceAsAngleObserved = (pi_2 - atan2(camZ,pField.abs()));
        const double angleObserved = pField.angle();
        Vector2<double> uniquePositions[2];
        if(p == GoalPercept::UNKNOWN_OPPONENT)
        {
          uniquePositions[0] = Vector2<double>(theFieldDimensions.xPosOpponentGoalpost, theFieldDimensions.yPosLeftGoal);
          uniquePositions[1] = Vector2<double>(theFieldDimensions.xPosOpponentGoalpost, theFieldDimensions.yPosRightGoal);
        }
        else
        {
          uniquePositions[0] = Vector2<double>(theFieldDimensions.xPosOwnGoalpost, theFieldDimensions.yPosRightGoal);
          uniquePositions[1] = Vector2<double>(theFieldDimensions.xPosOwnGoalpost, theFieldDimensions.yPosLeftGoal);
        }
        // Iterate over all samples and compute weightings:
        for(int i = 0; i < samples.size(); ++i)
        {
          const SelfLocatorSample& s(samples.at(i));
          const Pose2D sPose(s.angle, s.translation.x, s.translation.y);
          double weighting0 = computeAngleWeighting(angleObserved, 
            uniquePositions[0], sPose, theSelfLocatorParameter.standardDeviationGoalpostAngle, 
            bestPossibleAngleWeighting);
          double weighting1 = computeAngleWeighting(angleObserved, 
            uniquePositions[1], sPose, theSelfLocatorParameter.standardDeviationGoalpostAngle, 
            bestPossibleAngleWeighting);
          if(post.distanceType != GoalPost::IS_CLOSER)
          {
            weighting0 *= computeDistanceWeighting(distanceAsAngleObserved, uniquePositions[0], 
              sPose, camZ, distanceStdDev, bestPossibleDistanceWeighting);
            weighting1 *= computeDistanceWeighting(distanceAsAngleObserved, uniquePositions[1], 
              sPose, camZ, distanceStdDev, bestPossibleDistanceWeighting);
          }
          if(updated)
            weightings[i] *= std::max(weighting0, weighting1);
          else
            weightings[i] = std::max(weighting0, weighting1);
        }
        updated = true;
      }
    }
    return FULL_SENSOR_UPDATE;
  }
};


#endif
