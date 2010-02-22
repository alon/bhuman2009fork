/**
* @file ShaderProgram.h
* 
* Definition of opengl glsl shader program class
*
* @author <A href="mailto:pachur@informatik.uni-bremen.de">Dennis Pachur</A>
*/

#ifndef SHADERPROGRAM_H_
#define SHADERPROGRAM_H_

#include <OpenGL/GLtexture.h>
#include <OpenGL/Constants.h>
#include <vector>
#include <map>
#include <set>

class PhysicalObject;
class FramebufferObject;
class Shader;

enum ProgramInfo {
  PROGRAM_LOG = 0,
  PROGRAM_DESCRIPTION,
  PROGRAM_SHADER,
  PROGRAM_UNIFORMS,
  PROGRAM_FILTERS,
};

struct UniformInfo {
  Uniform     ID;
  UniformType type;
  GLint       glID;
  Uniform     swapID;
  GLTexture*  tex;
  GLboolean   ignore;
  GLenum      texFilter;

  UniformInfo(const Uniform ID_, const UniformType type_,
              const Uniform swapID_, GLTexture* tex_, const GLboolean ignore_,
              const GLenum texFilter_)
    : ID(ID_), type(type_), glID(-1), swapID(swapID_), tex(tex_), ignore(ignore_),
    texFilter(texFilter_)
  {};
};

typedef std::vector<UniformInfo> UniformInfos;
typedef std::vector<UniformInfo>::iterator UniformInfosIter;
typedef std::vector<UniformInfo>::const_iterator ConstUniformInfosIter;

class ShaderProgram
{
protected:
  std::vector<Shader*> attachedShader;
  std::vector<ShaderProgram*> filters;
  ShaderProgram*       extension;
  std::string          name;
  GLchar*              description[20];
  GLint                numLines;
  GLuint               id;
  GLchar*              log;
  GLchar*              secondlog;
  GLboolean            build;
  GLboolean            isFilter;
  UniformInfos         uniforms;
  int                  numFboTextures;
  int                  numActiveFboTextures;
  int                  numStorageTextures;
  int                  numTextures;
  int                  MRT_count, MRT_clearcount;
  std::vector<int>     MRT_index, MRT_clearindex;
  GLenum               *bufferArray, *clearbufferArray;

  /**
  * Collect the unique fbo/non-fbo texture ids from assigned filters
  * @param textures The current collection of unique uniform ids
  */
  void getTextureIDs(std::set<Uniform>& textures);
  void getFboTextureIDs(std::set<Uniform>& textures);
  void getActiveFboTextureIDs(std::set<Uniform>& textures);
  void getStorageTextureIDs(std::set<Uniform>& textures);
  bool countTextures();

public:
  bool RPuniformsSwapped;
  bool PUuniformsSwapped;

  /** Create a new shader program
  * @param name_ the name of the program
  */
  ShaderProgram(const std::string& name_);
  ShaderProgram();
  ~ShaderProgram();

  /* get the shader programs name
  * @return the shader programs name
  */
  std::string& getName();

  /** Attach a shader
  * @param shader A pointer of the shader to be attached
  */
  void attachShader(Shader* shader);

  /** Bind a filter program
  * @param filter A pointer of the shader program to be bound
  */
  void attachFilter(ShaderProgram* filter);

  /** Create a shader from the previosly set states
  * @return 0 if everything is fine, 1 if failure in program creation, 2 if failure in filter creation
  */
  int create();

  /** destroy every memory allocation and reset variables */
  void destroy();

  /** get the id given by opengl
  * @return the program id
  */
  GLuint getId() const;

  /** check if program is usable
  * @return the usability-flag of the program
  */
  GLboolean usable() const;

  /** set program to be a post production filter 
  * @param filterflag The flag if it should be handled as filter
  *        (filter depend on a usable post program)
  */
  void setFilter(const bool filterFlag);

  /** get the attached filter programs*/
  std::vector<ShaderProgram*>& getFilters();

  /**
  * Print Information about the program
  * @param info What kind of information
  * @param posy The position on the viewport
  */
  void printInfo(ProgramInfo info, GLuint posy);

  /**
  * Adds a uniform variable to the program
  * @param uniform The Uniform type
  * @param swapTarget The uniform swap target
  * @param type The type of the Uniform
  * @param tex A pointer to a texture (if tex type) or NULL
  * @param ignore If the uniform is being ignored no glid is retrieved
  *               but the uniform acts as normal fbo target
  * @param texFilter filtering method for a texture (GL_LINEAR || GL_NEAREST)
  */
  void attachUniform(const Uniform uniform, const Uniform swapTarget,
                     const UniformType type, GLTexture* tex, const bool& ignore,
                     const GLenum texFilter);
  void handleUniforms(const PhysicalObject* object, int textureOffset = 0) const;

  /**
  * Get the uniform ids
  * @return The Uniform IDs
  */
  UniformInfos& getUniforms()
  { return uniforms; };

  /**
  * Get the number of textures defined as fbo/non-fbo attachment uniforms
  * @param restriction If UniformType_FBO the type fbo number of textures is returned
                       If UniformType_FBO_Swap the type fbo - type fbo swap number...
  * @return The number of unique uniform texture ids
  */
  int getNumTextures(const UniformType restriction = UniformType_TextureFile);

  /**
  * Get the texture filter assigned with the fbo attachments
  * @param a empty vector which will be filled with texture filter constants
  */
  void getTexFilter(std::vector<GLenum>& texFilter);

  /**
  * Bind all textures assigned through the uniforms
  * @param fbo The framebuffer object to use
  */
  void bindTextures(FramebufferObject* fbo);

  /**
  * Unbind previously bound textures
  * @param fbo The framebuffer object to use
  */
  void unbindTextures(FramebufferObject* fbo);

  /**
  * Bind a single texture by call
  * @param fbo The framebuffer object to use
  * @param i The texture occurance
  */
  void bindTexture(FramebufferObject* fbo, int i);

  /**
  * Unbind a single texture by call
  * @param fbo The framebuffer object to use
  * @param i The texture occurance
  */
  void unbindTexture(FramebufferObject* fbo, int i);

  /**
  * Retrieve the opengl id of a specific uniform variable
  * @param uni The uniform defition
  */
  int getUniformGLID(const Uniform uni) const;

  /**
  * Set the buffer which should be activated when rendering to fbos
  * @param bitmask A simple bitfield which specifies the buffer to enable
  * @param clearbitmask Another bitfield which specifies the buffer to clear before rendering
  */
  void setMRT(int bitmask, int clearbitmask);

  /**
  * Activate the draw buffer
  * @param the colorBufferExt current state from FBO
  */
  void activateDrawBuffers(GLenum* colorBufferExt);

  /**
  * Activate optional draw buffer and clear them
  * @param the colorBufferExt current state from FBO
  */
  void optionalClearDrawBuffers(GLenum* colorBufferExt);

  /**
  * Set an extension (use the fbo settings of the extension)
  * @param prog A pointer to the extension program
  */
  void setExtension(ShaderProgram* prog);

  /**
  * Get the previously set extension (or NULL)
  * @return A pointer to the extension program
  */
  ShaderProgram* getExtension();

  /**
  * Get the vector created from the multiple render target bitfield
  * @return the mrt vector
  */
  std::vector<int>& getMRTindex();
  std::vector<int>& getMRTclearindex();
};

#endif // SHADERPROGRAM_H_
