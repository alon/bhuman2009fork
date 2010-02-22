/**
* @file PoseCalculatorKMeansClustering.h
*
* A class for computing a pose from a given sample set.
*
* @author <a href="mailto:timlaue@informatik.uni-bremen.de">Tim Laue</a>
*/

#ifndef _PoseCalculatorKMeansClustering_H_
#define _PoseCalculatorKMeansClustering_H_

#include "PoseCalculator.h"


/**
* @class PoseCalculatorKMeansClustering
* A class for computing a pose from a given sample set.
*/
template <typename Sample, typename SampleContainer, int K, int R>
class PoseCalculatorKMeansClustering: public PoseCalculator<Sample, SampleContainer>
{
private:
  class Cluster
  {
  public:
    Vector2<int> position;
    int numberOfSamples;
  };

  Cluster clusters[K];
  int realK;
  const FieldDimensions& theFieldDimensions;
  bool assignmentHasChanged;

public:
  /** Default constructor. */
  PoseCalculatorKMeansClustering(SampleContainer& samples, const FieldDimensions& theFieldDimensions):
      PoseCalculator<Sample, SampleContainer>(samples), theFieldDimensions(theFieldDimensions)
  {}

  /** Set the robot's pose*/
  void calcPose(RobotPose& robotPose)
  {
    // Do clustering:
    realK = K;
    initializeClustersAndSamples(robotPose);
    while(assignmentHasChanged)
    {
      assignmentHasChanged = false;
      assignSamplesToClusters();
      moveClusterPositions();
    }
    // Find largest cluster:
    int numberOfSamplesInLargestCluster(clusters[0].numberOfSamples);
    int largestCluster(0);
    for(int k=1; k<realK; ++k)
    {
      if(clusters[k].numberOfSamples > numberOfSamplesInLargestCluster)
      {
        numberOfSamplesInLargestCluster = clusters[k].numberOfSamples;
        largestCluster = k;
      }
    }
    // Compute rotation for pose:
    int cosSum(0), sinSum(0);
    for(int i = 0; i < this->samples.size(); i++)
    {
      Sample& sample = this->samples.at(i);
      if(sample.cluster != largestCluster)
        continue;
      cosSum += sample.rotation.x;
      sinSum += sample.rotation.y;
    }
    double averageRotation = atan2(static_cast<double>(sinSum), static_cast<double>(cosSum));
    robotPose = Pose2D(averageRotation, static_cast<double>(clusters[largestCluster].position.x), 
      static_cast<double>(clusters[largestCluster].position.y));
    robotPose.validity = static_cast<double>(numberOfSamplesInLargestCluster) / this->samples.size();
  }

  void getClusterPose(RobotPose& robotPose, int index)
  {
    int cosSum(0), sinSum(0);
    for(int i = 0; i < this->samples.size(); i++)
    {
      Sample& sample = this->samples.at(i);
      if(sample.cluster != index)
        continue;
      cosSum += sample.rotation.x;
      sinSum += sample.rotation.y;
    }
    double averageRotation = atan2(static_cast<double>(sinSum), static_cast<double>(cosSum));
    robotPose = Pose2D(averageRotation, static_cast<double>(clusters[index].position.x), 
                                        static_cast<double>(clusters[index].position.y));
  }
  double getClusterValidity(int index)
  {
    if(index >= realK)
      return 0;
    return static_cast<double>(clusters[index].numberOfSamples) / this->samples.size();
  }

private:
  void initializeClustersAndSamples(RobotPose& robotPose)
  {
    // First cluster is always the previous result:
    int i(0);
    clusters[i].position.x = static_cast<int>(robotPose.translation.x);
    clusters[i].position.y = static_cast<int>(robotPose.translation.y);
    clusters[i].numberOfSamples = 0;
    ++i;
    // Other clusters must be at least R mm away from all previous ones:
    int startIndex = random(this->samples.size());
    int currentIndex = (startIndex + 1) % this->samples.size();
    while(i<realK && currentIndex != startIndex)
    {
      Vector2<int> currentPos = this->samples.at(currentIndex).translation;
      currentIndex = (currentIndex + 1) % this->samples.size();
      // Check distance constraint:
      int j(0);
      while(j<=i)
      {
        double dist = (currentPos - clusters[j].position).abs();
        if(dist < R)
          break;
        ++j;
      }
      if(j<=i)   //Did not pass check:
        continue;
      clusters[i].position = currentPos;
      clusters[i].numberOfSamples = 0;
      ++i;
    }
    if(i != realK)
    {
      realK = i+1;
    }
    // Samples do not belong to any cluster:
    for(int i=0; i < this->samples.size(); ++i)
    {
      this->samples.at(i).cluster = realK+1;
    }
    assignmentHasChanged = true;
  }

  void assignSamplesToClusters()
  {
    for(int i=0; i < this->samples.size(); ++i)
    {
      Sample& s(this->samples.at(i));
      int closestDist = (s.translation - clusters[0].position).abs();
      int closestCluster(0);
      for(int k=1; k<realK; ++k)
      {
        int currentDist = (s.translation - clusters[k].position).abs();
        if(currentDist < closestDist)
        {
          closestCluster = k;
          closestDist = currentDist;
        }
      }
      if(closestCluster != s.cluster)
      {
        assignmentHasChanged = true;
        s.cluster = closestCluster;
      }
    }
  }
  
  void moveClusterPositions()
  {
    for(int k=0; k<realK; ++k)
    {
      int numberOfSamples(0);
      Vector2<int> newPosition;
      for(int i=0; i < this->samples.size(); ++i)
      {
        Sample& s = this->samples.at(i);
        if(s.cluster == k)
        {
          ++numberOfSamples;
          newPosition += s.translation;
        }
      }
      if(numberOfSamples)
      {
        newPosition /= numberOfSamples;
        clusters[k].position = newPosition;
      }
      clusters[k].numberOfSamples = numberOfSamples;
    }
  }

  void draw()
  {
    for(int k=0; k<realK; ++k)
    {
      CIRCLE("module:SelfLocator:poseCalculator", clusters[k].position.x, clusters[k].position.y,
        100, 1, Drawings::ps_solid, ColorRGBA(255,0,0), Drawings::bs_solid, ColorRGBA(255,0,0));
    }
  }

};


#endif
