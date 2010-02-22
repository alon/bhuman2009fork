/** 
* @file BH2009BallSymbols.h
*
* Declaration of class BallSymbols.
*
* @author Max Risler
* @author Colin Graf
*/

#ifndef __BH2009BallSymbols_h_
#define __BH2009BallSymbols_h_

#include "../../Symbols.h"
#include "Representations/Modeling/BallModel.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/RobotInfo.h"
#include "Representations/Infrastructure/LEDRequest.h"
#include "Representations/Infrastructure/TeamMateData.h"
#include "Representations/Configuration/FieldDimensions.h"


/**
* The Xabsl symbols that are defined in "ball_symbols.xabsl"
*/ 
class BH2009BallSymbols : public Symbols
{
public:
  /** Constructor. */
  BH2009BallSymbols(const RobotInfo& robotInfo, const BallModel& ballModel, const FrameInfo& frameInfo, const RobotPose& robotPose, const TeamMateData& teamMateData, const FieldDimensions& fieldDimensions) : 
    robotInfo(robotInfo),
    frameInfo(frameInfo), 
    ballModel(ballModel), 
    robotPose(robotPose),
    teamMateData(teamMateData),
    fieldDimensions(fieldDimensions),
    ballWasSeen(false), 
    ballPositionRel(0,0), 
    ballPositionField(0,0), 
    ballSpeedRel(0,0), 
    ballSpeedField(0,0),
    timeSinceBallWasSeen(10000.0)
  {
    theInstance = this;

    memset(teamBallPositionsRelTime, 0, sizeof(teamBallPositionsRelTime));
    memset(teamBallPositionsFieldTime, 0, sizeof(teamBallPositionsFieldTime));
  }

  PROCESS_WIDE_STORAGE_STATIC BH2009BallSymbols* theInstance; /**< Points to the only instance of this class in this process or is 0 if there is none. */

  /** registers the symbols at an engine */
  void registerSymbols(xabsl::Engine& engine);

  /** updates the symbols */
  void update();

private:
  const RobotInfo& robotInfo;
  const FrameInfo& frameInfo;
  const BallModel& ballModel;
  const RobotPose& robotPose;
  const TeamMateData& teamMateData;
  const FieldDimensions& fieldDimensions;

  bool ballWasSeen;
  Vector2 <double> ballPositionRel;
  Vector2 <double> seenBallPositionRel;
  Vector2 <double> ballPositionField;
  Vector2 <double> ballSpeedRel;
  Vector2 <double> ballSpeedField;

  static double getBallSpeedFieldAbs();
  static double getBallSpeedRobotAbs();

  unsigned teamBallPositionsRelTime[TeamMateData::numOfPlayers];
  Vector2 <double> teamBallPositionsRel[TeamMateData::numOfPlayers];
  unsigned teamBallPositionsFieldTime[TeamMateData::numOfPlayers];
  Vector2 <double> teamBallPositionsField[TeamMateData::numOfPlayers];

  double getBallFieldRobotX();
  double getBallFieldRobotY();
  double getBallPositionRobotX();
  double getBallPositionRobotY();
  double getBallDistance();
  double getBallSeenDistance();
  double getBallAngle();
  double getSeenBallAngle();
  double getBallSpeed();
  double getYPosWhenBallReachesOwnYAxis();
  double getTimeWhenBallReachesOwnYAxis();

  bool getBallWasSeenByTeamMate();
  double getBallTimeSinceLastSeenByTeamMate();
  double getBallPositionFieldByTeamMateX();
  double getBallPositionFieldByTeamMateY();
  double getBallPositionRobotByTeamMateX();
  double getBallPositionRobotByTeamMateY();
  double getBallDistanceByTeamMate();
  double getBallAngleByTeamMate();
  double getBallDistanceToOwnGoal();

  int player;
  double timeSinceBallWasSeen;

  static double staticGetBallFieldRobotX() { return theInstance->getBallFieldRobotX(); };
  static double staticGetBallFieldRobotY() { return theInstance->getBallFieldRobotY(); };
  static double staticGetBallPositionRobotX() { return theInstance->getBallPositionRobotX(); };
  static double staticGetBallPositionRobotY() { return theInstance->getBallPositionRobotY(); };
  static double staticGetBallDistance() { return theInstance->getBallDistance(); };
  static double staticGetBallSeenDistance() { return theInstance->getBallSeenDistance(); };
  static double staticGetBallAngle() { return theInstance->getBallAngle(); };
  static double staticGetSeenBallAngle() { return theInstance->getSeenBallAngle(); };
  static double staticGetBallSpeed() { return theInstance->getBallSpeed(); };
  static double staticGetYPosWhenBallReachesOwnYAxis() { return theInstance->getYPosWhenBallReachesOwnYAxis(); };
  static double staticGetTimeWhenBallReachesOwnYAxis() { return theInstance->getTimeWhenBallReachesOwnYAxis(); };

  static bool staticGetBallWasSeenByTeamMate() { return theInstance->getBallWasSeenByTeamMate(); };
  static double staticGetBallTimeSinceLastSeenByTeamMate() { return theInstance->getBallTimeSinceLastSeenByTeamMate(); };
  static double staticGetBallPositionFieldByTeamMateX() { return theInstance->getBallPositionFieldByTeamMateX(); };
  static double staticGetBallPositionFieldByTeamMateY() { return theInstance->getBallPositionFieldByTeamMateY(); };
  static double staticGetBallPositionRobotByTeamMateX() { return theInstance->getBallPositionRobotByTeamMateX(); };
  static double staticGetBallPositionRobotByTeamMateY() { return theInstance->getBallPositionRobotByTeamMateY(); };
  static double staticGetBallDistanceByTeamMate() { return theInstance->getBallDistanceByTeamMate(); };
  static double staticGetBallAngleByTeamMate() { return theInstance->getBallAngleByTeamMate(); };
  static double staticGetBallDistanceToOwnGoal() { return theInstance->getBallDistanceToOwnGoal(); };

  friend class BH2009TeamSymbols;
  friend class BH2009SoccerSymbols;
};

#endif // __BallSymbols_h_
