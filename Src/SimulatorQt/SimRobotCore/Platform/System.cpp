
#ifdef _WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif

#include "System.h"

unsigned int System::getTime()
{
#ifdef _WIN32
  return GetTickCount();
#else
  struct timeval tv;
  gettimeofday(&tv, 0);
  return (unsigned int) (tv.tv_sec * 1000 + tv.tv_usec / 1000);
#endif
}

