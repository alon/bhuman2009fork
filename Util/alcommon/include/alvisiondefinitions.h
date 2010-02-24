/**
* @author Aldebaran Robotics 
* Aldebaran Robotics (c) 2007 All Rights Reserved - This file is confidential.\n
*
* Version : $Id$
*/


#ifndef ALVISIONDEFINITIONS_H
# define ALVISIONDEFINITIONS_H


// deprecated debbuging macro
#define DEB(x) /* x*/

namespace AL 
{

/**
*Format of the image
*/
const int kVGA = 2;                // 640*480
const int kQVGA =1;                // 320*240
const int kQQVGA = 0;              // 160*120



// Utility function that checks that a resolution index is valid.
// (ie, present in the definitions just above)
static inline bool isResolutionValid(int resIndex)
{
  return (resIndex >= 0 && resIndex <= kVGA);
}


// Utility function that takes a resolution index as an input and returns
// the corresponding width and height values.
// If resolution index is unknown, width and height are set to -1.
static inline void getSizeFromResolution(int resIndex, int& width, int& height) 
{
  switch (resIndex) 
  {
    case kVGA:
      width  = 640;
      height = 480;
      break;
    case kQVGA:
      width  = 320;
      height = 240;
      break;
    case kQQVGA:
      width  = 160;
      height = 120;
      break;
    default:
      width = -1;
      height = -1;
      break;
  }
}

// Utility function that takes width and height as inputs and returns the
// corresponding resolution index. If no resolution index is found, the return
// value is -1.
static inline int getResolutionFromSize(int width, int height) 
{
  if (width == 640 && height == 480)
    return kVGA;

  if (width == 320 && height == 240)
    return kQVGA;

  if (width == 160 && height == 120)
    return kQQVGA;

  return -1;
}


/**
*ColorSpace
*/
const int kYuvColorSpace = 0;
const int kyUvColorSpace = 1;
const int kyuVColorSpace = 2;
const int kRgbColorSpace = 3;
const int krGbColorSpace = 4;
const int krgBColorSpace = 5;
const int kHsyColorSpace = 6;
const int khSyColorSpace = 7;
const int khsYColorSpace = 8;
const int kYUV422InterlacedColorSpace = 9;
const int kYUVColorSpace = 10;
const int kRGBColorSpace = 11;
const int kHSYColorSpace = 12;
const int kBGRColorSpace = 13; //for opencv ease of use
const int kYYCbCrColorSpace = 14; //for tiff io implementation 
const int kH2RGBColorSpace = 15; //H from HSY to RGB in fake colors
const int kHSMixedColorSpace = 16; //HS and (H +S)/2


// Utility function that checks that a color space index is valid.
// (ie, present in the definitions just above)
static inline bool isColorSpaceValid(int colorspace) 
{
  return (colorspace >= 0 && colorspace <= kHSMixedColorSpace);
}


// Utility function that returns the number of layers for a given color space
// defined above. If color space is unknown the function returns -1.
static inline int getNumLayersInColorSpace(int colorSpace)
{
  switch(colorSpace)
  {
    case kYuvColorSpace : case kyUvColorSpace : case kyuVColorSpace :
    case kRgbColorSpace : case krGbColorSpace : case krgBColorSpace :
    case kHsyColorSpace : case khSyColorSpace : case khsYColorSpace :
      return 1;

    case kYUV422InterlacedColorSpace : case kYYCbCrColorSpace :
      return 2;

    case kYUVColorSpace : case kRGBColorSpace : case kBGRColorSpace :
    case kHSYColorSpace : case kH2RGBColorSpace : case kHSMixedColorSpace :
      return 3;
    default :
      return -1;
  }
}



/**
*Scale methods
*/
const int kSimpleScaleMethod = 0;
const int kAverageScaleMethod = 1;
const int kQualityScaleMethod = 2;
const int kNoScaling = 3;


/**
 *Standard Id
 */

const int kCameraBrightnessID       = 0;
const int kCameraContrastID         = 1;
const int kCameraSaturationID       = 2;
const int kCameraHueID              = 3;
const int kCameraRedChromaID        = 4;
const int kCameraBlueChromaID       = 5;
const int kCameraGainID             = 6;
const int kCameraHFlipID            = 7;
const int kCameraVFlipID            = 8;
const int kCameraLensXID            = 9;
const int kCameraLensYID            = 10;
const int kCameraAutoExpositionID   = 11;
const int kCameraAutoWhiteBalanceID = 12;
const int kCameraAutoGainID         = 13;
const int kCameraResolutionID       = 14;
const int kCameraFrameRateID        = 15;
const int kCameraBufferSizeID       = 16;
const int kCameraExposureID         = 17;
const int kCameraSelectID           = 18;
const int kCameraSetDefaultParamsID = 19;
const int kCameraColorSpaceID       = 20;
const int kCameraExposureCorrectionID = 21;
const int kCameraAecAlgorithmID     = 22;
const int kCameraFastSwitchID       = 23;
const int kCameraSharpnessID        = 24;

} // namespace AL


#endif // ALVISIONLAYEREXTRACTOR_H
