/**
 * @file MotorController.h
 *
 * Definition of abstract class MotorController and two
 * derived controller classes.
 *
 * @author <A href="mailto:kspiess@informatik.uni-bremen.de">Kai Spiess</A>
 */

#ifndef MOTORCONTROLLER_H_
#define MOTORCONTROLLER_H_

#include "../Parser/ParserUtilities.h"


/**
* @class MotorController
*
* A class for controlling the speed of angular motors
*/
class MotorController
{
public:
  /**
   * Returns the controller output.
   * @param measuredValue The current value of the joint
   * @param set point The desired value of the joint
   * @return The controller output
   */
  virtual double getControllerOutput(double measuredValue, double setpoint) = 0;

  /** 
   * Clones the motor controller
   * @return A pointer to a copy of the object
   */
  virtual MotorController* clone() const = 0;

  /** 
   * Returns a string describing the kind of motor controller
   * @return The kind
   */
  virtual std::string getKind() = 0;

  /**
   *  Parse the attributes of a motor controller
   *  @param name The name of the attribute element
   *  @param attributes The attributes of the element
   */
  virtual void parseController(const std::string& name, const AttributeSet& attributes) = 0;
};


/**
* @class PController
*
* A class for controlling the speed of angular motors
* using a simple proportional control factor.
*/
class PController : public MotorController
{
private:
  /** The constant factor for a proportional controller (gain) */
  double pControlFactor;
  /** The bias for the state of reached position */
  double stopBias;

public:
  /** Constructor */
  PController();

  /**
   * Returns the controller output.
   * @param measuredValue The current value of the joint
   * @param set point The desired value of the joint
   * @return The controller output
   */
  virtual double getControllerOutput(double measuredValue, double setpoint);

  /** 
   * Clones the motor controller
   * @return A pointer to a copy of the object
   */
  MotorController* clone() const;

  /** 
   * Returns a string describing the kind of motor controller
   * @return The kind
   */
  std::string getKind(){return "pController";};

  /**
   *  Parse the attributes of a motor controller
   *  @param name The name of the attribute element
   *  @param attributes The attributes of the element
   */
  void parseController(const std::string& name, const AttributeSet& attributes);

  /**
  * Updates the P control factor
  * @param p The new p value
  */
  void updateControlFactor(double p) {pControlFactor = p;}
};


/**
* @class PIDController
*
* A class for controlling the speed of angular motors
* combining three different controllers.
*/
class PIDController : public MotorController
{
private:
  /** The constant factor for a proportional control part (gain) */
  double pControlFactor;
  /** The constant factor for an integral control part (reset) */
  double iControlFactor;
  /** The constant factor for a derivative control part (rate) */
  double dControlFactor;
  /** The time step length of a simulation control step*/
  double timeStep;
  /** The sum of errorValues */
  double errorSum;
  /** The error the previous pass */
  double previousError;

public:
  /** Constructor */
  PIDController();

  /**
   * Returns the controller output.
   * @param measuredValue The current value of the joint
   * @param set point The desired value of the joint
   * @return The controller output
   */
  virtual double getControllerOutput(double measuredValue, double setpoint);

  /** 
   * Sets the time step length of a simulation control step
   * @param value the time step length
   */
  void setTimeStep(double value){timeStep = value;};

  /** 
   * Clones the motor controller
   * @return A pointer to a copy of the object
   */
  MotorController* clone() const;

  /** 
   * Returns a string describing the kind of motor controller
   * @return The kind
   */
  std::string getKind(){return "pidController";};

  /**
   *  Parse the attributes of a motor controller
   *  @param name The name of the attribute element
   *  @param attributes The attributes of the element
   */
  void parseController(const std::string& name, const AttributeSet& attributes);
  
  /**
  * Updates the all control factors
  * @param p The new p value
  * @param i The new p value
  * @param d The new p value
  */
  void updateControlFactors(double p, double i, double d) {pControlFactor = p; iControlFactor = i; dControlFactor = d;}
};

#endif //MOTORCONTROLLER_H_
