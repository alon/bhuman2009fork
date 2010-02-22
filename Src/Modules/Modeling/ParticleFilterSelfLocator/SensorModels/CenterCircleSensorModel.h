/**
* @file CenterCircleSensorModel.h
*
* Sensor model for updating samples by perceived center circles
*
* @author <a href="mailto:Tim.Laue@dfki.de">Tim Laue</a>
*/

#ifndef _CenterCircleSensorModel_h_
#define _CenterCircleSensorModel_h_


/**
* @class CenterCircleSensorModel
*/
class CenterCircleSensorModel: public SensorModel
{
private:
  /** Reference to line percept which contains center circle information */
  const LinePercept& theLinePercept;

public:
  /** Constructor. */
  CenterCircleSensorModel(const SelfLocatorParameter& selfLocatorParameter,
    const LinePercept& linePercept, const FrameInfo& frameInfo, 
    const FieldDimensions& fieldDimensions, const CameraMatrix& cameraMatrix, 
    const PerceptValidityChecker& perceptValidityChecker):
  SensorModel(selfLocatorParameter, frameInfo, fieldDimensions, cameraMatrix, 
              perceptValidityChecker, Observation::CENTER_CIRCLE), 
    theLinePercept(linePercept) {}

  /** Function for computing weightings for a sample set.
  * @param samples The samples (not changed by this function
  * @param selectedIndices The indices of the selected observations.
  * @param weightings List of weightings. -1 means: no update
  * @return An overall result of the computation
  */
  SensorModelResult computeWeightings(const SampleSet<SelfLocatorSample>& samples,
    const std::vector<int>& selectedIndices, std::vector<double>& weightings)
  {
    SensorModelResult result = FULL_SENSOR_UPDATE;
    // Precompute some stuff:
    result = FULL_SENSOR_UPDATE;
    const short distanceObserved = static_cast<short>(theLinePercept.circle.pos.abs());
    const double angleObserved = theLinePercept.circle.pos.angle();
    const double& camZ = theCameraMatrix.translation.z;
    const double distanceAsAngleObserved = (pi_2 - atan2(camZ,distanceObserved));
    const double bestPossibleAngleWeighting    = gaussianProbability(0.0, theSelfLocatorParameter.standardDeviationCenterCircleAngle);
    const double bestPossibleDistanceWeighting = gaussianProbability(0.0, theSelfLocatorParameter.standardDeviationCenterCircleDistance);
    const Vector2<double> centerCirclePosition(0,0);
    // Iterate over all samples and compute weightings:
    for(int i = 0; i < samples.size(); ++i)
    {
      const SelfLocatorSample& s(samples.at(i));
      const Pose2D sPose(s.angle, s.translation.x, s.translation.y);
      // Check, if circle might have been confused with goal net:
      if(thePerceptValidityChecker.pointIsProbablyInGoalNet(sPose, distanceObserved, angleObserved))
      {
        weightings[i] = -1;
        result = PARTIAL_SENSOR_UPDATE;
        continue;
      }
      // Compute weighting:
      weightings[i] = computeAngleWeighting(angleObserved, centerCirclePosition, sPose, 
        theSelfLocatorParameter.standardDeviationCenterCircleAngle, bestPossibleAngleWeighting);
      weightings[i] *= computeDistanceWeighting(distanceAsAngleObserved, centerCirclePosition, 
        sPose, camZ, theSelfLocatorParameter.standardDeviationCenterCircleDistance, bestPossibleDistanceWeighting);
    }
    return result;
  }
};


#endif
