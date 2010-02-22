/**
 * @file Slider.cpp
 * 
 * Implementation of class Slider
 *
 * @author <A href="mailto:kspiess@informatik.uni-bremen.de">Kai Spiess</A>
 */ 

#include "Slider.h"
#include "Motor.h"
#include "MotorController.h"
#include "../Simulation/Actuatorport.h"
#include "../Simulation/Simulation.h"
#include "../PhysicalObjects/PhysicalObject.h"
#include "../Simulation/PhysicsParameterSet.h"
#include "../Tools/SimGraphics.h"


Slider::Slider(const AttributeSet& attributes)
{
  name = ParserUtilities::getValueFor(attributes, "name");
}


bool Slider::parseAttributeElements(const std::string& name, 
                                    const AttributeSet& attributes,
                                    int line, int column,
                                    ErrorManager* errorManager)
{
  if(name == "Axis")
  {
    parseAxis(name, attributes);
    axis = &axes[0];
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
    axis->motor->parseController(name, attributes);
  }
  else if(name == "PIDController")
  {
    axis->motor->parseController(name, attributes);
    ((PIDController*)axis->motor->getMotorController())->setTimeStep(simulation->getStepLength());
  }
  else if(name == "Friction")
  {
    if(!axes.empty())
      axis->friction = ParserUtilities::toDouble(ParserUtilities::getValueFor(attributes, "value"));
  }
  return true;
}

SimObject* Slider::clone() const
{
  Slider* newSlider = new Slider();
  //copy SimObject members
  newSlider->copyStandardMembers(this);
  bool createsNewMovID = newSlider->adjustMovableID();
  //copy joint members
    //axes
  std::vector<AxisDescription>::const_iterator axesIter;
  for(axesIter = axes.begin(); axesIter != axes.end(); ++axesIter)
  {
    newSlider->axes.push_back(*axesIter);
    if(axesIter->motor != NULL)
    {
      newSlider->motors.push_back(axesIter->motor->clone());
      newSlider->axes.back().motor = newSlider->motors.back();
    }

  }
    //currentMotor
  if(!newSlider->motors.empty())
    newSlider->currentMotor = newSlider->motors.back();

  //copy slider members
  newSlider->axis = &(newSlider->axes[0]);

  //copy child nodes
  newSlider->childNodes.clear();
  std::list<SimObject*>::const_iterator pos;
  for(pos = childNodes.begin(); pos != childNodes.end(); ++pos)
  {
    SimObject* childNode = (*pos)->clone();
    newSlider->addChildNode(childNode, false);
  }

  if(createsNewMovID)
    simulation->removeCurrentMovableID();

  SimObject* newObject = newSlider;
  return newObject;
}

void Slider::addToLists(std::vector<SensorPort>& sensorPortList,
                        std::vector<Actuatorport*>& actuatorportList,
                        std::vector<Actuator*>& actuatorList) 
{
  if(axis)
  {
    sensorPortList.push_back(SensorPort(fullName+".position",AXIS_JOINT,this));
    if(axis->motor)
    {
      std::string actuatorDescription;
      std::string sensorDescription;
      double minVal=0.0, maxVal=0.0;
      if(axis->motor->getMotorType() == VELOCITY)
      {
        actuatorDescription = "motorVelocity";
        sensorDescription = "motorVelocitySensor";
        minVal = -axis->motor->getMaxVelocity();
        maxVal = axis->motor->getMaxVelocity();
      }
      else if(axis->motor->getMotorType() == ANGULAR)
      {
        actuatorDescription = "motorPosition";
        sensorDescription = "motorPositionSensor";
        minVal = axis->minValue;
        maxVal = axis->maxValue;
      }
      Actuatorport* value = new Actuatorport(actuatorDescription, this, AXIS_MOTOR, minVal, maxVal);
      actuatorportList.push_back(value);
      sensorPortList.push_back(SensorPort(fullName+"."+sensorDescription,AXIS_MOTOR,this));
      actuatorList.push_back(this);
    }
  }
}

void Slider::addToDescriptions(std::vector<ObjectDescription>& objectDescriptionTree,
                               int depth) 
{
  Sensor::addToDescriptions(objectDescriptionTree, depth);  
  //Add Sensorports:
  ObjectDescription valueDesc;
  valueDesc.name = "position";
  valueDesc.fullName = fullName + ".position";
  valueDesc.depth = depth + 1;
  valueDesc.type = OBJECT_TYPE_SENSORPORT;
  objectDescriptionTree.push_back(valueDesc);

  if(axis->motor)
  {
    std::string actuatorDescription;
    std::string sensorDescription;
    if(axis->motor->getMotorType() == VELOCITY)
    {
      actuatorDescription = "motorVelocity";
      sensorDescription = "motorVelocitySensor";
    }
    else if(axis->motor->getMotorType() == ANGULAR)
    {
      actuatorDescription = "motorPosition";
      sensorDescription = "motorPositionSensor";
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


void Slider::setValue(double value, int port)
{
  /* TEMPORARY FIX */
  if(port == AXIS_MOTOR)
  {
    double newValue = value;
    if(axis->motor->getMotorType() == ANGULAR)
    {
      if(value == axis->maxValue || value == axis->minValue)
      {
        if(value == 0.0 && axis->minValue == 0.0)
        {
          newValue = -0.01;
        }
        else if(value == 0.0 && axis->maxValue == 0.0)
        {
          newValue = 0.01;
        }
        else
          newValue -= (value *dReal(0.01));
      }
    }
    axis->motor->setValue(newValue, port);
  }
}

void Slider::act(bool initial)
{
  if(!initial)
  {
    if(axis->motor->getMotorType() == VELOCITY)
    {
      dReal desiredVelocity = dReal(axis->motor->getDesiredVelocity());
      dJointSetSliderParam(physicalJoint, dParamFMax, dReal(axis->motor->getMaxForce()));
      dJointSetSliderParam(physicalJoint, dParamVel, desiredVelocity);
    }
    else if(axis->motor->getMotorType() == ANGULAR)
    {
      double controllerOutput = axis->motor->getControllerOutput(dJointGetSliderPosition(physicalJoint));
      dJointSetSliderParam(physicalJoint, dParamFMax, dReal(axis->motor->getMaxForce()));
      dJointSetSliderParam(physicalJoint, dParamVel, dReal(controllerOutput));
    }
  }
}

void Slider::computeValue(double& value, int portId)
{
  if(portId == AXIS_JOINT)
    value = (dJointGetSliderPosition(physicalJoint));
  else if(portId == AXIS_MOTOR)
  {
    if(axis->motor->getMotorType() == VELOCITY)
      value = (dJointGetSliderParam(physicalJoint, dParamVel));
    else
      value = (dJointGetSliderPosition(physicalJoint));
  }
}

void Slider::createJointPhysics()
{
  Vector3d physAxis = axis->direction;
  physAxis.rotate(rotation);

  physicalJoint = dJointCreateSlider(*simulation->getPhysicalWorld(), 0);
  PhysicalObject* o1 = getPhysicalParentObject();
  PhysicalObject* o2 = getPhysicalChildObject(this);
  assert (o1 != 0 && o2 != 0);
  if(o1 && o2)
  {
    dJointAttach(physicalJoint, *(o2->getBody()), *(o1->getBody()));
    //set slider joint parameter
    dJointSetSliderAxis(physicalJoint, dReal(axis->direction.v[0]), dReal(axis->direction.v[1]), dReal(axis->direction.v[2]));

    if(axis->cfm != -1.0)
      dJointSetSliderParam(physicalJoint, dParamCFM, dReal(axis->cfm));

    if(axis->limited || (axis->motor != 0 && axis->motor->getMotorType() == ANGULAR))
    {
      dJointSetSliderParam(physicalJoint, dParamLoStop, dReal(axis->minValue));
      dJointSetSliderParam(physicalJoint, dParamHiStop, dReal(axis->maxValue));
      // this has to be done due to the way ODE sets joint stops
      dJointSetSliderParam(physicalJoint, dParamLoStop, dReal(axis->minValue));

      if(axis->fullScaleDeflectionCFM != -1.0)
        dJointSetSliderParam(physicalJoint, dParamStopCFM, dReal(axis->fullScaleDeflectionCFM));
      if(axis->fullScaleDeflectionERP != -1.0)
        dJointSetSliderParam(physicalJoint, dParamStopERP, dReal(axis->fullScaleDeflectionERP));
    }
  }
}

void Slider::applyFriction(bool initial)
{
  if(!initial)
  {
    if(motors.size()== 0)
    {
      double fmax = axis->friction * fabs((simulation->getPhysicsParameters())->getGravity());
      double velocity = 0.0;
      dJointSetSliderParam(physicalJoint, dParamFMax, dReal(fmax));
      dJointSetSliderParam(physicalJoint, dParamVel, dReal(velocity));
    }
  }
}

void Slider::drawObject(const VisualizationParameterSet& visParams)
{
  if(visParams.surfaceStyle == VisualizationParameterSet::PHYSICS_WIREFRAME ||
     visParams.surfaceStyle == VisualizationParameterSet::PHYSICS_FLAT ||
     visParams.surfaceStyle == VisualizationParameterSet::PHYSICS_SMOOTH ||
     visParams.surfaceStyle == VisualizationParameterSet::MIXED_APPEARANCE ||
     visParams.surfaceStyle == VisualizationParameterSet::MIXED_PHYSICS)
  {
    dVector3 tempA;
    dJointGetSliderAxis(physicalJoint, tempA);
    Vector3d tempAxis(tempA[0], tempA[1], tempA[2]);
    tempAxis *= 0.1;
    dBodyID b2 = dJointGetBody(physicalJoint, 0);
    if(b2 == NULL)
      b2 = dJointGetBody(physicalJoint, 1);
    const dReal* b2Pos = dBodyGetPosition(b2);
    Vector3d pos(b2Pos[0], b2Pos[1], b2Pos[2]);

    if(visParams.surfaceStyle == VisualizationParameterSet::MIXED_APPEARANCE)
    {
      glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
      glShadeModel(GL_FLAT);
      GLHelper::getGLH()->drawSlider(pos, tempAxis, true);
      glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
      glShadeModel(GL_SMOOTH);
    }
    else
      GLHelper::getGLH()->drawSlider(pos, tempAxis, false);
  }
}
