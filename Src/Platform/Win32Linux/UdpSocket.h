/**
 * @file Platform/Win32Linux/UdpSocket.h
 * Wrapper for non-blocking udp socket.
 * \author Armin
 */

#ifndef UdpSocket_H
#define UdpSocket_H

struct sockaddr;
struct sockaddr_in;

/**
* @class UdpSocket
*/
class UdpSocket
{
public:
  /**
  * Constructor.
  */
  UdpSocket();

  /**
  * Destructor.
  */
  ~UdpSocket();

  /**
  * Set default target address.
  * @param ip The ip address of the host system.
  * @param port The port used for the connection.
  * \return Does a connection exist? 
  */
  bool setTarget(const char* ip, int port);

  /**
   * Set broadcast mode.
   */
  bool setBroadcast(bool);

  bool setBlocking(bool);

  /**
   * Set multicast mode (please use multicast adresses to avoid confusion).
   */
  bool joinMulticast(const char*);

  bool setRcvBufSize(unsigned int);

  /**
  * bind to IN_ADDR_ANY to receive packets
  */
  bool bind(const char* addr, int port);

  /**
  * The function tries to read a package from a socket.
  * @return True if a package was read.
  */
  int read(char* data, int len, const int flags = 0);

  /**
  * The function writes a package to a socket.
  * @return True if the package was written.
  */
  bool write(const char* data, const int len, const int flags = 0);

private:
  struct sockaddr* target;
  int sock;
  bool resolve(const char*, int, struct sockaddr_in*);
};

#endif
