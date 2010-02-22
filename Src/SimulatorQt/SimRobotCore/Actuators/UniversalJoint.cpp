/**
 * @file UniversalJoint.cpp
 * 
 * Implementation of class UniversalJoint
 *
 * @author <A href="mailto:kspiess@informatik.uni-bremen.de">Kai Spiess</A>
 */ 

#include "UniversalJoint.h"
#include "Motor.h"
#include "MotorController.h"
#include "../Simulation/Actuatorport.h"
#include "../Simulation/Simulation.h"
#include "../PhysicalObjects/PhysicalObject.h"
#include "../Simulation/PhysicsParameterSet.h"
#include "../Tools/SimGraphics.h"


UniversalJoint::UniversalJoint() : anchorPoint(position), axis1(0), axis2(0)
{
}

UniversalJoint::UniversalJoint(const AttributeSet& attributes) : anchorPoint(position), axis1(0), axis2(0)
{
  name = ParserUtilities::getValueFor(attributes, "name");
}

bool UniversalJoint::parseAttributeElements(const std::string& name, 
                                       const AttributeSet& attributes,
                                       int line, int column,
                                       ErrorManager* errorManager)
{
  if(name == "AnchorPoint")
  {
    anchorPoint = parentNode->getPosition();
    anchorPoint += parentNode->getRotation() * (ParserUtilities::toPoint(attributes));
  }
  else if(name == "Axis1" || name == "Axis2")
  {
    parseAxis(name, attributes);
    if(name == "Axis1")
    {
      axis1 = &axes[0];
    }
    else if(name == "Axis2")
    {
      axis1 = &axes[0];
      axis2 = &axes[1];
    }
  }
  else if(name == "AngularMotor")
  {
    parseMotor(name, attributes);
  }
  else if(name == "VelocityMotor")
  {
    parseMotor(name, attributes);
  }
  else if(name == "Deflection")
  {
    axes.back().minValue = (ParserUtilities::toDouble(ParserUtilities::getValueFor(attributes, "min")));
    axes.back().maxValue = (ParserUtilities::toDouble(ParserUtilities::getValueFor(attributes, "max")));
    axes.back().limited = true;
  }
  else if(name == "PController")
  {
    axes.back().motor->parseController(name, attributes);
  }
  else if(name == "PIDController")
  {
    axes.back().motor->parseController(name, attributes);
    ((PIDController*)axes.back().motor->getMotorController())->setTimeStep(simulation->getStepLength());
  }
  else if(name == "Friction")
  {
    if(!axes.empty())
      axes.back().friction = ParserUtilities::toDouble(ParserUtilities::getValueFor(attributes, "value"));
  }
  return true;
}

SimObject* UniversalJoint::clone() const
{
  UniversalJoint* newUniversalJoint = new UniversalJoint();
  //copy SimObject members
  newUniversalJoint->copyStandardMembers(this);
  bool createsNewMovID = newUniversalJoint->adjustMovableID();
  //copy joint members
    //axes
  std::vector<AxisDescription>::const_iterator axesIter;
  for(axesIter = axes.begin(); axesIter != axes.end(); ++axesIter)
  {
    newUniversalJoint->axes.push_back(*axesIter);
    if(axesIter->motor != NULL)
    {
      newUniversalJoint->motors.push_back(axesIter->motor->clone());
      newUniversalJoint->axes.back().motor = newUniversalJoint->motors.back();
    }
  }
    //currentMotor
  if(!newUniversalJoint->motors.empty())
  {
    newUniversalJoint->currentMotor = newUniversalJoint->motors.back();
  }
  //copy universal joint members
  newUniversalJoint->anchorPoint = anchorPoint;
  newUniversalJoint->axis1 = &(newUniversalJoint->axes[0]);
  newUniversalJoint->axis2 = &(newUniversalJoint->axes[1]);

  //copy child nodes
  std::list<SimObject*>::const_iterator pos;
  for(pos = childNodes.begin(); pos != childNodes.end(); ++pos)
  {
    SimObject* childNode = (*pos)->clone();
    newUniversalJoint->addChildNode(childNode, false);
  }
  if(createsNewMovID)
  {
    simulation->removeCurrentMovableID();
  }
  SimObject* newObject = newUniversalJoint;
  return newObject;
}

void UniversalJoint::addToLists(std::vector<SensorPort>& sensorPortList,
                       std::vector<Actuatorport*>& actuatorportList,
                       std::vector<Actuator*>& actuatorList) 
{
  if(axis1 != 0 && axis2 != 0)
  {
    sensorPortList.push_back(SensorPort(fullName+".axis1Angle",AXIS1_JOINT,this));
    sensorPortList.push_back(SensorPort(fullName+".axis2Angle",AXIS2_JOINT,this));

    // Motor 1
    if(axis1->motor)
    {
      std::string actuatorDescription;
      std::string sensorDescription;
      double minV=0.0, maxV=0.0;
      if(axis1->motor->getMotorType() == VELOCITY)
      {
        actuatorDescription = "motorAxis1Velocity";
        sensorDescription = "motorAxis1VelocitySensor";
        if(axis1->limited)
        {
          minV = axis1->minValue;
          maxV = axis1->maxValue;
        }
        else
        {
          minV = -(axis1->motor->getMaxVelocity());
          maxV = axis1->motor->getMaxVelocity();
        }
      }
      else if(axis1->motor->getMotorType() == ANGULAR)
      {
        actuatorDescription = "motorAxis1Angle";
        sensorDescription = "motorAxis1AngleSensor";
        if(axis1->limited)
        {
          minV = axis1->minValue;
          maxV = axis1->maxValue;
        }
        else
        {
          minV = -180.0;
          maxV = 180.0;
        }
      }
      Actuatorport* value = new Actuatorport
        (actuatorDescription, this, AXIS1_MOTOR, minV, maxV);
      actuatorportList.push_back(value);
      sensorPortList.push_back(SensorPort(fullName+"."+sensorDescription,AXIS1_MOTOR,this));
    }

    // Motor 2
    if(axis2->motor)
    {
      std::string actuatorDescription;
      std::string sensorDescription;
      double minV=0.0, maxV=0.0;
      if(axis2->motor->getMotorType() == VELOCITY)
      {
        actuatorDescription = "motorAxis2Velocity";
        sensorDescription = "motorAxis2VelocitySensor";
        if(axis2->limited)
        {
          minV = axis2->minValue;
          maxV = axis2->maxValue;
        }
        else
        {
          minV = -(axis2->motor->getMaxVelocity());
          maxV = axis2->motor->getMaxVelocity();
        }
      }
      else if(axis2->motor->getMotorType() == ANGULAR)
      {
        actuatorDescription = "motorAxis2Angle";
        sensorDescription = "motorAxis2AngleSensor";
        if(axis2->limited)
        {
          minV = axis2->minValue;
          maxV = axis2->maxValue;
        }
        else
        {
          minV = -180.0;
          maxV = 180.0;
        }
      }
      Actuatorport* value = new Actuatorport
        (actuatorDescription, this, AXIS2_MOTOR, minV, maxV);
      actuatorportList.push_back(value);
      sensorPortList.push_back(SensorPort(fullName+"."+sensorDescription,AXIS2_MOTOR,this));
    }

    // Add to list of actuators
    if(axis1->motor != 0 || axis2->motor != 0)
    {
      actuatorList.push_back(this);
    }
  }
}

void UniversalJoint::addToDescriptions(std::vector<ObjectDescription>& objectDescriptionTree, int depth) 
{
  Sensor::addToDescriptions(objectDescriptionTree, depth);  
  //Add Sensorports:
  ObjectDescription valueDesc;
  valueDesc.name = "axis1Angle";
  valueDesc.fullName = fullName + ".axis1Angle";
  valueDesc.depth = depth + 1;
  valueDesc.type = OBJECT_TYPE_SENSORPORT;
  objectDescriptionTree.push_back(valueDesc);
  valueDesc.name = "axis2Angle";
  valueDesc.fullName = fullName + ".axis2Angle";
  valueDesc.depth = depth + 1;
  valueDesc.type = OBJECT_TYPE_SENSORPORT;
  objectDescriptionTree.push_back(valueDesc);

  if(axis1->motor)
  {
    std::string actuatorDescription;
    std::string sensorDescription;
    if(axis1->motor->getMotorType() == VELOCITY)
    {
      actuatorDescription = "motorAxis1Velocity";
      sensorDescription = "motorAxis1VelocitySensor";
    }
    else if(axis1->motor->getMotorType() == ANGULAR)
    {
      actuatorDescription = "motorAxis1Angle";
      sensorDescription = "motorAxis1AngleSensor";
    }
    //Add Actuatorport:
    valueDesc.name = actuatorDescription;
    valueDesc.fullName = fullName + "." + actuatorDescription;
    valueDesc.depth = depth + 1;
    valueDesc.type = OBJECT_TYPE_ACTUATORPORT;
    objectDescriptionTree.push_back(valueDesc);
    //Add Sensorport:
    valueDesc.name = sensorDescription;
    valueDesc.fullName = fullName + "." + sensorDescription;
    valueDesc.depth = depth + 1;
    valueDesc.type = OBJECT_TYPE_SENSORPORT;
    objectDescriptionTree.push_back(valueDesc);
  }

  if(axis2->motor)
  {
    std::string actuatorDescription;
    std::string sensorDescription;
    if(axis2->motor->getMotorType() == VELOCITY)
    {
      actuatorDescription = "motorAxis2Velocity";
      sensorDescription = "motorAxis2VelocitySensor";
    }
    else if(axis2->motor->getMotorType() == ANGULAR)
    {
      actuatorDescription = "motorAxis2Angle";
      sensorDescription = "motorAxis2AngleSensor";
    }
    //Add Actuatorport:
    valueDesc.name = actuatorDescription;
    valueDesc.fullName = fullName + "." + actuatorDescription;
    valueDesc.depth = depth + 1;
    valueDesc.type = OBJECT_TYPE_ACTUATORPORT;
    objectDescriptionTree.push_back(valueDesc);
    //Add Sensorport:
    valueDesc.name = sensorDescription;
    valueDesc.fullName = fullName + "." + sensorDescription;
    valueDesc.depth = depth + 1;
    valueDesc.type = OBJECT_TYPE_SENSORPORT;
    objectDescriptionTree.push_back(valueDesc);
  }
}

void UniversalJoint::setValue(double value, int port)
{
  /* TEMPORARY FIX */
  if(port == AXIS1_MOTOR)
  {
    double newValue = value;
    if(axis1->motor->getMotorType() == ANGULAR)
    {
      if(value == axis1->maxValue || value == axis1->minValue)
      {
        if(value == 0.0 && axis1->minValue == 0.0)
        {
          newValue = -0.01;
        }
        else if(value == 0.0 && axis1->maxValue == 0.0)
        {
          newValue = 0.01;
        }
        else
        {
          newValue -= (value *dReal(0.01));
        }
      }
      newValue = Functions::toRad(newValue);
    }
    axis1->motor->setValue(newValue, port);
  }
  else if(port == AXIS2_MOTOR)
  {
    double newValue = value;
    if(axis2->motor->getMotorType() == ANGULAR)
    {
      if(value == axis2->maxValue || value == axis2->minValue)
      {
        if(value == 0.0 && axis2->minValue == 0.0)
        {
          newValue = -0.01;
        }
        else if(value == 0.0 && axis2->maxValue == 0.0)
        {
          newValue = 0.01;
        }
        else
        {
          newValue -= (value *dReal(0.01));
        }
      }
      newValue = Functions::toRad(newValue);
    }
    axis2->motor->setValue(newValue, port);
  }
}

void UniversalJoint::act(bool initial)
{
  if(!initial)
  {
    if(axis1->motor)
    {
      if(axis1->motor->getMotorType() == VELOCITY)
      {
        dReal desiredVelocity(dReal(axis1->motor->getDesiredVelocity()));
        dJointSetUniversalParam(physicalJoint, dParamFMax, dReal(axis1->motor->getMaxForce()));
        dJointSetUniversalParam(physicalJoint, dParamVel, desiredVelocity);
      }
      else if(axis1->motor->getMotorType() == ANGULAR)
      {
        double controllerOutput = axis1->motor->getControllerOutput(dJointGetUniversalAngle1(physicalJoint));
        dJointSetUniversalParam(physicalJoint, dParamFMax, dReal(axis1->motor->getMaxForce()));
        dJointSetUniversalParam(physicalJoint, dParamVel, dReal(controllerOutput));
      }
    }

    if(axis2->motor)
    {
      if(axis2->motor->getMotorType() == VELOCITY)
      {
        dReal desiredVelocity(dReal(axis2->motor->getDesiredVelocity()));
        dJointSetUniversalParam(physicalJoint, dParamFMax2, dReal(axis2->motor->getMaxForce()));
        dJointSetUniversalParam(physicalJoint, dParamVel2, desiredVelocity);
      }
      else if(axis2->motor->getMotorType() == ANGULAR)
      {
        double controllerOutput = axis2->motor->getControllerOutput(dJointGetUniversalAngle2(physicalJoint));
        dJointSetUniversalParam(physicalJoint, dParamFMax2, dReal(axis2->motor->getMaxForce()));
        dJointSetUniversalParam(physicalJoint, dParamVel2, dReal(controllerOutput));
      }
    }
  }
}

void UniversalJoint::computeValue(double& value, int portId)
{
  if(portId == AXIS1_JOINT)
  {
    value = (dJointGetUniversalAngle1(physicalJoint) * 180.0 / M_PI);
  }
  else if(portId == AXIS1_MOTOR)
  {
    if(axis1->motor->getMotorType() == VELOCITY)
      value = (dJointGetUniversalParam(physicalJoint, dParamVel));
    else
      value = (dJointGetUniversalAngle1(physicalJoint) * 180.0 / M_PI);
  }
  else if(portId == AXIS2_JOINT)
  {
    value = (dJointGetUniversalAngle2(physicalJoint) * 180.0 / M_PI);
  }
  else if(portId == AXIS2_MOTOR)
  {
    if(axis2->motor->getMotorType() == VELOCITY)
      value = (dJointGetUniversalParam(physicalJoint, dParamVel2));
    else
      value = (dJointGetUniversalAngle2(physicalJoint) * 180.0 / M_PI);
  }
}

void UniversalJoint::createJointPhysics()
{
  physicalJoint = dJointCreateUniversal(*simulation->getPhysicalWorld(), 0);
  PhysicalObject* o1 = getPhysicalParentObject();
  PhysicalObject* o2 = getPhysicalChildObject(this);
  assert (o1 && o2);

  dJointAttach(physicalJoint, *(o2->getBody()), *(o1->getBody()));

  //set joint parameter
  dJointSetUniversalAnchor(physicalJoint, dReal(anchorPoint.v[0]), dReal(anchorPoint.v[1]), dReal(anchorPoint.v[2]));
  Vector3d physAxis(axis1->direction);
  physAxis.rotate(rotation);
  dJointSetUniversalAxis1(physicalJoint, dReal(physAxis.v[0]), dReal(physAxis.v[1]), dReal(physAxis.v[2]));
  physAxis = axis2->direction;
  physAxis.rotate(rotation);
  dJointSetUniversalAxis2(physicalJoint, dReal(physAxis.v[0]), dReal(physAxis.v[1]), dReal(physAxis.v[2]));

  if(axis1->cfm != -1.0)
    dJointSetUniversalParam(physicalJoint, dParamCFM, dReal(axis1->cfm));
  if(axis2->cfm != -1.0)
    dJointSetUniversalParam(physicalJoint, dParamCFM2, dReal(axis2->cfm));

  if(axis1->limited || (axis1->motor != 0 && axis1->motor->getMotorType() == ANGULAR))
  {
    double minAxisLimit(Functions::toRad(axis1->minValue));
    double maxAxisLimit(Functions::toRad(axis1->maxValue));
    //Set physical limits to higher values (+10%) to avoid strange hinge effects.
    //Otherwise, sometimes the motor exceeds the limits.
    assert(maxAxisLimit >= minAxisLimit);
    double totalAxisRange(maxAxisLimit - minAxisLimit);
    double internalTolerance(totalAxisRange / 10);
    minAxisLimit -= internalTolerance;
    maxAxisLimit += internalTolerance;
    dJointSetUniversalParam(physicalJoint, dParamLoStop, dReal(minAxisLimit));
    dJointSetUniversalParam(physicalJoint, dParamHiStop, dReal(maxAxisLimit));
    // this has to be done due to the way ODE sets joint stops
    dJointSetUniversalParam(physicalJoint, dParamLoStop, dReal(minAxisLimit));
    if(axis1->fullScaleDeflectionCFM != -1.0)
      dJointSetUniversalParam(physicalJoint, dParamStopCFM, dReal(axis1->fullScaleDeflectionCFM));
    if(axis1->fullScaleDeflectionERP != -1.0)
      dJointSetUniversalParam(physicalJoint, dParamStopERP, dReal(axis1->fullScaleDeflectionERP));
  }
  if(axis2->limited || (axis2->motor != 0 && axis2->motor->getMotorType() == ANGULAR))
  {
    double minAxisLimit(Functions::toRad(axis2->minValue));
    double maxAxisLimit(Functions::toRad(axis2->maxValue));
    //Set physical limits to higher values (+10%) to avoid strange hinge effects.
    //Otherwise, sometimes the motor exceeds the limits.
    assert(maxAxisLimit >= minAxisLimit);
    double totalAxisRange(maxAxisLimit - minAxisLimit);
    double internalTolerance(totalAxisRange / 10);
    minAxisLimit -= internalTolerance;
    maxAxisLimit += internalTolerance;
    dJointSetUniversalParam(physicalJoint, dParamLoStop2, dReal(minAxisLimit));
    dJointSetUniversalParam(physicalJoint, dParamHiStop2, dReal(maxAxisLimit));
    // this has to be done due to the way ODE sets joint stops
    dJointSetUniversalParam(physicalJoint, dParamLoStop2, dReal(minAxisLimit));
    if(axis2->fullScaleDeflectionCFM != -1.0)
      dJointSetUniversalParam(physicalJoint, dParamStopCFM2, dReal(axis2->fullScaleDeflectionCFM));
    if(axis2->fullScaleDeflectionERP != -1.0)
      dJointSetUniversalParam(physicalJoint, dParamStopERP2, dReal(axis2->fullScaleDeflectionERP));
  }
}

void UniversalJoint::applyFriction(bool initial)
{
  if(!initial)
  {
    double velocity = 0.0;
    double gravityFactor = fabs((simulation->getPhysicsParameters())->getGravity());
    double fmax;
    if(axis1->motor == 0)
    {
      fmax = axis1->friction * gravityFactor;
      dJointSetUniversalParam(physicalJoint, dParamFMax, dReal(fmax));
      dJointSetUniversalParam(physicalJoint, dParamVel, dReal(velocity));
    }
    if(axis2->motor == 0)
    {
      fmax = axis2->friction * gravityFactor;
      dJointSetUniversalParam(physicalJoint, dParamFMax2, dReal(fmax));
      dJointSetUniversalParam(physicalJoint, dParamVel2, dReal(velocity));
    }
  }
}

void UniversalJoint::drawObject(const VisualizationParameterSet& visParams)
{
  if(visParams.surfaceStyle == VisualizationParameterSet::PHYSICS_WIREFRAME ||
     visParams.surfaceStyle == VisualizationParameterSet::PHYSICS_FLAT ||
     visParams.surfaceStyle == VisualizationParameterSet::PHYSICS_SMOOTH ||
     visParams.surfaceStyle == VisualizationParameterSet::MIXED_APPEARANCE ||
     visParams.surfaceStyle == VisualizationParameterSet::MIXED_PHYSICS)
  {
    dVector3 tempA;
    dJointGetUniversalAnchor(physicalJoint, tempA);
    Vector3d tempAnchor(tempA[0],tempA[1], tempA[2]);

    dJointGetUniversalAxis1(physicalJoint, tempA);
    Vector3d tempAxis1(tempA[0], tempA[1], tempA[2]);
    tempAxis1 *= 0.1;
    dJointGetUniversalAxis2(physicalJoint, tempA);
    Vector3d tempAxis2(tempA[0], tempA[1], tempA[2]);
    tempAxis2 *= 0.1;

    if(visParams.surfaceStyle == VisualizationParameterSet::MIXED_APPEARANCE)
    {
      glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
      glShadeModel(GL_FLAT);
      GLHelper::getGLH()->drawUniversalJoint(tempAnchor, tempAxis1, tempAxis2, true);
      glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
      glShadeModel(GL_SMOOTH);
    }
    else
    {
      GLHelper::getGLH()->drawUniversalJoint(tempAnchor, tempAxis1, tempAxis2, false);
    }
  }
}
