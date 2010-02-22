/**
 * @file Accelerometer.h
 * 
 * Definition of class Accelerometer
 *
 * @author <A href="mailto:timlaue@informatik.uni-bremen.de">Tim Laue</A>
 */ 

#ifndef ACCELEROMETER_H_
#define ACCELEROMETER_H_

#include <ode/common.h>
#include "Sensor.h"


/**
* @class Accelerometer
*
* A sensor returning acceleration in three axes
*/
class Accelerometer : public virtual Sensor
{  
protected:
  Vector3d acceleration;       /**< The acceleration computed by this sensor*/
  Vector3d lastVelocity;       /**< The velocity at the last simulation step*/
  PhysicalObject* physObj;     /**< The body to which the measurements refer to*/
  dBodyID *bodyID;             /**< The ID of the body used for acceleration measurement */
  Matrix3d rotationOffset;     /**< Rotational offset of the sensor */
  bool isRotationOffsetSet;    /**< Simple flag */
  SensorReading sensorReading; /**< Sensor data delivered by this sensor */

public:
  /** Empty default Constructor*/
  Accelerometer() {};

  /** Constructor
  * @param attributes The attributes of the sensor
  */
  Accelerometer(const AttributeSet& attributes);

  /** Returns a string describing the kind of object
  * @return The kind
  */
  virtual std::string getKind() const {return "accelerometer";}

  /** Returns a reference to the result of the sensor data computation
   * @param localPortId The local sensor port
   * @return The reference
   */
  virtual SensorReading& getSensorReading(int localPortId);
  
  /** Adds the object to some internal lists
  * @param sensorPortList A list of all sensor ports in the scene
  * @param actuatorPortList A list of all actuator ports in the scene
  * @param actuatorList A list of all actuators in the scene
  */
  virtual void addToLists(std::vector<SensorPort>& sensorPortList,
                          std::vector<Actuatorport*>& actuatorPortList,
                          std::vector<Actuator*>& actuatorList);
  
  /** Adds descriptions of the object and its children to a list
  * @param objectDescriptionTree The list of descriptions
  * @param The depth of the object in the object tree
  */
  virtual void addToDescriptions(std::vector<ObjectDescription>& objectDescriptionTree,
    int depth);

  /** Clones the object and its child nodes
  * @return A pointer to a copy of the object
  */
  virtual SimObject* clone() const;

  /**
  * Parses the attributes of a simulated object which are
  * defined in seperate elements
  * @param name The name of the attribute element
  * @param attributes The attributes of the element
  * @param line The line in the scene description
  * @param column The column in the scene description
  * @param errorManager An object managing errors
  * @return true, if any element has been parsed
  */
  virtual bool parseAttributeElements(const std::string& name, 
                                      const AttributeSet& attributes,
                                      int line, int column, 
                                      ErrorManager* errorManager);

  /** Do internal computations after parsing.*/
  virtual void postProcessAfterParsing();
};

#endif //ACCELEROMETER_H_
