/**
* @file Platform/TeamHandlerUDP.h
* Inclusion of platform dependent code for team udp communication.
* @author Colin Graf
*/

#ifndef TEAMHANDLERUDP_INCLUDED

#ifdef TARGET_SIM

#ifdef SIMROBOTQT
#include "SimRobotQt/TeamHandlerUDP.h"
#define TEAMHANDLERUDP_INCLUDED
#endif

#endif

#ifndef TEAMHANDLERUDP_INCLUDED
#error "Unknown platform or target"
#endif

#endif //!TEAMHANDLERUDP_INCLUDED
