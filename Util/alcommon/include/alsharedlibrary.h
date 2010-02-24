/**
* @author Aldebaran Robotics 
* Aldebaran Robotics (c) 2007 All Rights Reserved - This file is confidential.\n
*
* Version : $Id$
*/

#ifndef AL_SHARED_LIBRARY
#define AL_SHARED_LIBRARY

#include <string>
#include <vector>
#include "alptr.h"
#include "alerror.h"

#ifdef WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

namespace AL
{
  class ALBroker;
  /**
  * ALSharedLibrary manage dynamic load of library
  *
  */
  class ALSharedLibrary
  {
  public:

   /**
    * constructor
    * @param ini file with library to load
    */
    ALSharedLibrary(ALPtr<ALBroker> pBroker);

   /**
    * destructor
    */
    ~ALSharedLibrary(void);
    
    /**
     * convert the libraryname to the filename then
     * load the library
     */
    void loadLibrary(const std::string& libraryName, 
        const std::string& pPath=std::string(""));

  private:

	  inline ALPtr<ALBroker> getParentBroker(void)
	  {
		  ALPtr<ALBroker> returnPtr = parentBroker.lock();
		  AL_ASSERT(returnPtr != NULL);
		  return returnPtr;
	  }

	  /**
     * load the library
     */
    ALSharedLibrary *loadFilename(std::string pPath, std::string pFileName);

    /**
    * getFunction
    * @param function name in library
    @return pointer on function
    */
    void *getFunction(const char *pFunctionName);


    /**
    * runModule
    * call function _createModule
    * @return AL_ERR if failed
    */
    int runModule(  );

  private:
    void *handle;

	  ALWeakPtr<ALBroker> parentBroker;
  };
}

#endif
