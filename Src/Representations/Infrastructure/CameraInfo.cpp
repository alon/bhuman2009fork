#include "CameraInfo.h"

void CameraInfo::init(Settings::Model model)
{
  resolutionWidth  = cameraResolutionWidth;
  resolutionHeight = cameraResolutionHeight;
  
  openingAngleWidth = 0.78674; // 45.08°
  openingAngleHeight  = 0.60349; // 34.58°

  focalLength = 385.54;
  opticalCenter.x = 160.0; // unchecked
  opticalCenter.y = 120.0; // unchecked

  calcAdditionalConstants();
}


