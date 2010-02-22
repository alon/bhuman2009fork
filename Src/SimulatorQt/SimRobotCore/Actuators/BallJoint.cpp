/**
 * @file BallJoint.cpp
 * 
 * Implementation of class BallJoint
 *
 * @author <A href="mailto:Tim.Laue@dfki.de">Tim Laue</A>
 */ 

#include "BallJoint.h"
#include "Motor.h"
#include "../Simulation/Actuatorport.h"
#include "../Simulation/Simulation.h"
#include "../PhysicalObjects/PhysicalObject.h"
#include "../Tools/SimGraphics.h"


BallJoint::BallJoint() : anchorPoint(position), motor(0)
{
}

BallJoint::BallJoint(const AttributeSet& attributes) : anchorPoint(position), motor(0)
{
  name = ParserUtilities::getValueFor(attributes, "name");
}

bool BallJoint::parseAttributeElements(const std::string& name, 
                                       const AttributeSet& attributes,
                                       int line, int column,
                                       ErrorManager* errorManager)
{
  if(name == "AnchorPoint")
  {
    anchorPoint = parentNode->getPosition();
    anchorPoint += parentNode->getRotation() * (ParserUtilities::toPoint(attributes));
  }
  else if(name == "AngularMotor")
  {
    parseMotor(name, attributes);
  }
  else if(name == "VelocityMotor")
  {
    parseMotor(name, attributes);
  }
  else if(name =="Axis1" || name == "Axis2" || name == "Axis3")
  {
    currentMotor->parseAxis(name, attributes);
  }
  else if(name == "Friction")
  {
    if(!axes.empty())
      axes.back().friction = ParserUtilities::toDouble(ParserUtilities::getValueFor(attributes, "value"));
  }
  return true;
}

SimObject* BallJoint::clone() const
{
  BallJoint* newBallJoint = new BallJoint();
  //copy SimObject members
  newBallJoint->copyStandardMembers(this);
  bool createsNewMovID = newBallJoint->adjustMovableID();
  //copy joint members
    //axes
  std::vector<AxisDescription>::const_iterator axesIter;
  for(axesIter = axes.begin(); axesIter != axes.end(); ++axesIter)
  {
    newBallJoint->axes.push_back(*axesIter);
  }
    //motors
  std::vector<Motor*>::const_iterator motorsIter;
  for(motorsIter = motors.begin(); motorsIter != motors.end(); ++motorsIter)
  {
    newBallJoint->motors.push_back((*motorsIter)->clone());
  }
    //currentMotor
  if(!newBallJoint->motors.empty())
    newBallJoint->currentMotor = newBallJoint->motors.back();

  //copy ball joint members
  newBallJoint->anchorPoint = this->anchorPoint;
  if(!motors.empty())
    newBallJoint->motor = motors.front();

  //copy child nodes
  newBallJoint->childNodes.clear();
  std::list<SimObject*>::const_iterator pos;
  for(pos = childNodes.begin(); pos != childNodes.end(); ++pos)
  {
    SimObject* childNode = (*pos)->clone();
    newBallJoint->addChildNode(childNode, false);
  }

  if(createsNewMovID)
    this->simulation->removeCurrentMovableID();

  SimObject* newObject = newBallJoint;
  return newObject;
}

void BallJoint::addToLists(std::vector<SensorPort>& sensorPortList,
                       std::vector<Actuatorport*>& actuatorportList,
                       std::vector<Actuator*>& actuatorList) 
{
  if(motors.size())
  {
    std::string actuatorDescription;
    std::string sensorDescription;
    Motor* motor = motors.front();
    std::vector<MotorAxisDescription>::const_iterator pos;

    for(int i = 0; i < motor->getNumberOfParsedAxes(); i++)
    {
      MotorAxisDescription* axis = motor->getAxis(i);
      actuatorDescription = "motorAxis" + i;
      actuatorDescription += "Actuator";
      sensorDescription = "motorAxis" + i;
      sensorDescription += "Sensor";

      Actuatorport* value = new Actuatorport
        (actuatorDescription, this, i, axis->minValue, axis->maxValue);
      actuatorportList.push_back(value);
      sensorPortList.push_back(SensorPort(fullName+"."+sensorDescription, i, this));
      actuatorList.push_back(this);
    }
  }
}

void BallJoint::addToDescriptions(std::vector<ObjectDescription>& objectDescriptionTree,
                              int depth) 
{
  if(motors.size())
  {
    Sensor::addToDescriptions(objectDescriptionTree, depth);  

    ObjectDescription valueDesc;

    std::string actuatorDescription;
    std::string sensorDescription;
    Motor* motor = motors.front();
    std::vector<MotorAxisDescription>::const_iterator pos;

    for(int i = 0; i < motor->getNumberOfParsedAxes(); i++)
    {
      motor->getAxis(i);
      actuatorDescription = "motorAxis" + i;
      actuatorDescription += "Actuator";
      sensorDescription = "motorAxis" + i;
      sensorDescription += "Sensor";

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
}

void BallJoint::setValue(double value, int port)
{
  currentMotor->setValue(value, port);
}

void BallJoint::act(bool initial)
{
  if(!initial)
  {
    for(unsigned int i=0; i < this->motors.size(); i++)
    {
      int numAxes = motors[i]->getNumberOfParsedAxes();
      if(numAxes)
      {
        if(motors[i]->getMotorType() == VELOCITY)
        {
          double desiredVelocity = motors[i]->getDesiredVelocity();
          dJointSetAMotorParam(*(motors[i]->getPhysicalMotor()), dParamVel, dReal(desiredVelocity));
        }
        if(numAxes > 1)
        {
          if(motors[i]->getMotorType() == VELOCITY)
          {
            double desiredVelocity = motors[i]->getDesiredVelocity();
            dJointSetAMotorParam(*(motors[i]->getPhysicalMotor()), dParamVel2, dReal(desiredVelocity));
          }
          if(numAxes > 2)
          {
            if(motors[i]->getMotorType() == VELOCITY)
            {
              double desiredVelocity = motors[i]->getDesiredVelocity();
              dJointSetAMotorParam(*(motors[i]->getPhysicalMotor()), dParamVel3, dReal(desiredVelocity));
            }
          }
        }
      }
    }
  }
}

void BallJoint::computeValue(double& value, int portId)
{
  if(portId == MOTOR_AXIS_1)
    value = (dJointGetAMotorParam(*(currentMotor->getPhysicalMotor()), dParamVel));
  else if(portId == MOTOR_AXIS_2)
    value = (dJointGetAMotorParam(*(currentMotor->getPhysicalMotor()), dParamVel2));
  else if(portId == MOTOR_AXIS_3)
    value = (dJointGetAMotorParam(*(currentMotor->getPhysicalMotor()), dParamVel3));
}

void BallJoint::createJointPhysics()
{
  physicalJoint = dJointCreateBall(*simulation->getPhysicalWorld(), 0);
  PhysicalObject* o1 = getPhysicalParentObject();
  PhysicalObject* o2 = getPhysicalChildObject(this);
  assert (o1 != 0 && o2 != 0);
  if(o1 && o2)
  {
    dJointAttach(physicalJoint, *(o2->getBody()), *(o1->getBody()));
    //set ball joint parameter
    dJointSetBallAnchor(physicalJoint, dReal(anchorPoint.v[0]), dReal(anchorPoint.v[1]), dReal(anchorPoint.v[2]));
  }
}

void BallJoint::createMotorPhysics()
{
  for(unsigned int i=0; i<motors.size(); i++)
  {
    dJointID physMotor = dJointCreateAMotor(*(simulation->getPhysicalWorld()), 0);
    motors[i]->setPhysicalMotor(physMotor);
    dJointAttach(physMotor, dJointGetBody(physicalJoint, 0), dJointGetBody(physicalJoint, 1));

    if(motors[i]->getKind() == "eulermotor")
      dJointSetAMotorMode(physMotor, dAMotorEuler);
    else
      dJointSetAMotorMode(physMotor, dAMotorUser);

    //user defined motor
    if(this->motors[i]->getKind() == "userdefinedmotor")
    {
      unsigned short numOfAxes = this->motors[i]->getNumberOfParsedAxes();
      dJointSetAMotorNumAxes(physMotor, numOfAxes);

      dBodyID b1= dJointGetBody(this->physicalJoint, 0);
      if(b1 != NULL)
      {
        for(int j=0; j < numOfAxes; j++)
        {
          MotorAxisDescription* ax = this->motors[i]->getAxis(j);
          dJointSetAMotorAxis(physMotor, j, 1, dReal(ax->direction.v[0]), dReal(ax->direction.v[1]), dReal(ax->direction.v[2]));
        }
      }
      else
      {
        for(int j=0; j < numOfAxes; j++)
        {
          MotorAxisDescription* ax = this->motors[i]->getAxis(j);
          dJointSetAMotorAxis(physMotor, j, 0, dReal(ax->direction.v[0]), dReal(ax->direction.v[1]), dReal(ax->direction.v[2]));
        }
      }

      //maxForce and maxVelocity are stored in each axis of the motor
      dJointSetAMotorParam(physMotor, dParamFMax, dReal(this->motors[i]->getAxis(0)->maxForce));
      if(numOfAxes > 1)
      {
        dJointSetAMotorParam(physMotor, dParamFMax2, dReal(this->motors[i]->getAxis(1)->maxForce));
        if(numOfAxes > 2)
        {
          dJointSetAMotorParam(physMotor, dParamFMax3, dReal(this->motors[i]->getAxis(2)->maxForce));
        }
      }
      //stops are useless for balljoint
    }
  }
}

void BallJoint::drawObject(const VisualizationParameterSet& visParams)
{
  if(visParams.surfaceStyle == VisualizationParameterSet::PHYSICS_WIREFRAME ||
     visParams.surfaceStyle == VisualizationParameterSet::PHYSICS_FLAT ||
     visParams.surfaceStyle == VisualizationParameterSet::PHYSICS_SMOOTH ||
     visParams.surfaceStyle == VisualizationParameterSet::MIXED_APPEARANCE ||
     visParams.surfaceStyle == VisualizationParameterSet::MIXED_PHYSICS)
  {
    dVector3 tempA;
    dJointGetBallAnchor(this->physicalJoint, tempA);
    Vector3d tempAnchor(tempA[0],tempA[1], tempA[2]);

    if(visParams.surfaceStyle == VisualizationParameterSet::MIXED_APPEARANCE)
    {
      glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
      glShadeModel(GL_FLAT);
      GLHelper::getGLH()->drawBallJoint(tempAnchor, true);
      glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
      glShadeModel(GL_SMOOTH);
    }
    else
      GLHelper::getGLH()->drawBallJoint(tempAnchor, false);
  }
}
