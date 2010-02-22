/**
* @file Processes/CMD/Cognition.cpp
* Implementation of a class that represents a process that receives data from the robot at about 15 Hz.
*/

#include "Cognition.h"
#define HAVE_BOOLEAN
#include "Modules/CognitionModules.h"
#include "Platform/GTAssert.h"
#include "Tools/Settings.h"
#include "Tools/Team.h"

Cognition::Cognition() : 
INIT_DEBUGGING,
INIT_RECEIVER(MotionToCognition, false),
INIT_SENDER(CognitionToMotion, false),
INIT_TEAM_COMM,
moduleManager("Cognition")
{
  theDebugSender.setSize(2160000);
  theDebugReceiver.setSize(400000);
  theTeamSender.setSize(1396); // 1 package without checksum and size
  theTeamReceiver.setSize(4 * 1450); // >= 4 packages
  theCognitionToMotionSender.moduleManager = theMotionToCognitionReceiver.moduleManager = &moduleManager;
}

void Cognition::init()
{
  Global::theTeamOut = &theTeamSender.out;
  START_TEAM_COMM;
  moduleManager.load();
  BH_TRACE_INIT("Cognition");
}

void Cognition::terminate()
{
  moduleManager.destroy();
  Process::terminate();
}

int Cognition::main() 
{
  RECEIVE_TEAM_COMM;

  // required to detect whether any messages are sent in this frame
  int streamSize = theDebugSender.getStreamedSize();

  if(CameraProvider::isNewImageAvailable() &&
     CognitionLogDataProvider::isFrameDataComplete())
  {
    // There must not be any TEAM_OUTPUT before this in each frame.
    TeamDataProvider::handleMessages(theTeamReceiver);

    // Reset coordinate system for debug field drawing
    DECLARE_DEBUG_DRAWING("origin:Reset", "drawingOnField"); // Set the origin to the (0,0,0)
    ORIGIN("origin:Reset", 0.0, 0.0, 0.0);

    STOP_TIME_ON_REQUEST("Cognition", moduleManager.execute(); );

    DEBUG_RESPONSE("process:Cognition:jointDelay", 
      if(&Blackboard::theInstance->theFrameInfo && &Blackboard::theInstance->theFilteredJointData)
      {
        OUTPUT(idText, text, Blackboard::theInstance->theFrameInfo.getTimeSince(Blackboard::theInstance->theFilteredJointData.timeStamp));
      }
    );

    DEBUG_RESPONSE("automated requests:DrawingManager", OUTPUT(idDrawingManager, bin, Global::getDrawingManager()); );  
    DEBUG_RESPONSE("automated requests:DrawingManager3D", OUTPUT(idDrawingManager3D, bin, Global::getDrawingManager3D()); );  
    DEBUG_RESPONSE("automated requests:StreamSpecification", OUTPUT(idStreamSpecification, bin, Global::getStreamHandler()); );

    theCognitionToMotionSender.timeStamp = SystemCall::getCurrentSystemTime();
    theCognitionToMotionSender.send();

    // Simulator polls for color table and will block if non is available. So make one available by default.
    if(!&Blackboard::theInstance->theColorTable64)
    {
      DEBUG_RESPONSE("representation:ColorTable64",
        ColorTable64* ct = new ColorTable64;
        OUTPUT(idColorTable64, bin, *ct);
        delete ct;
      );
    }

    if(&Blackboard::theInstance->theFilteredSensorData && Global::getReleaseOptions().sensorData)
    {
      SensorData s = Blackboard::theInstance->theFilteredSensorData;
      TEAM_OUTPUT(idSensorData, bin, s);
    }

    if(!theTeamSender.isEmpty())
    {
      if(&Blackboard::theInstance->theTeamMateData && Blackboard::theInstance->theTeamMateData.sendThisFrame)
      {
        TEAM_OUTPUT(idSendTimeStamp, bin, SystemCall::getCurrentSystemTime());
        SEND_TEAM_COMM;
      }
      theTeamSender.clear(); // team messages are purged even when not sent.
    }

    if(theDebugSender.getStreamedSize() != streamSize)
    { // messages were sent in this frame -> send process finished
      OUTPUT(idProcessFinished, bin, 'c');
    }
    theDebugSender.send();
  }
#ifndef RELEASE
  else if(Global::getDebugRequestTable().poll) 
    --Global::getDebugRequestTable().pollCounter;
#endif
#if TARGET_ROBOT
  // Wait a few milliseconds and check again for new image.
  return 1;
#else
  // simulate 30 Hz
  return -33;
#endif
}

bool Cognition::handleMessage(InMessage& message)
{
  switch (message.getMessageID())
  {
  case idModuleRequest:
  {
    unsigned timeStamp;
    message.bin >> timeStamp;
    moduleManager.update(message.bin, timeStamp);
    return true;
  }

  default:
    return CognitionLogDataProvider::handleMessage(message) ||
           CognitionConfigurationDataProvider::handleMessage(message) ||
           BH2009BehaviorControl::handleMessage(message) ||
           Process::handleMessage(message); 
  }
}

MAKE_PROCESS(Cognition);
