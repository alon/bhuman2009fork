/**
* @file Gyroscope.h
* 
* Definition of class Gyroscope
*
* @author <A href="mailto:timlaue@informatik.uni-bremen.de">Tim Laue</A>
*/ 

#ifndef GYROSCOPE_H_
#define GYROSCOPE_H_

#include "Sensor.h"
#include <Tools/SimGraphics.h> //is needed! Why?
#include <PhysicalObjects/Box.h>

/**
* @class Gyroscope
*
* A sensor returning angular velocity around three axes
*/
class Gyroscope : public virtual Sensor
{  
protected:
  PhysicalObject* physObj;     /**< Pointer to the object to which the measurements refer to */
  dBodyID *bodyID;             /**< The ID of the body used for velocity measurement */
  Vector3d valueAngleVel;      /**< The values calculated. */
  SensorReading sensorReading; /**< The sensor information to become returned to the controller */

public:
  /** Empty default Constructor*/
  Gyroscope() {};

  /** Constructor
  * @param attributes The attributes of the sensor
  */
  Gyroscope(const AttributeSet& attributes);

  /** Returns a string describing the kind of object
  * @return The kind
  */
  virtual std::string getKind() const {return "gyroscope";}

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

#endif //GYROSCOPE_H_
