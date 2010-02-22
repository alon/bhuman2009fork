/**
 * @file Image.h
 * 
 * Declaration of class Image
 */ 

#ifndef __Image_h_
#define __Image_h_

#include "CameraInfo.h"

#ifdef _WIN32
#define anonymous_struct struct
#else
#define anonymous_struct __extension__ struct
#endif

/**
* Platform independend definition of an image class
*/
class Image : public Streamable
{
private:
  void serialize(In* in, Out* out);

public:
  /**
  * Default constructor.
  */
  Image();

  Image(Settings::Model model);

  /** destructs an image */
  ~Image() {if(!isReference) delete [] image;}

  /**
  * Copy constructor.
  * @param other The image this is copied from.
  */
  Image(const Image& other)
    : isReference(true)
  {
    *this = other;
  }

  /**
  * Assignment operator.
  * @param other The image this is copied from.
  * @return This image.
  */
  Image& operator=(const Image& other);

  /**
  * The method sets an external image.
  * @param buffer The image buffer.
  */
  void setImage(const unsigned char* buffer);

  /** Converts an YCbCr image into an RGB image.
  *  @param ycbcrImage The given YCbCr image
  */
  void convertFromYCbCrToRGB(const Image& ycbcrImage);

  /** Converts an RGB image into an YCbCr image.
  *  @param rgbImage The given RGB image
  */
  void convertFromRGBToYCbCr(const Image& rgbImage);

  /** Converts an YCbCr image into a HSI image.
  *  @param ycrcbImage The given YCbCr image
  */
  void convertFromYCbCrToHSI(const Image& ycrcbImage);

  /** Converts a HSI image into an YCbCr image.
  *  @param hsiImage The given HSI image
  */
  void convertFromHSIToYCbCr(const Image& hsiImage);

  /** Converts an YCbCr image into a TSL image.
  *  @param ycbcrImage The given YCbCr image
  */
  void convertFromYCbCrToTSL(const Image& ycbcrImage);

  /**
  * The union defines a pixel in YCbCr space.
  */
  union Pixel
  {
    unsigned color; /**< Representation as single machine word. */
    unsigned char channels[4];  /**< Representation as an array of channels. */
    anonymous_struct
    {
      unsigned char yCbCrPadding, /**< Y channel. */
                    cb, /**< Cb channel. */
                    y, /**< Ignore. */
                    cr; /**< Cr channel. */
    };
    anonymous_struct
    {
      unsigned char r, /**< R channel. */
                    g, /**< G channel. */
                    b, /**< B channel. */
                    rgbPadding; /**< Ignore. */
    };
    anonymous_struct
    {
      unsigned char h, /**< H channel. */
                    s, /**< S channel. */
                    i, /**< I channel. */
                    hsiPadding; /**< Ignore. */
    };
    anonymous_struct
    {
      unsigned char t, /**< T channel. */
                    _s, /**< S channel. */
                    l, /**< L channel. */
                    tslPadding; /**< Ignore. */
    };
  };

  Pixel (*image)[cameraResolutionWidth * 2]; /**< The image. Please not that the second half of each row must be ignored. */
  CameraInfo cameraInfo; /**< Information on the camera used to take this image. */
  unsigned timeStamp; /**< The time stamp of this image. */
  bool isReference; /**< States whether this class holds the image, or only a reference to an image stored elsewhere. */

   /**
   * The function converts an address to pixel coordinates.
   * @param p An address in image.image.
   * @return The x- and y-coordinates of the corresponding pixel.
   */
  inline Vector2<int> getCoords(const Image::Pixel* p) const
  {
    const int diff(p - &image[0][0]);
    return Vector2<int>(diff % (cameraResolutionWidth*2), diff / (cameraResolutionWidth*2));
  }

  /**
   * Calculates the distance between the first three bytes of two colors
   * @param a The first color
   * @param b The second color
   * @return The distance
   */
  static inline double getColorDistance(const Image::Pixel& a, const Image::Pixel& b)
  {
    int dy = int(a.y)-b.y;
    int dcb = int(a.cb)-b.cb;
    int dcr = int(a.cr)-b.cr;
    dy *= dy;
    dcb *= dcb;
    dcr *= dcr;
    return sqrt(double(dy+dcb+dcr));
  }
};

#endif //__Image_h_
