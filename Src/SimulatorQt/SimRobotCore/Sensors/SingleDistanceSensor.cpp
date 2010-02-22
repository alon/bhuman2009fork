/**
 * @file SingleDistanceSensor.cpp
 * 
 * Implementation of of class SingleDistanceSensor
 *
 * @author <A href="mailto:Tim.Laue@dfki.de">Tim Laue</A>
 */

#include "Simulation/Simulation.h"
#include "SingleDistanceSensor.h"


SingleDistanceSensor::SingleDistanceSensor(): visualizationColor(0.7,0.0,0.5)
{
}

SingleDistanceSensor::SingleDistanceSensor(const AttributeSet& attributes): visualizationColor(0.7,0.0,0.5)
{
  name = ParserUtilities::getValueFor(attributes, "name");
  sensorReading.data.doubleValue = 1.0;
  sensorReading.minValue = 0.01;
  sensorReading.maxValue = 100.0;
  sensorReading.unitForDisplay = "m";
  sensorReading.dimensions.push_back(1);
  sensorReading.sensorType = SensorReading::doubleSensor;
  sensorReading.portName = "distance";
}

void SingleDistanceSensor::drawObject(const VisualizationParameterSet& visParams)
{
  if(visParams.visualizeSensors && !visParams.drawForSensor)
  {
    glColor3d(visualizationColor.v[0],
              visualizationColor.v[1],
              visualizationColor.v[2]);
    Vector3d distanceVec(sensorReading.data.doubleValue,0,0);
    distanceVec.rotate(rotation);
    distanceVec+=position;
    glBegin(GL_LINES);
      glVertex3d (position.v[0], position.v[1], position.v[2]);
      glVertex3d (distanceVec.v[0], distanceVec.v[1], distanceVec.v[2]);
    glEnd();
  }
}

bool SingleDistanceSensor::parseAttributeElements(const std::string& name, 
                                                  const AttributeSet& attributes,
                                                  int line, int column, 
                                                  ErrorManager* errorManager)
{
  if(SimObject::parseAttributeElements(name,attributes,line,column, errorManager))
    return true;
  else if(name == "Range")
    ParserUtilities::parseXYDoublePair(attributes, sensorReading.minValue, sensorReading.maxValue);
  else
    return false;
  return true;
}

void SingleDistanceSensor::addToLists(std::vector<SensorPort>& sensorPortList,
                                      std::vector<Actuatorport*>& actuatorportList,
                                      std::vector<Actuator*>& actuatorList) 
{
  sensorReading.fullSensorName = fullName;
  sensorPortList.push_back(SensorPort(fullName+"."+sensorReading.portName, 0, this));
}

void SingleDistanceSensor::addToDescriptions(std::vector<ObjectDescription>& objectDescriptionTree,
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

SensorReading& SingleDistanceSensor::getSensorReading(int localPortId)
{
  int simulationStep(simulation->getSimulationStep());
  if(simulationStep>lastComputationStep)
  {
    Vector3d ray(1.0,0.0,0.0);
    ray.rotate(rotation);
    double distance;
    if(simulation->intersectWorldWithRay(position + ray * sensorReading.minValue, ray, sensorReading.maxValue - sensorReading.minValue,distance))
    {
      sensorReading.data.doubleValue = distance + sensorReading.minValue;
    }
    else
    {
      sensorReading.data.doubleValue = sensorReading.maxValue;
    }
    lastComputationStep = simulationStep;
  }
  return sensorReading;
}

SimObject* SingleDistanceSensor::clone() const 
{
  SingleDistanceSensor* newSingleDistanceSensor = new SingleDistanceSensor();
  newSingleDistanceSensor->copyStandardMembers(this);
  newSingleDistanceSensor->sensorReading = sensorReading;
  std::list<SimObject*>::const_iterator pos;
  for(pos = childNodes.begin(); pos != childNodes.end(); ++pos)
  {
    SimObject* childNode = (*pos)->clone();
    newSingleDistanceSensor->addChildNode(childNode, false);
  }
  SimObject* newObject = newSingleDistanceSensor;
  return newObject;
}
