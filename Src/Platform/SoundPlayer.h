/**
* @file Platform/SoundPlayer.h
*
* Inclusion of platform dependend definitions of sound player. 
*
* @author Colin Graf
*/

#ifndef SOUNDPLAYER_INCLUDED


#ifdef TARGET_ROBOT

#ifdef LINUX
#include "linux/SoundPlayer.h"
#define SOUNDPLAYER_INCLUDED
#endif

#endif


#ifdef TARGET_SIM

#ifdef WIN32
#include "Win32/SoundPlayer.h"
#define SOUNDPLAYER_INCLUDED
#endif

#ifdef LINUX
#include "linux/SoundPlayer.h"
#define SOUNDPLAYER_INCLUDED
#endif

#endif


#ifndef SOUNDPLAYER_INCLUDED
#error "Unknown platform or target"
#endif

#endif
