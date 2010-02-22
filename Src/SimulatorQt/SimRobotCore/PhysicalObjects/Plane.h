/**
 * @file Plane.h
 *
 * Definition of class Plane
 *
 * @author <A href="mailto:kspiess@informatik.uni-bremen.de">Kai Spiess</A>
 */

#ifndef PLANE_H_
#define PLANE_H_


/**
 * @class Plane
 *
 * A class representing a plane in 3D space
 */
class Plane
{
protected:
  /** The extension factor for drawing the plane */
  static const int DRAWING_EXTENSION = 100;

  /** The parameter for the plane standard equation which has the form: a*x + b*y + c*z = d */
  double a ,b ,c ,d;
  /** The name of the object*/
  std::string name;
  /** The name of the object including the path in the object tree*/
  std::string fullName;
  /** A pointer to a surface object*/
  const Surface* surface; 
  /** Flag: object is invisible in scene, if true*/
  bool invisible;

  /** Precomputed drawing parameters*/
  Vector3d drawingPoint1, drawingPoint2, drawingPoint3, drawingPoint4, drawingNormal;

  /** Precomputes drawing parameters for the plane from its standard equation */
  void precomputeDrawingParameter();

  /** 
   * Projects a given point on the plane.
   * @param point the point which has to be projected
   * @return The projected point on the plane
   */
  Vector3d projectPointOnPlane(const Vector3d &point);

  /** 
   * Checks, if the paramters for the plane equation are valid. False if the paramters a, b and c are 0.
   * @return True, if valid else false
   */
  bool isPlaneEquationValid();

  /** The index of the material in the friction coefficient table */
  int materialIndex;
  /** The geometry of the object*/
  dGeomID geometry;

public:
  /** Constructor*/
  Plane();
  
  /** Destructor*/
  virtual ~Plane() {}

  /** Constructor
  * @param attributes The attributes of the plane
  */
  Plane(const AttributeSet& attributes);

  /** Returns a string describing the kind of object
  * @return The kind
  */
  std::string getKind() const {return "plane";}

  /** Sets the name of the object
  * @param newName The name
  */
  void setName(const std::string& newName) {name = newName;}

  /** Sets the full name of the object (name+path)
  * (The member name will not be set)
  * @param newFullName The full name
  */
  void setFullName(const std::string& newFullName) {fullName = newFullName;}

  /** Returns the name
  * @return The name
  */
  std::string getName() const {return name;}

  /** Returns the full name
  * @return The full name
  */
  std::string getFullName() const {return fullName;}

  /** Return the number of graphicsHandles
  * @return The number of primitives
  */
  virtual unsigned int getNumPrim() const
  { return 1; };

  /** Return the number of Vertices
  * @return The number of vertices
  */
  virtual unsigned int getNumVert() const
  { return 4; };

  /** Draws the plane
  * @param visParams Parameters for Visualization
  */
  void drawObject(const VisualizationParameterSet& visParams);
  void quickDrawObject();

  /**
   * Creats the physical objects used by the OpenDynamicsEngine (ODE).
   * These are a geometry object for collision detection and a body,
   * if the simulation object is movable.
   */
  void createPhysics(const dSpaceID& collisionSpace);

 /**
  * Parses the attributes of a simulated object which are
  * defined in seperate elements
  * @param attributes The attributes of the element
  * @param line The line in the scene description
  * @param column The column in the scene description
  * @param errorManager An object managing errors
  * @return true, if any element has been parsed
  */
  void parseAppearance(const AttributeSet& attributes,
                       int line, int column, 
                       ErrorManager* errorManager,
                       const Simulation& simulation);

 /**
  * Parses the attributes of the material of the object
  * @param attributes The attributes of the element
  * @param line The line in the scene description
  * @param column The column in the scene description
  * @param errorManager An object managing errors
  * @param simulation The simulation
  */
  void parseMaterial(const AttributeSet& attributes,
                     int line, int column, 
                     ErrorManager* errorManager,
                     const Simulation& simulation);

  /** 
   * Returns the index of the objetcs material in the friction coefficient table
   * @return The material index
   */
  int getMaterialIndex(){return materialIndex;};
};

#endif //PLANE_H_
