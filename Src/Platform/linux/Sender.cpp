/** 
 * @file Platform/linux/Sender.cpp
 *
 * This file implements classes related to senders.
 *
 * @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
 */
#include "Platform/ProcessFramework.h"

SenderList::SenderList(PlatformProcess* p,const char* senderName,bool blocking)
{
  // copy the sender's name. The name of the process is still missing.
  process = p;
  eventId = process->getNextId();
  strcpy(this->name,senderName);
  if(getFirst())
  {
    SenderList* p = getFirst();
    while(p->next)
      p = p->next;
    p->next = this;
  }
  else
    getFirst() = this;
  next = 0;
  this->blocking = blocking;
}

SenderList*& SenderList::getFirst()
{
  return process->getFirstSender();
}

void SenderList::finishFrame()
{
  for(SenderList* p = getFirst(); p; p = p->getNext())
    p->sendPackage();
}

void SenderList::checkAllForRequests()
{
  for(SenderList* p = getFirst(); p; p = p->getNext())
    p->checkForRequest();
}

SenderList* SenderList::lookup(const char* processName,const char* senderName)
{
  for(SenderList* p = getFirst(); p; p = p->getNext())
  {
    char buf[80];
    ASSERT(strlen(processName) + strlen(p->name) + 1 < sizeof(buf));
    strcpy(buf,processName),
    strcat(buf,".");
    strcat(buf,p->name);
    if(!strcmp(buf,senderName))
      return p;
  }
  return 0;
}
