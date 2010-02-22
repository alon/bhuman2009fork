/** 
* @file MotionSymbols.h
*
* Declaration of class MotionSymbols.
*
* @author Max Risler
*/

#ifndef __MotionSymbols_h_
#define __MotionSymbols_h_

#include "../Symbols.h"
#include "Representations/MotionControl/MotionRequest.h"
#include "Representations/MotionControl/MotionInfo.h"
#include "Representations/MotionControl/WalkingEngineOutput.h"
#include "Representations/Infrastructure/RobotInfo.h"
#include "Representations/Configuration/RobotName.h"
#include "Representations/Modeling/RobotPose.h"

/**
* The Xabsl symbols that are defined in "motion_symbols.xabsl"
*
* @author Max Risler
*/ 
class MotionSymbols : public Symbols
{
public:
  /*
  * Constructor.
  * @param motionRequest A reference to the MotionRequest.
  * @param motionInfo A reference to the MotionInfo.
  * @param walkingEngineOutput A reference to the WalkingEngineOutput.
  * @param robotInfo A reference to the RobotInfo.
  */
  MotionSymbols(MotionRequest& motionRequest, const MotionInfo& motionInfo, const WalkingEngineOutput& walkingEngineOutput, const RobotInfo& robotInfo, const RobotName& robotName, const RobotPose& robotPose) : 
    motionRequest(motionRequest),
    motionInfo(motionInfo),
    walkingEngineOutput(walkingEngineOutput),
    robotInfo(robotInfo),
    robotName(robotName),
    robotPose(robotPose),
    lastSpecialAction(SpecialActionRequest::playDead)
  {
    theInstance = this;
  }

  PROCESS_WIDE_STORAGE_STATIC MotionSymbols* theInstance; /**< Points to the only instance of this class in this process or is 0 if there is none. */

  MotionRequest& motionRequest; /**< A reference to the MotionRequest */
  const MotionInfo& motionInfo; /**< A reference to the MotionInfo */  
  const WalkingEngineOutput& walkingEngineOutput; /**< A reference to the walkingEngineOutput (to get the MaxSpeeds) */
  const RobotInfo& robotInfo; /**< A reference to the RobotInfo */ 
  const RobotName& robotName; /**< A reference to the RobotName */ 
  const RobotPose& robotPose; /**< A reference to the RobotPose */ 

  SpecialActionRequest::SpecialActionID lastSpecialAction; /**< The last requested not-specialized special action. */

  /** registers the symbols at an engine */
  void registerSymbols(xabsl::Engine& engine);
  void update();

  static void setWalkSpeedX(double x);
  static double getWalkSpeedX();
  static void setWalkSpeedY(double y);
  static double getWalkSpeedY();
  static void setWalkSpeedRot(double rot);
  static double getWalkSpeedRot();
  static double getExecutedWalkSpeedRot();
  static void setPercentualSpeedX(double percent);
  static double getPercentualSpeedX();
  static void setPercentualSpeedY(double percent);
  static double getPercentualSpeedY();
  static void setPercentualSpeedRot(double percent);
  static double getPercentualSpeedRot();
  static void setSpecialAction(int specialAction);
  static int getSpecialAction();
  static void setMotionType(int type);
  static int getMotionType();
  static void setWalkTargetX(double x);
  static double getWalkTargetX();
  static void setWalkTargetY(double y);
  static double getWalkTargetY();
  static void setWalkTargetRot(double rot);
  static double getWalkTargetRot();
  static bool getWalkTargetReached();

  static void setKick(int kick);
  static int getKick();

  static bool getNaoKickZReady();
  static bool getNaoKickReady();

  double ballX, ballY, hitZ, destAng, strikeOutFactor;
};

#endif // __MotionSymbols_h_

