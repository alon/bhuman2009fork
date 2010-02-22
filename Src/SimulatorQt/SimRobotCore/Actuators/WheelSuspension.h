/**
 * @file WheelSuspension.h
 * 
 * Definition of class WheelSuspension
 *
 * @author <A href="mailto:kspiess@informatik.uni-bremen.de">Kai Spiess</A>
 */ 

#ifndef WHEELSUSPENSION_H_
#define WHEELSUSPENSION_H_

#include "Joint.h"

/**
* @class WheelSuspension
*
* A class representing a wheel suspension joint.
*/
class WheelSuspension : public Joint
{
public:
  /** Constructor*/
  WheelSuspension();

  /** Constructor
  * @param attributes The attributes of the wheel suspension joint
  */
  WheelSuspension(const AttributeSet& attributes);

  /** Returns a string describing the kind of object
  * @return The kind
  */
  virtual std::string getKind() const {return "wheelsuspension";}

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

  /** Sets the value of one actuatorport
  * @param value The value
  * @param port The port
  */
  virtual void setValue(double value, int port);

  /** Let the actuator do what it is supposed to do
   * @param initial Is this the initial step?
   */
  virtual void act(bool initial);

  /** Computes the position of the joint
  * @param value The value
  * @param portId The id of the sensorport
  */
  virtual void computeValue(double& value, int portId);

  /** 
   * Creats the physical joints used by the OpenDynamicsEngine (ODE).
   * This has to be done after the physical objects (geometry and body)
   * have been created.
   */
  virtual void createJointPhysics();

  /** 
   * Apply friction for non-powered joint axes
   * @param initial Is this the initial step?
   */
  virtual void applyFriction(bool initial);

  /** Clones the object and its child nodes
   * @return A pointer to a copy of the object
   */
  virtual SimObject* clone() const;

  /** 
   * Draws the wheel suspension anchor point and the axes
   * @param visParams Parameters for Visualization
   */
  virtual void drawObject(const VisualizationParameterSet& visParams);

protected:

  enum WheelSuspensionPort{
    AXIS1_JOINT=0,
    AXIS2_JOINT,
    AXIS1_MOTOR,
    AXIS2_MOTOR
  };

  /** The anchor point of this joint*/
  Vector3d& anchorPoint;
  /** A pointer to the first axis of this joint*/
  AxisDescription* axis1;
  /** A pointer to the second axis of this joint*/
  AxisDescription* axis2;
};


#endif
