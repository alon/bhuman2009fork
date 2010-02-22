/**
 * @file CubeMap.h
 * 
 * Definition of class CubeMap
 *
 * @author <A href="mailto:pachur@informatik.uni-bremen.de">Dennis Pachur</A>
 */ 

#ifndef CUBEMAP_H_
#define CUBEMAP_H_

#include <vector>

#include "../OpenGL/GLtexture.h"

/**
* @class CubeMap
* A class describing an environment object for cube-mapping
*/
class CubeMap
{
protected:
  /** Cube map textures for displaying the surrounding directly */
  GLTexture posx_directTexture;
  GLTexture negx_directTexture;
  GLTexture posy_directTexture;
  GLTexture negy_directTexture;
  GLTexture posz_directTexture;
  GLTexture negz_directTexture;

  /** flag if a single texture is used for all 6 cube sides */
  bool useSingleTexture;

  /** flag if textures are used as direct textures */
  bool useAsDirectTextures;

public:
  /** default constructor */
  CubeMap();

  /** Cube map textures for environment mapping */
  GLTexture posxTexture;
  GLTexture negxTexture;
  GLTexture posyTexture;
  GLTexture negyTexture;
  GLTexture poszTexture;
  GLTexture negzTexture;

  /** Set a cube map texture
  * @param cubeSide The OpenGL enum for the destination texture
  * @param filename The filename to the texture file
  */
  void setCubeMapTexture(GLenum cubeSide, std::string filename);

  /** set singleCubeTexture */
  void setSingleTexture()
  { useSingleTexture = true; };

  /** initialize the cube map textures */
  void initCubeMap();

  /** Get the texture IDs
  * @return The texture IDs
  */
  std::vector<GLuint> getTextureIDs();

  /** set usage of direct texture
  * @param value The usage flag
  */
  void setUseAsDirectTextures(bool value)
  { useAsDirectTextures = value; }

  /** get usage of direct texture
  * @return the usage state
  */
  bool getUseAsDirectTextures()
  { return useAsDirectTextures; }

  /** use this function to override texture sizes when using dynamic env mapping
  * @param newSize The new Texture size
  */
  void changeTexSizes(GLuint newSize);
};

#endif //CUBEMAP_H_
