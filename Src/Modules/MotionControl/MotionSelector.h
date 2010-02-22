/** 
* @file Modules/MotionControl/MotionSelector.h
* This file declares a module that is responsible for controlling the motion.
* @author <A href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</A>
* @author <A href="mailto:allli@tzi.de">Alexander Härtl</A>
*/

#ifndef __MotionSelector_h_
#define __MotionSelector_h_

#include "Tools/Module/Module.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/MotionControl/SpecialActionsOutput.h"
#include "Representations/MotionControl/WalkingEngineOutput.h"
#include "Representations/MotionControl/WalkingEngineStandOutput.h"
#include "Representations/MotionControl/MotionRequest.h"
#include "Representations/MotionControl/MotionSelection.h"
#include "Representations/Perception/GroundContactState.h"

MODULE(MotionSelector)
  USES(SpecialActionsOutput)
  USES(WalkingEngineOutput)
  USES(WalkingEngineStandOutput)
  REQUIRES(FrameInfo)
  REQUIRES(MotionRequest)
  REQUIRES(GroundContactState)
  PROVIDES_WITH_MODIFY(MotionSelection)
END_MODULE

class MotionSelector : public MotionSelectorBase
{
private:
  MotionRequest::Motion lastMotion;
  MotionRequest::Motion prevMotion;
  unsigned lastExecution;
  SpecialActionRequest::SpecialActionID lastActiveSpecialAction;
  void update(MotionSelection& motionSelection);

public:
  /**
  * Default constructor.
  */
  MotionSelector() : lastMotion(MotionRequest::specialAction), prevMotion(MotionRequest::specialAction),
lastActiveSpecialAction(SpecialActionRequest::playDead) {}
};

#endif // __MotionSelector_h_
