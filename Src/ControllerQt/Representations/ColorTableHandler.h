/**
* @file ControllerQt/Representations/ColorTableHandler.h
*
* Declaration of class ColorTableHandler
*
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
*/

#ifndef __ColorTableHandler_h_
#define __ColorTableHandler_h_

#include "Representations/Configuration/ColorTable64.h"
#include "Tools/Math/Vector3.h"
#include "Tools/MessageQueue/InMessage.h"
#include <deque>

/**
* @class ColorTableHandler 
* 
* A class that represents and manipulates color tables.
*
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
*/
class ColorTableHandler
{
private:
  std::deque<void*> buffer;
  
  /**
  * The function stores the current color table in the undo buffer.
  */
  void push();

  /**
  * Creates a segment of a rectangle of an image and adds it's colors to the color table.
  * @param image The image containing the color information.
  * @param topLeft The top left corner of the image region to add.
  * @param bottomRight The bottom right corner of the image region to add.
  */
  void addSegment(const Image& image, const Vector2<int>& topLeft, const Vector2<int>& bottomRight);

public:
  ColorTable64& colorTable; /**< The color table. */
  unsigned timeStamp; /**< The time stamp of the last change of the color table. */
  ColorClasses::Color selectedColorClass, /**< The currently selected color class. */
                      maskColorClass; /**< If != none, only pixels of this color class will be updated. */
  int colorSpaceRadius, /**< The radius in color space filled with the selected class. 0 fills only a single pixel. */
      imageRadius; /**< The of the image area surrounding the selected pixel that is added to the color table. 0 only uses the selected pixel. */
  bool active, /**< States whether modification of the color table is activated or not. */
       replace, /**< States whether color classes will replace existing one or added as additional ones. */
       smart;
  

  /**
  * Default constructor.
  * Loads the color table for this location and robot design.
  */
  ColorTableHandler();

  /**
  * Destructor.
  */
  ~ColorTableHandler();

  /**
  * The function handles a stop watch message.
  * @param message The message.
  * @return Was it a stop watch message?
  */
  bool handleMessage(InMessage& message);

  /**
  * The function reverts the previous step.
  */
  void undo();

  /**
  * Adds a region surrounding the selected coordinates of an image to the color table.
  * The size of the region is determined by member "imageRadius". For each color in this
  * region, a cube of radius "colorSpaceRadius" in the color table is set to the selected color.
  * @param image The image containing the color information.
  * @param pos The center of the image region to add.
  */
  void addPixel(const Image& image, const Vector2<int>& pos);

  /**
  * Adds a rectangle of an image to the color table.
  * @param image The image containing the color information.
  * @param topLeft The top left corner of the image region to add.
  * @param bottomRight The bottom right corner of the image region to add.
  */
  void addRectangle(const Image& image, const Vector2<int>& topLeft, const Vector2<int>& bottomRight);

  /**
  * Adds all pixel in a rectangle of an image not classified so far to the color table.
  * @param image The image containing the color information.
  * @param topLeft The top left corner of the image region to add.
  * @param bottomRight The bottom right corner of the image region to add.
  */
  void fillRectangle(const Image& image, const Vector2<int>& topLeft, const Vector2<int>& bottomRight);

  /**
  * Removes all colors in a region surrounding the selected coordinates of an image from the 
  * color table if they are classified as the selected color class.
  * The size of the region is determined by member "imageRadius".
  * @param image The image containing the color information.
  * @param pos The center of the image region to remove.
  */
  void clearPixel(const Image& image, const Vector2<int>& pos);

  /**
  * The function removes all colors in a certain rectangle from the color table 
  * that are classified as the selected color class.
  * @param image The image containing the color information.
  * @param topLeft The top left corner of the image region to add.
  * @param bottomRight The bottom right corner of the image region to add.
  */
  void clearRectangle(const Image& image, const Vector2<int>& topLeft, const Vector2<int>& bottomRight);

  /**
  * The function clears the whole color table.
  */
  void clear();

  /**
  * The function clears the selected color channel.
  * @param color The color channel to clear.
  */
  void clear(ColorClasses::Color color);

  /**
  * The function removes all colors of an image from the color table 
  * that are classified as the given color class.
  * @param image The image containing the color information.
  * @param color The color class to remove.
  */
  void clearPixels(const Image& image, ColorClasses::Color color);

  /**
  * The function loads the color table from a file.
  * @param fileName The file name of the color table. Relative file names will be
  *                 interpreted relative to the location and robot design path. If 
  *                 the extension is missing, it will be added.
  * @return Did the file exist?
  */
  bool load(const std::string& fileName);

  /**
  * The function saves the color table under the given file name.
  * @param fileName The file name of the color table. Relative file names will be
  *                 interpreted relative to the location and robot design path. If 
  *                 the extension is missing, it will be added.
  */
  void save(const std::string& fileName) const;

  /**
  * The function sends the color table to a stream.
  * @param message The message the color table will be stored in.
  */
  void send(OutMessage& message) const;

  /**
  * The function sends the color table to a stream. It will be save to the memory 
  * stick by the robot
  * @param message The message the color table will be stored in.
  */
  void sendAndWrite(OutMessage& message) const;

  /**
  * This function marks the outer pixels of all segmented colors in the colorspace as noColor.
  */
  void shrink();

  /**
  * This function marks the outer pixels of the parameter color in the colorspace as noColor.
  * &param color The color to shrink
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

  /**
  * Replaces the current color table with a new one.
  * \param colorTable The new one.
  */
  void set(const ColorTable64& colorTable);
};

#endif //__ColorTableHandler_h_
