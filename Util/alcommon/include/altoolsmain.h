/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2007 All Rights Reserved - This file is confidential.\n
*
* Version : $Id$
*/


// dcoz. 11/03/09. TODO: LOTS OF CLEANING:
//
// - Fix encoding format of the comments !
//
// - move functions to a proper namespace ! (eg ALTools:: )
//  (functions such as replace() are likely to create nasty conflicts)
//
// - Replace AL_TRACE, AL_DEBUG_LINE with standard logging functions that
//   everybody can use.


/**
* Module where to put all various common tools to aldebaran naoqi, naoqicore, modules...
* It's only some small functions not doing any network message / remote connection...
*/

#ifndef AL_TOOLSMAIN_H
#define AL_TOOLSMAIN_H

#include "altypes.h"
#include "alsignal.hpp"
#include "alptr.h"
#include "altools.h"

namespace AL
{
  class  ALBroker;
}

typedef boost::function1<int, AL::ALPtr<AL::ALBroker> > TMainType;

namespace ALTools
{


  // called at ctrl-c
  void _termination_handler (int );

  // main function for remote modules
  int mainFunction(const std::string &pBrokerName, int argc, char *argv[], const TMainType &callback );

  // write pid in filename (linux)
  void writePid(const std::string &PIDFileName);
}

#endif // AL_TOOLS_H
