/** 
* @file  Platform/SystemCall.h
* @brief static class for system calls from the non NDA classes
*
* Inclusion of platform dependend definitions of system calls. 
*
* @author <a href="mailto:martin@martin-loetzsch.de">Martin Lötzsch</a>
*/

#ifndef SYSTEMCALL_INCLUDED

#ifdef TARGET_ROBOT

#ifdef LINUX
#include "linux/SystemCall.h"
#define SYSTEMCALL_INCLUDED
#endif

#endif


#ifdef TARGET_SIM

#ifdef SIMROBOTQT
#include "SimRobotQt/SystemCall.h"
#define SYSTEMCALL_INCLUDED
#else
#endif

#endif


#ifndef SYSTEMCALL_INCLUDED
#error "Unknown platform or target"
#endif

#endif

