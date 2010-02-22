/**
* @file ColorClassImage.h
*
* Declaration of class ColorClassImage
*
* @author Martin Lötzsch
*/

#ifndef __ColorClassImage_h_
#define __ColorClassImage_h_

#include "Tools/ColorClasses.h"
#include "Tools/Streams/InOut.h"

#include "Representations/Infrastructure/Image.h"

/**
* Represents a color segmented image. This doesn't have to be used by
* image processors, because they might be faster without generating a 
* representation like ColorClassImage, but it is needed for visualizing
* how color tables segment images.
*
* @author Martin Lötzsch
*/
class ColorClassImage
{
public:
  /** Constructor */
  ColorClassImage();

  void convertToImage(Image& image);

  /** A bitmap containing the color class for every pixel in the orignial image */
  unsigned char image[cameraResolutionHeight][cameraResolutionWidth];

  /** the height of the image */
  int height;

  /**  the width of the image  */
  int width;
};

/**
 * Streaming operator that writes a ColorClassImage to a stream.
 * @param stream The stream to write on.
 * @param colorClassImage The ColorClassImage object.
 * @return The stream.
 */ 
Out& operator << (Out& stream, const ColorClassImage& colorClassImage);

/**
 * Streaming operator that reads a ColorClassImage from a stream.
 * @param stream The stream from which is read.
 * @param colorClassImage The ColorClassImage object.
 * @return The stream.
 */ 
In& operator>>(In& stream,ColorClassImage& colorClassImage);


#endif //__ColorClassImage_h_
