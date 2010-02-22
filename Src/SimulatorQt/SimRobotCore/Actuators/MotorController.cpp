/**
 * @file MotorController.h
 *
 * Implementation of classes for motor control
 *
 * @author <A href="mailto:kspiess@informatik.uni-bremen.de">Kai Spiess</A>
 */

#include "MotorController.h"
#include <Tools/SimMath.h>


//************************
// P - C O N T R O L L E R
//************************

PController::PController() : pControlFactor(1.0), stopBias(0.0)
{
}

MotorController* PController::clone() const
{
  PController* newController = new PController();
  newController->pControlFactor = pControlFactor;
  newController->stopBias = stopBias;
  return newController;
}

void PController::parseController(const std::string& name, const AttributeSet& attributes)
{
  for(unsigned int i=0; i<attributes.size(); i++)
  {
    std::string attributeName(attributes[i].attribute);
    if(attributeName == "pControlFactor")
    {
      pControlFactor = ParserUtilities::toDouble(attributes[i].value);
    }
    else if(attributeName == "stopBias")
    {
      stopBias = Functions::toRad(ParserUtilities::toDouble(attributes[i].value));
    }
  }
}

double PController::getControllerOutput(double measuredValue, double setpoint)
{
  double error = setpoint - measuredValue;
  if(fabs(error) < stopBias)
    return 0.0;
  return error * pControlFactor;
}


//****************************
// P I D - C O N T R O L L E R
//****************************

PIDController::PIDController() : pControlFactor(1.0), iControlFactor(1.0),dControlFactor(1.0), timeStep(1.0),
  errorSum (0.0), previousError (0.0)
{
}

MotorController* PIDController::clone() const
{
  PIDController* newController = new PIDController();
  newController->pControlFactor = pControlFactor;
  newController->iControlFactor = iControlFactor;
  newController->dControlFactor = dControlFactor;
  newController->timeStep = timeStep;
  return newController;
}

void PIDController::parseController(const std::string& name, const AttributeSet& attributes)
{
  for(unsigned int i=0; i<attributes.size(); i++)
  {
    std::string attributeName(attributes[i].attribute);
    if(attributeName == "pControlFactor")
    {
      pControlFactor = ParserUtilities::toDouble(attributes[i].value);
    }
    else if(attributeName == "iControlFactor")
    {
      iControlFactor = ParserUtilities::toDouble(attributes[i].value);
    }
    else if(attributeName == "dControlFactor")
    {
      dControlFactor = ParserUtilities::toDouble(attributes[i].value);
    }
  }
}

double PIDController::getControllerOutput(double measuredValue, double setpoint)
{
  double error = setpoint - measuredValue;
  double pControlValue = pControlFactor * error;
  errorSum += error;
  double iControlValue = iControlFactor * timeStep * errorSum;
  double dControlValue = (dControlFactor * (error - previousError)) / timeStep;
  previousError = error;
  return pControlValue + iControlValue + dControlValue;
}
