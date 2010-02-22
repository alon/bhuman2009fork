/**
* @file Platform/linux/GameHandler.h
* The file declares a class to communicate with the GameController.
* @author <A href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</A>
*/

#ifndef GameHandler_h_
#define GameHandler_h_

#define GAME_CONTROL \
  GameHandler theGameHandler;
#define START_GAME_CONTROL \
  theGameHandler.start(GAMECONTROLLER_PORT);
#define RECEIVE_GAME_CONTROL(data) \
  theGameHandler.receive(data)
#define SEND_GAME_CONTROL(data) \
  theGameHandler.send(data)

#include "Representations/Infrastructure/RoboCupGameControlData.h"

class UdpSocket;

/** 
* @class GameHandler
* A class for team communication by broadcasting.
*/
class GameHandler
{
public:
  /**
  * Constructor.
  */
  GameHandler();

  /**
  * Destructor.
  */
  ~GameHandler();

  /**
  * The method starts the actual communication on the given port.
  * @param port The UDP port this handler is listening to.
  */
  void start(int port);

  /**
  * The method sends the outgoing message queue if possible.
  * @param data The data to send.
  * @return Was the data sent?
  */
  bool send(const RoboCup::RoboCupGameControlReturnData& data);

  /**
  * The method receives packages if available.
  * @param data If a new package was received, it will be written here.
  * @return Was data received?
  */
  bool receive(RoboCup::RoboCupGameControlData& data);

private:
  UdpSocket* udp; /**< The socket used to communicate. */
};

#endif 
