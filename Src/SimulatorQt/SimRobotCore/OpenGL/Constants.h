/**
 * @file Constants.h
 * 
 * Definition of some constant values
 *
 * @author <A href="mailto:pachur@informatik.uni-bremen.de">Dennis Pachur</A>
 */ 

#ifndef __CONSTANTS_H__
#define __CONSTANTS_H__

#include <GL/glew.h>

// constant colors
const GLfloat black[4] = {0.0f, 0.0f, 0.0f, 1.0f};
const GLfloat white[4] = {1.0f, 1.0f, 1.0f, 1.0f};
const GLfloat grey[4] = {0.5f, 0.5f, 0.5f, 1.0f};
const GLfloat darkgrey[4] = {0.2f, 0.2f, 0.2f, 1.0f};
const GLfloat lightgrey[4] = {0.8f, 0.8f, 0.8f, 1.0f};
const GLfloat red[4] = {1.0f, 0.0f, 0.0f, 1.0f};
const GLfloat lightred[4] = {1.0f, 0.3f, 0.3f, 1.0f};
const GLfloat green[4] = {0.0f, 1.0f, 0.0f, 1.0f};
const GLfloat lightgreen[4] = {0.5f, 1.0f, 0.5f, 1.0f};
const GLfloat blue[4] = {0.0f, 0.0f, 1.0f, 1.0f};
const GLfloat lightblue[4] = {0.5f, 0.5f, 1.0f, 1.0f};
const GLfloat nocolor[4] = {0.0f, 0.0f, 0.0f, 0.0f};
const GLfloat shadows[4] = {0.0f, 0.0f, 0.0f, 0.1f};
const GLfloat blackNoAlpha[4] = {0.0f, 0.0f, 0.0f, 0.0f};

const GLbitfield default_clear = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;
const GLbitfield extended_clear = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |GL_ACCUM_BUFFER_BIT;

// Environment
const GLfloat ecSize = 15.0f;
const GLfloat environmentCubeVertices[96] = {  ecSize, ecSize, -ecSize, ecSize, -ecSize, -ecSize,   // LEFT
                                               ecSize, -ecSize, ecSize, ecSize, ecSize, ecSize,
                                              -ecSize, ecSize, ecSize, -ecSize, -ecSize, ecSize,    // RIGHT
                                              -ecSize, -ecSize, -ecSize, -ecSize, ecSize, -ecSize,
                                               ecSize, ecSize, ecSize, -ecSize, ecSize, ecSize,     // FRONT
                                              -ecSize, ecSize, -ecSize, ecSize, ecSize, -ecSize,
                                               ecSize, -ecSize, -ecSize, -ecSize, -ecSize, -ecSize, // BACK
                                              -ecSize, -ecSize, ecSize, ecSize, -ecSize, ecSize,
                                               ecSize, -ecSize, ecSize, -ecSize, -ecSize, ecSize,   // TOP
                                              -ecSize, ecSize, ecSize, ecSize, ecSize, ecSize,
                                               ecSize, ecSize, -ecSize, -ecSize, ecSize, -ecSize,   // BOTTOM
                                              -ecSize, -ecSize, -ecSize, ecSize, -ecSize, -ecSize };
const GLfloat environmentCubeTexCoords[48] = { 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,  // LEFT
                                               1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, // RIGHT
                                               1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,  // FRONT
                                               1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,  // BACK
                                               1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,  // TOP
                                               0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f}; // BOTTOM

// shadow mapping
const GLfloat scaleMatrix2D[16] = {0.5f, 0.0f, 0.0f, 0.0f,
                                   0.0f, 0.5f, 0.0f, 0.0f,
                                   0.0f, 0.0f, 0.5f, 0.0f,
                                   0.5f, 0.5f, 0.5f, 1.0f};
const GLfloat scaleMatrix1D[16] = {0.0f, 0.0f, 0.0f, 0.0f,
                                   0.0f, 0.0f, 0.0f, 0.0f,
                                   0.5f, 0.0f, 0.0f, 0.0f,
                                   0.5f, 0.0f, 0.0f, 1.0f};

// antialiasing
const GLfloat jitterValues[58] = {0.0f, 0.0f,                                    // NO_ANTIALIASING
                                  0.25f, 0.75f, 0.75f, 0.25f,                                             // FSAA2
                                  0.503392f, 0.831796f, 0.7806016f, 0.250438f, 0.226182f, 0.4131553f,     // FSAA3
                                  0.375f, 0.25f, 0.125f, 0.75f, 0.875f, 0.25f, 0.625f, 0.75f,             // FSAA4
                                  0.5f, 0.5f, 0.3f, 0.1f, 0.7f, 0.9f, 0.9f, 0.3f, 0.1f, 0.7f,             // FSAA5
                                  0.464646f, 0.464646f, 0.131313f, 0.797979f, 0.535353f, 0.868686f,
                                  0.868686f, 0.535353f, 0.797979f, 0.131313f, 0.202020f, 0.202020f,        // FSAA6
                                  0.5625f, 0.4375f, 0.0625f, 0.9375f, 0.3125f, 0.6875f, 0.6875f, 0.8125f,
                                  0.8125f, 0.1875f, 0.9375f, 0.5625f, 0.4375f, 0.0625f, 0.1875f, 0.3125f};// FSAA8

// uniforms
const int NumUniqueUniforms = 19;
enum Uniform {
  Uniform_Texture0 = 0,
  Uniform_Texture1,
  Uniform_Texture2,
  Uniform_Texture3,
  Uniform_Texture4,
  Uniform_Texture5,
  Uniform_Texture6,
  Uniform_Texture7,
  Uniform_ModelMatrix,
  Uniform_ObjectMass,
  Uniform_ObjectSpeed,
  Uniform_Previous_ModelViewProjectionMatrix,
  Uniform_Apply_Texture,
  Uniform_Num_Lights,
  Uniform_Random,
  Uniform_ImageDimensions,
  Uniform_ExposureTime,
  Uniform_ShadowMap,
  Uniform_Undefined
};

const std::string UniformName[NumUniqueUniforms] = {
  "Texture0",
  "Texture1",
  "Texture2",
  "Texture3",
  "Texture4",
  "Texture5",
  "Texture6",
  "Texture7",
  "ModelMatrix",
  "ObjectMass",
  "ObjectSpeed",
  "previous_ModelViewProjectionMatrix",
  "ApplyTexture",
  "NumLights",
  "Random",
  "ImageDimensions",
  "ExposureTime",
  "ShadowMap",
  "Undefined"
};

enum UniformType {
  UniformType_Value = 0,
  UniformType_TextureFile,
  UniformType_Noise,
  UniformType_FBO,
  UniformType_FBO_PhysicUpdate_Swap,
  UniformType_FBO_RenderPass_Swap,
  UniformType_Storage
};


#endif // __CONSTANTS_H__
