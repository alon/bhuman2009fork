/** 
* @file  Platform/linux/SystemCall.cpp
* @brief static class for system calls, Linux implementation
* @attention this is the Linux implementation
*
* @author <A href=mailto:brunn@sim.informatik.tu-darmstadt.de>Ronnie Brunn</A>
* @author <A href=mailto:martin@martin-loetzsch.de>Martin Lötzsch</A>
* @author <A href=mailto:risler@sim.informatik.tu-darmstadt.de>Max Risler</A>
* @author <a href=mailto:dueffert@informatik.hu-berlin.de>Uwe Dffert</a>
*/

#include "Platform/SystemCall.h"
#include "Platform/GTAssert.h"
#include <sys/timeb.h>
#include <unistd.h>
#include <string>
#include <sys/sysinfo.h>
using namespace std;

unsigned SystemCall::getCurrentSystemTime() 
{
  return getRealSystemTime();
}

unsigned SystemCall::getRealSystemTime() 
{
  static unsigned base = 0;
  timeb sysTime;
  ftime(&sysTime);
  unsigned time = unsigned(sysTime.time * 1000 + sysTime.millitm);
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
  return physicalRobot;
}

void SystemCall::sleep(unsigned long ms)
{
  usleep(1000 * ms);
}

void SystemCall::getLoad(float& mem, float load[3])
{
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
}

