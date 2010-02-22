/** 
* @file Platform/SimRobotQt/TeamHandlerUDP.cpp
* The file implements a class for team communication between simulated robots.
* @author <A href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</A>
*/

#include "TeamHandlerUDP.h"
#include "Platform/GTAssert.h"
#include "Platform/SystemCall.h"
#include "Tools/Streams/OutStreams.h"
#include "Tools/Streams/InStreams.h"

#ifdef _WIN32
#include <winsock.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#else // linux
#include <errno.h>
#include <fcntl.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#endif

#ifdef _WIN32

//#define ERRNO WSAGetLastError()
//#define EWOULDBLOCK WSAEWOULDBLOCK
//#define EINPROGRESS WSAEINPROGRESS
#define CLOSE(socket) closesocket(socket)

#else

//#define ERRNO errno
#define CLOSE(socket) close(socket)
//#define BOOL int

#endif

TeamHandlerUDP::TeamHandlerUDP(MessageQueue& in, MessageQueue& out) :
in(in),
out(out),
port(0)
{
}

TeamHandlerUDP::~TeamHandlerUDP()
{
  CLOSE(udpSocket);
}

void TeamHandlerUDP::start(int port, const std::string& subnet)
{
  if(this->port)
    CLOSE(udpSocket);

  this->port = port;
  this->subnet = subnet;

  // Socket for receiving broadcast messages
  udpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  ASSERT(udpSocket > 0);

  sockaddr_in address;
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(port);
  VERIFY(bind(udpSocket, (struct sockaddr *) &address, sizeof(address)) == 0);

  int broadcast = 1;
  VERIFY(!setsockopt(udpSocket, SOL_SOCKET, SO_BROADCAST, (const char *) &broadcast, sizeof(broadcast)));
#ifdef _WIN32
  unsigned long nonblocking = 1;
  VERIFY(ioctlsocket(udpSocket, FIONBIO, &nonblocking) == 0)
#else
  VERIFY(fcntl(udpSocket, F_SETFL, O_NONBLOCK) == 0);
#endif
}

void TeamHandlerUDP::execute()
{
  if(!port)
    return; // not started yet

  if(!out.isEmpty())
  {
    out.out.bin << 0;
    out.out.finishMessage(idRobot);
    out.out.bin << SystemCall::getRealSystemTime();
    out.out.finishMessage(idSendTimeStamp);
    OutBinarySize size;
    size << out;
    char* package = new char[size.getSize() + 4];
    ASSERT(package);
    OutBinaryMemory memory(package + 4);
    memory << out;
    ((unsigned short*) package)[1] = size.getSize() + 4;
    ((unsigned short*) package)[0] = calcCRC16(package + 2, size.getSize() + 2);

    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = inet_addr(subnet.c_str());
    if(sendto(udpSocket, package, size.getSize() + 4, 0, (struct sockaddr *) &address, sizeof(address)) == int(size.getSize() + 4))
      out.clear();

    delete [] package;
  }

  char buffer[1400];
  int size;
  do
  {
    size = recv(udpSocket, buffer, sizeof(buffer), 0);
    if(size >= 4 && size == ((unsigned short*) buffer)[1] && 
       ((unsigned short*) buffer)[0] == calcCRC16(buffer + 2, size - 2))
    {
      in.out.bin << SystemCall::getRealSystemTime();
      in.out.finishMessage(idReceiveTimeStamp);
      InBinaryMemory memory(buffer + 4, size - 4);
      memory >> in;
      ASSERT(memory.eof());
    }
  }
  while(size > 0);
}

unsigned short TeamHandlerUDP::calcCRC16(const char* address, unsigned size) const
{
  unsigned short crc16 = 0;
  unsigned char lastChar = 0;
  while(size--)
  {
    if(crc16 & 0x8000)
      crc16 = (crc16 << 1) ^ 0x8005;
    else
      crc16 <<= 1;
    unsigned char c = *address++;
    crc16 ^= (unsigned short) (c | (lastChar << 8));
    lastChar = c;
  }
  return crc16;
}


