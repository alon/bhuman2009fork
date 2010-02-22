/** 
* @file  Platform/SimRobotQt/SystemCall.cpp
* @brief static class for system calls
* @attention this is the SimRobotQt implementation
*/

#include "SystemCall.h"
#include <ControllerQt/ConsoleRoboCupCtrl.h>
#include <sys/timeb.h>
#ifndef _WIN32
#include <sys/sysinfo.h>
#endif

unsigned SystemCall::getCurrentSystemTime() 
{
  if(RoboCupCtrl::getController())
    return RoboCupCtrl::getController()->getTime();
  else
    return getRealSystemTime();
}

unsigned SystemCall::getRealSystemTime() 
{
#ifdef _WIN32
  unsigned time = timeGetTime();
#else
  timeb sysTime;
  ftime(&sysTime);
  unsigned time = unsigned(sysTime.time * 1000 + sysTime.millitm);
#endif
  static unsigned base = 0;
  if(!base)
    base = time - 10000; // avoid time == 0, because it is often used as a marker
  return time - base;
}

const char* SystemCall::getHostName()
{
  static char buf[100];
  VERIFY(!gethostname(buf, sizeof(buf)));
  return buf;
}

SystemCall::Mode SystemCall::getMode()
{
  return ConsoleRoboCupCtrl::getMode();
}

void SystemCall::sleep(unsigned int ms)
{
#ifdef _WIN32
  Sleep(ms);
#else
  usleep(ms * 1000);
#endif
}

void SystemCall::getLoad(float& mem, float load[3])
{
#ifdef _WIN32
  load[0] = load[1] = load[2] = -1.f; //Not implemented yet
  MEMORYSTATUS memStat;
  memStat.dwLength = sizeof(MEMORYSTATUS);
  GlobalMemoryStatus(&memStat);
  mem = float(memStat.dwMemoryLoad) / 100.f;
#else
  struct sysinfo info;
  if(sysinfo(&info) == -1)
    load[0] = load[1] = load[2] = mem = -1.f;
  else
  {
    load[0] = float(info.loads[0]) / 100000.f;
    load[1] = float(info.loads[1]) / 100000.f;
    load[2] = float(info.loads[2]) / 100000.f;
    mem = float(info.totalram - info.freeram) / float(info.totalram);
  }
#endif
}

