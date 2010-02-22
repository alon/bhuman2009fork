/**
* @file Representations/MotionControl/HeadMotionRequest.h
* This file declares a class that represents the requested head motion.
* @author <A href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</A>
*/

#ifndef __HeadMotionRequest_H__
#define __HeadMotionRequest_H__

#include "Tools/Streams/Streamable.h"

/**
* @class HeadMotionRequest
* A class that represents the requested head motion.
*/
class HeadMotionRequest : public Streamable
{
protected:
  virtual void serialize( In* in, Out* out)
  {  
    STREAM_REGISTER_BEGIN( );
    STREAM(pan);
    STREAM(tilt);
    STREAM(speed);
    STREAM_REGISTER_FINISH();
  }

public:
  /**
  * Default constructor.
  */
  HeadMotionRequest() : pan(0), tilt(0), speed(1) {}

  double pan, /**< Head pan target angle in radians. */
         tilt, /**< Head tilt target angle in radians. */
         speed; /**< Maximum joint speed to reach target angles in radians/s. */
};

#endif // __HeadMotionRequest_H__
