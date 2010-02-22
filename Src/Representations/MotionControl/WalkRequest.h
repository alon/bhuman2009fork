/**
* @file Representations/MotionControl/WalkRequest.h
* This file declares a class that represents a walk request.
* @author <A href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</A>
*/

#ifndef __WalkRequest_H__
#define __WalkRequest_H__

#include "Tools/Math/Pose2D.h"

/**
* @class WalkRequest
* A class that represents a walk request.
*/
class WalkRequest : public Streamable
{
public:
  Pose2D speed; /**< Walking speeds, in mm/s and radians/s. */
  Pose2D target; /**< Walking target, in mm and radians, relative to the robot. Use either a speed or a target. */
  bool pedantic; /**< Allows to disable the step size stabilization when precision is indispensable. */

  /** Default constructor. */
  WalkRequest() : pedantic(false) {}

private:
  virtual void serialize(In* in, Out* out)
  {  
    STREAM_REGISTER_BEGIN( );
    STREAM(speed);
    STREAM(target);
    STREAM(pedantic)
    STREAM_REGISTER_FINISH();
  }
};

#endif // __WalkRequest_H__
