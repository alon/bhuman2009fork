/**
 * @file Controller/View.cpp
 * 
 * Implementation of class View
 *
 * @author <A href="mailto:Tim.Laue@dfki.de">Tim Laue</A>
 */ 

#include "View.h"


void View::addToLists(std::vector<SensorPort>& sensorPortList,
                std::vector<Actuatorport*>&,
                std::vector<Actuator*>&) 
{
  sensorPortList.push_back(SensorPort(fullName, 0, this));
}

void View::addToDescriptions(std::vector<ObjectDescription>& objectDescriptionTree,
                       int depth) 
{
  //Add Sensorport:
  ObjectDescription sensorportDesc;
  int p = name.find_last_of('.');
  sensorportDesc.name = p == -1 ? name : name.substr(p + 1);
  sensorportDesc.fullName = fullName;
  sensorportDesc.depth = depth;
  sensorportDesc.type = OBJECT_TYPE_SENSORPORT;
  objectDescriptionTree.push_back(sensorportDesc);
}
