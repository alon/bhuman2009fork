/**
 * @file Simulation/Simulation.h
 *
 * Definition of class Simulation
 *
 * @author <A href="mailto:Tim.Laue@dfki.de">Tim Laue</A>
 * @author <A href="mailto:kspiess@informatik.uni-bremen.de">Kai Spiess</A>
 * @author <A href="mailto:pachur@informatik.uni-bremen.de">Dennis Pachur</A>
 */

#ifndef SIMULATION_H_
#define SIMULATION_H_

#include <map>
#include <string>
#include <vector>
#include <list>
#include <stack>
#include <ode/ode.h>

#include "APIDatatypes.h"
#include "PhysicsParameterSet.h"
#include "../Sensors/Sensor.h"
#include "../PhysicalObjects/Plane.h"
#include "../OpenGL/Environment.h"
#include "../Tools/Errors.h"
#include "../Tools/Surface.h"
#include "../Tools/SimMath.h"
#include "../Tools/CoefficientTable.h"
#include "../Tools/Material.h"

class Actuatorport;
class Actuator;
class View;
class SAX2Parser;
class SimObject;
class GraphicsManager;
class Controller;
class Controller3DDrawing;
class CompoundPhysicalObject;
class OffscreenRenderer;
class ShaderProgram;


/**
* @class Simulation
*
* The main class of SimRobotCore.
* Simulation controls and wraps all simulation tasks:
* - loading, starting, stoppping
* - visualization
* - object management
* - error management
*/
class Simulation
{
private:
  /**
  * @class Selection
  * Encapsulates information about a selection in the scene
  */
  class Selection
  {
  public:
    /** Basic information about the user's perspective*/
    Vector3d cameraPos,
             cameraTarget,
             cameraUpVector;
    /** Additional information about the user's perspective*/
    double fovy;
    /** Information about the display resolution*/
    int width,
        height;
    /** The selection plane and an offset*/
    Vector3d plane,
             clickOffset;
    /** The selected object and the root object*/
    SimObject* object,
             * root;
    /** graphical statistics */
    unsigned int numVert,
                  numPrim;
    /** Constructor*/
    Selection() : object(0), numVert(0), numPrim(0) {}
  };

  /** The current selection*/
  Selection selection;

  /** A type for mapping object names to pointers*/
  typedef std::map<std::string, SimObject*> NameToPointerMap;

  /** A list of all ojects in the scene*/
  std::vector<SimObject*> objectList;
  /** A list of all movables in the scene*/
  std::map<int, SimObject*> movableMap;
    /** A list of all sensorports*/
  std::vector<SensorPort> sensorPortList;
  /** A list of all actuatorports*/
  std::vector<Actuatorport*> actuatorportList;
  /** A list of all actuators*/
  std::vector<Actuator*> actuatorList;
  /** A list of all object surfaces*/
  std::vector<Surface*> surfaces;
  /** A list of all environments possibly used*/
  std::vector<Environment*> environments;
  /** A list of all views to be drawn*/
  std::list<View*> viewList;
  /** A fast look-up table for object pointers*/
  NameToPointerMap objectMap;
  /** A fast look-up table for object pointers to macros*/
  NameToPointerMap macroMap;
  /** The currently parsed macro*/
  SimObject* currentMacro,
             childBoundLight;
  /** A stack of macros which are currently parsed*/
  std::stack<SimObject*> macroStack;
  /** The root node of the object tree*/
  SimObject* objectTree;
  /** The file to be opened*/
  std::string filename;
  /** The description text of the current scene*/
  std::string sceneDescription;
  /** The schema to be used*/
  std::string schemaName;
  /** The current status of the simulation*/
  SimulationStatus simulationStatus;
  /** A pointer to a parser for XML files*/
  SAX2Parser* parser;
  /** The error management object*/
  ErrorManager errorManager;
  /** An object for storing vertices and drawing primitives*/
  GraphicsManager* graphicsManager;
  /** The current simulation step*/
  int simulationStep;
  /** The current frame rate in frames per second*/
  int currentFrameRate;
  /** Just as the name of the attribute says ;-)*/
  unsigned long timeOfLastFrameRateComputation;
  /** Just as the name of the attribute says ;-)*/
  int stepAtLastFrameRateComputation;
  /** The time, one step takes*/
  double stepLength;
  /** A standard length for drawing*/
  double standardLength;
  /** Flag: Try to use hardware acceleration of the graphics card for offscreen rendering, if true.*/
  bool useHardwareRendering;
  /** A flag that stores whether the scene graph changed. */
  bool sceneGraphChanged;
  /** count the number of primitves, their polygons and display the information for the user */
  bool showStatistics;
  /** the cube map generate counter */
  bool saveCubeMap;
  /** Flag, simulation will be resetted completely during current step, if true*/
  bool resetFlag;
  /** Flag, used for resetting*/
  bool simulationReady;
  /** An instance of the offscreen renderer*/
  OffscreenRenderer* offscreenRenderer;

  /** The background surface */
  Surface* backgroundSurface;
  /** The default surface */
  Surface* defaultSurface;
  /** The clear color */
  const GLfloat* clearColor;

  /** Helper function for simple object selection.
  * @param x The selected x position on the display
  * @param y The selected y position on the display
  * @param vecFar Returns a vector to the point on the ray at the far clipping plane
  * @param vecNear Returns a vector to the point on the ray at the near clipping plane
  */
  void gluProjectClick(int x, int y, Vector3d& vecFar, Vector3d& vecNear);
  /** Helper function to set parameters for above function */
  void prepareGluParameters();

  /** Helper Function to set the current viewpoint */
  void setCamera(const Vector3d& cameraPos, const Vector3d& cameraTarget,
                             const Vector3d& cameraUpVector, const bool& snapToRoot,
                             const SimObject* drawingObject, const int& generateCubeMap = 0);

  /** Helper Function to draw the shadow maps */
  void drawShadowMaps(SimObject* rootObject, VisualizationParameterSet& visParams);

  /* PHYSICAL SIMULATION */
  /** The parameters for the physical simulation of the scene.*/
  PhysicsParameterSet* physicsParameters;
  /** The physical world.*/
  dWorldID physicalWorld;
  /** The collision space for static objects.*/
  dSpaceID staticSpace;
  /** The collision space for movable objects.*/
  dSpaceID movableSpace;
  /** The contact group for collision handling.*/
  dJointGroupID contactGroup;
  /** The root collision space.*/
  dSpaceID rootSpace;
  /** A pointer to the current selected collision space where the next object is inserted.*/
  dSpaceID* currentSpace;
  /** A ray for the computation of intersections with the scene.*/
  dGeomID intersectionRay;
  /** A map with all compound physical objects stored by their movable id*/
  std::map<int, CompoundPhysicalObject*> compoundObjects;
  /** A map with collision sensor objects stored by their geometry*/
  std::map<dGeomID, Sensor*> collisionSensors;
  /** A list of all planes */
  std::vector<Plane*> planes;
  /** A look up table with all defined friction coefficients for coulomb friction */
  CoefficientTable frictionCoefficientTable;
  /** A look up table with all defined rolling friction coefficients for coulomb friction */
  CoefficientTable rollingFrictionCoefficientTable;
  /** A look up table with all defined coefficients of restitution */
  CoefficientTable restitutionCoefficientTable;
  /** A list of all object materials */
  std::vector<Material*> materials;
  /** The ID of the current movable */
  int highestMovableID;
  /** A stack for movable IDs */
  std::stack<int> movableIDs;
  /** The time, one physics simulation step takes*/
  double physicsStepLength;
  /** A flag indicatinge whether the quick step solver for physics simulation should be used or not */
  bool useQuickSolverForPhysics;
  /** The number of iteration for one simulation step of the physics quick step solver; default is 20*/
  unsigned int numberOfIterationsForQuickSolver;
  /** A flag indicatinge whether automatic disabling of physical objects should be used or not */
  bool useAutomaticObjectDisabling;
  /** The parameter set for automatic disabling of physical objects */
  AutomaticObjectDisablingParameters autoObjectDisableParameters;
  /** The collision detection mode; if '0', two objects directly connected by a joint can not collide (default);
      if '1' two objects directly connected by a joint can collide */
  short collisionDetectionMode;
  /** Counter for statistics*/
  int numberOfCollisionContacts;
  /** A force factor for application of dynamics in drag and drop interaction */
  double applyDynamicsForceFactor;
  // the global motion blur mode
  MotionBlurMode motionBlurMode;

  /** Information about an intersection*/
  struct IntersectionInformation
  {
    /** The distance to the intersection*/
    double distance;
  };

  /** An instance of the previously defined class*/
  IntersectionInformation intersectionInformation;

  /**
   * Static callback method for collision handling. It is a wrapper for the member function collisionCallback.
   * @param pData User defined data for collision callback function. In this case a pointer to the simulation is passed.
   * @param geom1 The first geometry object for collision testing.
   * @param geom2 The second geometry object for collision testing.
   */
  static void staticCollisionCallback(void *pData, dGeomID geom1, dGeomID geom2);

  /**
   * Static callback method for collision handling in case of intersection tests
   * @param pData User defined data for collision callback function. In this case a pointer to the simulation is passed.
   * @param geom1 The first geometry object for collision testing.
   * @param geom2 The second geometry object for collision testing.
   */
  static void staticIntersectionCallback(void *pData, dGeomID geom1, dGeomID geom2);
  
  /**
   * Callback method for collision handling. It is used by the static wrapper method nearCallback.
   * @param geom1 The first geometry object for collision testing.
   * @param geom2 The second geometry object for collision testing.
   */
  void collisionCallback(dGeomID geom1, dGeomID geom2);

  /**
   * Callback method for intersection handling. It is called by staticIntersectionCallback.
   * @param geom1 The first geometry object for collision testing.
   * @param geom2 The second geometry object for collision testing.
   */
  void intersectionCallback(dGeomID geom1, dGeomID geom2);

  /**
   * Writes back the results from the physical simulation.
   * Sets position and rotation from body/geometry to simulation object after physical simulation step.
   * @param physObj A pointer to the root node of the object tree
   * @param store_last_results Stores last result to previous results storage (12 steps)
   */
  void writeBackPhysicalResults(SimObject* physObj, const bool& store_last_results);

  /**
   * Disables all childs of an object in physical simulation. The object itself will not be disabled.
   * This method is used for movable objects.
   * Depending on the drag mode, the dynamics of the dragged object are kept, reseted or new dynamics
   * are applied.
   * @param object The object whose childs should be disabled
   * @param disableGeometry Flag, if geometry should be disabled
   */
  void disablePhysics(SimObject* object, bool disableGeometry);
  
  /**
   * Enables all childs of an object in physical simulation. The object itself will not be enabled.
   * This method is used for movalble objects.
   * @param object The object whose childs should be enabled
   */
  void enablePhysics(SimObject* object);

  /**
   * Creates all static objects physics. These objects are of type PhysicalObject
   * and have a movable ID of 0.
   * @param object A pointer to the root node of the object tree
   */
  void createStaticObjectPhysics(SimObject* object);
  
  /**
   * Creates compound physical objects. Movable physical objects with fixed connections are
   * joined together by their movable ID to these compound physical objects. These fixed connections
   * are implicitly given by the structure of the object tree.
   * Single movable physical objects are stored also in a compound physical object structure but
   * treated different in the process of creating object physics.
   * @param object A pointer to the root node of the object tree
   */
  void createCompoundPhysicalObjects(SimObject* object);
  
  /**
   * Creates the object physics used by the OpenDynamicsEngine (ODE)
   * for the objects stored in compound physical objects map.
   * These physics are geometry objects for collision detection and a body for dynamics.
   */
  void createCompoundObjectPhysics();
  
  /**
   * Creates the physical joints and their motors used by the OpenDynamicsEngine (ODE)
   * for the objects in the object tree.
   * This has to be done after the physical objects (geometry and body)
   * have been created.
   * @param object A pointer to the root node of the object tree
   */
  void createJointAndMotorPhysics(SimObject* object);

  /**
   * Resets the linear and angular velocity of an object and its child objects
   * @param object A pointer to the root node of the object
   */
  void resetDynamicsForObject(SimObject* object);

  /**
   * Returns the mass for an object including its child objects
   * @param object A pointer to the root node of the object
   * @return The total mass of the object
   */
  double getMassForObject(SimObject* object) const;

  /**
   * Collects all collision based sensors and its belonging geometry in a map.
   */
  void collectCollisionSensors();

  /**
   *  Collects all geometry objects of a collision based sensor.
   *  The geometry are all physical child objects of the sensor
   *  @param sensor The sensor whose geomtry is collected
   *  @param object The object whose childs processed
   */
  void collectCollisionSensorGeometry(Sensor* sensor, SimObject* object);
  
  /**
   *  Resets all collision sensors. This is called each step befor the collision detection.
   */
  void resetCollisionSensors();

  /**
   *  Create physics used by the OpenDynamicsEngine (ODE) for all planes in the simulation scene
   */
  void createPlanePhysics();
  
  /** Apply additional friction for joints */
  void applyJointFriction();

  /**
   * Searches for an object given parts of its name
   * @param partsOfName An array of parts of the name of the object, 
   *                    in which they must appear in the given sequence.
   * @return A pointer to the first object fulfiling the naming constraints
   */
  SimObject* getObjectReference(const std::vector<std::string>& partsOfName, bool fullName = true);

public:
  /** Constructor*/
  Simulation();

  /** Destructor */
  ~Simulation() { closeSimulation(); dCloseODE(); }

  /** Get the filename */
  std::string getFilename()
  { return filename; };

  /** Draws the complete scene (or a subset) with OpenGL
  * (An OpenGL environment has to be set up before calling draw)
  * @param cameraPos The position of the camera
  * @param cameraTarget The target of the camera
  * @param cameraUpVector The UpVector of the camera (see OpenGL docu)
  * @param width The width of the view (in pixels)
  * @param height The height of the view (in pixels)
  * @param fovy The vertical openingAngle in degrees
  * @param visParams Parameters for drawing
  * @param updateGluParameters A flag if the selection matrices should be updated
  * @param drawingObject The object to be drawn
  * @param fbo_reg The frame buffer object identifier to handle multiple fbos
  */
  void draw(const Vector3d& cameraPos,
            const Vector3d& cameraTarget,
            const Vector3d& cameraUpVector,
            unsigned int width, unsigned int height, double fovy,
            VisualizationParameterSet& visParams,
            const bool updateGluParameters,
            SimObject* drawingObject,
            const int fbo_reg = -1);

  // the rendering instructions
  void renderingInstructions(
    const Vector3d& cameraPos, const Vector3d& cameraTarget, const Vector3d& cameraUpVector,
    unsigned int width, unsigned int height, double fovy, const bool& updateGluParameters,
    VisualizationParameterSet& visParams, SimObject* drawingObject, const int fbo_reg,
    MotionBlurMode mode, MotionBlurMode skip = MOTIONBLUR_OFF);

  // render a cube map (environment map)
  void renderCubeMap(const Vector3d& cameraPos, const Vector3d& cameraTarget, const Vector3d& cameraUpVector,
                     VisualizationParameterSet& visParams, SimObject* drawingObject);

  /** Draws all planes in the scene. Only called by object Camera
  * @param visParams Parameters for drawing
  */
  void drawPlanes(const VisualizationParameterSet& visParams);

  /** A function for computing the values of all sensors that
  *   cannot be computed while drawing the scene.
  *   Used for sensor value visualization
  */
  void precomputeSensorValues();

  /** Toggle Light show cones flag from graphics manager (encapsulated function) */
  void toggleLightShowCones();

  /** Changes CameraPos to look from light source (switches through light sources)
  * calculated by graphics manager (encapsulated function)
  * @param cameraPos The vector of the camera position
  * @param cameraTarget The vector of the camera target
  */
  void camToLightPos(Vector3d& cameraPos, Vector3d& cameraTarget);

  /** Selects a drawn object
  * @param x The selected x position on the display
  * @param y The selected y position on the display
  * @param cameraPos The position of the camera
  * @param cameraTarget The position of the camera target
  * @param cameraUpVector The upVector of the camera
  * @param width The width of the view (in pixels)
  * @param height The height of the view (in pixels)
  * @param dragMode The drag mode
  * @param simObject The root object of the scene displayed
  * @param selectButtons Flag: Also select interactive buttons, if true
  * @return The type of the selection result
  */
  InteractiveSelectionType selectObject(
                    int x, int y,
                    const Vector3d& cameraPos, const Vector3d& cameraTarget, const Vector3d& cameraUpVector,
                    int width, int height, DragAndDropPlane plane,
                    DragAndDropMode dragMode, SimObject* simObject, bool selectButtons);

  /** Removes a previous selection*/
  void unselectObject();

  /** Moves a selected object to a new position
  * @param x The selected x position on the display
  * @param y The selected y position on the display
  */
  void translateObject(int x, int y);

  /** Rotates a selected object
  * @param x The selected x position on the display
  * @param y The selected y position on the display
  */
  void rotateObject(int x, int y);

  /** Adds a macro to the macro list
  * @param name The name of the macro
  * @param macro A pointer to the macro object
  */
  void addMacro(const std::string& name, SimObject* macro);

  /** Creates an instances of a macro
  * @param macroName The name of the macro
  * @param instanceName The name of the new instance
  * @param parent The parent node of the instance
  * @return true, if the macro has been instantiated
  */
  bool instantiateMacro(const std::string& macroName, const std::string& instanceName,
                        SimObject* parent);

  /** Returns the pointer to a node inside a macro
  * @param name The name of the node or an empty string
  * @return A pointer to the node which has the given name, a pointer to the first node, otherwise.
  */
  SimObject* getNodeInMacro(const std::string& name) const;

  /** Adds current macro instance to the object tree
  *   as a child node of a given object
  *   @param parent The parent node of the instance
  */
  void addCurrentMacroInstanceToObjectTree(SimObject* parent);

  /** Returns the selected object
  * @return The selected object or 0 if no object is selected
  */
  SimObject* getSelectedObject() const {return selection.object;}

  /** Determines a zoom distance to fit an object or the whole scene in the current display
  * @param cameraTarget The vector of target of the camera
  * @param cameraPos The vector of the position of the camera
  * @param width The width of the display (in pixels)
  * @param height The height of the display (in pixels)
  * @param object The object to be fit in
  */
  void fitCamera(Vector3d& cameraTarget, Vector3d& cameraPos, int width,
                 int height, SimObject* object);

  /** Sets the color of the background of the scene
  * (The color has to be a triple of (R,G,B) with values [0.0,..,1.0]
  * @param color The color
  */
  void setBackgroundColor(const Vector3d& color);

  /** Returns a pointer to the current background surface object
  * @return A pointer to a surface object
  */
  Surface* getBackgroundSurface()
  { return backgroundSurface; };

  /** enable state display list call (wrapper for classes without simgraphics)
  * @param style The Surface Style
  */
  void enableStateDL(VisualizationParameterSet::SurfaceStyle style);

  /** disable state display list call (wrapper for classes without simgraphics)
  * @param style The Surface Style
  */
  void disableStateDL(VisualizationParameterSet::SurfaceStyle style);

  /** Sets the hardware rendering flag
  * @param useHardwareRendering The new value of the flag
  */
  void setUseHardwareRendering(bool useHardwareRendering) {this->useHardwareRendering = useHardwareRendering;}

  /** Returns the value of the hardware rendering flag
  * @return The value
  */
  bool getUseHardwareRendering() const {return useHardwareRendering;}

  /** Loads a file and initializes the simulation
  * @param filename The name of the file
  * @param schemaName The name of the schema used for validation
  */
  bool loadFile(const std::string& filename, const std::string& schemaName);

  /** Closes the simulation and destroys all object*/
  void closeSimulation();

  /** Returns the current status of the simulation
  * @return The status
  */
  SimulationStatus getSimulationStatus() const {return simulationStatus;}

  /** Returns the file name of the simulation
  * @return The file name
  */
  std::string getSimulationFileName() const {return filename;}

  /** Returns the name of the scene
  * @return The scene name
  */
  std::string getSceneName() const {return objectTree ? objectTree->getName() : "";}

  /** Returns the description of the scene
  * @return The description
  */
  std::string getSceneDescription() const {return sceneDescription;}

  /** Resets the simulation*/
  void resetSimulation();

  /** Executes one simulation step */
  void doSimulationStep();

  /** Announces a reset from the controller*/
  void setResetFlag() {resetFlag = true; simulationReady = false;}

  /** Returns the value of the reset flag*/
  bool getResetFlag() const {return resetFlag;}

  /** Returns true, if simulation is completely loaded an runnable*/
  bool isSimulationReady() const {return simulationReady;}

  /** Sets the length of one simulation step (in milliseconds)
  * @param stepLength The time which is simulated by one step
  */
  void setStepLength(double stepLength);

  /** Returns the step length
  * @return The time which is simulated by one step
  */
  double getStepLength() const {return stepLength;}

  /** Returns the standard length for drawing
  * @return The length
  */
  double getStandardLength() const {return standardLength;}

  /** Returns the current simulation step
  * @return The step
  */
  unsigned int getSimulationStep() const {return simulationStep;}

  /** Returns the current frame rate
  * @return The frame rate in frames per second
  */
  int getFrameRate() const {return currentFrameRate;};

  /** Returns the number of collisions during the last step
  * @return The number of collisions
  */
  int getNumberOfCollisions() const {return numberOfCollisionContacts;};

  /** Updates frame rate of simulator*/
  void updateFrameRate();

  /** Returns a pointer to an object (Hacking interface ;-)
  * @param objectName The name of the object
  * @return A pointer to an object
  */
  SimObject* getObjectReference(const std::string& objectName, bool fullName = true);

  /** Returns a pointer to the GraphicsManager (Hacking interface ;-)
  * @return The pointer
  */
  GraphicsManager* getGraphicsManager() {return graphicsManager;}

  /** Returns a pointer to the Offscreen Renderer (Hacking interface ;-)
  * @return The pointer
  */
  OffscreenRenderer* getOffscreenRenderer() {return offscreenRenderer;}

  /** Returns the id for a actuatorport
  * @param parts An array of parts of the name of the actuatorport, 
  *              in which they must appear in the given sequence.
  * @return The id
  */
  int getActuatorportId(const std::vector<std::string>& parts) const;

  /** Returns the minimum value to be set for an actuatorport
  * @param id The id of the port
  * @return The minimum value
  */
  double getActuatorportMinValue(int id) const;

  /** Returns the maximum value to be set for an actuatorport
  * @param id The id of the port
  * @return The maximum value
  */
  double getActuatorportMaxValue(int id) const;

  /** Sets the value for an actuatorport
  * @param id The id of the port
  * @param value The new value
  */
  void setActuatorport(int id, double value);

  /** Sets the value for an actuatorport
  * @param id The id of the port
  * @param value The new value
  */
  void setActuatorport(int id, bool value);

  /** Returns the id for a sensor port
  * @param parts An array of parts of the name of the sensor port, 
  *              in which they must appear in the given sequence.
  * @return The id
  */
  int getSensorPortId(const std::vector<std::string>& parts) const;

  /** Returns a constant reference to the data computed for a sensor
  * @param id The id of the sensor port
  * @return The sensor reading
  */
  SensorReading& getSensorReading(int id); 

  /** Returns the surface for a given name.
  * @param name The name of the surface searched for.
  * @return A pointer to the surface, or 0 if it does not exist.
  */
  Surface* getSurface(const std::string& name) const;

  /** Returns the shader program for a given name.
  * @param name The name of the shader program searched for.
  * @return A pointer to the shader program, or 0 if it does not exist.
  */
  ShaderProgram* getShaderProgram(const std::string& name) const;

  /** Returns the environment for a given name.
  * @param name The name of the environment searched for.
  * @return A pointer to the environment, or 0 if it does not exist.
  */
  Environment* getEnvironment(const std::string& name) const;

  /** Returns the default surface.
  * @return A pointer to the surface
  */
  Surface* getDefaultSurface() const { return defaultSurface; }

  /** Returns a list of object descriptions
  * @param objectDesciptionTree The list to be returned
  */
  void getObjectDescriptionTree(std::vector<ObjectDescription>& objectDesciptionTree) const;

  /** Gives direct access to actuator data. Handle with care!
  * @return A pointer to the actuator list
  */
  std::vector<Actuator*>* getActuatorList() {return &actuatorList;}

  /** Returns the first (the oldest) error and deletes it
  * @param The error to be returned
  */
  void getFirstError(ErrorDescription& error);

  /** Returns all current errors
  * @return errors A returned list of errors
  */
  void getAllErrors(std::vector<ErrorDescription>& errors);

  /** Deletes all current errors*/
  void deleteAllErrors();

  /** Adds a new view to be managed and updated
  * @param view The view
  * @param name The name of the view
  */
  void addView(View* view, const std::string& name);

  /** Returns the view with a certain sensor port id
  * @param id The sensor port id
  * @return The view
  */
  View* getView(int id);

  /** This function changes the physical parameters while
  *   the simulator is running. This is useful for parameters optimization.
  * @param newParams The new parameters
  */
  void changePhysicsParametersAtRunTime(const PhysicsParameterSet& newParams);
  /**
   * Gets the physics parameters for the simulation of the world represented by the scene
   * @return Pointer to the world physics object
   */
  PhysicsParameterSet* getPhysicsParameters(){return physicsParameters;}
  /** Gets a pointer to the physical world object
   * @return A pointer to physical world
   */
  dWorldID* getPhysicalWorld(){return &physicalWorld;}
  /** Gets a pointer to the collision space for static (non movable) objects
   * @return A pointer to the collision space
   */
  dSpaceID* getStaticSpace(){return &staticSpace;}
  /** Gets a pointer to the collision space for movable objects
   * @return A pointer to the collision space
   */
  dSpaceID* getMovableSpace(){return &movableSpace;}
  /** Gets a pointer to the currently selected collision space
   * @return A pointer to the collision space
   */
  dSpaceID* getCurrentSpace(){return currentSpace;}
  /** Sets the pointer to the currently selected collision space
   * @param currentSpace The pointer to the space
   */
  void setCurrentSpace(dSpaceID* currentSpace){this->currentSpace = currentSpace;}

  /**
  * Parses the attributes of the scene which are
  * defined in seperate elements
  * @param name The name of the attribute element
  * @param attributes The attributes of the element
  * @param line The line in the scene description
  * @param column The column in the scene description
  * @param errorManager An object managing errors
  */
  void parseAttributeElements(const std::string& name,
                              const AttributeSet& attributes,
                              int line, int column,
                              ErrorManager* errorManager);

  /**
  * Postprocesses all nodes of the object tree after parsing a scene
  * description file. Recursive function called by parser.
  * @param element The element to be postprocessed
  * @param path A string describing the path of the object in the tree
  */
  void postprocessObjectTree(SimObject* element, const std::string& path);

  /**
  * Again process whole tree and fill the list of all objects as well
  * as a map of all root nodes of movables
  * @param currentElement The element currently processed
  */
  void collectObjectsAndMovables(SimObject* currentElement);

  /**
   * Resets the linear and angular velocity of the selected object
   */
  void resetDynamicsForSelection();
  /**
   * Applys force to the selected object
   * @param x The selected x position on the display
   * @param y The selected y position on the display
   */
  void applyForceForSelection(int x, int y);
  /**
   * Applys torque to the selected object
   * @param x The selected x position on the display
   * @param y The selected y position on the display
   */
  void applyTorqueForSelection(int x, int y);
  /**
   * Returns the root object of a movable object
   * @param The movable object
   * @param The root object of the movable object
   * @return A pointer to the root object
   */
  SimObject* findRootOfMovableObject(SimObject* object) const;
  /**
   * Returns current movable ID
   * @return The current movable ID
   */
  int getCurrentMovableID(){return movableIDs.top();};
  /** Cretes a new movable ID and puts it on the stack of movable IDs */
  void createNewMovableID(){highestMovableID++; movableIDs.push(highestMovableID);};
  /** Removes the current movable ID from stack of movable IDs */
  void removeCurrentMovableID(){movableIDs.pop();};
  /**
   * Returns the index of the given material in the friction coefficent table. It marks the row/column of the material
   * @param materialName The name of the material
   * @return The index of the material
   */
  int getMaterialIndex(const std::string materialName) const;
  /** Creates the coefficient (look up) table for physical parameters*/
  void createPhysicalCoefficientTables();

  /** Return whether the scene graph has changed.
  * @return Has the scene graph changed since the last 
  *         call of doSimulationStep?
  */
  bool hasSceneGraphChanged() const {return sceneGraphChanged;}

  /** Resets that the scene graph has changed. */
  void resetSceneGraphChanged() {sceneGraphChanged = false;}

  /** Intersects the physical representations with a ray.
  *   Uses ODE to determine collision point.
  * @param pos The starting position of the ray
  * @param ray The ray
  * @param maxLength The length of the ray
  * @param distance The distance to the intersection point
  * @return true, if an intersection position has been found
  */
  bool intersectWorldWithRay(const Vector3d& pos, const Vector3d& ray,
                             double maxLength, double& distance);
  
  /** Returns the step length of the physics simulation
  * @return The time which is simulated by one physics simulation step
  */
  double getPhysicsStepLength() const {return this->physicsStepLength;}

  /** Returns a pointer to the object tree root object
   * @return The pointer to the object tree root
   */
  const SimObject* getObjectTreeRoot() const {return this->objectTree;}

  /** 
   * Registers controller drawings at an object in the simulation scene
   * @param parts An array of parts of the name of the object, 
   *              in which they must appear in the given sequence.
   * @param drawing The drawing
   */
  void registerControllerDrawingByPartOfName(const std::vector<std::string>& parts,
                                             Controller3DDrawing* drawing);

  /** 
   * Unregisters controller drawings at an object in the simulation scene
   * @param parts An array of parts of the name of the object, 
   *              in which they must appear in the given sequence.
   * @param drawing The drawing
   */
  void unregisterControllerDrawingByPartOfName(const std::vector<std::string>& parts,
                                               Controller3DDrawing* drawing);

  /**
  * Toggle the running state of the shader interface
  */
  void toggleShaderInterface();

  /**
  * Get the status of the shader interface
  * @return true if the shader interface is currently running
  */
  bool getShaderInterfaceState();

  /**
  * Reload all shader sources and recompile and relink shader and programs
  */
  void reloadShader();

  /**
  * Switch through the programs visible in the shader interface
  * @param previous if true the previous program is selected, the next if false
  */
  void switchProgram(bool previous);

  /**
  * Switch through the shader visible in the shader interface
  * @param previous if true the previous shader is selected, the next if false
  */
  void switchShader(bool previous);

  /**
  * Toggle information display of shader interface on or off
  */
  void toggleShaderShowInfos();

  /**
  * Get the status of the information display of the shader interface
  * @return true if the infos are displayed
  */
  bool getShaderShowInfos();

  /**
  * Switch through the information tabs visible in the shader interface
  * @param previous if true the previous info tab is selected, the next if false
  */
  void switchInfo(bool previous);

  /**
  * Set the currently selected program as post process shader program
  */
  void definePostProgram();
  bool getPostProgramDefined();

  /**
  * Set the currently selected program as global working shader program
  */
  void defineGlobalProgram();

  /** check graphics hardware capabilities for cdoc (which don't include the graphics manger)
  * @param mode The mode to check
  * @return true if the capability is available
  */
  bool checkMotionBlurCapability(const MotionBlurMode mode);

  // set a new motion blur mode
  void setMotionBlurMode(const MotionBlurMode mb_mode);

  // get the current motion blur mode
  MotionBlurMode getMotionBlurMode();

  // get / set frame buffer object options
  void toggleMultisampledFBOs();
  bool getMultisampledFBOsCapability() const;
  bool getMultisampledFBOsState() const;
  void setDebugFBOs(const int debugState);
  int getDebugFBOsState();
  void toggleOversizedFBOs();
  bool getOversizedFBOsState();

  // get the number of antialiasing samples
  int getAntialiasingSamples();

  /** get or toggle the frames per second drawings */
  bool getShowFPS();
  void toggleShowFPS();

  /** get or toggle the polygon count drawings */
  bool getShowStatistics();
  void toggleShowStatistics();

  /** generate a cube map for use with enivironment mapping */
  void setCubeMapGeneration();

  /** move object to target position.
  * @param object The object to move.
  * @param pos The target position.
  */
  void moveObject(SimObject* object, const Vector3d& pos);
    
  /** move object to target position.
  * @param object The object to move.
  * @param pos The target position.
  * @param rot The target rotation.
  */
  void moveObject(SimObject* object, const Vector3d& pos, const Vector3d& rot);

  /** interface with Naoqi - let naoqi decide where each joint will be moved, provide information
   * to naoqi. All of this is done using the naoqi simulation api.
   */
  void readWriteNaoqi();
};

#endif //SIMULATION_H_
