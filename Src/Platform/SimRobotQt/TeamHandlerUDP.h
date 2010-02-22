/** 
* @file Platform/SimRobotQt/TeamHandlerUDP.h
* The file declares a class for the PC to participate in the team communication between robots.
* @author <A href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</A>
*/

#ifndef	__TeamHandlerUDP_h_
#define	__TeamHandlerUDP_h_

#include "Tools/MessageQueue/MessageQueue.h"

/** 
* @class TeamHandlerUDP
* A class for team communication by broadcasting.
*/
class TeamHandlerUDP
{
public:
  /**
  * Constructor.
  * @param in Incoming debug data is stored here.
  * @param out Outgoing debug data is stored here.
  */
  TeamHandlerUDP(MessageQueue& in, MessageQueue& out);

  /**
  * Destructor.
  */
  ~TeamHandlerUDP();

  /**
  * The method starts the actual communication on the given port.
  * @param port The UDP port this handler is listening to.
  * @param subnet The subnet broadcasts are sent to.
  */
  void start(int port, const std::string& subnet);

  /**
  * The method sends the outgoing message queue if possible and 
  * receives packages if available.
  */
  void execute();

private:
  MessageQueue& in, /**< Incoming debug data is stored here. */
              & out; /**< Outgoing debug data is stored here. */
  int port; /**< The UDP port this handler is listening to. */
  std::string subnet; /**< The subnet broadcasts are sent to. */
  int udpSocket; /**< The socket used to communicate. */

  /**
  * The method calculates a CRC16 checksum of a memory area.
  * @param address The begin of the area.
  * @param size The length of the area in bytes.
  */
  unsigned short calcCRC16(const char* address, unsigned size) const;
};

#endif 
