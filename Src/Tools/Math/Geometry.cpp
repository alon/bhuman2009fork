/**
* @file Math/Geometry.cpp
* Implemets class Geometry
*
* @author <A href=mailto:juengel@informatik.hu-berlin.de>Matthias Jüngel</A>
* @author <a href="mailto:walter.nistico@uni-dortmund.de">Walter Nistico</a>
*/

#include "Geometry.h"

double Geometry::angleTo(const Pose2D& from, 
                                const Vector2<double>& to)
{
  Pose2D relPos = Pose2D(to) - from;
  return atan2(relPos.translation.y,relPos.translation.x);
}

double Geometry::distanceTo(const Pose2D& from, 
                                   const Vector2<double>& to)
{
  return (Pose2D(to) - from).translation.abs();
}

Vector2<double> Geometry::vectorTo(const Pose2D& from, const Vector2<double>& to)
{
  return (Pose2D(to) - from).translation;
}

Vector2<double> Geometry::rotate(const Vector2<double>& v, double a)
{
  Vector2<double> rV;
  double cos_a(cos(a));
  double sin_a(sin(a));
  rV.x = cos_a*v.x - sin_a*v.y;
  rV.y = sin_a*v.x + cos_a*v.y;
  return rV;
}

Vector2<int> Geometry::rotate(const Vector2<int>& v, double a)
{
  Vector2<int> rV;
  double cos_a(cos(a));
  double sin_a(sin(a));
  rV.x = static_cast<int>(cos_a*v.x - sin_a*v.y);
  rV.y = static_cast<int>(sin_a*v.x + cos_a*v.y);
  return rV;
}

void Geometry::Line::normalizeDirection()
{
  double distance = sqrt(sqr(direction.x) + sqr(direction.y));
  direction.x = direction.x / distance;
  direction.y = direction.y / distance;
}

Geometry::Circle Geometry::getCircle
(
 const Vector2<int>& point1,
 const Vector2<int>& point2,
 const Vector2<int>& point3
 )
{
  double x1 = point1.x;
  double y1 = point1.y;
  double x2 = point2.x;
  double y2 = point2.y;
  double x3 = point3.x;
  double y3 = point3.y;
  
  Circle circle;
  
  if((x2*y1 - x3*y1 - x1*y2 + x3*y2 + x1*y3 - x2*y3 == 0) )
  {
    circle.radius = 0;
  }
  else
  {
    circle.radius =
      0.5 * 
      sqrt(
      ( (sqr(x1 - x2) + sqr(y1 - y2) ) *
      (sqr(x1 - x3) + sqr(y1 - y3) ) *
      (sqr(x2 - x3) + sqr(y2 - y3) )
      )
      /
      sqr(x2*y1 - x3*y1 - x1*y2 + x3*y2 + x1*y3 - x2*y3)
      );
  }
  
  if( (2 * (-x2*y1 + x3*y1 + x1*y2 - x3*y2 - x1*y3 + x2*y3) == 0) )
  {
    circle.center.x = 0;
  }
  else
  {
    circle.center.x =
      (
      sqr(x3) * (y1 - y2) + 
      (sqr(x1) + (y1 - y2) * (y1 - y3)) * (y2 - y3) +
      sqr(x2) * (-y1 + y3)
      )
      /
      (2 * (-x2*y1 + x3*y1 + x1*y2 - x3*y2 - x1*y3 + x2*y3) );
  }   
  
  if((2 * (x2*y1 - x3*y1 - x1*y2 + x3*y2 + x1*y3 - x2*y3) == 0) )
  {
    circle.center.y = 0;
  }
  else
  {
    circle.center.y =
      (
      sqr(x1) * (x2 - x3) + 
      sqr(x2) * x3 + 
      x3*(-sqr(y1) + sqr(y2) ) - 
      x2*(+sqr(x3) - sqr(y1) + sqr(y3) ) + 
      x1*(-sqr(x2) + sqr(x3) - sqr(y2) + sqr(y3) )
      )
      /
      (2 * (x2*y1 - x3*y1 - x1*y2 + x3*y2 + x1*y3 - x2*y3) );
  } 
  return circle;
}

bool Geometry::getIntersectionOfLines
(
 const Line& line1,
 const Line& line2,
 Vector2<int>& intersection
 )
{
 Vector2<double> intersectionDouble;
 bool toReturn = getIntersectionOfLines(line1,line2,intersectionDouble);
 intersection.x = (int)intersectionDouble.x;
 intersection.y = (int)intersectionDouble.y;
 return toReturn;
}

bool Geometry::getIntersectionOfLines
(
 const Line& line1,
 const Line& line2,
 Vector2<double>& intersection
 )
{
  if(line1.direction.y * line2.direction.x == line1.direction.x * line2.direction.y)
  {
    return false;
  }
  
  intersection.x = 
    line1.base.x +
    line1.direction.x * 
    (
    line1.base.y * line2.direction.x - 
    line2.base.y * line2.direction.x + 
    (-line1.base.x + line2.base.x) * line2.direction.y
    )
    /
    ( (-line1.direction.y) * line2.direction.x + line1.direction.x * line2.direction.y );
  
  intersection.y = 
    line1.base.y + 
    line1.direction.y *
    (
    -line1.base.y * line2.direction.x + 
    line2.base.y * line2.direction.x + 
    (line1.base.x - line2.base.x) * line2.direction.y
    )
    /
    (line1.direction.y * line2.direction.x - line1.direction.x * line2.direction.y);
  
  return true;
}


int Geometry::getIntersectionOfCircles(
  const Circle &c0,
  const Circle &c1,
  Vector2<double> &p1,
  Vector2<double> &p2
  )
{
  double a, dx, dy, d, h, rx, ry;
  double x2, y2;

  /* dx and dy are the vertical and horizontal distances between
   * the circle centers.
   */
  dx = c1.center.x - c0.center.x;
  dy = c1.center.y - c0.center.y;

  /* Determine the straight-line distance between the centers. */
  d = sqrt((dy*dy) + (dx*dx));

  /* Check for solvability. */
  if (d > (c0.radius + c1.radius))
  {
    /* no solution. circles do not intersect. */
    return 0;
  }
  if (d < fabs(c0.radius - c1.radius))
  {
    /* no solution. one circle is contained in the other */
    return 0;
  }

  /* 'point 2' is the point where the line through the circle
   * intersection points crosses the line between the circle
   * centers.  
   */

  /* Determine the distance from point 0 to point 2. */
  a = ((c0.radius * c0.radius) - (c1.radius * c1.radius) + (d * d)) / (2.0 * d) ;

  /* Determine the coordinates of point 2. */
  x2 = c0.center.x + (dx * a/d);
  y2 = c0.center.y + (dy * a/d);

  /* Determine the distance from point 2 to either of the
   * intersection points.
   */
  h = sqrt((c0.radius * c0.radius) - (a*a));

  /* Now determine the offsets of the intersection points from
   * point 2.
   */
  rx = -dy * (h/d);
  ry =  dx * (h/d);

  /* Determine the absolute intersection points. */
  p1.x = x2 + rx;
  p2.x = x2 - rx;
  p1.y = y2 + ry;
  p2.y = y2 - ry;

  return 1;
}


bool Geometry::getIntersectionOfRaysFactor
(
 const Line& ray1,
 const Line& ray2,
 double& factor
 )
{
  double divisor = ray2.direction.x * ray1.direction.y - ray1.direction.x * ray2.direction.y;
  if(divisor==0)
  {
    return false;
  }
  double k=(ray2.direction.y*ray1.base.x - ray2.direction.y*ray2.base.x - ray2.direction.x*ray1.base.y + ray2.direction.x*ray2.base.y)/divisor;
  double l=(ray1.direction.y*ray1.base.x - ray1.direction.y*ray2.base.x - ray1.direction.x*ray1.base.y + ray1.direction.x*ray2.base.y)/divisor;
  if ((k>=0)&&(l>=0)&&(k<=1)&&(l<=1))
  {
    factor=k;
    return true;
  }
  return false;
}

double Geometry::getDistanceToLine
(
 const Line& line,
 const Vector2<double>& point
 )
{
  if (line.direction.x == 0 && line.direction.y == 0) 
    return distance(point, line.base);

  Vector2<double> normal;
  normal.x = line.direction.y;
  normal.y = -line.direction.x;
  normal.normalize();

  double c = normal * line.base;

  return normal * point - c;
}

double Geometry::getDistanceToEdge
(
 const Line& line,
 const Vector2<double>& point
 )
{
  if (line.direction.x == 0 && line.direction.y == 0) 
    return distance(point, line.base);

  double c = line.direction * line.base;

  double d = (line.direction * point - c) / (line.direction * line.direction);

  if (d < 0) 
    return distance(point, line.base);
  else if (d > 1.0)
    return distance(point, line.base + line.direction);
  else
    return fabs(getDistanceToLine(line, point));
}


double Geometry::distance
(
 const Vector2<double>& point1,
 const Vector2<double>& point2
 )
{
  return (point2 - point1).abs();
}

double Geometry::distance
(
 const Vector2<int>& point1,
 const Vector2<int>& point2
 )
{
  return (point2 - point1).abs();
}

void Geometry::calculateAnglesForPoint
(
 const Vector2<int>& point, 
 const CameraMatrix& cameraMatrix, 
 const CameraInfo& cameraInfo, 
 Vector2<double>& angles
 )
{
  double factor = cameraInfo.focalLength;
  
  Vector3<double> vectorToPoint(
    factor,
    cameraInfo.opticalCenter.x - point.x,
    cameraInfo.opticalCenter.y - point.y);
  
  Vector3<double> vectorToPointWorld = 
    cameraMatrix.rotation * vectorToPoint;
  
  angles.x =
    atan2(vectorToPointWorld.y,vectorToPointWorld.x);
  
  angles.y =
    atan2(
    vectorToPointWorld.z,
    sqrt(sqr(vectorToPointWorld.x) + sqr(vectorToPointWorld.y)) 
    );
}

bool Geometry::calculatePointByAngles
(
 const Vector2<double>& angles,
 const CameraMatrix& cameraMatrix, 
 const CameraInfo& cameraInfo, 
 Vector2<int>& point
 )
{
  Vector3<double> vectorToPointWorld, vectorToPoint;
  vectorToPointWorld.x = cos(angles.x);
  vectorToPointWorld.y = sin(angles.x);
  vectorToPointWorld.z = tan(angles.y);
  
  RotationMatrix rotationMatrix = cameraMatrix.rotation;
  vectorToPoint = rotationMatrix.invert() * vectorToPointWorld;
  
  double factor = cameraInfo.focalLength;
  
  double scale = factor / vectorToPoint.x;
  
  point.x = (int)(0.5 + cameraInfo.opticalCenter.x - vectorToPoint.y * scale);
  point.y = (int)(0.5 + cameraInfo.opticalCenter.y  - vectorToPoint.z * scale);
  return vectorToPoint.x > 0;
}

bool Geometry::clipLineWithQuadrangle
(
 const Line& lineToClip,
 const Vector2<double>& corner0, 
 const Vector2<double>& corner1,
 const Vector2<double>& corner2, 
 const Vector2<double>& corner3,
 Vector2<int>& clipPoint1, 
 Vector2<int>& clipPoint2
 )
{
  Vector2<double> point1, point2;
  bool toReturn = clipLineWithQuadrangle(
   lineToClip,
   corner0, corner1, corner2, corner3,
   point1, point2);
   clipPoint1.x = (int)point1.x;
   clipPoint1.y = (int)point1.y;
   clipPoint2.x = (int)point2.x;
   clipPoint2.y = (int)point2.y;
   return toReturn;
}

bool Geometry::clipLineWithQuadrangle
(
 const Line& lineToClip,
 const Vector2<double>& corner0, 
 const Vector2<double>& corner1,
 const Vector2<double>& corner2, 
 const Vector2<double>& corner3,
 Vector2<double>& clipPoint1, 
 Vector2<double>& clipPoint2
 )
{
  Geometry::Line side[4] , verticalLine;

  verticalLine.base = lineToClip.base;

  verticalLine.direction.x = -lineToClip.direction.y;
  verticalLine.direction.y = lineToClip.direction.x;
  
  Vector2<double> corner[4];
  corner[0] = corner0;
  corner[1] = corner1;
  corner[2] = corner2;
  corner[3] = corner3;

  side[0].base = corner0;
  side[0].direction = corner1;
  
  side[1].base = corner1;
  side[1].direction = corner3;
  
  side[2].base = corner2;
  side[2].direction = corner1;
  
  side[3].base = corner3;
  side[3].direction = corner3;
  
  Vector2<double> point1, point2, point;
  bool nextIsPoint1 = true;
  /*
  for(int i = 0; i < 4; i++)
  {
    if(Geometry::getIntersectionOfLines(side[i], lineToClip, point))
    {
      if(nextIsPoint1 && sign(point.x - corner[i]) != sign(point.x - corner[(i+1)%4])
        )
      {
        point1 = point;
        nextIsPoint1 = false;
      }
      else
        point2 = point;
    };
  }*/

  if(Geometry::getIntersectionOfLines(side[0], lineToClip, point))
  {
    if(corner[0].x < point.x && point.x < corner[1].x)
    {
      if(nextIsPoint1)
      {
        point1 = point;
        nextIsPoint1 = false;
      }
    }
  }

  if(Geometry::getIntersectionOfLines(side[1], lineToClip, point))
  {
    if(corner[1].y < point.y && point.y < corner[2].y)
    {
      if(nextIsPoint1)
      {
        point1 = point;
        nextIsPoint1 = false;
      }
      else
      point2 = point;
    }
  }

  if(Geometry::getIntersectionOfLines(side[2], lineToClip, point))
  {
    if(corner[2].x > point.x && point.x > corner[3].x)
    {
      if(nextIsPoint1)
      {
        point1 = point;
        nextIsPoint1 = false;
      }
      else
      point2 = point;
    }
  }

  if(Geometry::getIntersectionOfLines(side[3], lineToClip, point))
  {
    if(corner[3].y > point.y && point.y > corner[0].y)
    {
      if(nextIsPoint1)
      {
        point1 = point;
        nextIsPoint1 = false;
      }
      else
      point2 = point;
    }
  }

  if(nextIsPoint1)
    return false;

  if(getDistanceToLine(verticalLine, point1) < getDistanceToLine(verticalLine, point2) )
  {
    clipPoint1 = point1;
    clipPoint2 = point2;
  }
  else
  {
    clipPoint1 = point2;
    clipPoint2 = point1;
  }
  return true;
}

bool Geometry::isPointInsideRectangle
(
 const Vector2<double>& bottomLeftCorner, 
 const Vector2<double>& topRightCorner,
 const Vector2<double>& point
)
{
  return(
    bottomLeftCorner.x <= point.x && point.x <= topRightCorner.x &&
    bottomLeftCorner.y <= point.y && point.y <= topRightCorner.y
    );
}

bool Geometry::isPointInsideRectangle
(
 const Vector2<int>& bottomLeftCorner, 
 const Vector2<int>& topRightCorner,
 const Vector2<int>& point
)
{
  return(
    bottomLeftCorner.x <= point.x && point.x <= topRightCorner.x &&
    bottomLeftCorner.y <= point.y && point.y <= topRightCorner.y
    );
}

int Geometry::ccw(const Vector2<double>& p0, const Vector2<double>& p1, const Vector2<double>& p2)
{
  double dx1(p1.x - p0.x); 
  double dy1(p1.y - p0.y);
  double dx2(p2.x - p0.x);
  double dy2(p2.y - p0.y);
  if(dx1*dy2 > dy1*dx2)
    return 1;
  if(dx1*dy2 < dy1*dx2)
    return -1;
  // Now (dx1*dy2 == dy1*dx2) must be true:
  if((dx1*dx2 < 0.0) || (dy1*dy2 < 0.0))
    return -1;
  if((dx1*dx1 + dy1*dy1) >= (dx2*dx2 + dy2*dy2))
    return 0;
  return 1;
}

bool Geometry::isPointInsideConvexPolygon(
 const Vector2<double> polygon[],
 const int numberOfPoints,
 const Vector2<double>& point)
{
  int orientation(ccw(polygon[0], polygon[1], point));
  if(orientation == 0)
    return true;
  for(int i=1; i<numberOfPoints; i++)
  {
    int currentOrientation(ccw(polygon[i], polygon[(i+1) % numberOfPoints], point));
    if(currentOrientation == 0)
      return true;
    if(currentOrientation != orientation)
      return false;
  }
  return true;
}

bool Geometry::checkIntersectionOfLines(
    const Vector2<double>& l1p1,
    const Vector2<double>& l1p2,
    const Vector2<double>& l2p1,
    const Vector2<double>& l2p2)
{
  return (((ccw(l1p1, l1p2, l2p1) * ccw(l1p1,l1p2,l2p2)) <= 0)
          && ((ccw(l2p1, l2p2, l1p1) * ccw(l2p1,l2p2,l1p2)) <= 0));
}

bool Geometry::clipPointInsideRectangle(
  const Vector2<int>& bottomLeftCorner, 
  const Vector2<int>& topRightCorner,
  Vector2<int>& point
)
{
  bool clipped = false;
  if (point.x < bottomLeftCorner.x)
  {
    point.x = bottomLeftCorner.x;
    clipped = true;
  }
  if (point.x > topRightCorner.x)
  {
    point.x = topRightCorner.x;
    clipped = true;
  }
  if (point.y < bottomLeftCorner.y)
  {
    point.y = bottomLeftCorner.y;
    clipped = true;
  }
  if (point.y > topRightCorner.y)
  {
    point.y = topRightCorner.y;
    clipped = true;
  }
  return clipped;
}

bool Geometry::clipPointInsideRectangle(
  const Vector2<int>& bottomLeftCorner, 
  const Vector2<int>& topRightCorner,
  Vector2<double>& point
)
{
  bool clipped = false;
  if (point.x < bottomLeftCorner.x)
  {
    point.x = (double)bottomLeftCorner.x;
    clipped = true;
  }
  if (point.x > topRightCorner.x)
  {
    point.x = (double)topRightCorner.x;
    clipped = true;
  }
  if (point.y < bottomLeftCorner.y)
  {
    point.y = (double)bottomLeftCorner.y;
    clipped = true;
  }
  if (point.y > topRightCorner.y)
  {
    point.y = (double)topRightCorner.y;
    clipped = true;
  }
  return clipped;
}

bool Geometry::calculatePointOnFieldHacked
(
 const int x,
 const int y,
 const CameraMatrix& cameraMatrix,
 const CameraInfo& cameraInfo,
 Vector2<double>& pointOnField
 )
{
#ifdef TARGET_ROBOT // don't recalculate on real robot
  static 
#endif
  double xFactor = cameraInfo.focalLengthInv,
         yFactor = cameraInfo.focalLengthInv;

  Vector3<double> 
    vectorToCenter(1, (cameraInfo.opticalCenter.x - x) * xFactor, (cameraInfo.opticalCenter.y - y) * yFactor);
  
  Vector3<double> 
    vectorToCenterWorld = cameraMatrix.rotation * vectorToCenter;

  //Is the point above the horizon ? - return
  if(vectorToCenterWorld.z > -5 * yFactor) return false;
  
  double a1 = cameraMatrix.translation.x,
         a2 = cameraMatrix.translation.y,
         a3 = cameraMatrix.translation.z,
         b1 = vectorToCenterWorld.x,
         b2 = vectorToCenterWorld.y,
         b3 = vectorToCenterWorld.z,
         f = a3 / b3;
  
  pointOnField.x = a1 - f * b1;
  pointOnField.y = a2 - f * b2;

  //return fabs(pointOnField.x) < 10000 && fabs(pointOnField.y) < 10000;
  return true;
}

bool Geometry::calculatePointOnField
(
 const int x,
 const int y,
 const CameraMatrix& cameraMatrix,
 const CameraInfo& cameraInfo,
 Vector2<double>& pointOnField
 )
{
#ifdef TARGET_ROBOT // don't recalculate on real robot
  static 
#endif
  double xFactor = cameraInfo.focalLengthInv,
         yFactor = cameraInfo.focalLengthInv;

  Vector3<double> 
    vectorToCenter(1, (cameraInfo.opticalCenter.x - x) * xFactor, (cameraInfo.opticalCenter.y - y) * yFactor);
  
  Vector3<double> 
    vectorToCenterWorld = cameraMatrix.rotation * vectorToCenter;

  //Is the point above the horizon ? - return
  if(vectorToCenterWorld.z > -5 * yFactor) return false;
  
  double a1 = cameraMatrix.translation.x,
         a2 = cameraMatrix.translation.y,
         a3 = cameraMatrix.translation.z ,
         b1 = vectorToCenterWorld.x,
         b2 = vectorToCenterWorld.y,
         b3 = vectorToCenterWorld.z,
         f = a3 / b3;
  
  pointOnField.x = a1 - f * b1;
  pointOnField.y = a2 - f * b2;

  return fabs(pointOnField.x) < 10000 && fabs(pointOnField.y) < 10000;
}


bool Geometry::calculatePointOnField(const Vector2<>& image, const double& fieldZ, const CameraMatrix& cameraMatrix, const CameraInfo& cameraInfo, 
                                     Vector3<>& field)
{
  Vector3<> unscaledCamera(cameraInfo.focalLength, cameraInfo.opticalCenter.x - image.x, cameraInfo.opticalCenter.y - image.y);
  const Vector3<>& unscaledField(cameraMatrix.rotation * unscaledCamera);

  if(fieldZ < cameraMatrix.translation.z)
  {
    if(unscaledField.z > 0) return false;
  }
  else
  {
    if(unscaledField.z < 0) return false;
  }
  
  const double& scale((cameraMatrix.translation.z - fieldZ) / unscaledField.z); 
  field.x = cameraMatrix.translation.x - scale * unscaledField.x;
  field.y = cameraMatrix.translation.y - scale * unscaledField.y;
  field.z = fieldZ;
  return true;
}


bool Geometry::calculatePointInImage
( 
 const Vector2<int>& point,
 const CameraMatrix& cameraMatrix,
 const CameraInfo& cameraInfo,
 Vector2<int>& pointInImage
 )
{
  Vector2<double> offset(point.x - cameraMatrix.translation.x,
                         point.y - cameraMatrix.translation.y);
  return calculatePointByAngles(
    Vector2<double>(atan2(offset.y,offset.x),
                    -atan2(cameraMatrix.translation.z,offset.abs())),
    cameraMatrix, cameraInfo, 
    pointInImage
  );
}


bool Geometry::getIntersectionPointsOfLineAndRectangle(
 const Vector2<int>& bottomLeft, 
 const Vector2<int>& topRight,
 const Geometry::Line line,
 Vector2<int>& point1, 
 Vector2<int>& point2
)
{
  int foundPoints=0;
  Vector2<double> point[2];
  if (line.direction.x!=0)
  {
    double y1=line.base.y+(bottomLeft.x-line.base.x)*line.direction.y/line.direction.x;
    if ((y1>=bottomLeft.y)&&(y1<=topRight.y))
    {
      point[foundPoints].x=bottomLeft.x;
      point[foundPoints++].y=y1;
    }
    double y2=line.base.y+(topRight.x-line.base.x)*line.direction.y/line.direction.x;
    if ((y2>=bottomLeft.y)&&(y2<=topRight.y))
    {
      point[foundPoints].x=topRight.x;
      point[foundPoints++].y=y2;
    }
  }
  if (line.direction.y!=0)
  {
    double x1=line.base.x+(bottomLeft.y-line.base.y)*line.direction.x/line.direction.y;
    if ((x1>=bottomLeft.x)&&(x1<=topRight.x)&&(foundPoints<2))
    {
      point[foundPoints].x=x1;
      point[foundPoints].y=bottomLeft.y;
      if ((foundPoints==0)||((point[0]-point[1]).abs()>0.1))
      {
        foundPoints++;
      }
    }
    double x2=line.base.x+(topRight.y-line.base.y)*line.direction.x/line.direction.y;
    if ((x2>=bottomLeft.x)&&(x2<=topRight.x)&&(foundPoints<2))
    {
      point[foundPoints].x=x2;
      point[foundPoints].y=topRight.y;
      if ((foundPoints==0)||((point[0]-point[1]).abs()>0.1))
      {
        foundPoints++;
      }
    }
  }
  switch (foundPoints)
  {
  case 1:
    point1.x=(int)point[0].x;
    point2.x=point1.x;
    point1.y=(int)point[0].y;
    point2.y=point1.y;
    foundPoints++;
    return true;
  case 2:
    if ((point[1]-point[0])*line.direction >0)
    {
      point1.x=(int)point[0].x;
      point1.y=(int)point[0].y;
      point2.x=(int)point[1].x;
      point2.y=(int)point[1].y;
    }
    else
    {
      point1.x=(int)point[1].x;
      point1.y=(int)point[1].y;
      point2.x=(int)point[0].x;
      point2.y=(int)point[0].y;
    }
    return true;
  default:
    return false;
  }
}

bool Geometry::clipLineWithRectangleCohenSutherland
(
 const Vector2<int>& bottomLeft, 
 const Vector2<int>& topRight,
 Vector2<int>& point1, 
 Vector2<int>& point2
 )
{

  int swap = 0;

  Vector2<int> p;
  int c1 = cohenSutherlandOutCode(bottomLeft, topRight, point1);
  int c2 = cohenSutherlandOutCode(bottomLeft, topRight, point2);
  
  if(c1 == 0 && c2 == 0) return false;

  while ( ( !(c1 == 0) || !(c2 == 0) ) && ( (c1&c2) == 0 ) ) 
  {
    if ( c1 == 0 ) 
    {
      p = point1;
      point1 = point2;
      point2 = p;
      c1 = c2; 
      swap++;
    }
    if (c1 & 1) 
    {
      point1.y = intersection(point1.x,point1.y,point2.x,point2.y,bottomLeft.x) ;
      point1.x = bottomLeft.x ; 
    }
    else if (c1&2)
    {
      point1.y = intersection(point1.x,point1.y,point2.x,point2.y,topRight.x) ;
      point1.x = topRight.x ; 
    }
    else if (c1&4)
    {
      point1.x = intersection(point1.y,point1.x,point2.y,point2.x,bottomLeft.y) ;
      point1.y = bottomLeft.y ; 
    }
    else if (c1&8)
    {
      point1.x = intersection(point1.y,point1.x,point2.y,point2.x,topRight.y) ;
      point1.y = topRight.y ; 
    }
    c1 = cohenSutherlandOutCode(bottomLeft, topRight, point1);
    c2 = cohenSutherlandOutCode(bottomLeft, topRight, point2);
  }
  if ( c1 == 0 && c2 == 0 )
  {
/*    if(
      (point2WasTop && point2.y <= point1.y) ||
      (!point2WasTop && point2.y > point1.y) ||
      (point2WasRight && point2.x <= point1.x) ||
      (!point2WasRight && point2.x > point1.x) 
      )
*/
    if(swap % 2 == 1)
    {
      p = point1 ;
      point1 = point2;
      point2 = p ;
    }
    return true;
  }
  else
    return false;
}

int Geometry::intersection(int a1, int b1, int a2, int b2, int value)
{
  int result = 0 ;
  if ( a2 - a1 != 0 )
    result = (int) (b1 +(double) (value-a1) / (a2-a1) * (b2-b1));
  else
    result = 32767;
  return(result);
}

Vector2<double> Geometry::relative2FieldCoord(const Pose2D& rp, double x, double y)
{
  return rp * Vector2<double>(x,y);
}

Vector2<double> Geometry::relative2FieldCoord(const Pose2D& rp, Vector2<> relPosOnField)
{
  return rp * relPosOnField;
}

Vector2<double> Geometry::fieldCoord2Relative(const Pose2D& robotPose, const Vector2<double>& fieldCoord)
{
  Vector2<double> relativeCoord;
  double distance = Geometry::distanceTo(robotPose, fieldCoord);
  double angle = Geometry::angleTo(robotPose, fieldCoord);
  relativeCoord.x = distance * cos(angle);
  relativeCoord.y = distance * sin(angle);
  return relativeCoord;
}


bool Geometry::calculateBallInImage(const Vector2<double>& ballOffset,
                                    const CameraMatrix& cameraMatrix, const CameraInfo& cameraInfo, double ballRadius, Circle& circle)
{
  Vector2<double> offset(ballOffset.x - cameraMatrix.translation.x,
                         ballOffset.y - cameraMatrix.translation.y);
  double distance = offset.abs(),
         height = cameraMatrix.translation.z - ballRadius,
         cameraDistance = sqrt(sqr(distance) + sqr(height));
  circle.center = Vector2<double>(atan2(offset.y,offset.x),-atan2(height,distance));
  if(cameraDistance >= ballRadius)
  {
    double alpha = pi_2 - circle.center.y - acos(ballRadius / cameraDistance),
           yBottom = -atan2(height + cos(alpha) * ballRadius,
                            distance - sin(alpha) * ballRadius),
           beta = pi_2 - circle.center.y + acos(ballRadius / cameraDistance),
           yTop = -atan2(height + cos(beta) * ballRadius,
                         distance - sin(beta) * ballRadius);
    Vector2<int> top,
                 bottom;
    calculatePointByAngles(Vector2<double>(circle.center.x,yTop),cameraMatrix, cameraInfo, top);
    calculatePointByAngles(Vector2<double>(circle.center.x,yBottom),cameraMatrix, cameraInfo, bottom);
    circle.center.x = (top.x + bottom.x) / 2.0;
    circle.center.y = (top.y + bottom.y) / 2.0;
    circle.radius = (top - bottom).abs() / 2.0;
    return true;
  }
  else
    return false;
}

double Geometry::angleSizeToPixelSize(double angleSize, const CameraInfo& cameraInfo)
{
  return cameraInfo.focalLength * tan(angleSize);
}

double Geometry::pixelSizeToAngleSize(double pixelSize, const CameraInfo& cameraInfo)
{
  return atan(pixelSize * cameraInfo.focalLengthInv);  
}

double Geometry::getDistanceBySize
(
 const CameraInfo& cameraInfo,
 double sizeInReality,
 double sizeInPixels
 )
{
  double xFactor = cameraInfo.focalLength;
  return sizeInReality * xFactor / (sizeInPixels + 0.00000001);
}

double Geometry::getDistanceBySize
(
 const CameraInfo& cameraInfo,
 double sizeInReality,
 double sizeInPixels,
 double centerX,
 double centerY
 )
{
	double mx = centerX;
	double my = centerY;
	double cx = cameraInfo.opticalCenter.x;
	double cy = cameraInfo.opticalCenter.y;
  double focalLenPow2 = cameraInfo.focalLenPow2;
	double sqrImgRadius = (mx-cx)*(mx-cx) + (my-cy)*(my-cy);
	double imgDistance = sqrt(focalLenPow2 + sqrImgRadius);
	return imgDistance*sizeInReality/(sizeInPixels + 0.00000001);
}

double Geometry::getDistanceByAngleSize
(
 double sizeInReality,
 double sizeAsAngle
 )
{
  return (sizeInReality / 2.0) / tan(sizeAsAngle / 2.0 + 0.00000001);
}

double Geometry::getBallDistanceByAngleSize
(
 double sizeInReality,
 double sizeAsAngle
 )
{
  return (sizeInReality / 2.0) / sin(sizeAsAngle / 2.0 + 0.00000001);
}

double Geometry::getSizeByDistance
(
 const CameraInfo& cameraInfo,
 double sizeInReality,
 double distance
)
{
  double xFactor = cameraInfo.focalLength;
  return sizeInReality / distance * xFactor;
}


double Geometry::getSizeByDistance
(
 double sizeInReality,
 double distance,
 double imageWidthPixels,
 double imageWidthAngle
)
{
  double xFactor = imageWidthPixels / tan(imageWidthAngle / 2.0) / 2.0;
  return sizeInReality / distance * xFactor;
}

Geometry::Line Geometry::calculateHorizon
(
const CameraMatrix& cameraMatrix,
const CameraInfo& cameraInfo
)
{
  Line horizon;
  double r31 = cameraMatrix.rotation.c[0].z;
  double r32 = cameraMatrix.rotation.c[1].z;
  double r33 = cameraMatrix.rotation.c[2].z;
  
  if(r33 == 0) 
    r33 = 0.00001;
  
  double x1 = 0,
         x2 = cameraInfo.resolutionWidth - 1,
         v1 = cameraInfo.focalLength,
         v2 = cameraInfo.opticalCenter.x,
         v3 = cameraInfo.opticalCenter.y,
         y1 = (v3 * r33 + r31 * v1 + r32 * v2) / r33,
         y2 = (v3 * r33 + r31 * v1 - r32 * v2) / r33;
  
	// Mirror ends of horizon if Camera rotated to the left  
  if((cameraMatrix.rotation * Vector3<double>(0,0,1)).z < 0)
  {
    double t = x1;
    x1 = x2;
    x2 = t;
    t = y1;
    y1 = y2;
    y2 = t;
  }

  horizon.base.x = (x1 + x2) / 2.0;
  horizon.base.y = (y1 + y2) / 2.0;
  horizon.direction.x = x2 - x1;
  horizon.direction.y = y2 - y1;
  horizon.normalizeDirection();
  return horizon;
}


int Geometry::calculateLineSize
(
 const Vector2<int>& pointInImage, 
 const CameraMatrix& cameraMatrix,
 const CameraInfo& cameraInfo,
 double fieldLinesWidth
 )
{
  Vector2<int> pointOnField; //position on field, relative to robot
  if(Geometry::calculatePointOnField(pointInImage.x, pointInImage.y, cameraMatrix, cameraInfo, pointOnField))
  {
    int distance = (int) sqrt(sqr(cameraMatrix.translation.z) + sqr(pointOnField.abs()));
    return (int)Geometry::getSizeByDistance(cameraInfo, fieldLinesWidth, distance);
  }
  else
  {
    return 0;
  }
}

bool Geometry::calculatePointInImage(const Vector3<double>& pointInWorld, 
                                     const CameraMatrix& cameraMatrix, 
                                     const CameraInfo& cameraInfo,
                                     Vector2<int>& pointInImage)
{
  Vector2<double> offset(pointInWorld.x - cameraMatrix.translation.x,
                         pointInWorld.y - cameraMatrix.translation.y);
  return Geometry::calculatePointByAngles(
    Vector2<double>(atan2(offset.y, offset.x), atan2(pointInWorld.z - cameraMatrix.translation.z, offset.abs())),
    cameraMatrix, cameraInfo,
    pointInImage
  );
}

bool Geometry::calculatePointOnHorizontalPlane(const Vector2<int>& pointInImage, 
                                               double z,
                                               const CameraMatrix& cameraMatrix, 
                                               const CameraInfo& cameraInfo,
                                               Vector2<double>& pointOnPlane)
{
  double xFactor = cameraInfo.focalLengthInv,
         yFactor = cameraInfo.focalLengthInv;

  Vector3<double> 
    vectorToCenter(1, (cameraInfo.opticalCenter.x - pointInImage.x) * xFactor, 
                      (cameraInfo.opticalCenter.y - pointInImage.y) * yFactor);
  
  Vector3<double> 
    vectorToCenterWorld = cameraMatrix.rotation * vectorToCenter;

  double a1 = cameraMatrix.translation.x,
         a2 = cameraMatrix.translation.y,
         a3 = cameraMatrix.translation.z - z,
         b1 = vectorToCenterWorld.x,
         b2 = vectorToCenterWorld.y,
         b3 = vectorToCenterWorld.z;
  if(fabs(b3) > 0.00001)
  {
    pointOnPlane.x = (a1 * b3 - a3 * b1) / b3;
    pointOnPlane.y = (a2 * b3 - a3 * b2) / b3;
    return true;
  }
  else
    return false;
}
