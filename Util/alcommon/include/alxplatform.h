/**
* @author Aldebaran Robotics 
* Aldebaran Robotics (c) 2007 All Rights Reserved - This file is confidential.\n
*
* Version : $Id$
*/


#ifndef AL_XPLATFORM_H
#define AL_XPLATFORM_H

#ifndef NDEBUG
# ifndef DEBUG
#		define DEBUG
# endif
# ifndef _DEBUG
#		define _DEBUG
# endif
#endif

// standard type definitions
#ifndef uint32_defined
# define uint32_defined
  typedef unsigned long uInt32;
  typedef signed long sInt32;
  typedef unsigned short uInt16;
  typedef signed short sInt16;
  typedef unsigned char uInt8;
  typedef signed char sInt8;
  typedef unsigned char uChar;
  typedef signed char sChar;
#endif // uint32_defined


#if defined(__APPLE__) || defined(__linux__)
 #include <unistd.h>
#endif

//#ifdef _WIN32
#  ifdef __cplusplus
//#    include "ALH.h"
#  endif
 //#include "winsock2.h"
 //#include "windows.h"
//#endif

#if defined(__APPLE__) || defined(__linux__)
//inline void SleepMs(uInt32 pNbMilliseconds) {usleep(pNbMilliseconds*1000);};
#  define SleepMs(millisec) usleep(millisec*1000)
#endif
#if defined(_WIN32)
//inline void SleepMs(uInt32 pNbMilliseconds) {Sleep(pNbMilliseconds);};
#  define SleepMs(millisec) Sleep(millisec)
#endif

#ifdef WIN32
#	define strdup _strdup
# include <direct.h> // for _chdir
#	define chdir _chdir
#	define unlink _unlink
#else
# include <unistd.h> // for chdir
#endif

// Alma 08-04-03: Temporary addition TODO: is utsefull ? 
#ifdef WIN32
# define ALRANDOM(maxExclus) (rand()%(int)(maxExclus))
#else
# define ALRANDOM(maxExclus) (rand()%(int)(maxExclus))
#endif

#endif			// #define AL_XPLATFORM_H

