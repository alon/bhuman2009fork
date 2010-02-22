/**
 * @file Joint.cpp
 * 
 * Implementation of class Joint
 *
 * @author <A href="mailto:timlaue@informatik.uni-bremen.de">Tim Laue</A>
 */ 

#include <cfloat>
#include <Simulation/Actuatorport.h>
#include <Simulation/Simulation.h>
#include "Joint.h"
#include "Motor.h"


AxisDescription::AxisDescription(const AttributeSet& attributes)
{
  //default values
  minValue = -M_PI;
  maxValue = M_PI;
  friction = 0.0;
  limited = false;
  cfm = -1.0;
  fullScaleDeflectionCFM = -1.0;
  fullScaleDeflectionERP = -1.0;
  suspensionCFM = -1.0;
  suspensionERP = -1.0;
  motor = 0;

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
    else if(attName == "friction")
    {
      friction = ParserUtilities::toDouble(attributes[i].value);
    }
    else if(attName == "cfm")
    {
      cfm = ParserUtilities::toDouble(attributes[i].value);
    }
    else if(attName == "fullScaleDeflectionCFM")
    {
      fullScaleDeflectionCFM = ParserUtilities::toDouble(attributes[i].value);
    }
    else if(attName == "fullScaleDeflectionERP")
    {
      fullScaleDeflectionERP = ParserUtilities::toDouble(attributes[i].value);
    }
    else if(attName == "suspensionCFM")
    {
      suspensionCFM = ParserUtilities::toDouble(attributes[i].value);
    }
    else if(attName == "suspensionERP")
    {
      suspensionERP = ParserUtilities::toDouble(attributes[i].value);
    }
  }
}

Joint::~Joint()
{
  for(unsigned int i=0; i<motors.size(); i++)
  {
    delete motors[i];
  }
}

SimObject* Joint::clone() const
{
  Joint* newJoint = new Joint();
  newJoint->copyStandardMembers(this);
  bool createsNewMovID = newJoint->adjustMovableID();
  newJoint->childNodes.clear();

  std::list<SimObject*>::const_iterator pos;
  for(pos = childNodes.begin(); pos != childNodes.end(); ++pos)
  {
    SimObject* childNode = (*pos)->clone();
    newJoint->addChildNode(childNode, false);
  }

  if(createsNewMovID)
    simulation->removeCurrentMovableID();

  SimObject* newObject = newJoint;
  return newObject;
}

void Joint::parseAxis(const std::string& name,
                      const AttributeSet& attributes)
{
  AxisDescription newAxis(attributes);
  newAxis.direction = parentNode->getRotation() * newAxis.direction;
  axes.push_back(newAxis);
}

void Joint::parseMotor(const std::string& name,
                      const AttributeSet& attributes)
{
  if(name == "AngularMotor")
  {
    Motor* newMotor = new Motor(attributes);
    newMotor->setMotorType(ANGULAR);
    newMotor->setDefaultController();
    newMotor->setSimulation(simulation);
    motors.push_back(newMotor);
    currentMotor = newMotor;
    if(!axes.empty())
      axes.back().motor = motors.back();
  }
  else if(name == "VelocityMotor")
  {
    Motor* newMotor = new Motor(attributes);
    newMotor->setMotorType(VELOCITY);
    newMotor->setSimulation(simulation);
    motors.push_back(newMotor);
    currentMotor = newMotor;
    if(!axes.empty())
      axes.back().motor = motors.back();
  }
  else if(name == "Friction")
  {
    if(!axes.empty())
      axes.back().friction = ParserUtilities::toDouble(ParserUtilities::getValueFor(attributes, "value"));
  }
}

bool Joint::adjustMovableID()
{
  movableID = simulation->getCurrentMovableID();
  simulation->createNewMovableID();
  return true;
}

PhysicalObject* Joint::getPhysicalParentObject()
{
  SimObject* result = parentNode;
  while(result != 0 && result->castToPhysicalObject() == 0)
    result = result->getParentNode();
  if(result)
    return result->castToPhysicalObject();
  return 0;
}

PhysicalObject* Joint::getPhysicalChildObject(SimObject* object)
{
  ObjectList* childs = object->getPointerToChildNodes();
  ObjectList::const_iterator pos;
  for(pos = childs->begin(); pos != childs->end(); ++pos)
  {
    if((*pos)->castToPhysicalObject())
      return ((*pos)->castToPhysicalObject());
    else
      return(getPhysicalChildObject((*pos)));
  }
  return 0;
}
