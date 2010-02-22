/**
 * @file Tools/SimGeometry.cpp
 * 
 * Implementation of class SimGeometry
 *
 * @author <A href="mailto:timlaue@informatik.uni-bremen.de">Tim Laue</A>
 */ 

#include <float.h> // DBL_MAX

#include "SimGeometry.h"
#include "SimGraphics.h"


std::vector<Vector3d> SimGeometry::pointBuffer;


bool SimGeometry::pointInsidePolygon(const Vector3d& p, 
                                     const std::vector<Vector3d>& polygon,
                                     const unsigned int numberOfPoints,
                                     const Vector3d& normal)
{
  int evenOddCount = 0;
  if(fabs(normal.v[0]) > fabs(normal.v[1]) &&
     fabs(normal.v[0]) > fabs(normal.v[2]))
    for(unsigned i = 0; i < numberOfPoints; ++i) 
    {
      const Vector3d& v1 = polygon[i],
                      v2 = polygon[(i + 1) % numberOfPoints];
      if((v1.v[1] > p.v[1] && v2.v[1] <= p.v[1]) ||
	 (v1.v[1] <= p.v[1] && v2.v[1] > p.v[1]))
      {
        double s = (v1.v[2] - v2.v[2]) / (v1.v[1] - v2.v[1]) * (p.v[1] - v2.v[1]) + v2.v[2];
        if (s < p.v[2]) 
          ++evenOddCount;
      }
    }
  else if(fabs(normal.v[1]) > fabs(normal.v[2])) 
    for(unsigned i = 0; i < numberOfPoints; ++i) 
    {
      const Vector3d& v1 = polygon[i],
                      v2 = polygon[(i + 1) % numberOfPoints];
      if((v1.v[2] > p.v[2] && v2.v[2] <= p.v[2]) ||
         (v1.v[2] <= p.v[2] && v2.v[2] > p.v[2]))
      {
        double s = (v1.v[0] - v2.v[0]) / (v1.v[2] - v2.v[2]) * (p.v[2] - v2.v[2]) + v2.v[0];
        if(s < p.v[0]) 
          ++evenOddCount;
      }
    }
  else
    for(unsigned i = 0; i < numberOfPoints; ++i) 
    {
      const Vector3d& v1 = polygon[i],
                      v2 = polygon[(i + 1) % numberOfPoints];
      if((v1.v[1] > p.v[1] && v2.v[1] <= p.v[1]) ||
         (v1.v[1] <= p.v[1] && v2.v[1] > p.v[1])) 
      {
        double s = (v1.v[0] - v2.v[0]) / (v1.v[1] - v2.v[1]) * (p.v[1] - v2.v[1]) + v2.v[0];
        if(s < p.v[0]) 
          ++evenOddCount;
      }
    }
  return evenOddCount % 2 == 1;
}


bool SimGeometry::pointInsidePolygon(const Vector3d& p, 
                                     const std::vector<Vector3d>& polygon,
                                     const unsigned int numberOfPoints,
                                     double maxDistance) 
{
  unsigned int point;
  //Compute a ray v to another point, which is definitely outside the polygon
  //and intersects the first side of the polygon
  Vector3d pointOnSide(polygon[0] + ((polygon[1] - polygon[0])*0.5));
  Vector3d v(pointOnSide - p);
  v.normalize();
  v *= 2.0*maxDistance;
  //Now check for another intersection of the ray with the polygon
  Vector3d p1,p2;
  for(point=1; point < numberOfPoints; point++)
  {
    p1 = (polygon[point]);
    p2 = (polygon[(point+1) % numberOfPoints]);
    if(intersectRayAndLine(p,v,p1,p2))
    {
      return false;
    }
  }
  return true;
}


bool SimGeometry::intersectRayAndPlane(const Vector3d& point, const Vector3d& v, 
                                       const Vector3d& plane, const Vector3d& n, 
                                       Vector3d& intersection) 
{
  Vector3d p(plane - point);
  double denominator(n.vecmul(v));
  if(denominator == 0.0)
  {
    return false;
  }
  double r(n.vecmul(p) / denominator);
  if(r < 0.0)
  {
    return false;
  }
  intersection = v;
  intersection *=r;
  intersection += point;
  return true;
}


bool SimGeometry::intersectRayAndSphere(const Vector3d& rayPosition, const Vector3d& ray, 
                                        const Vector3d& spherePosition, double radius, 
                                        Vector3d& intersection)
{
  //Compute distance from the ray to the sphere center:
  Vector3d distVec(spherePosition - rayPosition);
  distVec = ray * distVec;
  double distance(distVec.getLength() / ray.getLength());
  //Does the ray intersect the sphere? :
  if(distance <= radius)
  {
    //formula: (pos + t*ray - position)² = radius²     | m = pos - position
    //     <=> (m + t*ray)² = radius²
    //     <=> m² + 2*t*m*ray + t²*ray² = radius²
    //     <=> ray²*t² + 2*m*ray*t + m²-radius² = 0    | a=ray², b=2*m*ray, c=m²-radius²
    //     <=> a*t² + b*t +c = 0;
    Vector3d m(rayPosition - spherePosition);
    double a(ray.vecmul(ray));
    double b(2 * m.vecmul(ray));
    double c(m.vecmul(m) - radius * radius);
    double p(b / a);
    double q(c / a);
    double det((p / 2) * (p / 2) - q);
    if(det >= 0)
    {
      double t1(- p / 2 + sqrt(det));
      double t2(- p / 2 - sqrt(det));
      if(t1 >= 0 && t1 < t2)
        intersection = rayPosition + ray * t1;
      else if(t2 >= 0) 
        intersection = rayPosition + ray * t2;
      else
        return false;
      return true;
    }
  }
  return false; //In all other cases, there is no intersection point
}


bool SimGeometry::intersectRayAndCylinder(const Vector3d& rayPosition, const Vector3d& ray, 
                                          const Vector3d& cylinderPosition, const Matrix3d& cylinderRotation, 
                                          double height, double radius, bool capped, Vector3d& intersection)
{
  // Considering an unrotated cylinder standing upright at the origin, the ray has to be transformed:
  Vector3d r,v;
  transformRayToObjectAtOrigin(rayPosition, ray, cylinderPosition, cylinderRotation, r, v);
  int numberOfIntersections(0);
  Vector3d bestIntersection;
  // Idea: Try to find two positions on the ray with a distance to the z-axis 
  //       equal to the radius of he cylinder:
  // Formula for the distance of a point to a straight line: 
  //         Let p be the point, a the position of the line and b the ray:
  //                 d = |b x (p - a)| / |b|
  //     => (d * |b|)² = (b x (p - a))²
  // Since the straight line is the z-axis and starts at the origin:
  //    <=> (d * |b|)² = (b x p)²
  // The point p must be on a the ray:
  //    <=> (d * |b|)² = (b x (r + s*v))²
  // Let b be (0,0,1) pointing along the z-axis, |b| ist 1:
  //    <=>         d² = (b x (r + s*v))*(b x (r + s*v))
  //    <=>          0 = s²(v1²+v0²)+2s(v1r1+v0r0)+r1²+r0²-d²
  //    <=>          0 = a*s² + 2sb + c
  double a(v.v[1]*v.v[1] + v.v[0]*v.v[0]);
  if(fabs(a) > EPSILON)
  {
    // Solve quadratic equation using the pq-formula:
    double b(v.v[1]*r.v[1] + v.v[0]*r.v[0]);
    double c(r.v[1]*r.v[1] + r.v[0]*r.v[0] - radius*radius);
    double p(2*b/a);
    double q(c/a);
    // s = -(p/2) +- sqrt((p/2)² -q)
    double denominator(p*p/4.0 -q);
    if(denominator >= 0)
    {
      // Compute the two possible intersection points with the cylinder:
      double s1(-p/2.0 + sqrt(denominator));
      double s2(-p/2.0 - sqrt(denominator));
      Vector3d point1,point2;
      bool point1Valid(isValidIntersectionPointOnCylinder(r,v,s1,height,point1));
      bool point2Valid(isValidIntersectionPointOnCylinder(r,v,s2,height,point2));
      if(point1Valid && point2Valid)
      {
        numberOfIntersections = 2;
        bestIntersection = s1 < s2 ? point1 : point2;
      }
      else if(point1Valid)
      {
        numberOfIntersections = 1;
        bestIntersection = point1;
      }
      else if(point2Valid)
      {
        numberOfIntersections = 1;
        bestIntersection = point2;
      }
    }
  }
  //Now try to intersect the ends of the cylinder:
  if(numberOfIntersections < 2)
  {
    Vector3d point1, point2;
    if(capped)  //At each and, there is a sphere
    {
      //Try to intersect upper sphere:
      if(SimGeometry::intersectRayAndSphere(r,v,Vector3d(0,0,height/2.0),radius,point1))
      {
        if(numberOfIntersections == 1) //The new intersection is the last possible one
        {
          if ((point1-r).getQuadLength() < bestIntersection.getQuadLength())
             bestIntersection = point1;
        }
        else //Check if the other sphere is intersected, too
        {
          if(SimGeometry::intersectRayAndSphere(r,v,Vector3d(0,0,-height/2.0),radius,point1))
          {
            double p1Dist((r-point1).getQuadLength());
            double p2Dist((r-point2).getQuadLength());
            bestIntersection = p1Dist < p2Dist ? point1 : point2;
          }
          else
          {
            bestIntersection = point1;
          }
        }
        numberOfIntersections = 1; //Real number does not matter, it is at least 1 ;-)
      }
      else if(SimGeometry::intersectRayAndSphere(r,v,Vector3d(0,0,-height/2.0),radius,point1))
      {
        if(numberOfIntersections > 0)
        {
          if ((point2-r).getQuadLength() < bestIntersection.getQuadLength())
             bestIntersection = point2;
        }
        else
        {
          bestIntersection = point2;
        }
        numberOfIntersections = 1; //Real number does not matter, it is at least 1 ;-)
      }
    }
    else        //The ends are closed by disks
    {
      //Try to intersect upper disk:
      if(SimGeometry::intersectDiskInXYPlane(r,v,height/2.0,radius,point1))
      {
        if(numberOfIntersections == 1) //The new intersection is the last possible one
        {
          if ((point1-r).getQuadLength() < bestIntersection.getQuadLength())
             bestIntersection = point1;
        }
        else //Check if the other disk is intersected, too
        {
          if(SimGeometry::intersectDiskInXYPlane(r,v,-height/2.0,radius,point2))
          {
            double p1Dist((r-point1).getQuadLength());
            double p2Dist((r-point2).getQuadLength());
            bestIntersection = p1Dist < p2Dist ? point1 : point2;
          }
          else
          {
            bestIntersection = point1;
          }
        }
        numberOfIntersections = 1; //Real number does not matter, it is at least 1 ;-)
      }
      else if(SimGeometry::intersectDiskInXYPlane(r,v,-height/2.0,radius,point2))
      {
        if(numberOfIntersections > 0)
        {
          if ((point2-r).getQuadLength() < bestIntersection.getQuadLength())
             bestIntersection = point2;
        }
        else
        {
          bestIntersection = point2;
        }
        numberOfIntersections = 1; //Real number does not matter, it is at least 1 ;-)
      }
    }
  }
  //Backtransform the result, if any exists:
  if(numberOfIntersections)
  {
    bestIntersection.rotate(cylinderRotation);
    bestIntersection += cylinderPosition;
    intersection = bestIntersection;
    return true;
  }
  return false;
}


bool SimGeometry::intersectRayAndBox(const Vector3d& rayPosition, const Vector3d& ray, 
                                     const Vector3d& boxPosition, const Matrix3d& boxRotation, 
                                     double length, double width, double height, Vector3d& intersection)
{                 
  // Considering an unrotated box at the origin, the ray has to be transformed:
  Vector3d rayPos,v;
  transformRayToObjectAtOrigin(rayPosition, ray, boxPosition, boxRotation, rayPos, v);
  // Now three sides of the box have to be chosen to be intersected:
  double l_2(length*0.5);
  double w_2(width*0.5);
  double h_2(height*0.5);
  Vector3d intersectionPos;
  Vector3d foundPoints[3];
  int numberOfPoints(0);
  if(rayPos.v[0]>0.0)
  {
    if(intersectRayWithAxisAlignedPlane(rayPos,v,l_2,0,w_2,h_2,intersectionPos))
      foundPoints[numberOfPoints++] = intersectionPos;
  }
  else
  {
    if(intersectRayWithAxisAlignedPlane(rayPos,v,-l_2,0,w_2,h_2,intersectionPos))
      foundPoints[numberOfPoints++] = intersectionPos;
  }
  if(rayPos.v[1]>0.0)
  {
    if(intersectRayWithAxisAlignedPlane(rayPos,v,w_2,1,h_2,l_2,intersectionPos))
      foundPoints[numberOfPoints++] = intersectionPos;
  }
  else
  {
    if(intersectRayWithAxisAlignedPlane(rayPos,v,-w_2,1,h_2,l_2,intersectionPos))
      foundPoints[numberOfPoints++] = intersectionPos;
  }
  if(rayPos.v[2]>0.0)
  {
    if(intersectRayWithAxisAlignedPlane(rayPos,v,h_2,2,l_2,w_2,intersectionPos))
      foundPoints[numberOfPoints++] = intersectionPos;
  }
  else
  {
    if(intersectRayWithAxisAlignedPlane(rayPos,v,-h_2,2,l_2,w_2,intersectionPos))
      foundPoints[numberOfPoints++] = intersectionPos;
  }
  if(numberOfPoints)
  {
    int nearestPoint(0);
    double shortestDist((rayPos-foundPoints[0]).getQuadLength());
    for(int i=1; i<numberOfPoints; i++)
    {
      double currentDist((rayPos-foundPoints[i]).getQuadLength());
      if(currentDist<shortestDist)
      {
        shortestDist = currentDist;
        nearestPoint =i;
      }
    }
    intersection = foundPoints[nearestPoint];
    intersection.rotate(boxRotation);
    intersection += boxPosition;
    return true;
  }
  else
  {
    return false;
  }
}

bool SimGeometry::intersectRayAndAxisAlignedBoundingBox(const Vector3d& rayPosition, const Vector3d& ray, 
                                                        const AxisAlignedBoundingBox& box)
{
  Vector3d boxCenter(box.mini + box.maxi);
  boxCenter /= 2.0;
  AxisAlignedBoundingBox localBox(box);
  localBox.mini -= boxCenter;
  localBox.maxi -= boxCenter;
  Vector3d rayPos(rayPosition - boxCenter);
  double l_2((localBox.maxi.v[0] - localBox.mini.v[0])/2.0);
  double w_2((localBox.maxi.v[1] - localBox.mini.v[1])/2.0);
  double h_2((localBox.maxi.v[2] - localBox.mini.v[2])/2.0);
  Vector3d intersectionPos;
  if(rayPos.v[0]>0.0)
  {
    if(intersectRayWithAxisAlignedPlane(rayPos,ray,l_2,0,w_2,h_2,intersectionPos))
      return true;
  }
  else
  {
    if(intersectRayWithAxisAlignedPlane(rayPos,ray,-l_2,0,w_2,h_2,intersectionPos))
      return true;
  }
  if(rayPos.v[1]>0.0)
  {
    if(intersectRayWithAxisAlignedPlane(rayPos,ray,w_2,1,h_2,l_2,intersectionPos))
      return true;
  }
  else
  {
    if(intersectRayWithAxisAlignedPlane(rayPos,ray,-w_2,1,h_2,l_2,intersectionPos))
      return true;
  }
  if(rayPos.v[2]>0.0)
  {
    if(intersectRayWithAxisAlignedPlane(rayPos,ray,h_2,2,l_2,w_2,intersectionPos))
      return true;
  }
  else
  {
    if(intersectRayWithAxisAlignedPlane(rayPos,ray,-h_2,2,l_2,w_2,intersectionPos))
      return true;
  }
  return false;
}

bool SimGeometry::intersectRayAndGraphicsPrimitive(const Vector3d& rayPosition, const Vector3d& ray, 
                                                   const GraphicsPrimitiveDescription& primitive, Vector3d& intersection)
{
  if(primitive.type == POLYGON)
  {
    Vector3d pos,normalVec;
    normalVec.v[0] = primitive.normals[0]; 
    normalVec.v[1] = primitive.normals[1]; 
    normalVec.v[2] = primitive.normals[2];
    if(intersectRayAndConvexPolygon(rayPosition,ray,primitive.vertices,normalVec,primitive.numberOfVertices,pos))
    {
      intersection = pos;
      return true;
    }
  }
  else //primitive.type == TRIANGLE_STRIP
  {
    double quadDistToNearestIntersection(DBL_MAX);
    bool intersectionFound(false);
    Vector3d bestIntersection, pos, normalVec;
    Vector3d trianglePoints[3];
    // Intersect first triangle "manually"
    trianglePoints[0].v[0] = primitive.vertices[0];
    trianglePoints[0].v[1] = primitive.vertices[1];
    trianglePoints[0].v[2] = primitive.vertices[2];
    trianglePoints[1].v[0] = primitive.vertices[3];
    trianglePoints[1].v[1] = primitive.vertices[4];
    trianglePoints[1].v[2] = primitive.vertices[5];
    trianglePoints[2].v[0] = primitive.vertices[6];
    trianglePoints[2].v[1] = primitive.vertices[7];
    trianglePoints[2].v[2] = primitive.vertices[8];
    normalVec.v[0] = primitive.normals[0]; 
    normalVec.v[1] = primitive.normals[1]; 
    normalVec.v[2] = primitive.normals[2];
    if(intersectRayAndTriangle(rayPosition, ray, trianglePoints[0], trianglePoints[1], trianglePoints[2],
                               normalVec, pos))
    {
      intersectionFound = true;
      bestIntersection = pos;
      quadDistToNearestIntersection = (pos-rayPosition).getQuadLength();
    }
    for(unsigned int i=3; i < primitive.numberOfVertices; i++)
    {
      int tIndex(i%3);
      trianglePoints[tIndex].v[0] = primitive.vertices[i*3];
      trianglePoints[tIndex].v[1] = primitive.vertices[i*3+1];
      trianglePoints[tIndex].v[2] = primitive.vertices[i*3+2];
      normalVec.v[0] = primitive.normals[i*3];
      normalVec.v[1] = primitive.normals[i*3+1];
      normalVec.v[2] = primitive.normals[i*3+2];
      Vector3d testNormal;
      normalVectorOfPlane(trianglePoints[0], trianglePoints[1], trianglePoints[2], testNormal);
      if(intersectRayAndTriangle(rayPosition, ray, trianglePoints[0], trianglePoints[1], trianglePoints[2],
                                 normalVec, pos))
      {
        intersectionFound = true;
        double currentQuadDist((pos-rayPosition).getQuadLength());
        if(currentQuadDist < quadDistToNearestIntersection)
        {
          quadDistToNearestIntersection = currentQuadDist;
          bestIntersection = pos;
        }
      }
    }
    if(intersectionFound)
    {
      intersection = bestIntersection;
      return true;
    }
  } // end of primitive.type == TRIANGLE_STRIP
  return false;
}


bool SimGeometry::intersectRayWithAxisAlignedPlane(const Vector3d& p, const Vector3d& v,
                                                   double distance, int thirdAxis, 
                                                   double maxAbsAxis1, double maxAbsAxis2,
                                                   Vector3d& intersectionPos)
{
  if(v.v[thirdAxis] != 0.0)
  {
    double s((distance-p.v[thirdAxis])/v.v[thirdAxis]);
    int i(thirdAxis);
    intersectionPos.v[i++] = distance;
    i %= 3;
    intersectionPos.v[i] = s*v.v[i] + p.v[i];
    if(fabs(intersectionPos.v[i]) > maxAbsAxis1) return false;
    i = (thirdAxis + 2) % 3;
    intersectionPos.v[i] = s*v.v[i] + p.v[i];
    if(fabs(intersectionPos.v[i]) > maxAbsAxis2) return false;
    return true;
  }
  else
  {
    //No possible solution
    return false;
  }
}

bool SimGeometry::fourPointsInPlane(const Vector3d& p, const Vector3d& a,
                                    const Vector3d& b, const Vector3d& c) 
{
  Vector3d va(a-p);
  Vector3d vb(b-p);
  Vector3d pc(c-p);
  Vector3d n;
  n = va * vb;
  double value(n.vecmul(pc));
  return (fabs(value)<EPSILON);
}

void SimGeometry::normalVectorOfPlane(const Vector3d& a, const Vector3d& b, const Vector3d& c, Vector3d& n)
{
  Vector3d ab(b-a);
  Vector3d ac(c-a);
  n = ab * ac;
}

bool SimGeometry::intersectRayAndLine(const Vector3d& p, const Vector3d& v,
                                      const Vector3d& p1, const Vector3d& p2)
{
  Vector3d w(p2-p1);
  Vector3d q(p1);
  Vector3d c(q-p);
  double denominator(v.v[0]*w.v[1]-v.v[1]*w.v[0]);
  double m;
  if(denominator == 0.0)
  {
    denominator = v.v[0]*w.v[2]-v.v[2]*w.v[0];
    if(denominator == 0.0)
    {
      denominator = v.v[1]*w.v[2]-v.v[2]*w.v[1];
      if(denominator == 0.0)
      {
        return false;
      }
      else
      {
        m = (c.v[1]*w.v[2] - c.v[2]*w.v[1])/denominator;
      }
    }
    else
    {
      m = (c.v[0]*w.v[2] - c.v[2]*w.v[0])/denominator;
    }
  }
  else
  {
    m = (c.v[0]*w.v[1] - c.v[1]*w.v[0])/denominator;
  }
  if((m >= 0.0) && (m < 1.0))
  {
    double n;
    if(w.v[0] != 0.0)
    {
      n = (m*v.v[0]-c.v[0]) / w.v[0];
    }
    else if(w.v[1] != 0.0)
    {
      n = (m*v.v[1]-c.v[1]) / w.v[1];
    }
    else if(w.v[2] != 0.0)
    {
      n = (m*v.v[2]-c.v[2]) / w.v[2];
    }
    else
    {
      return false;
    }
    return ((n>=0.0) && (n<=1.0));
  }
  else //Wrong side of the ray or too far away... :-(
  {
    return false;
  }
}


bool SimGeometry::isValidIntersectionPointOnCylinder(const Vector3d& rayPos, const Vector3d& ray,
                                                     double s, double height, Vector3d& point)
{
  if(s < 0) return false;
  point = (rayPos + (ray*s));
  if(Functions::between(-height/2,height/2,point.v[2]))
    return true;
  else
    return false;
}


bool SimGeometry::intersectDiskInXYPlane(const Vector3d& rayPos, const Vector3d& ray,
                                         double z, double radius, Vector3d& intersection)
{
  if(fabs(ray.v[2]) < EPSILON)
     return false;
  double s((z-rayPos.v[2])/ray.v[2]);
  intersection = ray*s + rayPos;
  //Check, if intersection is inside the radius:
  double dist(sqrt(intersection.v[0]*intersection.v[0] + intersection.v[1]*intersection.v[1]));
  if(dist < radius)
    return true;
  else
    return false;
}


void SimGeometry::transformRayToObjectAtOrigin(const Vector3d& rayPosition, const Vector3d& ray,
                                               const Vector3d& objPosition, const Matrix3d& objRotation,
                                               Vector3d& transformedPosition, Vector3d& transformedRay)
{
  transformedPosition = (rayPosition - objPosition);
  Matrix3d inverseObjRotation(objRotation);
  inverseObjRotation.transpose();
  transformedPosition.rotate(inverseObjRotation);
  transformedRay = ray;
  transformedRay.rotate(inverseObjRotation);
}


bool SimGeometry::intersectRayAndConvexPolygon(const Vector3d& rayPos, const Vector3d& ray,
                                               double* points, const Vector3d& normalVec, int numberOfPoints,
                                               Vector3d& intersectionPos)
{
  Vector3d point(points[0], points[1], points[2]);
  if(intersectRayAndPlane(rayPos,ray,point,normalVec,intersectionPos))
  {
    pointBuffer.clear();
    int i(0);
    while(i<numberOfPoints*3)
    {
      Vector3d p;
      p.v[0] = points[i++];
      p.v[1] = points[i++];
      p.v[2] = points[i++];
      pointBuffer.push_back(p);
    }
    if(pointInsidePolygon(intersectionPos, pointBuffer, numberOfPoints, normalVec))
      return true;
  }
  return false;
}


bool SimGeometry::intersectRayAndTriangle(const Vector3d& rayPos, const Vector3d& ray,
                                          const Vector3d& p1, const Vector3d& p2, const Vector3d& p3, 
                                          const Vector3d& normalVec, Vector3d& intersectionPos)
{
  if(intersectRayAndPlane(rayPos,ray,p1,normalVec,intersectionPos))
  {
    pointBuffer.clear();
    pointBuffer.push_back(p1);
    pointBuffer.push_back(p2);
    pointBuffer.push_back(p3);
    if(pointInsidePolygon(intersectionPos, pointBuffer, 3, normalVec))
      return true;
  }
  return false;
}
