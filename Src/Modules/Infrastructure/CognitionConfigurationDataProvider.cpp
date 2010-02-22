/** 
* @file CognitionConfigurationDataProvider.cpp
* This file implements a module that provides data loaded from configuration files.
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
*/

#include "CognitionConfigurationDataProvider.h"
#include "Tools/Settings.h"

PROCESS_WIDE_STORAGE CognitionConfigurationDataProvider* CognitionConfigurationDataProvider::theInstance = 0;

CognitionConfigurationDataProvider::CognitionConfigurationDataProvider() :
theFieldDimensions(0),
theColorTable64(0),
theCameraSettings(0),
theCameraCalibration(0),
theRobotDimensions(0),
theBehaviorConfiguration(0){
  theInstance = this;

  readFieldDimensions();
  readColorTable64();
  readCameraSettings();
  readCameraCalibration();
  readRobotDimensions();
  readBehaviorConfiguration();
}

CognitionConfigurationDataProvider::~CognitionConfigurationDataProvider()
{
  if(theFieldDimensions)
    delete theFieldDimensions;
  if(theColorTable64)
    delete theColorTable64;
  if(theCameraSettings)
    delete theCameraSettings;
  if(theCameraCalibration)
    delete theCameraCalibration;
  if(theBehaviorConfiguration)
    delete theBehaviorConfiguration;
  theInstance = 0;
}

void CognitionConfigurationDataProvider::update(FieldDimensions& fieldDimensions)
{
  ASSERT(loadedFieldDimensions);
  if(theFieldDimensions)
  {
    fieldDimensions = *theFieldDimensions;
    delete theFieldDimensions;
    theFieldDimensions = 0;
  }
}

void CognitionConfigurationDataProvider::update(ColorTable64& colorTable64)
{
  readIfRequired();
  ASSERT(loadedColorTable64);
  if(theColorTable64)
  {
    colorTable64 = *theColorTable64;
    delete theColorTable64;
    theColorTable64 = 0;
  }
}

void CognitionConfigurationDataProvider::update(CameraSettings& cameraSettings)
{
  readIfRequired();
  ASSERT(loadedCameraSettings);
  if(theCameraSettings)
  {
    cameraSettings = *theCameraSettings;
    delete theCameraSettings;
    theCameraSettings = 0;
  }
}

void CognitionConfigurationDataProvider::update(CameraCalibration& cameraCalibration)
{
  readIfRequired();
  ASSERT(loadedCameraCalibration);
  if(theCameraCalibration)
  {
    cameraCalibration = *theCameraCalibration;
    delete theCameraCalibration;
    theCameraCalibration = 0;
  }
}

void CognitionConfigurationDataProvider::update(RobotDimensions& robotDimensions)
{
  readIfRequired();
  ASSERT(loadedRobotDimensions);
  if(theRobotDimensions)
  {
    robotDimensions = *theRobotDimensions;
    delete theRobotDimensions;
    theRobotDimensions = 0;
  }
}

void CognitionConfigurationDataProvider::update(BehaviorConfiguration& behaviorConfiguration)
{
  ASSERT(loadedBehaviorConfiguration);
  if(theBehaviorConfiguration)
  {
    behaviorConfiguration = *theBehaviorConfiguration;
    delete theBehaviorConfiguration;
    theBehaviorConfiguration = 0;
  }
}

void CognitionConfigurationDataProvider::readIfRequired()
{
  if(theRobotName.name != "" && theRobotName.name != Global::getSettings().robot)
  {
    Global::getSettings().robot = theRobotName.name;
    readColorTable64();
    readCameraSettings();
    readCameraCalibration();
    readRobotDimensions();
  }
}

void CognitionConfigurationDataProvider::readFieldDimensions()
{
  loadedFieldDimensions = false;
  {
    InConfigFile stream(Global::getSettings().expandLocationFilename("field.cfg"));
    if(!stream.exists())
      return;
  }
  if(!theFieldDimensions)
    theFieldDimensions = new FieldDimensions;
  theFieldDimensions->load();
  loadedFieldDimensions = true;
}

void CognitionConfigurationDataProvider::readColorTable64()
{
  loadedColorTable64 = false;
  InBinaryFile* stream = new InBinaryFile(Global::getSettings().expandRobotLocationFilename("coltable.c64"));
  if(!stream->exists())
  {
    delete stream;
    stream = new InBinaryFile(Global::getSettings().expandLocationFilename("coltable.c64"));
  }

  if(stream->exists())
  {
    if(!theColorTable64)
      theColorTable64 = new ColorTable64;
    *stream >> *theColorTable64;
    loadedColorTable64 = true;
  }

  delete stream;
}

void CognitionConfigurationDataProvider::readCameraSettings()
{
  loadedCameraSettings = false;
  InConfigFile* stream = new InConfigFile(Global::getSettings().expandRobotLocationFilename("camera.cfg"));
  if(!stream->exists())
  {
    delete stream;
    stream = new InConfigFile(Global::getSettings().expandLocationFilename("camera.cfg"));
  }

  if(stream->exists())
  {
    if(!theCameraSettings)
      theCameraSettings = new CameraSettings;
    *stream >> *theCameraSettings;
    loadedCameraSettings = true;
  }

  delete stream;
}

void CognitionConfigurationDataProvider::readCameraCalibration()
{
  loadedCameraCalibration = false;
  InConfigFile stream(Global::getSettings().expandRobotFilename("cameraCalibration.cfg"));
  if(stream.exists())
  {
    if(!theCameraCalibration)
      theCameraCalibration = new CameraCalibration;
    stream >> *theCameraCalibration;
    loadedCameraCalibration = true;
  }
}

void CognitionConfigurationDataProvider::readRobotDimensions()
{
  loadedRobotDimensions = false;
  InConfigFile stream(Global::getSettings().expandRobotFilename("robotDimensions.cfg"));
  if(stream.exists())
  {
    theRobotDimensions = new RobotDimensions;
    stream >> *theRobotDimensions;
    loadedRobotDimensions = true;
  }
}

void CognitionConfigurationDataProvider::readBehaviorConfiguration()
{
  loadedBehaviorConfiguration = false;
  InConfigFile stream(Global::getSettings().expandLocationFilename("behavior.cfg"), "agent");
  if(stream.exists())
  {
    if(!theBehaviorConfiguration)
      theBehaviorConfiguration = new BehaviorConfiguration;
    stream >> *theBehaviorConfiguration;
    loadedBehaviorConfiguration = true;
  }
}

bool CognitionConfigurationDataProvider::handleMessage(InMessage& message)
{
  return theInstance && theInstance->handleMessage2(message);
}

bool CognitionConfigurationDataProvider::handleMessage2(InMessage& message)
{
  switch(message.getMessageID())
  {
  case idColorTable64:
    if(!theColorTable64)
      theColorTable64 = new ColorTable64;
    message.bin >> *theColorTable64;
    return true;

  case idWriteColorTable64:
    if(!theColorTable64)
      theColorTable64 = new ColorTable64;
    message.bin >> *theColorTable64;
    {
      OutBinaryFile stream(Global::getSettings().expandRobotLocationFilename("coltable.c64"));
      if(stream.exists())
        stream << (const ColorTable64&) *theColorTable64;
      else
      {
        OutBinaryFile stream(Global::getSettings().expandLocationFilename("coltable.c64"));
        stream << (const ColorTable64&) *theColorTable64;
      }
    }
    return true;

  default:
    return false; 
  }
}

MAKE_MODULE(CognitionConfigurationDataProvider, Infrastructure)

