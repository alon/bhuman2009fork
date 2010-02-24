/**
* @author Aldebaran Robotics 
* Aldebaran Robotics (c) 2007 All Rights Reserved - This file is confidential.\n
*
* Version : $Id$
*/


#ifndef AL_TYPES_H
#define AL_TYPES_H

#ifndef _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_DEPRECATE  // remove warning deprecated...
#endif


#include <string>
#include <vector>

#ifndef SOAP_EOF
#define SOAP_EOF			EOF
#endif

#ifndef SOAP_ERR
#define SOAP_ERR			EOF
#endif

#ifndef SOAP_OK
#define SOAP_OK				0
#endif

// string function plateform compatibility
#if defined( WIN32 )
# pragma warning(disable:4702)
# pragma warning(disable:4100)
# pragma warning(disable:4512)

#ifndef snprintf
  #define snprintf sprintf_s
  #define HAVE_SNPRINTF  // python warning
# endif
# define strncpy(dest,src,size) strcpy_s(dest,size,src)
# define stristr strstr_ignorecase
# define strcasecmp _stricmp
#else // WIN32
# define stristr strstr_ignorecase
# define stricmp strcasecmp
# define strcpy_s(dest,size,src) strncpy(dest,src,size)
# define _stricmp stricmp
#define strncat_s(dst,dstlen,src,srclenmax) strncat(dst,src,srclenmax)
#endif // WIN32

// standard type definitions
#ifndef uint32_defined
# define uint32_defined
typedef unsigned long uInt32;
typedef signed long sInt32;
typedef unsigned short uInt16;
typedef signed short sInt16;
typedef unsigned char uInt8;
typedef signed char sInt8;
typedef unsigned char uChar;
typedef signed char sChar;
#endif // uint32_defined

// Operating systems
const int kLinux = 0;
const int kWindows = 1;
const int kMacOS = 2;
const int kOther = 3;

// Broker port
const int kBrokerPort = 9559;
const int kMemoryPort = 9560;

// Communication mode
const int kLocal = 0;
const int kPlugged = 0;
const int kRemote = 1;

// Other
const int kForever = -1;

// Vision
const sInt32 kDefaultImageWidth  = 640;
const sInt32 kDefaultImageHeight = 480;

// Class definitions
namespace AL
{
 /* class ALModule;
  class ALBroker;
  class ALValue;
*/

  typedef enum _TVerbosity
  {
    VerbosityNone=0,
    VerbosityMini,
    VerbosityMedium,
    VerbosityFull,
  }
  Verbosity;

  typedef enum _TLanguageID
  {
    Language_En = 0,  // anglais Britain
    Language_Us,      // anglais am�ricain UK
    Language_Fr,      // Francais

    Language_Number,  // the number of different language
    Language_Unknown,
  }
  TLanguageID;




} // namespace AL

typedef std::vector<std::string>						TStringArray;
typedef std::vector<float>							    TFloatArray;
typedef std::vector<int>								    TIntArray;

typedef TStringArray::iterator							ITStringArray;
typedef TStringArray::const_iterator				CITStringArray;

typedef TFloatArray::iterator								ITFloatArray;
typedef TFloatArray::const_iterator					CITFloatArray;

typedef TIntArray::iterator									ITIntArray;
typedef TIntArray::const_iterator						CITIntArray;




#define AL_OK SOAP_OK
#define AL_ERR SOAP_ERR

// alcommon options
//#define AL_PERF_CALCULATION 
//#define DEBUG_TASK_VIEWER
//#define DEBUG_TASK_MSG(x) ;  // use telepathe plugin
#define DEBUG_TASK_MSG(x)


#endif
