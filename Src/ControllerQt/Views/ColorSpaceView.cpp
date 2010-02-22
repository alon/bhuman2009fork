/**
* @file ControllerQt/Views/ColorSpaceView.cpp
*
* Implementation of class ColorSpaceView
*
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
*/

#include <Platform/Thread.h>
#include <GL/gl.h>

#include "ColorSpaceView.h"
#include "../RobotConsole.h"
#include "../Visualization/OpenGLMethods.h"

ColorSpaceView::ColorSpaceView(RobotConsole& c, const std::string& n, ColorModel cm, int ch, const Vector3<float>& b)
: View3D(b),
  console(c),
  name(n),
  colorModel(cm),
  channel(ch),
  lastTimeStamp(0)
{
}

void ColorSpaceView::updateDisplayLists()
{
  SYNC_WITH(console);
  Image* image = 0,
       * raw = 0;
  RobotConsole::Images::const_iterator i = console.images.find(name);
  if(i != console.images.end())
    image = i->second.image;
  i = console.images.find("raw image");
  if(i != console.images.end())
    raw = i->second.image;
  if(image && (channel < 3 || raw))
  {
    if(!channel)
      OpenGLMethods::paintCubeToOpenGLList(256, 256, 256, 
        cubeId, true,
        127, //scale
        -127, -127, -127, // offsets
        int(background.x * 255) ^ 0xc0, 
        int(background.y * 255) ^ 0xc0, 
        int(background.z * 255) ^ 0xc0);
    else
      OpenGLMethods::paintCubeToOpenGLList(
        image->cameraInfo.resolutionWidth, image->cameraInfo.resolutionHeight, 128, 
        cubeId, true,
        127, //scale
        -image->cameraInfo.resolutionWidth / 2, -image->cameraInfo.resolutionHeight / 2, -65, // offsets
        int(background.x * 255) ^ 0xc0, 
        int(background.y * 255) ^ 0xc0, 
        int(background.z * 255) ^ 0xc0);

    OpenGLMethods::paintImagePixelsToOpenGLList(*image, colorModel, channel - 1, false, colorsId);
    lastTimeStamp = image->timeStamp;
  }
  else
  {
    glNewList(cubeId, GL_COMPILE_AND_EXECUTE);
    glEndList();
    glNewList(colorsId, GL_COMPILE_AND_EXECUTE);
    glEndList();
    lastTimeStamp = 0;
  }
}

bool ColorSpaceView::needsUpdate() const 
{
  SYNC_WITH(console);
  Image* image = 0;
  RobotConsole::Images::const_iterator i = console.images.find(name);
  if(i != console.images.end())
    image = i->second.image;
  return ((image && image->timeStamp != lastTimeStamp) ||
         (!image && lastTimeStamp));
}

