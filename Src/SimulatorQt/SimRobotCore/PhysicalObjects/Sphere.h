/**
 * @file Sphere.h
 * 
 * Definition of class Sphere
 *
 * @author <A href="mailto:timlaue@informatik.uni-bremen.de">Tim Laue</A>
 * @author <A href="mailto:kspiess@informatik.uni-bremen.de">Kai Spiess</A>
 */ 

#ifndef SPHERE_H_
#define SPHERE_H_

#include "PhysicalObject.h"

/**
* @class Sphere
*
* A class representing a sphere in 3D space
*/
class Sphere : public PhysicalObject
{
protected:
  /** The radius of the sphere*/
  double radius;
  /** A handle for drawing the sphere*/
  GraphicsPrimitiveHandle graphicsHandle;

  /** Intersects the sphere and its children with a ray
  * @param pos The starting position of the ray
  * @param ray The ray
  * @param intersectPos The intersection point nearest to pos (if found)
  * @return true, if an intersection point has been found
  */
  virtual bool intersectWithRay(const Vector3d& pos, const Vector3d& ray, Vector3d& intersectPos);

  /** Return the number of graphicsHandles
  * @return The number of primitives
  */
  virtual unsigned int getNumPrim() const
  { return 1; };

  /** Return the number of Vertices
  * @return The number of vertices
  */
  virtual unsigned int getNumVert() const
  { return 256; };

  /** Draws the sphere
  * @param visParams Parameters for Visualization
  */
  virtual void drawObject(const VisualizationParameterSet& visParams);
  virtual void quickDrawObject();

  /** Angular velocity of sphere in last simulation step.*/
  Vector3d lastAngularVelocity;
  
public:
  /** Constructor*/
  Sphere():radius(0) {}

  /** Constructor
  * @param attributes The attributes of the sphere
  */
  Sphere(const AttributeSet& attributes);

  /** Returns a string describing the kind of object
  * @return The kind
  */
  virtual std::string getKind() const {return "sphere";}

  /** Clones the object and its child nodes
  * @return A pointer to a copy of the object
  */
  virtual SimObject* clone() const;

  /** Returns the maximum distance of any point in the subtree to a given point
  * @param base The point to compute the distance to
  * @return The distance
  */
  virtual double getMaxDistanceTo(const Vector3d& base) const;

  /** Updates the local bounding box of the object */
  virtual void updateLocalBoundingBox();

  /** 
   * Creats the physical objects used by the OpenDynamicsEngine (ODE).
   * These are a geometry object for collision detection and a body,
   * if the simulation object is movable.
   */
  void createPhysics();

  /** 
   *  Create the body for this object including mass properties
   */
  virtual void createBody();

  /** 
   *  Create the geometry for this object and insert it in the given space
   *  @param space The space to insert the geometry into
   */
  virtual void createGeometry(dSpaceID space);

  /** 
   *  Compute the mass properties of the object
   *  @param m The mass object where the computed properties are stored
   */
  virtual void computeMassProperties(dMass& m);

  /** Checks if the object is affected by rolling friction 
   * @return True, if affected by rolling friction, otherwise false
   */
  bool isAffectedByRollingFriction() const {return rollingFrictionFlag;};

    /** Applies rolling friction to the object 
   *  @param angle The collision angle
   *  @param rfc The rolling friction coefficient
   */
  void applyRollingFriction(double angle, double rfc);

  /** Applies simple rolling friction to the object.
   *  @param rfc The rolling friction coefficient
   */
  void applySimpleRollingFriction(double rfc);

  /** Determines if object is (or better could be) affected by rolling friction and sets rolling friction flag */
  void determineRollingFrictionFlag();
};

#endif //SPHERE_H_
