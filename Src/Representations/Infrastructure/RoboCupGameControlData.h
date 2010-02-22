/**
* @file RoboCupGameControlData.h
* The file encapsulates definitions in the file RoboCupGameControlData.h 
* that is provided with the GameController in a namespace.
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
*/ 

#ifndef __RoboCupGameControlData_h_
#define __RoboCupGameControlData_h_

namespace RoboCup
{
  typedef unsigned char uint8;
  typedef unsigned short uint16;
  typedef unsigned int uint32;

#define teamColour teamColor
#include <../Util/gameController/RoboCupGameControlData.h>
}

#endif //__RoboCupGameControlData_h_
