/**
* @file Modules/Infrastructure/MotionRobotHealthProvider.h
* This file declares a module that provides information about the robot's health.
* @author <a href="mailto:timlaue@informatik.uni-bremen.de">Tim Laue</a>
*/

#ifndef __MotionRobotHealth_H__
#define __MotionRobotHealth_H__

#include "Tools/Module/Module.h"
#include "Tools/RingBuffer.h"
#include "Representations/Infrastructure/RobotHealth.h"

MODULE(MotionRobotHealthProvider)
  PROVIDES(MotionRobotHealth)
END_MODULE


/**
* @class MotionRobotHealthProvider
* A module that provides information about the robot's health
*/
class MotionRobotHealthProvider : public MotionRobotHealthProviderBase
{
private:
  /** The main function, called every cycle
  * @param motionRobotHealth The data struct to be filled
  */
  void update(MotionRobotHealth& motionRobotHealth);

  RingBuffer<unsigned, 30> timeBuffer;        /** Buffered timestamps of previous executions */
  //RingBuffer<unsigned, 100> sensorTimeBuffer; /** Buffered timestamps of received sensor data */

public:
  /** Constructor. */
  MotionRobotHealthProvider() {}
};

#endif  //__MotionRobotHealthProvider_h_
