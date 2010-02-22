/**
* @file Platform/GTAssert.h
*
* Inclusion of platform dependend definitions for low level debugging.
*
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
*/

#ifndef GTASSERT_INCLUDED

#ifdef TARGET_ROBOT

#ifdef LINUX
#include "linux/GTAssert.h"
#define GTASSERT_INCLUDED
#endif

#endif


#ifdef TARGET_SIM

#ifdef WIN32
#include "Win32/GTAssert.h"
#define GTASSERT_INCLUDED
#endif

#ifdef LINUX
#include "linux/GTAssert.h"
#define GTASSERT_INCLUDED
#endif

#endif


#ifndef GTASSERT_INCLUDED
#error "Unknown platform or target"
#endif

#endif
