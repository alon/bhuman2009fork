/**
 * @file Image.cpp
 *
 * Implementation of class Image.
 */

#include <cstring>
#include "Platform/GTAssert.h"
#include "Image.h"
#include "Tools/Math/Common.h"
#include "Tools/ImageProcessing/ColorModelConversions.h"

Image::Image()
: timeStamp(0),
  isReference(false)
{
  image = new Pixel[cameraResolutionHeight][cameraResolutionWidth * 2];
  for(int y = 0; y < cameraInfo.resolutionHeight; ++y)
    for(int x = 0; x < cameraInfo.resolutionWidth; ++x)
      image[y][x].color = 0x80008000;
}

Image::Image(Settings::Model model)
: cameraInfo(model),
  timeStamp(0),
  isReference(false)
{
  image = new Pixel[cameraResolutionHeight][cameraResolutionWidth * 2];
  for(int y = 0; y < cameraInfo.resolutionHeight; ++y)
    for(int x = 0; x < cameraInfo.resolutionWidth; ++x)
      image[y][x].color = 0x80008000;
}

Image& Image::operator=(const Image& other)
{
  if(isReference) 
  {
    image = new Pixel[cameraResolutionHeight][cameraResolutionWidth * 2];
    isReference = false;
  }
  cameraInfo = other.cameraInfo;
  timeStamp = other.timeStamp;
  for(int y = 0; y < cameraInfo.resolutionHeight; ++y)
    memcpy(&image[y], &other.image[y], cameraInfo.resolutionWidth * sizeof(Image::Pixel));
  return *this;
}

void Image::setImage(const unsigned char* buffer)
{
  if(!isReference) 
  {
    delete [] image;
    isReference = true;
  }
  image = (Pixel (*)[cameraResolutionWidth * 2]) buffer;
}

void Image::convertFromYCbCrToRGB(const Image& ycbcrImage)
{
  for(int y = 0; y < cameraInfo.resolutionHeight; ++y)
    for(int x = 0; x < cameraInfo.resolutionWidth; ++x)
      ColorModelConversions::fromYCbCrToRGB(ycbcrImage.image[y][x].y,
                                              ycbcrImage.image[y][x].cb,
                                              ycbcrImage.image[y][x].cr,
                                              image[y][x].r,
                                              image[y][x].g,
                                              image[y][x].b);
  this->cameraInfo = ycbcrImage.cameraInfo;
}

void Image::convertFromRGBToYCbCr(const Image& rgbImage)
{
  for(int y = 0; y < cameraInfo.resolutionHeight; ++y)
    for(int x = 0; x < cameraInfo.resolutionWidth; ++x)
      ColorModelConversions::fromRGBToYCbCr(rgbImage.image[y][x].r,
                                            rgbImage.image[y][x].g,
                                            rgbImage.image[y][x].b,
                                            image[y][x].y,
                                            image[y][x].cb,
                                            image[y][x].cr);
  this->cameraInfo = rgbImage.cameraInfo;
}

void Image::convertFromYCbCrToHSI(const Image& ycbcrImage)
{
  for(int y = 0; y < cameraInfo.resolutionHeight; ++y)
    for(int x = 0; x < cameraInfo.resolutionWidth; ++x)
      ColorModelConversions::fromYCbCrToHSI(ycbcrImage.image[y][x].y,
                                            ycbcrImage.image[y][x].cb,
                                            ycbcrImage.image[y][x].cr,
                                            image[y][x].h,
                                            image[y][x].s,
                                            image[y][x].i);
  this->cameraInfo = ycbcrImage.cameraInfo;
}

void Image::convertFromHSIToYCbCr(const Image& hsiImage)
{
  for(int y = 0; y < cameraInfo.resolutionHeight; ++y)
    for(int x = 0; x < cameraInfo.resolutionWidth; ++x)
      ColorModelConversions::fromHSIToYCbCr(hsiImage.image[y][x].h,
                                            hsiImage.image[y][x].s,
                                            hsiImage.image[y][x].i,
                                            image[y][x].y,
                                            image[y][x].cb,
                                            image[y][x].cr);
  this->cameraInfo = hsiImage.cameraInfo;
}

void Image::convertFromYCbCrToTSL(const Image& ycbcrImage)
{
  for(int y = 0; y < cameraInfo.resolutionHeight; ++y)
    for(int x = 0; x < cameraInfo.resolutionWidth; ++x)
      ColorModelConversions::fromYCbCrToTSL(ycbcrImage.image[y][x].y,
                                            ycbcrImage.image[y][x].cb,
                                            ycbcrImage.image[y][x].cr,
                                            image[y][x].t,
                                            image[y][x].s,
                                            image[y][x].l);
  this->cameraInfo = ycbcrImage.cameraInfo;
}

void Image::serialize(In* in, Out* out)
{
  int& resolutionWidth(cameraInfo.resolutionWidth);
  int& resolutionHeight(cameraInfo.resolutionHeight);

  STREAM_REGISTER_BEGIN();
  STREAM(resolutionWidth);
  STREAM(resolutionHeight);
  STREAM(timeStamp);

  if(out)
    for(int y = 0; y < cameraInfo.resolutionHeight; ++y)
      out->write(&image[y], cameraInfo.resolutionWidth * sizeof(Pixel));
  else
  {
    cameraInfo = CameraInfo();
    for(int y = 0; y < cameraInfo.resolutionHeight; ++y)
      in->read(&image[y], cameraInfo.resolutionWidth * sizeof(Pixel));
  }

  STREAM_REGISTER_FINISH();
}
