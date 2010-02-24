/**
* @author Aldebaran Robotics 
* Aldebaran Robotics (c) 2007 All Rights Reserved - This file is confidential.\n
*
* Version : $Id$
*/

/**
* Some tools used by Proxies object
*/

#ifndef AL_BROKER_TOOLS_H
#define AL_BROKER_TOOLS_H


#include "altypes.h"
#include "alptr.h"



//#include "alstub.h"
#include "almoduleinfo.h"

typedef AL::al__ALModuleInfoNaoqi								ALModuleInfo;
typedef std::vector < ALModuleInfo >						TALModuleInfoVector;
typedef TALModuleInfoVector::iterator				ITALModuleInfoVector;
typedef TALModuleInfoVector::const_iterator CITALModuleInfoVector;


// return a string describing the structure ALModuleInfo (mainly for debug purpose)
std::string ToString( const ALModuleInfo& pInfo );




#endif // AL_BROKER_TOOLS_H
