/**
* @file Process.cpp
*
* Implementation of class Process.
*/

#include "Process.h"
#include "Global.h"

Process::Process(MessageQueue& debugIn, MessageQueue& debugOut) 
: debugIn(debugIn), debugOut(debugOut),
  blackboard(new Blackboard) // uses custom new operator
{
  setGlobals(); // In Simulator: in GUI thread
  settings.load();

  initialized = false;
}

Process::~Process()
{
  delete blackboard;
}

int Process::processMain()
{
  if(!initialized)
  {
    setGlobals(); // In Simulator: in separate thread for process
    init();
    initialized = true;
  }
  
#ifndef RELEASE
  debugIn.handleAllMessages(*this);
  debugIn.clear();
#endif

int toReturn = this->main();

#ifndef RELEASE
  if(Global::getDebugRequestTable().poll) 
  {
    if(Global::getDebugRequestTable().pollCounter++ > 10)
    {
      Global::getDebugRequestTable().poll = false;
      OUTPUT(idDebugResponse, text, "pollingFinished");
    }
  }
#endif

  return toReturn;
}

void Process::setGlobals()
{
  Global::theDebugOut = &debugOut.out;
  Global::theSettings = &settings;
  Global::theDebugRequestTable = &debugRequestTable;
  Global::theDebugDataTable = &debugDataTable;
  Global::theStreamHandler = &streamHandler;
  Global::theDrawingManager = &drawingManager;
  Global::theDrawingManager3D = &drawingManager3D;
  Global::theReleaseOptions = &releaseOptions;

  Blackboard::theInstance = blackboard; // blackboard is NOT globally accessible
}

bool Process::handleMessage(InMessage& message)
{
  switch (message.getMessageID())
  {
  case idDebugRequest:
    {
      DebugRequest debugRequest;
      message.bin >> debugRequest;
      Global::getDebugRequestTable().addRequest(debugRequest);
      return true;
    }

  case idDebugDataChangeRequest:
    return Global::getDebugDataTable().processChangeRequest(message.bin);

  default:
    return false;
  }
}
