/**
 * @file APIDatatypes.h
 * 
 * Definition of types used for interfacing SimRobotCore
 *
 * @author <A href="mailto:Tim.Laue@dfki.de">Tim Laue</A>
 */ 

#ifndef API_DATATYPES_H_
#define API_DATATYPES_H_

#include <vector>
#include <string>

/** A type for distinguishing between several types of objects*/
enum ObjectType 
{
  OBJECT_TYPE_NONE,
  OBJECT_TYPE_OBJECT,
  OBJECT_TYPE_SENSORPORT, 
  OBJECT_TYPE_ACTUATORPORT
};

/** Describes the current status of the simulation*/
enum SimulationStatus 
{
  NOT_LOADED, 
  SCENE_LOADED, 
  SIM_RUNNING,
  SIM_PAUSE
};

/** Specifies the current plane for Drag&Drop translations*/
enum DragAndDropPlane 
{
  XY_PLANE,
  XZ_PLANE,
  YZ_PLANE
};

/** Specifies the result of an interactive selection*/
enum InteractiveSelectionType
{
  NO_SELECTION=0, 
  BUTTON_SELECTION, 
  OBJECT_SELECTION
};

/** The drag and drop modes for physical objects*/
enum DragAndDropMode
{
  KEEP_DYNAMICS=0,
  RESET_DYNAMICS,
  APPLY_DYNAMICS
};

/* Motion Blur Modes */
enum MotionBlurMode
  {
    // selectable from gui
    MOTIONBLUR_OFF = 0,
    MOTIONBLUR_6PASSES,
    MOTIONBLUR_12PASSES,
    MOTIONBLUR_24PASSES,
    MOTIONBLUR_VELOCITY_BUFFER,
    MOTIONBLUR_VELOCITY_BUFFER_2,
    MOTIONBLUR_VELOCITY_BUFFER_2_GEO,
    MOTIONBLUR_COMBINED_METHOD,
    MOTIONBLUR_COMBINED_METHOD_2,
    // internal methods
    MOTIONBLUR_ACCUMULATION
};

/** The set of parameters for visualizing the scene*/
class VisualizationParameterSet
{
public:
  enum SurfaceStyle 
  {
    WIREFRAME = 0, 
    FLAT_SHADING, 
    SMOOTH_SHADING,
    TEXTURE_SHADING,
    PHYSICS_WIREFRAME, 
    PHYSICS_FLAT, 
    PHYSICS_SMOOTH,
    MIXED_PHYSICS,
    MIXED_APPEARANCE,
    CAMERA_SIMULATION,
    SHADOW_MAPPING
  } surfaceStyle;

  enum CameraMode
  {
    TARGETCAM = 0,
    FREECAM,
    PRESENTATION
  } cameraMode;

  /** Flag for turning sensor visualization on and off*/
  bool visualizeSensors;
  /** Flag: turs off some visualizations, if true*/
  bool drawForSensor;
  /** Flag: move camera position and target with root object */
  bool snapToRoot;
  /** Indicates whether the controller is called to draw or not*/
  bool enableControllerDrawings;
  /** Number of Render Passes (depends on the shadow mapped light sources) */
  int renderPasses;
  /** Current Render Pass */
  int currentRenderPass;
  /** Limited to a specific render pass (debug option) */
  int limitToRenderPass;
  /** flag if object should provide shader variables while not using a local shader */
  bool provideUniforms;
  /** Motion Blur Settings */
  MotionBlurMode mb_mode;
  MotionBlurMode mb_skip;
  float mb_renderPasses;
  int mb_currentPass;
  bool mb_useAccum;
  bool finishRendering;

  /** Constructor*/
  VisualizationParameterSet():
    surfaceStyle(SMOOTH_SHADING),
    cameraMode(TARGETCAM),
    visualizeSensors(false),
    drawForSensor(false),
    snapToRoot(true),
    enableControllerDrawings(false),
    renderPasses(1),
    currentRenderPass(0),
    limitToRenderPass(0),
    provideUniforms(false),
    mb_mode(MOTIONBLUR_OFF),
    mb_skip(MOTIONBLUR_OFF),
    mb_renderPasses(1),
    mb_currentPass(1),
    mb_useAccum(false),
    finishRendering(true)
    {}
};

/** 
* @class ObjectDescription
* Describes an object 
* (used for displaying information in a GUI)
*/
class ObjectDescription
{
public:
  /** Constructor*/
  ObjectDescription() : type(OBJECT_TYPE_OBJECT), depth(0) {}
  
  /** Constructor which fills all fields.
  * @param kind A description of the kind of the object.
  * @param name The object name (the last part).
  * @param fullName The name together with the full path in the object tree.
  * @param type The type of the object.
  * @param depth The depth in the object tree (useful for displaying in a tree).
  */
  ObjectDescription(const std::string& kind, const std::string& name, const std::string& fullName, ObjectType type, int depth) :
  kind(kind), name(name), fullName(fullName), type(type), depth(depth) {}

  /** Copy-Constructor
  * @param other Another ObjectDesciption
  */
  ObjectDescription(const ObjectDescription& other)
  {*this = other;}
  
  /** Copy-Operator
  * @param other Another ObjectDesciption
  */
  void operator = (const ObjectDescription& other)
  {kind = other.kind; name = other.name; fullName = other.fullName; type = other.type; depth = other.depth;}
  
  /** A description of the kind of the object*/
  std::string kind;
  /** The name*/
  std::string name;
  /** The name together with the full path in the object tree*/
  std::string fullName;
  /** The type of the object*/
  ObjectType type;
  /** The depth in the object tree (useful for displaying in a tree)*/
  int depth;
};

/**
* @class ErrorDescription
* Describes an error
*/
class ErrorDescription
{
public:
  /** The title of the error (very short description)*/
  std::string title;
  /** The complete error message*/
  std::string text;
  /** Flag: true, if the position of the error is known*/
  bool position;
  /** The line in the XML file in which the error occured.
  * Only used if position is true
  */
  int line;
  /** The column in the XML file in which the error occured
  * Only used if position is true
  */
  int column;
};

/**
* @class SensorReading
*
* A class representing the data a simulated sensor delivers
*/
class SensorReading
{
public:
  SensorReading():minMaxRangeValid(true)
  {
  }

  double minValue;
  double maxValue;
  bool minMaxRangeValid;
  std::vector<int> dimensions;
  std::vector<std::string> descriptions;
  std::string unitForDisplay;
  std::string fullSensorName;
  std::string portName;

  enum SensorType
  {
    boolSensor,
    doubleSensor,
    cameraSensor,
    doubleArraySensor,
    viewSensor,
    noSensor
  } sensorType;

  union Data
  {
    bool boolValue;
    double doubleValue;
    const double* doubleArray;
    const unsigned char* byteArray;
  } data;

  void operator = (const SensorReading& other)
  {
    minValue = other.minValue;
    maxValue = other.maxValue;
    dimensions = other.dimensions;
    descriptions = other.descriptions;
    unitForDisplay = other.unitForDisplay;
    portName = other.portName;
    sensorType = other.sensorType;
    minMaxRangeValid = other.minMaxRangeValid;
    // Do not copy data and names! They are set elsewhere
  }
};

#endif //API_DATATYPES_H_

