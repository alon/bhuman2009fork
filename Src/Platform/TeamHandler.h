/**
* @file Platform/TeamHandler.h
* Inclusion of platform dependent code for team communication.
* @author <A href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</A>
*/

#ifndef TEAMHANDLER_INCLUDED

#ifdef TARGET_ROBOT

#ifdef LINUX
#include "linux/TeamHandler.h"
#define TEAMHANDLER_INCLUDED
#define INIT_TEAM_COMM \
  theTeamHandler(theTeamReceiver, theTeamSender)
#define START_TEAM_COMM \
  theTeamHandler.start(Global::getSettings().teamPort);
#define RECEIVE_TEAM_COMM \
  theTeamHandler.receive()
#endif

#endif


#ifdef TARGET_SIM

#ifdef SIMROBOTQT
#include "SimRobotQt/TeamHandler.h"
#define TEAMHANDLER_INCLUDED
#define INIT_TEAM_COMM \
  theTeamHandler(theTeamReceiver, theTeamSender, this)
#define START_TEAM_COMM
#define RECEIVE_TEAM_COMM
#endif

#endif


#ifndef TEAMHANDLER_INCLUDED
#error "Unknown platform or target"
#endif

#define TEAM_COMM \
  MessageQueue theTeamReceiver; \
  MessageQueue theTeamSender; \
  TeamHandler theTeamHandler;

#define SEND_TEAM_COMM \
  theTeamHandler.send()

#endif

