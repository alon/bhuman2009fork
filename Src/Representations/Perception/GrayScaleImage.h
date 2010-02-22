/**
 * @file GrayScaleImage.h
 * 
 * Declaration of class GrayScaleImage
 */ 

#ifndef __GrayScaleImage_h_
#define __GrayScaleImage_h_

#include <cstring>
#include "Representations/Infrastructure/Image.h"
#include "Tools/Streams/InOut.h"


/**
* Platform independend definition of an image class
*/
class GrayScaleImage
{
public:
  /** constructs an image */
  GrayScaleImage();

  void copyChannel(const Image& yuvImage, int channel);

  GrayScaleImage& operator=(const GrayScaleImage& other) 
  {
    memcpy((void *)this, (const void *)&other, sizeof(GrayScaleImage));
    return *this;
  }
  
  CameraInfo cameraInfo;
  unsigned char image[cameraResolutionHeight][cameraResolutionWidth];
};



/**
 * Streaming operator that writes a GrayScaleImage to a stream.
 * @param stream The stream to write on.
 * @param grayScaleImage The GrayScaleImage object.
 * @return The stream.
 */ 
Out& operator<<(Out& stream, const GrayScaleImage& grayScaleImage);

/**
 * Streaming operator that reads a GrayScaleImage to a stream.
 * @param stream The stream to read from.
 * @param grayScaleImage The GrayScaleImage object.
 * @return The stream.
 */ 
In& operator>>(In& stream, GrayScaleImage& grayScaleImage);


#endif //__GrayScaleImage_h_
