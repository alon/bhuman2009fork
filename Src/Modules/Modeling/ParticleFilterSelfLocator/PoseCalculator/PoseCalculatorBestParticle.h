/**
* @file PoseCalculatorBestParticle.h
*
* A class for computing a pose from a given sample set.
*
* @author <a href="mailto:timlaue@informatik.uni-bremen.de">Tim Laue</a>
*/

#ifndef _PoseCalculatorBestParticle_H_
#define _PoseCalculatorBestParticle_H_

#include "PoseCalculator.h"


/**
* @class PoseCalculatorBestParticle
* A class for computing a pose from a given sample set.
*/
template <typename Sample, typename SampleContainer>
class PoseCalculatorBestParticle: public PoseCalculator<Sample, SampleContainer>
{
public:
  /** Default constructor. */
  PoseCalculatorBestParticle(SampleContainer& samples):PoseCalculator<Sample, SampleContainer>(samples)
  {}

  /** Set the robot's pose to the best particle of the set*/
  void calcPose(RobotPose& robotPose)
  {
    // Find the best rated sample:
    double bestWeighting(0.0);
    Sample* bestSample(0);
    for(int i = 0; i < this->samples.size(); i++)
    {
      Sample& sample = this->samples.at(i);
      if(sample.weighting > bestWeighting)
      {
        bestWeighting = sample.weighting;
        bestSample = &sample;
      }
    } 
    // Set the pose:
    if(bestSample)
    {
      robotPose.translation.x = static_cast<double>(bestSample->translation.x);
      robotPose.translation.y = static_cast<double>(bestSample->translation.y);
      robotPose.rotation = atan2(static_cast<double>(bestSample->rotation.y), 
                                 static_cast<double>(bestSample->rotation.x));
      robotPose.validity = bestSample->weighting;
    } 
    else
      robotPose.validity = 0.0;
  }
};


#endif
