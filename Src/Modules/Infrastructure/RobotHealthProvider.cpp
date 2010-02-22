/**
* @file Modules/Infrastructure/RobotHealthProvider.h
* This file implements a module that provides information about the robot's health.
* @author <a href="mailto:timlaue@informatik.uni-bremen.de">Tim Laue</a>
*/

#include "RobotHealthProvider.h"
#include "Tools/Settings.h"
#include "Tools/Team.h"
#include "Tools/Debugging/ReleaseOptions.h"
#include "Platform/SoundPlayer.h"

void RobotHealthProvider::update(RobotHealth& robotHealth)
{
  // Transfer information from other process:
  robotHealth = theMotionRobotHealth;

  // Directly transfer temperature and batteryLevel data from SensorData:
  robotHealth.batteryLevel = theFilteredSensorData.data[SensorData::batteryLevel] == SensorData::off ? 1.f : theFilteredSensorData.data[SensorData::batteryLevel];
  unsigned char maxTemperature(0);
  for(int i=0; i<JointData::numOfJoints; ++i)
    if(theFilteredSensorData.temperatures[i] > maxTemperature)
      maxTemperature = theFilteredSensorData.temperatures[i];
  robotHealth.maxTemperature = maxTemperature;

  // Add cpu load, memory load and robot name:
  SystemCall::getLoad(robotHealth.memoryLoad, robotHealth.load);
  robotHealth.robotName = Global::getSettings().robot;
  
  // Compute frame rate of cognition process:
  timeBuffer.add(SystemCall::getCurrentSystemTime());
  unsigned timeDiffSum(0);
  int numOfTimeDiffs(0);
  for(int i=0; i < timeBuffer.getNumberOfEntries() - 1; ++i)
  {
    timeDiffSum += (timeBuffer[i] - timeBuffer[i+1]);
    ++numOfTimeDiffs;
  }
  robotHealth.cognitionFrameRate = timeDiffSum ? 1000.0F / (static_cast<float>(timeDiffSum)/numOfTimeDiffs) : 0.0F;

  // Communicate health:
  if(Global::getReleaseOptions().robotHealth)
  {
    TEAM_OUTPUT(idRobotHealth, bin, robotHealth);
  }

  if(theFilteredSensorData.timeStamp)
  {
    //battery warning
    if(lastBatteryLevel < robotHealth.batteryLevel)
      batteryVoltageFalling = false;
    else if(lastBatteryLevel > robotHealth.batteryLevel)
      batteryVoltageFalling = true;
    if(robotHealth.batteryLevel < theSensorCalibration.batteryLow)
    {
      if(batteryVoltageFalling && theFrameInfo.getTimeSince(startBatteryLow) > 1000)
      {
        SoundPlayer::play("lowbattery.wav");
        //next warning in 90 seconds
        //startBatteryLow = theFrameInfo.time + 90000;
        batteryVoltageFalling = false;
      }
    }
    else if(startBatteryLow < theFrameInfo.time)
      startBatteryLow = theFrameInfo.time;
    lastBatteryLevel = robotHealth.batteryLevel;

    //temperature warning
    if(maxTemperature > theSensorCalibration.temperatureHigh)
    {
      if(theFrameInfo.getTimeSince(highTemperatureSince) > 1000)
      {
        SoundPlayer::play("heat.wav");
        highTemperatureSince = theFrameInfo.time + 20000;
      }
    }
    else if(highTemperatureSince < theFrameInfo.time)
      highTemperatureSince = theFrameInfo.time;
  }
}

MAKE_MODULE(RobotHealthProvider, Infrastructure)
