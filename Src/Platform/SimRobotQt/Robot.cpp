/**
 * @file Platform/SimRobotQt/Robot.cpp
 *
 * This file implements the class Robot.
 *
 * @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
 */

#include "Platform/SimRobotQt/LocalRobot.h"
#include "Robot.h"
#include "Tools/Streams/InStreams.h"

Robot::Robot(const char* name, SimObject* obj)
: name(name)
{
  this->obj = obj;

  // open connect.cfg for CMD process layout
  InConfigFile stream("Processes/CMD/connect.cfg");
  ASSERT(stream.exists());

  // attach receivers to senders
  std::string senderName,
              receiverName;
  while(!stream.eof())
  {
    stream >> senderName >> receiverName;
    connect(getSender(senderName.c_str()), getReceiver(receiverName.c_str()));
  }

  // connect additional senders and receivers
  connect(getSender("Debug.Sender.MessageQueue.S"),getReceiver("LocalRobot.Receiver.MessageQueue.O"));
  connect(getSender("LocalRobot.Sender.MessageQueue.S"),getReceiver("Debug.Receiver.MessageQueue.O"));

  robotProcess = 0;
  for(ProcessList::const_iterator i = begin(); i != end() && !robotProcess; ++i)
    robotProcess = (LocalRobot*) (*i)->getProcess("LocalRobot");
  ASSERT(robotProcess);

  broadcastReceiver = 0;
  for(ProcessList::const_iterator i = begin(); i != end() && !broadcastReceiver; ++i)
    broadcastReceiver = (*i)->getBroadcastReceiver();
}

void Robot::update()
{
  robotProcess->update();
}

std::string Robot::getModel() const
{
  return "Nao";
}

void Robot::setBroadcastPackage(const char* package)
{
  ASSERT(broadcastReceiver);
  broadcastReceiver->setBroadcastPackage(package);
}

void Robot::connect(SenderList* sender, ReceiverList* receiver)
{
  if(sender && receiver)
    sender->add(receiver);
}

SenderList* Robot::getSender(const char* senderName)
{
  char name[NAME_LENGTH_MAX];
  for(ProcessList::const_iterator i = begin(); i != end(); ++i)
  {
    if(senderName[0] == '.')
      strcpy(name,(*i)->getName());
    else
      name[0] = 0;
    strcat(name, senderName);
    SenderList* sender = (*i)->lookupSender(name);
    if(sender)
      return sender;
  }
#ifdef _WIN32 // TODO: define TRACE(format, ...) somewhere and use it here (or is it already defined?)
  OutputDebugString(senderName);
  OutputDebugString(" not found\n");
#else
  fprintf(stderr, "%s not found\n", senderName);
#endif
  return 0;
}

ReceiverList* Robot::getReceiver(const char* receiverName)
{
  for(ProcessList::const_iterator i = begin(); i != end(); ++i)
  {
    ReceiverList* receiver = (*i)->lookupReceiver(receiverName);
    if(receiver)
      return receiver;
  }
#ifdef _WIN32 // TODO: define TRACE(format, ...) somewhere and use it here (or is it already defined?)
  OutputDebugString(receiverName);
  OutputDebugString(" not found\n");
#else
  fprintf(stderr, "%s not found\n", receiverName);
#endif
  return 0;
}
