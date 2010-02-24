/**
* @author Aldebaran Robotics 
* Aldebaran Robotics (c) 2007 All Rights Reserved - This file is confidential.\n
*
* Version : $Id$
*/


#ifndef AL_CRITICALFALSE_H
#define AL_CRITICALFALSE_H

#include "altypes.h"
#include "alptr.h"
#include <pthread.h>


namespace AL
{

  class ALMutex;

  class ALCriticalFalseIfLocked 
  {
  public:


    /**
    * \brief Constructor. lock mutex
    * 
    * 
    */
    ALCriticalFalseIfLocked(pthread_mutex_t *mutex, bool &alreadyLocked);
    ALCriticalFalseIfLocked(ALPtr<ALMutex> mutex, bool &alreadyLocked);

    /**
    * \brief Destructor. unlock mutex
    * 
    * 
    */
    ~ALCriticalFalseIfLocked(void);


    void ForceUnlockNow( void ); // force unlock at a specific moment (the destructor won't do anything)

  private:
    pthread_mutex_t * fMutex;  // pointer on mutex (not owned)
    bool fAlreadyLocked;
  };

}

#endif
