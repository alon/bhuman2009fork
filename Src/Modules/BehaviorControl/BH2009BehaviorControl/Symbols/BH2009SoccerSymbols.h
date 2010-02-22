/** 
* \file BH2009SoccerSymbols.h
* Implementation of symbols for our 4 roles.
* \author Colin Graf
*/

#ifndef __BH2009SoccerSymbols_h_
#define __BH2009SoccerSymbols_h_

#include "../../Symbols.h"
#include "Representations/BehaviorControl/BehaviorData.h"
#include "Representations/Perception/GoalPercept.h"
#include "Representations/Modeling/RobotPose.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Configuration/FieldDimensions.h"
#include "Representations/Modeling/BallModel.h"


/**
* \class BH2009SoccerSymbols
* The Xabsl symbols that are defined in "soccer_symbols.xabsl"
* \author Colin Graf
*/ 
class BH2009SoccerSymbols : public Symbols
{
public:
  /**
  * Constructor.
  */
  BH2009SoccerSymbols(BehaviorData& behaviorData, const GoalPercept& goalPercept, const RobotPose& robotPose, const FrameInfo& frameInfo, const FieldDimensions& fieldDimensions, const BallModel& ballModel) :

      behaviorData(behaviorData),
      robotPose(robotPose),
      goalPercept(goalPercept),
      frameInfo(frameInfo),
      fieldDimensions(fieldDimensions),
      ballModel(ballModel),

      disablePreInitialState(false)
  {
    theInstance = this;
  }

  PROCESS_WIDE_STORAGE_STATIC BH2009SoccerSymbols* theInstance;

  /** registers the symbols at an engine */
  void registerSymbols(xabsl::Engine& engine);

  /** updates the symbols */
  void update();

  /** initialize the symbols */
  void init();

static double staticSoccerOpponentGoalAngle(){return theInstance->soccerOpponentGoalAngle();}

double soccerOpponentGoalAngle();
double computePositionNextBallX();
double computePositionNextBallY();
double computeAngleNextBall();
double computePositionBehindBallX();
double computePositionBehindBallY();
double computePositionBehindBallAngle();
Vector2<double> computePositionBehindBall();
Vector2<double> computePosition();

static double staticSoccerPositionNextBallX(){return theInstance-> computePositionNextBallX();};
static double staticSoccerPositionNextBallY(){return theInstance-> computePositionNextBallY();};
static double staticSoccerAngleNextToBall(){return theInstance-> computeAngleNextBall();};
static double staticSoccerPositionBehindBallX(){return theInstance-> computePositionBehindBallX();};
static double staticSoccerPositionBehindBallY(){return theInstance-> computePositionBehindBallY();};
static double staticSoccerPositionBehindBallAngle(){return theInstance-> computePositionBehindBallAngle();};

private:
  BehaviorData& behaviorData;
  const RobotPose& robotPose;
  const GoalPercept& goalPercept;
  const FrameInfo& frameInfo;
  const FieldDimensions& fieldDimensions;
  const BallModel& ballModel;

  bool disablePreInitialState;
  bool sideLeft;

};


#endif // __BH2009SoccerSymbols_h_

