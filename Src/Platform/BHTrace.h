/**
* @file Platform/BHTrace.h
* Inclusion of platform dependend definitions for low level flow tracing.
* @author Colin Graf
*/

#ifndef BHTRACE_INCLUDED

#ifdef TARGET_ROBOT

#ifdef LINUX
#include "linux/NaoBody.h"
#define BHTRACE_INCLUDED
#endif

#endif

#ifdef TARGET_SIM
#define BH_TRACE ((void)0)
#define BH_TRACE_INIT(x) ((void)0)
#define BH_TRACE_MSG(x) ((void)0)
#define BHTRACE_INCLUDED
#endif

#ifndef BHTRACE_INCLUDED
#error "Unknown platform or target"
#endif

#endif
