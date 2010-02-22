/**
* @file FramebufferObject.h
* 
* Implementation of an easy interface to the framebuffer object extension
*
* @author <A href="mailto:pachur@informatik.uni-bremen.de">Dennis Pachur</A>
*/ 

#ifndef FRAMEBUFFEROBJECT_H_
#define FRAMEBUFFEROBJECT_H_

#include <GL/glew.h>
#include <vector>
#include <map>

#define MAX_NUM_FBOS 8
// normally 16 but render buffer could be used as texture replacements
#define MAX_NUM_FBO_ATTACHMENTS 8

class FramebufferObject
{
protected:
  GLuint id;
  GLuint blit_id;
  GLuint num_of_attachments;
  GLuint depthbuffer;
  GLuint blit_depthbuffer;
  bool   initialized;
  bool   antialiased;
  bool   oversized;
  bool   storage;
  GLuint samples;
  GLuint textures[MAX_NUM_FBO_ATTACHMENTS];
  GLenum texFilter[MAX_NUM_FBO_ATTACHMENTS];
  GLuint renderbuffers[MAX_NUM_FBO_ATTACHMENTS];
  GLuint blit_renderbuffers[MAX_NUM_FBO_ATTACHMENTS];
  GLuint width, height;
  GLuint oversize_width, oversize_height;
  GLenum colorBuffersExt[8];

  // destroy the object if created
  void destroy();

  // display the failure codes
  void displayStatus(const GLenum status) const;

public:
  bool   PUswapped, RPswapped;
  /**
  * Constructor
  */
  FramebufferObject(const int& attachments,
    const std::vector<GLenum>& textureFilter,
    const bool& oversized_, const bool& antialiased_,
    const int& samples_, const bool& storage_);

  /**
  * Desctructor
  */
  ~FramebufferObject();

  /** update and check values
  * @param width_ The width of the framebuffer (is modified if framebuffer is oversized)
  * @param height_ The height of the framebuffer (is modified if framebuffer is oversized)
  * @param forceNew Force the buffer objects to reinitialize itself
  * @return If the fbo is usable
  */
  bool update(const GLuint width_,
              const GLuint height_,
              const bool forceNew = false);

  /** check usablility of framebuffer object
  * @return The initialization status
  */
  bool usable()
  { return initialized; };

  /** get the framebuffer object internal id
  * @return The openGL fbo ID
  */
  GLuint getID()
  { return id; };

  /** set antialiasing flag
  * @param flag If multisampling is activated or not
  * @param numOfSamples The number of samples to use
  */
  void setAntialising(const bool flag, const GLuint numOfSamples);
  bool getAntialising()
  { return antialiased; };

  /** set framebuffer size
  * @param width_ The width of the framebuffer
  * @param height_ The height of the framebuffer
  */
  void setSize(const GLuint width_, const GLuint height_)
  { width = width_; height = height_; };

  /** get the number of attachments
  * @return the number of attachments
  */
  int getNumOfAttachments()
  { return num_of_attachments; };

  /** copy pixels if antialiased
  * @param the multi render target array (copy only these buffers which we used)
  */
  void handleTextures(std::vector<int>& MRT_index);

  /** bind a specific color buffer attachment textures
  * @param i the id of the color buffer attachment
  */
  void bindTexture(const int i) const;

  /** set/get Oversize flag /use 10% more image space) */
  void setOversize(const bool state);

  // swap attachments: first to second
  void swapAttachments(int firstpos, int secondpos);

  // get the render buffer attachment points
  GLenum* getColorBuffersExt();

  // get the current framebuffer and store the results
  void storeFramebuffer();
};

#endif // FRAMEBUFFEROBJECT_H_
