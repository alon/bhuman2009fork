/**
* @file ColorClassImage.cpp
*
* Implementation of class ColorClassImage
*
* @author Martin Lötzsch
*/

#include "ColorClassImage.h"
#include <string.h>


ColorClassImage::ColorClassImage() : height(cameraResolutionHeight), width(cameraResolutionWidth)
{
  memset(image,0,sizeof(image));
}

void ColorClassImage::convertToImage(Image& imageOut)
{
  imageOut.cameraInfo.resolutionHeight = height;
  imageOut.cameraInfo.resolutionWidth = width;

  unsigned char yy[256],
                cr[256],
                cb[256];
  for(int i = 0; i < ColorClasses::numOfColors; ++i)
    ColorClasses::getColorClassColor((ColorClasses::Color) i, yy[i], cr[i], cb[i]);
  for(int i = 129; i < 256; ++i)
  {
    int ySum = 0,
        crSum = 0,
        cbSum = 0,
        count = 0;
    for(int j = 0; j < 7; ++j)
      if(i & 1 << j)
      {
        ColorClasses::getColorClassColor((ColorClasses::Color) (j + 1), yy[i], cr[i], cb[i]);
        ySum += yy[i];
        crSum += cr[i];
        cbSum += cb[i];
        ++count;
      }
    yy[i] = (unsigned char) (ySum / count);
    cr[i] = (unsigned char) (crSum / count);
    cb[i] = (unsigned char) (cbSum / count);
  }
  for(int y = height-1; y >= 0; --y) 
    for(int x = width-1; x >= 0; --x)
    {
      Image::Pixel& set = imageOut.image[y][x];
      const unsigned char get = image[y][x];
      set.y = yy[get];
      set.cr = cr[get];
      set.cb = cb[get];
    }
}

Out& operator << (Out& stream, const ColorClassImage& colorClassImage)
{
  stream << colorClassImage.height << colorClassImage.width;

  // the current color class in the color table
  unsigned char currentColorClass = (unsigned char)colorClassImage.image[0][0];

  // the number of bytes in one sequence that have the same color class
  int currentLength = 0;

  for(int y = 0; y < colorClassImage.height; ++y)
    for(int x = 0; x < colorClassImage.width; ++x)
    {
      unsigned char nextColorClass = (unsigned char) colorClassImage.image[y][x];
    
      if (nextColorClass != currentColorClass)
      {
        stream << currentLength << currentColorClass;
        currentColorClass = nextColorClass;
        currentLength = 1;
      }
      else
        ++currentLength;
    }
  return stream << currentLength << currentColorClass;
}

In& operator>>(In& stream,ColorClassImage& colorClassImage)
{
  // the current color class in the color table
  unsigned char currentColorClass;

  // the number of bytes in one sequence that have the same color class
  unsigned int currentLength = 0;

  stream >> colorClassImage.height >> colorClassImage.width;

  for(int y = 0; y < colorClassImage.height; ++y)
    for(int x = 0; x < colorClassImage.width; ++x)
    {
      if(!currentLength)
        stream >> currentLength >> currentColorClass;
      colorClassImage.image[y][x] = currentColorClass;
      --currentLength;
    }
  return stream;
}
