/**
 * @file Platform/SimRobotQt/Receiver.cpp
 *
 * This file implements classes related to receivers.
 *
 * @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
 */
#include "Platform/ProcessFramework.h"
#include "Platform/GTAssert.h"

ReceiverList::ReceiverList(PlatformProcess* p,const char* receiverName,bool blocking)
{
  process = p;
  eventId = process->getNextId();
  // copy the receiver's name. The name of the process is still missing.
  strcpy(this->name,receiverName);
  if(getFirst())
  {
    ReceiverList* p = getFirst();
    while(p->next)
      p = p->next;
    p->next = this;
  }
  else
    getFirst() = this;
  next = 0;
  this->blocking = blocking;
  package = 0;
}

ReceiverList*& ReceiverList::getFirst()
{
  return process->getFirstReceiver();
}

bool ReceiverList::receivedNew() const
{
  return (process->getEventMask() & 1 << eventId) != 0;
}

void ReceiverList::finishFrame()
{
  for(ReceiverList* p = getFirst(); p; p = p->getNext())
    process->setBlockingId(p->eventId,p->blocking);
}

void ReceiverList::checkAllForPackages()
{
  for(ReceiverList* p = getFirst(); p; p = p->getNext())
    p->checkForPackage();
}

ReceiverList* ReceiverList::lookup(const char* processName,const char* receiverName)
{
  for(ReceiverList* p = getFirst(); p; p = p->getNext())
  {
    char buf[80];
    ASSERT(strlen(processName) + strlen(p->name) + 1 < sizeof(buf));
    strcpy(buf,processName),
    strcat(buf,".");
    strcat(buf,p->name);
    if(!strcmp(buf,receiverName))
      return p;
  }
  return 0;
}
