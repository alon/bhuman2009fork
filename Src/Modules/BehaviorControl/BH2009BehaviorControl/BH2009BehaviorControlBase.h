/**
* @file BH2009BehaviorControlBase.h
* 
* Definition of abstract class BehaviorControlBase
* This just contains the interface specifications for BehaviorControl module solutions.
* @author Max Risler
*/

#ifndef __BH2009BehaviorControlBase_h_
#define __BH2009BehaviorControlBase_h_

#include "Tools/Module/Module.h"

#include "Representations/Configuration/BehaviorConfiguration.h"
#include "Representations/Configuration/FieldDimensions.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/CameraInfo.h"
#include "Representations/Infrastructure/TeamInfo.h"
#include "Representations/Infrastructure/TeamMateData.h"
#include "Representations/Infrastructure/RobotInfo.h"
#include "Representations/Configuration/RobotName.h"
#include "Representations/Infrastructure/GameInfo.h"
#include "Representations/Infrastructure/LEDRequest.h"
#include "Representations/Infrastructure/JointData.h"
#include "Representations/Infrastructure/SoundRequest.h"
#include "Representations/Infrastructure/KeyStates.h"
#include "Representations/Perception/CameraMatrix.h"
#include "Representations/Perception/GoalPercept.h"
#include "Representations/Perception/GroundContactState.h"
#include "Representations/Infrastructure/SensorData.h"
#include "Representations/Modeling/FallDownState.h"
#include "Representations/Modeling/RobotPose.h"
#include "Representations/Modeling/BallModel.h"
#include "Representations/Modeling/ObstacleModel.h"
#include "Representations/MotionControl/MotionInfo.h"
#include "Representations/MotionControl/WalkingEngineOutput.h"
#include "Representations/BehaviorControl/BehaviorControlOutput.h"
#include "Representations/Configuration/JointCalibration.h"
#include "Representations/Configuration/RobotDimensions.h"
#include "Representations/Perception/BallPercept.h"

MODULE(BH2009BehaviorControl)
  //CameraMatrix, JointCalibration, FilteredJointData & CameraInfo is needed by HeadControlSymbols
  REQUIRES(CameraMatrix)
  REQUIRES(FilteredJointData)
  REQUIRES(CameraInfo)
  REQUIRES(FrameInfo)
  REQUIRES(BehaviorConfiguration) 
  REQUIRES(FieldDimensions)
  REQUIRES(FallDownState)
  REQUIRES(KeyStates)
  REQUIRES(MotionInfo)
  REQUIRES(WalkingEngineOutput)
  REQUIRES(BallPercept) // added
  REQUIRES(BallModel)
  REQUIRES(GoalPercept)
  REQUIRES(RobotPose)
  REQUIRES(GroundTruthRobotPose)
  REQUIRES(RobotInfo)
  REQUIRES(RobotName)
  REQUIRES(OwnTeamInfo)
  REQUIRES(GameInfo)
  REQUIRES(TeamMateData)
  REQUIRES(ObstacleModel)
  REQUIRES(JointCalibration)
  REQUIRES(RobotDimensions)
  REQUIRES(FilteredSensorData)
  REQUIRES(GroundContactState)
  PROVIDES_WITH_MODIFY(BehaviorControlOutput)
  REQUIRES(BehaviorControlOutput)
  PROVIDES_WITH_MODIFY_AND_DRAW(MotionRequest)
  PROVIDES_WITH_MODIFY(HeadMotionRequest)
  PROVIDES_WITH_MODIFY_AND_OUTPUT(LEDRequest)
  PROVIDES_WITH_MODIFY(SoundRequest)
END_MODULE

#endif // __BH2009BehaviorControlBase_h_
