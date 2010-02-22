/**
* @file CameraCalibration.h
* Declaration of a class for representing the calibration values of the PDA camera.
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
*/

#ifndef __CameraCalibration_H__
#define __CameraCalibration_H__

#include "Tools/Streams/Streamable.h"
#include "Tools/Math/Vector3.h"

class CameraCalibration : public Streamable
{
private:
  virtual void serialize(In* in, Out* out)
  {
    STREAM_REGISTER_BEGIN();
    STREAM(cameraTiltCorrection);
    STREAM(cameraRollCorrection);
    STREAM(bodyTiltCorrection);
    STREAM(bodyRollCorrection);
    STREAM(bodyTranslationCorrection);
    STREAM_REGISTER_FINISH()
  }

public:
  double cameraTiltCorrection, /**< The correction of the camera tilt angle in radians. */
         cameraRollCorrection, /**< The correction of the camera roll angle in radians. */
         bodyTiltCorrection, /**< The correction of the body tilt angle in radians. */
         bodyRollCorrection; /**< The correction of the body roll angle in radians. */
  Vector3<> bodyTranslationCorrection; /**< The correction of the body translation in mm. */

  /** 
  * Default constructor.
  */
  CameraCalibration() : cameraTiltCorrection(0.0), cameraRollCorrection(0.0), bodyTiltCorrection(0.0), bodyRollCorrection(0.0) {}        
};

#endif
