/**
* @file ControllerQt/Views/ColorTableView.cpp
*
* Implementation of class ColorTableView
*
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
*/

#include <Platform/Thread.h>

#include "ColorTableView.h"
#include "../RobotConsole.h"
#include "../Visualization/OpenGLMethods.h"

ColorTableView::ColorTableView(RobotConsole& c, const ColorTableHandler& cth, const Vector3<float>& b)
: View3D(b),
  console(c),
  colorTableHandler(cth),
  lastTimeStamp(0)
{
}

void ColorTableView::updateDisplayLists()
{
  SYNC_WITH(console);
  OpenGLMethods::paintCubeToOpenGLList(256, 256, 256, 
    cubeId, true,
    127, //scale
    -127, -127, -127, // offsets
    int(background.x * 255) ^ 0xc0, 
    int(background.y * 255) ^ 0xc0, 
    int(background.z * 255) ^ 0xc0);
  OpenGLMethods::paintColorTable(colorTableHandler.colorTable, colorsId);
  lastTimeStamp = colorTableHandler.timeStamp;
}

bool ColorTableView::needsUpdate() const
{
  return colorTableHandler.timeStamp != lastTimeStamp;
}

