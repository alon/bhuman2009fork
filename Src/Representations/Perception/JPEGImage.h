/**
 * @file JPEGImage.h
 * 
 * Declaration of class JPEGImage
 */ 

#ifndef __JPEGImage_h_
#define __JPEGImage_h_

#include "Representations/Infrastructure/Image.h"

#ifndef LINUX

// INT32 and FAR conflict with any other header files...
#define INT32 _INT32
#undef FAR

// "boolean" conflicts with "rpcndr.h", so we force "jpeglib.h" not to define boolean
#ifdef __RPCNDR_H__
#define HAVE_BOOLEAN
#endif

#include <../Util/libjpeg/src/jpeglib.h>

#undef INT32
#undef FAR


#else
extern "C"
{
#include <../Util/libjpeg/src/jpeglib.h>
}
#endif

/**
 * Definition of a class for JPEG-compressed images.
 */
class JPEGImage : public Image
{
private:
  void serialize(In* in, Out* out);

  /**
  * The class is required during JPEG-compression to access
  * the main JPEGImage object from library handlers.
  */
  class DestDescriptor : public jpeg_destination_mgr
  {
  public:
    JPEGImage* theObject; /**< A pointer to the JPEGImage object. */
  };

  int size; /**< The size of the JPEG image. */

  //!@name Handlers for JPEG-compression
  //!@{
  static void onDestInit(j_compress_ptr cInfo);
  static int onDestEmpty(j_compress_ptr);
  static void onDestTerm(j_compress_ptr cInfo);

  static void onSrcSkip(j_decompress_ptr cInfo,long numBytes);
  static int onSrcEmpty(j_decompress_ptr);
  static void onSrcIgnore(j_decompress_ptr);
  //!@}

public:
  /** 
  * Empty constructor. 
  */
  JPEGImage() : size(0) {}

  /** 
  * Constructs a JPEG image from an image.
  * @param src The image used as template.
  */
  JPEGImage(const Image& src);

  /**
  * Assignment operator.
  * @param src The image used as template.
  * @return The resulting JPEG image.
  */
  JPEGImage& operator=(const Image& src);

  /**
  * Uncompress image.
  * @param dest Will receive the uncompressed image.
  */
  void toImage(Image& dest) const;
};

#endif //__JPEGImage_h_
