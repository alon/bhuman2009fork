/** 
* @file HeadMotionEngine.cpp
* This file implements a module that creates head joint angles from desired head motion.
* @author <a href="allli@informatik.uni-bremen.de">Alexander Härtl</a> 
*/

#include "HeadMotionEngine.h"

HeadMotionEngine::HeadMotionEngine()
{
  lastSpeed.x = 0;
  lastSpeed.y = 0;  
}

void HeadMotionEngine::update(HeadJointRequest& headJointRequest)
{
  const double speed = theHeadMotionRequest.speed * theRobotDimensions.motionCycleTime,
    lastPan = headJointRequest.pan == JointData::off ? theFilteredJointData.angles[JointData::headPan] : headJointRequest.pan,
    lastTilt = headJointRequest.tilt == JointData::off ? theFilteredJointData.angles[JointData::headTilt] : headJointRequest.tilt;
  const Vector2<double> lastPosition(lastPan, lastTilt);
  const Vector2<double> target((theHeadMotionRequest.pan == JointData::off ? 0 : theHeadMotionRequest.pan),
                               (theHeadMotionRequest.tilt == JointData::off ? 0 : theHeadMotionRequest.tilt));
  const double distanceToTarget = (lastPosition-target).abs();
      
  float acc = 0.0025f;
  MODIFY("module:HeadMotionEngine:acceleration", acc);

  const double maxSpeedForDistance = sqrt(2.0*acc*distanceToTarget);
  const double maxSpeed = std::min(maxSpeedForDistance, speed);

  //set speed
  Vector2<double> newSpeed = target-lastPosition;
  if(distanceToTarget > maxSpeed)
    newSpeed.normalize(maxSpeed);
  //limit acceleration
  if((newSpeed-lastSpeed).abs() > acc)
    newSpeed = lastSpeed + (newSpeed-lastSpeed).normalize(acc);
    
  //set new position and speed
  headJointRequest.pan = theHeadMotionRequest.pan == JointData::off ? JointData::off : lastPosition.x + newSpeed.x;
  headJointRequest.tilt = theHeadMotionRequest.tilt == JointData::off ? JointData::off : lastPosition.y + newSpeed.y;
  lastSpeed = newSpeed;
}



MAKE_MODULE(HeadMotionEngine, Motion Control)
