/**
* @file PoseCalculator.h
*
* Declares an abstract base class for computing a pose
* from a given sample set.
*
* @author <a href="mailto:timlaue@informatik.uni-bremen.de">Tim Laue</a>
*/

#ifndef _PoseCalculator_H_
#define _PoseCalculator_H_


/**
* @class PoseCalculator
* Abstract base class
*/
template <typename Sample, typename SampleContainer>
class PoseCalculator
{
protected:
  SampleContainer& samples;

public:
  /** Default constructor. */
  PoseCalculator(SampleContainer& samples):samples(samples){}

  virtual ~PoseCalculator() {}

  virtual void calcPose(RobotPose& robotPose) = 0;

  virtual int getIndexOfBestCluster() const
  { return -1;}

  virtual int getNewClusterIndex()
  { return 0; }

  virtual void init() {};

  virtual void draw() {};
};


#endif
