/** 
* @file Platform/SimRobotQt/LocalRobot.h
*
* Declaration of LocalRobot.
*
* @author <A href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</A>
*/


#ifndef __LocalRobot_H__
#define __LocalRobot_H__

#include <ControllerQt/RobotConsole.h>
#include "Representations/Infrastructure/Image.h"
#include "Representations/Infrastructure/JointData.h"
#include "Representations/Infrastructure/SensorData.h"
#include "Representations/Modeling/RobotPose.h"
#include "Representations/Modeling/BallModel.h"
#include "Representations/MotionControl/OdometryData.h"
#include "Oracle.h"

/**
* @class LocalRobot
*
* A process that is instantiated to either directly control a physical robot,
* a simulated one, or to replay a logfile.
*/
class LocalRobot : public RobotConsole
{
private:
  DEBUGGING;
  Image image; /**< The simulated image sent to the robot code. */
  JointData jointData; /**< The simulated joint measurements sent to the robot code. */
  SensorData sensorData; /**< The simulated sensor data sent to the robot code. */
  GroundTruthRobotPose robotPose; /**< The simulated ground truth robot pose sent to the robot code. */
  GroundTruthBallModel ballModel; /**< The simulated ground truth ball model sent to the robot code. */
  GroundTruthOdometryData odometryData; /**< The simulated odometry data sent to the robot code. */
  unsigned imageTimeStamp, /**< The theoretical timestamp of the last calculated image. */
           imageLastTimeStampSent, /**< The timestamp of the last sent image. */
           jointLastTimeStampSent; /**< The timestamp pf the last sent joint data. */
  Oracle oracle;

public:
  /**
  * Constructor.
  */
  LocalRobot();

  /**
  * The function is called from the framework once in every frame 
  */
  virtual int main();

  /**
  * The function must be called to exchange data with SimRobot.
  * It sends the motor commands to SimRobot and acquires new sensor data.
  */
  void update();
};

#endif // __LocalRobot_H__
