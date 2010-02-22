/** 
* \file BH2009SoccerSymbols.cpp
* Implementation of symbols for our 4 roles.
* \author Colin Graf
* \author <a href="mailto:aschreck@informatik.uni-bremen.de">Andr&eacute; Schreck</a>
* \author Katharina Gillmann
*/

#include <limits>
#include "Tools/Debugging/Modify.h"
#include "BH2009SoccerSymbols.h"
#include "BH2009TeamSymbols.h"
#include "BH2009LocatorSymbols.h"
#include "BH2009BallSymbols.h"
#include "Tools/Team.h"


PROCESS_WIDE_STORAGE BH2009SoccerSymbols* BH2009SoccerSymbols::theInstance = 0;

void BH2009SoccerSymbols::registerSymbols(xabsl::Engine& engine)
{
  engine.registerEnumElement("soccer.behavior_action", "soccer.behavior_action.unknown", BehaviorData::unknown);
  engine.registerEnumElement("soccer.behavior_action", "soccer.behavior_action.dribble", BehaviorData::dribble);
  engine.registerEnumElement("soccer.behavior_action", "soccer.behavior_action.go_to_ball", BehaviorData::goToBall);
  engine.registerEnumElement("soccer.behavior_action", "soccer.behavior_action.search_for_ball", BehaviorData::searchForBall);
  engine.registerEnumElement("soccer.behavior_action", "soccer.behavior_action.go_to_target", BehaviorData::goToTarget);
  engine.registerEnumElement("soccer.behavior_action", "soccer.behavior_action.prepare_kick", BehaviorData::prepareKick);
  engine.registerEnumElement("soccer.behavior_action", "soccer.behavior_action.kick", BehaviorData::kick);
  engine.registerEnumElement("soccer.behavior_action", "soccer.behavior_action.kick_sidewards", BehaviorData::kickSidewards);
  engine.registerEnumElement("soccer.behavior_action", "soccer.behavior_action.pass", BehaviorData::pass);
  engine.registerEnumElement("soccer.behavior_action", "soccer.behavior_action.block", BehaviorData::block);
  engine.registerEnumElement("soccer.behavior_action", "soccer.behavior_action.hold", BehaviorData::hold);
  engine.registerEnumElement("soccer.behavior_action", "soccer.behavior_action.stand_up", BehaviorData::standUp);
  engine.registerEnumElement("soccer.behavior_action", "soccer.behavior_action.patrol", BehaviorData::patrol);
  engine.registerEnumElement("soccer.behavior_action", "soccer.behavior_action.pass_before_goal", BehaviorData::passBeforeGoal);
  engine.registerEnumElement("soccer.behavior_action", "soccer.behavior_action.kickoff", BehaviorData::kickoff);
  engine.registerEnumElement("soccer.behavior_action", "soccer.behavior_action.wait_for_pass", BehaviorData::waitForPass);
  engine.registerEnumElement("soccer.behavior_action", "soccer.behavior_action.prepare_pass", BehaviorData::preparePass);
  engine.registerEnumeratedOutputSymbol("soccer.behavior_action", "soccer.behavior_action", (int*)&behaviorData.action);

  engine.registerBooleanInputSymbol("soccer.disable_pre_initial", &disablePreInitialState);

  engine.registerDecimalInputSymbol("soccer.opponent_goal_angle", &staticSoccerOpponentGoalAngle);

  engine.registerDecimalInputSymbol("soccer.position_next_to_ball.x", &staticSoccerPositionNextBallX);
  engine.registerDecimalInputSymbolBooleanParameter("soccer.position_next_to_ball.x", "soccer.position_next_to_ball.x.side", &sideLeft);

  engine.registerDecimalInputSymbol("soccer.position_next_to_ball.y",&staticSoccerPositionNextBallY);
  engine.registerDecimalInputSymbolBooleanParameter("soccer.position_next_to_ball.y", "soccer.position_next_to_ball.y.side", &sideLeft);

  engine.registerDecimalInputSymbol("soccer.position_next_to_ball.angle", &staticSoccerAngleNextToBall);
  engine.registerDecimalInputSymbolBooleanParameter("soccer.position_next_to_ball.angle", "soccer.position_next_to_ball.angle.side", &sideLeft);

  engine.registerDecimalInputSymbol("soccer.position_behind_ball.x", &staticSoccerPositionBehindBallX);
  engine.registerDecimalInputSymbol("soccer.position_behind_ball.y", &staticSoccerPositionBehindBallY);
  engine.registerDecimalInputSymbol("soccer.position_behind_ball.angle", &staticSoccerPositionBehindBallAngle);
}


void BH2009SoccerSymbols::init()
{
#ifdef TARGET_SIM
  disablePreInitialState = true;
#else
  disablePreInitialState = Global::getSettings().recover;
#endif
}


void BH2009SoccerSymbols::update()
{
  
}

double BH2009SoccerSymbols::soccerOpponentGoalAngle()
{
  if(frameInfo.getTimeSince(goalPercept.timeWhenOppGoalLastSeen) < 4000) 
  {
    double angleLeft = toDegrees(goalPercept.posts[GoalPercept::LEFT_OPPONENT].positionOnField.angle());
    double angleRight = toDegrees(goalPercept.posts[GoalPercept::RIGHT_OPPONENT].positionOnField.angle());
    return (angleLeft + angleRight) / 2;
  }
  else
  {
    return toDegrees(Geometry::angleTo(robotPose, Vector2<double>(fieldDimensions.xPosOpponentGroundline, 0)));
  }
}

double BH2009SoccerSymbols::computePositionNextBallX()
{
  return computePosition().x;
}

double BH2009SoccerSymbols::computePositionNextBallY()
{
  return computePosition().y;
}

double BH2009SoccerSymbols::computeAngleNextBall()
{
  return toDegrees(Geometry::angleTo(theInstance->robotPose,computePosition()));
}

Vector2<double> BH2009SoccerSymbols::computePosition()
{
  Vector2<double> ballPosition = ballModel.estimate.getPositionInFieldCoordinates(theInstance->robotPose);
  Vector2<double> temp = ballPosition - Vector2<double>(fieldDimensions.xPosOpponentGroundline, 0);
  if(sideLeft) temp.rotateLeft();
  else temp.rotateRight();
  temp.normalize(300);
  return (temp + ballPosition);
}

double BH2009SoccerSymbols::computePositionBehindBallX()
{
  return computePositionBehindBall().x;
}

double BH2009SoccerSymbols::computePositionBehindBallY()
{
  return computePositionBehindBall().y;
}

double BH2009SoccerSymbols::computePositionBehindBallAngle()
{
  return toDegrees(Geometry::angleTo(theInstance->robotPose,computePositionBehindBall()));
}

Vector2<double> BH2009SoccerSymbols::computePositionBehindBall()
{
  Vector2<double> ballPosition = ballModel.estimate.getPositionInFieldCoordinates(theInstance->robotPose);
  Vector2<double> temp = ballPosition - Vector2<double>(fieldDimensions.xPosOpponentGroundline, 0);
  temp.normalize(250);
  return (temp + ballPosition);
}
