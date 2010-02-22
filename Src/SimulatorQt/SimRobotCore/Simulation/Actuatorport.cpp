/**
 * @file Actuatorport.cpp
 * 
 * Implementation of class Actuatorport
 *
 * @author <A href="mailto:timlaue@informatik.uni-bremen.de">Tim Laue</A>
 */

#include "Actuatorport.h"


Actuatorport::Actuatorport(const std::string& name, Actuator* actuator, int portId,
                           double minValue, double maxValue)
{
  this->name = actuator->getFullName() + "." + name;
  this->actuator = actuator;
  this->portId = portId;
  this->minValue = minValue;
  this->maxValue = maxValue;
}

void Actuatorport::setValue(double value)
{
  if(value > maxValue)
  {
    value = maxValue;
  }
  else if(value < minValue)
  {
    value = minValue;
  }
  actuator->setValue(value, portId);
}
