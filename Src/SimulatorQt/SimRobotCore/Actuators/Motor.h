/**
 * @file Motor.h
 * 
 * Definition of abstract class Motor
 *
 * @author <A href="mailto:timlaue@informatik.uni-bremen.de">Tim Laue</A>
 * @author <A href="mailto:kspiess@informatik.uni-bremen.de">Kai Spiess</A>
 */ 

#ifndef MOTOR_H_
#define MOTOR_H_

#include <ode/ode.h>
#include "../Parser/ParserUtilities.h"

class Simulation;
class MotorController;


/**
* @class MotorAxisDescription
*
* A class describing the axis of a motor
*/
class MotorAxisDescription
{
public:
  Vector3d direction;
  double maxVelocity;
  double maxForce;
  double minValue;
  double maxValue;

  MotorAxisDescription(const AttributeSet& attributes);
};

/** The two types of motors*/
enum MotorType
{
  VELOCITY,
  ANGULAR
};

/** The types of motor control*/
enum MotorControlType
{
  P_CONTROL,
  PID_CONTROL
};

/**
* @class Motor
*
* An abstract class representing a motor
*/
class Motor
{
public:
  /** Constructor*/
  Motor(const AttributeSet& attributes);
  
  /** Constructor*/
  Motor();

  /** Destructor*/
  ~Motor();

  /** Returns a string describing the kind of object
  * @return The kind
  */
  std::string getKind() const {return "motor";}

  /** Sets the value of one actuatorport
  * @param value The value
  * @param port The port
  */
  void setValue(double value, int port);

  /**
   *  Parse the attributes of an axis, which are defined in seperate elements.
   *  @param name The name of the attribute element
   *  @param attributes The attributes of the element
   */
  void parseAxis(const std::string& name,
                 const AttributeSet& attributes);

  /** Sets the pointer to the Simulation core
  * @param simulation The address of the simulation object
  */
  void setSimulation(Simulation* sim) {simulation = sim;}

  /** Clones the motor
   *  @return A pointer to a copy of the motor
   */
  Motor* clone() const;

  /** Copies the members of another motor
  * @param other The other motor
  */
  void copyStandardMembers(const Motor* other);

  /** 
   * Sets the physical motor object
   * @param physMotor The physical motor
   */
  void setPhysicalMotor(dJointID physMotor){physicalMotor = physMotor;};

  /**
   * Returns a pointer to the physical motor object
   * @return A pointer to the physical motor object
   */
  dJointID* getPhysicalMotor(){return &physicalMotor;};

  /**
   *  Returns the desired velocity of the motor
   *  @return The desired velocity
   */
  double getDesiredVelocity() const {return desiredVelocity;};

  /**
   *  Returns the type of the motor (velocity or angular)
   *  @return The motor type
   */
  MotorType getMotorType() const {return motorType;};

  /**
   *  Sets the type of the motor (velocity or angular)
   *  @param newMotorType The motor type
   */
  void setMotorType(MotorType newMotorType){motorType = newMotorType;};

  /**
   *  Returns the number of parsed axes for the motor
   *  @return The number of parsed axes
   *
   */
  unsigned short getNumberOfParsedAxes(){return numberOfParsedAxes;};

  /**
   *  Returns a pointer to the motor axis description of a motor axis
   *  @param index The number of the desired axis
   *  @return The pointer to the motor axis description
   */
  MotorAxisDescription* getAxis(int index){return &axes[index];};

  /**
   *  Returns the maximum force of the motor
   *  @return The maximum force
   */
  double getMaxForce() const {return maxForce;};

  /**
   *  Returns the maximum velocity of the motor
   *  @return The maximum velocity
   */
  double getMaxVelocity() const {return maxVelocity;};

  /**
   *  Returns the control value for the motor depending on the used controler type.
   *  @param currentJointValue the current (angular/linear) value (rotation/position) of the joint this motor belongs to
   *  @return the control value of the motor
   */
  double getControllerOutput(const double currentJointValue);

  /**
   *  Parse the attributes of a motor controller
   *  @param name The name of the attribute element
   *  @param attributes The attributes of the element
   */
  void parseController(const std::string& name, const AttributeSet& attributes);

  /**
   * Returns a pointer to the motor controller
   * @return The motor controller
   */
  MotorController* getMotorController() {return motorController;}

  /** Sets a default motor controller (P controller)*/
  void setDefaultController();

  /**
  * Only parameter update (e.g. for optimization experiments)
  * @param newMaxForce Maximum motor force
  * @param newMaxVelocity Maximum motor speed
  * @param newP P control value
  * @param newI I control value (not used for P controller, of course)
  * @param newD D control value (not used for P controller, of course)
  */
  void setNewParameters(double newMaxForce, double newMaxVelocity, double newP, double newI, double newD);

protected:
  /** The name of the motor*/
  std::string name;
  /** The number of parsed axes*/
  unsigned short numberOfParsedAxes;
  /** The axes*/
  std::vector<MotorAxisDescription> axes;
  /** The physical motor object*/
  dJointID physicalMotor;
  /** The desired velocity of the motor.*/
  double desiredVelocity;
  /** The desired value of the motor axis.*/
  double desiredValue;
  /** The type of the motor.*/
  MotorType motorType;
  /** The maximum force of the motor*/
  double maxForce;
  /** The maximum velocity of the motor*/
  double maxVelocity;
  /** The motor control type*/
  MotorControlType motorControlType;
  /** The motor controller*/
  MotorController* motorController;
  /** A pointer to the simulation core (motor needs information at runtime)*/
  Simulation* simulation;
};

#endif //MOTOR_H_
