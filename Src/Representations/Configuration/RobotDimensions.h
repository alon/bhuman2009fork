/**
 * @file RobotDimensions.h
 *
 * Description of the Dimensions of the Kondo Robot
 *
 * @author Cord Niehaus
 */

#ifndef __RobotDimensions_H__
#define __RobotDimensions_H__

#include "Tools/Streams/Streamable.h"
#include "Tools/Math/Vector3.h"

class RobotDimensions : public Streamable
{
private:
  virtual void serialize(In* in, Out* out)
  { 
    STREAM_REGISTER_BEGIN();

    STREAM(lengthBetweenLegs);
    STREAM(upperLegLength);
    STREAM(lowerLegLength);
    STREAM(heightLeg5Joint);
 
    STREAM(zLegJoint1ToHeadPan);
    STREAM(xHeadTiltToCamera);
    STREAM(zHeadTiltToCamera);
    STREAM(headTiltToCameraTilt);

    STREAM(armOffset);
    STREAM(upperArmLength);
    STREAM(lowerArmLength);

    STREAM(motionCycleTime);         
    STREAM(imageRecordingTime);

    STREAM_REGISTER_FINISH();
  }

public:

  double lengthBetweenLegs;         //!<length between leg joints LL1 and LR1
  double upperLegLength;            //!<length between leg joints LL2 and LL3 in z-direction
  double lowerLegLength;            //!<length between leg joints LL3 and LL4 in z-direction
  double heightLeg5Joint;           //!<height of leg joints LL4 and LR4 of the ground
 
  double zLegJoint1ToHeadPan;       //!<height offset between LL1 and head pan joint 
  double xHeadTiltToCamera;         //!<forward offset between head tilt joint and camera
  double zHeadTiltToCamera;         //!<height offset between head tilt joint and 
  double headTiltToCameraTilt;      //!<tilt of camera against head tilt

  Vector3<double> armOffset;        //! The offset of the first left arm joint relative to the middle between the hip joints
  double upperArmLength;            //! The length between the shoulder and the elbow in y-direction
  double lowerArmLength;            //!< height off lower arm starting at arm2/arm3

  double motionCycleTime;           //! Length of one motion cycle in seconds.
  double imageRecordingTime;        //! Time the camera requires to take an image (in s, for motion compensation). 
};

#endif
