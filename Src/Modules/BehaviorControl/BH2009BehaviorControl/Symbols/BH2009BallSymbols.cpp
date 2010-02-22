/** 
* @file BH2009BallSymbols.cpp
*
* Implementation of class BallSymbols.
*
* @author Max Risler
* \author Colin Graf
*/

#include "BH2009BallSymbols.h"
#include "BH2009TeamSymbols.h"
#include "Tools/Debugging/Modify.h"


PROCESS_WIDE_STORAGE BH2009BallSymbols* BH2009BallSymbols::theInstance = 0;

void BH2009BallSymbols::registerSymbols(xabsl::Engine& engine)
{
  engine.registerDecimalInputSymbol("ball.position.field.x", &ballPositionField.x);
  engine.registerDecimalInputSymbol("ball.position.field.y", &ballPositionField.y);

  engine.registerDecimalInputSymbol("ball.x", &ballPositionRel.x);
  engine.registerDecimalInputSymbol("ball.y", &ballPositionRel.y);
  //This is a little bit faster with the Position because it's just using the last percept
  engine.registerDecimalInputSymbol("ball.seen.x", &seenBallPositionRel.x);
  engine.registerDecimalInputSymbol("ball.seen.y", &seenBallPositionRel.y);
  engine.registerDecimalInputSymbol("ball.seen.angle", &staticGetSeenBallAngle);
  engine.registerDecimalInputSymbol("ball.seen.distance", &staticGetBallSeenDistance);

  engine.registerDecimalInputSymbol("ball.distance", &staticGetBallDistance);
  engine.registerDecimalInputSymbol("ball.angle", &staticGetBallAngle);
  engine.registerBooleanInputSymbol("ball.was_seen", &ballWasSeen);
  engine.registerDecimalInputSymbol("ball.time_since_last_seen", &timeSinceBallWasSeen);

  engine.registerDecimalInputSymbol("ball.speed.field.x", &ballSpeedField.x);
  engine.registerDecimalInputSymbol("ball.speed.field.y", &ballSpeedField.y);
  engine.registerDecimalInputSymbol("ball.speed.robot.x", &ballSpeedRel.x);
  engine.registerDecimalInputSymbol("ball.speed.robot.y", &ballSpeedRel.y);

  engine.registerDecimalInputSymbol("ball.time_when_own_y_axis_reached", &staticGetTimeWhenBallReachesOwnYAxis);
  engine.registerDecimalInputSymbol("ball.position_when_ball_reaches_own_y_axis.y", &staticGetYPosWhenBallReachesOwnYAxis);

  engine.registerBooleanInputSymbol("ball.was_seen.by_team_mate", &staticGetBallWasSeenByTeamMate);
  engine.registerBooleanInputSymbolEnumeratedParameter("ball.was_seen.by_team_mate","ball.was_seen.by_team_mate.player", "team.mate", &player);
  engine.registerDecimalInputSymbol("ball.time_since_last_seen.by_team_mate", &staticGetBallTimeSinceLastSeenByTeamMate);
  engine.registerDecimalInputSymbolEnumeratedParameter("ball.time_since_last_seen.by_team_mate","ball.time_since_last_seen.by_team_mate.player", "team.mate", &player);
  engine.registerDecimalInputSymbol("ball.position.field.by_team_mate.x", &staticGetBallPositionFieldByTeamMateX);
  engine.registerDecimalInputSymbolEnumeratedParameter("ball.position.field.by_team_mate.x","ball.position.field.by_team_mate.x.player", "team.mate", &player);
  engine.registerDecimalInputSymbol("ball.position.field.by_team_mate.y", &staticGetBallPositionFieldByTeamMateY);
  engine.registerDecimalInputSymbolEnumeratedParameter("ball.position.field.by_team_mate.y","ball.position.field.by_team_mate.y.player", "team.mate", &player);
  engine.registerDecimalInputSymbol("ball.position.robot.by_team_mate.x", &staticGetBallPositionRobotByTeamMateX);
  engine.registerDecimalInputSymbolEnumeratedParameter("ball.position.robot.by_team_mate.x","ball.position.robot.by_team_mate.x.player", "team.mate", &player);
  engine.registerDecimalInputSymbol("ball.position.robot.by_team_mate.y", &staticGetBallPositionRobotByTeamMateY);
  engine.registerDecimalInputSymbolEnumeratedParameter("ball.position.robot.by_team_mate.y","ball.position.robot.by_team_mate.y.player", "team.mate", &player);
  engine.registerDecimalInputSymbol("ball.distance.by_team_mate", &staticGetBallDistanceByTeamMate);
  engine.registerDecimalInputSymbolEnumeratedParameter("ball.distance.by_team_mate","ball.distance.by_team_mate.player", "team.mate", &player);
  engine.registerDecimalInputSymbol("ball.angle.by_team_mate", &staticGetBallAngleByTeamMate);
  engine.registerDecimalInputSymbolEnumeratedParameter("ball.angle.by_team_mate","ball.angle.by_team_mate.player", "team.mate", &player);
  engine.registerDecimalInputSymbol("ball.distance.own_goal", &staticGetBallDistanceToOwnGoal);
}

void BH2009BallSymbols::update()
{
  timeSinceBallWasSeen = frameInfo.getTimeSince(theInstance->ballModel.timeWhenLastSeen);
  ballWasSeen = timeSinceBallWasSeen < 500;
  const BallState& estimate = ballModel.estimate;
  ballPositionRel = estimate.position;
  ballPositionField = estimate.getPositionInFieldCoordinates(robotPose);
  ballSpeedRel = estimate.velocity;
  ballSpeedField = estimate.getVelocityInFieldCoordinates(robotPose);
  seenBallPositionRel = ballModel.lastPerception.position;
}


double BH2009BallSymbols::getBallFieldRobotX()
{
  return ballPositionField.x;
}

double BH2009BallSymbols::getBallFieldRobotY()
{
  return ballPositionField.y;
}

double BH2009BallSymbols::getBallPositionRobotX()
{
  return ballPositionRel.x;
}

double BH2009BallSymbols::getBallPositionRobotY()
{
  return ballPositionRel.y;
}

double BH2009BallSymbols::getBallAngle()
{
  return toDegrees(ballModel.estimate.getAngle());
}

double BH2009BallSymbols::getSeenBallAngle()
{
  return toDegrees(ballModel.lastPerception.getAngle());
}

double BH2009BallSymbols::getBallDistance()
{
  return ballModel.estimate.getDistance();
}

double BH2009BallSymbols::getBallSeenDistance()
{
  return ballModel.lastPerception.getDistance();
}

double BH2009BallSymbols::getTimeWhenBallReachesOwnYAxis()
{
  double result = 1000000;

  double decel = (double)fieldDimensions.ballFriction;
  if(ballModel.estimate.velocity.x == 0.0)
    return result;
  double decelX = ballModel.estimate.velocity.x * decel / ballModel.estimate.velocity.abs();
  double decelTime = ballModel.estimate.velocity.x / decelX;
  double xPosWhenStopping = ballModel.estimate.position.x + decelTime * ballModel.estimate.velocity.x - 0.5 * decel * sqr(decelTime);
  if(xPosWhenStopping * ballModel.estimate.position.x < 0 )
  {
    double p(ballModel.estimate.velocity.x * 2 / decelX), q(2.0 / decelX * ballModel.estimate.position.x);
    double temp =  p * -0.5 + sqrt(sqr(p * 0.5) - q);
    return temp;
  }
  return result;
}

double BH2009BallSymbols::getYPosWhenBallReachesOwnYAxis()
{  
  if(ballSpeedRel.x == 0 || ballPositionRel.x * ballSpeedRel.x > 0) // Ball does not move or moves away
  {
    return 0.0;
  }
  double timeWhenAxisIsReached = ballPositionRel.x / ballSpeedRel.x;
  Vector2<double> finalBallPos = ballPositionRel + (ballSpeedRel * timeWhenAxisIsReached);
  
  MODIFY("behavior symbols:ball:finalBallPos", finalBallPos);
  
  return finalBallPos.y;
}

bool BH2009BallSymbols::getBallWasSeenByTeamMate()
{
  if(player == theInstance->robotInfo.number)
    return ballWasSeen;
  if(player >= TeamMateData::firstPlayer && player < TeamMateData::numOfPlayers)
    return frameInfo.getTimeSince(teamMateData.ballModels[player].timeWhenLastSeen) < BH2009TeamSymbols::networkTimeout;
  return false;
}

double BH2009BallSymbols::getBallTimeSinceLastSeenByTeamMate()
{
  if(player == robotInfo.number)
    return timeSinceBallWasSeen;
  if(player >= TeamMateData::firstPlayer && player < TeamMateData::numOfPlayers)
    return frameInfo.getTimeSince(teamMateData.ballModels[player].timeWhenLastSeen);
  return 0;
}

double BH2009BallSymbols::getBallPositionFieldByTeamMateX()
{
  if(player == robotInfo.number)
    return ballPositionField.x;
  if(player >= TeamMateData::firstPlayer && player < TeamMateData::numOfPlayers)
  {
    Vector2<double>& teamBallPositionField = teamBallPositionsField[player];
    if(teamBallPositionsFieldTime[player] < frameInfo.time)
    {
      teamBallPositionsFieldTime[player] = frameInfo.time;
      const RobotPose& matePose = teamMateData.robotPoses[player];
      const BallModel& mateBallModel = teamMateData.ballModels[player];
      teamBallPositionField = mateBallModel.estimate.getPositionInFieldCoordinates(matePose);
    }
    return teamBallPositionField.x;
  }
  return 0;
}

double BH2009BallSymbols::getBallPositionFieldByTeamMateY()
{
  if(player == robotInfo.number)
    return ballPositionField.y;
  if(player >= TeamMateData::firstPlayer && player < TeamMateData::numOfPlayers)
  {
    Vector2<double>& teamBallPositionField = teamBallPositionsField[player];
    if(teamBallPositionsFieldTime[player] < frameInfo.time)
    {
      teamBallPositionsFieldTime[player] = frameInfo.time;
      const RobotPose& matePose = teamMateData.robotPoses[player];
      const BallModel& mateBallModel = teamMateData.ballModels[player];
      teamBallPositionField = mateBallModel.estimate.getPositionInFieldCoordinates(matePose);
    }
    return teamBallPositionField.y;
  }
  return 0;
}

double BH2009BallSymbols::getBallPositionRobotByTeamMateX()
{
  if(player == robotInfo.number)
    return ballPositionRel.x;
  if(player >= TeamMateData::firstPlayer && player < TeamMateData::numOfPlayers)
  {
    Vector2<double>& teamBallPositionRel = teamBallPositionsRel[player];
    if(teamBallPositionsRelTime[player] < frameInfo.time)
    {
      teamBallPositionsRelTime[player] = frameInfo.time;
      const RobotPose& matePose = teamMateData.robotPoses[player];
      const BallModel& mateBallModel = teamMateData.ballModels[player];
      teamBallPositionRel = Geometry::fieldCoord2Relative(robotPose, mateBallModel.estimate.getPositionInFieldCoordinates(matePose));
    }
    return teamBallPositionRel.x;
  }
  return 0;
}

double BH2009BallSymbols::getBallPositionRobotByTeamMateY()
{
  if(player == robotInfo.number)
    return ballPositionRel.y;
  if(player >= TeamMateData::firstPlayer && player < TeamMateData::numOfPlayers)
  {
    Vector2<double>& teamBallPositionRel = teamBallPositionsRel[player];
    if(teamBallPositionsRelTime[player] < frameInfo.time)
    {
      teamBallPositionsRelTime[player] = frameInfo.time;
      const RobotPose& matePose = teamMateData.robotPoses[player];
      const BallModel& mateBallModel = teamMateData.ballModels[player];
      teamBallPositionRel = Geometry::fieldCoord2Relative(robotPose, mateBallModel.estimate.getPositionInFieldCoordinates(matePose));
    }
    return teamBallPositionRel.y;
  }
  return 0;
}

double BH2009BallSymbols::getBallDistanceByTeamMate()
{
  if(player == robotInfo.number)
    return getBallDistance();
  if(player >= TeamMateData::firstPlayer && player < TeamMateData::numOfPlayers)
  {
    Vector2<double>& teamBallPositionRel = teamBallPositionsRel[player];
    if(teamBallPositionsRelTime[player] < frameInfo.time)
    {
      teamBallPositionsRelTime[player] = frameInfo.time;
      const RobotPose& matePose = teamMateData.robotPoses[player];
      const BallModel& mateBallModel = teamMateData.ballModels[player];
      teamBallPositionRel = Geometry::fieldCoord2Relative(robotPose, mateBallModel.estimate.getPositionInFieldCoordinates(matePose));
    }
    return teamBallPositionRel.abs();
  }
  return 0;
}

double BH2009BallSymbols::getBallAngleByTeamMate()
{
  if(player == robotInfo.number)
    return getBallAngle();
  if(player >= TeamMateData::firstPlayer && player < TeamMateData::numOfPlayers)
  {
    Vector2<double>& teamBallPositionRel = teamBallPositionsRel[player];
    if(teamBallPositionsRelTime[player] < frameInfo.time)
    {
      teamBallPositionsRelTime[player] = frameInfo.time;
      const RobotPose& matePose = teamMateData.robotPoses[player];
      const BallModel& mateBallModel = teamMateData.ballModels[player];
      teamBallPositionRel = Geometry::fieldCoord2Relative(robotPose, mateBallModel.estimate.getPositionInFieldCoordinates(matePose));
    }
    return toDegrees(teamBallPositionRel.angle());
  }
  return 0;
}

double BH2009BallSymbols::getBallDistanceToOwnGoal()
{ 
  return (ballPositionField - Vector2<double>(fieldDimensions.xPosOwnGroundline,0)).abs();
}
