/**
 * @file PressureSensor.cpp
 * 
 * Implementation of class PressureSensor
 *
 * @author Jeff 
 */ 

#include "PressureSensor.h"
#include "../Actuators/Motor.h"
#include "../Actuators/MotorController.h"
#include "../Simulation/Actuatorport.h"
#include "../Simulation/Simulation.h"
#include "../PhysicalObjects/PhysicalObject.h"
#include "../Simulation/PhysicsParameterSet.h"
#include "../Tools/SimGraphics.h"

PressureSensor::PressureSensor()
{
  feedback = new dJointFeedback();
  gain.init(1,1,1);
  offset.init(0,0,0);
}

PressureSensor::PressureSensor(const AttributeSet& attributes)
{
  name = ParserUtilities::getValueFor(attributes, "name");
  feedback = new dJointFeedback();

  gain.init(1,1,1);
  offset.init(0,0,0);
}

bool PressureSensor::parseAttributeElements(const std::string& name, 
                                    const AttributeSet& attributes,
                                    int line, int column,
                                    ErrorManager* errorManager)
{
  if(name == "Gain")
    gain = ParserUtilities::toPoint(attributes);
  else if(name =="Offset")
    offset = ParserUtilities::toPoint(attributes);

  return true;
}

SimObject* PressureSensor::clone() const
{
  PressureSensor* newPressureSensor = new PressureSensor();
  //copy SimObject members
  newPressureSensor->copyStandardMembers(this);
  bool createsNewMovID = newPressureSensor->adjustMovableID();
  //copy pressure sensor members
  newPressureSensor->gain = gain;
  newPressureSensor->offset = offset;
  //copy joint members
    //axes
  std::vector<AxisDescription>::const_iterator axesIter;
  for(axesIter = axes.begin(); axesIter != axes.end(); ++axesIter)
  {
    newPressureSensor->axes.push_back(*axesIter);
    if(axesIter->motor != NULL)
    {
      newPressureSensor->motors.push_back(axesIter->motor->clone());
      newPressureSensor->axes.back().motor = newPressureSensor->motors.back();
    }

  }

  //copy child nodes
  newPressureSensor->childNodes.clear();
  std::list<SimObject*>::const_iterator pos;
  for(pos = childNodes.begin(); pos != childNodes.end(); ++pos)
  {
    SimObject* childNode = (*pos)->clone();
    newPressureSensor->addChildNode(childNode, false);
  }

  if(createsNewMovID)
    this->simulation->removeCurrentMovableID();

  SimObject* newObject = newPressureSensor;
  return newObject;
}

void PressureSensor::addToLists(std::vector<SensorPort>& sensorPortList,
                        std::vector<Actuatorport*>& actuatorportList,
                        std::vector<Actuator*>& actuatorList) 
{
  sensorPortList.push_back(SensorPort(fullName+".pressure", 0, this));
}

void PressureSensor::addToDescriptions(std::vector<ObjectDescription>& objectDescriptionTree,
                               int depth) 
{
  Sensor::addToDescriptions(objectDescriptionTree, depth);  
  //Add Sensorports:
  ObjectDescription valueDesc;
  valueDesc.name = "pressure";
  valueDesc.fullName = fullName + ".pressure";
  valueDesc.depth = depth + 1;
  valueDesc.type = OBJECT_TYPE_SENSORPORT;
  objectDescriptionTree.push_back(valueDesc);

}

SensorReading& PressureSensor::getSensorReading(int localPortId)
{
  	  // Set step counter and check, if new data needs to be computed:
  int simulationStep(simulation->getSimulationStep());
  if(simulationStep>lastComputationStep)
  {
    //get the pressure on the joint
  feedback = dJointGetFeedback(this->physicalJoint);
  for(int i = 0; i < 3; i++)
    valuePressure.v[i] = (double)feedback->f1[i];

  // get the orientation of the pressure sensor
  // orientationMatrix contains a 4x3 Matrix
  // but only the information from the left 3x3 part is needed to get the orientation of the Obj.
  const dReal *orientationMatrix = dBodyGetRotation(*bodyID);
  Matrix3d  objOrientation;
  objOrientation.col[0] = Vector3d(orientationMatrix[0],orientationMatrix[1],orientationMatrix[2]);
  objOrientation.col[1] = Vector3d(orientationMatrix[4],orientationMatrix[5],orientationMatrix[6]);
  objOrientation.col[2] = Vector3d(orientationMatrix[8],orientationMatrix[9],orientationMatrix[10]);  
  // project the pressure vector which is still in world coordinates to the robot coordinate system
  valuePressure = objOrientation * valuePressure;
  lastComputationStep = simulationStep;  

  for(int i = 0; i < 3; i++)
  {
    valuePressure.v[i] *= gain.v[i];
    valuePressure.v[i] += offset.v[i];
  }
  }

  sensorReading.data.doubleArray = valuePressure.v;
  return sensorReading;
}

void PressureSensor::postProcessAfterParsing()
{
  // Get body ID of physical body
  physObj = (*(childNodes.begin()))->castToPhysicalObject();
  bodyID = physObj->getBody();
}

void PressureSensor::createJointPhysics()
{
  this->physicalJoint = dJointCreateSlider(*this->simulation->getPhysicalWorld(), 0);
  PhysicalObject* o1 = this->getPhysicalParentObject();
  PhysicalObject* o2 = this->getPhysicalChildObject(this);
  assert (o1 != NULL && o2 != NULL);
  if(o1 && o2)
  {
    dJointAttach(this->physicalJoint, *(o2->getBody()), *(o1->getBody()));
    //set slider joint parameter
    dJointSetSliderAxis(this->physicalJoint, dReal(0), dReal(0), dReal(1));
    dJointSetFeedback(this->physicalJoint, feedback);

    dJointSetSliderParam(this->physicalJoint, dParamCFM, dReal(1));
    //dJointSetSliderParam(this->physicalJoint, dParamERP, dReal(1));
    //dJointSetSliderParam(this->physicalJoint, dParamStopERP, dReal(1));
    dJointSetSliderParam(this->physicalJoint, dParamStopCFM, dReal(1));

    dJointSetSliderParam(this->physicalJoint, dParamLoStop, dReal(0));
    dJointSetSliderParam(this->physicalJoint, dParamHiStop, dReal(0));
    
  }
}


void PressureSensor::drawObject(const VisualizationParameterSet& visParams)
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
