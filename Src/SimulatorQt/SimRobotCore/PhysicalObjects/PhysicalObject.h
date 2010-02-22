/**
 * @file PhysicalObject.h
 * 
 * Definition of class PhysicalObject
 *
 * @author <A href="mailto:kspiess@informatik.uni-bremen.de">Kai Spiess</A>
 * @author <A href="mailto:timlaue@informatik.uni-bremen.de">Tim Laue</A>
 */ 

#ifndef PHYSICALOBJECT_H_
#define PHYSICALOBJECT_H_

#include <Simulation/SimObject.h>
#include <Tools/Surface.h>
#include <OpenGL/ShaderInterface.h>
#include <Parser/ParserUtilities.h>
#include <PhysicalObjects/CompoundPhysicalObject.h>
#include <ode/ode.h>


/**
 * @class PhysicalObject
 *
 * A class representing a simple solid object with its body and geometry.
 */
class PhysicalObject : public SimObject
{
protected:
  /** A pointer to a surface object*/
  Surface* surface; 
  /** A pointer to a shader program */
  ShaderProgram* shaderProgram;
  /** Flag: object is invisible in scene, if true*/
  bool invisible;
  /** Flag: Invert colors, if true*/
  bool inverted;
  /** The body of the object*/
  dBodyID body;
  /** The geometry of the object*/
  dGeomID geometry;
  /** The mass of the object*/
  double mass;
  /** Physical world this object is belonging to */
  dWorldID* pWorldObject;
  /** Collsision space this object is inserted in */
  dSpaceID* pCollisionSpace;
  /** Flag, if the geometry of the object is transfomed relative to the body */
  bool transformedGeometry;
  /** A pointer to the compound object this object is belonging to*/
  CompoundPhysicalObject* compoundPhysicalObject;
  /** Flag, if physics or a part of it is disabled */
  bool physicsDisabled;
  /** The name of the material of this object*/ 
  std::string material;
  /** The index of the material in the friction coefficient table */
  int materialIndex;
  /** A flag indicating if object can collide with other ohysical objects (default: true)*/
  bool collideBit;
  /** A flag indicating if object is (or better could be) affected by rolling friction */
  bool rollingFrictionFlag;

  void parseMass(const AttributeSet& attributes);
  void parseFriction(const AttributeSet& attributes);
  void parseCenterOfMass(const AttributeSet& attributes);
  void parseAppearance(const AttributeSet& attributes,
                       int line, int column, 
                       ErrorManager* errorManager);
  void parseShaderProgram(const AttributeSet& attributes,
                          int line, int column, 
                          ErrorManager* errorManager);
 /**
  * Parses the attributes of the material of the object
  * @param attributes The attributes of the element
  * @param line The line in the scene description
  * @param column The column in the scene description
  * @param errorManager An object managing errors
  */
  void parseMaterial(const AttributeSet& attributes,
                     int line, int column, 
                     ErrorManager* errorManager);
public: 
  /** Constructor*/
  PhysicalObject();

  /** Past positions and rotations (needed from motion blur) */
  Vector3d previous_positions[12];
  Matrix3d previous_rotations[12];
  int cycle_order_offset;

  /** Returns a string describing the kind of object
  * @return The kind
  */
  virtual std::string getKind() const 
  {return "physicalObject";}

  /**
   * Writes back the results from the physical simulation.
   * Sets position and rotation from body/geometry to simulation object.
   * @param store_last_results If true the last 12 results will be stored
   */
  virtual void writeBackPhysicalResult(const bool& store_last_results);

  /** Sets the graphical surface of the object
  * @param surface The surface
  */
  void setSurface(Surface* surface) 
  {this->surface = surface;}

  /** Return the graphical surface of the object
  */
  Surface* getSurface() const
  {return surface;}

  /** Replaces the current surface with a new one
  * @param newSurface A pointer to the new surface
  */
  virtual void replaceSurface(Surface* newSurface);

  /** Replaces the current surface with a new one,
  *   if it is equal to a given surface
  * @param oldSurface A pointer to a surface to compare with
  * @param newSurface A pointer to the new surface
  */
  virtual void replaceSurface(Surface* oldSurface, Surface* newSurface);

  /**
   * Sets the geometry of the object
   * @param geometry The geometry
   */
  void setGeometry(dGeomID geometry);

  /**
   * Sets the body of the object
   * @param body The body
   */
  virtual void setBody(dBodyID body);

  /** 
   * Gets the mass of the object
   * @return The mass
   */
  double getMass() const {return mass;}

  /** 
   * Gets a pointer to the geometry of the object
   * @return Pointer to the geometry
   */
  dGeomID* getGeometry() {return &geometry;}

  /** 
   * Gets a pointer to the body of the object
   * @return Pointer to the body
   */
  dBodyID* getBody() {return &body;}

  /** 
   * Sets the mass of the object
   * @param mass The mass of the object
   */
  void setMass(double mass){this->mass = mass;}

  /**
   * Gets a pointer to the physical world this object is belonging to
   * @return Pointer to physical world
   */
  dWorldID* getPointerToPhysicalWorld(){return pWorldObject;}

  /**
   * Sets the pointer to the physical world this object is belonging to
   * @param ptw Pointer to the physical world
   */
  void setPointerToPhysicalWorld(dWorldID* ptw){this->pWorldObject = ptw;}

  /**
   * Gets a pointer to the collision space this object is inserted in
   * @return Pointer to collsion space
   */
  dSpaceID* getPointerToCollisionSpace(){return pCollisionSpace;}

  /**
   * Sets a pointer to the collision space this object is inserted in
   * @param ptcs Pointer to collsion space
   */
  void setPointerToCollisionSpace(dSpaceID* ptcs){this->pCollisionSpace = ptcs;}

  /** Inverts the colors of the object and all subobjects
  *   Used for displaying selections.
  */
  virtual void invertColors();

  /** 
   * Casts the SimObject pointer to a PhysicalObject pointer. This is used to prevent dynamic downcasts.
   * Has to be overloaded by the appropriate class.
   * @ return A pointer to the physical object.
   */
  PhysicalObject* castToPhysicalObject(){return this;}

  /** Copies the members of another object
  * @param other The other object
  */
  virtual void copyStandardMembers(const SimObject* other);

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
  virtual void postProcessAfterParsing();

  /** 
   *  Create the body for this object including mass properties
   */
  virtual void createBody(){};
  /** 
   *  Create the geometry for this object and insert it in the given space
   *  @param space The space to insert the geometry into
   */
  virtual void createGeometry(dSpaceID space) = 0;
  /** 
   *  Compute the mass properties of the object
   *  @param m The mass object where the computed properties are stored
   */
  virtual void computeMassProperties(dMass& m) = 0;
  /**
   * Returns true, if the geometry is transformed relative to the belonging body 
   * (e.g. by compound physical objects)
   * @return true, if geometry is transformed
   */
  bool isGeometryTransformed() const {return transformedGeometry;}
  /**
   * Sets the value of transformedGeometry. True, if the geometry is transformed 
   * relative to the belonging body (e.g. by compound physical objects)
   * @param transformGeom true, if geometry is transformed 
   */
  void setTransformedGeometry(bool transformGeom){transformedGeometry = transformGeom;};

  /**
   * Returns the compound object this physical object is belonging to
   * @return pointer to compound object
   */
  CompoundPhysicalObject* getCompoundPhysicalObject(){return compoundPhysicalObject;};
  /** 
   * Sets a pointer to the compound object this physical object is belonging to
   * @param compoundObject pointer to the compound object
   */
  void setCompoundPhysicalObject(CompoundPhysicalObject* compoundObject){compoundPhysicalObject = compoundObject;};

  /**
   * Rotates the body of the physical object
   * @param rotationMatrix The rotation matrix (as an ODE datatype)
   */
  virtual void rotateBody(dMatrix3 rotationMatrix);

  /** 
   * Disables physics for this object
   * @param disableGeometry Flag, if geometry should be disabled
   */
  virtual void disablePhysics(bool disableGeometry);

  /** Enables physics for this object */
  virtual void enablePhysics();

  /** 
   * Returns the index of the objetcs material in the friction coefficient table
   * @return The material index
   */
  int getMaterialIndex() const {return materialIndex;};
  /**
   * Sets the material for this object
   * @param newMaterial The material
   */
  void setMaterial(const std::string& newMaterial){material = newMaterial;};
  /**
   * Sets the material index in the friction coefficient table for the material of this object
   * @param newMaterialIndex The index of the material
   */
  void setMaterialIndex(const int& newMaterialIndex){materialIndex = newMaterialIndex;};

  /**
   *  Set the flag indicating if the object can collide with other objects
   *  @param value true if object can collide
   */
  virtual void setCollideBit(bool value){collideBit = value;};
  
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
  /** Checks if the object is affected by rolling friction 
   * @return True, if affected by rolling friction, otherwise false
   */
  virtual bool isAffectedByRollingFriction() const {return false;}
  /** Applies rolling friction to the object 
   *  @param angle The collision angle
   *  @param rfc The rolling friction coefficient
   */
  virtual void applyRollingFriction(double angle, double rfc){};
  /** Applies simple rolling friction to the object.
   *  @param rfc The rolling friction coefficient
   */
  virtual void applySimpleRollingFriction(double rfc){};
  /** Determines if object is (or better could be) affected by rolling friction and sets rolling friction flag */
  virtual void determineRollingFrictionFlag(){};

  /** Use a shader program to draw this object
  * @param shader The shader program
  */
  void setShaderProgram(ShaderProgram* shader)
  {shaderProgram = shader;}

  /** Replaces the current shader program with a new one
  * @param newShaderProgram A pointer to the new shaderProgram
  */
  virtual void replaceShaderProgram(ShaderProgram* newShaderProgram);

  /** Replaces the current shader program with a new one,
  *   if it is equal to a given shader program
  * @param oldShaderProgram A pointer to a shader program to compare with
  * @param newShaderProgram A pointer to the new shader program
  */
  virtual void replaceShaderProgram(ShaderProgram* oldShaderProgram,
                                    ShaderProgram* newShaderProgram);

  /** Get a pointer to the defined shader program
  * @return a pointer to the shader program
  */
  ShaderProgram* getShaderProgram()
  { return shaderProgram; };
};


#endif // PHYSICALOBJECT_H_
