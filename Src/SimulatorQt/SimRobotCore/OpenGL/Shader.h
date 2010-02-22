/**
* @file Shader.h
* 
* Definition of opengl glsl shader class
*
* @author <A href="mailto:pachur@informatik.uni-bremen.de">Dennis Pachur</A>
*/

#ifndef SHADER_H_
#define SHADER_H_

#include <GL/glew.h>
#include <string>

enum ShaderInfo {
  SHADER_LOG = 0,
  SHADER_SOURCE
};

class Shader
{
protected:
  GLenum       type;
  GLchar*      source;
  GLchar*      log;
  std::string  name;
  std::string  file;
  GLuint       id;
  GLboolean    read;
  GLboolean    build;

public:
  /** Create a new shader object
  * @param name_ The name of the shader
  * @param type_ The Shader Type (GL_VERTEX_SHADER || GL_FRAGMENT_SHADER || GL_GEOMETRY_SHADER)
  * @param filname The filename of the shader file
  */
  Shader(std::string& name_, GLenum& type_, std::string& filename);

  /** standard destructor */
  ~Shader();

  /* get shader id
  * @return the shaders id given by opengl
  */
  GLuint getId();

  std::string getName();

  /** check if shader is usable
  * @return the usability-flag of the shader
  */
  GLboolean usable();

  /** create the shader
  * @return if everything is alright
  */
  bool create();

  /** destroy every memory allocation and reset variables */
  void destroy();

  /**
  * Print Information about the shader
  * @param info What kind of information
  * @param posy The position on the viewport
  */
  void printInfo(ShaderInfo info, GLuint posy);
};

#endif // SHADER_H_
