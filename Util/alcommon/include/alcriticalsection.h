/**
* @author Aldebaran-Robotics
* Aldebaran Robotics (c) 2007 All Rights Reserved - This file is confidential.\n
*
* Version : $Id$
*/

#ifndef AL_CRITICAL_SECTION_H
#define AL_CRITICAL_SECTION_H

#include "altypes.h"
#include "alptr.h"
#include <pthread.h>

//#define AL_CRITICAL_SECTION_DEBUG(...) printf(__VA_ARGS__)
#define AL_CRITICAL_SECTION_DEBUG(...)


namespace AL
{
  class ALMutex;

  /* ALCriticalSection manage mutex: auto lock initialisation and descruction */
  class ALCriticalSection
  {
  public:


    /**
     * \brief Constructor. lock mutex
     * 
     * 
     */
    ALCriticalSection(pthread_mutex_t *mutex);
    /**
     * Also works with ALMutex !!
     */
    ALCriticalSection(ALPtr<ALMutex> mutex);

    /**
     * \brief Destructor. unlock mutex
     * 
     * 
     */
    ~ALCriticalSection(void);

    void ForceUnlockNow( void ); // force unlock at a specific moment (the destructor won't do anything)

  private:
    pthread_mutex_t * fMutex;  // pointer on mutex (not owned)
    ALPtr<ALMutex>    fMutexSmartPtr; // Keep track of this ptr when 2nd constructor is used, to avoid crash !
  };

}

#endif //AL_CRITICAL_SECTION_H
