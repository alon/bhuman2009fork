/**
 * @file Bumper.h
 * 
 * Definition of class Bumper which detects collisions.
 *
 * @author <A href="mailto:Tim.Laue@dfki.de">Tim Laue</A>
 * @author <A href="mailto:kspiess@informatik.uni-bremen.de">Kai Spiess</A>
 */ 

#ifndef BUMPER_H_
#define BUMPER_H_

#include "Sensor.h"


/**
* @class Bumper
*
* A class representing a bumper.
* This sensor does not compute its value. This is realized
* via the simulation main loop which checks for collisions anyway.
*/
class Bumper : public Sensor
{
protected:
  /** The sensor data of the bumper (in fact true/false)*/
  SensorReading sensorReading;
  
public:
  /** Empty default constructor */
  Bumper() {};

  /** Constructor
  * @param attributes The attributes of the bumper
  */
  Bumper(const AttributeSet& attributes);

  /** Returns a string describing the kind of object
  * @return The kind
  */
  virtual std::string getKind() const {return "bumper";}
  
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
  * Returns a pointer to the bumper. This is used to prevent dynamic downcasts from sensor objects to bumper objects   
  * @ return A pointer to the bumper   
  */
  Bumper* castToBumper(){return this;};

  /** Sets the value of the bumper
   *  @param newValue the new value
   */
  void setValue(bool newValue){sensorReading.data.boolValue = newValue;}
};

#endif //BUMPER_H_
