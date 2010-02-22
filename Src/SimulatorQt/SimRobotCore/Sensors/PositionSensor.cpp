/**
 * @file PositionSensor.cpp
 * 
 * Implementation of of class PositionSensor
 *
 * @author <A href="mailto:Tim.Laue@dfki.de">Tim Laue</A>
 */

#include <Simulation/Simulation.h>
#include "PositionSensor.h"


PositionSensor::PositionSensor(const AttributeSet& attributes)
{
  name = ParserUtilities::getValueFor(attributes, "name");
  sensorReading.data.doubleArray = position.v;
  sensorReading.minMaxRangeValid = false;
  sensorReading.unitForDisplay = "m";
  sensorReading.descriptions.push_back("x");
  sensorReading.descriptions.push_back("y");
  sensorReading.descriptions.push_back("z");
  sensorReading.dimensions.push_back(3);
  sensorReading.sensorType = SensorReading::doubleArraySensor;
  sensorReading.portName = "position";
}

bool PositionSensor::parseAttributeElements(const std::string& name, 
                                            const AttributeSet& attributes,
                                            int line, int column, 
                                            ErrorManager* errorManager)
{
  return SimObject::parseAttributeElements(name,attributes,line,column, errorManager);
}

void PositionSensor::addToLists(std::vector<SensorPort>& sensorPortList,
                                std::vector<Actuatorport*>& actuatorportList,
                                std::vector<Actuator*>& actuatorList) 
{
  sensorPortList.push_back(SensorPort(fullName+"."+sensorReading.portName,0,this));
}

void PositionSensor::addToDescriptions(std::vector<ObjectDescription>& objectDescriptionTree,
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

SensorReading& PositionSensor::getSensorReading(int localPortId)
{
  return sensorReading;
}

SimObject* PositionSensor::clone() const 
{
  PositionSensor* newPositionSensor = new PositionSensor();
  newPositionSensor->copyStandardMembers(this);
  newPositionSensor->sensorReading = sensorReading;
  newPositionSensor->sensorReading.data.doubleArray = newPositionSensor->position.v;
  std::list<SimObject*>::const_iterator pos;
  for(pos = childNodes.begin(); pos != childNodes.end(); ++pos)
  {
    SimObject* childNode = (*pos)->clone();
    newPositionSensor->addChildNode(childNode, false);
  }
  SimObject* newObject = newPositionSensor;
  return newObject;
}
