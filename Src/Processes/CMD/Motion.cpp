/** 
* @file Processes/CMD/Motion.cpp
* Implementation of a class that represents the process that sends commands to the robot at 50Hz.
*/
#include "Motion.h"
#include "Modules/MotionModules.h"

Motion::Motion() :
INIT_DEBUGGING,
INIT_RECEIVER(CognitionToMotion, false),
INIT_SENDER(MotionToCognition, false),
moduleManager("Motion")
{
  theDebugReceiver.setSize(200000);
  theDebugSender.setSize(20000);
  theMotionToCognitionSender.moduleManager = theCognitionToMotionReceiver.moduleManager = &moduleManager;
  if(SystemCall::getMode() == SystemCall::physicalRobot)
    setPriority(80);
}

void Motion::init()
{
  moduleManager.load();
  BH_TRACE_INIT("Motion");
}

void Motion::terminate()
{
  moduleManager.destroy();
  Process::terminate();
}

int Motion::main() 
{
  // required to detect whether any messages are sent in this frame
  int streamSize = theDebugSender.getStreamedSize();
 
  if (MotionLogDataProvider::isFrameDataComplete())
  {
    NaoProvider::prepareFrame();
    STOP_TIME_ON_REQUEST("Motion", moduleManager.execute(); );
    NaoProvider::finishFrame();

    DEBUG_RESPONSE("automated requests:DrawingManager", OUTPUT(idDrawingManager, bin, Global::getDrawingManager()); );  
    DEBUG_RESPONSE("automated requests:DrawingManager3D", OUTPUT(idDrawingManager3D, bin, Global::getDrawingManager3D()); );  
    DEBUG_RESPONSE("automated requests:StreamSpecification", OUTPUT(idStreamSpecification, bin, Global::getStreamHandler()); );

    theMotionToCognitionSender.timeStamp = SystemCall::getCurrentSystemTime();
    theMotionToCognitionSender.send();

    if(theDebugSender.getStreamedSize() != streamSize)
    { // messages were sent in this frame -> send process finished
      OUTPUT(idProcessFinished, bin, 'm');
    }
    theDebugSender.send();
  }

  int delay = -20;
  MODIFY("motion:delay", delay);
  return delay;
}

bool Motion::handleMessage(InMessage& message)
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
    return MotionLogDataProvider::handleMessage(message) ||
           SpecialActions::handleMessage(message) ||
           Process::handleMessage(message);
  }
}

MAKE_PROCESS(Motion);
