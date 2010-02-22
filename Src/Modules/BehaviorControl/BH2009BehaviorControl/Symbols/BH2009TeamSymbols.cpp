/** 
* \file BH2009TeamSymbols.cpp
* Implementation of symbols for TeamMateData.
* \author Colin Graf
* \author Katharina Gillmann
*/

#include "BH2009TeamSymbols.h"
#include "BH2009BallSymbols.h"
#include "BH2009LocatorSymbols.h"
#include <limits>

PROCESS_WIDE_STORAGE BH2009TeamSymbols* BH2009TeamSymbols::theInstance = 0;

void BH2009TeamSymbols::registerSymbols(xabsl::Engine& engine)
{
  // team mate enumeration
  engine.registerEnumElement("team.mate", "team.mate.player1", 1);
  engine.registerEnumElement("team.mate", "team.mate.keeper", 1); 
  engine.registerEnumElement("team.mate", "team.mate.player2", 2);
  engine.registerEnumElement("team.mate", "team.mate.player3", 3);
  engine.registerEnumElement("team.mate", "team.mate.player4", 4);
  
  // team ball symbols
  engine.registerDecimalInputSymbol("team.ball.distance", &staticGetBallDistance);
  engine.registerDecimalInputSymbolEnumeratedParameter("team.ball.distance", "team.ball.distance.player", "team.mate", &player);

  engine.registerDecimalInputSymbol("team.distance.to", &staticDistanceTo);
  engine.registerDecimalInputSymbolEnumeratedParameter("team.distance.to", "team.distance.to.player", "team.mate", &player);
  engine.registerDecimalInputSymbolDecimalParameter("team.distance.to", "team.distance.to.x", &xPosition);
  engine.registerDecimalInputSymbolDecimalParameter("team.distance.to", "team.distance.to.y", &yPosition);
  
  engine.registerDecimalInputSymbol("team.ball.angle", &staticGetBallAngle);
  engine.registerDecimalInputSymbolEnumeratedParameter("team.ball.angle", "team.ball.angle.player", "team.mate", &player);
  
  engine.registerDecimalInputSymbol("team.ball.x", &staticGetBallX);
  engine.registerDecimalInputSymbolEnumeratedParameter("team.ball.x", "team.ball.x.player", "team.mate", &player);
  
  engine.registerDecimalInputSymbol("team.ball.y", &staticGetBallY);
  engine.registerDecimalInputSymbolEnumeratedParameter("team.ball.y", "team.ball.y.player", "team.mate", &player);

  engine.registerDecimalInputSymbol("team.ball.position.field.x", &staticGetBallPositionFieldX);
  engine.registerDecimalInputSymbolEnumeratedParameter("team.ball.position.field.x", "team.ball.position.field.x.player", "team.mate", &player);
  
  engine.registerDecimalInputSymbol("team.ball.position.field.y", &staticGetBallPositionFieldY);
  engine.registerDecimalInputSymbolEnumeratedParameter("team.ball.position.field.y", "team.ball.position.field.y.player", "team.mate", &player);

  // team locator symbols
  engine.registerDecimalInputSymbol("team.locator.pose.x", &staticGetLocatorPoseX);
  engine.registerDecimalInputSymbolEnumeratedParameter("team.locator.pose.x", "team.locator.pose.x.player", "team.mate", &player);
  
  engine.registerDecimalInputSymbol("team.locator.pose.y", &staticGetLocatorPoseY);
  engine.registerDecimalInputSymbolEnumeratedParameter("team.locator.pose.y", "team.locator.pose.y.player", "team.mate", &player);
  
  engine.registerDecimalInputSymbol("team.locator.pose.angle", &staticGetLocatorPoseAngle);
  engine.registerDecimalInputSymbolEnumeratedParameter("team.locator.pose.angle", "team.locator.pose.angle.player", "team.mate", &player);

  // tean role symbols
  engine.registerEnumeratedInputSymbol("team.role", "role.role", &staticGetRole);
  engine.registerEnumeratedInputSymbolEnumeratedParameter("team.role", "team.role.player", "team.mate", &player);

  engine.registerEnumeratedInputSymbol("team.action", "soccer.behavior_action", &staticGetAction);
  engine.registerEnumeratedInputSymbolEnumeratedParameter( "team.action", "team.action.player", "team.mate", &player);

  engine.registerBooleanInputSymbol("team.is_attacking", &staticIsTeamAttacking);


  engine.registerEnumeratedInputSymbol("team.first_team_mate","team.mate", &staticGetFirstTeamMate);
  engine.registerEnumeratedInputSymbol("team.second_team_mate","team.mate", &staticGetSecondTeamMate);

  engine.registerDecimalInputSymbol("team.ball_distance_all_players", &staticGetBallDistanceAllPlayers);
  engine.registerDecimalInputSymbol("team.ball_distance_team_mate_all_players", &staticGetBallDistanceTeamMateAllPlayers);
  engine.registerDecimalInputSymbolEnumeratedParameter("team.ball_distance_team_mate_all_players", "team.ball_distance_team_mate_all_players.player", "team.mate", &player);

  engine.registerBooleanInputSymbol("team.kickoff_in_progress", &behaviorData.kickoffInProgress);
  engine.registerBooleanInputSymbolEnumeratedParameter("team.kickoff_in_progress", "team.kickoff_in_progress.player", "team.mate", &player);

  engine.registerDecimalInputSymbol("team.ball_position_all_players.x", &staticGetBallPositionAllPlayersX);
  engine.registerDecimalInputSymbol("team.ball_position_all_players.y", &staticGetBallPositionAllPlayersY);

}

double BH2009TeamSymbols::getBallDistance()
{
  if(player == robotInfo.number)
    return BH2009BallSymbols::staticGetBallDistance();
  if(player >= TeamMateData::firstPlayer && player < TeamMateData::numOfPlayers)
    return teamMateData.ballModels[player].estimate.getDistance();
  return 0;
}

double BH2009TeamSymbols::distanceTo()
{
  if(player == robotInfo.number)
    return BH2009LocatorSymbols::distanceTo();
  if(player >= TeamMateData::firstPlayer && player < TeamMateData::numOfPlayers)
    return Geometry::distanceTo(teamMateData.robotPoses[player], Vector2<double>(xPosition, yPosition));
  return 0;
}

double BH2009TeamSymbols::getBallAngle()
{
  if(player == robotInfo.number)
    return BH2009BallSymbols::staticGetBallAngle();
  if(player >= TeamMateData::firstPlayer && player < TeamMateData::numOfPlayers)
    return toDegrees(teamMateData.ballModels[player].estimate.getAngle());
  return 0;
}

double BH2009TeamSymbols::getBallX()
{
  if(player == robotInfo.number)
    return BH2009BallSymbols::staticGetBallPositionRobotX();
  if(player >= TeamMateData::firstPlayer && player < TeamMateData::numOfPlayers)
    return teamMateData.ballModels[player].estimate.position.x;
  return 0;
}

double BH2009TeamSymbols::getBallY()
{
  if(player == robotInfo.number)
    return BH2009BallSymbols::staticGetBallPositionRobotY();
  if(player >= TeamMateData::firstPlayer && player < TeamMateData::numOfPlayers)
    return teamMateData.ballModels[player].estimate.position.y;
  return 0;
}

double BH2009TeamSymbols::getBallPositionFieldX()
{
  if(player == robotInfo.number)
    return BH2009BallSymbols::staticGetBallPositionRobotX();
  if(player >= TeamMateData::firstPlayer && player < TeamMateData::numOfPlayers)
    return teamMateData.ballModels[player].estimate.getPositionInFieldCoordinates(teamMateData.robotPoses[player]).x;
  return 0;
}

double BH2009TeamSymbols::getBallPositionFieldY()
{
  if(player == robotInfo.number)
    return BH2009BallSymbols::staticGetBallPositionRobotY();
  if(player >= TeamMateData::firstPlayer && player < TeamMateData::numOfPlayers)
    return teamMateData.ballModels[player].estimate.getPositionInFieldCoordinates(teamMateData.robotPoses[player]).y;
  return 0;
}

double BH2009TeamSymbols::getLocatorPoseX()
{
  if(player == robotInfo.number)
    return BH2009LocatorSymbols::getPoseX();
  if(player >= TeamMateData::firstPlayer && player < TeamMateData::numOfPlayers)
    return teamMateData.robotPoses[player].translation.x;
  return 0;
}

double BH2009TeamSymbols::getLocatorPoseY()
{
  if(player == robotInfo.number)
    return BH2009LocatorSymbols::getPoseY();
  if(player >= TeamMateData::firstPlayer && player < TeamMateData::numOfPlayers)
    return teamMateData.robotPoses[player].translation.y;
  return 0;
}

double BH2009TeamSymbols::getLocatorPoseAngle()
{
  if(player == robotInfo.number)
    return BH2009LocatorSymbols::getPoseAngle();
  if(player >= TeamMateData::firstPlayer && player < TeamMateData::numOfPlayers)
    return toDegrees(teamMateData.robotPoses[player].getAngle());
  return 0;
}

double BH2009TeamSymbols::getConnectedPlayers()
{
  return teamMateData.numOfConnectedPlayers;
}

int BH2009TeamSymbols::getFirstTeamMate()
{
  return teamMateData.firstTeamMate;
}

int BH2009TeamSymbols::getSecondTeamMate()
{
  return teamMateData.secondTeamMate;
}

bool BH2009TeamSymbols::getLostConnection()
{
  return teamMateData.numOfConnectedPlayers <= 0 && teamMateData.wasConnected;
}

int BH2009TeamSymbols::getRole()
{
  if(player >= TeamMateData::firstPlayer && player < TeamMateData::numOfPlayers)
    return teamMateData.behaviorData[player].role;
  return BehaviorData::undefined;
}

int BH2009TeamSymbols::getAction()
{
  if(player >= TeamMateData::firstPlayer && player < TeamMateData::numOfPlayers)
    return teamMateData.behaviorData[player].action;
  return BehaviorData::unknown;
}

bool BH2009TeamSymbols::isTeamAttacking()
{
  if(frameInfo.getTimeSince(ballModel.timeWhenLastSeen) < 2000)
  {
    if(ballModel.estimate.getPositionInFieldCoordinates(robotPose).x > -500.)
      return true;
  }
  else
  {
    for(int i = TeamMateData::firstPlayer; i < TeamMateData::numOfPlayers; ++i)
    //  if(teamMateData.behaviorData[i].role == BehaviorData::striker)
    //I think you wanted to test the timestamp here or?
     //if(frameInfo.getTimeSince(teamMateData.timeStamps[i] && 
     //   teamMateData.ballModels[i].estimate.getPositionInFieldCoordinates(teamMateData.robotPoses[i]).x > 0.)
        if(frameInfo.getTimeSince(teamMateData.ballModels[i].timeWhenLastSeen) < 2000 && teamMateData.ballModels[i].estimate.getPositionInFieldCoordinates(teamMateData.robotPoses[i]).x > -500.)
          return true;
  }


//This would make when I sometime had the ball and not seen rolling away, return true
 /* if(behaviorData.ballHold)
    return true;

//This would make, that when at sometime in the past someone of the robots had somehow the ball and when he never saw it somewhere else far away, then return
true
  for(int i = TeamMateData::firstPlayer; i < TeamMateData::numOfPlayers; ++i)
    if(teamMateData.timeStamps[i] && teamMateData.behaviorData[i].ballHold)
      return true;*/
  return false;
}

double BH2009TeamSymbols::getBallDistanceAllPlayers()
{
  if(frameInfo.getTimeSince(ballModel.timeWhenLastSeen) < 5000 || teamMateData.numOfConnectedPlayers == 0)
    return ballModel.estimate.getDistance();
  else 
  {
    int minTimeSinceBallSeen = std::numeric_limits<int>::max();
    int teamMate = 0;
    for(int i = TeamMateData::firstPlayer; i < TeamMateData::numOfPlayers; i++)
    {
      if(frameInfo.getTimeSince(teamMateData.timeStamps[i]) < networkTimeout && frameInfo.getTimeSince(teamMateData.ballModels[i].timeWhenLastSeen) < minTimeSinceBallSeen)
      {
        minTimeSinceBallSeen = frameInfo.getTimeSince(teamMateData.ballModels[i].timeWhenLastSeen);
        teamMate = i;
      }
    }
    ASSERT(teamMate >=TeamMateData::firstPlayer && teamMate < TeamMateData::numOfPlayers);
    return Geometry::fieldCoord2Relative(robotPose, teamMateData.ballModels[teamMate].estimate.getPositionInFieldCoordinates(teamMateData.robotPoses[teamMate])).abs();
  }
}

Vector2<double> BH2009TeamSymbols::computeBallPositionAllPlayers()
{
  if(frameInfo.getTimeSince(ballModel.timeWhenLastSeen) < 5000 || teamMateData.numOfConnectedPlayers == 0)
    return ballModel.lastPerception.getPositionInFieldCoordinates(robotPose);
  else 
  {
    int minTimeSinceBallSeen = std::numeric_limits<int>::max();
    int teamMate = 0;
    for(int i = TeamMateData::firstPlayer; i < TeamMateData::numOfPlayers; i++)
    {
      if(frameInfo.getTimeSince(teamMateData.timeStamps[i]) < networkTimeout && frameInfo.getTimeSince(teamMateData.ballModels[i].timeWhenLastSeen) < minTimeSinceBallSeen)
      {
        minTimeSinceBallSeen = frameInfo.getTimeSince(teamMateData.ballModels[i].timeWhenLastSeen);
        teamMate = i;
      }
    }
    ASSERT(teamMate >=TeamMateData::firstPlayer && teamMate < TeamMateData::numOfPlayers);
    return teamMateData.ballModels[teamMate].lastPerception.getPositionInFieldCoordinates(teamMateData.robotPoses[teamMate]);
  }
} 

  double BH2009TeamSymbols::getBallPositionAllPlayersX()
  {
    return computeBallPositionAllPlayers().x;
  }

  double BH2009TeamSymbols::getBallPositionAllPlayersY()
  {
    return computeBallPositionAllPlayers().y;
  }

double BH2009TeamSymbols::getBallDistanceTeamMateAllPlayers()
{
  if(frameInfo.getTimeSince(teamMateData.ballModels[player].timeWhenLastSeen) < 5000)
    return teamMateData.ballModels[player].estimate.getDistance();
  else
  {
    int minTimeSinceBallSeen = std::numeric_limits<int>::max();
    int teamMate = 0;
    for(int i = TeamMateData::firstPlayer; i < TeamMateData::numOfPlayers; i++)
    {
      if(i != player)
      {
        if(i != robotInfo.number)
        {
          if(frameInfo.getTimeSince(teamMateData.timeStamps[i]) < networkTimeout && frameInfo.getTimeSince(teamMateData.ballModels[i].timeWhenLastSeen) < minTimeSinceBallSeen)
          {
           minTimeSinceBallSeen = frameInfo.getTimeSince(teamMateData.ballModels[i].timeWhenLastSeen);
           teamMate = i;
          }
        }
        else
        {
          if(frameInfo.getTimeSince(ballModel.timeWhenLastSeen) < minTimeSinceBallSeen)
          {
            minTimeSinceBallSeen = frameInfo.getTimeSince(ballModel.timeWhenLastSeen);
            teamMate = i;
          }
        }
      }
    }
      if(teamMate != robotInfo.number)
        return Geometry::fieldCoord2Relative(teamMateData.robotPoses[player], teamMateData.ballModels[teamMate].estimate.getPositionInFieldCoordinates(teamMateData.robotPoses[teamMate])).abs();
      else
        return Geometry::fieldCoord2Relative(teamMateData.robotPoses[player], ballModel.estimate.getPositionInFieldCoordinates(robotPose)).abs();
  }
}
