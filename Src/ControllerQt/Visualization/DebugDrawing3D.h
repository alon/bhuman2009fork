/** 
* @file Visualization/DebugDrawing3D.h
* Declaration of class DebugDrawing3D.
*
* @author Philippe Schober
*/

#ifndef __DebugDrawing3D_h_
#define __DebugDrawing3D_h_

#include "Tools/Math/Pose2D.h"
#include "Tools/Math/Pose3D.h"
#include "Tools/Math/Vector3.h"
#include "Tools/Debugging/DebugDrawings3D.h"
#include <SimRobotCore/Controller/Controller3DDrawing.h>

#include "Tools/MessageQueue/InMessage.h"
//#include "Tools/List.h"

#include <vector>

class DebugDrawing3D;
class RobotConsole;

/**
* Streaming operator that reads a DebugDrawing3D from a stream.
* @param stream The stream from which is read.
* @param debugDrawing3D The DebugDrawing3D object.
* @return The stream.
*/ 
In& operator>>(In& stream, DebugDrawing3D& debugDrawing3D);

/**
* Streaming operator that writes a DebugDrawing3D to a stream.
* @param stream The stream to write on.
* @param debugDrawing3D The DebugDrawing3D object.
* @return The stream.
*/ 
Out& operator<<(Out& stream, const DebugDrawing3D& debugDrawing3D);

/**
*/
class DebugDrawing3D : public Controller3DDrawing
{
public:
  /** Default constructor. */
  DebugDrawing3D();

  /** Copy constructor for a DebugDrawing object. */
  DebugDrawing3D(const DebugDrawing3D& other);
  
  /** Copy constructor for a DebugDrawing object. */
  DebugDrawing3D(const DebugDrawing3D* pDebugDrawing3D);
  
  /** Destructor. */
  ~DebugDrawing3D();

  /** draw the elements of this drawing. */
  virtual void draw();

  /** The function empties the drawing. */
  void reset();
  
  /** Assignment operator.*/
  const DebugDrawing3D& operator=(const DebugDrawing3D& other);
  
  /** Adds the contents of another debug drawing to this one. */
  const DebugDrawing3D& operator+=(const DebugDrawing3D& other);

  
  /** base class for all drawing elements */
  class Element
  {
  public:
    enum {DOT, LINE, POLYGON, QUAD, SPHERE, CYLINDER, IMAGE} type; 
    ColorRGBA color;
    float width;
  };

  /** Stores a quad */
  class Quad : public Element
  {
  public: 
    Vector3<double> points[4];
    Quad() { type = QUAD; }
  };

  /** Stores a polygon */
  class Polygon : public Element
  {
  public: 
    Vector3<double> points[3];
    Polygon() { type = POLYGON; }
  };
      
  /** Stores a line */
  class Line : public Element
  {
  public:
    Vector3<double> points[2];
    Line() { type = LINE; }
  };

  class Dot : public Element
  {
  public:
    Vector3<double> point;
    Dot() { type = DOT; }
  };
  
  class Sphere : public Element
  {
  public:
    Vector3<double> point;
    double radius;
    Sphere() { type = SPHERE; }
  };
  
  class Cylinder : public Element
  {
  public:
    Vector3<double> point;
    Vector3<double> rotation;
    double radius, 
           height;
    Cylinder() { type = CYLINDER; }
  };
  
  class Image3D : public Element
  {
  public:
    Vector3<double> point;
    Vector3<double> rotation;
    double width, 
           height;
    Image* image;
    Image3D() : image(0) { type = IMAGE; }
    Image3D(const Image3D& other) : image(0) { *this = other; }
    ~Image3D() {if(image) delete image;}
    const Image3D& operator=(const Image3D& other)
    {
      if(image) delete image;
      (Element&) *this = other;
      point = other.point;
      rotation = other.rotation;
      width = other.width;
      height = other.height;
      image = other.image;
      // dirty hack: works only for the current use of this class
      const_cast<Image3D&>(other).image = 0;
      return *this;
    }
  };
  
  /**
  * Adds a line to the debug drawing.
  * @param xStart Specifies the x-coordinate of the startpoint for the line. 
  * @param yStart Specifies the y-coordinate of the startpoint for the line.
  * @param zStart Specifies the z-coordinate of the startpoint for the line.
  * @param xEnd Specifies the x-coordinate of the endpoint for the line.
  * @param yEnd Specifies the y-coordinate of the endpoint for the line.
  * @param zEnd Specifies the z-coordinate of the endpoint for the line.
  * @param width Specifies the width of the line.
  * @param color Specifies the color of the line.
  */
  void line(
    double xStart, 
    double yStart, 
    double zStart,
    double xEnd,
    double yEnd,
    double zEnd,
    float width,
    ColorRGBA color
   );

  /**
  * Adds a line to the debug drawing. The line is a solid black line with width 1.
  * @param xStart Specifies the x-coordinate of the startpoint for the line. 
  * @param yStart Specifies the y-coordinate of the startpoint for the line.
  * @param zStart Specified the z-coordinate of the startpoint fot the line.
  * @param xEnd Specifies the x-coordinate of the endpoint for the line.
  * @param yEnd Specifies the y-coordinate of the endpoint for the line.
  * @param zEnd Specifies the z-coordinate of the endpoint for the line.
	*/
  void line(double xStart, double yStart, double zStart, double xEnd, double yEnd, double zEnd);
  void line(Vector3<double> *points, float width, ColorRGBA color);

  /** 
   * Adds a quad to the debug drawing.
   * @param points Points to an array of points that specifies the vertices of the quad.
   * @param width Specifies the width of the border.
   * @param color Specifies the color of the polygon.
   */
  void quad(
    const Vector3<double>* points,
    float width,
    ColorRGBA color
    );

  /**
  * Adds a polygon to the debug drawing.
  * @param points Points to an array of points that specifies the vertices of the polygon. Each point in the array is a Point.
  * @param width Specifies the width of the border.
  * @param color Specifies the color of the polygon.
  */
  void polygon(
    const Vector3<double>* points,
    float width,
    ColorRGBA color
    );
    
  /**
  * Adds a filled square to the debug drawing. The border of the square is a solid line with width 0.
  * The square is a 3x3 square.
  * @param v Position of the dot
  * @param w The size of the dot
  * @param color The color of the dot.
  */  
  void dot(
    Vector3<double> v, float w, ColorRGBA color
    );  
  
  /**
  * Adds a sphere to the debug drawing.
  * @param v Position of the sphere
  * @param r Radius of the sphere
  * @param color The color of the sphere.
  */  
  void sphere(
    Vector3<double> v, double radius, ColorRGBA color
    );  
  
  /**
  * Adds a cylinder to the debug drawing.
  * @param v Position of the cylinder
  * @param rot Rotation of the cylinder around x/y/z axes
  * @param r Radius of the cylinder
  * @param h Height of the cylinder
  * @param color The color of the cylinder.
  */  
  void cylinder(
    Vector3<double> v, Vector3<double> rot, double r, double h, ColorRGBA color
    );  

  /**
  * Adds an image to the debug drawing.
  * @param v Position of the image
  * @param rot Rotation of the image around x/y/z axes
  * @param w Width of the image
  * @param h Height of the height
  * @param i Pointer to the image. The image will automatically be freed.
  */  
  void image(
    Vector3<double> v, Vector3<double> rot, double w, double h, Image* i
    );  

  bool addShapeFromQueue(InMessage& message, Drawings3D::ShapeType shapeType, char identifier);
  
  bool drawn; /**< is this drawing already registered? */
  char processIdentifier; /**< To which process does this drawing belong? */
  RobotConsole* robotConsole;
  unsigned timeStamp; /**< The time when this drawing was created. */

private:
  double scaleX,  scaleY,  scaleZ;
  double rotateX, rotateY, rotateZ;
  double transX,  transY,  transZ;

  std::vector<Line>     lines;
  std::vector<Dot>      dots;
  std::vector<Polygon>  polygons;
  std::vector<Quad>     quads;
  std::vector<Sphere>   spheres;
  std::vector<Cylinder> cylinders;
  std::vector<Image3D>  images;

  char* copyImage(const Image& srcImage, int& width, int& height) const;
};

#endif //DebugDrawing3D_h
