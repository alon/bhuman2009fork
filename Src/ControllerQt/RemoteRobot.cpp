/**
* @file ControllerQt/RemoteRobot.cpp
* Implementation of the base class of processes that communicate with a remote robot.
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
*/

#include "RemoteRobot.h"
#include "ConsoleRoboCupCtrl.h"
#include "Platform/itoa.h"

RemoteRobot::RemoteRobot(const char* name, const char* ip) :
RobotConsole(theDebugReceiver,theDebugSender),
theDebugReceiver(this,"Receiver.MessageQueue.O",false),
theDebugSender(this,"Sender.MessageQueue.S",false),
bytesTransfered(0),
transferSpeed(0),
timeStamp(0)
{
  strcpy(this->name, name);
  strcpy(this->ip, ip);
  mode = SystemCall::remoteRobot;

  // try to connect for one second
  Thread<RemoteRobot>::start(this, &RemoteRobot::connect);
  Thread<RemoteRobot>::stop();
}

void RemoteRobot::connect()
{
  TcpConnection::connect(*ip ? ip : 0, 0xA1BD, TcpConnection::sender);
}

void RemoteRobot::run()
{
  while(isRunning())
    SystemCall::sleep(processMain());
}

int RemoteRobot::main() 
{
  unsigned char* sendData = 0,
               * receivedData;
  int sendSize = 0,
      receivedSize = 0;
  MessageQueue temp;

  {
    // If there is something to send, prepare a package
    if(!theDebugSender.isEmpty())
    {
      SYNC;
      OutBinarySize size;
      size << theDebugSender;
      sendSize = size.getSize();
      sendData = new unsigned char[sendSize];
      OutBinaryMemory stream(sendData);
      stream << theDebugSender;
      // make backup
      theDebugSender.moveAllMessages(temp);
    }
  }

  // exchange data with the router
  if(!sendAndReceive(sendData,sendSize,receivedData,receivedSize) && sendSize)
  { // sending failed, restore theDebugSender
    SYNC;
    // move all messages since cleared (if any)
    theDebugSender.moveAllMessages(temp);
    // restore
    temp.moveAllMessages(theDebugSender);
  }

  // If a package was prepared, remove it
  if(sendSize)
  {
    delete [] sendData;
    sendSize = 0;
  }

  // If a package was received from the router program, add it to receiver queue
  if(receivedSize > 0)
  {
    SYNC;
    InBinaryMemory stream(receivedData,receivedSize);
    stream >> theDebugReceiver;
    delete [] receivedData;
  }

  return receivedSize > 0 ? 1 : 20;
}

void RemoteRobot::announceStop()
{
  {
    SYNC;
    debugOut.out.bin << DebugRequest("disableAll");
    debugOut.out.finishMessage(idDebugRequest);
  }
  SystemCall::sleep(1000);
  Thread<RemoteRobot>::announceStop();
}

void RemoteRobot::update()
{
  RobotConsole::update();
  
  if(SystemCall::getTimeSince(timeStamp) >= 2000)
  {
    int bytes = this->getOverallBytesSent() + this->getOverallBytesReceived() - bytesTransfered;
    bytesTransfered += bytes;
    timeStamp = SystemCall::getCurrentSystemTime();
    transferSpeed = bytes / 2000.0;
  }

  char buf[20];
  sprintf(buf, "%.1lf kb/s", transferSpeed);
  std::string statusText = robotName.substr(robotName.find_last_of(".") + 1) + ": connected to " + 
                           ip + ", " + buf;

  if(logPlayer.getNumberOfMessages() != 0)
  {
    char buf[10];
    statusText += std::string(", recorded ") + itoa_s(logPlayer.numberOfFrames, buf, sizeof(buf), 10);
  }

  if(pollingFor)
  {
    statusText += statusText != "" ? ", polling for " : "polling for ";
    statusText += pollingFor;
  }

  ctrl->printStatusText(statusText);
}
