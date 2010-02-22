/**
* @file Platform/SimRobotQt/Oracle.h
* Declaration of class Oracle for SimRobotQt.
* @author Colin Graf
*/

#ifndef Oracle_H
#define Oracle_H

#include "Representations/Configuration/JointCalibration.h"
#include "Tools/Math/Vector2.h"

class RoboCupCtrl;
class SimObject;
class Pose2D;
class OdometryData;
class Image;
class SensorData;
class JointData;
class RobotPose;
class BallModel;

/**
* This class provides oracled data from the simulated scene. 
*/
class Oracle
{
public:
  /** Constructor */
  Oracle();

  /** Destructor */
  ~Oracle();

  /**
  * Creates an oracle for the currently constructed robot.
  */
  void init();

  /**
  * Determines the pose of the simulated robot.
  * @param robotPose The determined pose of the robot.
  */
  void getRobotPose(RobotPose& robotPose) const;

  /**
  * Determines the odometry data of the simulated robot.
  * @param robotPose The pose of the robot.
  * @param odometryData The determined odometry data of the robot.
  */
  void getOdometryData(const RobotPose& robotPose, OdometryData& odometryData) const;

  /**
  * Determines the ball model of the simulated ball.
  * @param robotPose The pose of the robot.
  * @param ballModel The determined model of the ball.
  */
  void getBallModel(const RobotPose& robotPose, BallModel& ballModel);

  /**
  * Determines the camera image of the simulated robot.
  * @param image The determined image.
  */
  void getImage(Image& image);

  /**
  * Determines the current joint data of the simulated robot and sets new ones.
  * @param jointRequest The joint data to set.
  * @param jointData The determined joint data.
  */
  void getAndSetJointData(const JointData& jointRequest, JointData& jointData) const;

  /**
  * Determines the sensor data of the simulated robot.
  * @param sensorData The determined sensor data.
  */
  void getSensorData(SensorData& sensorData) const;

  /**
  * Accesses the SimObject of the simulated robot.
  * @return The SimObject.
  */
  SimObject* getRobot() const;

  /**
  * Accesses the SimObject of the simulated ball.
  * @return The SimObject.
  */
  SimObject* getBall() const;

private:
  bool blue; /**< Whether this robot has blue as team color or not. */
  RoboCupCtrl* ctrl; /**< The simulation controller. */
  SimObject* me; /**< The simulated roboter object. */
  SimObject* leftFoot; /**< The simulated left foot of the roboter. */
  SimObject* rightFoot; /**< The simulated right foot of the roboter. */
  SimObject* ball; /**< The simulated ball. */
  unsigned char* imageYUV422x2; /**< Image data in the NAO format interpretation that we use*/
  bool naoV3; /**< Flag, indicates robot model type in simulation */

  Vector2<double> lastBallPosition; /**< The last ball position on field to determine the velocity of the ball. */

  int spJoints[JointData::numOfJoints], /**< The handles to the sensor ports of the joints. */      
      apJoints[JointData::numOfJoints], /**< The handles to the actuator ports of the joints. */
      psJoints[JointData::numOfJoints]; /**< The handles to the power switches of the joints. */
  int spCamera; /**< The handle to the sensor port of the virtual camera. */
  int spAcc;    /**< The handle to the sensor port of the virtual accelerometer. */
  int spGyro;   /**< The handle to the sensor port of the virtual gyrosope. */
  int spFSR[4 * 2]; /**< The handles to the sensor port of the virtual foot pressure sensors */
  int spFSRBump[2];
  int spUsLeft;     /** The handle to the sensor port of the virtual us sensor */
  int spUsRight;    /** The handle to the sensor port of the virtual us sensor */
  int spUsCenterLeft; /** The handle to the sensor port of the virtual us sensor */
  int spUsCenterRight; /** The handle to the sensor port of the virtual us sensor */

  JointCalibration jointCalibration; /**< The simulated robot is perfectly calibrated, but we need the signs. */

  /**
  * Determines the 2-D pose of a SimRobot object.
  * @param obj The object of which the pose will be determined.
  * @param pose2D The 2-D pose of the specified object.
  */
  void getPose2D(const SimObject* obj, Pose2D& pose2D) const;
};

#endif // Oracle_H
