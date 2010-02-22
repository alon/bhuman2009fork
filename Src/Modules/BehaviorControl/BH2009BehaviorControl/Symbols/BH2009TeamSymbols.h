/** 
* \file BH2009TeamSymbols.h
* Implementation of symbols for TeamMateData.
* \author Colin Graf
*/

#ifndef __BH2009TeamSymbols_h_
#define __BH2009TeamSymbols_h_

#include "../../Symbols.h"
#include "Representations/Infrastructure/RobotInfo.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/TeamMateData.h"
#include "Representations/Modeling/BallModel.h"
#include "Representations/Configuration/FieldDimensions.h"

/**
* \class BH2009TeamSymbols
* The Xabsl symbols that are defined in "team_symbols.xabsl"
* \author Colin Graf
*/ 
class BH2009TeamSymbols : public Symbols
{
public:
  /**
  * Constructor.
  * \param robotInfo A reference to the RobotInfo.
  * \param robotPose A reference to the RobotPose.
  * \param ballModel A reference to the BallModel.
  * \param teamMateData A refernece to the TeamMateData.
  * \param fieldDimensions A reference to the FieldDimensions.
  * \param frameInfo a reference to the FrameInfo.
  * \param behaviorData A reference to the BehaviorData.
  */
  BH2009TeamSymbols(
    const RobotInfo& robotInfo, 
    const RobotPose& robotPose,
    const BallModel& ballModel,
    const TeamMateData& teamMateData,
    const FieldDimensions& fieldDimensions,
    const FrameInfo& frameInfo,
    const BehaviorData& behaviorData) : 
      robotInfo(robotInfo),
      ballModel(ballModel),
      robotPose(robotPose),
      teamMateData(teamMateData),
      fieldDimensions(fieldDimensions),
      frameInfo(frameInfo),
      behaviorData(behaviorData)
  {
    theInstance = this;
  }

  PROCESS_WIDE_STORAGE_STATIC BH2009TeamSymbols* theInstance;

  const static int networkTimeout = 2000;

  /** registers the symbols at an engine */
  void registerSymbols(xabsl::Engine& engine);

  /** updates the symbols */
  void update() {};
  static Vector2<double>  staticComputeBallPositionAllPlayers() { return theInstance->computeBallPositionAllPlayers();}

private:

  double getBallDistance();
  double distanceTo();
  double getBallAngle();
  double getBallX();
  double getBallY();
  double getBallPositionFieldX();
  double getBallPositionFieldY();
  double getLocatorPoseX();
  double getLocatorPoseY();
  double getLocatorPoseAngle();
  double getConnectedPlayers();
  int getFirstTeamMate();
  int getSecondTeamMate();
  bool getLostConnection();
  int getRole();
  int getAction();
  bool isTeamAttacking();
  double getBallDistanceAllPlayers();
  double getBallDistanceTeamMateAllPlayers();
  double getBallPositionAllPlayersX();
  double getBallPositionAllPlayersY();
  Vector2<double> computeBallPositionAllPlayers();


  // data sources
  const RobotInfo& robotInfo;
  const BallModel& ballModel;
  const RobotPose& robotPose;
  const TeamMateData& teamMateData;
  const FieldDimensions& fieldDimensions;
  const FrameInfo& frameInfo;
  const BehaviorData& behaviorData;

  // symbol parameter
  int player;
  double xPosition;
  double yPosition;

  // static symbol provider functions for the xabsl engine
  static double staticGetBallDistance() { return theInstance->getBallDistance(); };
  static double staticDistanceTo() { return theInstance->distanceTo(); };
  static double staticGetBallAngle() { return theInstance->getBallAngle(); };
  static double staticGetBallX() { return theInstance->getBallX(); };
  static double staticGetBallY() { return theInstance->getBallY(); };
  static double staticGetBallPositionFieldX() { return theInstance->getBallPositionFieldX(); };
  static double staticGetBallPositionFieldY() { return theInstance->getBallPositionFieldY(); };
  static double staticGetLocatorPoseX() { return theInstance->getLocatorPoseX(); };
  static double staticGetLocatorPoseY() { return theInstance->getLocatorPoseY(); };
  static double staticGetLocatorPoseAngle() { return theInstance->getLocatorPoseAngle(); };
  static double staticGetConnectedPlayers() { return theInstance->getConnectedPlayers(); }
  static int staticGetFirstTeamMate() { return theInstance->getFirstTeamMate(); }
  static int staticGetSecondTeamMate() { return theInstance->getSecondTeamMate(); }
  static bool staticGetLostConnection() { return theInstance->getLostConnection(); }
  static int staticGetRole() { return theInstance->getRole(); }
  static int staticGetAction() { return theInstance->getAction(); }
  static bool staticIsTeamAttacking() { return theInstance->isTeamAttacking(); }
  static double staticGetBallDistanceAllPlayers() { return theInstance->getBallDistanceAllPlayers(); }
  static double staticGetBallPositionAllPlayersX() { return theInstance->getBallPositionAllPlayersX(); }
  static double staticGetBallPositionAllPlayersY() { return theInstance->getBallPositionAllPlayersY(); }
  static double staticGetBallDistanceTeamMateAllPlayers() { return theInstance->getBallDistanceTeamMateAllPlayers(); }


  friend class BH2009SoccerSymbols; 
  friend class BH2009RoleSymbols;
};


#endif // __BH2009TeamSymbols_h_

