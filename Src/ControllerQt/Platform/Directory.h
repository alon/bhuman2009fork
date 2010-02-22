/**
* @file ControllerQt/Platform/Directory.h
* Inclusion of platform dependend implementation of a class for accessing directories. 
*
* @author Colin Graf
*/

#ifndef __DIRECTORY_H__

#ifdef _WIN32
#include "Win32/Directory.h"
#endif

#ifdef LINUX
#include "Linux/Directory.h"
#endif

#ifndef __DIRECTORY_H__
#error "Unknown platform"
#endif

#endif //__DIRECTORY_H__
