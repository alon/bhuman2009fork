/**
* @file GLTexture.h
* 
* Definition of class GLTexture
*
* @author <A href="mailto:pachur@informatik.uni-bremen.de">Dennis Pachur</A>
*/

#ifndef GLTEXTURE_H
#define GLTEXTURE_H

#include <map>
#include <string>
#include <GL/glew.h>

#include "../Tools/SimMath.h"

class GLTexture  
{
  friend class CubeMap;

protected:

  // ensure texture id clearence on mulitple render context system without wgl share lists
  static unsigned int runningTexID;
  static std::map<std::string, GLuint> texRegister;

  /** The name of the texture */
  std::string texturename;

  /** The texture mode of opengl texture application */
  GLfloat mode;

  /** The texture image dimensions */
  GLuint width, height, depth;

  /** The priority of the texture */
  GLclampf priority;

  /** The border color */
  GLfloat borderColor[4];

  /** Texture wrap modes */
  GLfloat gl_texture_wrap_s_mode;
  GLfloat gl_texture_wrap_t_mode;
  GLfloat gl_texture_wrap_r_mode;

  /** Texture filter modes */
  GLfloat minFilter;
  GLfloat magFilter;

  /** 1D, 2D or 3D Texture */
  GLenum texType;

  /** texture format RGB, RGBA, etc. */
  GLuint type;

  /** Flag if texture has to be handled like a cubemap texture
  * GL_NONE hints to a standard texture */
  GLenum cubemapside;

  // helper functions for the different texture formats
  bool LoadTGA(const std::string& name);
  bool LoadBMP(const std::string& name);
  bool LoadImageData(GLubyte* imageData, GLuint width, GLuint height, GLuint type, GLenum texType);

public:

  /** The texture number given by OpenGL */
  unsigned int textureID;

  GLTexture();
  ~GLTexture();

  GLfloat* getBorderColorPtr()
  { return borderColor; }

  GLuint getWidth() const
  { return width; }

  void setWidth(const GLuint width_)
  { width = width_; }

  std::string& getTexName()
  { return texturename; }

  void setTexName(const std::string newName)
  { texturename = newName; }

  void setMode(const GLfloat newMode)
  { mode = newMode; }

  void setWrapS(const GLenum mode)
  { gl_texture_wrap_s_mode = (GLfloat) mode; }

  void setWrapT(const GLenum mode)
  { gl_texture_wrap_t_mode = (GLfloat) mode; }

  void setWrapR(const GLenum mode)
  { gl_texture_wrap_r_mode = (GLfloat) mode; }

  void setPriority(const GLclampf prio)
  { priority = prio; }

  /** Loads the texture defined in texturename into OpenGL's TextureBuffer
  * (a OpenGL context must exist and the routine has to be
  * called from this context!)
  */
  bool LoadTexture();

  /** Loads a texture from file into OpenGL's TextureBuffer
  * (a OpenGL context must exist and the routine has to be
  * called from this context!)
  * @param filename
  */
  bool LoadTexture(std::string& name);
  
  /** Bind the texture */
  bool BindTexture();
  void UnsaveBindTexture() const;

  /** Unbind the texture
  * @param unsetTexGen If texture generation has to be disabled
  */
  void UnBindTexture(bool unsetTexGen);

  /** Set texture priority
  * Making textures resident may improve spee
  * @param priority Priority has to be between 0 and 1
  */
  void SetTexturePriority(GLclampf priority);
  /** Build simple one-color texture
  * @param color A 3d-vector which specifies the RGB values of the color
  */
  void BuildColorTexture(Vector3d& color);

  /** Build noise texture
  * @param size The texture resolution (mem usage = size^3*4)
  * @return true if the texture has been created
  */
  bool Build3DNoiseTexture(const int size);

  /** Ask for an existing alpha channel
  * @return true if the texture has rgba format
  */
  bool HasAlpha() const;

  /** Clear the texture register
  * WARNING: only one static register
  */
  static void ClearTexRegister();

};

#endif // GLTEXTURE_H
