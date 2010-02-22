/**
* @file SimGraphics.h
* 
* Definition of class GraphicsManager and several datatypes
*
* @author <A href="mailto:pachur@informatik.uni-bremen.de">Dennis Pachur</A>
* @author <A href="mailto:timlaue@informatik.uni-bremen.de">Tim Laue</A>
*/ 

#ifndef SIMGRAPHICS_H_
#define SIMGRAPHICS_H_

#include <map>
#include <list>
#include <Tools/SimMath.h>
#include <Tools/Surface.h>
#include <OpenGL/Light.h>
#include <OpenGL/Environment.h>
#include <OpenGL/GLHelper.h>
#include <OpenGL/FPS_Counter.h>
#include <OpenGL/ShaderInterface.h>

class AxisAlignedBoundingBox;
class GraphicsManager;

/** Special type for storing vectors with names and texture coordinates*/
typedef std::map<std::string, Vertex> NameToVertexMap;

/** Special type for storing vectors with names*/
typedef std::map<std::string, Vector3d> NameToVectorMap;

/** Special type for handles*/
typedef unsigned int GraphicsPrimitiveHandle;

/** A type to distinguish between different primitives*/
enum PrimitiveType {TRIANGLE_STRIP, POLYGON, QUAD, TRIANGLE, GRAPHICS_BOX, 
GRAPHICS_SPHERE, GRAPHICS_CYLINDER, GRAPHICS_CAPPED_CYLINDER, ENVIRONMENT};

/** The maximal number of surface styles
* refer to APIDatatypes.h */
const unsigned int SURFACESTYLESIZE = 9;

/**
* @class PreGraphicsPrimitiveDescription
* A description of polygonal meshes with references through vertex names.
* This class is used to get a clue of the topological order of the vertices.
* In fact these objects are only temp objects and have to be converted.
*/
class PreGraphicsPrimitiveDescription
{
public:
  /** Constructor */
  PreGraphicsPrimitiveDescription(){};

  /** Type of the PrePrimitive (Polygon or TriangleStrip) */
  PrimitiveType type;
  /** A List of the vertexNames */
  std::vector<std::string> vertexNames;
  /** The surface normals of the surfaces */
  std::vector<Vector3d> surfaceNormals;
};

/** 
* @class GraphicsPrimitiveDescription
* A description of a graphics primitive to be drawn
*/
class GraphicsPrimitiveDescription
{
public:
  /** Constructor */
  GraphicsPrimitiveDescription()
  {
    vertices = 0;
    vertexNormals = 0;
    textureCoordinates = 0;
    numTextureCoordinates = 0;
    normals = 0;
    numberOfVertices = 0;
    type = POLYGON;
    displayListHandle = 0;
    graphicsManager = 0;
  }

  /** Compile a display list of this primitive*/
  void compileDisplayList();

  /** Draws by calling the display list*/
  void callDisplayList();

  /** Destroys the display list*/
  void destroyDisplayList();

  /** Expands a bounding box to completely include the object
  * @param box The bounding box
  * @param position The position of the object
  * @param rotation The rotation of the object
  */
  void expandAxisAlignedBoundingBox(AxisAlignedBoundingBox& box,
    const Vector3d& position,
    const Matrix3d& rotation) const;

  /** The components of the vertices*/
  double* vertices;
  /** The normals of the components */
  double* vertexNormals;
  /** The texture coordinates */
  double* textureCoordinates;
  /** The number of texture coordinates ( 0 | 1 | 2 ) */
  unsigned char numTextureCoordinates;
  /** The Surface Normals*/
  double* normals;
  /** The number of vertices*/
  unsigned int numberOfVertices;
  /** The type of the primitive*/
  PrimitiveType type;
  /** The handle for a display list*/
  GLuint displayListHandle;
  /** A pointer to the GraphicsManager*/
  GraphicsManager* graphicsManager;
};


/**
* @class VertexList
* A storage class for a set of named vertices
*/
class VertexList
{
private:
  /** A look-up table to assing names to vertices*/
  NameToVertexMap vertices;
  /** The normals of the vertices */
  NameToVectorMap vertexNormals;
  /** A counter of normals added to a vertex (for normalizing purposes) */
  std::map<std::string, int> counter;
  /** flag if vertex list supports texture coordinates */
  unsigned char numTextureCoordinates;

public:
  /** Default Constructor*/
  VertexList(unsigned char numTextureCoordinates_)
  {
    numTextureCoordinates = numTextureCoordinates_;
  }

  /** Copy-Constructor
  * @param other Another AttributeValuePair
  */
  VertexList(const VertexList& other)
  {
    numTextureCoordinates = other.numTextureCoordinates;
    vertices = other.vertices;
  }

  /** Adds a new vertices to the list
  * @param attributes The attributes to be parsed
  */
  void addVertex(const AttributeSet& attributes);

  /** Returns a vertices from the list
  * @param name The name of the vertex
  * @param vertex The vertex to be returned
  * @return true, if the vertex has been found
  */
  bool getVertex(const std::string& name, Vertex& vertex);

  /** Returns a normal from the list
  * @param name The name of the vertex
  * @param normal The normal to be returned
  * @return true, if the vertex has been found
  */
  bool getVertexNormal(const std::string& name, Vector3d& normal);

  /** Add a surface normal of a vertex to his normal stack
  * @param name The name of the vertex
  * @param normal The surfaceNormal
  */
  void addSurfaceNormal(const std::string& name, const Vector3d& normal);

  /** Calculate the vertex normal by average the surface normals */
  void flipVertexNormals();
  void calculateVertexNormals();

  /** Clear all vertex normals of all vertices*/
  void clearVertexNormals();

  /** Return if the vertex list has texture coordinates assigned with the vertices
  * @return The number of texture coordinates that have been assigned
  */
  unsigned char getNumTextureCoordinates()
  { return numTextureCoordinates; };
};

/**
* @class GraphicsManager
*
* A class for storing vertex sets and drawing
* primitives
*/
class GraphicsManager
{
private:

  /* GLEW */
  bool glewInitialized;
  GLenum glewError;
  std::string glewErrorString;

  /* Lights */
  std::vector<Light*> lights;
  bool showLightCones,
       firstShadowRendering,
       copyTexImage;
  std::vector<Light*>::iterator shadowMapIter, lastLightIter;
  GLfloat globalAmbientLight[4];

  /* VertexLists / Vertex Buffer Objects */
  VertexList* currentVertexList;
  std::vector<Vector3d> newPrimitiveVertexBuffer;
  std::map<std::string, VertexList*> vertexLists;
  std::vector<GraphicsPrimitiveDescription> primitives;
  GraphicsPrimitiveDescription currentGraphicsPrimitive;
  /** Some Helper for normal vector calculation */
  std::vector<PreGraphicsPrimitiveDescription> preprimitives;
  PreGraphicsPrimitiveDescription currentPrePrimitive;

  /** Some default OpenGL settings which could be replaced by parsing them from file */
  GLenum      GLSETTINGS_LightModel_twoSided;
  GLenum      GLSETTINGS_LightModel_localCameraViewer;
  GLenum      GLSETTINGS_LightModel_localObjectViewer;
  GLenum      GLSETTINGS_UseBaseColors;
  GLint       GLSETTINGS_Antialiasing_Samples;
  GLenum      GLSETTINGS_Culling_face;
  GLenum      GLSETTINGS_Culling_front;
  GLenum      GLSETTINGS_Material_face;
public:
  GLboolean   GLSETTINGS_Disable_WGL_Share_Lists;
private:
  GLfloat     GLSETTINGS_PolygonOffset_factor;
  GLfloat     GLSETTINGS_PolygonOffset_units;
  GLboolean   GLSETTINGS_Shader_InitASAP;
  std::string GLSETTINGS_Filename;

  /** A class for counter frames per second */
  std::map<int, FPS_Counter*> fpsCounter;
  std::string lastProgramName;

  /* general information */
  bool displayListsHaveBeenCompiled;
  GLint nextFreeTextureID;
  Vector3d* previous_positions;
  Matrix3d* previous_rotations;
  int cycle_order_offset;
  Vector3d* cam_previous_positions;
  Matrix3d* cam_previous_rotations;
  int cam_cycle_order_offset;

  /** additional stuff */
  unsigned int numOfTotalPrimitives, numOfTotalVertices;
  bool numOfTotalPrimitivesIsSet, numOfTotalVerticesIsSet;
  Environment* environment;
  unsigned int lastScreenshotID;

  /** motion blur stuff */
  float exposure2Physic;
  float exposureTime;

  /* Prepare Object Drawing
  * @param position The Position of the Object
  * @param rotation The Rotation of the Object
  * @param surface The surface definition including color and texture information
  * @param inverted Inverted flag to emphasis this object as being selected
  * @param drawForSensor Invisibility ignoration flag
  * @param generateTexCoordinates Flag if texture coordinates should be generated
  * @param visParams the visual parameter set
  */
  void prepareObjectDrawing(const Vector3d& position, const Matrix3d& rotation, 
                            Surface& surface, bool inverted, bool drawForSensor,
                            bool generateTexCoordinates,
                            const VisualizationParameterSet& visParams);

  /* Finish Object Drawing
  * @param surface The surface object of the primitive
  * @param generateTexCoordinates Flag if texture coordinates should be generated
  */
  void finishObjectDrawing(Surface& surface, bool generateTexCoordinates);

  /* Prepare Object Drawing
  * @param position The Position of the Object
  * @param rotation The Rotation of the Object
  * @param surface The surface definition including color and texture information
  */
  void prepareQuickObjectDrawing(const Vector3d& position, const Matrix3d& rotation, 
                                 Surface& surface);

  /* Finish Quick Object Drawing (used by shadow mapping) */
  void finishQuickObjectDrawing();

  /** Compile Display Lists for State Changes
  * wireframe, flat, smooth or texture shading
  */
  void compileStateChangeDisplayLists();

  /** Initialize OpenGL States and compile display lists */
  void init();

  /** Enable all OpenGL Features for wireframe mode */
  void enableWireframeMode();

  /** Disable all OpenGL Features of wireframe mode */
  void disableWireframeMode();

  /** Enable all OpenGL Features of flat shading mode */
  void enableFlatShadingMode();

  /** Disable all OpenGL Features of flat shading mode */
  void disableFlatShadingMode();

  /** Enable all OpenGL Features of smooth shading mode */
  void enableSmoothShadingMode();

  /** Disable all OpenGL Features of smooth shading mode */
  void disableSmoothShadingMode();

  /** Enable all OpenGL Features of texture shading mode */
  void enableTextureShadingMode();

  /** Disable all OpenGL Features of texture shading mode */
  void disableTextureShadingMode();

  /** Enable all OpenGL Features of camera simulation mode */
  void enableCameraSimulationMode();

  /** Disable all OpenGL Features of camera simulation mode */
  void disableCameraSimulationMode();

  /** Enable all OpenGL Features for increasing speed of texture mapping */
  void enableShadowMappingMode();

  /** Disable all OpenGL Features for increasing speed of texture mapping */
  void disableShadowMappingMode();

  /** Helper Function: Get a light from reference name
  * @param name The name of the light
  * @return A pointer to the light
  */
  Light* getLightByName(const std::string& name);
  /* Helper Function: Get an unused lightID
  * @return The unused lighID or GL_NONE (0x0000)
  */
  GLenum getUnusedLightID();

public:
  /** Constructor
  * @param filename The filename of the currently opened file
  */
  GraphicsManager(const std::string& filename);

  /** Destructor */
  ~GraphicsManager();

  /** modelview matrix */
  GLdouble mvmatrix[16];
  /** projection matrix */
  GLdouble projmatrix[16];
  /** viewport matrix */
  GLint viewport[4];

  // public opengl settings
  Vector3d GLSETTINGS_CubeMap_CamPos;
  GLuint   GLSETTINGS_CubeMap_Size;
  GLdouble GLSETTINGS_CubeMap_ClipNear;
  GLdouble GLSETTINGS_CubeMap_ClipFar;
  GLuint   GLSETTINGS_CubeMap_jitterEnd;
  GLuint   GLSETTINGS_CubeMap_jitterIndex;
  GLuint   GLSETTINGS_CubeMap_UpdateRate;
  GLuint   GLSETTINGS_CubeMap_UpdateCycle;
  bool showFPS;

  /** Multitexturing */
  unsigned int currentTextureUnitOffset;
  bool texturingIsEnabled;

  /** dynamic reflectivity */
  SimObject* reflectionObject;
  /** A class for handling the shader language of opengl */
  ShaderInterface* shader;

  /** render settings */
  int    currentWidth;
  int    currentHeight;
  double currentFOV;
  double currentAspect;
  double currentNearClip;
  double currentFarClip;

  /** question if it is time for an environment mapping update
  *   if so: the according settings are set
  * @return true if environment mapping should be done
  */
  bool doEnvRendering();

  // Helper for counting fps
  void beginFPSCount(const int fbo_reg);
  void endFPSCount(const int fbo_reg);

  // Helper for info messages
  unsigned int getNumOfTotalPrimitives();
  unsigned int getNumOfTotalVertices();
  unsigned int getNumVertForCS(GraphicsPrimitiveHandle gph) const;

  /** Parse OpenGL settings
  * @param name The name of the attribute element
  * @param attributes The attributes of the element
  * @param line The line in the scene description
  * @param column The column in the scene description
  * @param errorManager An object managing errors
  */
  void parseOpenGLSettings(const std::string& name, const AttributeSet& attributes,
                           int line, int column, ErrorManager* errorManager);

  /** Initializes the currently selected OpenGL context (and adds displaylists if required).
  * @param hasSharedDisplayLists Whether the currently selected context has shared displaylists
  */
  void initContext(bool hasSharedDisplayLists);

  /** Display List IDs to enable surface style according OpenGL features */
  std::map<VisualizationParameterSet::SurfaceStyle, GLuint> enableStateDL;
  /** Display List IDs to disable surface style according OpenGL features */
  std::map<VisualizationParameterSet::SurfaceStyle, GLuint> disableStateDL;

  /** Prepare light to be enabled later and set according settings
  * @param name The name of the light
  * @param name The name of a sub macro of the current simObject (or "")
  * @param simObject A object to bind the light to (pass NULL to avoid binding)
  */
  unsigned char preEnableLight(const std::string& name, const std::string& objName, SimObject* simObject);
  unsigned char preEnableLight(const std::string& name);

  /** Get global ambient light */
  float* getGlobalAmbientLight()
  { return globalAmbientLight; };

  /** Set global ambient light */
  void setGlobalAmbientLight()
  { glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbientLight); };

  /** Reenable lights */
  void reenableLights();
  void dimmLights();
  void undimmLights();

  /** Enable all prepared lights (OpenGL Context must be present) */
  void enableLights();

  /** Disable light */
  void disableLight(const std::string& name);

  /** Disable all lights */
  void disableAllLights();

  /** Define a new light */
  void defineLight(Light* light);

  /** Check number of lights that are currently used
  * @param onlyShadowMapped If only shadow mapped lights should be counted
  * @return The number of lights currently enabled
  */
  int getNumLightsOn(bool onlyShadowMapped = false) const;

  /** Get number of lights
  * @return The number of lights totally defined
  */
  size_t getNumLights() const
  { return lights.size(); }

  /** Get the names of the lights
  * @return The names of the lights
  */
  std::string getNamesOfLights() const;

  /** Get the light show cones flag state
  * @return If the light show cones flag is checked
  */
  bool getLightShowCones() const
  { return showLightCones; }

  /** Toggle the light show cones flag */
  void toggleLightShowCones()
  { showLightCones ^= 1; }

  /** Get a specific light by its OpenGL ID
  * @return A pointer to the requested light or NULL
  */
  Light* getLightByID(GLenum lightID);

  /** Get the light vector
  * @return The light vector
  */
  const std::vector<Light*> getLightVector()
  { return lights; };

  /** Update Lights
  * @param useShadows Flag if shadows should be drawn
  * @param dimmPass Flag if 1. render pass of software shadowmapping
  * @param shader Flag if a shader is using the shadowmap
  */
  void updateLights(bool useShadows, bool dimmPass, bool shader);

  void revertLightTextureUnits(bool shader);

  /** Changes CameraPos to look from light source (switches through light sources)
  * @param cameraPos The vector of the camera position
  * @param cameraTarget The vector of the camera target
  */
  void camToLightPos(Vector3d& cameraPos, Vector3d& cameraTarget) const;

  /** Prepare the next light in row for its render pass (draw the shadow map)
  * @param visParams the visual parameter set
  * @return If there are more lights to be prepared
  */
  bool prepareLightForShadowMapping(VisualizationParameterSet& visParams);

  /** Get the size of the current light shadow map
  * @return The shadow map size of the current processed light
  */
  unsigned int getCurrentShadowMapSize();

  /** Reset shadow map iteration */
  void resetShadowMapIter();

  /** Returns true if a shadow mapped light source is used
  * @return Flag if a shadow mapped light source is used
  */
  bool shadowMappingIsUsed();

  /** Set a new environment
  * @param env The Environment Definition
  */
  void setEnvironment(Environment* env)
  { environment = env; };

  /** Get the current used environment
  * @return The Environment Definition
  */
  Environment* getEnvironment()
  { return environment; };

  /** Get the current image dimensions
  */
  void getCurrentImageDimensions(int& height, int& width) const;

  /* Render the scene environment
  * @param farClippingPlaneDistance The distance of the far clipping plane of the current perspective
  */
  void renderEnvironment(double farClippingPlaneDistance);

  void checkDisplayListRequirements();

  double distanceToFarthestPoint(const std::list<GraphicsPrimitiveHandle>& handles);

  bool intersectSetOfPrimitivesWithRay(const Vector3d& rayPosition, const Vector3d& ray,
                                       const Vector3d& objPosition, const Matrix3d& objRotation,
                                       const std::list<GraphicsPrimitiveHandle>& handles, Vector3d& intersection);

  // Functions for adding new vertices: ****************************

  void addVertex(const AttributeSet& attributes)
  {
    currentVertexList->addVertex(attributes);
  }

  void calculateNormalsOfVertexList()
  {
    currentVertexList->calculateVertexNormals();
  }

  void newVertexList(const std::string& name, unsigned char numTextureCoordinates);

  void setActiveVertexList(const std::string& listName);

  // Functions for adding new primitives: **************************

  void createNewQuadPrePrimitive(const AttributeSet& attributes, bool flipSurfaceNormal);

  void createNewTrianglePrePrimitive(const AttributeSet& attributes, bool flipSurfaceNormal);

  void createNewPrePrimitive(PrimitiveType type_);

  void addVertexToCurrentPrePrimitive(const std::string& vertexName);

  void currentPrePrimitiveHasBeenFinished();

  void flipVertexNormals();
  void calculateVertexNormals();

  void clearVertexNormals();

  void combinePrePrimitives();

  void flipVertexOrder();
  GraphicsPrimitiveHandle convertPrePrimitive(bool averageNormals);

  bool hasPrePrimitives()
  { return !(preprimitives.empty()); };

  GraphicsPrimitiveHandle createNewBox(const Vector3d corners[]);

  GraphicsPrimitiveHandle createNewSphere(double radius);

  GraphicsPrimitiveHandle createNewCylinder(double radius, double height, bool capped=false);


  // Drawing functions: ********************************************

  /** standard object drawing call */
  void drawPrimitive(GraphicsPrimitiveHandle handle, const Vector3d& position, const Matrix3d& rotation, 
                     Surface& surface, bool inverted, bool drawForSensor,
                     const VisualizationParameterSet& visParams);

  /** standard objects drawing call */
  void drawPrimitives(const std::list<GraphicsPrimitiveHandle>& handles,const Vector3d& position, 
                      const Matrix3d& rotation, Surface& surface, bool inverted, bool drawForSensor,
                      const VisualizationParameterSet& visParams);

  /** Object drawing for shadow mapping */
  void quickDrawPrimitive(GraphicsPrimitiveHandle handle, const Vector3d& position,
                          const Matrix3d& rotation, Surface& surface);

  /** Objects drawing for shadow mapping */
  void quickDrawPrimitives(const std::list<GraphicsPrimitiveHandle>& handles,const Vector3d& position, 
                           const Matrix3d& rotation, Surface& surface);

  // Functions needed by physics:
  const GraphicsPrimitiveDescription* getGraphicsPrimitive(GraphicsPrimitiveHandle handle) const;

  GLUquadricObj* q;

  // Additional Utility Functions: ********************************************

  /** Save the current object window image to a file
  * @param width The width of the current viewport
  * @param height The height of the current viewport
  * @param name The filename to save the image to
  * @param generateFilename if a unique filename should be generated
  */
  void saveScreenshot(unsigned int width, unsigned int height,
    std::string& name, bool generateFilename);

  /** Accumulated glFrustrum function for depth of field and antialiasing
  * @param left
  * @param right
  * @param bottom
  * @param top
  * @param zNear
  * @param zFar
  * @param pixdx
  * @param pixdy
  * @param eyedx
  * @param eyedy
  * @param focus
  */
  void accFrustrum(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top,
                   GLdouble zNear, GLdouble zFar, GLdouble pixdx, GLdouble pixdy,
                   GLdouble eyedx, GLdouble eyedy, GLdouble focus);

  /** Accumulated gluPerspective function for depth of field and antialiasing
  * @param zNear
  * @param zFar
  * @param pixdx
  * @param pixdy
  * @param eyedx
  * @param eyedy
  * @param focus
  */
  void accPerspective(GLdouble zNear, GLdouble zFar,
                      GLdouble pixdx, GLdouble pixdy, GLdouble eyedx, GLdouble eyedy,
                      GLdouble focus);

  /** set perspective by previously set values */
  void setPerspective();
  void setCubeMapPerspective();
  void setCubeMapCamera(const int generateCubeMap, const Vector3d& pos);

  /** determine if cube mapping is dynamic or a static call and write data to textures or files
  * @param cubeMapSide The cube map side of the current render
  * @param filename The name of the current document
  */
  void handleCubeMapping(int cubeMapSide, const std::string& filename);

  /** check if a specific motion blur mode is executable by the grahics hardware
  * @param mode The motion blur mode to check
  * @return true If the motion blur mode is exectuable
  */
  bool checkMotionBlurCapability(const MotionBlurMode mode);

  /** get the number of render passes needed for applying the motion blur setting
  * @param visParams The Visualization Parameter Set (to hold the values)
  * @return The number of render passes needed
  */
  void setUpMotionBlurSettings(VisualizationParameterSet& visParams);

  /** set the pointer to the previous positions and rotations (motion blur)
  * @param pp A pointer to the previous positions
  * @param pr A pointer to the previous rotations
  * @param coo The offset in the array
  */
  void setMBInfos(Vector3d* pp, Matrix3d* pr, const int& coo);

  /** set the pointer to the previous positions and rotations of camera (motion blur)
  * @param cam_pp A pointer to the previous positions (of the camera)
  * @param cam_pr A pointer to the previous rotations (of the camera)
  * @param cam_coo The offset in the array (of the camera)
  */
  void setCamMBInfos(Vector3d* cam_pp, Matrix3d* cam_pr, const int& cam_coo);

  /** set / unset changes to the shader interface and internal states
  * motionBlurMode The last motion blur mode
  */
  void unsetMotionBlurMode(const MotionBlurMode motionBlurMode);
  void setMotionBlurMode(const MotionBlurMode motionBlurMode);

  // request / toggle multisampled fbos capability / state
  bool getMultisampledFBOsState() const;
  bool checkMultisampledFBOCapability() const;
  void toggleMultisampledFBOs();

  /** load the previous model view projection matrix into uniform location */
  void setupPreviousMVPM();

  /** Set the option to interpolate the pyhsic updated positions and rotations
  *   to fit the exposure time of the imaging sensor (only for the next frame)
  * @param exposureTime The time of the sensor exposure
  * @param simStepLength The time of a single physic update (update of previous pos and rot)
  */
  void setExposure(const double exposureTime, const double simStepLength);

  // get the choosen number of samples for antialising
  int getAntialiasingSamples();

  // get the exposure to physics calculation factor
  float getExposure2Physic() const
  { return exposure2Physic; }

  // get the ExposureTIme
  float getExposureTime() const
  { return exposureTime; }

  // prepare the rendering (and do all those crappy assignments inside the graphics manager)
  void getShaderUsage(VisualizationParameterSet& visParams);
  void prepareRendering(const VisualizationParameterSet& visParams, GLuint width, GLuint height, GLdouble aspect,
                        GLdouble fov, GLdouble nearClip, GLdouble farClip, const int fbo_reg);
  void finishRendering(const VisualizationParameterSet& visParams, const bool isObjectWindow);
};


#endif //SIMGRAPHICS_H_
