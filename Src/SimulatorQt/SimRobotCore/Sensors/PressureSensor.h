/**
 * @file PressureSensor.h
 * 
 * Definition of class PressureSensor
 *
 * @author Jeff
 */ 

#ifndef PressureSensor_H_ 
#define PressureSensor_H_

#include <Tools/SimMath.h>
#include "../Actuators/Joint.h"

/**
* @class PressureSensor
*
* A class representing a slider joint.
*/
class PressureSensor : public Joint
{
public:
  /** Constructor*/
   PressureSensor();
  /** Constructor
  * @param attributes The attributes of the slider joint
  */
  PressureSensor(const AttributeSet& attributes);

  /** Returns a string describing the kind of object
  * @return The kind
  */
  virtual std::string getKind() const {return "PressureSensor";}

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
  virtual void addToDescriptions(std::vector<ObjectDescription>& objectDescriptionTree, int depth);
  
  /** Returns a reference to the result of the sensor data computation
   * @param localPortId The local sensor port
   * @return The reference
   */
  virtual SensorReading& getSensorReading(int localPortId); 

  /** 
   * Creats the physical joints used by the OpenDynamicsEngine (ODE).
   * This has to be done after the physical objects (geometry and body)
   * have been created.
   */
  virtual void createJointPhysics();
  /** Clones the object and its child nodes
   * @return A pointer to a copy of the object
   */
  virtual SimObject* clone() const;
  /** 
   * Draws the slider axis
   * @param visParams Parameters for Visualization
   */
  virtual void drawObject(const VisualizationParameterSet& visParams);

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

protected:
  SensorReading sensorReading; /**< The sensor information to become returned to the controller */
  dJointFeedback *feedback;  /**< Pointer to the feedback from the joint */
  Vector3d valuePressure;    /**< The computed value of the sensor */
  PhysicalObject* physObj;   /**< Pointer to the object to which the measurements refer to */
  dBodyID *bodyID;           /**< The ID of the body used for velocity measurement */
  Vector3d gain,             /**< The gain for each axis */
           offset;           /**< The offset for axis */ 
};

#endif
