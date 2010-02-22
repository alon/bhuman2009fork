/** 
* @file OpenGLMethods.cpp
* Implementation of class OpenGLMethods.
*
* @author <A href="mailto:juengel@informatik.hu-berlin.de">Matthias Jüngel</A>
*/

#include "OpenGLMethods.h"
#include <GL/glew.h>

void OpenGLMethods::paintCubeToOpenGLList
(
 int xLow, int yLow, int zLow, 
 int xHigh, int yHigh, int zHigh,
 int listID, bool paintFullCube,
 int scale,
 int offsetX,int offsetY,int offsetZ,
 int r, int g, int b,
 bool beginList, bool endList
 )
{
  GLUquadricObj* pQuadric = gluNewQuadric();
  
  if(beginList) glNewList(listID, GL_COMPILE_AND_EXECUTE);
  
  double xL, yL, zL;
  double xH, yH, zH;
  
  xH = OpenGLMethods::transformCoordinates(scale, offsetX, xHigh);
  yH = OpenGLMethods::transformCoordinates(scale, offsetY, yHigh);
  zH = OpenGLMethods::transformCoordinates(scale, offsetZ, zHigh);
  xL = OpenGLMethods::transformCoordinates(scale, offsetX, xLow);
  yL = OpenGLMethods::transformCoordinates(scale, offsetY, yLow);
  zL = OpenGLMethods::transformCoordinates(scale, offsetZ, zLow);
  
  glBegin(GL_LINE_LOOP);
  glColor3ub(r,g,b);
  glVertex3d(xL,yL,zL);
  glVertex3d(xH,yL,zL);
  glVertex3d(xH,yH,zL);
  glVertex3d(xL,yH,zL);
  glEnd();
  
  if(paintFullCube)
  {
    glBegin(GL_LINE_LOOP);
    glVertex3d(xL,yL,zH);
    glVertex3d(xH,yL,zH);
    glVertex3d(xH,yH,zH);
    glVertex3d(xL,yH,zH);
    glEnd();
    
    glBegin(GL_LINES);
    
    glVertex3d(xL,yL,zL);
    glVertex3d(xL,yL,zH);
    glVertex3d(xH,yL,zL);
    glVertex3d(xH,yL,zH);
    glVertex3d(xH,yH,zL);
    glVertex3d(xH,yH,zH);
    glVertex3d(xL,yH,zL);
    glVertex3d(xL,yH,zH);
    
    glEnd();
  }
  if(endList) glEndList();
  
  gluDeleteQuadric(pQuadric);
}

inline void OpenGLMethods::getColorClassColor(ColorClasses::Color colorClass, 
                                              unsigned char& r, unsigned char& g, unsigned char& b)
{
  switch(colorClass)
  {
  case ColorClasses::white:
    r = 255;
    g = 255;
    b = 255;
    break;
  case ColorClasses::green:
    r = 0;
    g = 255;
    b = 0;
    break;
  case ColorClasses::orange:
    r = 255;
    g = 127;
    b = 64;
    break;
  case ColorClasses::yellow:
    r = 255;
    g = 255;
    b = 0;
    break;
  case ColorClasses::blue:
    r = 0;
    g = 0;
    b = 255;
    break;
  case ColorClasses::black:
    r = 0;
    g = 0;
    b = 0;
    break;
  case ColorClasses::red:
    r = 255;
    g = 0;
    b = 0;
    break;
  case ColorClasses::robotBlue:
    r = 0;
    g = 255;
    b = 255;
    break;
  default:
    r = 64;
    g = 0;
    b = 0;
  }
}

void OpenGLMethods::paintColorTable
(
 const ColorTable64& colorTable,
 int listID
 )
{
  // Build a new list
  ::glNewList(listID,GL_COMPILE_AND_EXECUTE);
  
  unsigned char r[256],
                g[256],
                b[256];

  for(int i = 0; i < ColorClasses::numOfColors; ++i)
    getColorClassColor((ColorClasses::Color) i, r[i], g[i], b[i]);
  for(int i = 129; i < 256; ++i)
  {
    int rSum = 0,
        gSum = 0,
        bSum = 0,
        count = 0;
    for(int j = 0; j < 7; ++j)
      if(i & 1 << j)
      {
        getColorClassColor((ColorClasses::Color) (j + 1), r[i], g[i], b[i]);
        rSum += r[i];
        gSum += g[i];
        bSum += b[i];
        ++count;
      }
    r[i] = (unsigned char) (rSum / count);
    g[i] = (unsigned char) (gSum / count);
    b[i] = (unsigned char) (bSum / count);
  }

  glBegin(GL_POINTS);
  
  double tmp = 2.0f/255.0f;
  double x,y;
  double z00;
  
  for(int colorSpaceY = 0; colorSpaceY < 255; colorSpaceY += 4)
    for(int colorSpaceU = 0; colorSpaceU < 255; colorSpaceU += 4)
      for(int colorSpaceV = 0; colorSpaceV < 255; colorSpaceV += 4)
      {
        ColorClasses::Color color = colorTable.getColorClass(colorSpaceY, colorSpaceU, colorSpaceV);
        if(color != ColorClasses::none)
        {
          x   = (double)(-1.0 + colorSpaceV*tmp);
          y   = (double)(-1.0 + colorSpaceU*tmp);
          z00 = (double)(-1.0 + colorSpaceY*tmp);
          glColor3ub(r[color], g[color], b[color]);
          glVertex3d(x, y, z00);
        }
      }
  glEnd();
  glPolygonMode(GL_FRONT,GL_LINE);
  glPolygonMode(GL_BACK,GL_LINE);
  ::glEndList();
}

void OpenGLMethods::paintImagePixelsToOpenGLList
(
 const Image& image,
 int colorModel,
 int zComponent,
 bool polygons,
 int listID,
 int x1,
 int x2,
 int y1,
 int y2
 )
{
  // Build a new list
  ::glNewList(listID,GL_COMPILE_AND_EXECUTE);
  
  if(polygons)
  {
    glPolygonMode(GL_FRONT,GL_FILL);
    glPolygonMode(GL_BACK,GL_FILL);
    glBegin(GL_QUADS);
  }
  else
  {
    glBegin(GL_POINTS);
  }
  
  double tmp = 2.0f/255.0f;
  double x,y;
  double xn,yn;
  double z00, z01, z10, z11;
  
  Image rgbImage;
  rgbImage.convertFromYCbCrToRGB(image);
  
  Image convertedImage;
  switch(colorModel)
  {
  case 0: //yuv
    convertedImage = image;
    break;
  case 1: //rgb
    convertedImage.convertFromYCbCrToRGB(image);
    break;
  case 2: //tsl
    convertedImage.convertFromYCbCrToTSL(image);
    break;
  case 3: //hsi
    convertedImage.convertFromYCbCrToHSI(image);
    break;
  }
  
  for(int j = y1; j < y2 - 2; j++)
  {
    for(int i = x1; i < x2 - 2; i++)
    {
      if(zComponent == -1)
      {
        x   = (double)(-1.0 + convertedImage.image[j][i].cr*tmp);
        y   = (double)(-1.0 + convertedImage.image[j][i].cb*tmp);
        z00 = (double)(-1.0 + convertedImage.image[j][i].y*tmp);
        glColor3ub(rgbImage.image[j][i].r, rgbImage.image[j][i].g, rgbImage.image[j][i].b);
        glVertex3d(x, y, z00);
      }
      else
      {
        x = (double)( +i - image.cameraInfo.resolutionWidth / 2) * tmp;
        y = (double)( -j + image.cameraInfo.resolutionHeight / 2) * tmp;
        xn = (double)( +i + 1 - image.cameraInfo.resolutionWidth / 2) * tmp;
        yn = (double)( -j - 1 + image.cameraInfo.resolutionHeight / 2) * tmp;
        z00 = (double)(-0.5 + convertedImage.image[j][i].channels[zComponent] * tmp / 2);
        if(polygons)
        {
          z01 = (double)(-0.5 + convertedImage.image[j+1][i].channels[zComponent] * tmp / 2);
          z10 = (double)(-0.5 + convertedImage.image[j][i+1].channels[zComponent] * tmp / 2);
          z11 = (double)(-0.5 + convertedImage.image[j+1][i+1].channels[zComponent] * tmp / 2);
          glColor3ub(rgbImage.image[j][i].r, rgbImage.image[j][i].g, rgbImage.image[j][i].b);
          glVertex3d(x, y, z00);
          
          glColor3ub(rgbImage.image[j][i+1].r, rgbImage.image[j][i+1].g, rgbImage.image[j][i+1].b);
          glVertex3d(xn, y, z10);
          
          glColor3ub(rgbImage.image[j+1][i+1].r, rgbImage.image[j+1][i+1].g, rgbImage.image[j+1][i+1].b);
          glVertex3d(xn, yn, z11);
          
          glColor3ub(rgbImage.image[j+1][i].r, rgbImage.image[j+1][i].g, rgbImage.image[j+1][i].b);
          glVertex3d(x, yn, z01);
        }
        else
        {
          glColor3ub(rgbImage.image[j][i].r, rgbImage.image[j][i].g, rgbImage.image[j][i].b);
          glVertex3d(x, y, z00);
        }
      }
    }
  }
  glEnd();
  glPolygonMode(GL_FRONT,GL_LINE);
  glPolygonMode(GL_BACK,GL_LINE);
  ::glEndList();
}
