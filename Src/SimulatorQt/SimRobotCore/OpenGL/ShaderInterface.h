/**
* @file ShaderInterface.h
* 
* Definition of an interface to shader definitions and the shader and program classes
*
* @author <A href="mailto:pachur@informatik.uni-bremen.de">Dennis Pachur</A>
*/ 
#ifndef SHADERINTERFACE_H_
#define SHADERINTERFACE_H_

#include <OpenGL/ShaderProgram.h>
#include <OpenGL/Shader.h>

class GraphicsManager;
class FramebufferObject;

typedef std::map<std::string, ShaderProgram*>::iterator ProgIter;
typedef std::map<std::string, Shader*>::iterator ShadIter;

class ShaderInterface
{
protected:
  GLboolean    initialized;
  GLboolean    usable;
  bool         postShaderUsable;
  bool         globalShaderUsable;
  bool         running;
  bool         showInfos;
  ProgIter     runningProgram,
               postProgram,
               currentProgram,
               previousProgram,
               nextProgram,
               initialProgram;
  ShadIter     currentShader,
               previousShader,
               nextShader;
  ProgramInfo  programInfoState;
  ShaderInfo   shaderInfoState;

  GLdouble     original_fov;
  GLuint       original_width;
  GLuint       original_height;

  // framebuffer objects
  std::map<int, FramebufferObject*> fbos;
  int          fbo_debug;
  bool         fbo_storage;
  bool         fbo_antialiased;
  int          fbo_samples;
  bool         fbo_oversized;
  int          currentFBO;
  int          fboInit;

  // additional textures
  std::vector<GLTexture*> staticTextures;

  GraphicsManager* simgraphics;

  // a map of shader names to shader objects
  std::map<std::string, Shader*> shaders;

  // a map of shader program names to shader programs
  std::map<std::string, ShaderProgram*> programs;
  // a map of filter program names to shader programs
  std::map<std::string, ShaderProgram*> filters;

  /**
  * Get the next defined program
  * @param iter The iter to the current selected program
  * @return An iter to the next defined program
  */
  ProgIter getNextProgram(ProgIter iter);

  /**
  * Get the previous defined program
  * @param iter The iter to the current selected program
  * @return An iter to the previous defined program
  */
  ProgIter getPreviousProgram(ProgIter iter);

  /**
  * Get the next defined shader
  * @param iter The iter to the current selected shader
  * @return An iter to the next defined shader
  */
  ShadIter getNextShader(ShadIter iter);

  /**
  * Get the previous defined shader
  * @param iter The iter to the current selected shader
  * @return An iter to the previous defined shader
  */
  ShadIter getPreviousShader(ShadIter iter);

  /**
  * Render renderbuffers to seperate surfaces for debug purposes
  * (function is called from within drawPostSurface() )
  */
  void drawDebugPostSurfaces();

  /**
  * Little texture manager function: be aware of duplicate static images
  * @param file The name of the texture to load
  */
  GLTexture* getTexture(const std::string& filename);

public:
  ShaderInterface(GraphicsManager* sg);
  ~ShaderInterface();

  /** create a shader object
  * @param name The name of the shader
  * @param type The type of the shader
  * @param filename The filename of the shadersource
  */
  void createShader(std::string& name, GLenum& type, std::string& filename);

  /** create a program
  * @param name The name of the program
  * @param extensionName Mark the program to use the fbo settings of the extension
  * @param isFilter A flag if the program is just a post production integrated filter
  */
  void createProgram(const std::string& name, const std::string& extensionName, const bool isFilter = false);

  // get the sources and build the stuff
  void createShadersAndPrograms();

  /** attach a shader to a program
  * @param pname the name of the program
  * @param sname the name of the shader
  */
  void attachShader(const std::string& pname, const std::string& sname);

  /** bind a filter to a post program
  * @param pname the name of the program
  * @param fname the name of the filter
  */
  void attachFilter(const std::string& pname, const std::string& fname);

  /**
  * Adds a uniform variable to a shader program
  * @param pname The name of the program
  * @param uname The name of the predefined uniform variable
  * @param swapTarget Target must be a texture (as well as uname)
  * @param type Type of uniform
  * @param file File information if a texture has to be loaded
  * @param ignore If the uniform is being ignored no glid is retrieved
  *               but the uniform acts as normal fbo target
  * @param filter Filter method if texture (linear or nearest)
  */
  void addUniform(const std::string& pname, const std::string& uname,
                  std::string& swapTarget, const std::string& type,
                  const std::string& file, const bool& ignore,
                  const std::string& filter);

  /**
  * Set the buffer which should be activated when rendering to fbos
  * @param pname The name of the program
  * @param bitmask A simple bitfield which specifies the buffer to enable
  * @param clearbitmask Another bitfield which specifies the buffer to clear before rendering
  */
  void setMRT(const std::string& pname, int bitmask, int clearbitmask);

  /**
  * Handle the uniforms defined with the currently active shader program.
  * Provide global uniform values (function is called by geometric objects).
  * @param obj The physical object
  * @param the program to use (either running or post)
  */
  void handleUniforms(PhysicalObject* obj, bool usePostProgram = false);

  /**
  * Initialize the shader interface
  * Has to be done only once (but returns automatically if called again)
  */
  void init();

  /**
  * Toggle shader interface on or off
  * The shader interface stays of if it is not supported by the graphics hardware
  */
  void toggleRunning();

  /**
  * set shader interface on or off
  * The shader interface stays of if it is not supported by the graphics hardware
  * @param state The activation state
  */
  void setRunning(bool state);

  /**
  * Check if the shader interface is currently running
  * @return if is running or not
  */
  bool getRunning() const
  { return running; }

  /**
  * Reload and recompile the shader sources
  * @param killFBOs flag if the frame buffer objects should be destroyed
  */
  void reloadShader(const bool killFBOs = true);

  /**
  * Switch the current selected program
  * @param previous Flag if switch forward or backward
  */
  void switchProgram(const GLboolean previous);

  /**
  * Switch the current selected shader
  * @param previous Flag if switch forward or backward
  */
  void switchShader(const GLboolean previous);

  /**
  * Define the current global program
  * @param immediate_usage The override flag to switch shader from inside the application
  */
  void defineGlobalProgram(const bool immediate_usage = false);

  /**
  * Define the current program as post production program
  * @param immediate_usage The override flag to switch shader from inside the application
  */
  void definePostProgram();
  bool getPostProgramDefined();
  /**
  * Render information about shader and programs to gui
  * @param width The width of the display
  * @param width The height of the display
  */
  void renderInfos(GLuint width, GLuint height) const;

  /**
  * Toggle shader interface gui on or off
  */
  void toggleShowInfos();

  /**
  * Get the flag if the shader interface gui is currently active
  * @return the activation flag
  */
  bool getShowInfosState() const;

  /**
  * Toggle through the information tabs of the shader interfaces gui
  * @param previous flag if previous tab or next tab
  */
  void switchInfo(const GLboolean previous);

  /**
  * Get a pointer to the shader program (its fast than string compare each time)
  * @param name The name of the shader program
  * @return A pointer to the shader program
  */
  ShaderProgram* getShaderProgram(const std::string& name);

  /**
  * Enable a global working shader (manually: shader interface must be running)
  * @param pname The name of the shader to be activated
  * @return true if the program has been found and could be activated
  */
  bool enableGlobalShader(const std::string& pname);

  /**
  * Set a global working shader (set, but use shader when activated)
  * @param pname The name of the shader to be activated
  * @param immediate_usage The override flag to switch shader from inside the application
  */
  void setGlobalShader(const std::string& pname, const bool immediate_usage = false);

  /**
  * Set a post production shader (set, but use shader when activated)
  * @param pname The name of the shader to be activated
  * @param immediate_usage The override flag to switch shader from inside the application
  */
  void setPostShader(const std::string& pname, const bool immediate_usage = false);

  /**
  * Enable a shader for a sequence of objects
  * @param pname The shader programs name
  */
  void enableLocalShader(const std::string& pname);

  /**
  * Enable a shader for a sequence of objects
  * @param obj A pointer to a physical object
  */
  void enableLocalShader(PhysicalObject* obj) const;

  /**
  * Disable a previously enabled local shader
  */
  void disableLocalShader() const;

  /**
  * Init a single Frame buffer object for post-process shader effects
  * fbo_reg The frame buffer object registration identifier
  * @return true if the fbo is usable
  */
  bool initFramebufferObject(const int fbo_reg);

  /**
  * Destroy all frame buffer objects
  */
  void destroyFramebufferObjects();

  /**
  * Prepare the post production runthrough (render to fbo)
  * @param width The width of the surface
  * @param height The width of the surface
  * @param fov The aspect of the frustrum
  * @param fbo_reg The fbo to bind
  */
  void preparePostProduction(GLuint& width, GLuint& height,
                             GLdouble& fov, const int fbo_reg = -1);

  /**
  * Render to visable frame buffer using a post production shader
  * @param isObjectWindow needed when using oversized fbos (projection matrix for
  *                       selection has to be reset)
  */
  void drawPostSurface(const bool isObjectWindow);
  /**
  * Render to the frame buffer using a post production shader and
  * reenable the global shader again (use this method for render pass post production
  */
  void drawPostSurface();

  /**
  * Render to current frame buffer using a post production shader (filter pass)
  * @param filter A pointer to the shader program
  */
  void applyKernel(ShaderProgram* filter);

  /**
  * forbid frame buffer objects if accumulation buffers are used
  */
  void postShaderUsage(const bool state)
  { postShaderUsable = state; }

  /**
  * forbid global shader usage if shader are already fixed
  */
  void globalShaderUsage(const bool state)
  { globalShaderUsable = state; }

  /**
  * get the multisampled frame buffer object state
  */
  bool getMultisampledFBOsState();

  /**
  * toggle the multisampled frame buffer object state
  * @param samples The number of samples for the multisample option
                   (must be the same as the default frame buffer)
  */
  void toggleMultisampledFBOsState(int samples);

  // set / get FBO debug flag
  void setDebugFBOs(const int debugState);
  int getDebugFBOsState() const;

  // get id of previous model view projection matrix uniform
  int getPreviousMVPMID();

  // toggle or get the oversize flag of the frame buffer objects
  void toggleOversizedFBOs();
  bool getOversizedFBOsState();
  void setOriginalViewport(const bool isObjectWindow);

  // swaping uniform ids of textures (render pass or physic update tagged uniforms)
  void swapRPRenderTargets();
  void swapPURenderTargets();

  // copy the current framebuffer to a texture
  // @param fbo_reg the frame buffer object registration id
  void copyFramebufferToTexture(const int fbo_reg);
};

#endif // SHADERINTERFACE_H_
