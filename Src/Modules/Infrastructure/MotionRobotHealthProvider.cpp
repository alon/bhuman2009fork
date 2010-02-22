/**
* @file Modules/Infrastructure/MotionRobotHealthProvider.h
* This file implements a module that provides information about the robot's health.
* @author <a href="mailto:timlaue@informatik.uni-bremen.de">Tim Laue</a>
*/

#include "MotionRobotHealthProvider.h"


void MotionRobotHealthProvider::update(MotionRobotHealth& motionRobotHealth)
{
  // Compute frame rate of motion process:
  timeBuffer.add(SystemCall::getCurrentSystemTime());
  unsigned timeDiffSum(0);
  int numOfTimeDiffs(0);
  for(int i=0; i < timeBuffer.getNumberOfEntries() - 1; ++i)
  {
    timeDiffSum += (timeBuffer[i] - timeBuffer[i+1]);
    ++numOfTimeDiffs;
  }
  motionRobotHealth.motionFrameRate = timeDiffSum ? 1000.0F / (static_cast<float>(timeDiffSum)/numOfTimeDiffs) : 0.0F;
/*
  // Compute the number of frames from robot which have been received within the last second:
  if(sensorTimeBuffer.getNumberOfEntries() == 0 && theFilteredSensorData.timeStamp != 0)
    sensorTimeBuffer.add(theFilteredSensorData.timeStamp);
  else if(theFilteredSensorData.timeStamp != sensorTimeBuffer[0])
    sensorTimeBuffer.add(theFilteredSensorData.timeStamp);
  if(sensorTimeBuffer.getNumberOfEntries())
  {
    unsigned timeDiff = SystemCall::getCurrentSystemTime() - sensorTimeBuffer[sensorTimeBuffer.getNumberOfEntries()-1];
    motionRobotHealth.robotFramesPerSecond = static_cast<float>(sensorTimeBuffer.getNumberOfEntries()) / timeDiff;
    motionRobotHealth.robotFramesPerSecond *= 1000;
  }
  else
    motionRobotHealth.robotFramesPerSecond = 0.0F;
*/
}

MAKE_MODULE(MotionRobotHealthProvider, Infrastructure)
