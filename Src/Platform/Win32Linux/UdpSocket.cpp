/**
 * @file Platform/Win32Linux/UdpSocket.cpp
 * Wrapper for non-blocking udp socket.
 * \author Armin
 */

#include "UdpSocket.h"

#include <iostream>
#if defined(WIN32)
#  define BHUMAN_USE_INET_ADDR
#  include <winsock2.h>
#  include <ws2tcpip.h>
#elif defined(LINUX)
#  include <errno.h>
#  include <unistd.h>
#  include <fcntl.h>
#  include <sys/socket.h>
#  include <sys/types.h>
#  include <sys/ioctl.h>
#  include <net/if.h>
#  include <netinet/in.h>
#  include <arpa/inet.h>
#  include <cstring>
#endif

#include "Platform/GTAssert.h"

UdpSocket::UdpSocket()
{
  sock = socket( AF_INET, SOCK_DGRAM, 0 );
  target = (struct sockaddr*)(new struct sockaddr_in);

  ASSERT(-1 != sock);
}

UdpSocket::~UdpSocket()
{
#ifdef WIN32
  closesocket(sock);
#else
  close(sock);
#endif

  delete (struct sockaddr_in*)target;
}


bool UdpSocket::resolve(const char* addrStr, int port, struct sockaddr_in* addr)
{
  memset(addr, 0, sizeof(struct sockaddr_in) );
  addr->sin_family = AF_INET;
  addr->sin_port = htons(port);
#ifdef BHUMAN_USE_INET_ADDR
  addr->sin_addr.s_addr = inet_addr(addrStr);
  if (-1 == addr->sin_addr.s_addr )
  {
    std::cerr << addrStr << " is not a valid dotted ipv4 address" << std::endl;
    return false;
  }
#else
  if(1 != inet_pton(AF_INET, addrStr, &(addr->sin_addr.s_addr) ) )
  {
    std::cerr << addrStr << " is not a valid dotted ipv4 address" << std::endl;
    return false;
  }
#endif

  return true;
}


bool UdpSocket::setTarget(const char* addrStr, int port)
{
  struct sockaddr_in* addr = (struct sockaddr_in*)target;
  if (!resolve(addrStr, port, addr))
    return false;


  // if(0 != ::connect(sock, (struct sockaddr*)addr, sizeof(struct sockaddr_in)) )
  // {
  //   std::cerr << "UdpSocket::connect() failed: " << strerror(errno) 
  //             << ". Missing route?" << std::endl;
  //   return false;
  // }
  return true;
}


bool UdpSocket::setBlocking(bool block)
{
#ifdef WIN32
  int yes = block ? 1 : 0;
  if (ioctlsocket(sock, FIONBIO, (u_long*)&yes))
    return false;
  else
    return true;
#else
  bool result(false);
  if(block)
  {
    if(-1 != fcntl(sock, F_SETFL, 0))
      result = true;
  }
  else
  {
    if(-1 != fcntl(sock, F_SETFL, O_NONBLOCK))
      result = true;
  }
  return result;
#endif
}

bool UdpSocket::joinMulticast(const char* addrStr)
{
  struct sockaddr_in group;
  if (!resolve(addrStr, 0, &group))
    return false;

  //join multicast group for every interface
  if(IN_MULTICAST(ntohl(group.sin_addr.s_addr)))
  {
#ifndef WIN32
    struct ip_mreq mreq;
    struct ifconf ifc;
    struct ifreq* item;
    char buf[1024];

    ifc.ifc_len = sizeof(buf);
    ifc.ifc_buf = buf;
    if( ioctl(sock, SIOCGIFCONF, &ifc) < 0 )
    {
      std::cerr << "cannot get interface list" << std::endl;
      return false;
    }
    else
    {
      bool could_join(false);
      for (unsigned int i = 0; i < ifc.ifc_len / sizeof(struct ifreq); i++)
      {
        item = &ifc.ifc_req[i];
        mreq.imr_multiaddr = group.sin_addr;
        mreq.imr_interface = ((struct sockaddr_in *)&item->ifr_addr)->sin_addr;
        if (0 == setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP,
                            (void*)&mreq, sizeof(mreq)))
        {
          could_join = true;
        }
      }
      if (! could_join)
      {
        std::cerr << "join multicast group failed for interface" << std::endl;
        return false;
      }
    }
#else
  char host[128];
  struct hostent *pHost;
  if(gethostname(host, sizeof(host)) < 0 || !(pHost = (struct hostent*)gethostbyname(host)))
  {
    std::cerr << "cannot get interface list" << std::endl;
    return false;
  }
  
  struct ip_mreq mreq;
  bool couldJoin(false);
  for(int i = 0; pHost->h_addr_list[i]; i++)
  {
    mreq.imr_multiaddr = group.sin_addr;
    mreq.imr_interface = *((struct in_addr*)pHost->h_addr_list[i]);
    if(setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&mreq, sizeof(mreq)) == 0)
      couldJoin = true;
  }
  if(!couldJoin)
  {
    std::cerr << "join multicast group failed for interface" << std::endl;
    return false;
  }
#endif
    return true;
  }
  else
    std::cerr << "not a multicast address" << std::endl;
  return false;
}


bool UdpSocket::setBroadcast(bool enable)
{
  int yes = enable ? 1 : 0;
  if (0 == setsockopt(sock, SOL_SOCKET, SO_BROADCAST,
                  (const char *) &yes, sizeof(yes)))
  {
    return true;
  }
  else
  {
    std::cerr << "UdpSocket::setBroadcast() failed: " << strerror(errno) << std::endl;
    return false;
  }
}

bool UdpSocket::setRcvBufSize(unsigned int rcvbuf)
{
  if (0 == setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (char*) &rcvbuf, sizeof(rcvbuf)))
  {
    std::cerr << "multicast-socket: setsockopt for SO_RCVBUF failed: "
              << strerror(errno) << std::endl;
    return false;
  }

  int result;
  socklen_t result_len = sizeof(result);
  if (0 == getsockopt(sock, SOL_SOCKET, SO_RCVBUF, (char*) &result, &result_len))
  {
    std::cerr << "multicast-socket: receive buffer set to "
              << result << " Bytes." << std::endl;
    return true;
  }

  std::cerr << "multicast-socket: could not get sockopt SO_RCVBUF" << std::endl;
  return false;
}


bool UdpSocket::bind(const char* addr_str, int port)
{
  static const int yes = 1;
  struct sockaddr_in addr;
  addr.sin_addr.s_addr = INADDR_ANY; //addr.sin_addr;
  addr.sin_port = htons(port);
  addr.sin_family = AF_INET;

#ifdef BHUMAN_USE_INET_ADDR
  addr.sin_addr.s_addr = inet_addr(addr_str);
#else
  int r = inet_pton(AF_INET, addr_str, &(addr.sin_addr) );
  if(r <= 0)
  {
    std::cerr << "UdpSocket::bind() failed: invalid address " << addr_str << std::endl;
    return false;
  }
#endif

#ifdef SO_REUSEADDR
    if (-1 == setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char*)&yes, sizeof(yes)))
      std::cerr << "UdpSocket: could not set SO_REUSEADDR" << std::endl;
#endif
#ifdef SO_REUSEPORT
    if (-1 == setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, (const char*)&yes, sizeof(yes)))
      std::cerr << "UdpSocket: could not set SO_REUSEPORT" << std::endl;
#endif
  if (-1 == ::bind(sock, (struct sockaddr*)&addr, sizeof(struct sockaddr_in)) )
  {
    std::cout << "UdpSocket::bind() failed: " << strerror(errno) << std::endl;
    return false;
  }

  return true;
}

int UdpSocket::read(char* data, int len, int flags)
{
  return ::recv(sock,data,len,flags);
}

bool UdpSocket::write(const char* data, const int len, int flags)
{
  int result = ::sendto(sock,data,len,flags,
                        target, sizeof(struct sockaddr_in));
  if(len != result)
  {
    std::cerr << "UdpSocket::write(): sendto returned " << result << ". " << strerror(errno) << std::endl;
    return false;
  }

  return true;
}
