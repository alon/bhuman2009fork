/**
* @file PoseCalculatorParticleHistory.h
*
* A class for computing a pose from a given sample set.
*
* @author <a href="mailto:timlaue@informatik.uni-bremen.de">Tim Laue</a>
*/

#ifndef _PoseCalculatorParticleHistory_H_
#define _PoseCalculatorParticleHistory_H_

#include "PoseCalculator.h"


/**
* @class PoseCalculatorParticleHistory
* A class for computing a pose from a given sample set.
*/
template <typename Sample, typename SampleContainer>
class PoseCalculatorParticleHistory: public PoseCalculator<Sample, SampleContainer>
{
public:
  enum MergingStrategy {
    NONE, 
    MERGE_RANDOM_TO_LARGEST, 
    MERGE_TWO_LARGEST,
    PARTICLE_BLEEDING
  };

private:
  std::vector<int> clusterCount;              /**< Count number of particels per cluster. */
  std::vector<int> freeClusterIndizes;        /**< Keep track of free cluster indizes. */
  int maxClusterIdx;                          /**< The index of the largest cluster.*/
  int lastClusterIdx;                         /**< The index of the selected cluster in last run.*/
  int clusterSwitchPercentage;                /**< Threshold for required "improvement" when changing the best cluster*/
  std::vector< std::pair<int,int> > clusters; /**< A list of all clusters which really contain samples.*/
  MergingStrategy mergingStrategy;

public:
  /** Default constructor. */
  PoseCalculatorParticleHistory(SampleContainer& samples):PoseCalculator<Sample, SampleContainer>(samples), 
    maxClusterIdx(0), lastClusterIdx(0), clusterSwitchPercentage(50),
    mergingStrategy(PARTICLE_BLEEDING)
  {
    clusterCount.resize(samples.size() * 2);
    clusterCount.assign(clusterCount.size(), 0);
    clusters.reserve(clusterCount.size());
    freeClusterIndizes.reserve(samples.size());
  }

  void calcPose(RobotPose& robotPose)
  {
    // Reset all counters to zero / clear list of clusters:
    for(unsigned i = 0; i < clusterCount.size(); ++i)
      if(clusterCount[i] > 0)
        clusterCount[i] = 0;
    clusters.clear();

    // Count number of samples per cluster:
    for(int i = 0; i < this->samples.size(); ++i)
      clusterCount[this->samples.at(i).cluster]++;

    // Set all cluster counters which are 0 now to -1 and put index on stack
    // All clusters which in fact have some content are added to the clusters list
    for(unsigned i = 0; i < clusterCount.size(); ++i)
    {
      if(clusterCount[i] == 0)
      {
        clusterCount[i] = -1;
        freeClusterIndizes.push_back(i);
      }
      else if(clusterCount[i] != -1)
      {
        clusters.push_back(std::make_pair(i,clusterCount[i]));
      }
    }

    // Find maximum size cluster:
    maxClusterIdx = 0;
    for(unsigned i = 1; i < clusterCount.size(); ++i)
      if(clusterCount[i] > clusterCount[maxClusterIdx])
        maxClusterIdx = i;

    // Apply merging strategy:
    switch(mergingStrategy)
    {
    case MERGE_RANDOM_TO_LARGEST: mergeRandomClusterToLargest(); break;
    case MERGE_TWO_LARGEST: mergeTwoLagestClusters(); break;
    case PARTICLE_BLEEDING: mergeRandomSampleToLargestCluster(); break;
    default: break;
    }

    // Check, if currently largest cluster is clusterSwitchPercentage percent
    // larger than the previously selected cluster:
    int maxClusterSize = clusterCount[maxClusterIdx];
    int lastClusterSize = clusterCount[lastClusterIdx];
    if((lastClusterSize != -1) && (lastClusterIdx != maxClusterIdx)) // Test, if last cluster did not become invalid
    {
       maxClusterSize *= 100;
       lastClusterSize *= (100+clusterSwitchPercentage);
       if(maxClusterSize < lastClusterSize)
         maxClusterIdx = lastClusterIdx;
    }
    lastClusterIdx = maxClusterIdx;
    // Determine the average pose of all samples in the winner cluster
    calcPoseOfCluster(robotPose, maxClusterIdx); 
  }

  void calcPoseOfCluster(RobotPose& robotPose, unsigned int numOfCluster)
  {
    int xSum = 0,
      ySum = 0,
      cosSum = 0,
      sinSum = 0;
    for(int i = 0; i < this->samples.size(); i++)
    {
      Sample& sample(this->samples.at(i));
      if(sample.cluster != static_cast<int>(numOfCluster))
        continue;
      xSum += sample.translation.x;
      ySum += sample.translation.y;
      cosSum += sample.rotation.x;
      sinSum += sample.rotation.y;
    } 
    // Now calculate the pose as the averages
    int clusterSize(clusterCount[numOfCluster]);
    if(clusterSize)
    {
      double averageRotation = atan2(static_cast<double>(sinSum), static_cast<double>(cosSum));
      robotPose = Pose2D(averageRotation, xSum / clusterSize, ySum / clusterSize);
      robotPose.validity = static_cast<double>(clusterSize) / this->samples.size();
    } 
  }

  int getIndexOfBestCluster() const
  { 
    return maxClusterIdx;
  }

  int getNewClusterIndex()
  {
    int newIndex = freeClusterIndizes.back();
    freeClusterIndizes.pop_back();
    clusterCount[newIndex] = 1;
    return newIndex;
  }

  void init()
  {
    freeClusterIndizes.clear();
    for(int i = 0; i < this->samples.size(); ++i)
    {
      Sample& sample = this->samples.at(i);
      sample.cluster = i;
      clusterCount[i] = 1;
      clusterCount[i + this->samples.size()] = -1;
      freeClusterIndizes.push_back(i + this->samples.size());
    }
  }

  std::vector< std::pair<int,int> > getClusters() const
  {
    return clusters;
  }

private:
  void mergeRandomClusterToLargest()
  {
    // Randomly select cluster:
    int startIdx = random(clusterCount.size());
    int clusterIdx = (startIdx + 1) % clusterCount.size();
    while(clusterIdx != startIdx)
    {
      if(clusterCount[clusterIdx] < 1 || clusterIdx == maxClusterIdx)
        clusterIdx = (clusterIdx + 1) % clusterCount.size();
      else
        break;
    }
    if(clusterIdx == startIdx)  // No cluster != the maximum cluster found
      return;
    // Cluster has been found, now try to merge:
    if(clusterBCompatibleToA(maxClusterIdx, clusterIdx))
      mergeClusterBToA(maxClusterIdx, clusterIdx);
  }

  void mergeTwoLagestClusters()
  {
    // Find largest cluster:
    int clusterIdx(-1);
    int largestClusterSize(0);
    for(int i=0; i<this->samples.size(); ++i)
    {
      if((clusterCount[i] > largestClusterSize) && (i != maxClusterIdx))
      {
        largestClusterSize = clusterCount[i];
        clusterIdx = i;
      } 
    }
    if(clusterIdx == -1)  // No cluster found
      return;
    // Cluster has been found, now try to merge:
    if(clusterBCompatibleToA(maxClusterIdx, clusterIdx))
      mergeClusterBToA(maxClusterIdx, clusterIdx);
  }

  void mergeRandomSampleToLargestCluster()
  {
    // Randomly select a sample and check, if it does not already belong to the largest cluster:
    Sample& sMerge = this->samples.at(random(this->samples.size()));
    if(sMerge.cluster == maxClusterIdx)
      return;
    // Check, if sample is inside cluster:
    Vector2<int> aabbMin(std::numeric_limits<int>::max(), std::numeric_limits<int>::max()), 
      aabbMax(std::numeric_limits<int>::min(), std::numeric_limits<int>::min());
    for(int i=0; i<this->samples.size(); ++i)
    {
      Sample& s = this->samples.at(i);
      if(s.cluster == maxClusterIdx)
      {
        if(s.translation.x < aabbMin.x)
          aabbMin.x = s.translation.x;
        if(s.translation.y < aabbMin.y)
          aabbMin.y = s.translation.y;
        if(s.translation.x > aabbMax.x)
          aabbMax.x = s.translation.x;
        if(s.translation.y > aabbMax.y)
          aabbMax.y = s.translation.y;
      }
    }
    if((sMerge.translation.x > aabbMin.x) &&
      (sMerge.translation.y > aabbMin.y) && 
      (sMerge.translation.x < aabbMax.x) &&
      (sMerge.translation.y < aabbMax.y))
    {
      int oldCluster = sMerge.cluster;
      sMerge.cluster = maxClusterIdx;
      clusterCount[maxClusterIdx]++;
      clusterCount[oldCluster]--;
      if(clusterCount[oldCluster] == 0)
      {
        clusterCount[oldCluster] = -1;
        freeClusterIndizes.push_back(oldCluster);
      }
    }
  }

  bool clusterBCompatibleToA(int a, int b)
  {
    // First try: AABB of cluster a:
    Vector2<int> aabbMin(std::numeric_limits<int>::max(), std::numeric_limits<int>::max()), 
      aabbMax(std::numeric_limits<int>::min(), std::numeric_limits<int>::min());
    for(int i=0; i<this->samples.size(); ++i)
    {
      Sample& s = this->samples.at(i);
      if(s.cluster == a)
      {
        if(s.translation.x < aabbMin.x)
          aabbMin.x = s.translation.x;
        if(s.translation.y < aabbMin.y)
          aabbMin.y = s.translation.y;
        if(s.translation.x > aabbMax.x)
          aabbMax.x = s.translation.x;
        if(s.translation.y > aabbMax.y)
          aabbMax.y = s.translation.y;
      }
    }
    // Cluster b must be inside a completely:
    for(int i=0; i<this->samples.size(); ++i)
    {
      Sample& s = this->samples.at(i);
      if(s.cluster == b)
      {
        if((s.translation.x < aabbMin.x) ||
           (s.translation.y < aabbMin.y) || 
           (s.translation.x > aabbMax.x) ||
           (s.translation.y > aabbMax.y))
          return false;
      }
    }
    return true;
  }

  void mergeClusterBToA(int a, int b)
  {
    for(int i = 0; i < this->samples.size(); ++i)
    {
      Sample& sample = this->samples.at(i);
      if(sample.cluster == b)
        sample.cluster = a;
    }
    clusterCount[a] += clusterCount[b];
    clusterCount[b] = -1;
    freeClusterIndizes.push_back(b);
  }
};


#endif
