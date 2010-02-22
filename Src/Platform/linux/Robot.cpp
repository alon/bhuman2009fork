/**
* @file Platform/linux/Robot.cpp
* This file implements a class that implements a robot as a list of processes.
* @author <a href="mailto:Bernd.Gersdorf@dfki.de">Bernd Gersdorf</a>
*/

#include "Robot.h"
#include "Tools/Streams/InStreams.h"

Robot::Robot()
{

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
  return 0;
}
