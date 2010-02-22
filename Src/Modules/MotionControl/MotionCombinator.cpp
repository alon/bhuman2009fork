/** 
* @file Modules/MotionControl/MotionCombinator.cpp
* This file implements a module that combines the motions created by the different modules.
* @author <A href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</A>
*/

#include "MotionCombinator.h"

void MotionCombinator::update(UnstableJointRequest& jointRequest)
{
  const JointRequest* jointRequests[MotionRequest::numOfMotions] = 
  {
    &theWalkingEngineOutput,
    &theSpecialActionsOutput,
    &theWalkingEngineStandOutput
  };

  jointRequest.angles[JointData::headPan] = theHeadJointRequest.pan;
  jointRequest.angles[JointData::headTilt] = theHeadJointRequest.tilt;
  copy(*jointRequests[theMotionSelection.targetMotion], jointRequest);

  int i;
  for(i = 0; i < MotionRequest::numOfMotions; ++i)
    if(theMotionSelection.ratios[i] == 1.)
    {
      // default values
      motionInfo.executedMotionRequest.motion = MotionRequest::Motion(i);
      motionInfo.isMotionStable = true;
      motionInfo.positionInWalkCycle = 0;

      lastJointData = theFilteredJointData;
  
      if(theMotionSelection.ratios[MotionRequest::walk] == 1.)
      {
        odometryData += theWalkingEngineOutput.odometryOffset;

        motionInfo.executedMotionRequest.walkRequest.speed = theWalkingEngineOutput.speed;
        motionInfo.executedMotionRequest.walkRequest.target = theWalkingEngineOutput.target;
        motionInfo.positionInWalkCycle = theWalkingEngineOutput.positionInWalkCycle;
      }
      else if(theMotionSelection.ratios[MotionRequest::specialAction] == 1.)
      {
        odometryData += theSpecialActionsOutput.odometryOffset;
        motionInfo.executedMotionRequest.specialActionRequest = theSpecialActionsOutput.executedSpecialAction;
        motionInfo.isMotionStable = theSpecialActionsOutput.isMotionStable;
      }
      else if(theMotionSelection.ratios[MotionRequest::stand] == 1.)
      {
        motionInfo.executedMotionRequest.motion = MotionRequest::stand;
      }
      break;
    }

  if(i == MotionRequest::numOfMotions)
  {
    for(i = 0; i < MotionRequest::numOfMotions; ++i)
      if(i != theMotionSelection.targetMotion && theMotionSelection.ratios[i] > 0.)
      {
        bool interpolateHardness = !(i == MotionRequest::specialAction && theMotionSelection.specialActionRequest.specialAction == SpecialActionRequest::playDead); // do not interpolate from play_dead
        interpolate(*jointRequests[i], *jointRequests[theMotionSelection.targetMotion], theMotionSelection.ratios[i], jointRequest, interpolateHardness);
      }      
    
    if(theMotionSelection.targetMotion == MotionRequest::walk)
      motionInfo.positionInWalkCycle = theWalkingEngineOutput.positionInWalkCycle;
    else
      motionInfo.positionInWalkCycle = 0;
  }

#ifndef RELEASE
  JointDataDeg jointRequestDeg(jointRequest);
#endif
  MODIFY("representation:JointRequestDeg", jointRequestDeg);
}

void MotionCombinator::update(JointRequest& jointRequest)
{
  update((UnstableJointRequest&)jointRequest);

#ifndef RELEASE
  double sum(0);
  int count(0);
  for(int i = JointData::legLeft0; i < JointData::numOfJoints; i++)
  {
    if(jointRequest.angles[i] != JointData::off && jointRequest.angles[i] != JointData::ignore && lastJointRequest.angles[i] != JointData::off && lastJointRequest.angles[i] != JointData::ignore)
    {
      sum += fabs(jointRequest.angles[i] - lastJointRequest.angles[i]);
      count++;
    }
  }
  PLOT("module:MotionCombinator:deviations:JointRequest:legsOnly", sum / count);
  for(int i = 0; i < JointData::legLeft0; i++)
  {
    if(jointRequest.angles[i] != JointData::off && jointRequest.angles[i] != JointData::ignore && lastJointRequest.angles[i] != JointData::off && lastJointRequest.angles[i] != JointData::ignore)
    {
      sum += fabs(jointRequest.angles[i] - lastJointRequest.angles[i]);
      count++;
    }
  }
  PLOT("module:MotionCombinator:deviations:JointRequest:all", sum / count);

  sum = 0;
  count = 0;
  for(int i = JointData::legLeft0; i < JointData::numOfJoints; i++)
  {
    if(lastJointRequest.angles[i] != JointData::off && lastJointRequest.angles[i] != JointData::ignore)
    {
      sum += fabs(lastJointRequest.angles[i] - theFilteredJointData.angles[i]);
      count++;
    }
  }
  PLOT("module:MotionCombinator:differenceToJointData:legsOnly", sum / count);

  for(int i = 0; i < JointData::legLeft0; i++)
  {
    if(lastJointRequest.angles[i] != JointData::off && lastJointRequest.angles[i] != JointData::ignore)
    {
      sum += fabs(lastJointRequest.angles[i] - theFilteredJointData.angles[i]);
      count++;
    }
  }
  lastJointRequest = jointRequest;
  PLOT("module:MotionCombinator:differenceToJointData:all", sum / count);
#endif
}

void MotionCombinator::copy(const JointRequest& source, JointRequest& target) const
{
  for(int i = 0; i < JointData::numOfJoints; ++i)
  {
    if(source.angles[i] != JointData::ignore)
      target.angles[i] = source.angles[i];
    target.jointHardness.hardness[i] = source.angles[i] != JointData::off ? source.jointHardness.getHardness(i) : 0;
  }
}

void MotionCombinator::interpolate(const JointRequest& from, const JointRequest& to,
                                   double fromRatio, JointRequest& target, bool interpolateHardness) const
{
  for(int i = 0; i < JointData::numOfJoints; ++i)
  {
    double f = from.angles[i];
    double t = to.angles[i];

    if(t == JointData::ignore && f == JointData::ignore)
      continue;

    if(t == JointData::ignore)
      t = target.angles[i];
    if(f == JointData::ignore)
      f = target.angles[i];

    const double fHardness = f != JointData::off ? from.jointHardness.getHardness(i) : 0;
    const double tHardness = t != JointData::off ? to.jointHardness.getHardness(i) : 0;

    if(t == JointData::off || t == JointData::ignore)
      t = lastJointData.angles[i];
    if(f == JointData::off || f == JointData::ignore)
      f = lastJointData.angles[i];
    if(target.angles[i] == JointData::off || target.angles[i] == JointData::ignore)
      target.angles[i] = lastJointData.angles[i];
 
    ASSERT(target.angles[i] != JointData::off && target.angles[i] != JointData::ignore);
    ASSERT(t != JointData::off && t != JointData::ignore);
    ASSERT(f != JointData::off && f != JointData::ignore);

    target.angles[i] += -fromRatio * t + fromRatio * f;
    if(interpolateHardness)
      target.jointHardness.hardness[i] += int(-fromRatio * tHardness + fromRatio * fHardness);
    else
      target.jointHardness.hardness[i] = int(tHardness);
  }
}

void MotionCombinator::update(OdometryData& odometryData)
{
  odometryData = this->odometryData;

#ifndef RELEASE
  Pose2D odometryOffset(odometryData);
  odometryOffset -= lastOdometryData;
  PLOT("module:MotionCombinator:odometryOffsetX", odometryOffset.translation.x);
  PLOT("module:MotionCombinator:odometryOffsetY", odometryOffset.translation.y);
  PLOT("module:MotionCombinator:odometryOffsetRotation", toDegrees(odometryOffset.rotation));
  lastOdometryData = odometryData;
#endif
}

MAKE_MODULE(MotionCombinator, Motion Control)
