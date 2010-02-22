/** 
* @file ColorTable64.h
* Declaration of class ColorTable64.
*
* @author <A href="mailto:juengel@informatik.hu-berlin.de">Matthias Jüngel</A>
* @author <A href="mailto:martin@martin-loetzsch.de">Martin Lötzsch</A>
*/

#ifndef _ColorTable64_h_
#define _ColorTable64_h_

#include "Tools/Math/Vector3.h"
#include "Representations/Perception/ColorClassImage.h"
#include "Representations/Infrastructure/Image.h"

/**
* @class ColorTable64
*
* Contains a ColorTable64 which can decode the color for
* every 4 * 4 * 4 cube in the 255 * 255 * 255 YUV color space.
*
* @author <A href="mailto:juengel@informatik.hu-berlin.de">Matthias Jüngel</A>
* @author <A href="mailto:martin@martin-loetzsch.de">Martin Lötzsch</A>
*/
class ColorTable64
{
public:
  /** Constructor */
  ColorTable64();

  /** 
  * Constructor that creates a copy of another ColorTable64.
  * \param ct The other ColorTable64.
  */
  ColorTable64(const ColorTable64& ct);

  /** Assignment operator
  * \param other The other ColorTable64 that is assigned to this one.
  * \return A reference to this object after the assignment.
  */
  ColorTable64& operator=(const ColorTable64& other);

  /** 
  * Calculates the color class of a pixel.
  * @param y the y value of the pixel
  * @param u the u value of the pixel
  * @param v the v value of the pixel
  * @return the color class
  */
  ColorClasses::Color getColorClass(const unsigned char y, 
    const unsigned char u, 
    const unsigned char v) const
  {
    return (ColorClasses::Color) colorClasses[y/4][u/4][v/4];
  }

  /**
  * Segments an image to an color class image.
  * 
  * This doesn't need to used in the image processor, but is needed for visualisation of color tables.
  * @param image A reference to the image to be segmented
  * @param colorClassImage A reference to the color class image to be created
  */
  void generateColorClassImage(const Image& image, ColorClassImage& colorClassImage) const;

  /**
  * Visualizes the difference between this and another ColorTable64 in an Image.
  * \param colorTable The other ColorTable64.
  * \param source The images used for detecting the differences.
  * \param view The image used for visulisation.
  * \param image The output image.
  */
  void generateDiffImage(const ColorTable64 colorTable, const Image& source, const Image& view, Image& image) const;

  /** 
  * The color table.
  * Each element in the array contains the color class of a 4x4x4 cube in the color space.
  */
  unsigned char colorClasses[64][64][64];

  /** Sets the color class of every 4x4x4 to noColor */
  void clear();

  /** Sets all cubes that have the given color class to noColor */
  void clearChannel(ColorClasses::Color colorClass);

  /** 
  * Sets the color class for a cube with the size "range" around a pixel
  * given by y,u,v to the given color class.
  */
  void addColorClass(ColorClasses::Color colorClass,
                     unsigned char y, 
                     unsigned char u, 
                     unsigned char v, 
                     unsigned char range);

  /** 
  * Sets the color class for a pixel
  * given by y,u,v to the given color class.
  */
  void addColorClass(ColorClasses::Color colorClass,
                                 unsigned char y,
                                 unsigned char u,
                                 unsigned char v);

  /** 
  * Replaces one color class by another for a cube with the size "range" around a pixel
  * given by y,u,v to the given color class.
  */
  void replaceColorClass(ColorClasses::Color from,
                         ColorClasses::Color to,
                         unsigned char y, 
                         unsigned char u, 
                         unsigned char v, 
                         unsigned char range);

  /** 
  * Replaces one color class by another for a pixel
  * given by y,u,v to the given color class.
  */
  void replaceColorClass(ColorClasses::Color from,
                         ColorClasses::Color to,
                         unsigned char y, 
                         unsigned char u, 
                         unsigned char v);

  /**
  * This function marks the outer pixels of all segmented colors in the colorspace as noColor.
  */
  void shrink();

  /**
  * This function marks the outer pixels of the parameter color in the colorspace as noColor.
  * @param color The color to shrink
  */
  void shrink(unsigned char color);

  /**
  * This function marks all noColor pixels in the colorspace, which are neighbors of segmented 
  * pixels, with the neighboring color.
  */
  void grow();

  /**
  * This function marks all noColor pixels in the colorspace, which are neighbors of the parameter color, 
  * with the neighboring color.
  */
  void grow(unsigned char color);

private:   
  /**
  * Returns true if the pixel y,u,v in the  colorClassesArray has more than x or x neighbors.
  * @param y the y coordinate of the pixel in the colorspace
  * @param u the u coordinate of the pixel in the colorspace
  * @param v the v coordinate of the pixel in the colorspace
  * @param x the number of desired pixels
  * @param colorClassesArray the Array with the colorClasses
  */
  bool hasXNeighbors(unsigned char y, unsigned char u, unsigned char v, int x, unsigned char colorClassesArray[64][64][64]);

  /**
  * Returns the color of one of the neighboring pixels of the pixel y,u,v.
  * If no direct neighbor is present, noColor s returned.
  * @param y the y coordinate of the pixel in the colorspace
  * @param u the u coordinate of the pixel in the colorspace
  * @param v the v coordinate of the pixel in the colorspace
  * @param colorClassesArray the Array with the colorClasses
  */
  unsigned char getNeighborColor(unsigned char y, unsigned char u, unsigned char v, unsigned char colorClassesArray[64][64][64]);
};

/**
* Streaming operator that writes a ColorTable64 to a stream.
* @param stream The stream to write on.
* @param colorTable64 The ColorTable64 object.
* @return The stream.
*/ 
Out& operator<<(Out& stream, const ColorTable64& colorTable64);

/**
* Streaming operator that reads a ColorTable64 from a stream.
* @param stream The stream from which is read.
* @param colorTable64 The ColorTable64 object.
* @return The stream.
*/ 
In& operator>>(In& stream,ColorTable64& colorTable64);

#endif   //  _ColorTable64_h_
