/** 
* @file MotionSymbols.cpp
*
* Implementation of class MotionSymbols.
*
* @author Max Risler
*/

#include "MotionSymbols.h"
#include "Tools/Xabsl/GT/GTXabslTools.h"
#include "Tools/Math/Vector3.h"
#include "Tools/Math/Matrix.h"
#include "Tools/Math/Geometry.h"

PROCESS_WIDE_STORAGE MotionSymbols* MotionSymbols::theInstance = 0;

void MotionSymbols::registerSymbols(xabsl::Engine& engine)
{
  char s[256];
  char* dest;

  // "motion.type"
  dest = s + sprintf(s, "motion.type.");
  for(int i = 0; i < MotionRequest::numOfMotions; ++i)
  {
    getXabslString(dest, MotionRequest::getMotionName((MotionRequest::Motion)i));
    engine.registerEnumElement("motion.type",s,i);
  }
  engine.registerEnumeratedOutputSymbol("motion.type","motion.type", (int*)&motionRequest.motion);
  
  // "executed_motion.type"
  engine.registerEnumeratedInputSymbol("executed_motion.type","motion.type", (const int*)&motionInfo.executedMotionRequest.motion);

  // "motion.walk_speed.x"
  engine.registerDecimalOutputSymbol("motion.walk_speed.x",
    &motionRequest.walkRequest.speed.translation.x);
  // "motion.percentual.walk_speed.x"
  engine.registerDecimalOutputSymbol("motion.walk_speed.percentual.x",
    &setPercentualSpeedX, &getPercentualSpeedX);
  // "executed_motion.walk_speed.x"
  engine.registerDecimalInputSymbol("executed_motion.walk_speed.x",
    &motionInfo.executedMotionRequest.walkRequest.speed.translation.x);

  // "motion.walk_speed.y"
  engine.registerDecimalOutputSymbol("motion.walk_speed.y",
    &motionRequest.walkRequest.speed.translation.y);
  // "motion.percentual.walk_speed.y"
  engine.registerDecimalOutputSymbol("motion.walk_speed.percentual.y",
    &setPercentualSpeedY, &getPercentualSpeedY);
  // "executed_motion.walk_speed.y"
  engine.registerDecimalInputSymbol("executed_motion.walk_speed.y",
    &motionInfo.executedMotionRequest.walkRequest.speed.translation.y);
  
  // "motion.walk_speed.rot"
  engine.registerDecimalOutputSymbol("motion.walk_speed.rot",
    &setWalkSpeedRot,
    &getWalkSpeedRot);
  // "motion.percentual.walk_speed.rot"
  engine.registerDecimalOutputSymbol("motion.walk_speed.percentual.rot",
    &setPercentualSpeedRot, &getPercentualSpeedRot);
  // "executed_motion.walk_speed.rot"
  engine.registerDecimalInputSymbol("executed_motion.walk_speed.rot",
    &getExecutedWalkSpeedRot);

  // "motion.special_action"
  dest = s + sprintf(s, "motion.special_action.");
  for(int i = 0; i < SpecialActionRequest::numOfSpecialActions; ++i)
  {    
    getXabslString(dest, SpecialActionRequest::getSpecialActionName((SpecialActionRequest::SpecialActionID)i));
    engine.registerEnumElement("motion.special_action",s,i);
  }
  engine.registerEnumeratedOutputSymbol("motion.special_action","motion.special_action", &setSpecialAction, &getSpecialAction);

  engine.registerEnumeratedOutputSymbol("motion.last_special_action","motion.special_action", (int*)&lastSpecialAction);

  // "executed_motion.special_action"
  engine.registerEnumeratedInputSymbol("executed_motion.special_action","motion.special_action", (const int*)&motionInfo.executedMotionRequest.specialActionRequest.specialAction);

  // "motion.special_action.mirror"
  engine.registerBooleanOutputSymbol("motion.special_action.mirror", &motionRequest.specialActionRequest.mirror);
  // "executed_motion.special_action.mirror"
  engine.registerBooleanInputSymbol("executed_motion.special_action.mirror", &motionInfo.executedMotionRequest.specialActionRequest.mirror);

  engine.registerDecimalOutputSymbol("motion.walk_target.x", &setWalkTargetX, &getWalkTargetX);
  engine.registerDecimalOutputSymbol("motion.walk_target.y", &setWalkTargetY, &getWalkTargetY);
  engine.registerDecimalOutputSymbol("motion.walk_target.rot", &setWalkTargetRot, &getWalkTargetRot);
  engine.registerBooleanInputSymbol("motion.walk_target.reached", &getWalkTargetReached);
  engine.registerBooleanOutputSymbol("motion.walk_pedantic", &motionRequest.walkRequest.pedantic);

}

void MotionSymbols::setWalkSpeedX(double x)
{
  theInstance->motionRequest.walkRequest.speed.translation.x = x;
  theInstance->motionRequest.walkRequest.target = Pose2D();
}

double MotionSymbols::getWalkSpeedX()
{
  return theInstance->motionRequest.walkRequest.speed.translation.x;
}

void MotionSymbols::setWalkSpeedY(double y)
{
  theInstance->motionRequest.walkRequest.speed.translation.y = y;
  theInstance->motionRequest.walkRequest.target = Pose2D();
}

double MotionSymbols::getWalkSpeedY()
{
  return theInstance->motionRequest.walkRequest.speed.translation.y;
}

void MotionSymbols::setWalkSpeedRot(double rot)
{
  theInstance->motionRequest.walkRequest.speed.rotation = fromDegrees(rot);
  theInstance->motionRequest.walkRequest.target = Pose2D();
}

double MotionSymbols::getWalkSpeedRot()
{
  return toDegrees(theInstance->motionRequest.walkRequest.speed.rotation);
}

double MotionSymbols::getExecutedWalkSpeedRot()
{
  return toDegrees(theInstance->motionInfo.executedMotionRequest.walkRequest.speed.rotation);
}

void MotionSymbols::setPercentualSpeedX(double percent)
{
  const double& maxSpeed = percent > 0 ? theInstance->walkingEngineOutput.maxSpeed.translation.x : -theInstance->walkingEngineOutput.maxBackwardSpeed;
  theInstance->motionRequest.walkRequest.speed.translation.x = percent / 100.0 * maxSpeed;
  theInstance->motionRequest.walkRequest.target = Pose2D();
}

double MotionSymbols::getPercentualSpeedX()
{
  const double& speed = theInstance->motionRequest.walkRequest.speed.translation.x;
  const double& maxSpeed = speed > 0 ? theInstance->walkingEngineOutput.maxSpeed.translation.x : -theInstance->walkingEngineOutput.maxBackwardSpeed;
  if(maxSpeed == 0)
    return 0;
  return speed / maxSpeed * 100.0;
}

void MotionSymbols::setPercentualSpeedY(double percent)
{
  const double& maxSpeed = theInstance->walkingEngineOutput.maxSpeed.translation.y;
  theInstance->motionRequest.walkRequest.speed.translation.y = percent / 100.0 * maxSpeed;
  theInstance->motionRequest.walkRequest.target = Pose2D();
}

double MotionSymbols::getPercentualSpeedY()
{
  const double& maxSpeed = theInstance->walkingEngineOutput.maxSpeed.translation.y;
  const double& speed = theInstance->motionRequest.walkRequest.speed.translation.y;
  if(maxSpeed == 0)
    return 0;
  return speed / maxSpeed * 100.0;
}

void MotionSymbols::setPercentualSpeedRot(double percent)
{
  const double& maxSpeed = theInstance->walkingEngineOutput.maxSpeed.rotation;
  theInstance->motionRequest.walkRequest.speed.rotation = percent / 100.0 * maxSpeed;
  theInstance->motionRequest.walkRequest.target = Pose2D();
}

double MotionSymbols::getPercentualSpeedRot()
{
  const double& maxSpeed = theInstance->walkingEngineOutput.maxSpeed.rotation;
  const double& speed = theInstance->motionRequest.walkRequest.speed.rotation;
  if(maxSpeed == 0)
    return 0;
  return speed / maxSpeed * 100.0;
}

void MotionSymbols::setSpecialAction(int specialAction)
{
  theInstance->motionRequest.specialActionRequest.specialAction = SpecialActionRequest::SpecialActionID(specialAction);
  theInstance->motionRequest.specialActionRequest.specialize(theInstance->robotInfo.robotModel, theInstance->robotName);
}

int MotionSymbols::getSpecialAction()
{
  return int(theInstance->motionRequest.specialActionRequest.specialAction);
}

void MotionSymbols::setWalkTargetX(double x)
{
  theInstance->motionRequest.walkRequest.target.translation.x = x;
  theInstance->motionRequest.walkRequest.speed = Pose2D();
}

double MotionSymbols::getWalkTargetX()
{
  return theInstance->motionRequest.walkRequest.target.translation.x;
}

void MotionSymbols::setWalkTargetY(double y)
{
  theInstance->motionRequest.walkRequest.target.translation.y = y;
  theInstance->motionRequest.walkRequest.speed = Pose2D();
}

double MotionSymbols::getWalkTargetY()
{
  return theInstance->motionRequest.walkRequest.target.translation.y;
}

void MotionSymbols::setWalkTargetRot(double rot)
{
  theInstance->motionRequest.walkRequest.target.rotation = fromDegrees(rot);
  theInstance->motionRequest.walkRequest.speed = Pose2D();
}

double MotionSymbols::getWalkTargetRot()
{
  return toDegrees(theInstance->motionRequest.walkRequest.target.rotation);
}

bool MotionSymbols::getWalkTargetReached()
{
  return theInstance->motionInfo.executedMotionRequest.walkRequest.target.translation.abs() < 10. &&
    fabs(theInstance->motionInfo.executedMotionRequest.walkRequest.target.rotation) < fromDegrees(1.);
}

void MotionSymbols::update()
{
  motionRequest.walkRequest.pedantic = false;
}

