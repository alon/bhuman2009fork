/** 
* @file InverseKinematicEngine.cpp
* This file implements a module that creates joint angles from desired foot positions.
* @author <a href="allli@informatik.uni-bremen.de">Alexander Härtl</a> 
*/

#include "InverseKinematicEngine.h"
#include "Tools/InverseKinematic.h"
#include "Tools/Debugging/Debugging.h"
#include "Tools/Range.h"

InverseKinematicEngine::InverseKinematicEngine()
{
  params.relativeFootPositionLeft = Vector3<double>(0, 50, -170);
  params.relativeFootPositionRight = Vector3<double>(0, -50, -170);
  params.ratio = 0.5;
}

void InverseKinematicEngine::update(WalkingEngineStandOutput& standOutput)
{
  MODIFY("inverseKinematic", params);
  if(theMotionSelection.ratios[MotionRequest::stand])
  {
    generateFootPosition();
    InverseKinematic::calcLegJoints(left, right, standOutput, theRobotDimensions, params.ratio);
  }
}

void InverseKinematicEngine::generateFootPosition()
{
  for(int side = 0; side < 2; ++side)
  {
    Pose3D& pose = side ? right : left;
    Vector3<double> rel = side ? params.relativeFootPositionRight : params.relativeFootPositionLeft;
    pose = Pose3D().rotateY(params.bodyTilt)
                   .rotateX(params.bodyRotation)
                   .translate(rel)
                   .rotateZ((side==0 ? 1 : -1) * params.footRotation);
  }
}

MAKE_MODULE(InverseKinematicEngine, Motion Control)
