/**
 * @file ControllerQt/RoboCupCtrl.cpp
 *
 * This file implements the class RoboCupCtrl.
 *
 * @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
 * @author <A href="mailto:kspiess@tzi.de">Kai Spiess</A>
 */
#include "RoboCupCtrl.h"
#include "RobotConsole.h"
#include "Tools/Streams/InStreams.h"
#include "Platform/GTAssert.h"
#include "Tools/Settings.h"
#include <SimRobotCore/Simulation/SimObject.h>
#include <SimRobotCore/Tools/Surface.h>

RoboCupCtrl* RoboCupCtrl::controller = 0;

std::string RoboCupCtrl::robotName;

RoboCupCtrl::RoboCupCtrl() : simTime(false), dragTime(true), lastTime(0), joystickID(0)
{
  controller = this;
  time = 10000 - SystemCall::getRealSystemTime();

  obj = 0;
  SimObject* group = getObjectReference("RoboCup.robots");
  for(unsigned currentRobot = 0; currentRobot < group->getNumberOfChildNodes(); ++currentRobot)
  {
    obj = group->getChildNode(currentRobot);
    robotName = obj->getFullName();
    robots.push_back(new Robot(obj->getName().c_str(), obj));
  }

  if(getObjectReference("RoboCup.robots")->getNumberOfChildNodes())
    ball = getObjectReference("RoboCup.balls.ball");
  else
    ball = 0;
  
  setGUIUpdateDelay(100);
}

void RoboCupCtrl::start()
{
  for(std::list<Robot*>::iterator i = robots.begin(); i != robots.end(); ++i)
    (*i)->start();
#ifdef _WIN32
  SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_IDLE);
#endif
}

void RoboCupCtrl::stop()
{
  for(std::list<Robot*>::iterator i = robots.begin(); i != robots.end(); ++i)
    (*i)->announceStop();
  if(!robots.empty())
    SystemCall::sleep(1000);
  for(std::list<Robot*>::iterator i = robots.begin(); i != robots.end(); ++i)
    delete *i;
  controller = 0;
}

void RoboCupCtrl::execute()
{
  if(dragTime)
  {
    unsigned t = SystemCall::getRealSystemTime();
    if(lastTime + 20 > t)
    {
      SystemCall::sleep(lastTime + 20 - t);
      lastTime += 20;
    }
    else if(lastTime + 40 > t)
      lastTime += 20;
    else
      lastTime = t;
  }

  statusText = "";
  for(std::list<Robot*>::iterator i = robots.begin(); i != robots.end(); ++i)
    (*i)->update();
  if(simTime)
    time += 20;
}

const char* RoboCupCtrl::getRobotName() const
{
  unsigned threadId = Thread<ProcessBase>::getCurrentId();
  for(std::list<Robot*>::const_iterator i = robots.begin(); i != robots.end(); ++i)
    for(ProcessList::const_iterator j = (*i)->begin(); j != (*i)->end(); ++j)
      if((*j)->getId() == threadId)
        return (*i)->getName();
  return obj ? obj->getName().c_str() : "Robot1";
}

std::string RoboCupCtrl::getModelName() const
{
 return "Nao";
}

unsigned RoboCupCtrl::getTime() const
{
  if(simTime)
    return unsigned(time);
  else
    return unsigned(SystemCall::getRealSystemTime() + time);
}

void RoboCupCtrl::broadcastPackage(const char* package, unsigned size)
{
  for(std::list<Robot*>::const_iterator i = robots.begin(); i != robots.end(); ++i)
    if((*i)->listensForBroadcastPackages())
    {
      char* buf = new char[size];
      memcpy(buf, package, size);
      (*i)->setBroadcastPackage(buf);
    }
}
