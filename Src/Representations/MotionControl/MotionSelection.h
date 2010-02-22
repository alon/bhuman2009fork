/**
* @file Representations/MotionControl/MotionSelection.h
* This file declares a class that represents the motions actually selected based on the constraints given.
* @author <A href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</A>
*/

#ifndef __MotionSelection_H__
#define __MotionSelection_H__

#include "MotionRequest.h"

/**
* @class MotionSelection
* A class that represents the motions actually selected based on the constraints given.
*/
class MotionSelection : public Streamable
{
private:
  virtual void serialize(In* in, Out* out)
  {  
    STREAM_REGISTER_BEGIN();
    STREAM_ENUM(specialActionMode, numOfActivationModes, MotionSelection::getActivationModeName);
    STREAM_ARRAY(ratios);
    STREAM(specialActionRequest);
    STREAM(walkRequest);
    STREAM_REGISTER_FINISH();
  }

public:
  enum ActivationMode
  {
    deactive,
    active,
    first,
    numOfActivationModes
  };

  MotionRequest::Motion targetMotion; /**< The motion that is the destination of the current interpolation. */
  ActivationMode specialActionMode; /**< Whether and how the special action module is active. */
  double ratios[MotionRequest::numOfMotions]; /**< The current ratio of each motion in the final joint request. */
  SpecialActionRequest specialActionRequest; /**< The special action request, if it is an active motion. */
  WalkRequest walkRequest; /**< The walk request, if it is an active motion. */

  /** 
  * The function returns the names of the activation modes.
  * @param mode The activation mode the name of which is returned.
  * @return The corresponding name.
  */
  static const char* getActivationModeName(ActivationMode mode)
  {
    switch(mode)
    {
    case deactive: return "deactive";
    case active: return "active";
    case first: return "first";
    default: return "unknown";
    }
  }

  /** 
  * Default constructor.
  */
  MotionSelection() : targetMotion(MotionRequest::specialAction), specialActionMode(active)
  {
    memset(ratios, 0, sizeof(ratios));
    ratios[MotionRequest::specialAction] = 1;
  }
};

#endif // __MotionSelection_H__
