/**
* @author Aldebaran Robotics 
* Aldebaran Robotics (c) 2007 All Rights Reserved - This file is confidential.\n
*
* Version : $Id$
*/


#ifndef AL_THREAD_TYPES_H_
# define AL_THREAD_TYPES_H_

#define POOL_INITIAL_THREAD 92
#define POOL_MAX_THREAD 184
#define POOL_UPDATE_CYCLE 800
#define POOL_UPDATE_THRESHOLD 500
#define POOL_UPDATE_OFFSET 0

#define AL_THREAD_POOL_STOP_KILL true
#define AL_THREAD_POOL_STOP_WAIT false

namespace AL
{
  class ALTask;
  class ALThread;
  class ALMonitor;
  class ALThreadPool;
}

# ifdef _WIN32
#  include <sys/timeb.h>
#  include <sys/types.h>
#  include <winsock.h>

//int gettimeofday(struct timeval* t,void* timezone);

# else
#  include <sys/time.h>
# endif // _WIN32

// give duration time between two dates (in micro seconds).
// WARNING: limited to duration between -35 min and 35 min (car uint equivaut a 1h11) !!!!!
int duration( const struct timeval & begin, const struct timeval & end );

#endif // !AL_THREAD_TYPES_H_
