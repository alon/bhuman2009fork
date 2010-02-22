/**
 * @file Cylinder.h
 * 
 * Definition of class Cylinder
 *
 * @author <A href="mailto:timlaue@informatik.uni-bremen.de">Tim Laue</A>
 */ 

#ifndef CYLINDER_H_
#define CYLINDER_H_

#include "PhysicalObject.h"
#include <Tools/SimGraphics.h>


/**
* @class Cylinder
*
* A class representing a cylinder in 3D space.
*/
class Cylinder : public PhysicalObject
{
protected:
  /** The radius of the cylinder*/
  double radius;
  /** The height of the cylinder*/
  double height;
  /** Flag indicating whether the cylinder has caps or not*/
  bool capped;
  /** The corners of a box around the cylinder*/
  Vector3d boxCorners[8];
  
  /** Intersects the cylinder and its children with a ray
  * @param pos The starting position of the ray
  * @param ray The ray
  * @param intersectPos The intersection point nearest to pos (if found)
  * @return true, if an intersection point has been found
  */
  virtual bool intersectWithRay(const Vector3d& pos, const Vector3d& ray, Vector3d& intersectPos);

  /** Calculates the corners of the box around the cylinder*/
  virtual void calculateCorners();

  /** Return the number of graphicsHandles
  * @return The number of primitives
  */
  virtual unsigned int getNumPrim() const
  { return 1; };

  /** Return the number of Vertices
  * @return The number of vertices
  */
  virtual unsigned int getNumVert() const
  { return capped ? 288 : 66; };

  /** Draws the cylinder 
  * @param visParams Parameters for Visualization
  */
  virtual void drawObject(const VisualizationParameterSet& visParams);
  virtual void quickDrawObject();

  /** A handle for drawing the cylinder*/
  GraphicsPrimitiveHandle graphicsHandle;

public:
  /** Empty standard Constructor*/
  Cylinder() {}

  /** Constructor
  * @param attributes The attributes of the cylinder
  */
  Cylinder(const AttributeSet& attributes);

  /** Returns a string describing the kind of object
  * @return The kind
  */
  virtual std::string getKind() const {return "cylinder";}

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
  virtual void createPhysics();

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
  virtual bool isAffectedByRollingFriction() const {return rollingFrictionFlag;};

  /** Applies rolling friction to the object 
   *  @param angle The collision angle
   *  @param rfc The rolling friction coefficient
   */
  virtual void applyRollingFriction(double angle, double rfc);

  /** Applies simple rolling friction to the object.
   *  @param rfc The rolling friction coefficient
   */
  virtual void applySimpleRollingFriction(double rfc);

  /** Determines if object is (or better could be) affected by rolling friction and sets rolling friction flag */
  virtual void determineRollingFrictionFlag();

};

#endif //CYLINDER_H_
