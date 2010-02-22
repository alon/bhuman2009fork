/** 
* @file MotionLogDataProvider.cpp
* This file implements a module that provides data replayed from a log file.
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
*/

#include "MotionLogDataProvider.h"
#include "Tools/Debugging/DebugDrawings.h"

PROCESS_WIDE_STORAGE MotionLogDataProvider* MotionLogDataProvider::theInstance = 0;

#define UPDATE2(representation, cmd) \
void MotionLogDataProvider::update(representation& _##representation) \
{ \
  if(representationBuffer[id##representation]) \
    _##representation = *((representation*)representationBuffer[id##representation]); \
  cmd \
}

#define UPDATE(representation) UPDATE2(representation, )

#define ALLOC(representation) \
  if(!representationBuffer[id##representation]) \
    representationBuffer[id##representation] = new representation;

#define HANDLE2(representation, cmd) \
  case id##representation:\
  { \
    ALLOC(representation) \
    message.bin >> *((representation*)representationBuffer[id##representation]); \
    cmd \
    return true; \
  }

#define HANDLE(representation) HANDLE2(representation, )

MotionLogDataProvider::MotionLogDataProvider() :
  frameDataComplete(false),
  theGroundTruthOdometryData(0)
{
  theInstance = this;
  for(int i = 0; i < numOfDataMessageIDs; i++)
    representationBuffer[i] = 0;
}

MotionLogDataProvider::~MotionLogDataProvider()
{
  if(theGroundTruthOdometryData)
    delete theGroundTruthOdometryData;
  for(int i = 0; i < numOfDataMessageIDs; i++)
    if(representationBuffer[i])
      delete representationBuffer[i];
  theInstance = 0;
}

UPDATE(BoardInfo)
UPDATE(FrameInfo)
UPDATE2(JointData,
  JointDataDeg jointDataDeg(_JointData);
  MODIFY("representation:JointDataDeg", jointDataDeg);
)
UPDATE(KeyStates)
UPDATE(OdometryData)
UPDATE(SensorData)
UPDATE(FilteredSensorData)
UPDATE(FilteredJointData)

void MotionLogDataProvider::update(GroundTruthOdometryData& groundTruthOdometryData)
{
  if(theGroundTruthOdometryData)
    groundTruthOdometryData = *theGroundTruthOdometryData;
#ifndef RELEASE
  Pose2D odometryOffset(groundTruthOdometryData);
  odometryOffset -= lastOdometryData;
  PLOT("module:MotionLogDataProvider:odometryOffsetX", odometryOffset.translation.x);
  PLOT("module:MotionLogDataProvider:odometryOffsetY", odometryOffset.translation.y);
  PLOT("module:MotionLogDataProvider:odometryOffsetRotation", toDegrees(odometryOffset.rotation));
  lastOdometryData = groundTruthOdometryData;
#endif
}

bool MotionLogDataProvider::handleMessage(InMessage& message)
{
  return theInstance && theInstance->handleMessage2(message);
}

bool MotionLogDataProvider::isFrameDataComplete()
{
  if(!theInstance)
    return true;
  else if(theInstance->frameDataComplete)
  {
    OUTPUT(idLogResponse, bin, '\0');
    theInstance->frameDataComplete = false;
    return true;
  }
  else
    return false;
}

bool MotionLogDataProvider::handleMessage2(InMessage& message)
{
  switch(message.getMessageID())
  {
  HANDLE(BoardInfo)
  HANDLE2(JointData,
    ALLOC(FrameInfo)
    ((FrameInfo&) *representationBuffer[idFrameInfo]).time = ((JointData&) *representationBuffer[idJointData]).timeStamp;
  )
  HANDLE(KeyStates)
  HANDLE2(OdometryData,
    if(!theGroundTruthOdometryData)
      theGroundTruthOdometryData = new GroundTruthOdometryData;
    *theGroundTruthOdometryData = (GroundTruthOdometryData&) *representationBuffer[idOdometryData];
  )
  HANDLE(SensorData)
  HANDLE(FilteredSensorData)

  case idProcessFinished:
    frameDataComplete = true;
    return true;

  default:
    return false; 
  }
}

MAKE_MODULE(MotionLogDataProvider, Infrastructure)
