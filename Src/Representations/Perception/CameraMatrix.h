/**
* @file CameraMatrix.h
* Declaration of CameraMatrix and RobotCameraMatrix representation.
* @author Colin Graf
*/ 

#ifndef CameraMatrix_H
#define CameraMatrix_H

#include "Tools/Math/Pose3D.h"

/**
* Matrix describing transformation from center of hip to camera.
*/
class RobotCameraMatrix : public Pose3D
{
public:
  /** Draws the camera matrix. */
  void draw();
};

/**
* Matrix describing transformation from ground (center between booth feet) to camera.
*/
class CameraMatrix : public Pose3D
{
private:
  bool isValid; /**< Matrix is only valid if motion was stable. */

public:
  /** Kind of copy-constructor. 
  * @param pose The other pose.
  */
  CameraMatrix(const Pose3D& pose): Pose3D(pose), isValid(true) {}

  /** Default constructor. */
  CameraMatrix() : isValid(true) {}

  /** Draws the camera matrix. */
  void draw();

private:
  /**
  * The method makes the object streamable.
  * @param in The stream from which the object is read.
  * @param out The stream to which the object is written. 
  */
  virtual void serialize(In* in, Out* out)
  {
    STREAM_REGISTER_BEGIN();
      STREAM_BASE(Pose3D);
      STREAM(isValid);
    STREAM_REGISTER_FINISH();
  }  
};

#endif
