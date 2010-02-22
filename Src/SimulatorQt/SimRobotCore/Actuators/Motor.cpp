/**
 * @file Motor.cpp
 * 
 * Implementation of abstract class Motor
 *
 * @author <A href="mailto:timlaue@informatik.uni-bremen.de">Tim Laue</A>
 * @author <A href="mailto:kspiess@informatik.uni-bremen.de">Kai Spiess</A>
 */ 

#include "Motor.h"
#include "MotorController.h"
#include "../Simulation/Simulation.h"


MotorAxisDescription::MotorAxisDescription(const AttributeSet& attributes)
{
  minValue = -180.0;
  maxValue = 180.0;

  for(unsigned int i=0; i<attributes.size(); i++)
  {
    std::string attName(attributes[i].attribute);
    if(attName == "x")
    {
      direction.v[0] = ParserUtilities::toDouble(attributes[i].value);
    }
    else if(attName == "y")
    {
      direction.v[1] = ParserUtilities::toDouble(attributes[i].value);
    }
    else if(attName == "z")
    {
      direction.v[2] = ParserUtilities::toDouble(attributes[i].value);
    }
    else if(attName == "maxVelocity")
    {
      maxVelocity = ParserUtilities::toDouble(attributes[i].value);
    }
    else if(attName == "maxForce")
    {
      maxForce = ParserUtilities::toDouble(attributes[i].value);
    }
    else if(attName == "minValue")
    {
      minValue = ParserUtilities::toDouble(attributes[i].value);
    }
    else if(attName == "maxValue")
    {
      maxValue = ParserUtilities::toDouble(attributes[i].value);
    }
  }
}

Motor::Motor(const AttributeSet& attributes): numberOfParsedAxes(0), desiredVelocity(0), desiredValue(0), 
                                              motorType(VELOCITY), maxForce(10.0), maxVelocity(1.0), 
                                              motorControlType(P_CONTROL), motorController(NULL), simulation(0)
{
  for(unsigned int i=0; i<attributes.size(); i++)
  {
    if(attributes[i].attribute == "name")
    {
      name = attributes[i].value;
    }
    else if(attributes[i].attribute == "maxVelocity")
    {
      maxVelocity = ParserUtilities::toDouble(attributes[i].value);
    }
    else if(attributes[i].attribute == "maxForce")
    {
      maxForce = ParserUtilities::toDouble(attributes[i].value);
    }
  }
}

Motor::Motor(): name(""), numberOfParsedAxes(0), physicalMotor(0), desiredVelocity(0), desiredValue(0), 
                motorType(VELOCITY), maxForce(10.0), maxVelocity(1.0), motorControlType(P_CONTROL),
                motorController(NULL)
{
}

Motor::~Motor()
{
  if(motorController)
    delete motorController;
}

void Motor::setValue(double value, int port)
{
  if(motorType == VELOCITY)
  {
    if(value >= maxVelocity)
      desiredVelocity = maxVelocity;
    else
      desiredVelocity = value;
  }
  else if(motorType == ANGULAR)
  {
    desiredValue = value;
  }
}

void Motor::parseAxis(const std::string& name,
                      const AttributeSet& attributes)
{
  MotorAxisDescription newAxis(attributes);
  axes.push_back(newAxis);
  numberOfParsedAxes++;
}

Motor* Motor::clone() const
{
  Motor* newMotor = new Motor();
  newMotor->copyStandardMembers(this);
  if(motorController)
    newMotor->motorController = motorController->clone();
  return newMotor;
}

void Motor::copyStandardMembers(const Motor* other)
{
  name = other->name;
  simulation = other->simulation;
  numberOfParsedAxes = other->numberOfParsedAxes;
  //copy axes
  std::vector<MotorAxisDescription>::const_iterator iter;
  for(iter = other->axes.begin(); iter != other->axes.end(); ++iter)
  {
    axes.push_back(*iter);
  }
  motorType = other->motorType;
  maxForce = other->maxForce;
  maxVelocity = other->maxVelocity;
  motorControlType = other->motorControlType;
}

double Motor::getControllerOutput(const double currentJointValue)
{
  if(motorType == VELOCITY)
  {
    return getDesiredVelocity();
  }
  else // motorType == ANGULAR
  {
    // Correct desired value by clipping to maximum velocity
    double maxValueChange = maxVelocity * simulation->getStepLength();
    if(std::abs(desiredValue - currentJointValue) > maxValueChange)
    {
      if(desiredValue < currentJointValue)
        desiredValue = currentJointValue - maxValueChange;
      else 
        desiredValue = currentJointValue + maxValueChange;
    }
    return motorController->getControllerOutput(currentJointValue, desiredValue); 
  }
}

void Motor::parseController(const std::string& name, const AttributeSet& attributes)
{
  if(motorController)
    delete motorController;

  if(name == "PController")
  {
    motorControlType = P_CONTROL;
    motorController = new PController();
    motorController->parseController(name, attributes);
  }
  else if(name == "PIDController")
  {
    motorControlType = PID_CONTROL;
    motorController = new PIDController();
    motorController->parseController(name, attributes);
  }
}

void Motor::setDefaultController()
{
  if(motorController == 0)
  {
    motorController = new PController();
  }
}

void Motor::setNewParameters(double newMaxForce, double newMaxVelocity, double newP, double newI, double newD)
{
  maxForce = newMaxForce;
  maxVelocity = newMaxVelocity;
  if(motorController)
  {
    if(motorController->getKind() == "pController")
    {
      PController* controller = (PController*)motorController;
      controller->updateControlFactor(newP);
    }
    else if(motorController->getKind() == "pidController")
    {
      PIDController* controller = (PIDController*)motorController;
      controller->updateControlFactors(newP, newI, newD);
    }
  }
}
