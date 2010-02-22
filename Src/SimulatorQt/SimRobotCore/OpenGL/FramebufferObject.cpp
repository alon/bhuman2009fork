/**
* @file FramebufferObject.h
* 
* Implementation of an easy interface to the framebuffer object extension
*
* @author <A href="mailto:pachur@informatik.uni-bremen.de">Dennis Pachur</A>
*/ 

#include "FramebufferObject.h"
#include <OpenGL/GLHelper.h>
#include <set>

FramebufferObject::FramebufferObject(const int& attachments,
                                     const std::vector<GLenum>& textureFilter,
                                     const bool& oversized_,
                                     const bool& antialiased_,
                                     const int& samples_,
                                     const bool& storage_)
: id(0), blit_id(0), num_of_attachments(attachments), depthbuffer(0), blit_depthbuffer(0),
  initialized(false), antialiased(antialiased_), oversized(oversized_), storage(storage_),
  samples(samples_), width(0), height(0)
{
  for(int i=0; i<MAX_NUM_FBO_ATTACHMENTS; i++)
  {
    textures[i] = 0;
    texFilter[i] = GL_LINEAR;
    renderbuffers[i] = 0;
    blit_renderbuffers[i] = 0;
  }
  oversize_width = 0;
  oversize_height = 0;

  // reset render buffer targets
  colorBuffersExt[0] = GL_COLOR_ATTACHMENT0_EXT;
  colorBuffersExt[1] = GL_COLOR_ATTACHMENT1_EXT;
  colorBuffersExt[2] = GL_COLOR_ATTACHMENT2_EXT;
  colorBuffersExt[3] = GL_COLOR_ATTACHMENT3_EXT;
  colorBuffersExt[4] = GL_COLOR_ATTACHMENT4_EXT;
  colorBuffersExt[5] = GL_COLOR_ATTACHMENT5_EXT;
  colorBuffersExt[6] = GL_COLOR_ATTACHMENT6_EXT;
  colorBuffersExt[7] = GL_COLOR_ATTACHMENT7_EXT;
  PUswapped = false;
  RPswapped = false;

  // set tex filter
  for(unsigned int i=0; i < textureFilter.size(); i++)
    texFilter[i] = textureFilter[i];
}  

FramebufferObject::~FramebufferObject()
{
  destroy();
}

void FramebufferObject::destroy()
{
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
  glBindTexture(GL_TEXTURE_2D, 0);

  if(initialized)
  {
    // remove standard attachments
    for(unsigned int i = 0; i < num_of_attachments; i++)
    {
      if(antialiased)
      {
        glDeleteRenderbuffersEXT(1, &renderbuffers[i]);
        renderbuffers[i] = 0;
        glDeleteRenderbuffersEXT(1, &blit_renderbuffers[i]);
        blit_renderbuffers[i] = 0;
      }
      glDeleteTextures(1, &textures[i]);
      textures[i] = 0;
    }
    // remove storage attachments (if any)
    for(unsigned int i = 0; i < MAX_NUM_FBO_ATTACHMENTS; i++)
    {
      if(textures[i] != 0)
      {
        glDeleteTextures(1, &textures[i]);
        textures[i] = 0;
      }
    }
    // remove render buffer
    glDeleteRenderbuffersEXT(1, &depthbuffer);
    glDeleteFramebuffersEXT(1, &id);
    if(antialiased)
    {
      glDeleteRenderbuffersEXT(1, &blit_depthbuffer);
      glDeleteFramebuffersEXT(1, &blit_id);
    }
    initialized = false;
  }

  // reset variables
  id = 0;
  blit_id = 0;
  depthbuffer = 0;
  blit_depthbuffer = 0;
}

bool FramebufferObject::update(const GLuint width_,
                               const GLuint height_,
                               const bool forceNew)
{
  // if up-to-date: return
  if(width_ == width &&
    height_ == height &&
    !forceNew)
    return initialized;

  // else create new
  destroy();

  // prepare size
  if(oversized)
  {
    oversize_width = (GLuint)((float)width_*1.4f);
    oversize_height = (GLuint)((float)height_*1.4f);
    width = oversize_width;
    height = oversize_height;
  }
  else
  {
    width = width_;
    height = height_;
  }

  // check multisample render buffer size
  int max_size;
  glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE_EXT, &max_size);
  if((int)width > max_size || (int)height > max_size)
  {
    width = width_;
    height = height_;
    antialiased = false;
    GLHelper::getGLH()->glPrintText(TextObject::WARNING_HINTS, TextObject::WARNING, 5000,
      "WARNING: Mulitsample Renderbuffer are limited to size %i.", max_size);
  }

  // create additional textures first (they are fbo independent)
  if(storage)
  {
    glGenTextures(1, &textures[num_of_attachments]);
    glBindTexture(GL_TEXTURE_2D, textures[num_of_attachments]);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width,
      height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);

    /*glGenTextures(1, &textures[num_of_attachments+1]);
    glBindTexture(GL_TEXTURE_2D, textures[num_of_attachments+1]);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width,
      height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);*/
  }

  // create frame buffer object
  glGenFramebuffersEXT(1, &id);
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, id);

  // create render buffer for depth component
  glGenRenderbuffersEXT(1, &depthbuffer);
  glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depthbuffer);
  if(antialiased)
    glRenderbufferStorageMultisampleEXT(GL_RENDERBUFFER_EXT,
        samples, GL_DEPTH_COMPONENT, width, height);
  else
    glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, width, height);
  glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,
    GL_RENDERBUFFER_EXT, depthbuffer);

  // create and bind number of requested attachments (either render buffers or textures)
  // but create textures anywhere to make renderings of multisampled buffer accessable
  for(unsigned int i = 0; i < num_of_attachments; i++)
  {
    // only color-components can be antialiased, no textures and no depth components
    // render to color component and copy back pixels if antialiasing is forced
    if(antialiased)
    {
      glGenRenderbuffersEXT(1, &renderbuffers[i]);
      glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, renderbuffers[i]);
      glRenderbufferStorageMultisampleEXT(GL_RENDERBUFFER_EXT,  samples, GL_RGBA, width, height);
      glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT+i,
        GL_RENDERBUFFER_EXT, renderbuffers[i]);
    }
    // set up textures
    glGenTextures(1, &textures[i]);
    glBindTexture(GL_TEXTURE_2D, textures[i]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,  width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texFilter[i]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texFilter[i]);

    // attach the textures (if they are render targets)
    if(!antialiased)
      glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT+i, GL_TEXTURE_2D, textures[i], 0);
    glBindTexture(GL_TEXTURE_2D, 0);
  }

  // check status and unbind
  GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
  if(status == GL_FRAMEBUFFER_COMPLETE_EXT)
  {
    // create blit fbo if antialising is activated
    if(antialiased)
    {
      // create frame buffer object
      glGenFramebuffersEXT(1, &blit_id);
      glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, blit_id);

      // create render buffer for depth component
      glGenRenderbuffersEXT(1, &blit_depthbuffer);
      glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, blit_depthbuffer);
      glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, width, height);
      glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,
        GL_RENDERBUFFER_EXT, blit_depthbuffer);

      for(unsigned int i = 0; i < num_of_attachments; i++)
      {
        glGenRenderbuffersEXT(1, &blit_renderbuffers[i]);
        glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, blit_renderbuffers[i]);
        glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_RGBA, width, height);
        glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT+i,
          GL_RENDERBUFFER_EXT, blit_renderbuffers[i]);
      }
      // check status and unbind
      GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
      glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
      if(status != GL_FRAMEBUFFER_COMPLETE_EXT)
      {
        displayStatus(status);
        // set initialized to true in order to run the destory function
        // which will set it back to false
        initialized = true;
        destroy();
        return initialized;
      }
    }
    width = width_;
    height = height_;
    initialized = true;
    return initialized;
  }
  displayStatus(status);
  // set initialized to true in order to run the destory function
  // which will set it back to false
  initialized = true;
  destroy();
  return initialized;
}

void FramebufferObject::setAntialising(const bool flag, const GLuint numOfSamples)
{
  if(antialiased == flag &&
    samples == numOfSamples)
    return;

  destroy();

  antialiased = (flag && numOfSamples > 0);
  samples = numOfSamples;

  update(width, height, true);
}

void FramebufferObject::displayStatus(const GLenum status) const
{
  switch(status)
  {
  case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
    GLHelper::getGLH()->glPrintText(TextObject::WARNING_HINTS, TextObject::WARNING, 7500, "WARNING: FBO Incomplete Attachment");
    break;
  case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
    GLHelper::getGLH()->glPrintText(TextObject::WARNING_HINTS, TextObject::WARNING, 7500, "WARNING: FBO Missing Attachment");
    break;
  case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
    GLHelper::getGLH()->glPrintText(TextObject::WARNING_HINTS, TextObject::WARNING, 7500, "WARNING: FBO Incomplete Dimensions");
    break;
  case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
    GLHelper::getGLH()->glPrintText(TextObject::WARNING_HINTS, TextObject::WARNING, 7500, "WARNING: FBO Incomplete Formats");
    break;
  case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
    GLHelper::getGLH()->glPrintText(TextObject::WARNING_HINTS, TextObject::WARNING, 7500, "WARNING: FBO Incomplete Draw Buffer");
    break;
  case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
    GLHelper::getGLH()->glPrintText(TextObject::WARNING_HINTS, TextObject::WARNING, 7500, "WARNING: FBO Incomplete Read Buffer");
    break;
  case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
    GLHelper::getGLH()->glPrintText(TextObject::WARNING_HINTS, TextObject::WARNING, 7500, "WARNING: FBO Frame Buffer is unsupported");
    break;
  case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE_EXT:
    GLHelper::getGLH()->glPrintText(TextObject::WARNING_HINTS, TextObject::WARNING, 7500, "WARNING: FBO Incomplete Multisample");
    break;
  default:
    std::string glerrStr;
    for(int i=sizeof(int) -1; i>=0; i--)
      glerrStr += "0123456789ABCDEF"[((status >> i*4) & 0xF)];
    // display the default (unknown error) message for a short time (maybe the entry has not been freed before)
    GLHelper::getGLH()->glPrintMarkedText(TextObject::WARNING_HINTS, glerrStr.c_str(), TextObject::WARNING, 500,
      "GL Frame Buffer Error: %u (0x%s).",
    status, glerrStr.c_str());
    break;
  }
}

void FramebufferObject::handleTextures(std::vector<int>& MRT_index)
{
  if(antialiased)
  {
    // get width and height to copy
    GLuint temp_width;
    GLuint temp_height;
    if(oversized)
    {
      temp_width = oversize_width;
      temp_height = oversize_height;
    }
    else
    {
      temp_width = width;
      temp_height = height;
    }

    glActiveTexture(GL_TEXTURE0);
    glEnable(GL_TEXTURE_2D);

    std::set<int> MRT_attachments;
    for(unsigned int i=0; i < MRT_index.size(); i++)
      MRT_attachments.insert(colorBuffersExt[MRT_index[i]] - GL_COLOR_ATTACHMENT0_EXT);
    std::set<int>::iterator MRTAiter;
    for(MRTAiter = MRT_attachments.begin(); MRTAiter != MRT_attachments.end(); MRTAiter++)
    {
      // use blit extension to copy data from multisampled fbo to single sampled fbo
      glBindFramebufferEXT(GL_READ_FRAMEBUFFER_EXT, id);
      glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER_EXT, blit_id);
      glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT + *MRTAiter);
      glReadBuffer(GL_COLOR_ATTACHMENT0_EXT + *MRTAiter);
      glBlitFramebufferEXT(0, 0, temp_width, temp_height,
                           0, 0, temp_width, temp_height,
                           GL_COLOR_BUFFER_BIT, GL_NEAREST);
      glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, blit_id);

      // copy data from single sampled framebuffer to texture
      glBindTexture(GL_TEXTURE_2D, textures[*MRTAiter]);
      glReadBuffer(GL_COLOR_ATTACHMENT0_EXT + *MRTAiter);
      glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
        0, 0, temp_width, temp_height, 0);
    }
    glDisable(GL_TEXTURE_2D);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, id);
  }
}

void FramebufferObject::bindTexture(const int i) const 
{
  glBindTexture(GL_TEXTURE_2D, textures[i]);
}

void FramebufferObject::setOversize(const bool state)
{
  oversized = state;
}

void FramebufferObject::swapAttachments(int firstpos, int secondpos)
{
  GLenum temp = colorBuffersExt[firstpos];
  colorBuffersExt[firstpos] = colorBuffersExt[secondpos];
  colorBuffersExt[secondpos] = temp;
}

GLenum* FramebufferObject::getColorBuffersExt()
{
  return colorBuffersExt;
}

void FramebufferObject::storeFramebuffer()
{
  if(storage)
  {
    //render color buffer to texture
    glBindTexture(GL_TEXTURE_2D, textures[num_of_attachments]);
    glCopyTexSubImage2D(GL_TEXTURE_2D,0,0,0,0,0,width, height);
    glBindTexture(GL_TEXTURE_2D, 0);

    //render depth to texture
    /*glBindTexture(GL_TEXTURE_2D, textures[num_of_attachments+1]);
    glCopyTexImage2D(GL_TEXTURE_2D,0,GL_DEPTH_COMPONENT, 0,0, width, height, 0);
    glBindTexture(GL_TEXTURE_2D, 0);*/
  }
}
