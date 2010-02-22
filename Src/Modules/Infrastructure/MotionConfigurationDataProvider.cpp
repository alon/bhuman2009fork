/** 
* @file MotionConfigurationDataProvider.cpp
* This file implements a module that provides data loaded from configuration files.
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
*/

#include "MotionConfigurationDataProvider.h"
#include "Tools/Settings.h"


MotionConfigurationDataProvider::MotionConfigurationDataProvider() :
theJointCalibration(0),
theSensorCalibration(0),
theRobotDimensions(0),
theMassCalibration(0)
{
  for(int i = 0; i < 8; i++)
    fsrOffsets[i] = 0;
  fsrFullSum = -1;
  fsrSumFactorLeft = -1;
  fsrSumFactorRight = -1;
  readIfRequired();
}

MotionConfigurationDataProvider::~MotionConfigurationDataProvider()
{
  if(theJointCalibration)
    delete theJointCalibration;
  if(theSensorCalibration)
    delete theSensorCalibration;
  if(theRobotDimensions)
    delete theRobotDimensions;
  if(theMassCalibration)
    delete theMassCalibration;
}

void MotionConfigurationDataProvider::update(RobotName& robotName)
{
  readIfRequired();
  robotName.name = this->robot;
}

void MotionConfigurationDataProvider::update(JointCalibration& jointCalibration)
{
  readIfRequired();
  ASSERT(loadedJointCalibration);
  if(theJointCalibration)
  {
    jointCalibration = *theJointCalibration;
    delete theJointCalibration;
    theJointCalibration = 0;
  }
   DEBUG_RESPONSE("representation:jointCalibration", OUTPUT(idJointCalibration, bin, jointCalibration););
}

void MotionConfigurationDataProvider::update(SensorCalibration& sensorCalibration)
{
  readIfRequired();
  ASSERT(loadedSensorCalibration);
  if(theSensorCalibration)
  {
    sensorCalibration = *theSensorCalibration;
    delete theSensorCalibration;
    theSensorCalibration = 0;
  }
}

void MotionConfigurationDataProvider::update(RobotDimensions& robotDimensions)
{
  readIfRequired();
  ASSERT(loadedRobotDimensions);
  if(theRobotDimensions)
  {
    robotDimensions = *theRobotDimensions;
    delete theRobotDimensions;
    theRobotDimensions = 0;
  }
  DEBUG_RESPONSE("representation:robotDimensions", OUTPUT(idRobotDimensions, bin,robotDimensions););
}

void MotionConfigurationDataProvider::update(MassCalibration& massCalibration)
{
  readIfRequired();
  ASSERT(loadedMassCalibration);
  if(theMassCalibration)
  {
    massCalibration = *theMassCalibration;
    delete theMassCalibration;
    theMassCalibration = 0;
  }
}

void MotionConfigurationDataProvider::readIfRequired()
{
  if(robot != Global::getSettings().robot)
  {
    robot = Global::getSettings().robot;
    readJointCalibration();
    readSensorCalibration();
    readRobotDimensions();
    readMassCalibration();
  }
}

void MotionConfigurationDataProvider::readJointCalibration()
{
  loadedJointCalibration = false;
  InConfigFile stream(Global::getSettings().expandRobotFilename("jointCalibration.cfg"));
  if(stream.exists())
  {
    if(!theJointCalibration)
      theJointCalibration = new JointCalibration;
    stream >> *theJointCalibration;
    loadedJointCalibration = true;
  }
}

void MotionConfigurationDataProvider::readSensorCalibration()
{
  loadedSensorCalibration = false;
  InConfigFile stream(Global::getSettings().expandRobotFilename("sensorCalibration.cfg"));
  if(stream.exists())
  {
    if(!theSensorCalibration)
      theSensorCalibration = new SensorCalibration;
    stream >> *theSensorCalibration;
    loadedSensorCalibration = true;
  }
}

void MotionConfigurationDataProvider::readRobotDimensions()
{
  loadedRobotDimensions = false;
  InConfigFile stream(Global::getSettings().expandRobotFilename("robotDimensions.cfg"));
  if(stream.exists())
  {
    if(!theRobotDimensions)
      theRobotDimensions = new RobotDimensions;
    stream >> *theRobotDimensions;
    loadedRobotDimensions = true;
  }
}

void MotionConfigurationDataProvider::readMassCalibration()
{
  loadedMassCalibration = false;
  InConfigFile stream(Global::getSettings().expandRobotFilename("masses.cfg"));
  if(stream.exists())
  {
    if(!theMassCalibration)
      theMassCalibration = new MassCalibration;
    stream >> *theMassCalibration;
    loadedMassCalibration = true;
  }
}


MAKE_MODULE(MotionConfigurationDataProvider, Infrastructure)
