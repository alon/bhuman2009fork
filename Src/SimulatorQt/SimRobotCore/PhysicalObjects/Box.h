/**
 * @file Box.h
 * 
 * Definition of class Box
 *
 * @author <A href="mailto:kspiess@informatik.uni-bremen.de">Kai Spiess</A>
 * @author <A href="mailto:timlaue@informatik.uni-bremen.de">Tim Laue</A>
 */ 

#ifndef BOX_H_
#define BOX_H_

#include "PhysicalObject.h"


/**
 * @class Box
 *
 * A class representing a box in 3D space.
 */
class Box : public PhysicalObject
{
protected:
  /** The length of the box*/
  double length;
  /** The width of the box*/
  double width;
  /** The height of the box*/
  double height;
  /** The corners of the box*/
  Vector3d corners[8];
  /** A handle for drawing the box*/
  GraphicsPrimitiveHandle graphicsHandle;

  /** 
   * Intersects the box and its children with a ray
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
  { return 24; };

  /** 
   * Draws the box
   * @param visParams Parameters for Visualization
   */
  virtual void drawObject(const VisualizationParameterSet& visParams);
  virtual void quickDrawObject();

  /** Calculates the corners of the box relative to its position using the dimensions (length, width, height) of it*/
  void calculateCorners();

public:
  /** Empty default Constructor*/
  Box() {};

  /** Constructor
  * @param attributes The attributes of the box
  */
  Box(const AttributeSet& attributes);

  /** Returns a string describing the kind of object
  * @return The kind
  */
  virtual std::string getKind() const {return "box";}
  
  /** 
   * Clones the object and its child nodes.
   * @return A pointer to a copy of the object
   */
  virtual SimObject* clone() const;

  /** 
   * Returns the maximum distance of any point in the subtree to a given point
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

  /** 
   * Sets the dimensions (length, width, height) of the box object
   * @param length The length of the box
   * @param width The width of the box
   * @param height The height of the box
   */
  void setDimensions(double newLength, double newWidth, double newHeight);

  /**
   * Creates the graphical object for the box.
   */
  void createGraphics();
};

#endif //BOX_H_
