/** 
* @file GameDataProvider.h
* This file implements a module that provides the data received from the game controller.
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
*/

#ifndef __GameDataProvider_h_
#define __GameDataProvider_h_

#include "Tools/Module/Module.h"
#include "Representations/Configuration/FieldDimensions.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/RobotInfo.h"
#include "Representations/Configuration/RobotName.h"
#include "Representations/Infrastructure/TeamInfo.h"
#include "Representations/Infrastructure/GameInfo.h"
#include "Representations/BehaviorControl/BehaviorControlOutput.h"
#include "Platform/GameHandler.h"

MODULE(GameDataProvider)
  REQUIRES(FieldDimensions)
  REQUIRES(FrameInfo)
  REQUIRES(RobotInfo)
  REQUIRES(RobotName)
  USES(BehaviorControlOutput)
  PROVIDES_WITH_MODIFY(RobotInfo)
  PROVIDES(OwnTeamInfo)
  PROVIDES_WITH_MODIFY(OpponentTeamInfo)
  PROVIDES_WITH_MODIFY(GameInfo)
END_MODULE

class GameDataProvider : public GameDataProviderBase
{
private:
  GAME_CONTROL; /**< The connection to the game controller. */
  unsigned gameControlTimeStamp; /**< The time when the last GameControlData was received. */
  bool ownTeamInfoSet; /**< Was the own team info already set? */
  RoboCup::RoboCupGameControlData gameControlData; /**< The game control data received from the game controller. */

  void update(RobotInfo& robotInfo);
  void update(OwnTeamInfo& ownTeamInfo);
  void update(OpponentTeamInfo& opponentTeamInfo);
  void update(GameInfo& gameInfo);

  void init();

public:
  /**
  * Default constructor.
  */
  GameDataProvider();
};

#endif // __GameDataProvider_h_
