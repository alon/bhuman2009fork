/**
* @file FallDownStateDetector.cpp
*
* This file implements a module that provides information about the current state of the robot's body.
*
* @author <a href="mailto:timlaue@informatik.uni-bremen.de">Tim Laue</a>
*/

#include "FallDownStateDetector.h"
#include "Representations/Infrastructure/JointData.h"


void FallDownStateDetector::update(FallDownState& fallDownState)
{
  //Buffer data:
  buffers[accX].add(theFilteredSensorData.data[SensorData::accX]);
  buffers[accY].add(theFilteredSensorData.data[SensorData::accY]);
  buffers[accZ].add(theFilteredSensorData.data[SensorData::accZ]);

  //Set default state:
  fallDownState.state = FallDownState::upright;

  //Compute average acceleration values and angles:
  double accXaverage(buffers[accX].getAverage());
  double accYaverage(buffers[accY].getAverage());
  double accZaverage(buffers[accZ].getAverage());
  double accelerationAngleXZ(atan2(accZaverage, accXaverage));
  double accelerationAngleYZ(atan2(accZaverage, accYaverage));
  MODIFY("module:FallDownStateDetector:accX",  accXaverage);
  MODIFY("module:FallDownStateDetector:accY",  accYaverage);
  MODIFY("module:FallDownStateDetector:accZ",  accZaverage);
  MODIFY("module:FallDownStateDetector:accAngleXZ", accelerationAngleXZ);
  MODIFY("module:FallDownStateDetector:accAngleYZ", accelerationAngleYZ);

  //Set state:
  if(sensorAvailable(accXaverage, accYaverage, accZaverage))
  {
    if (fabs(accelerationAngleXZ) < 0.5)
    {
      fallDownState.state = FallDownState::lyingOnFront;
    }
    else if(fabs(accelerationAngleXZ) > 2.5)
    {
      fallDownState.state = FallDownState::lyingOnBack;
    }
    else if (fabs(accelerationAngleYZ) < 0.5)
    {
      fallDownState.state = FallDownState::lyingOnLeftSide;
    }
    else if(fabs(accelerationAngleYZ) > 2.5)
    {
      fallDownState.state = FallDownState::lyingOnRightSide;
    }
  }
}

bool FallDownStateDetector::sensorAvailable(double avgX, double avgY, double avgZ) const
{
  // If the sensor is not available, all values are set to SensorData::off.
  // Therefore, we need to check, if all values have been seet to this value:
  double diff(fabs(avgX - float(SensorData::off)) + fabs(avgY - float(SensorData::off)) + fabs(avgZ - float(SensorData::off)));
  return (diff > 1.0);
}


MAKE_MODULE(FallDownStateDetector, Modeling)
