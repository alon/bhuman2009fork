/**
 * @file JPEGImage.cpp
 * 
 * Implementation of class JPEGImage
 */ 

#include "Platform/GTAssert.h"
#include "JPEGImage.h"

JPEGImage::JPEGImage(const Image& image)
{
  *this = image;
}

JPEGImage& JPEGImage::operator=(const Image& src)
{
  cameraInfo = src.cameraInfo;
  timeStamp = src.timeStamp;

  jpeg_compress_struct cInfo;
  jpeg_error_mgr jem;
  cInfo.err = jpeg_std_error(&jem);
  jpeg_create_compress(&cInfo);

  if(!cInfo.dest)
    cInfo.dest = (jpeg_destination_mgr*)
      (*cInfo.mem->alloc_small) ((j_common_ptr) &cInfo,JPOOL_PERMANENT,
                                 sizeof(DestDescriptor));
  cInfo.dest->init_destination = onDestInit;
  cInfo.dest->empty_output_buffer = onDestEmpty;
  cInfo.dest->term_destination = onDestTerm;
  ((DestDescriptor*) cInfo.dest)->theObject = this;

  cInfo.image_width = cameraInfo.resolutionWidth;
  cInfo.image_height = cameraInfo.resolutionHeight;
  cInfo.input_components = 4;
  cInfo.in_color_space = JCS_CMYK;
  cInfo.jpeg_color_space = JCS_CMYK;
  jpeg_set_defaults(&cInfo);
  cInfo.dct_method = JDCT_FASTEST;
  jpeg_set_quality(&cInfo,75,true);

  jpeg_start_compress(&cInfo,true);

  while(cInfo.next_scanline < cInfo.image_height) 
  {
    JSAMPROW rowPointer = const_cast<JSAMPROW>((unsigned char*) &src.image[cInfo.next_scanline]);
    jpeg_write_scanlines(&cInfo,&rowPointer, 1);
  }

  jpeg_finish_compress(&cInfo);
  jpeg_destroy_compress(&cInfo);
  return *this;
}

void JPEGImage::toImage(Image& dest) const
{
  dest.cameraInfo = cameraInfo;
  dest.timeStamp = timeStamp;

  jpeg_decompress_struct cInfo;
  jpeg_error_mgr jem;
  cInfo.err = jpeg_std_error(&jem);

  jpeg_create_decompress(&cInfo);

  if(!cInfo.src)
    cInfo.src = (jpeg_source_mgr *)
      (*cInfo.mem->alloc_small)((j_common_ptr) &cInfo,JPOOL_PERMANENT,
                                 sizeof(jpeg_source_mgr));
  cInfo.src->init_source       = onSrcIgnore;
  cInfo.src->fill_input_buffer = onSrcEmpty;
  cInfo.src->skip_input_data   = onSrcSkip;
  cInfo.src->resync_to_restart = jpeg_resync_to_restart;
  cInfo.src->term_source       = onSrcIgnore;
  cInfo.src->bytes_in_buffer = size;
  cInfo.src->next_input_byte = (const JOCTET*) image;

  jpeg_read_header(&cInfo,true);
  jpeg_start_decompress(&cInfo);

  // setup rows
  while (cInfo.output_scanline < cInfo.output_height) 
  {
    JSAMPROW rowPointer = (unsigned char*) (&dest.image[cInfo.output_scanline]);
    (void) jpeg_read_scanlines(&cInfo,&rowPointer,1);
  }

  // decompress
  jpeg_finish_decompress(&cInfo);
  jpeg_destroy_decompress(&cInfo);

}

void JPEGImage::onDestInit(j_compress_ptr cInfo)
{
  JPEGImage& image = *((DestDescriptor*) cInfo->dest)->theObject;
  cInfo->dest->next_output_byte = (JOCTET*) image.image;
  cInfo->dest->free_in_buffer = image.cameraInfo.resolutionWidth * image.cameraInfo.resolutionHeight;
}

int JPEGImage::onDestEmpty(j_compress_ptr)
{
  ASSERT(false);
  return false;
}

void JPEGImage::onDestTerm(j_compress_ptr cInfo)
{
  JPEGImage& image = *((DestDescriptor*) cInfo->dest)->theObject;
  image.size = (char*) cInfo->dest->next_output_byte - (char*) image.image;
}

void JPEGImage::onSrcSkip(j_decompress_ptr,long)
{
}

int JPEGImage::onSrcEmpty(j_decompress_ptr)
{
  ASSERT(false);
  return false;
}

void JPEGImage::onSrcIgnore(j_decompress_ptr)
{
}

void JPEGImage::serialize(In* in, Out* out)
{
  int& resolutionWidth(cameraInfo.resolutionWidth);
  int& resolutionHeight(cameraInfo.resolutionHeight);
  unsigned char* image = (unsigned char*) this->image;

  STREAM_REGISTER_BEGIN();
  STREAM(resolutionWidth);
  STREAM(resolutionHeight);
  STREAM(timeStamp);
  STREAM_DYN_ARRAY(image, size);
  STREAM_REGISTER_FINISH();

  if(in)
    cameraInfo = CameraInfo();
}
