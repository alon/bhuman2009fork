/**
 * @file BehaviorControlOutput.h
 * Declaration of class BehaviorControlOutput
 *
 * @author Max Risler
 */ 

#ifndef __BehaviorControlOutput_h_
#define __BehaviorControlOutput_h_

#include "Tools/Streams/Streamable.h"
#include "Representations/MotionControl/MotionRequest.h"
#include "Representations/MotionControl/HeadMotionRequest.h"
#include "Representations/Infrastructure/LEDRequest.h"
#include "Representations/Infrastructure/SoundRequest.h"
#include "Representations/BehaviorControl/BehaviorData.h"
#include "Representations/Infrastructure/TeamInfo.h"
#include "Representations/Infrastructure/GameInfo.h"


/**
 * A class collecting the output from the behavior control module
 */
class BehaviorControlOutput : public Streamable
{
public:
  BehaviorControlOutput() {}

  MotionRequest motionRequest;

  HeadMotionRequest headMotionRequest;

  LEDRequest ledRequest;

  SoundRequest soundRequest;

  OwnTeamInfo ownTeamInfo;

  RobotInfo robotInfo;

  GameInfo gameInfo;

  BehaviorData behaviorData;

  virtual void serialize( In* in, Out* out)
  {
    STREAM_REGISTER_BEGIN();
    STREAM(motionRequest);
    STREAM(headMotionRequest);
    STREAM(ledRequest);
    STREAM(soundRequest);
	STREAM(robotInfo);
    STREAM(gameInfo);
    STREAM(ownTeamInfo);
    STREAM(behaviorData);
    STREAM_REGISTER_FINISH();
  }
};

#endif //__BehaviorControlOutput_h_
