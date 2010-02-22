/** 
* @file MotionInfo.h
*
* Definition of class MotionInfo.
*
* @author Martin Lötzsch
*/

#ifndef __MotionInfo_h__
#define __MotionInfo_h__

#include "MotionRequest.h"

/**
* @class MotionInfo
* Contains information about the motions which are executed by the Motion process.
*/
class MotionInfo : public Streamable
{
private:
  virtual void serialize(In* in, Out* out)
  {  
    STREAM_REGISTER_BEGIN();
    STREAM(executedMotionRequest);
    STREAM(isMotionStable);
    STREAM(positionInWalkCycle);
    STREAM_REGISTER_FINISH();
  }

public:
  /** 
  * Default constructor.
  */
  MotionInfo() : isMotionStable(false), positionInWalkCycle(0) {}
  
  MotionRequest executedMotionRequest; /**< The motion request that is currently executed. */
  bool isMotionStable; /**< If true, the motion is stable, leading to a valid camera matrix. */
  double positionInWalkCycle; /**< The position inside the step of the walking engine slides from 0 to 1 during one step. */
};

#endif // __MotionInfo_h__
