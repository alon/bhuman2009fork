/**
 * @file OffscreenRenderer.cpp
 * 
 * An implementation of a platform dependent hardware accelerated
 * offscreen rendering module.

 * @author <A href="mailto:roefer@informatik.uni-bremen.de">Thomas Rfer</A>
 * @author <A href="mailto:timlaue@informatik.uni-bremen.de">Tim Laue</A>
 * @author Colin Graf
 */

#include <GL/glew.h>
#include <QtOpenGL/QGLPixelBuffer>
#include <QtOpenGL/QGLWidget>

#include "OffscreenRenderer.h"
#include "../Tools/SimGraphics.h"
#include "Assert.h"

OffscreenRenderer::OffscreenRenderer(GraphicsManager* graphicsManager) : 
  graphicsManager(graphicsManager),
  pbuffer(0), glWidget(0),
  width(0), height(0),
  flippingBuffer(0), useOffset(false),
  framebuffer(0), renderbuffer(0)
{
}

OffscreenRenderer::~OffscreenRenderer()
{
  if(pbuffer)
    delete pbuffer;
  if(glWidget)
    delete glWidget;
  if(framebuffer)
    glDeleteFramebuffersEXT(1, &framebuffer);
  if(renderbuffer)
    glDeleteRenderbuffersEXT(1, &renderbuffer);
  if(flippingBuffer)
    delete[] flippingBuffer;
}

bool OffscreenRenderer::makeCurrent()
{
  ASSERT(pbuffer || glWidget);
  if(pbuffer)
    return pbuffer->makeCurrent();
  glWidget->makeCurrent();
  if(framebuffer)
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
  return true;
}

bool OffscreenRenderer::doneCurrent()
{
  ASSERT(pbuffer || glWidget);
  if(pbuffer)
    return pbuffer->doneCurrent();
  glWidget->doneCurrent();
  return true;
}

void OffscreenRenderer::prepareRendering(int w, int h)
{
  if((!pbuffer && !glWidget) || width < w || height < h)
  {
    bool usePBuffer = true;
    if(pbuffer)
    {
      delete pbuffer;
      pbuffer = 0;
    }
    if(glWidget)
    {
      delete glWidget;
      glWidget = 0;
      usePBuffer = false;
    }
    if(framebuffer)
    {
      glDeleteFramebuffersEXT(1, &framebuffer);
      framebuffer = 0;
    }
    if(renderbuffer)
    {
      glDeleteRenderbuffersEXT(1, &renderbuffer);
      renderbuffer = 0;
    }
    if(flippingBuffer)
    {
      delete[] flippingBuffer;
      flippingBuffer = 0;
    }
    
    QGLFormat format;
    format.setStencil(false);
    //format.setDoubleBuffer(false);
    format.setSwapInterval(0);
    if(usePBuffer)
    {
#ifdef _WIN32
      pbuffer = new QGLPixelBuffer(QSize(w, h), format);
#else
      pbuffer = 0;
#endif
    }
    if(!pbuffer || !pbuffer->isValid())
    {
      if(pbuffer)
      {
        delete pbuffer;
        pbuffer = 0;
      }
      
      // maybe fall back to "onscreen rendering"
      if(!glWidget)
      {
        glWidget = new QGLWidget(format, 0, 0, Qt::WindowStaysOnTopHint);
        glWidget->setFixedSize(w, h);
        glWidget->makeCurrent();

        width = w;
        height = h;
          
        graphicsManager->initContext(false);
      }

      // QGLFramebufferObject didn't work properly... but well, the framebuffer object extension is platform independend anyway
      if(GLEW_EXT_framebuffer_object)
      {
        GLenum status;
        glGenFramebuffersEXT(1, &framebuffer);
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, framebuffer);
        glGenRenderbuffersEXT(1, &renderbuffer);
        glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, renderbuffer);
        glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_RGBA8, w, h);
        glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_RENDERBUFFER_EXT, renderbuffer);
        status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
        if(status != GL_FRAMEBUFFER_COMPLETE_EXT)
        {
          if(framebuffer)
          {
            glDeleteFramebuffersEXT(1, &framebuffer);
            framebuffer = 0;
          }
          if(renderbuffer)
          {
            glDeleteRenderbuffersEXT(1, &renderbuffer);
            renderbuffer = 0;
          }
        }
      }
      
      if(framebuffer)
      {
        TRACE("OffscreenRenderer: using framebuffer object renderer");
      }
      else
      {
        TRACE("OffscreenRenderer: using window renderer");
      }

      determineImagePositionInBuffer(w, h);
    }
    else
    {
      TRACE("OffscreenRenderer: yeah, using pixel buffer renderer");
      pbuffer->makeCurrent();
      const QSize& size(pbuffer->size());
      width = size.width();
      height = size.height();
      ASSERT(width >= w && height >= h);  
      graphicsManager->initContext(false);
      determineImagePositionInBuffer(w, h);
    }
  }
  VERIFY(makeCurrent());
}

void OffscreenRenderer::determineImagePositionInBuffer(int w, int h)
{

  ASSERT(flippingBuffer == 0);
  if(height > 1)
  {
    ASSERT(width >= w && height >= h);
    ASSERT(sizeof(float) >= 4); // ;)
    char* image = new char[width * height * sizeof(float)];
#if DETERMINE_IMAGE_POSITION_IN_BUFFER
    // clear whole surface
    VERIFY(makeCurrent());
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(20, double(width) / height, 0.01, 2);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glClearColor(0.0 , 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     
  
    glShadeModel(GL_FLAT);
    gluLookAt(0, 0, 0, 
              0, 0, 1,
              1, 0, 0);
    glColor3f(0, 0, 0);
    glBegin(GL_POLYGON);
      glVertex3d (200, -200, 1);
      glVertex3d (200, 200, 1);
      glVertex3d (-200, 200, 1);
      glVertex3d (-200, -200, 1);
    glEnd();
    glFlush();
    VERIFY(doneCurrent());

    // repaint image part
    VERIFY(makeCurrent());
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(20, double(w) / h, 0.01, 2);
  
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     
  
    glShadeModel(GL_FLAT);
    gluLookAt(0, 0, 0, 
              0, 0, 1,
              1, 0, 0);

    glColor3f(1,1,1);
    glBegin(GL_POLYGON);
      glVertex3d (100, -100, 1);
      glVertex3d (0, -100, 1);
      glVertex3d (0, 0, 1);
      glVertex3d (100, 0, 1);
    glEnd();
    glBegin(GL_POLYGON);
      glVertex3d (-100, 100, 1);
      glVertex3d (0, 100, 1);
      glVertex3d (0, 0, 1);
      glVertex3d (-100, 0, 1);
    glEnd();

    glFlush();

    glPixelStorei(GL_PACK_ALIGNMENT,1);
    glReadPixels(0, 0, w, h, GL_BGR, GL_UNSIGNED_BYTE, image);
    VERIFY(doneCurrent());

    if(image[0] && image[w * h * 3 - 1])
    {
      //flippingBuffer = 0;
    }
    else if(image[w * 3 - 1] && image[w * (h - 1) * 3])
    {
      flippingBuffer = image;
      image = 0;
      TRACE("OffscreenRenderer: using flipping buffer");
    }
    else 
    {
      useOffset = true;
      if(image[(height - h) * w * 3])
      {
        //flippingBuffer = 0;
      }
      else
      {
        flippingBuffer = image;
        image = 0;
        TRACE("OffscreenRenderer: using flipping buffer");
      }
    }

    if(image)
      delete [] image;
#else
    useOffset = false;
    flippingBuffer = image;
    image = 0;
#endif
  }
}


void OffscreenRenderer::finishRendering(void* image, int w, int h, OffscreenRenderer::Content content)
{
  if(content == IMAGE)
  {
    int heightOffset(useOffset ? height - h : 0);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    if(flippingBuffer) // upside down
    {
      glReadPixels(0, heightOffset, w, h, GL_BGR, GL_UNSIGNED_BYTE, flippingBuffer);
      char* pSrc = (char*) flippingBuffer + w * h * 3,
          * pDest = (char*) image;
      const int width3(w * 3);
      for(int y = 0; y < h; ++y)
      {
        pSrc -= width3;
        memcpy(pDest, pSrc, width3);
        pDest += width3;
      }
    }
    else // not upside down
    {
      glReadPixels(0, heightOffset, w, h, GL_BGR, GL_UNSIGNED_BYTE, image);
    }
  }
  else if(content == IMAGE_RGBA)
  {
    int heightOffset(useOffset ? height - h : 0);
    glPixelStorei(GL_PACK_ALIGNMENT, 4);
    if(flippingBuffer) // upside down
    {
      glReadPixels(0, heightOffset, w, h, GL_RGBA, GL_UNSIGNED_BYTE, flippingBuffer);
      char* pSrc = (char*) flippingBuffer + w * h * 4,
          * pDest = (char*) image;
      const int width4(w * 4);
      for(int y = 0; y < h; ++y)
      {
        pSrc -= width4;
        memcpy(pDest, pSrc, width4);
        pDest += width4;
      }
    }
    else // not upside down
    {
      glReadPixels(0, heightOffset, w, h, GL_RGBA, GL_UNSIGNED_BYTE, image);
    }
  }
  else //content == DEPTH
  {
    glPixelStorei(GL_PACK_ALIGNMENT,1);
    int heightOffset(useOffset ? height - h : 0);
    if(flippingBuffer) // upside down
    {
      glReadPixels(0, heightOffset, w, h, GL_DEPTH_COMPONENT, GL_FLOAT, flippingBuffer);
      char* pSrc = (char*) flippingBuffer + w * h * sizeof(float),
          * pDest = (char*) image;
      const int totalWidth(w*sizeof(float));
      for(int y = 0; y < h; ++y)
      {
        pSrc -= totalWidth;
        memcpy(pDest, pSrc, totalWidth);
        pDest += totalWidth;
      }
    }
    else // not upside down
    {
      glReadPixels(0, heightOffset, w, h, GL_DEPTH_COMPONENT, GL_FLOAT, image);
    }
  }

  if(glWidget && glWidget->isVisible())
    glWidget->swapBuffers();
}

OffscreenRenderer::Method OffscreenRenderer::getRenderingMethod() const
{
  if(pbuffer)
    return PIXELBUFFER;
  if(framebuffer)
    return FRAMEBUFFEROBJECT;
  if(glWidget)
    return WINDOW;
  return UNKNOWN;
}

bool OffscreenRenderer::showWindow(bool show)
{
  if(framebuffer || !glWidget)
    return false;
  glWidget->setVisible(show);
  return true;
}

bool OffscreenRenderer::isWindowVisible() const
{
  if(framebuffer || !glWidget)
    return false;
  return glWidget->isVisible();
}
