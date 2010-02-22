/**
* @file GameInfo.h
* The file declares a class that encapsulates the structure RoboCupGameControlData
* defined in the file RoboCupGameControlData.h that is provided with the GameController.
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
*/ 

#ifndef __GameInfo_h_
#define __GameInfo_h_

#include "RoboCupGameControlData.h"
#include "Tools/Streams/Streamable.h"

class GameInfo : public RoboCup::RoboCupGameControlData, public Streamable  
{
private:
  /**
  * The method makes the object streamable.
  * @param in The stream from which the object is read (if in != 0).
  * @param out The stream to which the object is written (if out != 0).
  */
  virtual void serialize(In* in, Out* out)
  {  
    STREAM_REGISTER_BEGIN();
    STREAM(state); // STATE_READY, STATE_PLAYING, ...
    STREAM(firstHalf); // 1 = game in first half, 0 otherwise
    STREAM(kickOffTeam); // TEAM_BLUE, TEAM_RED
    STREAM(secsRemaining); // estimate of number of seconds remaining in the half.
    STREAM(dropInTeam); // TEAM_BLUE, TEAM_RED
    STREAM(dropInTime); // number of seconds passed since the last drop in. -1 before first dropin.
    STREAM(secondaryState);  // Extra state information - (STATE2_NORMAL, STATE2_PENALTYSHOOT, etc)
    STREAM_REGISTER_FINISH();
  }

  using RoboCup::RoboCupGameControlData::teams; // Make teams private, the information is provided in other representations.

public:
  /**
  * Default constructor.
  */
  GameInfo()
  {
    memset((RoboCup::RoboCupGameControlData*) this, 0, sizeof(RoboCup::RoboCupGameControlData));
  }
};

#endif //__GameInfo_h_
