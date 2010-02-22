/**
* @file Representations/MotionControl/MotionRequest.h
* This file declares a class that represents the motions that can be requested from the robot.
* @author <A href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</A>
*/

#ifndef __MotionRequest_H__
#define __MotionRequest_H__

#include "SpecialActionRequest.h"
#include "WalkRequest.h"
#include "Tools/Debugging/DebugDrawings.h"

const double walkMaxBackwardSpeed = 70,
             walkMaxForwardSpeed = 100,
             walkMaxLeftRightSpeed = 50,
             walkMaxRotationSpeed = 0.7;

/**
* @class MotionRequest
* A class that represents the motions that can be requested from the robot.
*/
class MotionRequest : public Streamable
{
protected:
  virtual void serialize(In* in, Out* out)
  {  
    STREAM_REGISTER_BEGIN();
    STREAM_ENUM(motion, numOfMotions, MotionRequest::getMotionName);
    STREAM(specialActionRequest);
    STREAM(walkRequest);
    STREAM_REGISTER_FINISH();
  }

public:
  enum Motion
  {
    walk,
    specialAction,
    stand,
    numOfMotions
  };

  Motion motion; /**< The selected motion. */
  SpecialActionRequest specialActionRequest; /**< The special action request, if it is the selected motion. */
  WalkRequest walkRequest; /**< The walk request, if it is the selected motion. */

  /** 
  * The function returns names of motions.
  * @param motion The motion the name of which is returned.
  * @return The corresponding name.
  */
  static const char* getMotionName(Motion motion)
  {
    switch(motion)
    {
    case walk: return "walk";
    case specialAction: return "specialAction";
    case stand : return "stand";
    default: return "unknown";
    }
  }

  /** 
  * Default constructor.
  */
  MotionRequest() : motion(specialAction) {}

  /**
  * Prints the motion request to a readable string. (E.g. "walk: 100mm/s 0mm/s 0°/s")
  * @param destination The string to fill
  */
  void printOut(char* destination) const
  {
    strcpy(destination, getMotionName(motion));
    destination += strlen(destination);
    switch (motion)
    {

    case walk:
      if(walkRequest.speed == Pose2D() && walkRequest.target != Pose2D())
        sprintf(destination, ": %.0lfmm %.0lfmm %.0lf° %s",
                walkRequest.target.translation.x, walkRequest.target.translation.y,
                toDegrees(walkRequest.target.rotation), walkRequest.pedantic ? "(pedantic)" : "");
      else
        sprintf(destination, ": %.0lfmm/s %.0lfmm/s %.0lf°/s %s",
                walkRequest.speed.translation.x, walkRequest.speed.translation.y,
                toDegrees(walkRequest.speed.rotation), walkRequest.pedantic ? "(pedantic)" : "");
      break;
    case stand:
      sprintf(destination,": stand");
      break;
    case specialAction:
      sprintf(destination, ": %s", SpecialActionRequest::getSpecialActionName(specialActionRequest.specialAction));
      break;
    default:
      break;
    }
  }

  /** Draws something*/
  void draw() 
  {
    DECLARE_DEBUG_DRAWING("representation:MotionRequest", "drawingOnField"); // drawing of a request walk vector
    if(motion == walk)
    {
      Vector2<double> translation = walkRequest.speed.translation;
      translation *= 10.0;

      ARROW("representation:MotionRequest", 0, 0, 
        translation.x, translation.y, 4, Drawings::ps_solid, ColorRGBA(0xcd, 0, 0));
      if(walkRequest.speed.rotation != 0.0)
      {
        translation.y = 0;
        translation.x = 750;
        translation.rotate(walkRequest.speed.rotation);
        ARROW("representation:MotionRequest", 0, 0, 
          translation.x, translation.y, 4, Drawings::ps_solid, ColorRGBA(0xcd, 0, 0, 127));
      }
    }
  }
};

#endif // __MotionRequest_H__
