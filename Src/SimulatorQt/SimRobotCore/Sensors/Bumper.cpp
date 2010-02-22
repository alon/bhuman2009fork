/**
 * @file Bumper.cpp
 * 
 * Implementation of class Bumper
 *
 * @author <A href="mailto:Tim.Laue@dfki.de">Tim Laue</A>
 * @author <A href="mailto:kspiess@informatik.uni-bremen.de">Kai Spiess</A>
 */ 

#include <Simulation/Simulation.h>
#include "Bumper.h"


Bumper::Bumper(const AttributeSet& attributes) 
{
  name = ParserUtilities::getValueFor(attributes, "name");
  sensorReading.data.boolValue = false;
  sensorReading.minValue = 0.0;
  sensorReading.maxValue = 1.0;
  sensorReading.dimensions.push_back(1);
  sensorReading.sensorType = SensorReading::boolSensor;
  sensorReading.portName = "collision";
}

void Bumper::addToLists(std::vector<SensorPort>& sensorPortList,
                        std::vector<Actuatorport*>& actuatorportList,
                        std::vector<Actuator*>& actuatorList) 
{
  sensorPortList.push_back(SensorPort(fullName+"."+sensorReading.portName,0,this));
}

void Bumper::addToDescriptions(std::vector<ObjectDescription>& objectDescriptionTree,
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

SensorReading& Bumper::getSensorReading(int localPortId)
{
  return sensorReading;
}

SimObject* Bumper::clone() const 
{
  Bumper* newBumper = new Bumper();
  newBumper->copyStandardMembers(this);
  newBumper->sensorReading = sensorReading;
  bool createsNewMovID = newBumper->adjustMovableID();
  std::list<SimObject*>::const_iterator pos;
  for(pos = childNodes.begin(); pos != childNodes.end(); ++pos)
  {
    SimObject* childNode = (*pos)->clone();
    newBumper->addChildNode(childNode, false);
  }
  if(createsNewMovID)
    this->simulation->removeCurrentMovableID();
  SimObject* newObject = newBumper;
  return newObject;
}
