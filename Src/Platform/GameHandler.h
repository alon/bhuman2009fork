/**
* @file Platform/GameHandler.h
* Inclusion of platform dependent code for game controller handling.
* @author <A href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</A>
*/


#ifndef GAMEHANDLER_INCLUDED
#define GAMEHANDLER_INCLUDED

#if defined(TARGET_ROBOT) && defined(LINUX)
#  include "linux/GameHandler.h"
#else
// #  warning no game handler
#  define GAME_CONTROL
#  define START_GAME_CONTROL
#  define RECEIVE_GAME_CONTROL(data) false
#  define SEND_GAME_CONTROL(data) false
#endif

#endif
