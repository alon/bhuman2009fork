/** 
* @file Modules/MotionControl/HeadMotionEngine.h
* This file declares a module that creates head joint angles from desired head motion.
* @author <A href="mailto:allli@informatik.uni-bremen.de">Alexander Härtl</A>
*/

#ifndef __HeadMotionEngine_h_
#define __HeadMotionEngine_h_

#include "Tools/Math/Vector2.h"
#include "Tools/Module/Module.h"
#include "Representations/MotionControl/HeadMotionRequest.h"
#include "Representations/MotionControl/HeadJointRequest.h"
#include "Representations/Infrastructure/JointData.h"
#include "Representations/Configuration/RobotDimensions.h"

MODULE(HeadMotionEngine)
  REQUIRES(HeadMotionRequest)
  REQUIRES(FilteredJointData)
  REQUIRES(RobotDimensions)
  PROVIDES_WITH_MODIFY(HeadJointRequest)
END_MODULE

class HeadMotionEngine: public HeadMotionEngineBase
{
private:

  /**
  * The update method to generate the head joint angles from desired head motion.
  */
  void update(HeadJointRequest& headJointRequest);

  Vector2<double> lastSpeed;

public:
  /**
  * Default constructor.
  */
  HeadMotionEngine();

};

#endif // __HeadMotionEngine_h_
