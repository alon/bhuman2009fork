/**
* @file CameraSettings.h
* Declaration of a class representing the settings of the PDA camera.
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
*/

#ifndef __CameraSettings_H__
#define __CameraSettings_H__

#include "Tools/Streams/Streamable.h"

/**
* @class Properties
* The class represents the properties of the camera.
*/
class CameraSettings : public Streamable
{
private:
  /**
  * The method streams this class.
  * Implements an abstract method of class Streamable.
  * @param in Pointer to a stream to read from.
  * @param out Pointer to a stream to write to.
  */
  void serialize(In* in, Out* out)
  {
    STREAM_REGISTER_BEGIN();
    STREAM(exposure);
    STREAM(gain);
    STREAM(red);
    STREAM(blue);
    STREAM(brightness);
    STREAM_REGISTER_FINISH();
  }

public:
  int exposure; /**< The exposure time in the range of [0 .. 1023]. */
  int gain; /**< The gain level in the range of [0 .. 127]. */
  int red; /**< White balance red ratio in the range of [0 .. 255]. */
  int blue; /**< White balance blue ratio in the range of [0 .. 255]. */
  int brightness; /* The brightness in range of [0 .. 255] */

  /**
  * Default constructor.
  * Initializes everything with invalid values.
  */
  CameraSettings() :
    exposure(-1), 
    gain(-1), 
    red(-1), 
    blue(-1), 
    brightness(-1)
  {}
};

#endif
