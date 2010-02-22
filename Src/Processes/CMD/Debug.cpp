/** 
* @file Processes/CMD/Debug.cpp
* 
* Implementation of class Debug.
*
* @author Martin L�tzsch
*/

#include "Debug.h"
#include "Tools/Debugging/Debugging.h"
#include "Platform/GTAssert.h"
#include "Platform/SystemCall.h"

Debug::Debug() : 
INIT_EXTERNAL_DEBUGGING,

INIT_DEBUG_RECEIVER(Cognition),
INIT_DEBUG_RECEIVER(Motion),

INIT_DEBUG_SENDER(Cognition),
INIT_DEBUG_SENDER(Motion),

sendTime(0),
processIdentifier(0),
fout(0)
{
  theDebugSender.setSize(MAX_PACKAGE_SEND_SIZE - 2000);
  theDebugReceiver.setSize(MAX_PACKAGE_RECEIVE_SIZE - 2000);
  theCognitionReceiver.setSize(2160000);
  theCognitionSender.setSize(400000);
  
  theMotionReceiver.setSize(40000);
  theMotionSender.setSize(200000);
  if(SystemCall::getMode() == SystemCall::physicalRobot)
    setPriority(5);
}

int Debug::main() 
{
  // Copying messages from debug queues from cognition and motion
  switch( outQueueMode.behavior)
  {
  case QueueFillRequest::sendCollected:
  case QueueFillRequest::discardNew:
  case QueueFillRequest::discardAll:
    // Discard new messages
    theCognitionReceiver.clear();
    theMotionReceiver.clear();
    break;

  default:
    // Move the messages from other processes' debug queues to the outgoing queue
    if(!theCognitionReceiver.isEmpty())
    {
      OUTPUT(idProcessBegin, bin, 'c');
      theCognitionReceiver.moveAllMessages(theDebugSender);
    }
    if(!theMotionReceiver.isEmpty())
    {
      OUTPUT(idProcessBegin, bin, 'm');
      theMotionReceiver.moveAllMessages(theDebugSender);
    }
  }

  // Handing behaviour
  bool sendNow = false,
       sendToGUI = false;
  switch( outQueueMode.behavior)
  {
  case QueueFillRequest::sendAfter:
    if( SystemCall::getCurrentSystemTime() > sendTime)
    {
      // Send messages that are in the queue (now matter how long it takes), but don't take new messages
      sendNow = true;
      outQueueMode.behavior = QueueFillRequest::sendCollected;
    }
    break;

  case QueueFillRequest::sendEvery:
    if( SystemCall::getCurrentSystemTime() > sendTime)
    {
      // Send now (if the network is busy, this send time is effectively skipped)
      sendNow = true;

      // Compute time for next sending
      sendTime = SystemCall::getCurrentSystemTime() + outQueueMode.timingMilliseconds;
    }
    break;

  case QueueFillRequest::collect:
  case QueueFillRequest::discardNew:
    // Don't send now
    break;

  case QueueFillRequest::discardAll:
    // Clear output queue
    theDebugSender.clear();
    break;

  case QueueFillRequest::sendImmediately:
  case QueueFillRequest::sendCollected:
  default:
    sendNow = true;
  }

  if(sendNow)
  {
    // Apply filter
    switch( outQueueMode.filter)
    {
    case QueueFillRequest::latestOnly:
      // Send only latest of each type
      theDebugSender.removeRepetitions();
      break;

    case QueueFillRequest::sendEverything:
    default:
      ; // Do nothing
    }

    // Send or save
    switch( outQueueMode.target)
    {
    case QueueFillRequest::writeToStick:
      if( !theDebugSender.isEmpty())
      {
        if(!fout)
        {
          fout = new OutBinaryFile("logfile.log");
          theDebugSender.writeAppendableHeader(*fout);
        }
        // Append the outgoing queue to the file on the memory stick
        theDebugSender.append(*fout);
        theDebugSender.clear();
      }
      break;

    case QueueFillRequest::sendViaNetwork:
    default:
#if defined(LINUX)
      if (messageWasReceived) 
#endif
        sendToGUI = true;
      break;
    }
  }

  // Send messages to the processes
  theCognitionSender.send();
  theMotionSender.send();

  DO_EXTERNAL_DEBUGGING(sendToGUI);

#ifndef RELEASE
  return 1; // Wait at least 1 millisecond, then recall main()
#else
  return 0; // do not recall main()
#endif
}

void Debug::init()
{
  // read requests.dat
  InBinaryFile stream("requests.dat");
  if (stream.exists() && !stream.eof())
    stream >> theDebugReceiver;

  theDebugReceiver.handleAllMessages(*this);
  theDebugReceiver.clear();
  messageWasReceived = false;
}

bool Debug::handleMessage(InMessage& message)
{
  messageWasReceived = true;
  
  switch(message.getMessageID())
  {
  case idText: // loop back to GUI
    message >> theDebugSender;
    return true;

  // messages to Cognition
  case idColorTable64:
  case idWriteColorTable64:
    message >> theCognitionSender;
    return true;

  // messages to Motion
  case idMotionNet:
    message >> theMotionSender; 
    return true;

  // messages to Debug
  case idQueueFillRequest:
    // Read message queue settings and compute time when next to send (if in a timed mode)
    message.bin >> outQueueMode;
    sendTime = SystemCall::getCurrentSystemTime() + outQueueMode.timingMilliseconds;
    if(fout)
    {
      delete fout;
      fout = 0;
    }
    return true;

  // messages to Cognition and Motion
  case idModuleRequest:
  case idXabslDebugRequest:
  case idXabslIntermediateCode:
  case idDebugDataChangeRequest:
    message >> theCognitionSender; 
    message >> theMotionSender; 
    return true;

  // messages to all processes
  case idDebugRequest:
    message >> theCognitionSender; 
    message >> theMotionSender; 
    return Process::handleMessage(message);

  case idProcessBegin:
    message.bin >> processIdentifier;
    message.resetReadPosition();
    // no break

  default:
    if(processIdentifier == 'm')
      message >> theMotionSender;
    else
      message >> theCognitionSender;
      
    return true;
  }
}

MAKE_PROCESS(Debug);
