#ifndef SELFLOCATORPARAMETER_H
#define SELFLOCATORPARAMETER_H

#include <Tools/Math/Pose2D.h>
#include <Tools/Settings.h>

/**
* A collection of all parameters of the module.
*/
class SelfLocatorParameter : public Streamable
{
private:
  /**
  * The method makes the object streamable.
  * @param in The stream from which the object is read 
  * @param out The stream to which the object is written 
  */
  virtual void serialize(In* in, Out* out)
  {  
    STREAM_REGISTER_BEGIN();
    STREAM(numberOfSamples);
    STREAM(standardDeviationFieldLines);
    STREAM(standardDeviationCorners);
    STREAM(standardDeviationGoalpostAngle);
    STREAM(standardDeviationGoalpostBearingDistance);
    STREAM(standardDeviationGoalpostSizeDistance);
    STREAM(standardDeviationGoalpostSampleBearingDistance);
    STREAM(standardDeviationGoalpostSampleSizeDistance);
    STREAM(standardDeviationCenterCircleAngle);
    STREAM(standardDeviationCenterCircleDistance);
    STREAM(alphaSlow);
    STREAM(alphaFast);
    STREAM(resamplingThreshold);
    STREAM(translationNoise);
    STREAM(rotationNoise);
    STREAM(movedDistWeight);
    STREAM(movedAngleWeight);
    STREAM(majorDirTransWeight);
    STREAM(minorDirTransWeight);
    STREAM(maxCrossingLength);
    STREAM(numberOfObservations);
    STREAM(disableSensorResetting);
    STREAM(considerGameState);
    STREAM(knownStartPose);
    STREAM(startPose);
    STREAM(startPoseStandardDeviation);
    STREAM(clipTemplateGeneration);
    STREAM(clipTemplateGenerationRangeX);
    STREAM(clipTemplateGenerationRangeY);
    STREAM_REGISTER_FINISH();
  }

public:
  SelfLocatorParameter();
  void load(const std::string& fileName);

  int numberOfSamples;
  int standardDeviationFieldLines;
  int standardDeviationCorners;
  double standardDeviationGoalpostAngle;
  double standardDeviationGoalpostBearingDistance;
  double standardDeviationGoalpostSizeDistance;
  double standardDeviationGoalpostSampleBearingDistance;
  double standardDeviationGoalpostSampleSizeDistance;
  double standardDeviationCenterCircleAngle;
  double standardDeviationCenterCircleDistance;
  double alphaSlow;
  double alphaFast;
  double resamplingThreshold;
  double translationNoise;
  double rotationNoise;
  double movedDistWeight;
  double movedAngleWeight;
  double majorDirTransWeight;
  double minorDirTransWeight;
  int maxCrossingLength;
  int numberOfObservations;
  bool disableSensorResetting;
  bool considerGameState;
  bool knownStartPose;
  Pose2D startPose;
  Pose2D startPoseStandardDeviation;
  bool clipTemplateGeneration;
  Range<double> clipTemplateGenerationRangeX;
  Range<double> clipTemplateGenerationRangeY;
};


#endif
