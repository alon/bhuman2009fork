/**
* @file Modules/Infrastructure/RobotHealthProvider.h
* This file declares a module that provides information about the robot's health.
* @author <a href="mailto:timlaue@informatik.uni-bremen.de">Tim Laue</a>
*/

#ifndef __RobotHealth_H__
#define __RobotHealth_H__

#include "Tools/Module/Module.h"
#include "Tools/RingBuffer.h"
#include "Representations/Infrastructure/RobotHealth.h"
#include "Representations/Infrastructure/SensorData.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Configuration/SensorCalibration.h"

MODULE(RobotHealthProvider)
  REQUIRES(MotionRobotHealth)
  REQUIRES(FilteredSensorData)
  REQUIRES(SensorCalibration)
  REQUIRES(FrameInfo)
  PROVIDES(RobotHealth)
END_MODULE


/**
* @class RobotHealthProvider
* A module that provides information about the robot's health
*/
class RobotHealthProvider : public RobotHealthProviderBase
{
private:
  /** The main function, called every cycle
  * @param robotHealth The data struct to be filled
  */
  void update(RobotHealth& robotHealth);

  /** Buffered timestamps of previous executions */
  RingBuffer<unsigned, 30> timeBuffer;    

  unsigned startBatteryLow; /**<Last time the battery state was not low. */
  float lastBatteryLevel;
  bool batteryVoltageFalling;
  unsigned highTemperatureSince;

public:
  /** Constructor. */
  RobotHealthProvider() :
    startBatteryLow(0),
    lastBatteryLevel(1),
    batteryVoltageFalling(false),
    highTemperatureSince(0)
  {}
};

#endif  //__RobotHealthProvider_h_
