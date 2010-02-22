/**
 * @file SimGeometry.h
 *
 * Definition of static class SimGeometry
 * A collection of functions for geometric computations
 *
 * @author <A href="mailto:timlaue@informatik.uni-bremen.de">Tim Laue</A>
 */ 

#ifndef SIMGEOMETRY_H_
#define SIMGEOMETRY_H_

#include "SimMath.h"
#include <vector>

class SimObject;
class GraphicsPrimitiveDescription;


/**
* @class Intersection
* A class describing an intersection point with an object
*/
class Intersection
{
public:
  /** The adress of the intersected object*/
  SimObject* object;
  /** True, if the intersection has a known position*/
  bool hasPosition;
  /** The position of the intersection, if known*/
  Vector3d position;
};


/**
* @class AxisAlignedBoundingBox
* A bounding box used for intersection and zoom computations
*/
class AxisAlignedBoundingBox
{
public:
  /** Constructor
  * @param initial Initial value for mini and maxi
  */
  AxisAlignedBoundingBox(const Vector3d& initial): mini(initial), maxi(initial)
  {}

  /** Checks, if a given point is inside the box
  * @param p The point
  * @return true, if the point is inside the box
  */
  bool isInside(const Vector3d& p) const
  {
    return (p.v[0] < maxi.v[0]) && (p.v[0] > mini.v[0]) &&
           (p.v[1] < maxi.v[1]) && (p.v[1] > mini.v[1]) &&
           (p.v[2] < maxi.v[2]) && (p.v[2] > mini.v[2]);
  }

  /** Expands the bounding box (if necessary) to include a new point
  * @param p The point
  */
  void expand(const Vector3d& p)
  {
    if(p.v[0] < mini.v[0])
      mini.v[0] = p.v[0];
    else if(p.v[0] > maxi.v[0])
      maxi.v[0] = p.v[0];
    if(p.v[1] < mini.v[1])
      mini.v[1] = p.v[1];
    else if(p.v[1] > maxi.v[1])
      maxi.v[1] = p.v[1];
    if(p.v[2] < mini.v[2])
      mini.v[2] = p.v[2];
    else if(p.v[2] > maxi.v[2])
      maxi.v[2] = p.v[2];
  }

  /** Expands the bounding box (if necessary) to include another bounding box
  * @param other The other bounding box
  */
  void expand(const AxisAlignedBoundingBox& other)
  {
    expand(other.maxi);
    expand(other.mini);
  }

  /* Computes all eight corner points of the box. 
   * @param corners An array of positions. Memory has to be allocated by caller!
   */
  void getCorners(Vector3d corners[])
  {
    corners[0] = maxi;
    corners[1].v[0] = maxi.v[0];
    corners[1].v[1] = maxi.v[1];
    corners[1].v[2] = mini.v[2];
    corners[2].v[0] = maxi.v[0];
    corners[2].v[1] = mini.v[1];
    corners[2].v[2] = mini.v[2];
    corners[3].v[0] = maxi.v[0];
    corners[3].v[1] = mini.v[1];
    corners[3].v[2] = maxi.v[2];
    corners[4].v[0] = mini.v[0];
    corners[4].v[1] = mini.v[1];
    corners[4].v[2] = maxi.v[2];
    corners[5].v[0] = mini.v[0];
    corners[5].v[1] = maxi.v[1];
    corners[5].v[2] = maxi.v[2];
    corners[6].v[0] = mini.v[0];
    corners[6].v[1] = maxi.v[1];
    corners[6].v[2] = mini.v[2];
    corners[7] = mini;
  }

  void expandMaxX(double x)
  {
    if(x > maxi.v[0]) maxi.v[0] = x;
  }

  void expandMinX(double x)
  {
    if(x < mini.v[0]) mini.v[0] = x;
  }

  void expandMaxY(double y)
  {
    if(y > maxi.v[1]) maxi.v[1] = y;
  }

  void expandMinY(double y)
  {
    if(y < mini.v[1]) mini.v[1] = y;
  }

  void expandMaxZ(double z)
  {
    if(z > maxi.v[2]) maxi.v[2] = z;
  }

  void expandMinZ(double z)
  {
    if(z < mini.v[2]) mini.v[2] = z;
  }

  /** The minimum values of all axes*/
  Vector3d mini;
  /** The maximum values of all axes*/
  Vector3d maxi;
};


/**
 * @class SimGeometry
 *
 * A collection of functions for geometric computations
 */
class SimGeometry
{
public:
  /** Tests if a point is inside a polygon
  * @param p The point
  * @param polygon The polygon
  * @param numberOfPoints The number of points of the polygon
  * @param normal The normal vector of the polygon
  * @return true, if the point is inside the polygon
  */
  static bool pointInsidePolygon(const Vector3d& p, 
                                 const std::vector<Vector3d>& polygon,
                                 const unsigned int numberOfPoints,
                                 const Vector3d& normal);

  /** Tests if a point is inside a polygon
  * @param p The point
  * @param polygon The polygon
  * @param numOfPoints The number of points of the polygon
  * @param maxDistance The maximum distance of any point inside to point[0]
  * @return true, if the point is inside the polygon
  */
  static bool pointInsidePolygon(const Vector3d& p, 
                                 const std::vector<Vector3d>& polygon,
                                 const unsigned int numOfPoints,
                                 double maxDistance);
  
  /** Tests if a ray and a plane have an intersection point and
  *   returns that point.
  * @param point The starting point of the ray
  * @param v The ray
  * @param plane The position of the plane
  * @param n The normal vector of the plane
  * @param intersection The intersection point, if any exists
  * @return true, if any intersection point exists
  */
  static bool intersectRayAndPlane(const Vector3d& point, const Vector3d& v, 
                                   const Vector3d& plane, const Vector3d& n, 
                                   Vector3d& intersection);

  /** Tests if a ray and an axis aligned bounding box intersect
  * @param rayPosition The starting point of the ray
  * @param ray The ray
  * @param box The box
  * @return true, if any intersection point exists
  */
  static bool intersectRayAndAxisAlignedBoundingBox(const Vector3d& rayPosition, const Vector3d& ray, 
                                                    const AxisAlignedBoundingBox& box);

  /** Tests if a ray and a box have an intersection point and
  *   returns that point.
  * @param rayPosition The starting point of the ray
  * @param ray The ray
  * @param boxPosition The position of the box
  * @param boxRotation The rotation of the box
  * @param length The total length of the box
  * @param width The total width of the box
  * @param height The total height of the box
  * @param intersection The intersection point, if any exists
  * @return true, if any intersection point exists
  */
  static bool intersectRayAndBox(const Vector3d& rayPosition, const Vector3d& ray, 
                                 const Vector3d& boxPosition, const Matrix3d& boxRotation, 
                                 double length, double width, double height, Vector3d& intersection);

  /** Tests if a ray and a cylinder have an intersection point and
  *   returns that point.
  * @param rayPosition The starting point of the ray
  * @param ray The ray
  * @param cylinderPosition The position of the cylinder
  * @param cylinderRotation The rotation of the cylinder
  * @param height The height of the cylinder
  * @param radius The radius of the cylinder
  * @param capped Indicates whether the cylinder has spheres or disks at the ends
  * @param intersection The intersection point, if any exists
  * @return true, if any intersection point exists
  */
  static bool intersectRayAndCylinder(const Vector3d& rayPosition, const Vector3d& ray, 
                                      const Vector3d& cylinderPosition, const Matrix3d& cylinderRotation, 
                                      double height, double radius, bool capped, Vector3d& intersection);

  /** Tests if a ray and a sphere have an intersection point and
  *   returns that point.
  * @param rayPosition The starting point of the ray
  * @param ray The ray
  * @param spherePosition The position of the sphere
  * @param radius The radius of the sphere
  * @param intersection The intersection point, if any exists
  * @return true, if any intersection point exists
  */
  static bool intersectRayAndSphere(const Vector3d& rayPosition, const Vector3d& ray, 
                                    const Vector3d& spherePosition, double radius, 
                                    Vector3d& intersection);

  /** Tests if a ray and a graphics primitive have an intersection point and
  *   returns that point.
  * @param rayPosition The starting point of the ray
  * @param ray The ray
  * @param primitive The primitive
  * @param intersection The intersection point, if any exists
  * @return true, if any intersection point exists
  */
  static bool intersectRayAndGraphicsPrimitive(const Vector3d& rayPosition, const Vector3d& ray, 
                                               const GraphicsPrimitiveDescription& primitive, Vector3d& intersection);

  /** Checks if a point b is in the same plane as the points a,b and c.
  * @param p The point p
  * @param a The point a
  * @param b The point b
  * @param c The point c
  * @return true, if all four points are in one plane
  */
  static bool fourPointsInPlane(const Vector3d& p, const Vector3d& a,
                                const Vector3d& b, const Vector3d& c);

  /** Computes the normal vector of a plane described by three points
  * @param a First point
  * @param b Second point
  * @param c Third point
  * @return n The normal vector
  */
  static void normalVectorOfPlane(const Vector3d& a, const Vector3d& b, const Vector3d& c, Vector3d& n);

  /** Transforms a ray in the same way as an object which is transformed to the origin
  * @param rayPosition The starting point of the ray
  * @param ray The ray
  * @param objPosition The position of the object before the transformation
  * @param objRotation The position of the object before the rotation
  * @param transformedPosition The new position of the ray is returned here
  * @param transformedRay The new ray is returned here
  */
  static void transformRayToObjectAtOrigin(const Vector3d& rayPosition, const Vector3d& ray,
                                           const Vector3d& objPosition, const Matrix3d& objRotation,
                                           Vector3d& transformedPosition, Vector3d& transformedRay);
private:
  /** Intersects a ray with a line
  * @param p The starting position of the ray
  * @param v The ray
  * @param p1 The first point of the line
  * @param p2 The second point of the line
  * @return true, if there is any intersection
  */
  static bool intersectRayAndLine(const Vector3d& p, const Vector3d& v,
                                  const Vector3d& p1, const Vector3d& p2);

  /** Intersects a ray with an axis-aligned plane
  * @param p The starting position of the ray
  * @param v The ray
  * @param distance The distance of the plane to the origin
  * @param thirdAxis The number (0,1,2) of the axis to which the ray is not aligned
  * @param maxAbsAxis1 The absolute maximum value on the first axis
  * @param maxAbsAxis2 The absolule maximum value on the second axis
  * @param intersectionPos The position of the intersection
  * @return true, if any point has been computed
  */
  static bool intersectRayWithAxisAlignedPlane(const Vector3d& p, const Vector3d& v,
                                               double distance, int thirdAxis,
                                               double maxAbsAxis1, double maxAbsAxis2,
                                               Vector3d& intersectionPos);

  static bool isValidIntersectionPointOnCylinder(const Vector3d& rayPos, const Vector3d& ray,
                                                 double s, double height, Vector3d& point);

  static bool intersectDiskInXYPlane(const Vector3d& rayPos, const Vector3d& ray,
                                     double z, double radius, Vector3d& intersection);

  static bool intersectRayAndConvexPolygon(const Vector3d& rayPos, const Vector3d& ray,
                                           double* points, const Vector3d& normalVec, int numberOfPoints,
                                           Vector3d& intersectionPos);

  static bool intersectRayAndTriangle(const Vector3d& rayPos, const Vector3d& ray,
                                      const Vector3d& p1, const Vector3d& p2, const Vector3d& p3, 
                                      const Vector3d& normalVec, Vector3d& intersectionPos);

  static std::vector<Vector3d> pointBuffer;
};


#endif //SIMGEOMETRY_H_
