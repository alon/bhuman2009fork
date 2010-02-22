/**
* @file Platform/Thread.h
*
* Inclusion of platform dependend definitions for thread usage. 
*
* @author <a href="mailto:ingsie@informatik.uni-bremen.de">Ingo Sieverdingbeck</a>
*/

#ifndef THREAD_INCLUDED

#ifdef TARGET_ROBOT

#ifdef LINUX
#include "linux/Thread.h"
#define THREAD_INCLUDED
#endif

#endif


#ifdef TARGET_SIM

#ifdef _WIN32
#include "Win32/Thread.h"
#define THREAD_INCLUDED
#endif

#ifdef LINUX
#include "linux/Thread.h"
#define THREAD_INCLUDED
#endif

#endif


#ifndef THREAD_INCLUDED
#error Unknown platform
#endif

#endif

