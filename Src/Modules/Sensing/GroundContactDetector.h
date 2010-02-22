/**
* @file GroundContactDetector.h
* Declaration of module GroundContactDetector.
* @author Colin Graf
*/

#ifndef GroundContactDetector_H
#define GroundContactDetector_H

#include "Tools/Module/Module.h"
#include "Tools/Range.h"
#include "Representations/Infrastructure/SensorData.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Perception/GroundContactState.h"
#include "Representations/MotionControl/MotionInfo.h"
#include "Representations/MotionControl/MotionRequest.h"

MODULE(GroundContactDetector)
  REQUIRES(SensorData)
  REQUIRES(FrameInfo)
  REQUIRES(MotionRequest)
  USES(MotionInfo)
  PROVIDES_WITH_MODIFY(GroundContactState)
END_MODULE

/**
* @class GroundContactDetector
* A module for sensor data filtering.
*/
class GroundContactDetector : public GroundContactDetectorBase
{
public:
  /** Default constructor. */
  GroundContactDetector();

private:
  /**
  * A collection of parameters for the walking engine.
  */
  class Parameters : public Streamable
  {
  public:
    /** Default constructor. */
    Parameters() {}

    float minContactPercent;
    unsigned int minFootContactUnity;
    unsigned int minCycleContactUnity;
    int safeContactTime;

  private:
    /**
    * The method makes the object streamable.
    * @param in The stream from which the object is read.
    * @param out The stream to which the object is written. 
    */
    virtual void serialize(In* in, Out* out)
    {  
      STREAM_REGISTER_BEGIN();
        STREAM(minContactPercent);
        STREAM(minFootContactUnity);
        STREAM(minCycleContactUnity);
        STREAM(safeContactTime);
      STREAM_REGISTER_FINISH();
    }
  };

  Parameters p;

  Range<float> leftRange;
  Range<float> rightRange;

  unsigned int framesWithContact;
  unsigned int framesWithoutContact;
  unsigned int contactStartTime;

  bool contact;
  bool wasContacted;

  /**
  * Updates the GroundContactState representation .
  * @param groundContactState The ground contact representation which is updated by this module.
  */
  void update(GroundContactState& groundContactState);

  void getMinAndMax(const float* data, float& min, float& max);
};

#endif
