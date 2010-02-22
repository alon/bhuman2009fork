/**
* @file Platform/ProcessFramework.h
*
* Inclusion of platform dependend process related classes and macros.
*
* @author <a href="mailto:martin@martin-loetzsch.de">Martin Lötzsch</a>
*/

#ifndef PROCESSFRAMEWORK_INCLUDED

#ifdef TARGET_ROBOT

#ifdef LINUX
#include "linux/ProcessFramework.h"
#define PROCESSFRAMEWORK_INCLUDED
#endif

#endif


#ifdef TARGET_SIM

#ifdef SIMROBOTQT
#include "SimRobotQt/ProcessFramework.h"
#define PROCESSFRAMEWORK_INCLUDED
#endif

#endif


#ifndef PROCESSFRAMEWORK_INCLUDED
#error "Unknown platform or target"
#endif

#endif

