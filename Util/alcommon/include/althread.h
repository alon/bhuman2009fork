/**
 * @author Aldebaran-Robotics
 * Aldebaran Robotics (c) 2007 All Rights Reserved - This file is confidential.\n
 *
 * Version : $Id$
 */


#ifndef AL_THREAD_H_
# define AL_THREAD_H_

// External headers
# include <iostream>
# include <pthread.h>
# include "alptr.h"
# include "alerror.h"

// Internal headers
# include "althreadtypes.h"
#include "altimeval.h"

namespace AL
{
  void * _AL_Thread_Pool_Atom (void * pALThread);

  class ALMutex;
  class ALMutexRW;
  class ALCondition;

  class ALThread: public AnyPtr<ALThread>
  {
    friend void * _AL_Thread_Pool_Atom (void * pALThread);
  public:

    /**
     * \brief Constructor. Enqueue the thread in the thread pool.
     *
     * Create the structure and enqueue the thread into the thread pool.
     * Store the unique ID of the thread for its future destruction.
     *
     * @param pThreadPool the thread pool managing this thread.
     * @param pID unique ID of the thread
     * @param pThreadWait mutex blocking a task when it has nothing to do
     * @param pWaitForTask cond associated to the mutex
     */

    ALThread(ALPtr<ALThreadPool> pThreadPool,
	     int pID,
	     ALPtr<ALMutex> pThreadWait,
	     ALPtr<ALCondition> pWaitForTask
	     );

    /**
     * \brief Destructor. Do nothing for the moment.
     *
     * Do nothing.
     *
     * @param pThreadPool the thread pool managing this thread.
     */

    ~ALThread();

    /**
     * \brief launch. Create and launch the pthread
     *
     * Create and launch the pthread
     *
     * @return pthread_create error code
     */

    int launch();

    /**
     * \brief getTask. Ask the pool for a task
     *
     * Give a new task to the thread.
     *
     * @return true if a task is assigned.
     */

    bool getTask();

    /**
     * \brief runTask. Start the new task.
     *
     * Start the new task.
     */

    void runTask();

    /**
     * \brief apoptosis. Asks the pool for dying.
     *
     * Thread scheduling implies creating threads when needed but also deleting threads
     * when they are no more needed. This function asks the pool thread if the thread
     * is still needed or not.
     *
     * @return true if the thread should kill itself.
     */

    bool apoptosis ();

    /**
     * \brief setID. Set a new ID to the thread
     *
     * Since a thread ID is its position in the pool, it can be moved and this ID can
     * change. This function, called ONLY by the thread pool, is here for this purpose
     *
     * @param pID new ID
     */

    void setID(int pID) { fID = pID; }

    /**
     * \brief getThreadID. Return the ThreadID
     *
     * Return the ThreadID
     *
     * @return Thread ID
     */

    pthread_t getThreadID() { return fThreadID; }

    /**
     * \brief getID. Return the ID
     *
     * Return the ID
     *
     * @return ID
     */

    int getID() { return fID; }

    struct timeval GetIdleSum( void ) const
    {
      return fIdleSum;
    }

    void ResetIdleSum( void )
    {
      reset( fIdleSum );
    }

    protected:
    //! Mutex to stop execution of thread if no task
    ALPtr<ALMutex>      fThreadWait;
    //! Blocking condition
    ALPtr<ALCondition>  fWaitForTask;
    // Task Mutex
    ALPtr<ALMutexRW>    fTaskMutex;

    public:
      inline ALPtr<ALThreadPool> getThreadPool(void)
      {
          ALPtr<ALThreadPool> returnPtr = fThreadPool.lock();
          // it can be possible that the task has to thread pool, but this function should never be called when it's the case.
          // indeed the thread pool set the task to kill itself when that's the case, and therefore it should exit the thread.
          return returnPtr;
      }
      /**
       * This thread has been ask to kill its pthread. You just have to wake it up now !
       */
      void killThread();

  private :

    //! last time of beginning of an idle period
    struct timeval fIdleDate;

    //! Thread Idle Time (in usec)
    // int fIdleTime; // limited to duration shorter than 1h11 !!!!!
    struct timeval fIdleSum;

    //! Unique ID of the thread.
    int fID;

    //! Unique ID of the pthread
    pthread_t fThreadID;

    //! Parent Thread Pool
    ALWeakPtr<ALThreadPool> fThreadPool;
    
    bool fKillThread;    

    //! Current task
    ALPtr<ALTask> fTask;    // not owned (set to NULL if no task)
  };
}
#endif // !AL_THREAD_H_
