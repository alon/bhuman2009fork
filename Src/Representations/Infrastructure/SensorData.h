/**
* @file Representations/Infrastructure/SensorData.h
*
* This file declares a class to represent the sensor data received from the robot.
*
* @author <A href="mailto:Harlekin@tzi.de">Philippe Schober</A>
*/

#ifndef __SensorData_H__
#define __SensorData_H__

#include "Tools/Streams/Streamable.h"
#include "Representations/Infrastructure/JointData.h"

/**
* @class SensorData
* A class to represent the sensor datas received from the robot.
*/
class SensorData : public Streamable
{
protected:
  virtual void serialize(In* in, Out* out)
  {  
    STREAM_REGISTER_BEGIN();
    STREAM_ARRAY(data);
    STREAM_ARRAY(currents);
    STREAM_ARRAY(temperatures);
    STREAM_ENUM(usSensorType, numOfUsSensorTypes, SensorData::getUsSensorTypeName);
    STREAM(timeStamp);
    STREAM_REGISTER_FINISH();
  }

public:
  enum Sensor
  {
    gyroX,
    gyroY,
    gyroZ,
    accX,
    accY,
    accZ,
    batteryLevel,
    fsrLFL,     //the feetsensors of the Nao-Robot
    fsrLFR,
    fsrLBL,
    fsrLBR,
    fsrRFL,
    fsrRFR,
    fsrRBL,
    fsrRBR,
    us,
    angleX,
    angleY,
    numOfSensors
  };

  enum
  {
    off = JointData::off /**< A special value to indicate that the sensor is missing. */
  };

  enum UsSensorTypes
  {
    left,
    leftToRight,
    rightToLeft,
    right,
    numOfUsSensorTypes
  };

  float data[numOfSensors]; /**< The data of all sensors. */
  UsSensorTypes usSensorType; /**< The ultrasonice measure method which was used for measuring \c data[us]. */
  short currents[JointData::numOfJoints]; /**< The currents of all motors. */  
  unsigned char temperatures[JointData::numOfJoints]; /**< The temperature of all motors. */  
  unsigned timeStamp; /**< The time when the sensor data was received. */

  /**
  * Default constructor.
  */
  SensorData() : usSensorType(leftToRight), timeStamp(0)
  {
    for(int i = 0; i < numOfSensors; ++i)
      data[i] = off;
    for(int i = 0; i < JointData::numOfJoints; ++i)
      currents[i] = temperatures[i] = 0;
  }

  /**
  * The function returns the name of a sensor.
  * @param sensor The sensor the name of which will be returned.
  * @return The name.
  */
  static const char* getSensorName(Sensor sensor)
  {
    switch(sensor)
    {
    case gyroX: return "gyroX";
    case gyroY: return "gyroY";
    case gyroZ: return "gyroZ";
    case accY: return "accY";
    case accZ: return "accZ";
    case accX: return "accX";
    case batteryLevel: return "batteryLevel";
    case fsrLFL: return "fsrLFL";
    case fsrLFR: return "fsrLFR";
    case fsrLBL: return "fsrLBL";
    case fsrLBR: return "fsrLBR";
    case fsrRFL: return "fsrRFL";
    case fsrRFR: return "fsrRFR";
    case fsrRBL: return "fsrRBL";
    case fsrRBR: return "fsrRBR";
    case us: return "us";
    case angleX: return "angleX";
    case angleY: return "angleY";
    default: return "unknown";
    }
  }

  /**
  * The function returns the name of an ultrasonice measure method.
  * @param type The measure method the name of which will be returned.
  * @return The name.
  */
  static const char* getUsSensorTypeName(UsSensorTypes type)
  {
    switch(type)
    {
    case left: return "left";
    case leftToRight: return "leftToRight";
    case rightToLeft: return "rightToLeft";
    case right: return "right";
    default: return "unknown";
    }
  }
};

/**
* A class to represent filtered sensor data.
*/
class FilteredSensorData : public SensorData {};

#endif // __SensorData_H__
