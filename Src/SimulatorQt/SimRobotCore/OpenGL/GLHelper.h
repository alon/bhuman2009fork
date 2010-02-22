/**
* @file GLHelper.h
*
* Functions to drawing text messages and debug drawings
* as well as some common matrix calculations
*
* @author <A href="mailto:pachur@informatik.uni-bremen.de">Dennis Pachur</A>
*/

#ifndef __GLHELPER_H__
#define __GLHELPER_H__

#include <vector>
#include <string>

class Vector3d;
class Matrix3d;

/** A Text Object to print on a OpenGl surface for a amount of time */
class TextObject
{
public:

  enum TextColor
  {
    WHITE,
    LIGHTGREY,
    RED,
    LIGHTRED,
    BLUE,
    LIGHTBLUE,
    GREEN,
    LIGHTGREEN,
    WARNING
  };

  enum TextType
  {
    NO_HINTS = 0,
    WARNING_HINTS,
    COMMON_HINTS,
    ALL_HINTS
  };

  /** The constructor
  * @param text The text message with additional values included
  * @param textNoValues The original text message without additional values
  * @param duration The amount of time the message will exist in msec
  * @param color The text color (WARNING will blink)
  */
  TextObject(const std::string& text, const std::string& textNoValues, unsigned long duration, TextColor color);

  /** The extended constructor
  * @param text The text message with additional values included
  * @param textNoValues The original text message without additional values
  * @param duration The amount of time the message will exist in msec
  * @param marker A marker to avoid replacing the textobject
  */
  TextObject(const std::string& text, const std::string& textNoValues, unsigned long duration, TextColor color, const std::string& marker);

  /** The destructor */
  ~TextObject(){};

  /** Check if the displaytime of the message is still in duration
  * @return If it is still within the defined duration
  */
  bool checkInDuration() const;

  /** Get the remaining displaytime of the message
  * @return The remaining displaytime in milliseconds
  */
  unsigned int getRemainingTime() const;

  /** Get the text color
  * @param channel The color channel
  * @return The text color
  */
  float getColor(unsigned int channel) const;

  /** Get the text message */
  const char* getText() const { return text.c_str(); }

  /** Get the text message without additional values */
  const char* getTextNoValues() const { return textNoValues.c_str(); }

  /** Get the marker */
  const char* getMarker() const { return marker.c_str(); }

  /** Update duration and textmessage
  * @param text The new text message
  * @param color The new text color
  * @param duration The new duration
  */
  void updateDuration(const std::string& text, TextColor color, unsigned long duration);

protected:
  /** The text with values */
  std::string text;
  /** The text without values */
  std::string textNoValues;
  /** A marker to keep track of object */
  std::string marker;
  /** The amount of time the message will exist */
  unsigned long duration;
  /** The systemtime of creation */
  unsigned int creationtime;
  /** The Textcolor */
  TextColor color;

};

// as singelton
class GLHelper
{
protected:
  static GLHelper* pinstance;
  /** constructor */
  GLHelper();
  /** destructor */
  ~GLHelper();

  /** a list of textobjects */
  std::vector<TextObject*> textobjects;
  /** flag if the base font has already been build */
  bool fonthasbeenbuild;
  /** display list handle for basefont */
  unsigned int basefontDLs;
  unsigned int shaderfontDLs;
  /** counter of additional printouts (creates y offset) */
  unsigned int additionalPrints;
  /** Textbackround size */
  float sizeOfTextBackground;
  /** the maximum type of message to print */
  TextObject::TextType maxMessageType;

  /** FBO registration */
  bool fboRegister[8];

  // helper function for interpolation of cycle order offset
  void getInterpolationPositionsAndFactors(const float& pos_offset, int& pos1, int& pos2, double& f1, double& f2);

public:

  // get a pointer to this singelton
  static GLHelper* getGLH();

  // destory the singleton
  static void destroy();

  /** Build a set of display lists to provide courier new character drawings */ 
  void CreateBaseFont();

  /** Kill the set of display lists */
  void DeleteFont();

  /** print function like printf that draws to the OpenGl viewport and
  * takes additional raster position coordinates
  * @param bold If the text should be bold
  * @param posx The x-coordinate where the text should begin
  * @param posy The y-coordinate where the text should begin
  * @param text The String to print
  * @param ParameterList to replace tags in the string (printf is used)
  */
  void glPrintf(bool bold, unsigned int &posx, unsigned int &posy, const char* text, ...);

  /** prints a string to the current gl Window for the amount of time
  * the function will automatically place the text and update the
  * timer if the same message is called again
  * @param type The type of the message (only activated messages are displayed)
  * @param color The textcolor
  * @param duration The time the text should be displayed
  * @param text The text which should be displayed
  * @param ParameterList to replace tags in the string (printf is used)
  */
  void glPrintText(const TextObject::TextType type, const TextObject::TextColor color,
    const unsigned long duration, const char* text, ...);

  /** prints a string to the current gl Window for the amount of time
  * the function will automatically place the text and update the
  * timer if the same message is called again (a marker keeps track of object)
  * @param type The type of the message (only activated messages are displayed)
  * @param marker A marker to avoid replacing by same text blocks
  * @param color The textcolor
  * @param duration The time the text should be displayed
  * @param text The text which should be displayed
  * @param ParameterList to replace tags in the string (printf is used)
  */
  void glPrintMarkedText(const TextObject::TextType type, const char* marker,
    const TextObject::TextColor color, const unsigned long duration, const char* text, ...);

  /** print setup of an interval of texture units
  * @param interval_begin The begin of the interval (usually 0)
  * @param interval_end The end of the interval (if 0 is passed, the max texture units number is used)
  * @param marker A const char* to an identifier of the code passage
  */
  void glPrintTextureUnits(int interval_begin, int interval_end, const char* marker);

  /** render all text objects and check some possible errors
  * @param width The width of the window
  * @param height The height of the window
  */
  void renderText(unsigned int width, unsigned int height);

  /** set the maximum type of messages to handle (ALL / COMMON / WARNING / NONE)
  * @param type The maximum type to display
  */
  void setMaxMessageType(const TextObject::TextType type);

  /** render a small border around the inner of the viewport
  * @param width The viewports width
  * @param width The viewports height
  */
  void renderViewportBorder(unsigned int width, unsigned int height);

  /** Set the translation and rotation into an user given matrix
  * @param translation The Vector of the object translation
  * @param rotation The Matrix of the object rotations
  * @param modelmatrix The modelmatrix to set (4x4)
  */
  void setMatrix(const Vector3d& translation,
                   const Matrix3d& rotation,
                   float* modelmatrix);

  /** Multiply the translation and rotation with the modelview matrix of OpenGL
  * @param translation The Vector of the object translation
  * @param rotation The Matrix of the object rotations
  */
  void setMatrix(const Vector3d& translation,
                   const Matrix3d& rotation);

  /** Multiply the translation and rotation of 2 seperate physic updates
  *   with the modelview matrix of OpenGL
  * @param translation The Vector of the object translation
  * @param rotation The Matrix of the object rotations
  * @param pos_offset The offset of the physic updated position array (12 elements)
  */
  void setInterpolatedMatrix(const Vector3d* translations,
                               const Matrix3d* rotations,
                               const float pos_offset);

  /** Set the translation and rotation of 2 seperate physic updates
  *   into an user given matrix
  * @param translation The Vector of the object translation
  * @param rotation The Matrix of the object rotations
  * @param pos_offset The offset of the physic updated position array (12 elements)
  * @param modelmatrix The modelmatrix to set (4x4)
  */
  void setInterpolatedMatrix(const Vector3d* translations,
                               const Matrix3d* rotations,
                               const float pos_offset,
                               double* modelmatrix);

  /** Use the gluLookAt-function with 2 seperate physic updates
  * @param translation The Vector of the object translation
  * @param rotation The Matrix of the object rotations
  * @param pos_offset The offset of the physic updated position array (12 elements)
  */
  void setInterpolatedCameraLookAt(const Vector3d* translations,
                                     const Matrix3d* rotations,
                                     const float pos_offset);

  bool prepareMatrixForUnProject(const double modelViewMatrix[16],
                                      const double projectionMatrix[16],
                                      double unProjectMatrix[16]);

  bool unProject(double winx, double winy, double winz,
                      const double unProjectMatrix[16], 
                      const int viewport[4],
                      double *objx, double *objy, double *objz);

  /** Check if an extension is supported
  * deprecated: use GLEW querys instead
  * @param extensionName The name of the extension
  */
  bool isExtensionSupported(char* extensionName);

  /** draw a hinge */
  void drawHinge(const Vector3d& position, const Vector3d& axis, const bool whiteDrawing);

  /** draw a universal joint */
  void drawUniversalJoint(const Vector3d& position, const Vector3d& axis1, const Vector3d& axis2, const bool whiteDrawing);

  /** draw a wheel suspension */
  void drawWheelSuspension(const Vector3d& position, const Vector3d& axis1, const Vector3d& axis2, const bool whiteDrawing);

  /** draw a slider */
  void drawSlider(const Vector3d& position, const Vector3d& axis, const bool whiteDrawing);

  /** draw a ball joint */
  void drawBallJoint(const Vector3d& position, const bool whiteDrawing);

  /** window 2 FBO registration */
  int getFBOreg();
  void removeFBOreg(const int i);
};

#endif //__GLHELPER_H__
