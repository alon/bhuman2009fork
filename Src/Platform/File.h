/**
* @file Platform/File.h
*
* Inclusion of platform dependend definitions of simple file handling. 
*
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
*/

#ifndef FILE_INCLUDED

#ifdef TARGET_ROBOT

#ifdef LINUX
#include "linux/File.h"
#define FILE_INCLUDED
#endif

#endif


#ifdef TARGET_SIM

#ifdef WIN32
#include "Win32/File.h"
#define FILE_INCLUDED
#endif


#ifdef LINUX
#include "linux/File.h"
#define FILE_INCLUDED
#endif

#endif


#ifndef FILE_INCLUDED
#error "Unknown platform or target"
#endif

#endif
