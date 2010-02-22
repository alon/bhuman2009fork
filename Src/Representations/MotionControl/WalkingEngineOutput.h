/**
* @file Representations/MotionControl/WalkingEngineOutput.h
* This file declares a class that represents the output of modules generating motion.
* @author <A href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</A>
*/

#ifndef __WalkingEngineOutput_H__
#define __WalkingEngineOutput_H__

#include "Representations/Infrastructure/JointData.h"
#include "Tools/Math/Pose2D.h"

/**
* @class WalkingEnigeOutput
* A class that represents the output of the walking engine.
*/
class WalkingEngineOutput : public JointRequest
{
public:
  Pose2D speed, /**< The current walking speed. */
         target, /**< The current walking target. */
         odometryOffset; /**< The body motion performed in this step. */
  bool isLeavingPossible; /**< Is leaving the motion module possible now? */
  double positionInWalkCycle; /**< The current position in the walk cycle in the range [0..1[. */
  Pose2D maxSpeed; /**< The maximum regular walking speed. */
  double maxBackwardSpeed; /**< The maximum regular backwards walking speed. */
  bool enforceStand; /**< This allows the walking engine to enforce a stand motion selection. (good for exception handling) */
  double instability; /**< An evaluation of the current walk stability. */

  /** 
  * Default constructor.
  */
  WalkingEngineOutput() : isLeavingPossible(true), positionInWalkCycle(0), 
    maxBackwardSpeed(0), enforceStand(false), instability(0.) {}

private:
  virtual void serialize(In* in, Out* out)
  {  
    STREAM_REGISTER_BEGIN();
    STREAM_BASE(JointRequest);
    STREAM(speed);
    STREAM(odometryOffset);
    STREAM(isLeavingPossible);
    STREAM(positionInWalkCycle);
    STREAM(maxSpeed);
    STREAM(maxBackwardSpeed);
    STREAM(enforceStand);
    STREAM(instability);
    STREAM_REGISTER_FINISH();
  }
};

#endif // __WalkingEngineOutput_H__
