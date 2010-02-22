/** 
* @file Platform/linux/TeamHandler.cpp
* The file implements a class for team communication between simulated robots.
* @author <A href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</A>
*/

#include "TeamHandler.h"
#include "Platform/GTAssert.h"
#include "Platform/SystemCall.h"
#include "Tools/Streams/OutStreams.h"
#include "Tools/Streams/InStreams.h"
#include <netdb.h>
#include <sys/ioctl.h>

TeamHandler::TeamHandler(MessageQueue& in, MessageQueue& out) :
in(in),
out(out),
port(0)
{
}

TeamHandler::~TeamHandler()
{
  close(udpSocket);
}

void TeamHandler::start(int port)
{
  if(this->port)
    close(udpSocket);

  this->port = port;
  struct protoent *udpEntry = getprotobyname("udp");
  udpSocket = socket(AF_INET, SOCK_DGRAM, udpEntry ? udpEntry->p_proto : 0);
  ASSERT(udpSocket != -1);

  int reuse = 1;
  setsockopt(udpSocket, SOL_SOCKET, SO_REUSEADDR, (const char*) &reuse, sizeof(reuse));
  int broadcast = 1;
  setsockopt(udpSocket, SOL_SOCKET, SO_BROADCAST, (const char *) &broadcast, sizeof(broadcast));
  unsigned long nonblocking = 1;
  ioctl(udpSocket, FIONBIO, &nonblocking);

  sockaddr_in address;
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(port);
  VERIFY(bind(udpSocket, (struct sockaddr *) &address, sizeof(address)) != -1);
}

void TeamHandler::send()
{
  if(!port)
    return; // not started yet

  if(!out.isEmpty())
  {
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
    address.sin_addr.s_addr = INADDR_BROADCAST;
    if(sendto(udpSocket, package, size.getSize() + 4, 0, (struct sockaddr *) &address, sizeof(address)) == (int)size.getSize() + 4)
      out.clear();

    delete [] package;
  }
}

void TeamHandler::receive()
{
  if(!port)
    return; // not started yet

  char buffer[1400];
  int size;
  do
  {
    size = recv(udpSocket, buffer, sizeof(buffer), 0);
    if(size >= 4 && size == ((unsigned short*) buffer)[1] && 
       ((unsigned short*) buffer)[0] == calcCRC16(buffer + 2, size - 2))
    {
      in.out.bin << SystemCall::getCurrentSystemTime();
      in.out.finishMessage(idReceiveTimeStamp);
      InBinaryMemory memory(buffer + 4, size - 4);
      memory >> in;
      ASSERT(memory.eof());
    }
  }
  while(size > 0);
}

unsigned short TeamHandler::calcCRC16(const char* address, unsigned size) const
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


