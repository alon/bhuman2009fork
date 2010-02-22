/**
* @file Representations/MotionControl/HeadJointRequest.h
* This file declares a class that represents the requested head joint angles.
* @author <A href="mailto:allli@informatik.uni-bremen.de">Alexander Härtl</A>
*/

#ifndef __HeadJointRequest_H__
#define __HeadJointRequest_H__

#include "Tools/Streams/Streamable.h"

/**
* @class HeadJointRequest
* A class that represents the requested head joint angles.
*/
class HeadJointRequest : public Streamable
{
private:
  virtual void serialize( In* in, Out* out)
  {  
    STREAM_REGISTER_BEGIN();
    STREAM(pan);
    STREAM(tilt);
    STREAM_REGISTER_FINISH();
  }

public:
  /**
  * Default constructor.
  */
  HeadJointRequest() : pan(0), tilt(0) {}

  double pan, /**< Head pan target angle in radians. */
         tilt; /**< Head tilt target angle in radians. */
};

#endif // __HeadJointRequest_H__
