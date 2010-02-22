/**
* @file Representations/Infrastructure/LEDRequest.h
*
* This file contains the LEDRequest class.
*
* @author <A href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</A>
*/

#ifndef __LEDRequest_H__
#define __LEDRequest_H__

#include "Tools/Streams/Streamable.h"

/**
* This describes a LEDRequest
*/
class LEDRequest : public Streamable
{
private:
  virtual void serialize( In* in, Out* out)
  {  
    STREAM_REGISTER_BEGIN( );
    STREAM_ENUM_ARRAY( ledStates, numOfLEDStates, LEDRequest::getLEDStateName);
    STREAM_REGISTER_FINISH();
  }

public:
  /** ids for all LEDs */
  enum LED
  {
    faceLeftRed0Deg = 0,
    faceLeftRed45Deg,
    faceLeftRed90Deg,
    faceLeftRed135Deg,
    faceLeftRed180Deg,
    faceLeftRed225Deg,
    faceLeftRed270Deg,
    faceLeftRed315Deg,
    faceLeftGreen0Deg,
    faceLeftGreen45Deg,
    faceLeftGreen90Deg,
    faceLeftGreen135Deg,
    faceLeftGreen180Deg,
    faceLeftGreen225Deg,
    faceLeftGreen270Deg,
    faceLeftGreen315Deg,
    faceLeftBlue0Deg,
    faceLeftBlue45Deg,
    faceLeftBlue90Deg,
    faceLeftBlue135Deg,
    faceLeftBlue180Deg,
    faceLeftBlue225Deg,
    faceLeftBlue270Deg,
    faceLeftBlue315Deg,
    faceRightRed0Deg,
    faceRightRed45Deg,
    faceRightRed90Deg,
    faceRightRed135Deg,
    faceRightRed180Deg,
    faceRightRed225Deg,
    faceRightRed270Deg,
    faceRightRed315Deg,
    faceRightGreen0Deg,
    faceRightGreen45Deg,
    faceRightGreen90Deg,
    faceRightGreen135Deg,
    faceRightGreen180Deg,
    faceRightGreen225Deg,
    faceRightGreen270Deg,
    faceRightGreen315Deg,
    faceRightBlue0Deg,
    faceRightBlue45Deg,
    faceRightBlue90Deg,
    faceRightBlue135Deg,
    faceRightBlue180Deg,
    faceRightBlue225Deg,
    faceRightBlue270Deg,
    faceRightBlue315Deg,
    earsLeft0Deg,
    earsLeft36Deg,
    earsLeft72Deg,
    earsLeft108Deg,
    earsLeft144Deg,
    earsLeft180Deg,
    earsLeft216Deg,
    earsLeft252Deg,
    earsLeft288Deg,
    earsLeft324Deg,
    earsRight0Deg,
    earsRight36Deg,
    earsRight72Deg,
    earsRight108Deg,
    earsRight144Deg,
    earsRight180Deg,
    earsRight216Deg,
    earsRight252Deg,
    earsRight288Deg,
    earsRight324Deg,
    chestRed,
    chestGreen,
    chestBlue,
    footLeftRed,
    footLeftGreen,
    footLeftBlue,
    footRightRed,
    footRightGreen,
    footRightBlue,
    numOfLEDs
  };

  enum LEDState
  {
    off,
    on,
    blinking,
    fastBlinking,
    numOfLEDStates
  };

  LEDState ledStates[numOfLEDs]; /**< The intended states of the LEDs (use type State). */

  LEDRequest()
  {
    for(int i = 0; i < numOfLEDs; ++i)
      ledStates[i] = off;
  }

  static const char* getNaoLEDName(LED led)
  {
    switch(led)
    {
    case faceLeftRed0Deg: return "faceLeftRed0Deg";
    case faceLeftRed45Deg: return "faceLeftRed45Deg";
    case faceLeftRed90Deg: return "faceLeftRed90Deg";
    case faceLeftRed135Deg: return "faceLeftRed135Deg";
    case faceLeftRed180Deg: return "faceLeftRed180Deg";
    case faceLeftRed225Deg: return "faceLeftRed225Deg";
    case faceLeftRed270Deg: return "faceLeftRed270Deg";
    case faceLeftRed315Deg: return "faceLeftRed315Deg";
    case faceLeftGreen0Deg: return "faceLeftGreen0Deg";
    case faceLeftGreen45Deg: return "faceLeftGreen45Deg";
    case faceLeftGreen90Deg: return "faceLeftGreen90Deg";
    case faceLeftGreen135Deg: return "faceLeftGreen135Deg";
    case faceLeftGreen180Deg: return "faceLeftGreen180Deg";
    case faceLeftGreen225Deg: return "faceLeftGreen225Deg";
    case faceLeftGreen270Deg: return "faceLeftGreen270Deg";
    case faceLeftGreen315Deg: return "faceLeftGreen315Deg";
    case faceLeftBlue0Deg: return "faceLeftBlue0Deg";
    case faceLeftBlue45Deg: return "faceLeftBlue45Deg";
    case faceLeftBlue90Deg: return "faceLeftBlue90Deg";
    case faceLeftBlue135Deg: return "faceLeftBlue135Deg";
    case faceLeftBlue180Deg: return "faceLeftBlue180Deg";
    case faceLeftBlue225Deg: return "faceLeftBlue225Deg";
    case faceLeftBlue270Deg: return "faceLeftBlue270Deg";
    case faceLeftBlue315Deg: return "faceLeftBlue315Deg";
    case faceRightRed0Deg: return "faceRightRed0Deg";
    case faceRightRed45Deg: return "faceRightRed45Deg";
    case faceRightRed90Deg: return "faceRightRed90Deg";
    case faceRightRed135Deg: return "faceRightRed135Deg";
    case faceRightRed180Deg: return "faceRightRed180Deg";
    case faceRightRed225Deg: return "faceRightRed225Deg";
    case faceRightRed270Deg: return "faceRightRed270Deg";
    case faceRightRed315Deg: return "faceRightRed315Deg";
    case faceRightGreen0Deg: return "faceRightGreen0Deg";
    case faceRightGreen45Deg: return "faceRightGreen45Deg";
    case faceRightGreen90Deg: return "faceRightGreen90Deg";
    case faceRightGreen135Deg: return "faceRightGreen135Deg";
    case faceRightGreen180Deg: return "faceRightGreen180Deg";
    case faceRightGreen225Deg: return "faceRightGreen225Deg";
    case faceRightGreen270Deg: return "faceRightGreen270Deg";
    case faceRightGreen315Deg: return "faceRightGreen315Deg";
    case faceRightBlue0Deg: return "faceRightBlue0Deg";
    case faceRightBlue45Deg: return "faceRightBlue45Deg";
    case faceRightBlue90Deg: return "faceRightBlue90Deg";
    case faceRightBlue135Deg: return "faceRightBlue135Deg";
    case faceRightBlue180Deg: return "faceRightBlue180Deg";
    case faceRightBlue225Deg: return "faceRightBlue225Deg";
    case faceRightBlue270Deg: return "faceRightBlue270Deg";
    case faceRightBlue315Deg: return "faceRightBlue315Deg";
    case earsLeft0Deg: return "earsLeft0Deg";
    case earsLeft36Deg: return "earsLeft36Deg";
    case earsLeft72Deg: return "earsLeft72Deg";
    case earsLeft108Deg: return "earsLeft108Deg";
    case earsLeft144Deg: return "earsLeft144Deg";
    case earsLeft180Deg: return "earsLeft180Deg";
    case earsLeft216Deg: return "earsLeft216Deg";
    case earsLeft252Deg: return "earsLeft252Deg";
    case earsLeft288Deg: return "earsLeft288Deg";
    case earsLeft324Deg: return "earsLeft324Deg";
    case earsRight0Deg: return "earsRight0Deg";
    case earsRight36Deg: return "earsRight36Deg";
    case earsRight72Deg: return "earsRight72Deg";
    case earsRight108Deg: return "earsRight108Deg";
    case earsRight144Deg: return "earsRight144Deg";
    case earsRight180Deg: return "earsRight180Deg";
    case earsRight216Deg: return "earsRight216Deg";
    case earsRight252Deg: return "earsRight252Deg";
    case earsRight288Deg: return "earsRight288Deg";
    case earsRight324Deg: return "earsRight324Deg";
    case chestRed: return "chestRed";
    case chestGreen: return "chestGreen";
    case chestBlue: return "chestBlue";
    case footLeftRed: return "footLeftRed";
    case footLeftGreen: return "footLeftGreen";
    case footLeftBlue: return "footLeftBlue";
    case footRightRed: return "footRightRed";
    case footRightGreen: return "footRightGreen";
    case footRightBlue: return "footRightBlue";
    default: return "unknown";
    }
  }

  static const char* getLEDStateName(LEDState state)
  {
    switch(state)
    {
    case off: return "off";
    case on: return "on";
    case blinking: return "blinking";
    case fastBlinking: return "fastBlinking";
    default: return "unknown";
    }
  }

  bool operator==(const LEDRequest& other) const
  {
    for(int i = 0; i < numOfLEDs; i++)
      if(ledStates[i] != other.ledStates[i])
        return false;
    return true;
  }

  bool operator!=(const LEDRequest& other) const
  {
    return !(*this == other);
  }
};

#endif // __LEDRequest_H__
