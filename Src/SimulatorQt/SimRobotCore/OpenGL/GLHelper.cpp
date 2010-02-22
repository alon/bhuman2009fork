/**
* @file OpenGL/GLHelper.cpp
*
* Functions to drawing debug drawings for SimRobot
*
* @author <A href="mailto:pachur@informatik.uni-bremen.de">Dennis Pachur</A>
*/

#include <cstring>
#include <sstream>
#include <cstdarg>
#include <cstdio>

#ifndef _WIN32
#include <GL/glew.h>
#include <GL/glxew.h>
#endif

#ifdef _WIN32
#include <windows.h>
#endif

#include "GLHelper.h"
#include "../Tools/SimMath.h"
#include "../Platform/OffscreenRenderer.h"
#include "../Platform/System.h"
#include "../OpenGL/Constants.h"
#include "../OpenGL/ShaderInterface.h"

TextObject::TextObject(const std::string& text, const std::string& textNoValues, unsigned long duration, TextColor color) :
  text(text),
  textNoValues(textNoValues),
  duration(duration),
  creationtime(System::getTime()),
  color(color) {}

  TextObject::TextObject(const std::string& text, const std::string& textNoValues, unsigned long duration, TextColor color, const std::string& marker) :
  text(text),
  textNoValues(textNoValues),
  marker(marker),
  duration(duration),
  creationtime(System::getTime()),
  color(color) {}

bool TextObject::checkInDuration() const
{
  return System::getTimeSince(creationtime) < duration;
}

unsigned int TextObject::getRemainingTime() const
{
  unsigned int timePassed = System::getTimeSince(creationtime);
  return timePassed >= duration ? 0 : (duration - timePassed);
}

void TextObject::updateDuration(const std::string& text, TextColor color, unsigned long duration)
{
  TextObject::text = text;
  TextObject::color = color;
  TextObject::duration = duration;
  creationtime = System::getTime();
};

float TextObject::getColor(unsigned int channel) const
{
  if(channel > 2)
  {
    GLfloat remainingtime = ((GLfloat)getRemainingTime()/500.0f);
    return remainingtime > 1.0f ? 1.0f : remainingtime;
  }
  switch(color)
  {
  case WHITE:
    return white[channel];
  case LIGHTGREY:
    return lightgrey[channel];
  case GREEN:
    return green[channel];
  case LIGHTGREEN:
    return lightgreen[channel];
  case BLUE:
    return blue[channel];
  case LIGHTBLUE:
    return lightblue[channel];
  case RED:
    return red[channel];
  case LIGHTRED:
    return lightred[channel];
  case WARNING:
    if((getRemainingTime()/400)%2 == 0)
      return red[channel];
    else
      return lightred[channel];
  default:
    return 0.0f;
  }
}

GLHelper* GLHelper::pinstance = NULL;

GLHelper* GLHelper::getGLH()
{
  // If no instance exists yet, create one
  if (pinstance == NULL)
    pinstance = new GLHelper();

  return pinstance;
}

GLHelper::GLHelper()
{
  fonthasbeenbuild = false;
  additionalPrints = 0;
  sizeOfTextBackground = 0.0;
  maxMessageType = TextObject::WARNING_HINTS;
  for(int i=0; i<8; i++)
    fboRegister[i] = false;
}

GLHelper::~GLHelper()
{
  // delete text objects
  std::vector<TextObject*>::iterator iter;
  for(iter = textobjects.begin(); iter != textobjects.end(); ++iter)
    delete (*iter);
  textobjects.clear();

  // delete font
  DeleteFont();
}

void GLHelper::destroy() 
{
  delete pinstance;
  pinstance = 0;
}

GLvoid GLHelper::CreateBaseFont()
{
  basefontDLs = glGenLists(96);

#ifndef _WIN32
  Display* display = XOpenDisplay(0);
#endif

#ifdef _WIN32
  HFONT font = CreateFontA(
    -16,                      // Height Of Font
    0,                        // Width Of Font
    0,                        // Angle Of Escapement
    0,                        // Orientation Angle
    FW_NORMAL,                // Font Weight
    FALSE,                    // Italic
    FALSE,                    // Underline
    FALSE,                    // Strikeout
    DEFAULT_CHARSET,          // Character Set Identifier
    OUT_TT_PRECIS,            // Output Precision
    CLIP_DEFAULT_PRECIS,      // Clipping Precision
    DEFAULT_QUALITY,          // Output Quality
    FF_DONTCARE|DEFAULT_PITCH,// Family And Pitch
    "Tahoma");           // Font Name

  HDC hDC = wglGetCurrentDC();
  HFONT oldfont = (HFONT)SelectObject(hDC, font);
  wglUseFontBitmaps(hDC, 32, 96, basefontDLs);
  SelectObject(hDC, oldfont);
  DeleteObject(font);
#else
  XFontStruct* font_info = XLoadQueryFont(display, "fixed"); // TODO: use another font
  glXUseXFont(font_info->fid, 32, 96, basefontDLs);
  XFreeFont(display, font_info);
#endif

  shaderfontDLs = glGenLists(96);
  
#ifdef _WIN32
  font = CreateFontA(
    -20,                      // Height Of Font
    0,                        // Width Of Font
    0,                        // Angle Of Escapement
    0,                        // Orientation Angle
    FW_BOLD,                  // Font Weight
    FALSE,                    // Italic
    FALSE,                    // Underline
    FALSE,                    // Strikeout
    DEFAULT_CHARSET,          // Character Set Identifier
    OUT_TT_PRECIS,            // Output Precision
    CLIP_DEFAULT_PRECIS,      // Clipping Precision
    DEFAULT_QUALITY,          // Output Quality
    FF_DONTCARE|DEFAULT_PITCH,// Family And Pitch
    "Impact");           // Font Name

  hDC = wglGetCurrentDC();
  oldfont = (HFONT)SelectObject(hDC, font);
  wglUseFontBitmaps(hDC, 32, 96, shaderfontDLs);
  SelectObject(hDC, oldfont);
  DeleteObject(font);
#else
  font_info = XLoadQueryFont(display, "fixed"); // TODO: use another font
  glXUseXFont(font_info->fid, 32, 96, shaderfontDLs);
  XFreeFont(display, font_info);
#endif
#ifndef _WIN32
  XCloseDisplay(display);
#endif

  fonthasbeenbuild = true;
}

GLvoid GLHelper::DeleteFont()
{
  if(fonthasbeenbuild)
  {
    fonthasbeenbuild = false;
    glDeleteLists(basefontDLs, 96);
    glDeleteLists(shaderfontDLs, 96);
  }
}

GLvoid GLHelper::glPrintf(bool bold, GLuint& posx, GLuint& posy, const char* fmt, ...)
{
  if(!fonthasbeenbuild || fmt == NULL)
    return;

  if(GLEW_VERSION_1_4)
    glWindowPos2i(posx, posy);
  else
  {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, 600.0, 0.0, 600.0, 0.0, 0.2);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glTranslatef(GLfloat(0.0), GLfloat(0.0), GLfloat(-0.1));
    glRasterPos2i(posx, posy);
  }

  char text[256];
  va_list ap;

  va_start(ap, fmt);
  vsprintf(text, fmt, ap);
  va_end(ap);

  glPushAttrib(GL_LIST_BIT);
  if(bold)
    glListBase(shaderfontDLs - 32);
  else
    glListBase(basefontDLs - 32);
  glCallLists(strlen(text), GL_UNSIGNED_BYTE, text);
  glPopAttrib();
}

GLvoid GLHelper::glPrintText(const TextObject::TextType type, const TextObject::TextColor color, const unsigned long displaytime, const char* fmt, ...)
{
  if(fmt == NULL || type > maxMessageType)
    return;

  char text[256];
  va_list ap;

  va_start(ap, fmt);
  vsprintf(text, fmt, ap);
  va_end(ap);

  std::stringstream textNoValues, textStr;
  textNoValues << fmt;
  textStr << text;

  std::vector<TextObject*>::iterator iter;
  for(iter = textobjects.begin(); iter != textobjects.end(); ++iter)
  {
    if(strcmp(textNoValues.str().c_str(), (*iter)->getTextNoValues()) == 0)
    {
      (*iter)->updateDuration(textStr.str(), color, displaytime);
      return;
    }
  }
  TextObject* tobject = new TextObject(textStr.str(), textNoValues.str(), displaytime, color);
  textobjects.push_back(tobject);
}

GLvoid GLHelper::glPrintMarkedText(const TextObject::TextType type, const char* marker, const TextObject::TextColor color, const unsigned long displaytime, const char* fmt, ...)
{
  if(fmt == NULL || type > maxMessageType)
    return;

  char text[256];
  va_list ap;

  va_start(ap, fmt);
  vsprintf(text, fmt, ap);
  va_end(ap);

  std::stringstream textNoValues, textStr;
  textNoValues << fmt;
  textStr << text;

  std::vector<TextObject*>::iterator iter;
  for(iter = textobjects.begin(); iter != textobjects.end(); ++iter)
  {
    if(strcmp(textNoValues.str().c_str(), (*iter)->getTextNoValues()) == 0)
    {
      if(strcmp((*iter)->getMarker(), marker) == 0)
      {
        (*iter)->updateDuration(textStr.str(), color, displaytime);
        return;
      }
    }
  }
  TextObject* tobject = new TextObject(textStr.str(), textNoValues.str(),
    displaytime, color, std::string(marker));
  textobjects.push_back(tobject);
}


GLvoid GLHelper::renderText(GLuint width, GLuint height)
{
  if(!fonthasbeenbuild)
    return;

  additionalPrints = 0;

  GLenum glerr = glGetError();
  if(glerr != GL_NO_ERROR)
  {
    switch(glerr)
    {
    case GL_INVALID_ENUM:
      glPrintText(TextObject::WARNING_HINTS, TextObject::WARNING, 5000, "GLError: GL_INVALID_ENUM");
      break;
    case GL_INVALID_VALUE:
      glPrintText(TextObject::WARNING_HINTS, TextObject::WARNING, 5000, "GLError: GL_INVALID_VALUE");
      break;
    case GL_INVALID_OPERATION:
      glPrintText(TextObject::WARNING_HINTS, TextObject::WARNING, 5000, "GLError: GL_INVALID_OPERATION");
      break;
    case GL_STACK_OVERFLOW:
      glPrintText(TextObject::WARNING_HINTS, TextObject::WARNING, 5000, "GLError: GL_STACK_OVERFLOW");
      break;
    case GL_STACK_UNDERFLOW:
      glPrintText(TextObject::WARNING_HINTS, TextObject::WARNING, 5000, "GLError: GL_STACK_UNDERFLOW");
      break;
    case GL_OUT_OF_MEMORY:
      glPrintText(TextObject::WARNING_HINTS, TextObject::WARNING, 5000, "GLError: GL_OUT_OF_MEMORY");
      break;
    default:
      std::string glerrStr;
      for(int i=sizeof(int) -1; i>=0; i--)
        glerrStr += "0123456789ABCDEF"[((glerr >> i*4) & 0xF)];
      glPrintText(TextObject::WARNING_HINTS, TextObject::WARNING, 5000, "GLError: %u (0x%s). See gl.h for more information.",
        glerr, glerrStr.c_str());
      break;
    }
  }

  if(textobjects.size() == 0 && sizeOfTextBackground == 0.0f)
    return;

  std::vector<TextObject*>::iterator iter;
  for(iter = textobjects.begin(); iter != textobjects.end();)
  {
    if( !((*iter)->checkInDuration()) )
    {
      delete (*iter);
      textobjects.erase(iter);
      iter = textobjects.begin();
    }
    else
      ++iter;
  }
  // draw background
  glDisable(GL_DEPTH_TEST);
  glColor4f(0.0f, 0.0f, 0.0f, 0.6f);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0.0, width, 0.0, height, 0.0, 0.2);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(GLfloat(0.0), GLfloat(0.0), GLfloat(-0.1));
  GLfloat neededBackgroundSize = textobjects.size()*20.0f
    + (textobjects.size() > 0 ? 10.f : 0.0f);
  if(neededBackgroundSize > sizeOfTextBackground)
    sizeOfTextBackground = neededBackgroundSize;
  else if(sizeOfTextBackground - neededBackgroundSize > 40.0f)
    sizeOfTextBackground -= 6.0f;
  else if(sizeOfTextBackground - neededBackgroundSize > 20.0f)
    sizeOfTextBackground -= 4.0f;
  else if(neededBackgroundSize < sizeOfTextBackground)
    sizeOfTextBackground -= 2.0f;
  glBegin(GL_QUADS);
  glVertex2f(0.0f, 0.0f);
  glVertex2f((GLfloat)width, 0.0f);
  glVertex2f((GLfloat)width, sizeOfTextBackground);
  glVertex2f(0.0f, sizeOfTextBackground);
  glEnd();

  // draw text
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);
  GLint x(10), y((10 + (textobjects.size()-1)*20));
  glPushAttrib(GL_LIST_BIT);
  glListBase(basefontDLs - 32);
  for(iter = textobjects.begin(); iter != textobjects.end(); ++iter)
  {
    glColor4f((*iter)->getColor(0), (*iter)->getColor(1),
      (*iter)->getColor(2), (*iter)->getColor(3));
    glRasterPos2i(x, y);
    glCallLists(strlen((*iter)->getText()), GL_UNSIGNED_BYTE, (*iter)->getText());
    y -= 20;
  }
  glDisable(GL_BLEND);
  glEnable(GL_DEPTH_TEST);
  glPopAttrib();
}

void GLHelper::setMaxMessageType(const TextObject::TextType type)
{
  maxMessageType = type;
}

GLvoid GLHelper::renderViewportBorder(GLuint width, GLuint height)
{
  glDisable(GL_DEPTH_TEST);
  glColor4f(1.0f, 0.5f, 0.5f, 0.8f);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0.0, width, 0.0, height, 0.0, 0.2);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(0.0, 0.0, GLfloat(-0.1));
  glBegin(GL_QUADS);
  glVertex2f(0.0f, (GLfloat)height);
  glVertex2f(3.0f, (GLfloat)height-3.0f);
  glVertex2f(3.0f, 3.0f);
  glVertex2f(0.0f, 0.0f);
  glEnd();
  glBegin(GL_QUADS);
  glVertex2f((GLfloat)width, (GLfloat)height);
  glVertex2f((GLfloat)width-3.0f, (GLfloat)height-3.0f);
  glVertex2f(3.0f, (GLfloat)height-3.0f);
  glVertex2f(0.0f, (GLfloat)height);
  glEnd();
  glBegin(GL_QUADS);
  glVertex2f((GLfloat)width, 0.0f);
  glVertex2f((GLfloat)width-3.0f, 3.0f);
  glVertex2f((GLfloat)width-3.0f, (GLfloat)height-3.0f);
  glVertex2f((GLfloat)width, (GLfloat)height); 
  glEnd();
  glBegin(GL_QUADS);
  glVertex2f(0.0f, 0.0f);
  glVertex2f(3.0f, 3.0f);
  glVertex2f((GLfloat)width-3.0f, 3.0f);
  glVertex2f((GLfloat)width, 0.0f);
  glEnd();
  glDisable(GL_BLEND);
  glEnable(GL_DEPTH_TEST);
}

GLvoid GLHelper::glPrintTextureUnits(int interval_begin, int interval_end, const char* marker)
{
  int max_units;
  glGetIntegerv(GL_MAX_TEXTURE_UNITS, &max_units);

  if(interval_begin < 0)
    interval_begin = 0;
  else if(interval_begin > max_units)
    interval_begin = max_units;

  if(interval_end == 0)
    interval_end = max_units;
  else if(interval_end < interval_begin)
    interval_end = interval_begin;
  else if(interval_end > max_units)
    interval_end = max_units;

  glPrintMarkedText(TextObject::COMMON_HINTS, marker, TextObject::WHITE, 1000, "Max Texture Units supported %d:", max_units);
  glPrintMarkedText(TextObject::COMMON_HINTS, marker, TextObject::WHITE, 1000, "Texture Unit Setup at \"%s\":", marker);
  for(int i=interval_begin; i<interval_end; i++)
  {
    if(GLEW_VERSION_1_3)
      glActiveTexture(GL_TEXTURE0 + i);
    GLboolean tu_2d = glIsEnabled(GL_TEXTURE_2D);
    GLboolean tu_cubemap = glIsEnabled(GL_TEXTURE_CUBE_MAP);
    GLboolean tu_texgenS = glIsEnabled(GL_TEXTURE_GEN_S);
    GLboolean tu_texgenT = glIsEnabled(GL_TEXTURE_GEN_T);
    GLboolean tu_texgenR = glIsEnabled(GL_TEXTURE_GEN_R);
    GLboolean tu_texgenQ = glIsEnabled(GL_TEXTURE_GEN_Q);
    std::stringstream buf;
    buf << "TextureUnit";
    buf << i;
    buf << ": GL_TEXTURE2D=%d GL_CUBE_MAP=%d texGenS=%d texGenT=%d texGenR=%d texGenQ=%d";
    glPrintMarkedText(TextObject::COMMON_HINTS, marker, TextObject::WHITE, 1000, buf.str().c_str(), (int)tu_2d, (int)tu_cubemap,
      (int)tu_texgenS, (int)tu_texgenT, (int)tu_texgenR, (int)tu_texgenQ);
    if(!GLEW_VERSION_1_3)
      break;
  }
}

GLvoid GLHelper::setMatrix(const Vector3d& translation,
                           const Matrix3d& rotation,
                           GLfloat* modelmatrix)
{
  modelmatrix[0] = (float)rotation.col[0].v[0];
  modelmatrix[1] = (float)rotation.col[0].v[1];
  modelmatrix[2] = (float)rotation.col[0].v[2];
  modelmatrix[3] = modelmatrix[7] = modelmatrix[11] = 0.0f;
  modelmatrix[4] = (float)rotation.col[1].v[0];
  modelmatrix[5] = (float)rotation.col[1].v[1];
  modelmatrix[6] = (float)rotation.col[1].v[2];
  modelmatrix[8] = (float)rotation.col[2].v[0];
  modelmatrix[9] = (float)rotation.col[2].v[1];
  modelmatrix[10] = (float)rotation.col[2].v[2];
  modelmatrix[12] = (float)translation.v[0];
  modelmatrix[13] = (float)translation.v[1];
  modelmatrix[14] = (float)translation.v[2];
  modelmatrix[15] = 1.0f;
}

GLvoid GLHelper::setMatrix(const Vector3d& translation,
                           const Matrix3d& rotation)
{
  GLdouble modelmatrix[16];
  modelmatrix[0] = rotation.col[0].v[0];
  modelmatrix[1] = rotation.col[0].v[1];
  modelmatrix[2] = rotation.col[0].v[2];
  modelmatrix[3] = modelmatrix[7] = modelmatrix[11] = 0.0f;
  modelmatrix[4] = rotation.col[1].v[0];
  modelmatrix[5] = rotation.col[1].v[1];
  modelmatrix[6] = rotation.col[1].v[2];
  modelmatrix[8] = rotation.col[2].v[0];
  modelmatrix[9] = rotation.col[2].v[1];
  modelmatrix[10] = rotation.col[2].v[2];
  modelmatrix[12] = translation.v[0];
  modelmatrix[13] = translation.v[1];
  modelmatrix[14] = translation.v[2];
  modelmatrix[15] = 1.0f;
  glMultMatrixd(modelmatrix);
}

GLvoid GLHelper::setInterpolatedMatrix(const Vector3d* translations,
                                       const Matrix3d* rotations,
                                       const float pos_offset,
                                       GLdouble* modelmatrix)
{
  int pos1, pos2;
  double f1, f2;
  getInterpolationPositionsAndFactors(pos_offset, pos1, pos2, f1, f2);

  Vector3d translation1 = *(translations + pos1);
  Vector3d translation2 = *(translations + pos2);

  Matrix3d rotation1 = *(rotations + pos1);
  Matrix3d rotation2 = *(rotations + pos2);

  // set linear interpolated values
  modelmatrix[0] = f1*rotation1.col[0].v[0] + f2*rotation2.col[0].v[0];
  modelmatrix[1] = f1*rotation1.col[0].v[1] + f2*rotation2.col[0].v[1];
  modelmatrix[2] = f1*rotation1.col[0].v[2] + f2*rotation2.col[0].v[2];
  modelmatrix[3] = modelmatrix[7] = modelmatrix[11] = 0.0f;
  modelmatrix[4] = f1*rotation1.col[1].v[0] + f2*rotation2.col[1].v[0];
  modelmatrix[5] = f1*rotation1.col[1].v[1] + f2*rotation2.col[1].v[1];
  modelmatrix[6] = f1*rotation1.col[1].v[2] + f2*rotation2.col[1].v[2];
  modelmatrix[8] = f1*rotation1.col[2].v[0] + f2*rotation2.col[2].v[0];
  modelmatrix[9] = f1*rotation1.col[2].v[1] + f2*rotation2.col[2].v[1];
  modelmatrix[10] = f1*rotation1.col[2].v[2] + f2*rotation2.col[2].v[2];
  modelmatrix[12] = f1*translation1.v[0] + f2*translation2.v[0];
  modelmatrix[13] = f1*translation1.v[1] + f2*translation2.v[1];
  modelmatrix[14] = f1*translation1.v[2] + f2*translation2.v[2];
  modelmatrix[15] = 1.0f;
}

GLvoid GLHelper::setInterpolatedMatrix(const Vector3d* translations,
                                       const Matrix3d* rotations,
                                       const float pos_offset)
{
  int pos1, pos2;
  double f1, f2;
  getInterpolationPositionsAndFactors(pos_offset, pos1, pos2, f1, f2);

  Vector3d translation1 = *(translations + pos1);
  Vector3d translation2 = *(translations + pos2);

  Matrix3d rotation1 = *(rotations + pos1);
  Matrix3d rotation2 = *(rotations + pos2);

  // set linear interpolated values
  GLdouble modelmatrix[16];
  modelmatrix[0] = f1*rotation1.col[0].v[0] + f2*rotation2.col[0].v[0];
  modelmatrix[1] = f1*rotation1.col[0].v[1] + f2*rotation2.col[0].v[1];
  modelmatrix[2] = f1*rotation1.col[0].v[2] + f2*rotation2.col[0].v[2];
  modelmatrix[3] = modelmatrix[7] = modelmatrix[11] = 0.0f;
  modelmatrix[4] = f1*rotation1.col[1].v[0] + f2*rotation2.col[1].v[0];
  modelmatrix[5] = f1*rotation1.col[1].v[1] + f2*rotation2.col[1].v[1];
  modelmatrix[6] = f1*rotation1.col[1].v[2] + f2*rotation2.col[1].v[2];
  modelmatrix[8] = f1*rotation1.col[2].v[0] + f2*rotation2.col[2].v[0];
  modelmatrix[9] = f1*rotation1.col[2].v[1] + f2*rotation2.col[2].v[1];
  modelmatrix[10] = f1*rotation1.col[2].v[2] + f2*rotation2.col[2].v[2];
  modelmatrix[12] = f1*translation1.v[0] + f2*translation2.v[0];
  modelmatrix[13] = f1*translation1.v[1] + f2*translation2.v[1];
  modelmatrix[14] = f1*translation1.v[2] + f2*translation2.v[2];
  modelmatrix[15] = 1.0f;
  glMultMatrixd(modelmatrix);
}

GLvoid GLHelper::setInterpolatedCameraLookAt(const Vector3d* translations,
                                             const Matrix3d* rotations,
                                             const float pos_offset)
{
  int pos1, pos2;
  double f1, f2;
  getInterpolationPositionsAndFactors(pos_offset, pos1, pos2, f1, f2);

  Vector3d translation1 = *(translations + pos1);
  Vector3d translation2 = *(translations + pos2);

  Matrix3d rotation = *(rotations + pos1);
  Matrix3d rotation2 = *(rotations + pos2);

  // calculate interpolation
  rotation.interpolate(rotation2, f1, f2);

  // set linear interpolated values
  Vector3d forwardVec(1.0,0.0,0.0);
  Vector3d upVec(0.0,0.0,1.0);
  forwardVec.rotate(rotation);
  upVec.rotate(rotation);
  gluLookAt(f1*translation1.v[0] + f2*translation2.v[0],
            f1*translation1.v[1] + f2*translation2.v[1],
            f1*translation1.v[2] + f2*translation2.v[2],
            f1*translation1.v[0] + f2*translation2.v[0] + forwardVec.v[0],
            f1*translation1.v[1] + f2*translation2.v[1] + forwardVec.v[1], 
            f1*translation1.v[2] + f2*translation2.v[2] + forwardVec.v[2],
            upVec.v[0], upVec.v[1], upVec.v[2]);
}

GLvoid GLHelper::getInterpolationPositionsAndFactors(const float& pos_offset, int& pos1, int& pos2, double& f1, double& f2)
{
  // calculate the 2 physic results which have to be combined
  pos1 = (int)(pos_offset + 1.0);
  if(pos1 < 0)
    pos1 += 12;
  else if(pos1 >= 12)
    pos1 -= 12;
  pos2 = (int)(pos_offset);
  if(pos2 < 0)
    pos2 += 12;

  // the interpolation factors
  float posf = pos_offset + 12.0f;
  f1 = posf - (int)posf;
  f2 = 1.0f - f1;
}

bool GLHelper::prepareMatrixForUnProject(const GLdouble modelViewMatrix[16],
                                              const GLdouble projectionMatrix[16],
                                              GLdouble unProjectMatrix[16])
{
  int i, j, k, swap;
  GLdouble temp[4][4];
  //Multiplicate matrices and store result temporarily:
  for (i = 0; i < 4; i++) 
    for (j = 0; j < 4; j++) 
      temp[i][j] = modelViewMatrix[i*4+0]*projectionMatrix[j] +
      modelViewMatrix[i*4+1]*projectionMatrix[j+4] +
      modelViewMatrix[i*4+2]*projectionMatrix[j+8] +
      modelViewMatrix[i*4+3]*projectionMatrix[j+12];
  //Set identity:
  unProjectMatrix[0] = 1.0; unProjectMatrix[1] = 0.0; unProjectMatrix[2] = 0.0; unProjectMatrix[3] = 0.0;
  unProjectMatrix[4] = 0.0; unProjectMatrix[5] = 1.0; unProjectMatrix[6] = 0.0; unProjectMatrix[7] = 0.0;
  unProjectMatrix[8] = 0.0; unProjectMatrix[9] = 0.0; unProjectMatrix[10] = 1.0; unProjectMatrix[11] = 0.0;
  unProjectMatrix[12] = 0.0; unProjectMatrix[13] = 0.0; unProjectMatrix[14] = 0.0; unProjectMatrix[15] = 1.0;
  //Invert Matrix:
  double t(0.0);
  for (i = 0; i < 4; i++) 
  {
    swap = i;
    for (j = i + 1; j < 4; j++) 
    {
      if (fabs(temp[j][i]) > fabs(temp[i][i])) 
      {
        swap = j;
      }
    }
    if (swap != i) 
    {
      for (k = 0; k < 4; k++) 
      {
        t = temp[i][k];
        temp[i][k] = temp[swap][k];
        temp[swap][k] = t;
        t = unProjectMatrix[i*4+k];
        unProjectMatrix[i*4+k] = unProjectMatrix[swap*4+k];
        unProjectMatrix[swap*4+k] = t;
      }
    }
    if (temp[i][i] == 0) 
    {
      return false;
    }
    t = temp[i][i];
    for (k = 0; k < 4; k++) 
    {
      temp[i][k] /= t;
      unProjectMatrix[i*4+k] /= t;
    }
    for (j = 0; j < 4; j++) 
    {
      if (j != i) 
      {
        t = temp[j][i];
        for (k = 0; k < 4; k++) 
        {
          temp[j][k] -= temp[i][k]*t;
          unProjectMatrix[j*4+k] -= unProjectMatrix[i*4+k]*t;
        }
      }
    }
  }
  return true;
}

bool GLHelper::unProject(GLdouble winx, GLdouble winy, GLdouble winz,
                              const GLdouble unProjectMatrix[16], 
                              const GLint viewport[4],
                              GLdouble *objx, GLdouble *objy, GLdouble *objz)
{
  double in[4];
  double out[4];

  in[0]=winx;
  in[1]=winy;
  in[2]=winz;
  in[3]=1.0;

  /* Map x and y from window coordinates */
  in[0] = (in[0] - viewport[0]) / viewport[2];
  in[1] = (in[1] - viewport[1]) / viewport[3];

  /* Map to range -1 to 1 */
  in[0] = in[0] * 2 - 1;
  in[1] = in[1] * 2 - 1;
  in[2] = in[2] * 2 - 1;

  for (int i=0; i<4; i++) 
  {
    out[i] = in[0] * unProjectMatrix[i] +
      in[1] * unProjectMatrix[i+4] +
      in[2] * unProjectMatrix[i+8] +
      in[3] * unProjectMatrix[i+12];
  }
  if (out[3] == 0.0)
    return false;
  out[0] /= out[3];
  out[1] /= out[3];
  out[2] /= out[3];
  *objx = out[0];
  *objy = out[1];
  *objz = out[2];
  return true;
}

bool GLHelper::isExtensionSupported(char* extensionName)
{
  const unsigned char *pszExtensions = NULL;
  const unsigned char *pszStart;
  unsigned char *pszWhere, *pszTerminator;

  // Extension names should not have spaces
  pszWhere = (unsigned char *) strchr( extensionName, ' ' );
  if( pszWhere || *extensionName == '\0' )
    return GL_FALSE;

  // Get Extensions String
  pszExtensions = glGetString( GL_EXTENSIONS );

  // Search The Extensions String For An Exact Copy
  pszStart = pszExtensions;
  for(;;)
  {
    pszWhere = (unsigned char *) strstr( (const char *) pszStart, extensionName );
    if( !pszWhere )
      break;
    pszTerminator = pszWhere + strlen( extensionName );
    if( pszWhere == pszStart || *( pszWhere - 1 ) == ' ' )
      if( *pszTerminator == ' ' || *pszTerminator == '\0' )
        return GL_TRUE;
    pszStart = pszTerminator;
  }
  return GL_FALSE;
}

GLvoid GLHelper::drawHinge(const Vector3d& position, const Vector3d& axis, const bool whiteDrawing)
{
  glPushMatrix();

  Matrix3d rotation;
  //Go to position and rotation:
  setMatrix(position, rotation);

  // draw joint axis
  if(whiteDrawing)
    glColor4f(float(1.0), float(1.0), float(1.0),float(1.0));
  else
    glColor4f(float(0.98),float(0.59),float(0.0), float(1.0));
  glBegin(GL_LINES);
  glVertex3d(0.0,0.0,0.0);
  glVertex3d(axis.v[0], axis.v[1], axis.v[2]);
  glEnd();
  // draw anchor point
  if(whiteDrawing)
    glColor4f(float(1.0), float(1.0), float(1.0),float(1.0));
  else
    glColor4f(float(0.1), float(0.1), float(0.9), float(1.0));
  GLUquadricObj* q = gluNewQuadric();
  gluSphere(q, 0.003, 15, 15);
  gluDeleteQuadric(q);

  glPopMatrix();
}


GLvoid GLHelper::drawUniversalJoint(const Vector3d& position, const Vector3d& axis1, const Vector3d& axis2, const bool whiteDrawing)
{
  glPushMatrix();

  Matrix3d rotation;
  //Go to position and rotation:
  setMatrix(position, rotation);

  // draw joint axis
  if(whiteDrawing)
    glColor4f(float(1.0), float(1.0), float(1.0),float(1.0));
  else
    glColor4f(float(0.98),float(0.59),float(0.0), float(1.0));
  glBegin(GL_LINES);
  glVertex3d(0.0,0.0,0.0);
  glVertex3d(axis1.v[0], axis1.v[1], axis1.v[2]);
  glEnd();
  glBegin(GL_LINES);
  glVertex3d(0.0,0.0,0.0);
  glVertex3d(axis2.v[0], axis2.v[1], axis2.v[2]);
  glEnd();
  // draw anchor point
  if(whiteDrawing)
    glColor4f(float(1.0), float(1.0), float(1.0),float(1.0));
  else
    glColor4f(float(0.1), float(0.1), float(0.9), float(1.0));
  GLUquadricObj* q = gluNewQuadric();
  gluSphere(q, 0.003, 15, 15);
  gluDeleteQuadric(q);

  glPopMatrix();
}


GLvoid GLHelper::drawWheelSuspension(const Vector3d& position, const Vector3d& axis1, const Vector3d& axis2, const bool whiteDrawing)
{
  glPushMatrix();

  Matrix3d rotation;
  //Go to position and rotation:
  setMatrix(position, rotation);

  // draw joint axis
  if(whiteDrawing)
    glColor4f(float(1.0), float(1.0), float(1.0),float(1.0));
  else
    glColor4f(float(0.98),float(0.59),float(0.0), float(1.0));
  glBegin(GL_LINES);
  glVertex3d(0.0,0.0,0.0);
  glVertex3d(axis1.v[0], axis1.v[1], axis1.v[2]);
  glEnd();
  glBegin(GL_LINES);
  glVertex3d(0.0,0.0,0.0);
  glVertex3d(axis2.v[0], axis2.v[1], axis2.v[2]);
  glEnd();
  // draw anchor point
  if(whiteDrawing)
    glColor4f(float(1.0), float(1.0), float(1.0),float(1.0));
  else
    glColor4f(float(0.1), float(0.1), float(0.9), float(1.0));
  GLUquadricObj* q = gluNewQuadric();
  gluSphere(q, 0.003, 15, 15);
  gluDeleteQuadric(q);

  glPopMatrix();
}


GLvoid GLHelper::drawSlider(const Vector3d& position, const Vector3d& axis, const bool whiteDrawing)
{
  glPushMatrix();

  Matrix3d rotation;
  //Go to position and rotation:
  setMatrix(position, rotation);

  // draw joint axis
  if(whiteDrawing)
    glColor4f(float(1.0), float(1.0), float(1.0),float(1.0));
  else
    glColor4f(float(0.98),float(0.59),float(0.0), float(1.0));
  glBegin(GL_LINES);
  glVertex3d(0.0,0.0,0.0);
  glVertex3d(axis.v[0], axis.v[1], axis.v[2]);
  glEnd();
  // draw anchor point
  if(whiteDrawing)
    glColor4f(float(1.0), float(1.0), float(1.0),float(1.0));
  else
    glColor4f(float(0.1), float(0.1), float(0.9), float(1.0));
  GLUquadricObj* q = gluNewQuadric();
  gluSphere(q, 0.003, 15, 15);
  gluDeleteQuadric(q);

  glPopMatrix();
}


GLvoid GLHelper::drawBallJoint(const Vector3d& position, const bool whiteDrawing)
{
  glPushMatrix();

  Matrix3d rotation;
  //Go to position and rotation:
  setMatrix(position, rotation);

  // draw anchor point
  if(whiteDrawing)
    glColor4f(float(1.0), float(1.0), float(1.0),float(1.0));
  else
    glColor4f(float(0.1), float(0.1), float(0.9), float(1.0));
  GLUquadricObj* q = gluNewQuadric();
  gluSphere(q, 0.003, 15, 15);
  gluDeleteQuadric(q);

  glPopMatrix();
}

int GLHelper::getFBOreg()
{
  for(int i=0; i<8; i++)
  {
    if(!fboRegister[i])
    {
      fboRegister[i] = true;
      return i;
    }
  }
  return -1;
}

void GLHelper::removeFBOreg(const int i)
{
  if(i >= 0 && i < 8)
    fboRegister[i] = false;
}
