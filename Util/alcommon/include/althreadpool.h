/**
 * @author Aldebaran-Robotics
 * Aldebaran Robotics (c) 2007 All Rights Reserved - This file is confidential.\n
 *
 * Version : $Id$
 */


#ifndef AL_THREAD_POOL_H_
# define AL_THREAD_POOL_H_

// Protecting queue inclusion
# ifdef min
#  undef min
# endif

# ifdef max
#  undef max
# endif

// External headers
# include <queue>
# include <iostream>

#include "alxplatform.h"
# include <pthread.h>

// Internal headers
# include "althreadtypes.h"
#include "alcriticalsectionread.h"
#include "alcriticalsectionwrite.h"
#include "alptr.h"
#include "alerror.h"


namespace AL
{
class ALBroker;
class ALTask;
class ALThread;
class ALMonitor;
class ALMutex;
class ALMutexRW;
class ALCondition;

class ALThreadPool: public AnyPtr<ALThreadPool>
{
 public:
	 ALWeakPtr<ALBroker> fParentBroker;

  void init(int pInitialThread, int pMaxThread,
                              int pUpdateCycle, int pUpdateThreshold,
                              int pUpdateOffset);

	 ALPtr<ALBroker> getParentBroker()
	 {
		 ALPtr<ALBroker> returnPtr = fParentBroker.lock();
		 AL_ASSERT(returnPtr != NULL);
		 return returnPtr;
	 }
  /**
   * \brief Constructor. Init the pool of thread, the queue of task and the monitor.
   *
   * The constructor init the queue of thread with pInitialThread threads. It also
   * init the monitor dans make the pool ready to get a task.
   *
   * @param pInitialThread initial number of thread (default is POOL_INITIAL_THREAD).
   * @param pMaxThread maximum number of thread (default is POOL_MAX_THREAD).
   * @param pUpdateCycle number of task between each thread pool adjustment
   *          (default is POOL_UPDATE_CYCLE)
   * @param pUpdateThreshold (in percent) threshold from which an adjustement is done
   *          (default is POOL_UPDATE_THRESHOLD, see monitor documentation)
   * @param pUpdateOffset number of thread created or destroyed at each adjustment.
   *          (default is POOL_UPDATE_OFFSET)
   */

  ALThreadPool (ALPtr<ALBroker> pBroker, int pInitialThread	= POOL_INITIAL_THREAD,
		int pMaxThread		= POOL_MAX_THREAD,
		int pUpdateCycle	= POOL_UPDATE_CYCLE,
		int pUpdateThreshold	= POOL_UPDATE_THRESHOLD,
		int pUpdateOffset	= POOL_UPDATE_OFFSET);


  /**
   * \brief Destructor. Free all memory, destruct dependecies
   *
   * The destructor call the destructor of the monitor
   *
   * @param pKillProcess AL_THREAD_POOL_STOP_KILL (= true) if all working threads must be killed
   *          (default is AL_THREAD_POOL_STOP_WAIT (=false))
   */

  ~ALThreadPool();

  /**
   * \brief shutdown. Shutdown the process, wait or kill all working threads.
   *
   * If pKillProcess is set to AL_THREAD_POOL_STOP_KILL, all working threads are destroyed whereas
   * if it is not set or set to AL_THREAD_POOL_STOP_WAIT, it will wait termination of all threads
   * before deleting them. It also send back waiting tasks.
   * After the shutdown, ThreadPool is ready to be deleted. Any function call
   * behaviour is undefined.
   *
   * @param pKillProcess AL_THREAD_POOL_STOP_KILL (= true) if all working threads must be killed
   *          (default is AL_THREAD_POOL_STOP_WAIT (=false))
   */

  void shutdown( bool pKillProcess = AL_THREAD_POOL_STOP_WAIT );

  /**
   * \brief Enqueue a new task.
   *
   * Enqueue the task and launch the dispatch.
   *
   * @param pTask task to treat.
   * @return the unique ID of the task (maybe to enable pausing later?)
   */

  int enqueue ( ALPtr<ALTask> pTask);

  /**
   * \brief adjustThread. Allow the monitor ONLY to add or remove new threads.
   *
   * Add or remove pNbThread. If some threads must be removed and all are working,
   * the thread pool will wait for a thread to finish and kill it. The parameter
   * is given there if someone wants to make a dynamic adjustment of the update
   * offset.
   * If the new thread number is contained between 0 and max thread, the adjustment
   * is done, whereas nothing is done.
   *
   * @param pNbThread number of thread to add or remove.
   * @return true if the adjustment is done, false otherwise.
   */

  bool adjustThread( int pNbThread );

  /**
   * \brief taskEnded. Treat an ended task.
   *
   * Take the ended task and send it to the monitor. Then put its termination flag.
   * (maybe send a callback?)
   *
   * @param pTask ended task
   */

  void taskEnded(ALPtr<ALTask> pTask);


  /**
   * \brief getTask. Return a waiting task if any.
   *
   * Called by a free Thread, this function return a waiting task if there is one
   * or NULL in order to make the thread wait.
   *
   * @return a pointer to a task
   */

  ALPtr<ALTask> getTask();

  /**
   * \brief apoptosis. Tell a thread if it must kill itself.
   *
   * Thread scheduling implies creating threads when needed but also deleting threads
   * when they are no more needed. This function is called by a thread when its task
   * is finished in order to know if it must kill itself.
   *
   * @param pThread the thread calling the function
   *
   * @return true if the thread should kill itself.
   */

  bool apoptosis (ALPtr<ALThread> pThread);

  /**
   * \brief getNbThread. Return the current number of thread.
   *
   * This function is for diagnostic purpose.
   *
   * @return the number of thread
   */

  int getNbThread() { return fNbThread; }

  /**
   * \brief getNbTask waiting for a thread
   *
   * This function is for diagnostic purpose.
   *
   * @return the number of task
   */
  int getNbTask() { return fWaitingTasks.size(); }


  /**
   * \brief computeSumThreadIdle. Compute the sum of idle time of each thread.
   *
   * WARNING: please don't call it too much often...
   *
   * @return a timeval containing the sum
   */
  struct timeval computeSumThreadIdle( void ); // can't const because mutex use

  void resetSumThreadIdle( void );



  inline void increaseUsedThread()
  {
    ALCriticalSectionWrite sectionNbThread(fUsedThreadMutex);
    fUsedThread++;
  }

  inline void decreaseUsedThread()
  {
    ALCriticalSectionWrite sectionNbThread(fUsedThreadMutex);
    fUsedThread--;
  }

  inline int getUsedThread()
  {
    ALCriticalSectionRead sectionNbThread(fUsedThreadMutex);
    return fUsedThread;
  }

 private:


  /**
   * \brief xIncreaseThread. Increase the number of thread. (same as adjustThread, but simpler, tested and working)
   *
   * @return true if the enlargement is done, false otherwise.
   */

  bool xIncreaseThread( void );

private:

  //! Array of thread
  //ALThread **fThreadPool;
  //ALPtr <ALPtr <ALThread> > fThreadPool;
  std::vector < ALPtr<ALThread> > fThreadPool;

  //! Queue of tasks waiting to be executed
  std::queue< ALPtr<ALTask> > fWaitingTasks;

  //! Number of threads
  int fNbThread;

  //! Max number of threadsb
  int fMaxThread;

  //! Monitor adjusting the pool parameters.
  ALPtr<ALMonitor> fMonitor;

  //! Number of threads to be killed (to be removed)
  int fnbrSuperfluThreads;

  //! fThreadPool Mutex
  const ALPtr<ALMutex>      fThreadPoolMutex;

  //! fThreadPool Mutex
  ALPtr<ALMutexRW>      fUsedThreadMutex;


  //! fWaitingTasks Mutex
  const ALPtr<ALMutex>      fWaitingTasksMutex;

  //! global mutex for waiting
  const ALPtr<ALMutex>      fThreadWait;

  //! global condition to wait
  const ALPtr<ALCondition>  fWaitForTask;

  int fUsedThread;
};
  
}

#endif // !AL_THREAD_POOL_H_
