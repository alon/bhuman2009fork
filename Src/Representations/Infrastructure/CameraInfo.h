/**
* @file CameraInfo.h
* 
* Declaration of class CameraInfo
*
* @author <a href="mailto:juengel@informatik.hu-berlin.de">Matthias Juengel</a>
* @author <a href="mailto:walter.nistico@uni-dortmund.de">Walter Nistico</a>
*/ 

#ifndef __CameraInfo_h_
#define __CameraInfo_h_

#include "Tools/Math/Vector2.h"
#include "Tools/Settings.h"

const int cameraResolutionWidth = 320;
const int cameraResolutionHeight = 240;

/**
* Information about the camera which provides the images for the robot
*/
class CameraInfo : public Streamable
{
public:
  /**
  * Default constructor.
  */

  CameraInfo()
  {
    init(Global::getSettings().model);
  }
  CameraInfo(Settings::Model model)
  {
    init(model);
  }
  
  void init(Settings::Model model);

  virtual void serialize(In* in, Out* out)
  {
    STREAM_REGISTER_BEGIN();
    STREAM(resolutionWidth);
    STREAM(resolutionHeight);
    STREAM(openingAngleWidth);
    STREAM(openingAngleHeight);
    STREAM(focalLength);
    STREAM(opticalCenter);
    STREAM_REGISTER_FINISH();
    if(in)
      calcAdditionalConstants();
  }

  int resolutionWidth;
  int resolutionHeight;
  double openingAngleWidth;
  double openingAngleHeight;

  /** Intrinsic camera parameters: axis skew is modelled as 0 (90° perfectly orthogonal XY) 
  * and the same has been modeled for focal axis aspect ratio; distortion is considering
  * only 2nd and 4th order coefficients of radial model, which account for about 95% of total.
  */
  double focalLength;
  double focalLengthInv; // (1/focalLength) used to speed up certain calculations 
  Vector2<double> opticalCenter;
  double focalLenPow2;  
  double focalLenPow4;

private:

  /**
  * Calculates some additional constants based on focal length for faster calculations.
  */
  void calcAdditionalConstants()
  {
    focalLenPow2 = focalLength * focalLength;
    focalLenPow4 = focalLenPow2 * focalLenPow2;
    focalLengthInv = 1.0 / focalLength;
  }

};

#endif //__CameraInfo_h_
