/**
* @author Aldebaran Robotics 
* Aldebaran Robotics (c) 2007 All Rights Reserved - This file is confidential.\n
*
* Version : $Id$
*/

#ifndef   	TIME_WINU_H_
# define   	TIME_WINU_H_

# include "configcore.h"

/**
 * missing function under windows and mac
 */
# if (TARGET_HOST != TARGET_HOST_LINUX)

#  include "altimeval.h"
#  include "rttime.h"
#  include "alxplatform.h"
#  define CLOCK_MONOTONIC (0)
#  define CLOCK_REALTIME (0)
#  define TIMER_ABSTIME (0)

inline int clock_getres(int clk, struct timespec *tm)
{
  return 0;
}

inline int clock_gettime(int clk, struct timespec *tm)
{
  struct timeval tv;
  gettimeofday(&tv, 0);
  tm->tv_sec = tv.tv_sec;
  tm->tv_nsec = tv.tv_usec * 1000;
  return 0;
}

inline int clock_nanosleep(int clk, int flag, const struct timespec *tm, struct timespec *tm2)
{
  struct timespec t1;
  long diff;
  int diffms = 0;

  clock_gettime(0, &t1);

//  printf("T1: (%ld,%ld)\n", (&t1)->tv_sec, (&t1)->tv_nsec);
//  printf("T2: (%ld,%ld)\n", tm->tv_sec, tm->tv_nsec);
  diff = timespec_diff_ns(*tm, t1);
  diffms = diff / 1000 / 1000;
//printf("diff(%ld): %ld\n", diff, diffms);
  if (diffms > 0)
    SleepMs(diffms);
  return 0;
}

# endif

/**
 * missing function under windows
 */
# if (TARGET_HOST == TARGET_HOST_WINDOWS)

inline int nanosleep(const struct timespec *tm, const struct timespec *tm2)
{
  SleepMs(tm->tv_nsec * 1000 * 1000);
  return 0;
}
# endif

#endif
