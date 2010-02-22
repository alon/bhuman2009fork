/** 
* @file BH2009GameSymbols.cpp
*
* Implementation of class BH2009GameSymbols.
*
* @author Judith Müller
*/

#include "BH2009GameSymbols.h"
#include "BH2009TeamSymbols.h"

PROCESS_WIDE_STORAGE BH2009GameSymbols* BH2009GameSymbols::theInstance = 0;

void BH2009GameSymbols::registerSymbols(xabsl::Engine& engine)
{
  engine.registerEnumElement("game.team_color", "game.team_color.blue", TEAM_BLUE);
  engine.registerEnumElement("game.team_color", "game.team_color.red", TEAM_RED);

  engine.registerEnumElement("game.state", "game.state._initial", STATE_INITIAL);
  engine.registerEnumElement("game.state", "game.state.ready", STATE_READY);
  engine.registerEnumElement("game.state", "game.state.set", STATE_SET);
  engine.registerEnumElement("game.state", "game.state.playing", STATE_PLAYING);
  engine.registerEnumElement("game.state", "game.state.finished", STATE_FINISHED);

  engine.registerEnumElement("game.penalty", "game.penalty.none", PENALTY_NONE);
  engine.registerEnumElement("game.penalty", "game.penalty.ball_holding", PENALTY_BALL_HOLDING);
  engine.registerEnumElement("game.penalty", "game.penalty.goalie_pushing", PENALTY_GOALIE_PUSHING);
  engine.registerEnumElement("game.penalty", "game.penalty.player_pushing", PENALTY_PLAYER_PUSHING);
  engine.registerEnumElement("game.penalty", "game.penalty.illegal_defender", PENALTY_ILLEGAL_DEFENDER);
  engine.registerEnumElement("game.penalty", "game.penalty.illegal_defense", PENALTY_ILLEGAL_DEFENSE);
  engine.registerEnumElement("game.penalty", "game.penalty.obstruction", PENALTY_OBSTRUCTION);
  engine.registerEnumElement("game.penalty", "game.penalty.req_for_pickup", PENALTY_REQ_FOR_PICKUP);
  engine.registerEnumElement("game.penalty", "game.penalty.leaving", PENALTY_LEAVING);
  engine.registerEnumElement("game.penalty", "game.penalty.damage", PENALTY_DAMAGE);
  engine.registerEnumElement("game.penalty", "game.penalty.manual", PENALTY_MANUAL);
  engine.registerEnumeratedOutputSymbol("game.team_color", "game.team_color", &setOwnTeamColor ,&getOwnTeamColor);

  engine.registerEnumeratedInputSymbol("game.opponent_team_color", "game.team_color", &getOppTeamColor);

  engine.registerEnumeratedInputSymbol("game.player_number", "role.role", &getPlayerNumber);

  engine.registerEnumeratedOutputSymbol("game.penalty", "game.penalty", &setPenalty, &getPenalty);

  engine.registerDecimalInputSymbol("game.penalty.remaining_time", &getPenaltyRemainingTime);

  engine.registerEnumeratedOutputSymbol("game.state", "game.state", &setState ,&getState);

  engine.registerEnumeratedOutputSymbol("game.kickoff_team", "game.team_color", &setKickoffTeam,&getKickoffTeam);

  engine.registerDecimalInputSymbol("game.own_score", &getOwnScore);

  engine.registerDecimalInputSymbol("game.remaining_time", &getRemainingTime);

  engine.registerBooleanOutputSymbol("game.startup", &startup);

  engine.registerDecimalInputSymbol("game.time_since_playing_started", &staticGetTimeSincePlayingState);

  engine.registerBooleanInputSymbol("game.kickoff_in_progress", &behaviorData.kickoffInProgress);
}

void BH2009GameSymbols::update()
{
  if(gameInfo.state == STATE_PLAYING && lastGameState != STATE_PLAYING)
  {
    timeStamp = frameInfo.time;
    behaviorData.kickoffInProgress = gameInfo.kickOffTeam == ownTeamInfo.teamColor;
    counter = 0;
  }
  lastGameState = gameInfo.state;
  if(behaviorData.kickoffInProgress && frameInfo.getTimeSince(ballModel.timeWhenLastSeen) == 0)
  {
    if(ballModel.lastPerception.getPositionInFieldCoordinates(robotPose).abs() > 700)
      counter++;
    else if(counter != 0)
      counter--;
    if(counter >= 10)
     behaviorData.kickoffInProgress = false;
  }
}

double BH2009GameSymbols::getTimeSincePlayingState()
{
  return (frameInfo.getTimeSince(timeStamp));
}
