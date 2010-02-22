/** 
* @file GameDataProvider.cpp
* This file implements a module that provides the data received from the game controller.
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
*/

#include "GameDataProvider.h"
#include "Tools/Settings.h"

GameDataProvider::GameDataProvider() :
gameControlTimeStamp(0),
ownTeamInfoSet(false)
{
  memset(&gameControlData, 0, sizeof(gameControlData));  
  gameControlData.teams[0].teamNumber = Global::getSettings().teamNumber;
  gameControlData.teams[0].teamColor = Global::getSettings().teamColor;
  START_GAME_CONTROL;
}

void GameDataProvider::init()
{
  gameControlTimeStamp = theFrameInfo.time;
}

void GameDataProvider::update(RobotInfo& robotInfo)
{
  if(RECEIVE_GAME_CONTROL(gameControlData))
    gameControlTimeStamp = theFrameInfo.time;
  
  bool received = false;
  if(theFrameInfo.getTimeSince(gameControlTimeStamp) < 500)
  {
    for(int i = 0; i < 2; ++i)
      if(Global::getSettings().teamNumber == (int)gameControlData.teams[i].teamNumber)
      {
        const RoboCup::TeamInfo& t = gameControlData.teams[i];
        ASSERT(Global::getSettings().playerNumber >= 1 && Global::getSettings().playerNumber <= MAX_NUM_PLAYERS);
        if(&theBehaviorControlOutput && robotInfo.penalty != theBehaviorControlOutput.robotInfo.penalty)
        {
          RoboCup::RoboCupGameControlReturnData returnPacket;
          memcpy(returnPacket.header, GAMECONTROLLER_RETURN_STRUCT_HEADER, sizeof(returnPacket.header));
          returnPacket.version = GAMECONTROLLER_RETURN_STRUCT_VERSION;
          returnPacket.team = Global::getSettings().teamNumber;
          returnPacket.player = Global::getSettings().playerNumber;
          returnPacket.message = theBehaviorControlOutput.robotInfo.penalty != PENALTY_NONE
                                 ? GAMECONTROLLER_RETURN_MSG_MAN_PENALISE
                                 : GAMECONTROLLER_RETURN_MSG_MAN_UNPENALISE;
          (void) SEND_GAME_CONTROL(returnPacket);
        }
        (RoboCup::RobotInfo&) robotInfo = t.players[Global::getSettings().playerNumber - 1];
        received = true;
      }
  }
  
  if(!received && &theBehaviorControlOutput && Global::getSettings().teamNumber == (int)theBehaviorControlOutput.ownTeamInfo.teamNumber)
    robotInfo = theBehaviorControlOutput.robotInfo;

  robotInfo.number = Global::getSettings().playerNumber;
  robotInfo.robotModel = RobotInfo::getRobotModelByRobotName(theRobotName.name.c_str());
}

void GameDataProvider::update(OwnTeamInfo& ownTeamInfo)
{
  if(!ownTeamInfoSet)
  {
    (RoboCup::TeamInfo&) ownTeamInfo = gameControlData.teams[0];
    ownTeamInfoSet = true;
  }

  if(theFrameInfo.getTimeSince(gameControlTimeStamp) < 500)
  {
    for(int i = 0; i < 2; ++i)
      if(Global::getSettings().teamNumber == (int)gameControlData.teams[i].teamNumber)
      {
        (RoboCup::TeamInfo&) ownTeamInfo = gameControlData.teams[i];
        break;
      }

  }
  else if(&theBehaviorControlOutput && Global::getSettings().teamNumber == (int)theBehaviorControlOutput.ownTeamInfo.teamNumber)
    ownTeamInfo = theBehaviorControlOutput.ownTeamInfo;

  MODIFY("representation:OwnTeamInfo", ownTeamInfo);
  theFieldDimensions.drawPolygons(ownTeamInfo.teamColor);
}

void GameDataProvider::update(OpponentTeamInfo& opponentTeamInfo)
{
  for(int i = 0; i < 2; ++i)
    if(Global::getSettings().teamNumber == (int)gameControlData.teams[i].teamNumber)
      (RoboCup::TeamInfo&) opponentTeamInfo = gameControlData.teams[1 - i];
}

void GameDataProvider::update(GameInfo& gameInfo)
{
  if((theFrameInfo.getTimeSince(gameControlTimeStamp) < 500 || !&theBehaviorControlOutput || Global::getSettings().teamNumber != (int)theBehaviorControlOutput.ownTeamInfo.teamNumber) &&
     (Global::getSettings().teamNumber == (int)gameControlData.teams[0].teamNumber || Global::getSettings().teamNumber == (int)gameControlData.teams[1].teamNumber))
    memcpy(&(RoboCup::RoboCupGameControlData&) gameInfo, &gameControlData, (char*) gameControlData.teams - (char*) &gameControlData);
  else if(&theBehaviorControlOutput && Global::getSettings().teamNumber == (int)theBehaviorControlOutput.ownTeamInfo.teamNumber)
    gameInfo = theBehaviorControlOutput.gameInfo;
}

MAKE_MODULE(GameDataProvider, Infrastructure)
