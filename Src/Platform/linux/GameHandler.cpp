/** 
* @file Platform/linux/GameHandler.cpp
* The file declares a class to communicate with the GameController.
* @author <A href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</A>
*/

#include "GameHandler.h"
#include "Platform/GTAssert.h"
#include "Platform/Win32Linux/UdpSocket.h"

#include <cstring> // needed for memcmp

GameHandler::GameHandler(): udp(0)
{
}

GameHandler::~GameHandler()
{
  if (udp)
    delete udp;
}

void GameHandler::start(int port)
{
  udp = new UdpSocket();
  VERIFY(udp->setBlocking(false));
  VERIFY(udp->setBroadcast(true));
  VERIFY(udp->bind("0.0.0.0", port));
  VERIFY(udp->setTarget("255.255.255.255", port)); //set default target
}

bool GameHandler::send(const RoboCup::RoboCupGameControlReturnData& data)
{
  return udp->write((const char*)&data, sizeof(data));
}

bool GameHandler::receive(RoboCup::RoboCupGameControlData& data)
{
  if(!udp)
    return false; // not started yet

  bool received = false;
  int size;
  RoboCup::RoboCupGameControlData buffer;
  while(0 < (size = udp->read((char*)&buffer, sizeof(buffer))))
  {
    if(size == sizeof(buffer) &&
       !std::memcmp(&buffer, GAMECONTROLLER_STRUCT_HEADER, 4) &&
       buffer.version == GAMECONTROLLER_STRUCT_VERSION)
    {
      data = buffer;
      received = true;
    }
  }
  return received;
}
