/** 
* @file BH2009GoalSymbols.cpp
*
* Implementation of class BH2009GoalSymbols.
*
* @author Tim Laue
*/

#include "BH2009GoalSymbols.h"

PROCESS_WIDE_STORAGE BH2009GoalSymbols* BH2009GoalSymbols::theInstance = 0;


void BH2009GoalSymbols::registerSymbols(xabsl::Engine& engine)
{
  // "goal.left_angle_to_free_part"
  engine.registerDecimalInputSymbol("goal.left_angle_to_free_part", &getAngleToLeftEnd);
  // "goal.right_angle_to_free_part"
  engine.registerDecimalInputSymbol("goal.right_angle_to_free_part", &getAngleToRightEnd);
  // "goal.angle_to_center_of_free_part"
  engine.registerDecimalInputSymbol("goal.angle_to_center_of_free_part", &getCenterOfFreePart);
  // "goal.time_since_last_seen"
  engine.registerDecimalInputSymbol("goal.time_since_last_seen", &timeSinceOppGoalWasSeen);
  // "goal.time_since_last_seen"
  engine.registerDecimalInputSymbol("goal.time_since_last_seen_any_goal", &timeSinceAnyGoalWasSeen);
  // "goal.was_seen"
  engine.registerBooleanInputSymbol("goal.was_seen", &goalWasSeen);
  // "goal.free_part_in_front_of_me"
  engine.registerBooleanInputSymbol("goal.free_part_in_front_of_me", &goalInFrontOfMe);
  // "goal.free_part_in_front_of_me_safe"
  engine.registerBooleanInputSymbol("goal.free_part_in_front_of_me_safe", &goalInFrontOfMeSafe);
  // "goal.free_part_size"
  engine.registerDecimalInputSymbol("goal.free_part_size", &getFreePartSize);
}

void BH2009GoalSymbols::update()
{
  timeSinceOppGoalWasSeen = theInstance->frameInfo.getTimeSince(goalPercept.timeWhenOppGoalLastSeen);
  timeSinceOwnGoalWasSeen = theInstance->frameInfo.getTimeSince(goalPercept.timeWhenOwnGoalLastSeen);
  timeSinceAnyGoalWasSeen = timeSinceOppGoalWasSeen < timeSinceOwnGoalWasSeen ? timeSinceOppGoalWasSeen : timeSinceOwnGoalWasSeen;
  goalWasSeen             = timeSinceOppGoalWasSeen < 500;
}

bool BH2009GoalSymbols::goalInFrontOfMe()
{
  return getAngleToLeftEnd() >= 0.0 && getAngleToRightEnd() <= 0.0;
}

bool BH2009GoalSymbols::goalInFrontOfMeSafe()
{
  const double safetyDist = 0.34906585039886590 / 2;
  return theInstance->goalPercept.posts[GoalPercept::LEFT_OPPONENT].positionOnField.angle() > safetyDist && 
         theInstance->goalPercept.posts[GoalPercept::RIGHT_OPPONENT].positionOnField.angle() < -safetyDist;
}

double BH2009GoalSymbols::getCenterOfFreePart()
{
  return (getAngleToLeftEnd()+getAngleToRightEnd()) / 2;
}

double BH2009GoalSymbols::getAngleToLeftEnd()
{
  return toDegrees(theInstance->goalPercept.posts[GoalPercept::LEFT_OPPONENT].positionOnField.angle());  
}

double BH2009GoalSymbols::getAngleToRightEnd()
{
  return toDegrees(theInstance->goalPercept.posts[GoalPercept::RIGHT_OPPONENT].positionOnField.angle());
}

double BH2009GoalSymbols::getFreePartSize()
{
  return toDegrees(getAngleToLeftEnd()-getAngleToRightEnd());
}
