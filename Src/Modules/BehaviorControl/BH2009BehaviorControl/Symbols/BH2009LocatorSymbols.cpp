/** 
* @file BH2009LocatorSymbols.cpp
*
* Implementation of class BH2009LocatorSymbols.
*
* @author Judith Müller
*/

#include "BH2009LocatorSymbols.h"
//#include "Tools/Debugging/Modify.h"

PROCESS_WIDE_STORAGE BH2009LocatorSymbols* BH2009LocatorSymbols::theInstance = 0;

void BH2009LocatorSymbols::registerSymbols(xabsl::Engine& engine)
{
  // position
  engine.registerDecimalInputSymbol("locator.pose.x",&robotPose.translation.x);
  engine.registerDecimalInputSymbol("locator.pose.y",&robotPose.translation.y);
  engine.registerDecimalInputSymbol("locator.pose.angle", &getPoseAngle);


  engine.registerDecimalInputSymbol("locator.opponent_goal.angle", &angleToOpponentGoal);
  engine.registerDecimalInputSymbol("locator.opponent_goal.angle_width", &angleWidthToOpponentGoal);

  engine.registerDecimalInputSymbol("locator.own_goal.angle", &angleToOwnGoal);
  engine.registerDecimalInputSymbol("locator.own_goal.angle_width", &angleWidthToOwnGoal);

  // "distance_to"
  engine.registerDecimalInputSymbol("locator.distance_to", &distanceTo);
  engine.registerDecimalInputSymbolDecimalParameter("locator.distance_to", "locator.distance_to.x", &x);
  engine.registerDecimalInputSymbolDecimalParameter("locator.distance_to", "locator.distance_to.y", &y);

  // "angle_to"
  engine.registerDecimalInputSymbol("locator.angle_to", &angleTo);
  engine.registerDecimalInputSymbolDecimalParameter("locator.angle_to", "locator.angle_to.x", &x);
  engine.registerDecimalInputSymbolDecimalParameter("locator.angle_to", "locator.angle_to.y", &y);

  // "field_to_relative.x"
  engine.registerDecimalInputSymbol("locator.field_to_relative.x", &getFieldToRelativeX);
  engine.registerDecimalInputSymbolDecimalParameter("locator.field_to_relative.x", "locator.field_to_relative.x.x", &x);
  engine.registerDecimalInputSymbolDecimalParameter("locator.field_to_relative.x", "locator.field_to_relative.x.y", &y);

  // "field_to_relative.y"
  engine.registerDecimalInputSymbol("locator.field_to_relative.y", &getFieldToRelativeY);
  engine.registerDecimalInputSymbolDecimalParameter("locator.field_to_relative.y", "locator.field_to_relative.y.x", &x);
  engine.registerDecimalInputSymbolDecimalParameter("locator.field_to_relative.y", "locator.field_to_relative.y.y", &y);

  engine.registerBooleanInputSymbol("locator.ground_contact", &groundContactState.contact);

  engine.registerDecimalInputSymbol("locator.angle_tolerance", &angleToleranceToOpponentGoal);
}

void BH2009LocatorSymbols::update()
{
  // calculate angle to opponent goal

  // if robot pose is behind the groundline it is clipped to a point close to the groundline
  // this is done because bad things happen to the calculated angle if the pose is behind the groundline
  Pose2D poseForOppGoalAngle = theInstance->robotPose;
  if (poseForOppGoalAngle.translation.x > theInstance->fieldDimensions.xPosOpponentGroundline - 50.0)
    poseForOppGoalAngle.translation.x = theInstance->fieldDimensions.xPosOpponentGroundline - 50.0;

  double angleToLeftOpponentGoalPost  = Geometry::angleTo(poseForOppGoalAngle, Vector2<double>(theInstance->fieldDimensions.xPosOpponentGroundline, theInstance->fieldDimensions.yPosLeftGoal));
  double angleToRightOpponentGoalPost = Geometry::angleTo(poseForOppGoalAngle, Vector2<double>(theInstance->fieldDimensions.xPosOpponentGroundline, theInstance->fieldDimensions.yPosRightGoal));

  if(angleToLeftOpponentGoalPost < angleToRightOpponentGoalPost)
    angleToLeftOpponentGoalPost += pi2;


  //Pose2D poseForBallToOppGoalAngle(0.0, theInstance->ballModel.lastPerception.getPositionInFieldCoordinates();
  //double angleFromBallToGoal = Geometry::angleTo(poseForBallToOppGoalAngle, Vector2<double>(theInstance->fieldDimensions.xPosOpponentGroundline, 0));
  
  //OUTPUT(idText, text, angleFromBallToGoal);

  angleToOpponentGoal = toDegrees(normalize((angleToLeftOpponentGoalPost + angleToRightOpponentGoalPost) / 2.0));
  angleWidthToOpponentGoal = toDegrees(fabs(normalize(angleToLeftOpponentGoalPost - angleToRightOpponentGoalPost) / 2.0));
  angleToleranceToOpponentGoal = std::max(10.0, angleWidthToOpponentGoal - 10.0);

  Pose2D poseForOwnGoalAngle = theInstance->robotPose;
  if (poseForOwnGoalAngle.translation.x > theInstance->fieldDimensions.xPosOpponentGroundline - 50.0)
    poseForOwnGoalAngle.translation.x = theInstance->fieldDimensions.xPosOpponentGroundline - 50.0;

  double angleToRightOwnGoalPost  = Geometry::angleTo(poseForOwnGoalAngle, Vector2<double>(theInstance->fieldDimensions.xPosOwnGroundline, theInstance->fieldDimensions.yPosLeftGoal));
  double angleToLeftOwnGoalPost = Geometry::angleTo(poseForOwnGoalAngle, Vector2<double>(theInstance->fieldDimensions.xPosOwnGroundline, theInstance->fieldDimensions.yPosRightGoal));
  if(angleToLeftOwnGoalPost < angleToRightOwnGoalPost)
    angleToLeftOwnGoalPost += pi2;

  angleToOwnGoal = toDegrees(normalize((angleToLeftOwnGoalPost + angleToRightOwnGoalPost) / 2.0));
  angleWidthToOwnGoal = toDegrees(fabs(normalize((angleToRightOwnGoalPost - angleToLeftOwnGoalPost) / 2.0)));

}

double BH2009LocatorSymbols::distanceTo()
{
  return Geometry::distanceTo(theInstance->robotPose,Vector2<double>(theInstance->x, theInstance->y));
}

double BH2009LocatorSymbols::angleTo()
{
  return toDegrees(Geometry::angleTo(theInstance->robotPose,Vector2<double>(theInstance->x, theInstance->y)));
}

double BH2009LocatorSymbols::getFieldToRelativeX()
{
  return Geometry::fieldCoord2Relative(theInstance->robotPose,Vector2<double>(theInstance->x, theInstance->y)).x;
}

double BH2009LocatorSymbols::getFieldToRelativeY()
{
  return Geometry::fieldCoord2Relative(theInstance->robotPose,Vector2<double>(theInstance->x, theInstance->y)).y;
}

double BH2009LocatorSymbols::getPoseX()
{
  return theInstance->robotPose.translation.x;
}

double BH2009LocatorSymbols::getPoseY()
{
  return theInstance->robotPose.translation.y;
}

double BH2009LocatorSymbols::getPoseAngle()
{
  return toDegrees(theInstance->robotPose.getAngle());
}
/*
double BH2009LocatorSymbols::getOpponentGoalAngle()
{
return theInstance->angleToOpponentGoal;
}
*/
