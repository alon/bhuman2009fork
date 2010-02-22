/**
 * @file Platform/linux/ProcessFramework.cpp
 *
 * This file implements classes corresponding to the process framework.
 *
 * @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
 */
#include "Platform/ProcessFramework.h"

void PlatformProcess::nextFrame()
  {
    int frameTime = processMain();
    if(getFirstSender())
      getFirstSender()->finishFrame();
    if(getFirstReceiver())
      getFirstReceiver()->finishFrame();
    resetEventMask();
    int currentTime = SystemCall::getCurrentSystemTime();
    if(frameTime < 0)
    {
      int toWait = lastTime - frameTime - currentTime;
      if(toWait < 0)
      {
        toWait = 0;
        lastTime = currentTime;
      }
      else
        lastTime -= frameTime;
      sleepUntil = currentTime + toWait;
    }
    else if(frameTime > 0)
      sleepUntil = currentTime + frameTime;
    else
      sleepUntil = 0;
    setBlockingId(31,frameTime != 0);
  }

/*
 * ProcessCreatorBase::list must be initialized before any code generated
 * by MAKE_PROCESS is executed. Therefore, early initialization is forced.
 */

std::list<ProcessCreatorBase*> ProcessCreatorBase::list
   __attribute__((init_priority(60000)));
// Note: the default priority for initialization of static objects
// using 'gcc' is 65535 (lowest), the highest is 101.

void PlatformProcess::setBlockingId(int id,bool block)
{
  if(block)
    blockMask |= 1 << id;
  else
    blockMask &= ~(1 << id);
}

void PlatformProcess::setEventId(int id)
{
  eventMask |= 1 << id; // id == 0 -> Always accept theDebugReceiver
  if(!id || (blockMask && (eventMask & blockMask)))
  {
    blockMask = 0;
    nextFrame();
  }
}
