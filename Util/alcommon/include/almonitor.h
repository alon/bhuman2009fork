/**
* @author Aldebaran Robotics 
* Aldebaran Robotics (c) 2007 All Rights Reserved - This file is confidential.\n
*
* Version : $Id$
*/



#ifndef AL_MONITORING_H_
# define AL_MONITORING_H_

#include "alxplatform.h"
#include "alptr.h"
#include "altimeval.h"
#include "almutex.h"

// External headers
# include <pthread.h>

namespace AL
{
  class ALTask;
  class ALThread;
  class ALThreadPool;
  class ALMonitor
  {
  public:

    /**
    * \brief Constructor. Init cycle, threshold, offset and reference to pool.
    *
    * Intialize all variables : cycle, threshold (in seconds), offset and reference to the pool.
    */

    ALMonitor(int pUpdateCycle,
      int pUpdateThreshold,
      int pUdpateOffset,
      ALThreadPool *pThreadPool);

    /**
    * \brief Destructor. Nothing to do.
    *
    * Nothing to do.
    */

    ~ALMonitor();

    /**
    * \brief assimilate. Take a task, read datas and update stats.
    *
    * Assimilate take a task and read idle and execution time of the task. Adjust the
    * number of thread according to these stats.
    *
    * @param pTask task to study.
    */

    void assimilate(ALPtr<ALTask> pTask);

    /**
    * \brief adjustPool. Readjust the thread number according to its stats.
    *
    * This function will adjust the number of threads in the pool according to the
    * needs of the program. See the documentation to know which algorithm is used.
    */

    void adjustPool();

  private:

    //! Associated Thread Pool
    ALThreadPool *fThreadPool;

    //! Mutex for task assimilation
    ALPtr<ALMutex> fAssimilator;

    //! Number of cycle between each thread adjustment
    int fUpdateCycle;

    //! Threshold from which an update is done (in seconds of idle time)
    int fUpdateThreshold; 

    //! Number of thread added or removed at each adjustment
    int fUpdateOffset;

    //! Counter of cycle
    int fCounterCycle;

    // int fTaskIdle; // WARNING: limits to 1h11
    struct timeval fTaskIdle;

    int fThreadIdle;
  };
} //namespace AL
#endif // !AL_MONITORING_H_
