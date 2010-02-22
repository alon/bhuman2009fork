/** 
* @file Modules/MotionControl/InverseKinematicEngine.h
* This file declares a module that creates joint angles from desired foot positions.
* @author <A href="mailto:allli@informatik.uni-bremen.de">Alexander Härtl</A>
*/

#ifndef __InverseKinematicEngine_h_
#define __InverseKinematicEngine_h_

#include "Tools/Module/Module.h"
#include "Representations/Configuration/RobotDimensions.h"
#include "Representations/Infrastructure/RobotInfo.h"
#include "Representations/MotionControl/MotionSelection.h"
#include "Representations/MotionControl/WalkingEngineStandOutput.h"
#include "Tools/Math/Vector2.h"
#include "Tools/Math/Pose3D.h"

MODULE(InverseKinematicEngine)
  REQUIRES(RobotDimensions)
  REQUIRES(RobotInfo)
  REQUIRES(MotionSelection)
  PROVIDES_WITH_MODIFY(WalkingEngineStandOutput)
END_MODULE

class Params : public Streamable
{
public:
  Vector3<double> relativeFootPositionLeft, relativeFootPositionRight;
  double bodyRotation,
    bodyTilt,
    footRotation,
    ratio;

  void serialize(In* in, Out* out)
  {
    STREAM_REGISTER_BEGIN();
    STREAM(relativeFootPositionLeft);
    STREAM(relativeFootPositionRight);
    STREAM(bodyRotation);
    STREAM(bodyTilt);
    STREAM(footRotation);
    STREAM(ratio);
    STREAM_REGISTER_FINISH();
  }
};

class InverseKinematicEngine: public InverseKinematicEngineBase
{
private:
  Params params;
  Pose3D left, right; /**< The target poses of the feet. */

  /**
  * The update method to generate the standing motion from walkingParameters
  * @param standOutput The WalkingEngineStandOutput (mainly the resulting joint angles)
  */
  void update(WalkingEngineStandOutput& standOutput);

  /**
  * This method generates a Pose3D from the temporary walking parameters
  */
  void generateFootPosition();

public:
  /**
  * Default constructor.
  */
  InverseKinematicEngine();

};

#endif // __InverseKinematicEngine_h_
