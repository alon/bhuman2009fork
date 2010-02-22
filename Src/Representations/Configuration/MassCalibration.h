/**
* @file MassCalibration.h
* Declaration of a class for representing the relative positions and masses of mass points.
* @author <a href="mailto:allli@informatik.uni-bremen.de">Alexander Härtl</a>
*/

#ifndef __MassCalibration_H__
#define __MassCalibration_H__

#include "Tools/Math/Vector3.h"
#include "Tools/Streams/Streamable.h"

class MassCalibration : public Streamable
{
private:
  virtual void serialize(In* in, Out* out)
  {
    STREAM_REGISTER_BEGIN();
    STREAM_ARRAY(masses);
    STREAM_REGISTER_FINISH()
  }

public:
  enum Limb
  {
    head,
    upperArmLeft,
    lowerArmLeft,
    upperArmRight,
    lowerArmRight,
    upperLegLeft,
    lowerLegLeft,
    footLeft,
    upperLegRight,
    lowerLegRight,
    footRight,
    torso,
    numOfLimbs
  };

  /** 
  * Information on the mass distribution of a limb of the robot.
  */
  class MassInfo : public Streamable
  {
  private:
    virtual void serialize(In* in, Out* out)
    {
      STREAM_REGISTER_BEGIN();
      STREAM(mass);
      STREAM(offset);
      STREAM_REGISTER_FINISH()
    }

  public:
    double mass; /**< The mass of this limb. */
    Vector3<double> offset; /**< The offset of the center of mass of this limb relative to its hinge. */

    /**
    * Default constructor.
    */
    MassInfo() : mass(0), offset() {}
  };

  MassInfo masses[numOfLimbs]; /**< Information on the mass distribution of all joints. */
};

#endif
