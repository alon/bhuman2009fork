/**
* @file TeamInfo.cpp
* The file implements a class that encapsulates the structure TeamInfo defined in
* the file RoboCupGameControlData.h that is provided with the GameController.
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
*/

#include "TeamInfo.h"

/**
* The class is a helper to be able to stream the players.
* The global RobotInfo cannot be used, because it has an additional attribute.
*/
class PlayerInfo : public RoboCup::RobotInfo, public ImplicitlyStreamable {};

/**
* Write a player info to a stream.
* @param stream The stream that is written to.
* @param playerInfo The data that is written.
* @return The stream.
*/
Out& operator<<(Out& stream, const PlayerInfo& playerInfo)
{
  STREAM_REGISTER_BEGIN_EXT(playerInfo);
  STREAM_EXT(stream, playerInfo.penalty);
  STREAM_EXT(stream, playerInfo.secsTillUnpenalised);
  STREAM_REGISTER_FINISH();
  return stream;
}

/**
* Read a player info from a stream.
* @param stream The stream that is read from.
* @param playerInfo The data that is read.
* @return The stream.
*/
In& operator>>(In& stream, PlayerInfo& playerInfo)
{
  STREAM_REGISTER_BEGIN_EXT(playerInfo);
  STREAM_EXT(stream, playerInfo.penalty);
  STREAM_EXT(stream, playerInfo.secsTillUnpenalised);
  STREAM_REGISTER_FINISH();
  return stream;
}

void TeamInfo::serialize(In* in, Out* out)
{  
  PlayerInfo (&players)[4] = reinterpret_cast<PlayerInfo (&)[4]>(this->players);
  
  STREAM_REGISTER_BEGIN();
  STREAM(teamNumber); // unique team number
  STREAM(teamColor); // TEAM_BLUE, TEAM_RED
  STREAM(score); // team's score
  STREAM_ARRAY(players); // the team's players
  STREAM_REGISTER_FINISH();
}
