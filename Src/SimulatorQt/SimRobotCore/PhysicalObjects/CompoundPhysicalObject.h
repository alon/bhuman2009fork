/**
 * @file CompoundPhysicalObject.h
 * 
 * Definition of class CompoundPhysicalObject
 *
 * @author <A href="mailto:kspiess@informatik.uni-bremen.de">Kai Spiess</A>
 */ 

#ifndef COMPOUNDPHYSICALOBJECT_H
#define COMPOUNDPHYSICALOBJECT_H

#include <vector>
#include <Simulation/SimObject.h>
#include <ode/ode.h>


/**
 * @class CompoundPhysicalObject
 *
 * A class representing a compound physical object consisting of single physical objects.
 * The compound object has a single body and several geometry objects for collision detection.
 */
class CompoundPhysicalObject
{
protected:
  /** The body of the object*/
  dBodyID body;
  /** All single geometry objects of the compound object*/
  std::vector<dGeomID> geometry;
  /** The total mass of the object*/
  double totalMass;
  /** The combined center of mass */
  Vector3d centerOfMass;
  /** A list of all objects the compound object consists of*/
  std::vector<SimObject*> objects;
  /** A pointer to the simulation*/
  Simulation* simulation;

  /** Computes the total mass of the compound object*/
  void computeTotalMass();
  /** Computes the combined center of mass of the compound object*/
  void computeCenterOfMass();

  /** A pointer to the parent object */
  SimObject* parent;
  /** Flag, if the compound object contains only a single physical object*/
  bool simple;
  /** The movable id of all physical objects belonging to the compound object*/
  int movableID;
  /** A list of the objects with the lowest depth in the scene tree belonging to the compound object*/
  std::vector<SimObject*> rootObjects;
  /** Collects the objects with the lowest scene tree depth of this compound object in a list */
  void setRootObjects();
  bool synchronized;

public: 
  /** Constructor*/
  CompoundPhysicalObject();

  /** Destructor*/
  virtual ~CompoundPhysicalObject() {}

  /** Returns a string describing the kind of object
  * @return The kind
  */
  virtual std::string getKind() const {return "compoundPhysicalObject";}
  /**
   * Creates the physics for the combined object used by ODE. These are a body with
   * total mass, combined center of mass, the combined inertia tensor and all geometry 
   * objects for collision detection transformed to the combined center of mass.
   */
  virtual void createPhysics();
  /** 
   * Sets a pointer to the simulation
   * @param sim A pointer to the simulation
   */
  void setSimulation(Simulation* sim){simulation = sim;};
  /** 
   * Adds a single object the combined object consists of
   * @param obj A pointer to the object to be added
   */
  void addObject(SimObject* obj){objects.push_back(obj);};

  /** 
   * Sets the parent node of the compound physical object
   * @param parentObj The parent object
   */
  void setParent(SimObject* parentObj){parent = parentObj;};
  /** 
   * Translates the compound object and its children in 3D space
   * @param translation A vector describing the translation
   */
  virtual void translate(const Vector3d& translation);
  /** 
   * Rotates the compound object and its children in 3D space
   * @param rotation The rotation in matrix form
   * @param origin The point to rotate around
   */
  virtual void rotate(const Matrix3d& rotation, 
                      const Vector3d& origin);
  /** 
   * Rotates the compound object around a certain axis
   * @param angles The rotation angle
   * @param origin The axis to rotate around
   */
  virtual void rotateAroundAxis(double angle, Vector3d axis);

  /**
   * Sets the flag, ift the compound object contains onla a single physical object
   * @param value True, if only single physical object
   */
  void setSimple(bool value){simple = value;};
  /** 
   * Checks, if the compound object contains only a single physical object
   * @return True, if single physical object is in the compound object
   */
  bool isSimple()const {return simple;}
  /** 
   * Inverts the colors of the object and all subobjects
   * Used for displaying selections.
   */
  virtual void invertColors();

  /** 
   * Sets the movable id of this compound object
   * id The movable id of the object
   */
  void setMovableID(int id){movableID = id;}
  /** 
   * Gets the movable id of this compound object
   * @return The movable id of the object
   */
  int getMovableID() const {return movableID;}

  /**
   * Returns the position of the compound object. This is the position of the body
   * belonging to the compound physical object.
   * @return The position
   */
  Vector3d getPosition() const;

  /** 
   * Disables physics for this object
   * @param disableGeometry Flag, if geometry should be disabled
   */
  virtual void disablePhysics(bool disableGeometry);

  /** Enables physics for this object */
  virtual void enablePhysics();


  bool isSynchronized(){return synchronized;};
  void setSynchronized(bool value){synchronized = value;};
  void translateDnD(const Vector3d& translation, bool incChilds);
  void rotateDnD(const Matrix3d& rot, const Vector3d& origin, bool incChilds);
  void rotateAroundAxisDnD(double angle, Vector3d axis, bool incChilds);

};

#endif
