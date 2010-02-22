/**
 * @file Platform/SimRobotQt/ProcessFramework.cpp
 *
 * This file implements classes corresponding to the process framework.
 *
 * @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
 */
#include "ControllerQt/RoboCupCtrl.h"
#include "Platform/ProcessFramework.h"
#include "Platform/GTAssert.h"

/*
 * ProcessCreatorBase::list must be initialized before any code generated
 * by MAKE_PROCESS is executed. Therefore, early initialization is forced.
 */
#ifdef _WIN32
#pragma warning(disable:4073)
#pragma init_seg(lib)
std::list<ProcessCreatorBase*> ProcessCreatorBase::list;
#else
std::list<ProcessCreatorBase*> ProcessCreatorBase::list
   __attribute__((init_priority(60000)));
// Note: the default priority for initialization of static objects
// using 'gcc' is 65535 (lowest), the highest is 101.
#endif

void PlatformProcess::setBlockingId(int id,bool block)
{
  if(block)
    blockMask |= 1 << id;
  else
    blockMask &= ~(1 << id);
}

void PlatformProcess::setEventId(int id)
{
  eventMask |= 1 << id;
  if(blockMask && (eventMask & blockMask))
  {
    blockMask = 0;
    nextFrame();
  }
}

void PlatformProcess::sendBroadcastPackage(const char* package, unsigned size)
{
  RoboCupCtrl::getController()->broadcastPackage(package, size);
}

void PlatformProcess::setBroadcastReceiver(BroadcastReceiver* broadcastReceiver, void (BroadcastReceiver::*onReceive)(const char*))
{
  this->broadcastReceiver = broadcastReceiver;
  this->onReceive = onReceive;
}

void PlatformProcess::setBroadcastPackage(const char* package)
{
  SYNC;
  packages.push_back(package);
}

void PlatformProcess::deliverBroadcastPackages()
{
  SYNC;
  for(std::list<const char*>::iterator i = packages.begin(); i != packages.end(); ++i)
  {
    ASSERT(broadcastReceiver);
    broadcastReceiver->onReceive(*i);
    delete *i;
  }
  packages.clear();
}

void PlatformProcess::nextFrame()
{
  deliverBroadcastPackages();
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
