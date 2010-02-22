/**
 * @file CubeMap.cpp
 * 
 * Definition of class CubeMap
 *
 * @author <A href="mailto:pachur@informatik.uni-bremen.de">Dennis Pachur</A>
 */
#include "CubeMap.h"

CubeMap::CubeMap()
  : useSingleTexture(false), useAsDirectTextures(false)
{
  posxTexture.cubemapside = GL_TEXTURE_CUBE_MAP_POSITIVE_X;
  posyTexture.cubemapside = GL_TEXTURE_CUBE_MAP_POSITIVE_Y;
  poszTexture.cubemapside = GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
  negxTexture.cubemapside = GL_TEXTURE_CUBE_MAP_NEGATIVE_X;
  negyTexture.cubemapside = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;
  negzTexture.cubemapside = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
}

void CubeMap::changeTexSizes(GLuint newSize)
{
  // Allocate the memory for the image data
  GLubyte *imageData = new GLubyte[newSize * newSize * 3];

  glBindTexture(GL_TEXTURE_2D, posxTexture.textureID);
  glDeleteTextures(1, &posxTexture.textureID);
  glBindTexture(GL_TEXTURE_2D, posxTexture.textureID);
  glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, newSize,
    newSize, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);
  posxTexture.width = posxTexture.width = newSize;

  glBindTexture(GL_TEXTURE_2D, negxTexture.textureID);
  glDeleteTextures(1, &negxTexture.textureID);
  glBindTexture(GL_TEXTURE_2D, negxTexture.textureID);
  glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, newSize,
    newSize, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);
  negxTexture.width = negxTexture.width = newSize;

  glBindTexture(GL_TEXTURE_2D, posyTexture.textureID);
  glDeleteTextures(1, &posyTexture.textureID);
  glBindTexture(GL_TEXTURE_2D, posyTexture.textureID);
  glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, newSize,
    newSize, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);
  posyTexture.width = posyTexture.width = newSize;

  glBindTexture(GL_TEXTURE_2D, negyTexture.textureID);
  glDeleteTextures(1, &negyTexture.textureID);
  glBindTexture(GL_TEXTURE_2D, negyTexture.textureID);
  glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, newSize,
    newSize, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);
  negyTexture.width = negyTexture.width = newSize;

  glBindTexture(GL_TEXTURE_2D, poszTexture.textureID);
  glDeleteTextures(1, &poszTexture.textureID);
  glBindTexture(GL_TEXTURE_2D, poszTexture.textureID);
  glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, newSize,
    newSize, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);
  poszTexture.width = poszTexture.width = newSize;

  glBindTexture(GL_TEXTURE_2D, negzTexture.textureID);
  glDeleteTextures(1, &negzTexture.textureID);
  glBindTexture(GL_TEXTURE_2D, negzTexture.textureID);
  glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, newSize,
    newSize, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);
  negzTexture.width = negzTexture.width = newSize;

  delete(imageData);
}

void CubeMap::setCubeMapTexture(GLenum cubeSide, std::string filename)
{
  switch(cubeSide)
  {
  case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
    posxTexture.texturename = filename;
    break;
  case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
    negxTexture.texturename = filename;
    break;
  case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
    posyTexture.texturename = filename;
    break;
  case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
    negyTexture.texturename = filename;
    break;
  case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
    poszTexture.texturename = filename;
    break;
  case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
    negzTexture.texturename = filename;
    break;
  case GL_TEXTURE_CUBE_MAP:
    posxTexture.texturename = filename;
  }
}

void CubeMap::initCubeMap()
{
  if(useSingleTexture)
  {
    if(useAsDirectTextures)
    {
      posx_directTexture.texturename = posxTexture.texturename;
      posx_directTexture.BindTexture();
    }
    if(GLEW_ARB_texture_cube_map)
    {
      posxTexture.cubemapside = GL_TEXTURE_CUBE_MAP;
      posxTexture.BindTexture();
    }
    else
    {
      posxTexture.texturename = "";
    }
    
  }
  else
  {
    if(useAsDirectTextures)
    {
      posx_directTexture.texturename = posxTexture.texturename;
      posx_directTexture.BindTexture();
      posy_directTexture.texturename = posyTexture.texturename;
      posy_directTexture.BindTexture();
      posz_directTexture.texturename = poszTexture.texturename;
      posz_directTexture.BindTexture();
      negx_directTexture.texturename = negxTexture.texturename;
      negx_directTexture.BindTexture();
      negy_directTexture.texturename = negyTexture.texturename;
      negy_directTexture.BindTexture();
      negz_directTexture.texturename = negzTexture.texturename;
      negz_directTexture.BindTexture();
    }
    if(GLEW_ARB_texture_cube_map)
    {
      posxTexture.BindTexture();
      posyTexture.BindTexture();
      poszTexture.BindTexture();
      negxTexture.BindTexture();
      negyTexture.BindTexture();
      negzTexture.BindTexture();
    }
    else
    {
      posxTexture.texturename = "";
      posyTexture.texturename = "";
      poszTexture.texturename = "";
      negxTexture.texturename = "";
      negyTexture.texturename = "";
      negzTexture.texturename = "";
    }
  }
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

std::vector<GLuint> CubeMap::getTextureIDs()
{
  std::vector<GLuint> texIDhandles;
  texIDhandles.push_back(posx_directTexture.textureID);
  if(!useSingleTexture)
  {
    texIDhandles.push_back(negx_directTexture.textureID);
    texIDhandles.push_back(posy_directTexture.textureID);
    texIDhandles.push_back(negy_directTexture.textureID);
    texIDhandles.push_back(posz_directTexture.textureID);
    texIDhandles.push_back(negz_directTexture.textureID);
  }
  return texIDhandles;
}

