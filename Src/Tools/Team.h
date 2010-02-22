/**
* @file Team.h
* Declaration of a class that contains pointers to global data.
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
*/
#ifndef __Team_h_
#define __Team_h_

#include "Tools/MessageQueue/OutMessage.h"
#include "Tools/Debugging/DebugRequest.h"
#include "Tools/Global.h"

/**
* A macro for broadcasting team messages.
* @param type The type of the message from the MessageID enum in MessageIDs.h
* @param format The message format of the message (bin or text).
* @param expression A streamable expression.
*/
#define TEAM_OUTPUT(type,format,expression) \
  { Global::getTeamOut().format << expression;\
    Global::getTeamOut().finishMessage(type); }

#endif //__Team_h_
