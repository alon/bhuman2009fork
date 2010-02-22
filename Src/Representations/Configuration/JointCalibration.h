/**
* @file JointCalibration.h
* Declaration of a class for representing the calibration values of joints.
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
*/

#ifndef __JointCalibration_H__
#define __JointCalibration_H__

#include "Representations/Infrastructure/JointData.h"

class JointCalibration : public Streamable
{
private:
  virtual void serialize(In* in, Out* out)
  {
    STREAM_REGISTER_BEGIN();
    STREAM_ARRAY(joints);
    STREAM_REGISTER_FINISH()
  }

public:
  class JointInfo : public Streamable
  {
  private:
    virtual void serialize(In* in, Out* out)
    {
      STREAM_REGISTER_BEGIN();
      double offset = toDegrees(this->offset);
      STREAM(offset);
      STREAM(sign);
      double maxAngle = toDegrees(this->maxAngle),
             minAngle = toDegrees(this->minAngle);
      STREAM(minAngle);
      STREAM(maxAngle);
      if(in)
      {
        this->offset = fromDegrees(offset);
        this->minAngle = fromDegrees(minAngle);
        this->maxAngle = fromDegrees(maxAngle);
      }
      STREAM_REGISTER_FINISH()
    }

  public:
    double offset; /**< An offset added to the angle. */
    short sign; /**< A multiplier for the angle (1 or -1). */
    double maxAngle; /** the maximal angle in radians */
    double minAngle;  /** the minmal angle in radians */

    /**
    * Default constructor.
    */
    JointInfo() : offset(0), sign(1), maxAngle(2.618), minAngle(-2.618) {}
  };

  JointInfo joints[JointData::numOfJoints]; /**< Information on the calibration of all joints. */
};

#endif
