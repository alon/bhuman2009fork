/**
* @file Platform/TcpComm.h
*
* Inclusion of platform dependent definitions of simple TCP/IP communication handling. 
*
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
*/

#ifdef APERIOS1_3_2
#include "Aperios1.3.2/TcpComm.h"
#define TCPCOMM_INCLUDED
#endif

#ifdef _WIN32
#include "Win32Linux/TcpComm.h"
#define TCPCOMM_INCLUDED
#endif

#ifdef LINUX
#include "Win32Linux/TcpComm.h"
#define TCPCOMM_INCLUDED
#endif

#ifdef CYGWIN
#include "Win32Linux/TcpComm.h"
#define TCPCOMM_INCLUDED
#endif

#ifndef TCPCOMM_INCLUDED
#error Unknown platform
#endif
