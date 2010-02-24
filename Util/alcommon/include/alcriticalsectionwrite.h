/**
* @author Aldebaran Robotics 
* Aldebaran Robotics (c) 2007 All Rights Reserved - This file is confidential.\n
*
* Version : $Id$
*/


#ifndef AL_CRITICAL_SECTION_WRITE_H
#define AL_CRITICAL_SECTION_WRITE_H

#include "altypes.h"
#include <pthread.h>
#include "alptr.h"


namespace AL
{
  class ALMutexRW;

  /* ALCriticalSection manage read lock: auto lock initialisation and descruction */
  class ALCriticalSectionWrite
  {
  public:


    /**
    * \brief Constructor. lock mutex
    * 
    * 
    */
    ALCriticalSectionWrite( pthread_rwlock_t * pMutex );
    ALCriticalSectionWrite( ALPtr<ALMutexRW> pMutex );

    /**
    * \brief Destructor. unlock mutex
    * 
    * 
    */
    ~ALCriticalSectionWrite( void );

  private:
    pthread_rwlock_t * fMutex;  // pointer on mutex (not owned)
    ALPtr<ALMutexRW>   fMutexSmartPtr; // Keep track of this ptr when 2nd constructor is used, to avoid crash !
  };

}

#endif // AL_CRITICAL_SECTION_WRITE_H
