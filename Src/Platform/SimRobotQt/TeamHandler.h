/** 
* @file Platform/SimRobotQt/TeamHandler.h
* The file declares a class for team communication between simulated robots.
* @author <A href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</A>
*/

#ifndef	__TeamHandler_h_
#define	__TeamHandler_h_

#include "Platform/ProcessFramework.h"
#include "Tools/MessageQueue/MessageQueue.h"

/** 
* @class TeamHandler
* A class for team communication by broadcasting.
*/
class TeamHandler : public BroadcastReceiver
{
public:
  /**
  * Constructor.
  * @param in Incoming debug data is stored here.
  * @param out Outgoing debug data is stored here.
  * @param process The process this handler is part of.
  */
  TeamHandler(MessageQueue& in, MessageQueue& out, PlatformProcess* process);

  /**
  * The method sends the outgoing message queue if possible.
  */
  void send();

private:
  MessageQueue& in, /**< Incoming debug data is stored here. */
              & out; /**< Outgoing debug data is stored here. */
  PlatformProcess* process; /**< The process this handler is part of. */

  /**
  * The method is called when a package is received.
  * @param data The address of the package.
  */
  virtual void onReceive(const char* data);
};

#endif 
