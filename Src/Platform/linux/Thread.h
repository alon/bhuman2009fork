/**
* @file Platform/linux/Thread.h
*
* Declaration of a template class for threads and some
* other classes for synchronization. Based on code
* from B-Smart (real author unknown).
*
* @author <a href="mailto:Bernd.Gersdorf@dfki.de">Bernd Gersdorf</a>
*/

#ifndef THREAD_H_
#define THREAD_H_

#include <pthread.h>
#include <unistd.h>
#include "Platform/GTAssert.h"

/**
* A class encapsulating a pthread
*/
template <class T> class Thread
{
private:
  pthread_t handle; /**< The pthread-handle */
  int priority; /**< The priority of the thread. */
  volatile bool running; /**< A flag which indicates the state of the thread */
  void (T::*function)(); /**< The address of the main function of the thread. */
  T* object; /**< A pointer to the object that is provided to the main function. */

  /**
  * The function is called when the thread is started.
  * It calls the main function of the thread as a member function of
  * an object.
  * @param p A pointer to the thread object.
  */
  static void* threadStart(Thread<T>* p)
  {
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, 0);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, 0);
    ((p->object)->*(p->function))();
    p->running = false;
    return 0;
  }

public:
  /** 
  * Default constructor.
  */
  Thread() : handle(0), running(false) {setPriority(0);}

  /** 
  * Destructor.
  * Stops the thread, if it is still running.
  */
  virtual ~Thread() {stop();}

  /**
  * The function starts a member function as a new thread.
  * @param o The object the member function operates on.
  * @param f The member function.
  */
  void start(T* o,void (T::*f)())
  {
    if(running)
      stop();
    function = f;
    object = o;
    running = true; 
    VERIFY(!pthread_create(&handle, 0, (void* (*)(void*)) &Thread<T>::threadStart, this));
    setPriority(priority);
  }

  /**
  * The function stops the thread.
  * It first signals its end by setting running to false. If the thread
  * does not terminate by itself, it will be killed after one second.
  */
  void stop()
  {
    if(running)
    {
      running = false;
      usleep(1000000); //Wait for 1 s
    }
    if(handle)
    {
      pthread_cancel(handle);
      pthread_join(handle, 0);
      handle = 0;
    }
  }

  /**
  * The function announces that the thread shall terminate.
  * It will not try to kill the thread.
  */
  void announceStop() {running = false;}

  /**
  * The function sets the priority of the thread.
  * @param prio Priority relative to "normal" priority.
  */
  void setPriority(int prio) 
  {
    priority = prio;
    if(handle && priority)
    {
      sched_param param;
      if(priority == 15)
        param.sched_priority = sched_get_priority_max(SCHED_FIFO);
      else
        param.sched_priority = priority;
      VERIFY(!pthread_setschedparam(handle, SCHED_FIFO, &param));
    }
  }

  /**
  * The function determines whether the thread should still be running.
  * @return Should it continue?
  */
  bool isRunning() const {return running;}

  /**
  * The function returns the thread id.
  * @return The thread id. Only valid after the thread was started.
  */
  unsigned getId() const {return handle;}

  /**
  * The function returns the id of the calling thread.
  * @return The id of the calling thread.
  */
  static unsigned getCurrentId() {return pthread_self();}
};

/**
* A class encapsulating a mutex lock.
*/
class SyncObject
{
private:
  pthread_mutex_t mutex; /**< The mutex. */

public:
  /** 
  * Constructor.
  */
  SyncObject()
  {
#ifdef DEBUG
    pthread_mutexattr_t attr;
    VERIFY(pthread_mutexattr_init(&attr) == 0);
    VERIFY(pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK_NP) == 0);
    VERIFY(pthread_mutex_init(&mutex, &attr) == 0);
    VERIFY(pthread_mutexattr_destroy(&attr) == 0);
#else
    pthread_mutex_init(&mutex, NULL);
#endif
  }

  /** 
  * Destructor 
  */
  ~SyncObject()
  {
    pthread_mutex_destroy(&mutex);
  }

  /**
  * The function enters the critical section.
  * It suspends the current thread, until the critical section
  * was left by all other threads.
  */
  void enter()
  {
#ifdef DEBUG
    ASSERT(pthread_mutex_lock(&mutex) == 0); // the mutex was problably already locked by this thread. THERE ARE NESTED SYNC BLOCKS !!!
#else
    pthread_mutex_lock(&mutex);
#endif
  }

  /**
  * The function leaves the critical section.
  */
  void leave()
  {
    pthread_mutex_unlock(&mutex);
  }
};

/**
* The class provides a handy interface to using SyncObjects.
*/
class Sync
{
private:
  SyncObject& syncObject; /**< A reference to a sync object. */

public:
  /**
  * Constructor.
  * @param s A reference to a sync object representing a critical
  *          section. The section is entered.
  */
  Sync(SyncObject& s) : syncObject(s) {syncObject.enter();}

  /**
  * Destructor.
  * The critical section is left.
  */
  ~Sync() {syncObject.leave();}
};

/**
* The macro places a SyncObject as member variable into a class.
* This is the precondition for using the macro SYNC.
*/
#define DECLARE_SYNC mutable SyncObject _syncObject

/**
* The macro SYNC ensures that the access to member variables is synchronized.
* So only one thread can enter a SYNC block for this object at the same time.
* The SYNC is automatically released at the end of the current code block.
* Never nest SYNC blocks, because this will result in a deadlock!
*/
#define SYNC Sync _sync(_syncObject)

/**
* The macro SYNC_WITH ensures that the access to the member variables of an 
* object is synchronized. So only one thread can enter a SYNC block for the
* object at the same time. The SYNC is automatically released at the end of
* the current code block. Never nest SYNC blocks, because this will result 
* in a deadlock!
*/
#define SYNC_WITH(obj) Sync _sync((obj)._syncObject)

#endif //THREAD_H_
