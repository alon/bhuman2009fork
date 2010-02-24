/**
* @author Aldebaran-robotics
* Aldebaran Robotics (c) 2007 All Rights Reserved - This file is confidential.\n
*
* Version : $Id$
*/

#ifndef AL_MUTEX_H
#define AL_MUTEX_H

#include "altypes.h"
#include "alptr.h"
#include <pthread.h>


namespace AL
{
  /* ALMutex manages mutex: auto mutex initialisation and descruction */
  class ALMutex : public AL::AnyPtr<ALMutex>
  {
    friend class ALCriticalSection;
    friend class ALCriticalFalseIfLocked;
    friend class ALCondition;
  protected:

   /**
   * \brief Constructor.
   */
    ALMutex() : fMutex(new pthread_mutex_t()) {pthread_mutex_init(fMutex, NULL);}

  public:
    static ALMutex::Ptr createALMutex(){return ALMutex::Ptr(new ALMutex());}
   /**
   * \brief Destructor.
   */
    virtual ~ALMutex(void) 
    {
      pthread_mutex_destroy(fMutex);
      delete fMutex;
    }

  protected:
    pthread_mutex_t* getMutex() {return fMutex;};

  protected:
    pthread_mutex_t * const fMutex;  // pointer on mutex
  };

  /* ALMutex manage mutex: auto mutex initialisation and descruction */
  class ALMutexRW : public AL::AnyPtr<ALMutexRW>
  {
    friend class ALCriticalSectionRead;
    friend class ALCriticalSectionWrite;
  protected:

   /**
   * \brief Constructor.
   */
    ALMutexRW() : fMutex(new pthread_rwlock_t()) {pthread_rwlock_init(fMutex, NULL);}

  public:
    static ALMutexRW::Ptr createALMutexRW(){return ALMutexRW::Ptr(new ALMutexRW());}
   /**
   * \brief Destructor.
   */
    virtual ~ALMutexRW(void) 
    {
      pthread_rwlock_destroy(fMutex);
      delete fMutex;
    }

  protected:
    pthread_rwlock_t* getMutex() {return fMutex;};

  private:
    pthread_rwlock_t * const fMutex;  // pointer on mutex
  };

  class ALCondition : public AL::AnyPtr<ALCondition>
  {
  protected:
   /**
   * \brief Constructor.
   */
    ALCondition() : fCondition(new pthread_cond_t()) {pthread_cond_init(fCondition, NULL);}

  public:
    static ALCondition::Ptr createALCondition(){return ALCondition::Ptr(new ALCondition());}

    inline void wait(ALMutex::Ptr pMutex) {pthread_cond_wait(fCondition, pMutex->getMutex());}
    inline void broadcast() {pthread_cond_broadcast(fCondition);}
    inline void signal() {pthread_cond_signal(fCondition);}

   /**
   * \brief Destructor.
   */
    virtual ~ALCondition(void) {pthread_cond_destroy(fCondition);delete fCondition;}

  protected:
    pthread_cond_t * const fCondition;  // pointer on mutex
  };
}

#endif
