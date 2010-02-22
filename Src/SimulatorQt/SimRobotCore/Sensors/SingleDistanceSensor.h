/**
 * @file SingleDistanceSensor.h
 * 
 * Definition of class SingleDistanceSensor
 *
 * @author <A href="mailto:timlaue@informatik.uni-bremen.de">Tim Laue</A>
 */ 

#ifndef SINGLE_DISTANCE_SENSOR_H_
#define SINGLE_DISTANCE_SENSOR_H_

#include "Sensor.h"


/**
* @class SingleDistanceSensor
*
* A sensor for computing one single distance.
* Based on ODE ray. Only senses physical object.
*/
class SingleDistanceSensor : public virtual Sensor
{
protected:
  /** The data computed by this sensor*/
  SensorReading sensorReading;
  /** The color, which is used to draw the sensor*/
  Vector3d visualizationColor;

  /** Draws the sensor ray
  * @param visParams Parameters for Visualization
  */
  virtual void drawObject(const VisualizationParameterSet& visParams);
  
public:
  /** Constructor*/
  SingleDistanceSensor();

  /** Constructor
  * @param attributes The attributes of the sensor
  */
  SingleDistanceSensor(const AttributeSet& attributes);

  /** Returns a string describing the kind of object
  * @return The kind
  */
  virtual std::string getKind() const {return "single-distance-sensor";}

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
};

#endif //SINGLE_DISTANCE_SENSOR_H_
