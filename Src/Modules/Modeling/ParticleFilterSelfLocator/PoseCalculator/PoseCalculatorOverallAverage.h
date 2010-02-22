/**
* @file PoseCalculatorOverallAverage.h
*
* A class for computing a pose from a given sample set.
*
* @author <a href="mailto:timlaue@informatik.uni-bremen.de">Tim Laue</a>
*/

#ifndef _PoseCalculatorOverallAverage_H_
#define _PoseCalculatorOverallAverage_H_

#include "PoseCalculator.h"


/**
* @class PoseCalculatorOverallAverage
* A class for computing a pose from a given sample set.
*/
template <typename Sample, typename SampleContainer>
class PoseCalculatorOverallAverage: public PoseCalculator<Sample, SampleContainer>
{
public:
  /** Default constructor. */
  PoseCalculatorOverallAverage(SampleContainer& samples):PoseCalculator<Sample, SampleContainer>(samples)
  {}

  /** Set the robot's pose to the average of the set*/
  void calcPose(RobotPose& robotPose)
  {
    int xSum(0),
      ySum(0),
      cosSum(0),
      sinSum(0);
    double weightingsSum(0.0);
    int sampleCount(0);
    // Sum data off all samples:
    for(int i = 0; i < this->samples.size(); i++)
    {
      Sample& sample = this->samples.at(i);
      if(sample.weighting)
      {
        xSum += sample.translation.x;
        ySum += sample.translation.y;
        cosSum += sample.rotation.x;
        sinSum += sample.rotation.y;
        weightingsSum += sample.weighting;
        ++sampleCount;
      }
    } 
    // Set the pose:
    if(sampleCount)
    {
      robotPose.translation.x = static_cast<double>(xSum) / sampleCount;
      robotPose.translation.y = static_cast<double>(ySum) / sampleCount;
      robotPose.rotation = atan2(static_cast<double>(sinSum), 
                                 static_cast<double>(cosSum));
      robotPose.validity = weightingsSum / sampleCount;
    } 
    else
      robotPose.validity = 0.0;
  }
};


#endif
