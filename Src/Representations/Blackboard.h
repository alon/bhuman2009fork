/** 
* @file Blackboard.h
* Declaration of a class representing the blackboard.
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
*/

#ifndef __Blackboard_h_
#define __Blackboard_h_

#include <stddef.h>

#include "Platform/SystemCall.h"

// Declare prototypes of all representations here:

// Infrastructure
class JointData;
class JointRequest;
class SensorData;
class KeyStates;
class LEDRequest;
class Image;
class CameraInfo;
class FrameInfo;
class CognitionFrameInfo;
class RobotInfo;
class OwnTeamInfo;
class OpponentTeamInfo;
class GameInfo;
class TeamStatus;
class SoundRequest;
class SoundOutput;
class TeamMateData;
class MotionRobotHealth;
class RobotHealth;
class BoardInfo;

// Configuration
class RobotName;
class ColorTable64;
class CameraSettings;
class FieldDimensions;
class RobotDimensions;
class JointCalibration;
class SensorCalibration;
class CameraCalibration;
class BehaviorConfiguration;
class MassCalibration;

// Perception
class CameraMatrix;
class RobotCameraMatrix;
class InertiaMatrix;
class ImageCoordinateSystem;
class BallSpots;
class LineSpots;
class BallPercept;
class ExtendedBallPercepts;
class LinePercept;
class RegionPercept;
class FilteredJointData;
class FilteredSensorData;
class GoalPercept;
class GroundContactState;

// Modeling
class FallDownState;
class BallModel;
class GroundTruthBallModel;
class ObstacleModel;
class RobotPose;
class GroundTruthRobotPose;
class RobotPoseHypotheses;
class SelfLocatorSampleSet;
class BallLocatorSampleSet;
class RobotModel;
class BodyContour;

// BehaviorControl
class BehaviorControlOutput;

// MotionControl
class OdometryData;
class GroundTruthOdometryData;
class MotionRequest;
class HeadMotionRequest;
class HeadJointRequest;
class MotionSelection;
class SpecialActionsOutput;
class WalkingEngineOutput;
class WalkingEngineStandOutput;
class MotionInfo;
class UnstableJointRequest;
class SensorPrediction;

// friends
class Process;
class Cognition;
class Motion;

/**
* @class Blackboard
* The class represents the blackboard that contains all representation.
* Note: The blackboard only contains references to the objects as attributes.
* The actual representations are constructed on the heap, because many copies of
* of the blackboard exist but only a single set of the representations shared
* by all instances.
*/
class Blackboard
{
protected:
  // Add all representations as constant references here:
  // Infrastructure
  const JointData& theJointData;
  const JointRequest& theJointRequest;
  const SensorData& theSensorData;
  const KeyStates& theKeyStates;
  const LEDRequest& theLEDRequest;
  const Image& theImage;
  const CameraInfo& theCameraInfo;
  const FrameInfo& theFrameInfo;
  const CognitionFrameInfo& theCognitionFrameInfo;
  const RobotInfo& theRobotInfo;
  const OwnTeamInfo& theOwnTeamInfo;
  const OpponentTeamInfo& theOpponentTeamInfo;
  const GameInfo& theGameInfo;
  const TeamStatus& theTeamStatus;
  const SoundRequest& theSoundRequest;
  const SoundOutput& theSoundOutput;
  const TeamMateData& theTeamMateData;
  const MotionRobotHealth& theMotionRobotHealth;
  const RobotHealth& theRobotHealth;
  const BoardInfo& theBoardInfo;

  // Configuration
  const RobotName& theRobotName;
  const ColorTable64& theColorTable64;
  const CameraSettings& theCameraSettings;
  const FieldDimensions& theFieldDimensions;
  const RobotDimensions& theRobotDimensions;
  const JointCalibration& theJointCalibration;
  const SensorCalibration& theSensorCalibration;
  const CameraCalibration& theCameraCalibration;
  const BehaviorConfiguration& theBehaviorConfiguration;
  const MassCalibration& theMassCalibration;

  // Perception
  const CameraMatrix& theCameraMatrix;
  const RobotCameraMatrix& theRobotCameraMatrix;
  const InertiaMatrix& theInertiaMatrix;
  const ImageCoordinateSystem& theImageCoordinateSystem;
  const BallSpots& theBallSpots;
  const LineSpots& theLineSpots;
  const BallPercept& theBallPercept;
  const ExtendedBallPercepts& theExtendedBallPercepts;
  const LinePercept& theLinePercept;
  const RegionPercept& theRegionPercept;
  const FilteredJointData& theFilteredJointData;
  const FilteredSensorData& theFilteredSensorData;
  const GoalPercept& theGoalPercept;
  const GroundContactState& theGroundContactState;

  // Modeling
  const FallDownState& theFallDownState;
  const BallModel& theBallModel;
  const GroundTruthBallModel& theGroundTruthBallModel;
  const ObstacleModel& theObstacleModel;
  const RobotPose& theRobotPose;
  const GroundTruthRobotPose& theGroundTruthRobotPose;
  const RobotPoseHypotheses& theRobotPoseHypotheses;
  const SelfLocatorSampleSet& theSelfLocatorSampleSet;
  const BallLocatorSampleSet& theBallLocatorSampleSet;
  const RobotModel& theRobotModel;
  const BodyContour& theBodyContour;

  // BehaviorControl
  const BehaviorControlOutput& theBehaviorControlOutput;

  // MotionControl
  const OdometryData& theOdometryData;
  const GroundTruthOdometryData& theGroundTruthOdometryData;
  const MotionRequest& theMotionRequest;
  const HeadMotionRequest& theHeadMotionRequest;
  const HeadJointRequest& theHeadJointRequest;
  const MotionSelection& theMotionSelection;
  const SpecialActionsOutput& theSpecialActionsOutput;
  const WalkingEngineOutput& theWalkingEngineOutput;
  const WalkingEngineStandOutput& theWalkingEngineStandOutput;
  const MotionInfo& theMotionInfo;
  const UnstableJointRequest& theUnstableJointRequest;
  const SensorPrediction& theSensorPrediction;

  PROCESS_WIDE_STORAGE_STATIC Blackboard* theInstance; /**< The only real instance in the current process. */

  /**
  * The method is a dummy that is called to prevent the compiler from certain
  * optimizations in a method generated in Module.h.
  * It is empty, but important, not defined inline.
  */
  static void distract();

private:
  /**
  * Default constructor.
  */
  Blackboard();

public:
  /**
  * Virtual destructor.
  * Required for derivations of this class.
  */
  virtual ~Blackboard() {}

  /**
  * Assignment operator.
  * Note: copies will share all representations.
  * @param other The instance that is cloned.
  */
  void operator=(const Blackboard& other);

  /**
  * The operator allocates a memory block that is zeroed.
  * Therefore, all members of this class are initialized with 0.
  * @attention This operator is only called if this class is instantiated by
  * a separate call to new, i.e. it cannot be created as a part of another class.
  * @param size The size of the block in bytes.
  * @return A pointer to the block.
  */
  static void* operator new(size_t size);

  /**
  * The operator frees a memory block.
  * @param p The address of the block to free.
  */
  static void operator delete(void* p);

  friend class Process; /**< The class Process can set theInstance. */
  friend class Cognition; /**< The class Cognition can read theInstance. */
  friend class Motion; /**< The class Motion can read theInstance. */
};

#endif //__Blackboard_h_
