/**
 * @file GrayScaleImage.cpp
 *
 * Implementation of class GrayScaleImage.
 */

#include "GrayScaleImage.h"


GrayScaleImage::GrayScaleImage()
{
  for(int y = 0; y < cameraInfo.resolutionHeight; ++y)
    for(int x = 0; x < cameraInfo.resolutionWidth; ++x)
    {
      image[y][x] = y;
    }
}

void GrayScaleImage::copyChannel(const Image& rgbImage, int channel)
{
  for(int y=0; y < rgbImage.cameraInfo.resolutionHeight; y++)
    for(int x=0; x < rgbImage.cameraInfo.resolutionWidth; x++)
      image[y][x] = rgbImage.image[y][x].channels[channel];
  this->cameraInfo = rgbImage.cameraInfo;
}

Out& operator<<(Out& stream, const GrayScaleImage& grayScaleImage)
{
  stream << grayScaleImage.cameraInfo.resolutionWidth
         << grayScaleImage.cameraInfo.resolutionHeight; 
  stream.write(&grayScaleImage.image[0][0], 
                grayScaleImage.cameraInfo.resolutionWidth*grayScaleImage.cameraInfo.resolutionHeight);
  return stream;
}

In& operator>>(In& stream, GrayScaleImage& grayScaleImage)
{
  stream >> grayScaleImage.cameraInfo.resolutionWidth
         >> grayScaleImage.cameraInfo.resolutionHeight; 
  stream.read(&grayScaleImage.image[0][0], 
               grayScaleImage.cameraInfo.resolutionWidth*grayScaleImage.cameraInfo.resolutionHeight);
  return stream;
}
