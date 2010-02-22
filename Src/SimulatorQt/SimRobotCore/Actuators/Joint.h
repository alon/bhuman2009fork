/**
 * @file Joint.h
 * 
 * Definition of class Joint
 *
 * @author <A href="mailto:timlaue@informatik.uni-bremen.de">Tim Laue</A>
 */ 

#ifndef JOINT_H_
#define JOINT_H_

#include <ode/ode.h>

#include "Actuator.h"
#include "../Sensors/Sensor.h"
#include "../Tools/SimMath.h"

class Motor;


/**
 * @class AxisDescription
 *
 * A class describing an axis with limits.
 */
class AxisDescription
{
public:
  Vector3d direction;
  double maxValue;
  double minValue;
  bool limited;
  double friction;
  double cfm;
  double fullScaleDeflectionCFM;
  double fullScaleDeflectionERP;
  double suspensionERP;
  double suspensionCFM;
  Motor* motor;

  AxisDescription(const AttributeSet& attributes);
};


/**
* @class Joint
*
* A class representing a joint.
*/
class Joint : public Actuator, public Sensor
{
protected:
  /** A list of motors*/
  std::vector<Motor*> motors;
  /** The current motor to which axes will be added*/
  Motor* currentMotor;
  /** The physical joint object*/
  dJointID physicalJoint;
  /** The axes*/
  std::vector<AxisDescription> axes;

  /**
   *  Parse the attributes of an axis, which are defined in seperate elements.
   *  @param name The name of the attribute element
   *  @param attributes The attributes of the element
   */
  virtual void parseAxis(const std::string& name,
                         const AttributeSet& attributes);

  /**
   *  Parse the attributes of a motor
   *  @param name The name of the attribute element
   *  @param attributes The attributes of the element
   */
  virtual void parseMotor(const std::string& name,
                         const AttributeSet& attributes);

  /** 
   * Adjusts the movable id. The current movable ID is set. A new movable ID is created. 
   * This is used during cloning the simulation object.
   * @return true, because a new movable ID has been created on the stack in the simulation
   */
  virtual bool adjustMovableID();

  /**
   *  Returns the nearest parent physical object. Skips non-physical and dummy objects, such as "Compound"
   *  @return A pointer to the parent physical object
   */
  virtual PhysicalObject* getPhysicalParentObject();

  /**
   *  Returns the first physical child object. Should be the first child object
   *  @param object The object to search below
   *  @param A pointer to the first physical chils object
   */
  virtual PhysicalObject* getPhysicalChildObject(SimObject* object);

public:
  /** Constructor*/
  Joint(): currentMotor(0), physicalJoint(0)
  {}

  /** Destructor*/
  virtual ~Joint();

  /** Returns a string describing the kind of object
  * @return The kind
  */
  virtual std::string getKind() const {return "joint";}

  /** Clones the object and its child nodes
  * @return A pointer to a copy of the object
  */
  virtual SimObject* clone() const;
  
  /** 
   * Casts the SimObject pointer to a Joint pointer. This is used to prevent dynamic downcasts.
   * Has to be overloaded by the appropriate class.
   * @ return A pointer to the physical object.
   */
  Joint* castToJoint(){return this;}
 
  /** 
   * Returns a pointer to the physical joint object.
   * @return A pointer to the physical joint
   */
  virtual dJointID* getPhysicalJoint(){return &physicalJoint;};
  
  /** 
   * Apply friction for non-powered joint axes
   * @param initial Is this the initial step?
   */
  virtual void applyFriction(bool initial){};
};

#endif
