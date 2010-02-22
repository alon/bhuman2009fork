/** 
* @file BH2009GameSymbols.h
*
* Declaration of class BH2009GameSymbols.
*
* @author Judith Müller
*/

#ifndef __BH2009GameSymbols_h_
#define __BH2009GameSymbols_h_

#include "Representations/BehaviorControl/BehaviorData.h"
#include "Representations/Infrastructure/RobotInfo.h"
#include "Representations/Infrastructure/TeamInfo.h"
#include "Representations/Infrastructure/GameInfo.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Modeling/BallModel.h"
#include "../../Symbols.h"

/**
* The Xabsl symbols that are defined in "game_symbols.xabsl"
*
* @author Max Risler
* @author Judith Müller
*/ 
class BH2009GameSymbols : public Symbols
{
public:
  /*
  * Constructor.
  * @param robotInfo A reference to the RobotInfo.
  * @param ownTeamInfo A reference to the OwnTeamInfo.
  * @param opponentTeamInfo A reference to the OpponentTeamInfo.
  * @param gameInfo A reference to the GameInfo.
  */
  BH2009GameSymbols(BehaviorData& behaviorData, RobotInfo& robotInfo, OwnTeamInfo& ownTeamInfo, GameInfo& gameInfo, const FrameInfo& frameInfo, const BallModel& ballModel, const RobotPose& robotPose) : 
    behaviorData(behaviorData),
    robotInfo(robotInfo), 
    ownTeamInfo(ownTeamInfo),
    gameInfo(gameInfo),
    frameInfo(frameInfo),
    ballModel(ballModel),
    robotPose(robotPose),
    startup(true)
  {
    theInstance = this;
    lastGameState = theInstance->gameInfo.state;
  }

  PROCESS_WIDE_STORAGE_STATIC BH2009GameSymbols* theInstance;

  BehaviorData& behaviorData;
  RobotInfo& robotInfo;

  OwnTeamInfo& ownTeamInfo;
  GameInfo& gameInfo;
  const FrameInfo& frameInfo;
  const BallModel& ballModel;
  const RobotPose& robotPose;

  bool startup;
  unsigned char lastGameState;
  unsigned timeStamp;

  double getTimeSincePlayingState();
  /** registers the symbols at an engine */
  void registerSymbols(xabsl::Engine& engine);

  static int getOwnTeamColor(){return (int)theInstance->ownTeamInfo.teamColor;}
  static void setOwnTeamColor(int color){theInstance->ownTeamInfo.teamColor = color;}

  static int getOppTeamColor()
  {
    if ((int)theInstance->ownTeamInfo.teamColor == TEAM_RED) return TEAM_BLUE;
    else return TEAM_RED;
  }

  static int getPlayerNumber(){return theInstance->robotInfo.number;}

  static int getPenalty(){return (int)theInstance->robotInfo.penalty;}
  static void setPenalty(int penalty){theInstance->robotInfo.penalty = penalty;}

  static double getPenaltyRemainingTime(){return (double)theInstance->robotInfo.secsTillUnpenalised;}

  static int getState(){return (int)theInstance->gameInfo.state;}
  static void setState(int state){theInstance->gameInfo.state = state;}

  static int getKickoffTeam(){return (int)theInstance->gameInfo.kickOffTeam;}
  static void setKickoffTeam(int kickoff){theInstance->gameInfo.kickOffTeam = kickoff;}

  static double getOwnScore(){return (double)theInstance->ownTeamInfo.score;}

  static double getRemainingTime(){return (double)theInstance->gameInfo.secsRemaining;}

  static double staticGetTimeSincePlayingState(){return theInstance->getTimeSincePlayingState();}

  void update();

private:

int counter;

};


#endif // __GameSymbols_h_

