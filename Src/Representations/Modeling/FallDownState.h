/**
* @file FallDownState.h
*
* Declaration of class FallDownState
*
* @author <A href="mailto:timlaue@informatik.uni-bremen.de">Tim Laue</A>
*/

#ifndef __FallDownState_h_
#define __FallDownState_h_

#include "Tools/Streams/Streamable.h"


/**
* @class FallDownState
*
* A class that represents the current state of the robot's body
*/
class FallDownState : public Streamable
{
private:
  /** Streaming function 
  * @param in Object for streaming in the one direction
  * @param out Object for streaming in the other direction
  */
  void serialize(In* in, Out* out)
  {
    STREAM_REGISTER_BEGIN();
      STREAM_ENUM(state, numOfStates, FallDownState::getStateName);
    STREAM_REGISTER_FINISH();
  }

public:
  /** Current state of the robot's body*/
  enum State
  { 
    undefined, 
    upright, 
    lyingOnFront, 
    lyingOnBack,
    lyingOnLeftSide,
    lyingOnRightSide,
    numOfStates
  } state;

  /** Default constructor. */
  FallDownState():state(undefined) {}

  /** Converts an enum element to a string
  * @param s The state
  * @return The name of the state
  */
  static const char* getStateName(State s)
  {
    switch(s)
    {
      case undefined: return "undefined";
      case upright: return "upright";
      case lyingOnFront: return "lyingOnFront";
      case lyingOnBack: return "lyingOnBack";
      case lyingOnLeftSide: return "lyingOnLeftSide";
      case lyingOnRightSide: return "lyingOnRightSide";
      default: return "please edit RobotState::getStateName";
    }
  }
};

#endif //__FallDownState_h_
