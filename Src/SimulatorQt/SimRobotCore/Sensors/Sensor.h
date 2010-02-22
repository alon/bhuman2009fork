/**
 * @file Sensor.h
 * 
 * Definition of class Sensor
 *
 * @author <A href="mailto:Tim.Laue@dfki.de">Tim Laue</A>
 */ 

#ifndef SENSOR_H_
#define SENSOR_H_

#include "../Simulation/SimObject.h"
#include <cassert>

class Bumper;

/**
* @class Sensor
*
* An abstract class simulating a sensor
* All sensor classes are derivated from Sensor and 
* reimplement the functions needed
*/
class Sensor : public virtual SimObject
{
protected:
  /** The number of step, the last value has been computed*/
  int lastComputationStep;

public:
  /** Constructor*/
  Sensor() {lastComputationStep = -1;}

  /** Returns a string describing the kind of object
  * @return The kind
  */
  virtual std::string getKind() const {return "sensor";}

  /** Returns a reference to the result of the sensor data computation
   * @param localPortId The local sensor port
   * @return The reference
   */
  virtual SensorReading& getSensorReading(int localPortId)
  {
    assert(false); // The base class' function should never be called
    SensorReading* dummySensorReading = new SensorReading;
    return *dummySensorReading;
  }

  /**
   * Returns a pointer to the bumper if the sensor is one. This is used to prevent dynamic downcasts
   * from sensor objects to bumper objects. Has to be overloaded by the appropriate subclass
   * @ return A pointer to the bumper
   */
  virtual Bumper* castToBumper(){return 0;};
};

#endif
