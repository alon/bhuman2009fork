/**
* @file KeyStates.h
* 
* Declaration of class KeyStates
*/ 

#ifndef __KeyStates_h_
#define __KeyStates_h_

#include "Tools/Streams/Streamable.h"

/**
* The class represents the states of the keys.
*/
class KeyStates : public Streamable
{
private:
  void serialize(In* in, Out* out)
  {
    STREAM_REGISTER_BEGIN();
    STREAM_ARRAY(pressed);
    STREAM_REGISTER_FINISH();
  }

public:
  enum Key
  {
    rightFootRight,
    rightFootLeft,
    leftFootRight,
    leftFootLeft,
    chest,
    numberOfKeys
  };

  bool pressed[numberOfKeys];

  KeyStates() 
  {
    for(int i = 0; i < numberOfKeys; ++i)
    {
      pressed[i] = false;
    }
  }

  static const char* getKeyName(Key key)
  {
    switch(key)
    {
    case rightFootRight: return "right_foot_right";
    case rightFootLeft: return "right_foot_left";
    case leftFootRight: return "left_foot_right";
    case leftFootLeft: return "left_foot_left";
    case chest: return "chest";
    default: return "unknown";
    }
  }
};


#endif //__KeyStates_h_
