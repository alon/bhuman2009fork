/**
 * @file ComplexShape.h
 *
 * Definition of class ComplexShape
 *
 * @author <A href="mailto:timlaue@informatik.uni-bremen.de">Tim Laue</A>
 * @author <A href="mailto:kspiess@informatik.uni-bremen.de">Kai Spiess</A>
 */

#ifndef COMPLEX_SHAPE_H_
#define COMPLEX_SHAPE_H_

#include "PhysicalObject.h"
#include <Tools/SimGraphics.h>

class TriMeshFace
{
public:
  int point[3];
};

class TriangleMesh
{
public:
  std::vector<Vector3d> vertices;
  std::vector<TriMeshFace> triangles;
  int numOfPoints;
  int numOfTriangles;

  double findMinProjectionOnVector(Vector3d vec)
  {
    double minResult = vec.vecmul(vertices[0]);
    double tempResult;
    for(unsigned int i=1; i < vertices.size(); i++)
    {
      tempResult = vec.vecmul(vertices[i]);

      if(tempResult < minResult)
      {
        minResult = tempResult;
      }
    }
    return minResult;
  };

  double findMaxProjectionOnVector(Vector3d vec)
  {
    double maxResult = vec.vecmul(vertices[0]);
    double tempResult;
    for(unsigned int i=1; i < vertices.size(); i++)
    {
      tempResult = vec.vecmul(vertices[i]);
      if(tempResult > maxResult)
      {
        maxResult = tempResult;
      }
    }
    return maxResult;
  };
};

/**
* @class ComplexShape
*
* A class representing a complex shape in 3D space.
*/
class ComplexShape : public PhysicalObject
{
protected:
  /** A list containing all the numbers of the primitives assigned to an object*/
  std::list<GraphicsPrimitiveHandle> graphicsPrimitives;

  /** Return the number of graphicsHandles
  * @return The number of primitives
  */
  virtual unsigned int getNumPrim() const;

  /** Return the number of Vertices
  * @return The number of vertices
  */
  virtual unsigned int getNumVert() const;

  /** Draws the object
  * @param visParams Parameters for Visualization
  */
  virtual void drawObject(const VisualizationParameterSet& visParams);
  virtual void quickDrawObject();

  /** Intersects the complex shape and its children with a ray
  * @param pos The starting position of the ray
  * @param ray The ray
  * @param intersectPos The intersection point nearest to pos (if found)
  * @return true, if an intersection point has been found
  */
  virtual bool intersectWithRay(const Vector3d& pos, const Vector3d& ray, Vector3d& intersectPos);


  /* PHYSICS */
  /** A list of simple physical objects for the phyical representation of the complex shape */
  std::vector<PhysicalObject*> physicalRepresentation;
  /** A list of al geometry transform objects for compound physical representation */
  std::vector<dGeomID> geometryTransforms;

  //triangle meshes for ODE
  /** An array of all vertices for a trianglemesh of this complex shape */
  dVector3 *physicsVertices;
  /** An array of all triangle indices for a trianglemesh of this complex shape */
  dTriIndex *physicsIndices;
  /** The number of vertices of the trianglemesh */
  int physVertexCount;
  /** The number of indices for the triangles of the trianglemesh */
  int physIndexCount;
  
  //WORK IN PROGRESS
  dVector3 *physicsNormals;
  int physNormalsCount;

  /** The position of the physical body relative to the position of the complex shape */
  Vector3d bodyPositionOffset;
  /** The rotation of the physical body relative to the rotation of the complex shape */
  Matrix3d bodyRotationOffset;

  /** A flag indicating if the physical representation of the complex shape is currently parsed */
  bool parsingPhysicalRepresentation;
  /** A flag indicating if a triagle mesh should be used as geometry */
  bool useTriangleMesh;
  /** A flag indicating if an object oriented bounding box should be used as geometry */
  bool useOBB;
  /** A flag indicating if an axis aligned bounding box should be used as geometry */
  bool useABB;

  /** The initial rotation of the object when physics is created (needed by current version of writeBackPhysicalResul)*/
  Matrix3d initialRotation;
  /** The initial rotation of the body when physics is created (needed by current version of writeBackPhysicalResul)*/
  Matrix3d initialBodyRotation;
  // work in progress
  void createAABB();
  /**
   * Creates a convex hull for the complex shape
   * @param convexHull The triangle mesh object where the convex hull is stored in
   */
  void createConvexHull(TriangleMesh* convexHull);
  /** Creates the object oriented bounding box for the complex shape */
  void createOBB();
  /** Creates the axis aligned bounding box for the complex shape */
  void createABB();

  /** Prepares the triangles, indices and counter for physics, processing the graphical representation */
  void prepareTrianglesForPhysics();

  /** Flag if normals should be averaged on topological connectivity (creates a smoother shading) */
  bool averageNormals;
  /** Flag if surface vertices should be ordered other way round */
  bool flipNormal;
  /** Flag if the primitives could be combined to increase rendering speed */
  bool combinePrimitives;
  /** Temporary variable during object construction. */
  Vector3d geometryOffset;

public:
  /** Constructor*/
  ComplexShape();

  /** Constructor
  * @param attributes The attributes of the complex shape
  */
  ComplexShape(const AttributeSet& attributes);

  /** Destructor*/
  ~ComplexShape();

  /** Returns a string describing the kind of object
  * @return The kind
  */
  virtual std::string getKind() const {return "complex-shape";}

  /** finish the parsed shape */
  void finishShape();

  /**
  * Parses the attributes of a simulated object which are
  * defined in seperate elements
  * @param name The name of the attribute element
  * @param attributes The attributes of the element
  * @param line The line in the scene description
  * @param column The column in the scene description
  * @param errorManager An object managing errors
  * @return true, if any element has been parsed
  */
  virtual bool parseAttributeElements(const std::string& name,
                                      const AttributeSet& attributes,
                                      int line, int column,
                                      ErrorManager* errorManager);

  /** Notification for internal parsing state handling
  * @param name The name of the finished element
  */
  virtual void finishedParsingCurrentElement(const std::string& name);

  /** Do internal computations after parsing, e.g. initialize 
   *  some members.
   */
  virtual void postProcessAfterParsing();

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

  /** Create the body for this object including mass properties */
  virtual void createBody();

  /** 
   *  Compute the mass properties of the object
   *  @param m The mass object where the computed properties are stored
   */
  virtual void computeMassProperties(dMass& m);
  /** 
   *  Create the geometry for this object and insert it in the given space
   *  @param space The space to insert the geometry into
   */
  virtual void createGeometry(dSpaceID space);

 /** Adds a child node. 
  * Adds physical objects to the physical representation list of the complex shape 
  * if parsingPhysicalRepresentation is true
  * @param child A pointer to a new child node
  * @param move Flag: translate and rotate child, if true. Keep child coordinates, otherwise
  */
  void addChildNode(SimObject* child, bool move);

  /**
   * Writes back the results from the physical simulation.
   * Sets position and rotation from body/geometry to simulation object.
   */
  virtual void writeBackPhysicalResult(const bool& store_last_results);

  /** 
   * Disables physics for this object
   * @param disableGeometry Flag, if geometry should be disabled
   */
  virtual void disablePhysics(bool disableGeometry);

  /** Enables physics for this object */
  virtual void enablePhysics();

  /** Translates the object and its children in 3D space
  * @param translation A vector describing the translation
  */
  virtual void translate(const Vector3d& translation);

  /** Rotates the object and its children in 3D space
  * @param rotation The rotation in matrix form
  * @param origin The point to rotate around
  */
  virtual void rotate(const Matrix3d& rotation, 
                      const Vector3d& origin);
  /** 
   * Returns the objects of the physical representation of the complex shape
   * @return A pointer to the physical representation objects
   */
  std::vector<PhysicalObject*>* getPhysicalRepresentations(){return &physicalRepresentation;};
  /**
   * Sets the body of the object
   * @param body The body
   */
  virtual void setBody(dBodyID body);
  /**
   * Sets the translational offset of the physical body to the graphical representation 
   * (used if this object is part of a compound object)
   * @param bodyPosOffset The translational offset
   */
  virtual void setBodyPositionOffset(const Vector3d& bodyPosOffset){bodyPositionOffset = bodyPosOffset;};
  /**
   * Returns the translational offset of the physical body to the graphical representation
   * (used if this object is part of a compound object)
   * @return The translational offset
   */
  const Vector3d& getBodyPositionOffset()const {return bodyPositionOffset;}
  /**
   * Sets the rotational offset of the physical body to the graphical representation 
   * (used if this object is part of a compound object)
   * @param bodyRotOffset The rotational offset
   */
  virtual void setBodyRotationOffset(const Matrix3d& bodyRotOffset){bodyRotationOffset = bodyRotOffset;};
  /**
   * Returns the rotational offset of the physical body to the graphical representation 
   * (used if this object is part of a compound object)
   * @return The rotational offset
   */
  virtual const Matrix3d& setBodyRotationOffset() const {return bodyRotationOffset;};
  /** Sets the new position of the object (children will be translated, too)
   * @param pos The new position
   */
  virtual void setPosition(const Vector3d& pos);
  /** Sets the new rotation of the object (children will be rotated, too)
   * @param pos The new rotation
   */
  virtual void setRotation(const Matrix3d& rot);
  /** 
   * Computes the total mass of the compound physical representation
   * @return the total mass
   */
  double computeTotalMass();
  /** 
   * Computes the combined center of mass of the compound physical representation
   * @return the combined center of mass
   */
  Vector3d computeCenterOfMass();
  /** 
   *  Sets the flag if a triabgle mesh sould be used for geometry
   *  @param newValue Flag: use triangle mesh if true
   */
  void setUseTriangleMesh(bool newValue){useTriangleMesh = newValue;};
  /**
   *  Returns true if triangle mesh is used for geoemtry otherwise false
   *  @return true if triangle mesh is used
   */
  bool getUseTriangleMesh(){return useTriangleMesh;};
  /**
   *  Set the flag indicating if the object can collide with other objects
   *  @param value true if object can collide
   */
  virtual void setCollideBit(bool value);
  /** Sets the collide bitfield for the goemtry depending on the collision bit of the object*/
  void setCollideBitfield();
  /** Translates the object and its children in 3D space during drag and drop action
  * @param translation A vector describing the translation
  */
  virtual void translateDnD(const Vector3d& translation);
  /** Rotates the object and its children in 3D space during drag and drop action
  * @param rotation The rotation in matrix form
  * @param origin The point to rotate around
  */
  virtual void rotateDnD(const Matrix3d& rot, const Vector3d& origin);
  /** Sets a pointer to the compound object this complex shape object belongs to
   * @parame compoundObject the pointer to the compound object
   */
  void setCompoundPhysicalObject(CompoundPhysicalObject* compoundObject);

};


#endif //COMPLEX_SHAPE_H_
