/** 
* @file Modules/MotionControl/MotionSelector.cpp
* This file implements a module that is responsible for controlling the motion.
* @author <A href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</A>
* @author <A href="mailto:allli@tzi.de">Alexander Härtl</A>
*/

#include "MotionSelector.h"
#include "Tools/Debugging/DebugDrawings.h"

void MotionSelector::update(MotionSelection& motionSelection)
{
  static const int interpolationTimes[MotionRequest::numOfMotions] =
  {
    790, // to walk
    200, // to specialAction
    600 // to stand
  };
  static const int playDeadDelay(2000);

  if(lastExecution)
  {
    MotionRequest::Motion requestedMotion = theMotionRequest.motion;
    if(theMotionRequest.motion == MotionRequest::walk && (!theGroundContactState.contactSafe || theWalkingEngineOutput.enforceStand))
      requestedMotion = MotionRequest::stand;

    // check if the target motion can be the requested motion (mainly if leaving is possible)
    if((lastMotion == MotionRequest::walk && (!&theWalkingEngineOutput || theWalkingEngineOutput.isLeavingPossible || !theGroundContactState.contactSafe)) ||
       lastMotion == MotionRequest::stand || // stand can always be left
       (lastMotion == MotionRequest::specialAction && (!&theSpecialActionsOutput || theSpecialActionsOutput.isLeavingPossible)) ||
       (requestedMotion == MotionRequest::specialAction && 
       (theMotionRequest.specialActionRequest.specialAction == SpecialActionRequest::standUpBackNao || 
       theMotionRequest.specialActionRequest.specialAction == SpecialActionRequest::standUpFrontNao/* ||
       theMotionRequest.specialActionRequest.specialAction == SpecialActionRequest::layDownKeeper*/)))
    {
      motionSelection.targetMotion = requestedMotion;
    }

    if(requestedMotion == MotionRequest::walk)
      motionSelection.walkRequest = theMotionRequest.walkRequest;
    else
      motionSelection.walkRequest = WalkRequest();

    if(requestedMotion == MotionRequest::specialAction)
    {
      motionSelection.specialActionRequest = theMotionRequest.specialActionRequest;
    }
    else
    {
      motionSelection.specialActionRequest = SpecialActionRequest();
      if(motionSelection.targetMotion == MotionRequest::specialAction)
        motionSelection.specialActionRequest.specialAction = SpecialActionRequest::numOfSpecialActions;
    }

    // increase / decrease all ratios according to target motion
    const unsigned deltaTime(theFrameInfo.getTimeSince(lastExecution));
    const int interpolationTime = prevMotion == MotionRequest::specialAction && lastActiveSpecialAction == SpecialActionRequest::playDead ? playDeadDelay : interpolationTimes[motionSelection.targetMotion];
    double delta((double)deltaTime / interpolationTime);
    double sum(0);
    for(int i = 0; i < MotionRequest::numOfMotions; i++)
    {
      if(i == motionSelection.targetMotion)
        motionSelection.ratios[i] += delta;
      else
        motionSelection.ratios[i] -= delta;
      motionSelection.ratios[i] = std::max(motionSelection.ratios[i], 0.0); // clip ratios
      sum += motionSelection.ratios[i];
    }
    ASSERT(sum != 0);
    // normalize ratios
    for(int i = 0; i < MotionRequest::numOfMotions; i++)
      motionSelection.ratios[i] /= sum;

    if(motionSelection.ratios[MotionRequest::specialAction] < 1.0)
    {
      if(motionSelection.targetMotion == MotionRequest::specialAction)
        motionSelection.specialActionMode = MotionSelection::first;
      else
        motionSelection.specialActionMode = MotionSelection::deactive;
    }
    else
      motionSelection.specialActionMode = MotionSelection::active;

    if(motionSelection.specialActionMode == MotionSelection::active && motionSelection.specialActionRequest.specialAction != SpecialActionRequest::numOfSpecialActions)
      lastActiveSpecialAction = motionSelection.specialActionRequest.specialAction;

    if(motionSelection.ratios[MotionRequest::walk] < 1.)
      motionSelection.walkRequest = WalkRequest();
  }
  lastExecution = theFrameInfo.time;
  if(lastMotion != motionSelection.targetMotion)
    prevMotion = lastMotion;
  lastMotion = motionSelection.targetMotion;

  PLOT("module:MotionSelector:ratios:walk", motionSelection.ratios[MotionRequest::walk]);
  PLOT("module:MotionSelector:ratios:stand", motionSelection.ratios[MotionRequest::stand]);
  PLOT("module:MotionSelector:ratios:specialAction", motionSelection.ratios[MotionRequest::specialAction]);
  PLOT("module:MotionSelector:lastMotion", lastMotion);
  PLOT("module:MotionSelector:prevMotion", prevMotion);
  PLOT("module:MotionSelector:targetMotion", motionSelection.targetMotion);
}

MAKE_MODULE(MotionSelector, Motion Control)
