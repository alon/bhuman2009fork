/**
* @file Platform/GTAssert.h
*
* Inclusion of platform dependend definitions for low level debugging.
*
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
*/

#ifndef ROBOCUPCTRL_INCLUDED

#ifdef TARGET_ROBOT
#define ROBOCUPCTRL_INCLUDED
#endif

#ifdef TARGET_SIM

#ifdef SIMROBOTQT
#include "SimRobotQt/RoboCupCtrl.h"
#define ROBOCUPCTRL_INCLUDED
#endif

#endif


#ifndef ROBOCUPCTRL_INCLUDED
#error "Unknown platform or target"
#endif

#endif
