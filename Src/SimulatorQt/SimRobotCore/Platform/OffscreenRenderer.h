/**
* @file OffscreenRenderer.h
* Declaration of class OffscreenRenderer.
* @author Colin Graf
*/

#ifndef OffscreenRenderer_H
#define OffscreenRenderer_H

class GraphicsManager;
class QGLPixelBuffer;
class QGLWidget;
class QGLFramebufferObject;

/**
* @class OffscreenRenderer
* A hardware accelerated offscreen rendering module that uses the Qt 4 OpenGL library.
*/
class OffscreenRenderer
{
public:

  /**
  * Constructor.
  * @param graphicsManager The graphics manager of the simulation.
  */
  OffscreenRenderer(GraphicsManager* graphicsManager);

  /*
  * Destructor.
  */
  ~OffscreenRenderer();

  enum Content
  {
    IMAGE, /**< BGR (3 byte) */
    IMAGE_RGBA, /**< (4 byte) */
    DEPTH,/**< values from depth buffer represented as float values*/
  };

  enum Method
  {
    UNKNOWN, /**< Call prepareRendering() first. */
    PIXELBUFFER, /**< Good. */
    FRAMEBUFFEROBJECT, /**< Good. */
    WINDOW, /**< Well..., it works at least. */
  };

  /**
  * Prepares the offscreen renderer to render something of the given size. This call changes the 
  * rendering context to the rendering context of the offscreen renderer. Call prepareRendering()
  * before "drawing" the opengl image.
  * @param width The maximum width.
  * @param height The maximum height.
  */
  void prepareRendering(int width, int height);

  /**
  * Reads an image from current rendering context.
  * @param image The buffer where is image will be saved to.
  * @param width The image width.
  * @param height The image heigt.
  * @param content The image content type.
  */
  void finishRendering(void* image, int width, int height, Content content);

  /**
  * Requests the used rendering method. Only availabe when prepareRendering() was called at least once.
  * @return The used rendering method.
  */
  Method getRenderingMethod() const;

  /**
  * Shows or hides the offscreen renderer window. This works only when \c getRenderingMethod() returns \c WINDOW.
  * @param show Whether to show the window. 
  */
  bool showWindow(bool show);

  /**
  * Requests the visibility of the offscreen renderer window.
  * @return Whether the window is visible or not.
  */
  bool isWindowVisible() const;

private:
  GraphicsManager* graphicsManager;
  QGLPixelBuffer* pbuffer;
  QGLWidget* glWidget;
  int width;
  int height;
  char* flippingBuffer;
  bool useOffset;
  unsigned int framebuffer;
  unsigned int renderbuffer;

  bool makeCurrent();
  bool doneCurrent();
  void determineImagePositionInBuffer(int width, int height);
};

#endif
