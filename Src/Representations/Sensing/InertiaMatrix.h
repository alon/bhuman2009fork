/**
* @file InertiaMatrix.h
* Declaration of class InertiaMatrix.
* @author Colin Graf
*/ 

#ifndef InertiaMatrix_H
#define InertiaMatrix_H

#include "Tools/Math/Pose3D.h"

/**
* @class InertiaMatrix
* Matrix describing the transformation from ground to the acceleration sensors.
*/
class InertiaMatrix : public Pose3D
{
public:
  /** Default constructor. */
  InertiaMatrix() {}
  
  Pose3D inertiaOffset; /**< The estimated offset (including odometry) from last inertia matrix to this one. (This is something like the velocity * 0.02.) */
  bool isValid; /**< Matrix is only valid if robot is on ground. */
  
private:
  /**
  * The method makes the object streamable.
  * @param in The stream from which the object is read 
  * @param out The stream to which the object is written 
  */
  virtual void serialize(In* in, Out* out)
  {
    STREAM_REGISTER_BEGIN();
      STREAM_BASE(Pose3D);
      STREAM(inertiaOffset);
      STREAM(isValid);
    STREAM_REGISTER_FINISH();
  }
};

#endif //InertiaMatrix_H
