/**
* @file TeamInfo.h
* The file declares a class that encapsulates the structure TeamInfo defined in
* the file RoboCupGameControlData.h that is provided with the GameController.
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
*/ 

#ifndef __TeamInfo_h_
#define __TeamInfo_h_

#include <cstring>
#include "RoboCupGameControlData.h"
#include "Tools/Streams/Streamable.h"

class TeamInfo : public RoboCup::TeamInfo, public Streamable  
{
private:
  /**
  * The method makes the object streamable.
  * @param in The stream from which the object is read (if in != 0).
  * @param out The stream to which the object is written (if out != 0).
  */
  virtual void serialize(In* in, Out* out);

public:
  /**
  * Default constructor.
  */
  TeamInfo()
  {
    memset((RoboCup::TeamInfo*) this, 0, sizeof(RoboCup::TeamInfo));
  }
};

class OwnTeamInfo : public TeamInfo {};
class OpponentTeamInfo : public TeamInfo {};

#endif //__TeamInfo_h_
