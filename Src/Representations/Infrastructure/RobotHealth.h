/**
* @file RobotHealth.h
* The file declares two classes to transport information about the current robot health
* @author <a href="mailto:timlaue@informatik.uni-bremen.de">Tim Laue</a>
*/ 

#ifndef __RobotHealth_h_
#define __RobotHealth_h_

#include "Tools/Streams/Streamable.h"


/**
* @class MotionRobotHealth
* All information collected within motion process.
*/
class MotionRobotHealth : public Streamable
{
public:
  /**
  * The method makes the object streamable.
  * @param in The stream from which the object is read (if in != 0).
  * @param out The stream to which the object is written (if out != 0).
  */
  virtual void serialize(In* in, Out* out)
  {  
    STREAM_REGISTER_BEGIN();
      STREAM(motionFrameRate);
    STREAM_REGISTER_FINISH();
  }

  /** Constructor */
  MotionRobotHealth(): motionFrameRate(0.0f) {}

  float motionFrameRate;       /*< Frames per second within process "Motion" */
};


/**
* @class RobotHealth
* Full information about the robot.
*/
class RobotHealth : public MotionRobotHealth
{
private:
  /**
  * The method makes the object streamable.
  * @param in The stream from which the object is read (if in != 0).
  * @param out The stream to which the object is written (if out != 0).
  */
  virtual void serialize(In* in, Out* out)
  {  
    STREAM_REGISTER_BEGIN();
      STREAM_BASE(MotionRobotHealth);
      STREAM(cognitionFrameRate);
      STREAM(batteryLevel);
      STREAM(maxTemperature);
      STREAM_ARRAY(load);
      STREAM(memoryLoad);
      STREAM(robotName);
    STREAM_REGISTER_FINISH();
  }

public:
  /** Constructor */
  RobotHealth():cognitionFrameRate(0.0), batteryLevel(0), maxTemperature(0), memoryLoad(0)
  {
    load[0] = load[1] = load[2] = 0;
  }

  /** Assigning MotionRobotHealth 
  * @param motionRobotHealth Information from the motion process
  */
  void operator=(const MotionRobotHealth& motionRobotHealth)
  {
    motionFrameRate = motionRobotHealth.motionFrameRate;
  }

  float cognitionFrameRate;    /*< Frames per second within process "Cognition" */
  float batteryLevel;               /*< Current batteryLevel of robot battery */
  int maxTemperature;        /*< Highest temperature of a robot actuator */
  float load[3]; /*< load averages */
  float memoryLoad;    /*< Percentage of used memory */
  std::string robotName;       /*< For fancier drawing :-) */
};

#endif //__RobotHealth_h_
