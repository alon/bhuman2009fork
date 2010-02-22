/** 
* @file Platform/linux/TeamHandler.h
* The file declares a class for the PC to participate in the team communication between robots.
* @author <A href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</A>
*/

#ifndef	__TeamHandler_h_
#define	__TeamHandler_h_

#include <sys/socket.h>
#include "Tools/MessageQueue/MessageQueue.h"

/** 
* @class TeamHandler
* A class for team communication by broadcasting.
*/
class TeamHandler
{
public:
  /**
  * Constructor.
  * @param in Incoming debug data is stored here.
  * @param out Outgoing debug data is stored here.
  */
  TeamHandler(MessageQueue& in, MessageQueue& out);

  /**
  * Destructor.
  */
  ~TeamHandler();

  /**
  * The method starts the actual communication on the given port.
  * @param port The UDP port this handler is listening to.
  */
  void start(int port);

  /**
  * The method sends the outgoing message queue if possible.
  */
  void send();

  /**
  * The method receives packages if available.
  */
  void receive();

private:
  MessageQueue& in, /**< Incoming debug data is stored here. */
              & out; /**< Outgoing debug data is stored here. */
  int port; /**< The UDP port this handler is listening to. */
  int udpSocket; /**< The socket used to communicate. */

  /**
  * The method calculates a CRC16 checksum of a memory area.
  * @param address The begin of the area.
  * @param size The length of the area in bytes.
  */
  unsigned short calcCRC16(const char* address, unsigned size) const;
};

#endif 
