/**
* @file GLTexture.cpp
* 
* Definition of class GLTexture
*
* @author <A href="mailto:pachur@informatik.uni-bremen.de">Dennis Pachur</A>
*/

#include "GLtexture.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#ifdef _WIN32
#include <GL/glaux.h>
#endif
#include <OpenGL/GLHelper.h>
#include <OpenGL/Noise.h>

unsigned int GLTexture::runningTexID = 0;
std::map<std::string, GLuint> GLTexture::texRegister;

GLTexture::GLTexture()
: mode(GL_REPLACE), width(0), height(0), depth(0), priority(0.0)
{
  borderColor[0] = 0.0f;
  borderColor[1] = 0.0f;
  borderColor[2] = 0.0f;
  borderColor[3] = 0.0f;

  gl_texture_wrap_s_mode = GL_REPEAT;
  gl_texture_wrap_t_mode = GL_REPEAT;
  gl_texture_wrap_r_mode = GL_REPEAT;

  textureID = 0;
  texType = GL_TEXTURE_2D;
  cubemapside = GL_NONE;

  minFilter = GL_LINEAR;
  magFilter = GL_LINEAR;

  type = GL_RGBA;
}

GLTexture::~GLTexture()
{
  if(textureID)
  {
    glDeleteTextures(1, &textureID);
    textureID = 0;
  }
}

bool GLTexture::LoadTexture()
{
  if(strstr(texturename.c_str(), ".bmp"))
  {
    bool loaded = LoadBMP(texturename);
    if(!loaded)
      texturename = "";
    return loaded;
  }
  else if(strstr(texturename.c_str(), ".tga"))
  {
    bool loaded = LoadTGA(texturename);
    if(!loaded)
      texturename = "";
    return loaded;
  }
  else if(texturename == "Noise")
  {
    bool loaded = Build3DNoiseTexture(width);
    if(!loaded)
      texturename = "";
    return loaded;
  }
  else
  {
    GLHelper::getGLH()->glPrintText(TextObject::WARNING_HINTS, TextObject::WARNING, 5000,
      "The file format of \"%s\" is not supported.", texturename.c_str());
    texturename = "";
    return false;
  }
}

bool GLTexture::LoadTexture(std::string& name)
{
  texturename = name;
  return LoadTexture();
}

void GLTexture::UnBindTexture(bool unsetTexGen)
{
  if (textureID)
  {
    glDisable(texType);
    if(unsetTexGen)
    {
      glDisable(GL_TEXTURE_GEN_S);
      glDisable(GL_TEXTURE_GEN_T);
    }
  }
}

bool GLTexture::BindTexture()
{
  if (textureID == 0)
  {
    if (strcmp(texturename.c_str(),"") != 0)
      return LoadTexture();
    else
      return false;
  }
  else if(!glIsTexture(textureID))
    LoadTexture();

  // Set Texture Parameter
  if(cubemapside == GL_NONE)
  {
    glEnable(texType);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, mode);
    glBindTexture(texType, textureID);
  }
  return true;
}

void GLTexture::UnsaveBindTexture() const
{
  glBindTexture(texType, textureID);
}

void GLTexture::SetTexturePriority(GLclampf priority_)
{
  priority = priority_;
  glPrioritizeTextures(1, &textureID, &priority);
}

#ifdef _WIN32
bool GLTexture::LoadBMP(const std::string& name)
{
  // Create a place to store the texture
  AUX_RGBImageRec *TextureImage[1];

  // Set the pointer to NULL
  memset(TextureImage,0,sizeof(void *)*1);

  // Load the bitmap and assign our pointer to it
  TextureImage[0] = auxDIBImageLoadA(name.c_str());

  // check if image has been loaded
  if(!TextureImage[0])
  {
    GLHelper::getGLH()->glPrintText(TextObject::WARNING_HINTS, TextObject::WARNING, 5000,
      "The texture file \"%s\" has not been found.", texturename.c_str());
    texturename = "";
    return false;
  }

  // Just in case we want to use the width and height later
  width = TextureImage[0]->sizeX;
  height = TextureImage[0]->sizeY;
  texType = GL_TEXTURE_2D;
  type = GL_RGB;

  // Generate the OpenGL texture id but keep track of the ids
  if(textureID == 0)
  {
    if(texRegister.find(texturename) == texRegister.end())
    {
      textureID = ++runningTexID;
      texRegister[texturename] = textureID;
    }
    else
      textureID = texRegister[texturename];
  }

  // Bind this texture to its id
  glBindTexture(texType, textureID);

  // set texture parameter
  glTexParameterf(texType, GL_TEXTURE_WRAP_S, gl_texture_wrap_s_mode);
  glTexParameterf(texType, GL_TEXTURE_WRAP_T, gl_texture_wrap_t_mode);

  // Generate the mipmaps (put data to texture object)
  switch(cubemapside)
  {
  case GL_TEXTURE_CUBE_MAP:
    glTexParameterf(texType, GL_TEXTURE_MAG_FILTER, magFilter);
    glTexParameterf(texType, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, type, TextureImage[0]->sizeX,
      TextureImage[0]->sizeY, 0, type, GL_UNSIGNED_BYTE, TextureImage[0]->data);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, type, TextureImage[0]->sizeX,
      TextureImage[0]->sizeY, 0, type, GL_UNSIGNED_BYTE, TextureImage[0]->data);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, type, TextureImage[0]->sizeX,
      TextureImage[0]->sizeY, 0, type, GL_UNSIGNED_BYTE, TextureImage[0]->data);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, type, TextureImage[0]->sizeX,
      TextureImage[0]->sizeY, 0, type, GL_UNSIGNED_BYTE, TextureImage[0]->data);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, type, TextureImage[0]->sizeX,
      TextureImage[0]->sizeY, 0, type, GL_UNSIGNED_BYTE, TextureImage[0]->data);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, type, TextureImage[0]->sizeX,
      TextureImage[0]->sizeY, 0, type, GL_UNSIGNED_BYTE, TextureImage[0]->data);
    break;
  case GL_NONE:
    minFilter = GL_LINEAR_MIPMAP_NEAREST;
    glTexParameterf(texType, GL_TEXTURE_MAG_FILTER, magFilter);
    glTexParameterf(texType, GL_TEXTURE_MIN_FILTER, minFilter);
    gluBuild2DMipmaps(texType, 3, TextureImage[0]->sizeX,
      TextureImage[0]->sizeY, type, GL_UNSIGNED_BYTE, TextureImage[0]->data);
    minFilter = GL_LINEAR_MIPMAP_NEAREST;
    break;
  default:
    glTexParameterf(texType, GL_TEXTURE_MAG_FILTER, magFilter);
    glTexParameterf(texType, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexImage2D(cubemapside, 0, type, TextureImage[0]->sizeX,
      TextureImage[0]->sizeY, 0, type, GL_UNSIGNED_BYTE, TextureImage[0]->data);
    break;
  }

  // Cleanup
  if (TextureImage[0])
  {
    if (TextureImage[0]->data)
      free(TextureImage[0]->data);
    free(TextureImage[0]);
  }
  return true;
}
#else


bool GLTexture::LoadBMP(const std::string& name)
{
  FILE *fp;
  int width;
  int height;
  short planes;
  short bpp;

  // open file
  fp = fopen(name.c_str(), "rb");
  if(!fp)
  {
    GLHelper::getGLH()->glPrintText(TextObject::WARNING_HINTS, TextObject::WARNING, 5000,
      "The texture file \"%s\" has not been found.", texturename.c_str());
    return false;
  }

  // seek forward to width and height info
  fseek(fp, 18, SEEK_CUR);

  // read info
  if(fread(&width, 4, 1, fp) != 1 ||
    fread(&height, 4, 1, fp) != 1 ||
    fread(&planes, 2, 1, fp) != 1 ||
    fread(&bpp, 2, 1, fp) != 1)
  {
    fclose(fp);
    GLHelper::getGLH()->glPrintText(TextObject::WARNING_HINTS, TextObject::WARNING, 5000,
      "The texture file \"%s\" is damaged.", texturename.c_str());
    return false;
  }
  
  // Check to make sure the targa is valid
  if(width <= 0 || height <= 0 || width % 4)
  {
    fclose(fp);
    GLHelper::getGLH()->glPrintText(TextObject::WARNING_HINTS, TextObject::WARNING, 5000,
      "The texture file \"%s\" has a invalid width/height.", texturename.c_str());
    return false;
  }

  // check format
  if(planes != 1 || (bpp != 24 && bpp != 32))
  {
    fclose(fp);
    GLHelper::getGLH()->glPrintText(TextObject::WARNING_HINTS, TextObject::WARNING, 5000,
      "The texture file \"%s\" has an invalid bit depth. Only 24 bits or 32 bits are supported.", texturename.c_str());
    return false;
  }
    
  // seek forward to image data
  fseek(fp, 24, SEEK_CUR);

  // read data  
  int bytePerPixel = bpp / 8;
  int imageSize = width * height * bytePerPixel;
  GLubyte* imageData = new GLubyte[imageSize];
  if(!imageData)
  {
    fclose(fp);
    return false;
  }
  if(fread(imageData, imageSize, 1, fp) != 1)
  {
    delete [] imageData;
    fclose(fp);
    GLHelper::getGLH()->glPrintText(TextObject::WARNING_HINTS, TextObject::WARNING, 5000,
      "The texture file \"%s\" is damaged.", texturename.c_str());
    return false;
  }

  // rearrange BGR to RGB
	GLubyte temp;
  for(GLubyte* data = imageData, * dataEnd = data + imageSize; data < dataEnd; data += bytePerPixel)
	{ 
		temp = data[0];
		data[0] = data[2];
		data[2] = temp;
  }
  
  bool loaded = LoadImageData(imageData, width, height, bpp == 24 ? GL_RGB : GL_RGBA, GL_TEXTURE_2D);
  delete [] imageData;
  return loaded;
}
#endif

bool GLTexture::LoadTGA(const std::string& name)
{
  GLubyte TGAheader[12] = {0,0,2,0,0,0,0,0,0,0,0,0};   // Uncompressed TGA header
  GLubyte TGAcompare[12];                              // Used to compare TGA header
  GLubyte header[6];                                   // First 6 useful bytes of the header
  GLuint  bytesPerPixel;
  GLuint  imageSize;
  GLuint  temp;
  GLubyte *imageData;
  GLuint  bpp;

  FILE *file = fopen(name.c_str(), "rb");               // Open the TGA file

  // Load the file and perform checks
  if(file == NULL ||                                                      // Does file exist?
    fread(TGAcompare,1,sizeof(TGAcompare),file) != sizeof(TGAcompare) ||  // Are there 12 bytes to read?
    memcmp(TGAheader,TGAcompare,sizeof(TGAheader)) != 0 ||                // Is it the right format?
    fread(header,1,sizeof(header),file) != sizeof(header))                // If so then read the next 6 header bytes
  {
    if(file == NULL) // If the file didn't exist then return
    {
      GLHelper::getGLH()->glPrintText(TextObject::WARNING_HINTS, TextObject::WARNING, 5000,
        "The texture file \"%s\" has not been found.", texturename.c_str());
      return false;
    }
    else
    {
      fclose(file); // If something broke then close the file and return
      GLHelper::getGLH()->glPrintText(TextObject::WARNING_HINTS, TextObject::WARNING, 5000,
        "The texture file \"%s\" is damaged.", texturename.c_str());
      return false;
    }
  }

  // Determine the TGA width and height (highbyte*256+lowbyte)
  width  = header[1] * 256 + header[0];
  height = header[3] * 256 + header[2];
  texType = GL_TEXTURE_2D;

  // Check to make sure the targa is valid
  if(width <= 0 || height <= 0)
  {
    fclose(file);
    GLHelper::getGLH()->glPrintText(TextObject::WARNING_HINTS, TextObject::WARNING, 5000,
      "The texture file \"%s\" has a invalid width/height.", texturename.c_str());
    return false;
  }
  // Only 24 or 32 bit images are supported
  if( (header[4] != 24 && header[4] != 32) )
  {
    fclose(file);
    GLHelper::getGLH()->glPrintText(TextObject::WARNING_HINTS, TextObject::WARNING, 5000,
      "The texture file \"%s\" has an invalid bit depth. Only 24 bits or 32 bits are supported.", texturename.c_str());
    return false;
  }

  bpp = header[4];  // Grab the bits per pixel
  bytesPerPixel = bpp/8;  // Divide by 8 to get the bytes per pixel
  imageSize = width * height * bytesPerPixel; // Calculate the memory required for the data

  // Allocate the memory for the image data
  imageData = new GLubyte[imageSize];
  if(!imageData)
  {
    fclose(file);
    return false;
  }

  // Load the image data
  if(fread(imageData, 1, imageSize, file) != imageSize)  // Does the image size match the memory reserved?
  {
    delete [] imageData;  // If so, then release the image data
    fclose(file); // Close the file
    GLHelper::getGLH()->glPrintText(TextObject::WARNING_HINTS, TextObject::WARNING, 5000,
      "The texture file \"%s\" is damaged.", texturename.c_str());
    return false;
  }

  // Loop through the image data and swap the 1st and 3rd bytes (red and blue)
  for(GLuint i = 0; i < imageSize; i += bytesPerPixel)
  {
    temp = imageData[i];
    imageData[i] = imageData[i + 2];
    imageData[i + 2] = temp;
  }

  // We are done with the file so close it
  fclose(file);

  // Set the type
  if (bpp == 24)
    type = GL_RGB;

  //
  bool loaded = LoadImageData(imageData, width, height, type, texType);
  
  // Cleanup
  delete [] imageData;

  return loaded;
}

bool GLTexture::LoadImageData(GLubyte* imageData, GLuint w, GLuint h, GLuint t, GLenum tT)
{
  width = w;
  height = h;
  type = t;
  texType = tT;
  
  // Generate the OpenGL texture id but keep track of the ids
  if(textureID == 0)
  {
    if(texRegister.find(texturename) == texRegister.end())
    {
      textureID = ++runningTexID;
      texRegister[texturename] = textureID;
    }
    else
      textureID = texRegister[texturename];
  }

  // Bind this texture to its id
  glBindTexture(texType, textureID);

  // set texture parameter
  glTexParameterf(texType, GL_TEXTURE_WRAP_S, gl_texture_wrap_s_mode);
  glTexParameterf(texType, GL_TEXTURE_WRAP_T, gl_texture_wrap_t_mode);

  // Generate the mipmaps
  switch(cubemapside)
  {
  case GL_TEXTURE_CUBE_MAP:
    glTexParameterf(texType, GL_TEXTURE_MAG_FILTER, magFilter);
    glTexParameterf(texType, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, type, width,
      height, 0, type, GL_UNSIGNED_BYTE, imageData);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, type, width,
      height, 0, type, GL_UNSIGNED_BYTE, imageData);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, type, width,
      height, 0, type, GL_UNSIGNED_BYTE, imageData);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, type, width,
      height, 0, type, GL_UNSIGNED_BYTE, imageData);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, type, width,
      height, 0, type, GL_UNSIGNED_BYTE, imageData);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, type, width,
      height, 0, type, GL_UNSIGNED_BYTE, imageData);
    break;
  case GL_NONE:
    minFilter = GL_LINEAR_MIPMAP_NEAREST;
    glTexParameterf(texType, GL_TEXTURE_MAG_FILTER, magFilter);
    glTexParameterf(texType, GL_TEXTURE_MIN_FILTER, minFilter);
    gluBuild2DMipmaps(texType, type, width, height,
      type, GL_UNSIGNED_BYTE, imageData);
    break;
  default:
    glTexParameterf(texType, GL_TEXTURE_MAG_FILTER, magFilter);
    glTexParameterf(texType, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexImage2D(cubemapside, 0, type, width,
      height, 0, type, GL_UNSIGNED_BYTE, imageData);
    break;
  }

  // Set base priority
  glPrioritizeTextures(1, &textureID, &priority);
  
  return true;
}

void GLTexture::BuildColorTexture(Vector3d& color)
{
  unsigned char data[12]; // a 2x2 texture at 24 bits

  // Store the data
  for(int i = 0; i < 12; i += 3)
  {
    data[i] = static_cast<unsigned char>(color.v[0]);
    data[i+1] = static_cast<unsigned char>(color.v[1]);
    data[i+2] = static_cast<unsigned char>(color.v[2]);
  }

  // set infos
  width = 2;
  height = 2;
  texType = GL_TEXTURE_2D;
  minFilter = GL_LINEAR_MIPMAP_NEAREST;
  type = GL_RGB;

  // Generate the OpenGL texture id
  textureID = ++runningTexID;

  // Bind this texture to its id
  glBindTexture(texType, textureID);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  // set texture parameter
  glTexParameterf(texType, GL_TEXTURE_WRAP_S, gl_texture_wrap_s_mode);
  glTexParameterf(texType, GL_TEXTURE_WRAP_T, gl_texture_wrap_t_mode);
  glTexParameterf(texType, GL_TEXTURE_MAG_FILTER, magFilter);
  glTexParameterf(texType, GL_TEXTURE_MIN_FILTER, minFilter);

  // Generate the texture
  gluBuild2DMipmaps(texType, 3, width, height, type, GL_UNSIGNED_BYTE, data);
}

bool GLTexture::Build3DNoiseTexture(const int size)
{
  // preset infos
  width = size;
  height = size;
  depth = size;
  texType = GL_TEXTURE_3D;
  type = GL_RGBA;

  Noise noise;
  noise.make3DPachNoiseTexture(size);

  textureID = ++runningTexID;
  glBindTexture(texType, textureID);
  glTexParameterf(texType, GL_TEXTURE_WRAP_S, gl_texture_wrap_s_mode);
  glTexParameterf(texType, GL_TEXTURE_WRAP_T, gl_texture_wrap_t_mode);
  glTexParameterf(texType, GL_TEXTURE_WRAP_R, gl_texture_wrap_r_mode);
  glTexParameterf(texType, GL_TEXTURE_MAG_FILTER, magFilter);
  glTexParameterf(texType, GL_TEXTURE_MIN_FILTER, minFilter);
  GLubyte* ptr = (GLubyte*)(noise.getTexPtr());
  glTexImage3D(texType, 0, type, width, height, depth, 0,
    type, GL_UNSIGNED_BYTE, ptr);

  // c++ warning work-around
  if(glIsTexture(textureID))
    return true;
  return false;
}

bool GLTexture::HasAlpha() const
{
  return type == GL_RGBA;
}

void GLTexture::ClearTexRegister()
{
  texRegister.clear();
  runningTexID = 0;
}
