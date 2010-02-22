/**
* @file Gyroscope.cpp
* 
* Implementation of class Gyroscope
*
* @author <A href="mailto:Tim.Laue@dfki.de">Tim Laue</A>
* @author <A href="mailto:tobias.schade@tu-dortmund.de">Tobias Schade</A>
* @author <A href="mailto:markus.kuech@tu-dortmund.de">Markus Küch</A>
* @author <A href="mailto:Christian.Buettner@tu-dortmund.de">Christian Büttner</A>
*/

#include <Simulation/Simulation.h>
#include "Gyroscope.h"
#include <PhysicalObjects/PhysicalObject.h>
#include <Tools/ODETools.h>


Gyroscope::Gyroscope(const AttributeSet& attributes)
{
  name = ParserUtilities::getValueFor(attributes, "name");
  sensorReading.data.doubleArray = valueAngleVel.v;
  sensorReading.minMaxRangeValid = false;
  sensorReading.unitForDisplay = "°/s";
  sensorReading.descriptions.push_back("x");
  sensorReading.descriptions.push_back("y");
  sensorReading.descriptions.push_back("z");
  sensorReading.dimensions.push_back(3);
  sensorReading.sensorType = SensorReading::doubleArraySensor;
  sensorReading.portName = "angularVelocities";
}

bool Gyroscope::parseAttributeElements(const std::string& name, 
                                       const AttributeSet& attributes,
                                       int line, int column, 
                                       ErrorManager* errorManager)
{
  return SimObject::parseAttributeElements(name,attributes,line,column, errorManager);
}

void Gyroscope::addToLists(std::vector<SensorPort>& sensorPortList,
                           std::vector<Actuatorport*>& actuatorportList,
                           std::vector<Actuator*>& actuatorList) 
{
  sensorPortList.push_back(SensorPort(fullName+"."+sensorReading.portName, 0, this));
}

void Gyroscope::addToDescriptions(std::vector<ObjectDescription>& objectDescriptionTree,
                                  int depth) 
{
  Sensor::addToDescriptions(objectDescriptionTree, depth);  
  ObjectDescription sensorportDesc;
  sensorportDesc.name = sensorReading.portName;
  sensorportDesc.fullName = fullName + "." + sensorReading.portName;
  sensorportDesc.depth = depth + 1;
  sensorportDesc.type = OBJECT_TYPE_SENSORPORT;
  objectDescriptionTree.push_back(sensorportDesc);
}

SensorReading& Gyroscope::getSensorReading(int localPortId)
{
  // Set step counter and check, if new data needs to be computed:
  int simulationStep(simulation->getSimulationStep());
  if(simulationStep>lastComputationStep)
  {
    // get the angle velocity of the sensor in world coordinates
    ODETools::convertVector(dBodyGetAngularVel(*bodyID), valueAngleVel);  
    // get the orientation of the gyro sensor
    // orientationMatrix contains a 4x3 Matrix
    // but only the information from the left 3x3 part is needed to get the orientation of the Obj.
    const dReal *orientationMatrix = dBodyGetRotation(*bodyID);
    Matrix3d  objOrientation;
    objOrientation.col[0] = Vector3d(orientationMatrix[0],orientationMatrix[1],orientationMatrix[2]);
    objOrientation.col[1] = Vector3d(orientationMatrix[4],orientationMatrix[5],orientationMatrix[6]);
    objOrientation.col[2] = Vector3d(orientationMatrix[8],orientationMatrix[9],orientationMatrix[10]);  
    // project the angle velocity vector which is still in world coordinates to the robot coordinate system
    valueAngleVel = objOrientation * valueAngleVel;
    lastComputationStep = simulationStep;  
  }
  sensorReading.data.doubleArray = valueAngleVel.v;
  return sensorReading;
}

SimObject* Gyroscope::clone() const 
{
  Gyroscope* newGyroscope = new Gyroscope();
  newGyroscope->copyStandardMembers(this);
  newGyroscope->sensorReading = sensorReading;
  newGyroscope->sensorReading.data.doubleArray = newGyroscope->valueAngleVel.v;
  std::list<SimObject*>::const_iterator pos;
  for(pos = childNodes.begin(); pos != childNodes.end(); ++pos)
  {
    SimObject* childNode = (*pos)->clone();
    newGyroscope->addChildNode(childNode, false);
  }
  SimObject* newObject = newGyroscope;
  return newObject;
}

void Gyroscope::postProcessAfterParsing()
{
  // Get body ID of physical body
  physObj = (*(childNodes.begin()))->castToPhysicalObject();
  bodyID = physObj->getBody();
}
