/**
* @file Tools/Math/Geometry.h
* Declares class Geometry
*
* @author <A href=mailto:juengel@informatik.hu-berlin.de>Matthias Jüngel</A>
* @author <a href="mailto:walter.nistico@uni-dortmund.de">Walter Nistico</a>
*/

#ifndef __Geometry_h__
#define __Geometry_h__

#include "Tools/Math/Pose2D.h"
#include "Representations/Perception/CameraMatrix.h"
#include "Representations/Infrastructure/CameraInfo.h"

/**
* The class Geometry defines representations for geometric objects and Methods
* for calculations with such object.
*
*/
class Geometry
{
public:
  
  /** Defines a circle by its center and its radius*/
  struct Circle
  {
    Circle():radius(0) {}
    Circle(const Vector2<double>& c, double r)
    {
      center = c;
      radius = r;
    }
    Vector2<double> center;
    double radius;
  };
  
  /** Defines a line by two vectors*/
  struct Line
  {
    Vector2<double> base;
    Vector2<double> direction;
    
    Line() {};
    Line(const Vector2<double>& base, const Vector2<double>& direction) : 
    base(base), 
      direction(direction)
    {};
    
    Line(const Vector2<int>& base, const Vector2<double>& direction) : 
    direction(direction)
    {
      this->base.x = base.x;
      this->base.y = base.y;
    };

    Line(const Vector2<int>& base, const Vector2<int>& direction)
    {
      this->base.x = base.x;
      this->base.y = base.y;
      this->direction.x = direction.x;
      this->direction.y = direction.y;
    };

    Line(const Pose2D& base, double length = 1.0)
    {
      this->base = base.translation;
      this->direction = (Pose2D(base.rotation) + Pose2D(Vector2<double>(length,0))).translation;
    }

    Line(double baseX, double baseY, double directionX, double directionY) 
    {
      base.x = baseX; 
      base.y = baseY; 
      direction.x = directionX;
      direction.y = directionY;
    };
    
    void normalizeDirection();
  };
  
  struct PixeledLine
  {
    PixeledLine(int x1, int x2, int y1, int y2):x1(x1),y1(y1),x2(x2),y2(y2) 
    {
      calculatePixels();
    };

    PixeledLine(const Vector2<int>& start, const Vector2<int>& end):x1(start.x),y1(start.y),x2(end.x),y2(end.y)
    {
      calculatePixels();
    };

    void calculatePixels()
    {
      char sign;
      if(x1 == x2 && y1 == y2)
      {
        numberOfPixels = 0;
      }
      else //begin and end differ
      {
        if(abs(x2 - x1) > abs(y2 - y1) )
        {
          if(x1 < x2) sign = 1; else sign = -1;
          numberOfPixels = abs(x2 - x1) + 1;
          for(int x = 0; x < numberOfPixels; x++)
          {
            int y = (int)(x * (y2 - y1) / (x2 - x1));
            x_coordinate[x] = x1 + x*sign;
            y_coordinate[x] = y1 + y*sign;
          }
        }
        else
        {
          if(y1 < y2) sign = 1; else sign = -1;
          numberOfPixels = abs(y2 - y1) + 1;
          for(int y = 0; y < numberOfPixels; y++)
          {
            int x = (int)(y * (x2 - x1) / (y2 - y1));
            x_coordinate[y] = x1 + x*sign;
            y_coordinate[y] = y1 + y*sign;
          }
        }
      } //begin and end differ
    }// calculatePixels
    
    inline int getNumberOfPixels() const
    {
      return numberOfPixels;
    }

    inline int getPixelX(int i) const
    {
      return x_coordinate[i];
    }

    inline int getPixelY(int i) const
    {
      return y_coordinate[i];
    }

  private:
    int x1, y1, x2, y2;
    int numberOfPixels;
	enum{maxNumberOfPixelsInLine = 600}; //diagonal size of BW image 
    int x_coordinate[maxNumberOfPixelsInLine];
    int y_coordinate[maxNumberOfPixelsInLine];
  };

  template <class V, int maxNumberOfPoints> class SetOfPoints
  {
  public:
    /** Constructor */
    SetOfPoints() {init();}
    void add(V x, V y)
    {
      if(numberOfPoints >= maxNumberOfPoints) return;
      points[numberOfPoints].x = x;
      points[numberOfPoints].y = y;
      numberOfPoints++;
    }
    void add(const Vector2<V>& newPoint)
    {
      if(numberOfPoints >= maxNumberOfPoints) return;
      points[numberOfPoints] = newPoint;
      numberOfPoints++;
    }

    void init() {numberOfPoints = 0;};
    
    double linearRegression(Line& line)
    {
      if(numberOfPoints == 0) return 0;
      double col1[maxNumberOfPoints], col2[maxNumberOfPoints], col3[maxNumberOfPoints], col4[maxNumberOfPoints], col5[maxNumberOfPoints];
      double sumX, sumY, averageX, averageY, c3sum, c4sum, c5sum, m, n;
      
      sumX=0; sumY=0; c3sum=0; c4sum=0; c5sum=0;

      int i;
      for (i = 0; i < numberOfPoints; i++)
      {
        sumX = sumX + points[i].x;
        sumY = sumY + points[i].y;
      }
      
      averageX = sumX / numberOfPoints;
      averageY = sumY / numberOfPoints;
      
      for (i = 0; i < numberOfPoints; i++)
      {
        col1[i] = points[i].x - averageX;
        col2[i] = points[i].y - averageY;
        col3[i] = col1[i] * col2[i];
        col4[i] = col1[i] * col1[i];
        col5[i] = col2[i] * col2[i];
      }
      
      for (i = 0; i < numberOfPoints; i++)
      {
        c3sum = c3sum + col3[i];
        c4sum = c4sum + col4[i];
        c5sum = c5sum + col5[i];
      }
      
      m = c3sum / c4sum;
      n = averageY - m * averageX;
      
      line.base.x = 0;
      line.base.y = n;
      line.direction.x = 1;
      line.direction.y = m;

      // reliability:
      return fabs(c3sum / sqrt(c4sum * c5sum));
    }
  private:
    Vector2<V> points[maxNumberOfPoints];
    int numberOfPoints;
  };

  /** 
  * Calculates the angle between a pose and a position
  * @param from The base pose.
  * @param to The other position.
  * @return the angle from the pose to the position.
  */
  static double angleTo(const Pose2D& from, 
    const Vector2<double>& to);

  /** 
  * Calculates the distance from a pose to a position
  * @param from The base pose.
  * @param to The other position.
  * @return the distance from the pose to the position.
  */
  static double distanceTo(const Pose2D& from, 
    const Vector2<double>& to);

  /** 
  * Calculates the relative vector from a pose to a position
  * @param from The base pose.
  * @param to The other position.
  * @return the vector from the pose to the position.
  */
  static Vector2<double> vectorTo(const Pose2D& from, 
    const Vector2<double>& to);

  /**
  * Rotates a vector 
  * @param v The vector
  * @param a The angle
  * @return The rotated vector
  */
  static Vector2<double> rotate(const Vector2<double>& v, double a);

  /**
  * Rotates a vector 
  * @param v The vector
  * @param a The angle
  * @return The rotated vector
  */
  static Vector2<int> rotate(const Vector2<int>& v, double a);

  /**
  * Returns the circle defined by the three points.
  * @param point1 The first point.
  * @param point2 The second point.
  * @param point3 The third point.
  * @return The circle defined by point1, point2 and point3.
  */
  static Circle getCircle(
    const Vector2<int>& point1,
    const Vector2<int>& point2,
    const Vector2<int>& point3
    );

  static int getIntersectionOfCircles(
    const Circle &c1,
    const Circle &c2,
    Vector2<double> &p1,
    Vector2<double> &p2
    );
  
  static bool checkIntersectionOfLines(
    const Vector2<double>& l1p1,
    const Vector2<double>& l1p2,
    const Vector2<double>& l2p1,
    const Vector2<double>& l2p2
    );

  static bool getIntersectionOfLines(
    const Line& line1,
    const Line& line2,
    Vector2<double>& intersection
    );

  static bool getIntersectionOfLines(
    const Line& line1,
    const Line& line2,
    Vector2<int>& intersection
    );

  static bool getIntersectionOfRaysFactor(
    const Line& ray1,
    const Line& ray2,
    double& intersection
    );
  
  static double getDistanceToLine(
    const Line& line,
    const Vector2<double>& point
    );
  
  static double getDistanceToEdge(
    const Line& line,
    const Vector2<double>& point
    );

  static double distance(
    const Vector2<double>& point1,
    const Vector2<double>& point2
    );

  static double distance(
    const Vector2<int>& point1,
    const Vector2<int>& point2
    );

private:
  static int ccw(
    const Vector2<double>& p0, 
    const Vector2<double>& p1, 
    const Vector2<double>& p2);

public:
  static void calculateAnglesForPoint(
    const Vector2<int>& point, 
    const CameraMatrix& cameraMatrix, 
    const CameraInfo& cameraInfo, 
    Vector2<double>& angles
    );
    
  static bool calculatePointByAngles(
    const Vector2<double>& angles,
    const CameraMatrix& cameraMatrix, 
    const CameraInfo& cameraInfo, 
    Vector2<int>& point
    );

  static bool clipLineWithQuadrangle(
    const Line& lineToClip,
    const Vector2<double>& corner0, 
    const Vector2<double>& corner1, 
    const Vector2<double>& corner2, 
    const Vector2<double>& corner3, 
    Vector2<double>& clipPoint1, 
    Vector2<double>& clipPoint2
    );

  static bool clipLineWithQuadrangle(
    const Line& lineToClip,
    const Vector2<double>& corner0, 
    const Vector2<double>& corner1, 
    const Vector2<double>& corner2, 
    const Vector2<double>& corner3, 
    Vector2<int>& clipPoint1, 
    Vector2<int>& clipPoint2
    );

  static bool isPointInsideRectangle(
    const Vector2<double>& bottomLeftCorner, 
    const Vector2<double>& topRightCorner,
    const Vector2<double>& point
    );
  
  static bool isPointInsideRectangle(
    const Vector2<int>& bottomLeftCorner, 
    const Vector2<int>& topRightCorner,
    const Vector2<int>& point
    );

  static bool isPointInsideConvexPolygon(
    const Vector2<double> polygon[],
    const int numberOfPoints,
    const Vector2<double>& point
    );
    
  static bool clipPointInsideRectangle(
    const Vector2<int>& bottomLeftCorner, 
    const Vector2<int>& topRightCorner,
    Vector2<int>& point
    );

  static bool clipPointInsideRectangle(
    const Vector2<int>& bottomLeftCorner, 
    const Vector2<int>& topRightCorner,
    Vector2<double>& point
    );

  /** 
  * Calculates where a pixel in the image lies on the ground (relative to the robot).
  * @param x Specifies the x-coordinate of the pixel.
  * @param y Specifies the y-coordinate of the pixel.
  * @param cameraMatrix The camera matrix of the image.
  * @param cameraInfo The camera info of the image.
  * @param pointOnField The resulting point.
  */
  static bool calculatePointOnField(
    const int x,
    const int y,
    const CameraMatrix& cameraMatrix,
    const CameraInfo& cameraInfo,
    Vector2<double>& pointOnField
    );
  static bool calculatePointOnFieldHacked(
    const int x,
    const int y,
    const CameraMatrix& cameraMatrix,
    const CameraInfo& cameraInfo,
    Vector2<double>& pointOnField
    );
  static bool calculatePointOnField(const Vector2<>& image, const double& fieldZ, const CameraMatrix& cameraMatrix, const CameraInfo& cameraInfo, 
               Vector3<>& field);



  /** 
  * Calculates where a pixel in the image lies on the ground (relative to the robot).
  * @param x Specifies the x-coordinate of the pixel.
  * @param y Specifies the y-coordinate of the pixel.
  * @param cameraMatrix The camera matrix of the image.
  * @param cameraInfo The camera info of the image.
  * @param pointOnField The resulting point.
  */
  static bool calculatePointOnField(
    const int x,
    const int y,
    const CameraMatrix& cameraMatrix,
    const CameraInfo& cameraInfo,
    Vector2<int>& pointOnField
    )
  { 
    Vector2<double> pointOnFieldDouble;
    bool onField = calculatePointOnField(x,y,cameraMatrix,cameraInfo,pointOnFieldDouble);
    pointOnField.x = (int)pointOnFieldDouble.x;
    pointOnField.y = (int)pointOnFieldDouble.y;
    return onField;
  }

  /** 
  * Calculates where a pixel in the image lies on a horizontal plane (relative to the robot).
  * @param pointInImage Specifies the coordinates of the pixel.
  * @param z Specifies the height of the plane above the ground.
  * @param cameraMatrix The camera matrix of the image.
  * @param cameraInfo The camera info of the image.
  * @param pointOnPlane The resulting point.
  */
  static bool calculatePointOnHorizontalPlane(
    const Vector2<int>& pointInImage, 
    double z,
    const CameraMatrix& cameraMatrix, 
    const CameraInfo& cameraInfo,
    Vector2<double>& pointOnPlane);

  /** 
  * Calculates where a relative point on the ground appears in an image.
  * @param point The coordinates of the point relative to the robot's origin.
  * @param cameraMatrix The camera matrix of the image.
  * @param cameraInfo The camera info of the image.
  * @param pointInImage The resulting point.
  * @return The result is valid, i.e. the point is in front of the camera. That
  *         still does not mean that the point is within the bounds of the image.
  */
  static bool calculatePointInImage( 
    const Vector2<int>& point,
    const CameraMatrix& cameraMatrix,
    const CameraInfo& cameraInfo,
    Vector2<int>& pointInImage);

  /** 
  * Calculates where a relative 3-D point appears in an image.
  * @param pointInWorld The coordinates of the point relative to the robot's origin.
  * @param cameraMatrix The camera matrix of the image.
  * @param cameraInfo The camera info of the image.
  * @param pointInImage The resulting point.
  * @return The result is valid, i.e. the point is in front of the camera. That
  *         still does not mean that the point is within the bounds of the image.
  */
  static bool calculatePointInImage(
    const Vector3<double>& pointInWorld, 
    const CameraMatrix& cameraMatrix, 
    const CameraInfo& cameraInfo,
    Vector2<int>& pointInImage);

  /**
  * Clips a line with a rectangle
  * @param bottomLeft The bottom left corner of the rectangle
  * @param topRight The top right corner of the rectangle
  * @param line The line to be clipped
  * @param point1 The starting point of the resulting line
  * @param point2 The end point of the resulting line
  * @return states whether clipping was necessary (and done)
  */
  static bool getIntersectionPointsOfLineAndRectangle(
   const Vector2<int>& bottomLeft, 
   const Vector2<int>& topRight,
   const Geometry::Line line,
   Vector2<int>& point1, 
   Vector2<int>& point2
   );


  /**
  * Clips a line with the Cohen-Sutherland-Algorithm
  * @param bottomLeft The bottom left corner of the rectangle
  * @param topRight The top right corner of the rectangle
  * @param point1 The starting point of the line
  * @param point2 The end point of the line
  * @return states whether clipping was necessary (and done)
  */
  static bool clipLineWithRectangleCohenSutherland(
   const Vector2<int>& bottomLeft, 
   const Vector2<int>& topRight,
   Vector2<int>& point1, 
   Vector2<int>& point2
   );


  /**
  * Calculates the Cohen Sutherland Code for a given point and a given rectangle
  */
  static int cohenSutherlandOutCode(
    const Vector2<int>& bottomLeft, 
    const Vector2<int>& topRight,
    Vector2<int>& point
    )
  { 
    int code;
    if ( point.x < bottomLeft.x )
      code = 1 ;
    else
      code = 0 ;
    if ( point.x > topRight.x )
      code += 2 ;
    if ( point.y < bottomLeft.y )
      code += 4 ;
    if ( point.y > topRight.y )
      code += 8 ;
    return(code) ;
  }

  /**
  * Calculates the intersection of an arbitrary line and a horizontal or vertical line.
  */
  static int intersection(int a1,int b1,int a2,int b2,int value);

  /**
  * Function does the transformation from 2d relative robot coordinates
  * to absolute field coordinates.
  * @param rp current Robot Pose.
  * @param x relative x-coordinate of ball (relative to robot)
  * @param y relative y-coordinate of ball (relative to robot)
  * @return Returns the ball positon in absolute coordinates
  */
  static Vector2<double> relative2FieldCoord(const Pose2D& rp, double x, double y);

  /**
  * Function does the transformation from 2d relative robot coordinates
  * to absolute field coordinates.
  * @param rp current Robot Pose.
  * @param relPosOnField relative position on the field (relative to robot)
  * @return Returns the ball positon in absolute coordinates
  */
  static Vector2<double> relative2FieldCoord(const Pose2D& rp, Vector2<> relPosOnField);


  /**
  * Function does the transformation from 2d field coordinates
  * to coordinates relative to the robot.
  * @param robotPose current Robot Pose.
  * @param fieldCoord
  * @return Returns the positon in relative
  */
  static Vector2<double> fieldCoord2Relative(const Pose2D& robotPose, const Vector2<double>& fieldCoord);

  /**
  * The function approximates the shape of a ball in the camera image.
  * Note: currently, the approximation is not exact.
  * @param ballOffset The ball's position relative to the robot's body origin.
  * @param cameraMatrix The position and orientation of the robot's camera.
  * @param cameraInfo The resolution and the opening angle of the robot's camera.
  * @param ballRadius The radius of the ball in mm.
  * @param circle The approximated shape generated by the function.
  * @return If false, only the center of the circle is valid, not the radius.
  */
  static bool calculateBallInImage(const Vector2<double>& ballOffset,
                                   const CameraMatrix& cameraMatrix, 
                                   const CameraInfo& cameraInfo, 
                                   double ballRadius,
                                   Circle& circle);

  /** 
  * The function determines how far an object is away depending on its real size and the size in the image.
  * @param cameraInfo Information about the camera (opening angles, resolution, etc.).
  * @param sizeInReality The real size of the object.
  * @param sizeInPixels The size in the image.
  * @return The distance between camera and object.
  */
  static double getDistanceBySize
    (
     const CameraInfo& cameraInfo,
     double sizeInReality,
     double sizeInPixels
     );

  /** 
  * The function determines how far an object is away depending on its real size and the size in the image
  * along with its center position, using camera intrinsic parameters.
  * @param cameraInfo Class containing the intrinsic paramaters
  * @param sizeInReality The real size of the object.
  * @param sizeInPixels The size in the image.
  * @param centerX X coordinate (in image reference) of object's baricenter. 
  * @param centerY Y coordinate (in image reference) of object's baricenter. 
  * @return The distance between camera and object.
  */
  static double getDistanceBySize(
    const CameraInfo& cameraInfo,
    double sizeInReality, 
    double sizeInPixels,
    double centerX,
    double centerY
    );
  
    /** 
  * The function determines how far an object is away depending on its real size and the size in the image.
  * @param sizeInReality The real size of the object.
  * @param sizeInPixels The size in the image.
  * @return The distance between camera and object.
  */
  static double getDistanceByAngleSize(
    double sizeInReality, 
    double sizeInPixels
    );

    /** 
  * The function determines how far the ball is away depending on its real size and the size in the image.
  * @param sizeInReality The real size of the ball.
  * @param sizeInPixels The size in the image.
  * @return The distance between the camera and the ball.
  */
  static double getBallDistanceByAngleSize(
    double sizeInReality, 
    double sizeInPixels
    );

  /** 
  * The function determines how big an object appears in the image depending on its distance and size.
  * @param sizeInReality The real size of the object.
  * @param distance The distance to the object.
  * @param imageWidthPixels The horizontal resolution of the image.
  * @param imageWidthAngle The horizontal opening angle of the camera.
  * @return The size as it would appear in the image.
  */
  static double getSizeByDistance(
    double sizeInReality,
    double distance,
    double imageWidthPixels,
    double imageWidthAngle
    );
    
  /** 
  * The function determines how big an object appears in the image depending on its distance and size.
  * @param cameraInfo Object containing camera paramters.
  * @param sizeInReality The real size of the object.
  * @param distance The distance to the object.
  * @return The size as it would appear in the image.
  */
  static double getSizeByDistance
    (
     const CameraInfo& cameraInfo,
     double sizeInReality,
     double distance
    );

  /** 
  * The function calculates the horizon.
  * @param cameraMatrix The camera matrix.
  * @param cameraInfo Object containing camera parameters.
  * @return The line of the horizon in the image.
  */
  static Geometry::Line calculateHorizon(
    const CameraMatrix& cameraMatrix,
    const CameraInfo& cameraInfo
    );

  /** 
  * The function calculates the expected size (pixel) of a field line in an image.
  * @param pointInImage The point where the line is expected.
  * @param cameraMatrix The camera matrix.
  * @param cameraInfo Object containing the camera parameters.
  * @param fieldLinesWidth The width of field lines in mm.
  * @return The size of a line pixel.
  */
  static int calculateLineSize(
    const Vector2<int>& pointInImage,
    const CameraMatrix& cameraMatrix,
    const CameraInfo& cameraInfo,
    double fieldLinesWidth
    );

  /** 
  * Calculates the angle size for a given pixel size.
  */
  static double pixelSizeToAngleSize(double pixelSize, const CameraInfo& cameraInfo);

  /** 
  * Calculates the pixel size for a given angle size.
  */
  static double angleSizeToPixelSize(double angleSize, const CameraInfo& cameraInfo);
};

#endif //__Geometry_h____
