/**
 * @file Simulation/SimObject.h
 * 
 * Definition of class SimObject
 *
 * @author <A href="mailto:timlaue@informatik.uni-bremen.de">Tim Laue</A>
 * @author <A href="mailto:kspiess@informatik.uni-bremen.de">Kai Spiess</A>
 */ 

#ifndef SIMOBJECT_H_
#define SIMOBJECT_H_

#include <string>
#include <vector>
#include <list>

#include "../Tools/SimMath.h"
#include "../Tools/SimGeometry.h"
#include "../Parser/ParserUtilities.h"
#include "APIDatatypes.h"

class Actuatorport;
class Actuator;
class SimObject;
class Intersection;
class ErrorManager;
class Simulation;
class GraphicsManager;
class PhysicalObject;
class Joint;
class Surface;
class ShaderProgram;
class Controller3DDrawing;
class InteractiveButton;
class Sensor;


/** A type for a list of objects*/
typedef std::list<SimObject*> ObjectList;

/** The "address" of a sensor port -> should be moved to a better place*/
class SensorPort
{
public:
  std::string name;
  int localPortId;
  Sensor* sensor;

  SensorPort():name("none"),localPortId(-1),sensor(0)
  {}

  SensorPort(const std::string& name, int localPortId, Sensor* sensor):
    name(name), localPortId(localPortId), sensor(sensor)
  {}
};

/**
* @class SimObject
* A class representing an object.
* All other objects are derivated from SimObject. The class itself
* will only be used from grouping other objects.
*/
class SimObject
{
protected:
  /** The position of the object in 3D space*/
  Vector3d position;
  /** The rotation of the object in 3D space*/
  Matrix3d rotation;
  /** The name of the object*/
  std::string name;
  /** The name of the object including the path in the object tree*/
  std::string fullName;
  /** The child nodes*/
  ObjectList childNodes;
  /** A pointer to the root node of the object tree*/
  static SimObject* rootNode;
  /** A pointer to the simulation*/
  static Simulation* simulation;
  /** A pointer to the GraphicsManager object*/
  static GraphicsManager* graphicsManager;
  /** A pointer to the parent node*/
  SimObject* parentNode;
  /** A list of drawings from the controller*/
  std::list<Controller3DDrawing*> controllerDrawings;
  /** A box around this object and all its child nodes*/
  AxisAlignedBoundingBox boundingBox;
  /** The movable ID of this object; if 0, object is static */
  int movableID;
  /** Flag if a light is bound to the object */
  bool boundLight;
  /** Motion Blur method to use if combined technique is used **/
  MotionBlurMode mbMethod;

  /** Intersects the object (and its child nodes) with a ray
  * @param pos The starting position of the ray
  * @param ray The ray
  * @param intersectPos The intersection point nearest to pos (if found)
  * @return true, if an intersection point has been found
  */
  virtual bool intersectWithRay(const Vector3d& pos, const Vector3d& ray, Vector3d& intersectPos) 
  {return false;}

  /** Return the number of graphicsHandles
  * @return The number of primitives
  */
  virtual unsigned int getNumPrim() const
  { return 0; };

  /** Return the number of Vertices
  * @return The number of vertices
  */
  virtual unsigned int getNumVert() const
  { return 0; };

  /** Draws the object
  * @param visParams Parameters for Visualization
  */
  virtual void drawObject(const VisualizationParameterSet& visParams) {};

  /** Draws the object for shadow mapping */
  virtual void quickDrawObject() {};

  /** 
   * Adjusts the movable id. The current movable ID is set. This is used during cloning the simulation object.
   * @return true, if a new movable ID has been created on the stack in the simulation
   */
  virtual bool adjustMovableID();

public:
  /** Constructor*/
  SimObject();
  
  /** Destructor*/
  virtual ~SimObject();
  
  /** Returns a string describing the kind of object
  * @return The kind
  */
  virtual std::string getKind() const {return "compound";}

  /** Returns a bool if a light is bound
  * @return The state
  */
  bool hasBoundLight() const
  { return boundLight; };

  /** Set the flag for a bound light
  */
  void setBoundLight()
  { boundLight = true; };

  /** Set a motion blur method for object and childs (will be used if
  *  combined method is used)
  */
  void setForcedMotionBlurMethod(const MotionBlurMode method);

  /** Return the number of primitives (including child primitives)
  * @return The number of primitives
  */
  unsigned int getNumOfPrimitives() const; 

  /** Return the number of primitives (including child primitives)
  * @return The number of primitives
  */
  unsigned int getNumOfVertices() const;
  
  /** Draws the object and its child nodes
  * @param visParams Parameters for Visualization
  */
  void draw(const VisualizationParameterSet& visParams);
  void draw(const VisualizationParameterSet& visParams, bool skipFirstChild);

  /** Draws the object with the simplest method (but really fast) */
  void quickDraw();

  /** Draws all drawings from the controller connected to this object
  * @param visParams Parameters for Visualization
  */
  void drawControllerDrawings(const VisualizationParameterSet& visParams);

  /** Attaches a drawing to the object. This drawing may
  *   be drawn together with the object (depending on current
  *   visualization parameters).
  * @param drawing The drawing
  */
  void attachDrawing(Controller3DDrawing* drawing);

  /** Removes a drawing from the object. 
  * @param drawing The drawing
  */
  void removeDrawing(Controller3DDrawing* drawing);

  /** Replaces the current surface with a new one
  *   (only needed by PhysicalObject)
  * @param newSurface A pointer to the new surface
  */
  virtual void replaceSurface(Surface* newSurface);

  /** Replaces the current surface with a new one,
  *   if it is equal to a given surface
  *   (only needed by PhysicalObject)
  * @param oldSurface A pointer to a surface to compare with
  * @param newSurface A pointer to the new surface
  */
  virtual void replaceSurface(Surface* oldSurface, Surface* newSurface);

  /** Replaces the current shader program with a new one
  *   (only needed by PhysicalObject)
  * @param newShaderProgram A pointer to the new shader program
  */
  virtual void replaceShaderProgram(ShaderProgram* newShaderProgram);

  /** Replaces the current shader program with a new one,
  *   if it is equal to a given shader program
  *   (only needed by PhysicalObject)
  * @param oldShaderProgram A pointer to a shader program to compare with
  * @param newShaderProgram A pointer to the new shader program
  */
  virtual void replaceShaderProgram(ShaderProgram* oldShaderProgram,
                                    ShaderProgram* newShaderProgram);
  
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
  
  /** Translates the object and its children in 3D space
  * @param translation A vector describing the translation
  */
  virtual void translate(const Vector3d& translation);

  /** Sets the new position of the object (children will be translated, too)
  * @param pos The new position
  */
  virtual void setPosition(const Vector3d& pos);

  /** Returns the position of the object
  * @return The position
  */
  Vector3d getPosition() const {return position;}

  /** Sets the new rotation of the object (children will be rotated, too)
  * @param pos The new rotation
  */
  virtual void setRotation(const Matrix3d& rot);

  /** Rotates the object and its children in 3D space
  * @param rotation The rotation in matrix form
  * @param origin The point to rotate around
  */
  virtual void rotate(const Matrix3d& rotation, 
                      const Vector3d& origin);

  /** Rotates the object and its children in 3D space
  * @param angles The rotation in angles
  * @param origin The point to rotate around
  */
  void rotate(const Vector3d& angles, const Vector3d& origin);

  /** Rotates the object around a certain axis
  * @param angles The rotation angle
  * @param origin The axis to rotate around
  */
  void rotateAroundAxis(double angle, Vector3d axis);

  /** Returns the rotation matrix
  * @return The matrix
  */
  Matrix3d getRotation() const {return rotation;}
  
  /** Adds a child node
  * @param child A pointer to a new child node
  * @param move Flag: translate and rotate child, if true. Keep child coordinates, otherwise
  */
  virtual void addChildNode(SimObject* child, bool move);

  /** Returns a pointer to the child nodes (hacking interface ;-)
  * @return A pointer to a list of child nodes
  */
  ObjectList* getPointerToChildNodes() {return &childNodes;}

  /** Returns the number of child nodes
  * @return The number of child nodes
  */
  unsigned int getNumberOfChildNodes() const {return childNodes.size();}

  /** Returns a pointer to a child node (hacking interface ;-)
  * @param num The number of the child node
  * @return A pointer to the child node
  */
  SimObject* getChildNode(int num) const;

  /** Sets the root node of the object tree
  * @param node A pointer to the root node
  */
  static void setRootNode(SimObject* node) {SimObject::rootNode = node;}

  /** Sets the pointer to the simulation object
  * @param node A pointer to the simulation object
  */
  static void setSimulation(Simulation* sim) {SimObject::simulation = sim;}

  /** Sets the pointer to the GraphicsManager object
  * @param node A pointer to the GraphicsManager object
  */
  static void setGraphicsManager(GraphicsManager* graphicsManager) {SimObject::graphicsManager = graphicsManager;}

  /** Sets the parent node of an object
  * @param node A pointer to the parent node
  */
  void setParentNode(SimObject* node) {parentNode = node;}

  /** Returns a pointer to the parent node
  * @return A pointer to the parent node
  */
  SimObject* getParentNode() const { return parentNode;}

  /** Searches for another object in the object's subtree
  * @param object A pointer to the searched object, if found
  * @param searchName The name of the object to search for
  * @param useFullName Compare full name of object, if true.
  */
  void findObject(SimObject*& object, const std::string& searchName, bool useFullName = true);

  /** Searches for another object in the object's subtree
  * @param object A pointer to the searched object, if found
  * @param partsOfName An array of parts of the name of the object, 
   *                   in which they must appear in the given sequence.
  */
  void findObject(SimObject*& object, const std::vector<std::string>& partsOfName);
  
  /** Clones the object and its child nodes
  * @return A pointer to a copy of the object
  */
  virtual SimObject* clone() const;

  /** Copies the members of another object
  * @param other The other object
  */
  virtual void copyStandardMembers(const SimObject* other);

  /** Adds the object to some internal lists
  * @param sensorPortList A list of all sensor ports in the scene
  * @param actuatorPortList A list of all actuator ports in the scene
  * @param actuatorList A list of all actuators in the scene
  */
  virtual void addToLists(std::vector<SensorPort>& sensorPortList,
                          std::vector<Actuatorport*>& actuatorPortList,
                          std::vector<Actuator*>& actuatorList) {};

  /** Adds descriptions of the object and its children to a list
  * @param objectDescriptionTree The list of descriptions
  * @param The depth of the object in the object tree
  */
  virtual void addToDescriptions(std::vector<ObjectDescription>& objectDescriptionTree,
    int depth);
  
  /** Intersects the object and its child nodes with a ray
  * @param intersections A list to insert intersection points into
  * @param pos The starting position of the ray
  * @param ray The ray
  * @return true, if any intersection occured
  */
  bool intersect(std::vector<Intersection*>& intersections, 
                 const Vector3d& pos, const Vector3d& ray);

  /** Returns the maximum distance of any point in the subtree to a given point
  * @param base The point to compute the distance to
  * @return The distance
  */
  virtual double getMaxDistanceTo(const Vector3d& base) const;

  /** Expands a bounding box to completely include the object
  * @param box The bounding box
  */
  void expandAxisAlignedBoundingBox(AxisAlignedBoundingBox& box);

  /** Updates the local bounding box of the object */
  virtual void updateLocalBoundingBox();
  
  /** Checks if the object is movable or part of a movable object or
  *   part of an interactive button
  *   (needed for interaction)
  * @param object A pointer to the object (if one has been found)
  * @param testClickable Flag: Also test for interactive buttons, if true
  * @return true, if the object is part of a movable object or an interactive button
  */
  virtual bool isMovableOrClickable(SimObject*& object, 
                                    bool testClickable);

  /** Inverts the colors of the object and all subobjects
  *   Used for displaying selections.
  */
  virtual void invertColors();

  /** Informs the object about being clicked by the user*/
  virtual void hasBeenClicked() {};

  /** Informs the object about being released by the user*/
  virtual void hasBeenReleased() {};

  /** Notification for internal parsing state handling
  * @param name The name of the finished element  */ 
  virtual void finishedParsingCurrentElement(const std::string& name) {}

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

  /** Do internal computations after parsing, e.g. initialize 
   *  some members.
   */
  virtual void postProcessAfterParsing() {}

  /** 
   * Creats the physical objects used by the OpenDynamicsEngine (ODE).
   * These are a geometry object for collision detection and a body,
   * if the simulation object is movable.
   */
  virtual void createPhysics(){};

  /** 
   * Creats the physical joints used by the OpenDynamicsEngine (ODE).
   * This has to be done after the physical objects (geometry and body)
   * have been created.
   */
  virtual void createJointPhysics(){};

  /** 
   * Creats the physical motors for the joints used by the OpenDynamicsEngine (ODE).
   * This has to be done after the physical joints have been created.
   */
  virtual void createMotorPhysics(){};

  /**
   * Checks if an object is movable.
   * @return true, if object is movable
   */
  bool isMovable() const;

  /**
   * Sets the movable ID for this object
   * @param newMovableID The movable ID
   */
  void setMovableID(const int& newMovableID){movableID = newMovableID;};

  /** 
   * Casts the SimObject pointer to a PhysicalObject pointer. This is used to prevent dynamic downcasts.
   * Has to be overloaded by the appropriate class.
   * @ return A pointer to the physical object.
   */
  virtual PhysicalObject* castToPhysicalObject(){return 0;};

  /** 
   * Casts the SimObject pointer to a Joint pointer. This is used to prevent dynamic downcasts.
   * Has to be overloaded by the appropriate class.
   * @ return A pointer to the physical object.
   */
  virtual Joint* castToJoint(){return 0;};

  /**
   * Returns a pointer to the interactive button if the sensor is one. This is used to prevent dynamic downcasts
   * from sensor objects to interactive button objects. Has to be overloaded by the appropriate subclass
   * @ return A pointer to the interactive button
   */
  virtual InteractiveButton* castToInteractiveButton(){return NULL;};

  /**
   * Writes back the results from the physical simulation.
   * Sets position and rotation from body/geometry to simulation object.
   */
  virtual void writeBackPhysicalResult(const bool& store_last_results){};

  /**
   * Writes back the results from the physical simulation. (and saves up to 12 steps)
   * Sets position and rotation from body/geometry to simulation object.
   */
  virtual void storePhysicalResult(){};

  /**
   * Returns the movable ID of the object; if 0, object is static
   * @return The movable ID
   */
  int getMovableID() const {return movableID;}

  /**
   *  Set the flag indicating if the object can collide with other objects
   *  @param value true if object can collide
   */
  virtual void setCollideBit(bool value){};

  /**
   * Changes the ability of this object and all childs to collide
   * @param collide Flag: true if object can collide, otherwise false
   */
  virtual void changeAbilityToCollide(bool collide);

  /** Translates the object and its children in 3D space during drag and drop action
  * @param translation A vector describing the translation
  */
  virtual void translateDnD(const Vector3d& translation);
  
  /** Rotates the object and its children in 3D space during drag and drop action
  * @param rotation The rotation in matrix form
  * @param origin The point to rotate around
  */
  virtual void rotateDnD(const Matrix3d& rotation, const Vector3d& origin);
  
  /** Rotates the object around a certain axis during drag and drop action
  * @param angles The rotation angle
  * @param origin The axis to rotate around
  */
  void rotateAroundAxisDnD(double angle, Vector3d axis);

};

#endif //SIMOBJECT_H_
