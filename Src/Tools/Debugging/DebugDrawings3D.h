/**
* @file Tools/Debugging/DebugDrawings3D.h
*/ 

#ifndef __DebugDrawings3D_h_
#define __DebugDrawings3D_h_

#include "Tools/Debugging/Debugging.h"

#include "Platform/hash_map.h"
USING_HASH_MAP

class Drawings3D
{
public:
  /** IDs for shape types.
  * Shapes are the basic drawings that can be sent.
  */
  enum ShapeType
  {
    dot,
    line,
    cube,
    polygon,
    quad,
    coordinates,
    scale,
    rotate,
    translate,
    sphere,
    cylinder,
    image
  };
};

class RobotConsole;
class DrawingManager3D;

In& operator>>(In& stream, DrawingManager3D&);
Out& operator<<(Out& stream, const DrawingManager3D&);

/**
* singleton drawing manager class
*/
class DrawingManager3D
{
private:
  /**
  * Default constructor.
  * No other instance of this class is allowed except the one accessible via getDrawingManager
  * therefore the constructor is private.
  */
  DrawingManager3D(): processIdentifier(0){
    drawing3DNameTypeTable["field"]    = 0;
    drawing3DNameTypeTable["robot"]    = 1;
    drawing3DNameTypeTable["camera"]   = 2;
    drawing3DNameTypeTable["origin"]   = 3;
    }

  /**
  * Copy constructor.
  * Copying instances of this class is not allowed
  * therefore the copy constructor is private.
  */
  DrawingManager3D(const DrawingManager3D&) {}

  /*
  * only a process is allowed to create the instance.
  */
  friend class Process;
  friend class RobotConsole;

  typedef hash_map<std::string, int> StringTable;
  StringTable stringTable;
  char processIdentifier;

  const char* getString(const std::string& string);

public:
  class DrawingNameIdTableEntry
  {
  public:
    char id;
    char type;
  };
  

  typedef hash_map< const char*, DrawingNameIdTableEntry> DrawingNameIdTable;
  DrawingNameIdTable drawingNameIdTable;
  
   typedef hash_map< const char*, char> Drawing3DNameTypeTable;
  Drawing3DNameTypeTable drawing3DNameTypeTable;

  ~DrawingManager3D() {clear();}

  void setProcess(char processIdentifier) {this->processIdentifier = processIdentifier;}

  void clear();

  void addDrawingId(const char* name, const char* type);

  char getDrawingId(const char* name) const
  { 
    DrawingNameIdTable::const_iterator i = drawingNameIdTable.find(name);
    if(i != drawingNameIdTable.end())
      return i->second.id;
    OUTPUT(idText, text, "Warning! Debug drawing " << name << " not declared");
    return -1;
  }

  const char* getDrawing3DType(const char* name) const
  { 
    StringTable::const_iterator s = stringTable.find(name);
    if(s != stringTable.end())
    {
      DrawingNameIdTable::const_iterator i = drawingNameIdTable.find(s->first.c_str());
      if(i != drawingNameIdTable.end())
      {
        for(Drawing3DNameTypeTable::const_iterator j = drawing3DNameTypeTable.begin(); 
            j != drawing3DNameTypeTable.end(); ++j)
          if(j->second == i->second.type)
            return j->first;
        OUTPUT(idText, text, "Warning! Debug3D drawing " << name << " has unknown type " << int(i->second.type));
        return "unknown";
      }
    }
    OUTPUT(idText, text, "Warning! Debug3D drawing " << name << " not declared");
    return "unknown";
  }

  const char* getDrawingName(char id) const
  {
    for(DrawingNameIdTable::const_iterator i = drawingNameIdTable.begin(); 
        i != drawingNameIdTable.end(); ++i)
      if(i->second.id == id)
        return i->first;
    OUTPUT(idText, text, "Warning! Unknown debug drawing id " << int(id));
    return "unknown";
  }

  friend In& operator>>(In& stream, DrawingManager3D&);
  friend Out& operator<<(Out& stream, const DrawingManager3D&);
};

#ifdef RELEASE

#define DECLARE_DEBUG_DRAWING3D(id, type) ((void) 0)
#define LINE3D(id, fromX, fromY, fromZ, toX, toY, toZ, size, color) ((void) 0)
#define COORDINATES3D(id, length, width) ((void) 0)
#define SCALE3D(id, x, y, z) ((void) 0)
#define ROTATE3D(id, x, y, z) ((void) 0)
#define TRANSLATE3D(id, x, y, z) ((void) 0)
#define POINT3D(id, x, y, z, size, color) ((void) 0)
#define COMPLEX_DRAWING3D(id, expression) ((void) 0)
#define CUBE3D(id, a, b, c, d, e, f, g, h, size, color)((void) 0)
#define SPHERE3D(id, x, y, z, radius, color) ((void) 0)
#define CYLINDER3D(id, x, y, z, a, b, c, radius, height, color) ((void) 0)
#define IMAGE3D(id, x, y, z, a, b, c, width, height, color) ((void) 0)

#else

/** 
* A macro that declares
* @param id A drawing id
* @param type A coordinate-system type ("field", "robot", "camera" or "origin")
*/
#define DECLARE_DEBUG_DRAWING3D(id, type) \
  Global::getDrawingManager3D().addDrawingId(id, type); \
  DEBUG_RESPONSE("debug drawing 3d:" id, ); \

/** 
* A macro that adds a line to a drawing.
* @param id The drawing to which the line should be added.
* @param fromX The x-coordinate of the first point.
* @param fromY The y-coordinate of the first point.
* @param fromZ The z-coordinate of the first point.
* @param toX The x-coordinate of the second point.
* @param toY The y-coordinate of the second point.
* @param toZ The z-coordinate of the second point.
* @param size The thickness of the line in pixels.
* @param color The color of the line
*/
#define LINE3D(id, fromX, fromY, fromZ, toX, toY, toZ, size, color) \
  NOT_POLLABLE_DEBUG_RESPONSE("debug drawing 3d:" id, \
    OUTPUT(idDebugDrawing3D, bin, \
      (char)Drawings3D::line << \
      (char)Global::getDrawingManager3D().getDrawingId(id) << \
      (double)(fromX) << (double)(fromY) << (double)(fromZ) << (double)(toX) << (double)(toY) << (double)(toZ) << \
      (float)(size) << \
      ColorRGBA(color) \
   ); \
 )

/** 
* A macro that adds a rectangle to a drawing.
* @param id The drawing to which the line should be added.
* @param a,b,c,d,e,f,g,h The x,y and z parameter of the cornerpoints.
* @param size The thickness of the line in pixels.
* @param color The color of the line
*/
#define CUBE3D(id, a, b, c, d, e, f, g, h, size, color) \
  NOT_POLLABLE_DEBUG_RESPONSE("debug drawing 3d:" id, \
  OUTPUT(idDebugDrawing3D, bin, \
  (char)Drawings3D::cube << \
  (char)Global::getDrawingManager3D().getDrawingId(id) << \
  Vector3<double>(a) << Vector3<double>(b) << Vector3<double>(c) << Vector3<double>(d) << Vector3<double>(e) << Vector3<double>(f) << \
  Vector3<double>(g) << Vector3<double>(h) <<\
  (float)(size) << \
  ColorRGBA(color) \
  ); \
  )

/**
* A macro that draws three lines on the three axis in positive direction.
* @param id The id of the drawing
* @param length The length of those lines in mm
* @param width The width of the lines in pixel
*/
#define COORDINATES3D(id, length, width) \
  NOT_POLLABLE_DEBUG_RESPONSE("debug drawing 3d:" id, \
    OUTPUT(idDebugDrawing3D, bin, \
      (char)Drawings3D::coordinates << \
      (char)Global::getDrawingManager3D().getDrawingId(id) << \
      (double)(length) << (float)(width) \
   ); \
 )

/**
* A macro that scales all drawing elements.
* Should be used to adjust the axes to those used be the robot
* (concerning -/+) (e.g. "robot",1.0,-1.0,1.0)
* @param id The drawing the scaling should be applied to.
* @param x Scale factor for x axis.
* @param y Scale factor for y axis.
* @param z Scale factor for z axis.
*/
#define SCALE3D(id, x, y, z) \
  NOT_POLLABLE_DEBUG_RESPONSE("debug drawing 3d:" id, \
    OUTPUT(idDebugDrawing3D, bin, \
      (char)Drawings3D::scale << \
      (char)Global::getDrawingManager3D().getDrawingId(id) << \
      (double)(x) << (double)(y) << (double)(z) \
   ); \
 )

/**
* A macro that rotates the drawing around the three axes. 
* First around x, then y, then z.
* @param id The drawing the rotation should be applied to.
* @param x Radiants to rotate ccw around the x axis.
* @param y Radiants to rotate ccw around the y axis.
* @param z Radiants to rotate ccw around the z axis.
*
*/
#define ROTATE3D(id, x, y, z) \
  NOT_POLLABLE_DEBUG_RESPONSE("debug drawing 3d:" id, \
    OUTPUT(idDebugDrawing3D, bin, \
      (char)Drawings3D::rotate << \
      (char)Global::getDrawingManager3D().getDrawingId(id) << \
      (double)(x) << (double)(y) << (double)(z) \
   ); \
 )

/**
* A macro that translates the drawing according to the given coordinates. 
* The translation is the last thing done (after rotating and scaling)
* @param id The drawing the translation should be applied to.
* @param x mms to translate in x direction.
* @param y mms to translate in x direction.
* @param z mms to translate in x direction.
*
*/
#define TRANSLATE3D(id, x, y, z) \
  NOT_POLLABLE_DEBUG_RESPONSE("debug drawing 3d:" id, \
    OUTPUT(idDebugDrawing3D, bin, \
      (char)Drawings3D::translate << \
      (char)Global::getDrawingManager3D().getDrawingId(id) << \
      (double)(x) << (double)(y) << (double)(z) \
   ); \
 )

/** A macro that adds a point/dot to a drawing
* @param id The drawing to which the point will be added
* @param x The x-coordinate of the point.
* @param y The y-coordinate of the point.
* @param z The z-coordinate of the point.
* @param size The size of the point.
* @param color The color of the line
*/
#define POINT3D(id, x, y, z, size, color) \
  NOT_POLLABLE_DEBUG_RESPONSE("debug drawing 3d:" id, \
    OUTPUT(idDebugDrawing3D, bin, \
      (char)Drawings3D::dot << \
      (char)Global::getDrawingManager3D().getDrawingId(id) << \
      (double)(x) << (double)(y) << (double)(z) << \
      (float)size << \
      ColorRGBA(color) << false \
   ); \
 )

/** A macro that adds a sphere to a drawing
* @param id The drawing to which the sphere will be added
* @param x The x-coordinate of the sphere.
* @param y The y-coordinate of the sphere.
* @param z The z-coordinate of the sphere.
* @param radius The radius of the sphere.
* @param color The color of the line
*/
#define SPHERE3D(id, x, y, z, radius, color) \
  NOT_POLLABLE_DEBUG_RESPONSE("debug drawing 3d:" id, \
    OUTPUT(idDebugDrawing3D, bin, \
      (char)Drawings3D::sphere << \
      (char)Global::getDrawingManager3D().getDrawingId(id) << \
      (double)(x) << (double)(y) << (double)(z) << \
      (double) (radius) << \
      ColorRGBA(color) \
   ); \
 )

/** A macro that adds a cylinder to a drawing
* @param id The drawing to which the cylinder will be added
* @param x The x-coordinate of the cylinder.
* @param y The y-coordinate of the cylinder.
* @param z The z-coordinate of the cylinder.
* @param a The rotation around the x-axis in radians.
* @param b The rotation around the y-axis in radians.
* @param c The rotation around the z-axis in radians.
* @param radius The radius of the cylinder.
* @param height The height of the cylinder.
* @param color The color of the line
*/
#define CYLINDER3D(id, x, y, z, a, b, c, radius, height, color) \
  NOT_POLLABLE_DEBUG_RESPONSE("debug drawing 3d:" id, \
    OUTPUT(idDebugDrawing3D, bin, \
      (char)Drawings3D::cylinder << \
      (char)Global::getDrawingManager3D().getDrawingId(id) << \
      (double)(x) << (double)(y) << (double)(z) << \
      (double)(a) << (double)(b) << (double)(c) << \
      (double) (radius) << (double) (height) << \
      ColorRGBA(color) \
   ); \
 )

/** A macro that adds an image to a drawing
* @param id The drawing to which the image will be added
* @param x The x-coordinate of the center of the image.
* @param y The y-coordinate of the center of the image.
* @param z The z-coordinate of the center of the image.
* @param a The rotation around the x-axis in radians.
* @param b The rotation around the y-axis in radians.
* @param c The rotation around the z-axis in radians.
* @param width The width of the image.
* @param height The height of the image.
* @param i The image.
*/
#define IMAGE3D(id, x, y, z, a, b, c, width, height, i) \
  NOT_POLLABLE_DEBUG_RESPONSE("debug drawing 3d:" id, \
    OUTPUT(idDebugDrawing3D, bin, \
      (char)Drawings3D::image << \
      (char)Global::getDrawingManager3D().getDrawingId(id) << \
      (double)(x) << (double)(y) << (double)(z) << \
      (double)(a) << (double)(b) << (double)(c) << \
      (double) (width) << (double) (height) << \
      i \
   ); \
 )

/** 
* Complex drawings should be encapsuled by this macro.
*/
#define COMPLEX_DRAWING3D(id, expression) \
  NOT_POLLABLE_DEBUG_RESPONSE("debug drawing 3d:" id, \
    expression; \
 )

#endif // RELEASE

#endif //__DebugDrawings_h_
