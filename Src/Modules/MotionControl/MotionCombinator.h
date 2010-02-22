/** 
* @file Modules/MotionControl/MotionCombinator.h
* This file declares a module that combines the motions created by the different modules.
* @author <A href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</A>
*/

#ifndef __MotionCombinator_h_
#define __MotionCombinator_h_

#include "Tools/Module/Module.h"
#include "Representations/Configuration/RobotDimensions.h"
#include "Representations/Infrastructure/JointData.h"
#include "Representations/MotionControl/MotionSelection.h"
#include "Representations/MotionControl/SpecialActionsOutput.h"
#include "Representations/MotionControl/WalkingEngineOutput.h"
#include "Representations/MotionControl/WalkingEngineStandOutput.h"
#include "Representations/MotionControl/OdometryData.h"
#include "Representations/MotionControl/MotionInfo.h"
#include "Representations/MotionControl/HeadJointRequest.h"

MODULE(MotionCombinator)
  REQUIRES(RobotDimensions)
  REQUIRES(FilteredJointData)
  REQUIRES(MotionSelection)
  REQUIRES(WalkingEngineOutput)
  REQUIRES(SpecialActionsOutput)
  REQUIRES(WalkingEngineStandOutput)
  REQUIRES(HeadJointRequest)
  PROVIDES_WITH_MODIFY_AND_OUTPUT(OdometryData)
  PROVIDES_WITH_MODIFY(UnstableJointRequest)
  PROVIDES_WITH_MODIFY_AND_OUTPUT(JointRequest)
  PROVIDES_WITH_MODIFY(MotionInfo)
END_MODULE

class MotionCombinator : public MotionCombinatorBase
{
private:
  JointData lastJointData; /**< The measured joint angles the last time when not interpolating. */
  OdometryData odometryData; /**< The odometry data. */
  MotionInfo motionInfo; /**< Information about the motion currently executed. */

  void update(OdometryData& odometryData);
  void update(UnstableJointRequest& jointRequest);
  void update(JointRequest& jointRequest);
  void update(MotionInfo& motionInfo) {motionInfo = this->motionInfo;}

#ifndef RELEASE
  OdometryData lastOdometryData;
  JointRequest lastJointRequest;
#endif

  /**
  * The method copies all joint angles from one joint request to another,
  * but only those that should not be ignored.
  * @param source The source joint request. All angles != JointData::ignore will be copied.
  * @param target The target joint request.
  */
  void copy(const JointRequest& source, JointRequest& target) const;

  /**
  * The method interpolates between two joint requests.
  * @param from The first source joint request. This one is the starting point.
  * @param to The second source joint request. This one has to be reached over time.
  * @param fromRatio The ratio of "from" in the target joint request.
  * @param target The target joint request.
  * @param interpolateHardness Whether to interpolate hardness.
  */
  void interpolate(const JointRequest& from, const JointRequest& to, double fromRatio, JointRequest& target, bool interpolateHardness) const;
};

#endif // __MotionCombinator_h_
