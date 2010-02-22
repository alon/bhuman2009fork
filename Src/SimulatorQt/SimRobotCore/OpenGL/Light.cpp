/**
* @file Light.cpp
* 
* Definition of class Ligt
*
* @author <A href="mailto:pachur@informatik.uni-bremen.de">Dennis Pachur</A>
*/

#include "Light.h"
#include <Simulation/SimObject.h>
#include <OpenGL/GLHelper.h>
#include <OpenGL/Constants.h>

void Light::setAmbientColor(GLfloat& red, GLfloat& green, GLfloat& blue, GLfloat& alpha)
{
  ambientColor[0] = red;
  ambientColor[1] = green;
  ambientColor[2] = blue;
  ambientColor[3] = alpha;
}

void Light::setDiffuseColor(GLfloat& red, GLfloat& green, GLfloat& blue, GLfloat& alpha)
{
  diffuseColor[0] = red;
  diffuseColor[1] = green;
  diffuseColor[2] = blue;
  diffuseColor[3] = alpha;
}

void Light::setSpecularColor(GLfloat& red, GLfloat& green, GLfloat& blue, GLfloat& alpha)
{
  specularColor[0] = red;
  specularColor[1] = green;
  specularColor[2] = blue;
  specularColor[3] = alpha;
}

void Light::setPosition(GLfloat& x, GLfloat& y, GLfloat& z, GLfloat& w)
{
  position[0] = x;
  position[1] = y;
  position[2] = z;
  position[3] = w;
}

void Light::setSpotDirection(GLfloat& x, GLfloat& y, GLfloat& z)
{
  spotDirection[0] = x;
  spotDirection[1] = y;
  spotDirection[2] = z;
}

Light::Light(const AttributeSet& attributes)
{
  // parsing name
  name = ParserUtilities::getValueFor(attributes, "name");

  // set important opengl ids to GL_NONE (0x0000)
  lightID = GL_NONE;
  shadowMapID = GL_NONE;
  shadowMapSize = 0;
  shadowMapFarClippingPlane = 20;
  shadowMapNearClippingPlane = 10;
  shadowMapIntensity[3] = GLfloat(0.1);
  hardwareShadowing = true;
  shadowMapData8Bit = 0;

  // set standard light parameters (defined by OpenGL ARB)
  // see OpenGL Architecture Review Board OpenGl Programming Guide
  // Fifth Edition, Version 2 for more information
  ambientColor[0] = 0.0;
  ambientColor[1] = 0.0;
  ambientColor[2] = 0.0;
  ambientColor[3] = 1.0;

  diffuseColor[0] = 1.0;
  diffuseColor[1] = 1.0;
  diffuseColor[2] = 1.0;
  diffuseColor[3] = 1.0;

  specularColor[0] = 1.0;
  specularColor[1] = 1.0;
  specularColor[2] = 1.0;
  specularColor[3] = 1.0;

  position[0] =  0.0;
  position[1] =  0.0;
  position[2] =  1.0;
  position[3] =  0.0;

  constantAttenuation = 1.0;
  linearAttenuation = 0.0;
  quadraticAttenuation = 0.0;

  spotCutoff = 180.0;
  spotExponent = 0.0;

  spotDirection[0] = 0.0;
  spotDirection[1] = 0.0;
  spotDirection[2] = -1.0;

  boundObject = 0;
}

Light::~Light()
{
  if(shadowMapID != 0)
    glDeleteTextures(1, &shadowMapID);
  if(shadowMapData8Bit)
    delete [] shadowMapData8Bit;
  shadowMapData8Bit=NULL;
}

void Light::parseAttributeElements(const std::string& name, 
                                   const AttributeSet& attributes,
                                   int line, int column, 
                                   ErrorManager* errorManager)
{
  if(name == "AmbientColor")
  {
    ParserUtilities::toRGBA(attributes, ambientColor);
  }
  else if(name == "DiffuseColor")
  {
    ParserUtilities::toRGBA(attributes, diffuseColor);
  }
  else if(name == "SpecularColor")
  {
    ParserUtilities::toRGBA(attributes, specularColor);
  }
  else if(name == "Position")
  {
    ParserUtilities::toUnnormalizedPoint(attributes, position);
  }
  else if(name == "ConstantAttenuation")
  {
    constantAttenuation = (GLfloat)ParserUtilities::toDouble(attributes[0].value);
  }
  else if(name == "LinearAttenuation")
  {
    linearAttenuation = (GLfloat)ParserUtilities::toDouble(attributes[0].value);
  }
  else if(name == "QuadraticAttenuation")
  {
    quadraticAttenuation = (GLfloat)ParserUtilities::toDouble(attributes[0].value);
  }
  else if(name == "SpotCutoff")
  {
    spotCutoff = (GLfloat)ParserUtilities::toDouble(attributes[0].value);
  }
  else if(name == "SpotExponent")
  {
    spotExponent = (GLfloat)ParserUtilities::toDouble(attributes[0].value);
  }
  else if(name == "SpotDirection")
  {
    ParserUtilities::toNormalizedPoint(attributes, spotDirection);
  }
  else if(name == "ShadowMap")
  {
    for(unsigned int i=0; i< attributes.size(); i++)
    {
      if(attributes[i].attribute == "size")
        shadowMapSize = ParserUtilities::toInt(attributes[i].value);
      else if(attributes[i].attribute == "nearClip")
        shadowMapNearClippingPlane = ParserUtilities::toInt(attributes[i].value);
      else if(attributes[i].attribute == "farClip")
        shadowMapFarClippingPlane = ParserUtilities::toInt(attributes[i].value);
      else if(attributes[i].attribute == "intensity")
        shadowMapIntensity[3] = (GLfloat)ParserUtilities::toDouble(attributes[i].value);
    }
  }
  else
  {
    errorManager->addError("Unknown Light Parameter", "The provided LightDefinition parameter \"" + name + "\" is unknown.", line, column);
  }
}

void Light::setObjectBinding(const SimObject* simObject)
{
  boundObject = simObject;
}

bool Light::updateBoundObjectTransformation()
{
  if(boundObject)
  {
    Matrix3d rot = boundObject->getRotation();
    Vector3d bO = boundObject->getPosition();

    boundPosition[0] = position[0] + (GLfloat)bO.v[0];
    boundPosition[1] = position[1] + (GLfloat)bO.v[1];
    boundPosition[2] = position[2] + (GLfloat)bO.v[2];
    boundPosition[3] = position[3];

    bO.v[0] = spotDirection[0];
    bO.v[1] = spotDirection[1];
    bO.v[2] = spotDirection[2];
    bO.rotate(rot);

    boundSpotDirection[0] = (GLfloat)bO.v[0];
    boundSpotDirection[1] = (GLfloat)bO.v[1];
    boundSpotDirection[2] = (GLfloat)bO.v[2];

    return true;
  }
  else
    return false;
}

void Light::updateLightPosition()
{
  if(boundObject)
  {
    glPushMatrix();
    GLHelper::getGLH()->setMatrix(boundObject->getPosition(), boundObject->getRotation());
    glLightfv(lightID, GL_POSITION, position);
    glLightfv(lightID, GL_SPOT_DIRECTION, spotDirection);
    glPopMatrix();
  }
  else
  {
    glLightfv(lightID, GL_POSITION, position);
    glLightfv(lightID, GL_SPOT_DIRECTION, spotDirection);
  }  
}

void Light::getSpotDirectionVector(Vector3d& vec)
{
  vec.v[0] = spotDirection[0];
  vec.v[1] = spotDirection[1];
  vec.v[2] = spotDirection[2];
  if(boundObject)
    vec.rotate(boundObject->getRotation());
}

void Light::getPositionVector(Vector3d& vec)
{
  vec.v[0] = position[0]/position[3];
  vec.v[1] = position[1]/position[3];
  vec.v[2] = position[2]/position[3];
  if(boundObject)
  {
    vec += boundObject->getPosition();
    vec.rotate(boundObject->getRotation());
  }
}

void Light::getTargetVector(Vector3d& vec)
{
  vec.v[0] = position[0]/position[3] + spotDirection[0];
  vec.v[1] = position[1]/position[3] + spotDirection[1];
  vec.v[2] = position[2]/position[3] + spotDirection[2];
}

void Light::drawLightCones(GLUquadric* qobj)
{
  // save the state
  glPushAttrib(GL_ALL_ATTRIB_BITS);

  // draw the helper
  glPushMatrix();
  if(boundObject)
  {
    glPushMatrix();
    GLHelper::getGLH()->setMatrix(boundObject->getPosition(), boundObject->getRotation());
  }
  glTranslated(position[0], position[1], position[2]);
  glColor3fv(diffuseColor);
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  gluSphere(qobj, 0.1, 10, 10);
  if(spotCutoff < 90.0)
  {
    glColor3fv(ambientColor);
    glBegin(GL_TRIANGLES);
    glVertex3d(0.01, 0.0, 0.0);
    glVertex3d(-0.01, 0.0, 0.0);
    glVertex3d(spotDirection[0], spotDirection[1], spotDirection[2]);
    glEnd();
    glTranslated(spotDirection[0], spotDirection[1], spotDirection[2]);
    glColor3fv(diffuseColor);
    gluSphere(qobj, 0.02, 5, 5);
  }
  if(boundObject)
    glPopMatrix();
  glPopMatrix();

  // get the state back
  glPopAttrib();
}

bool Light::createShadowMap()
{
  if(shadowMapSize > 0)
  {
    if(!(GLEW_ARB_shadow && GLEW_ARB_depth_texture))
      hardwareShadowing = false;
    if(!(GLEW_ARB_multitexture && GLEW_ARB_texture_env_combine))
    {
      shadowMapSize = 0;
      return false;
    }

    //Create the shadow map texture
    if(hardwareShadowing)
    {
      glGenTextures(1, &shadowMapID);
      glBindTexture(GL_TEXTURE_2D, shadowMapID);
      glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
        shadowMapSize, shadowMapSize, 0,
        GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
    }
    else
    {
      shadowMapData8Bit=new GLubyte[shadowMapSize*shadowMapSize];
      if(!shadowMapData8Bit)
      {
        GLHelper::getGLH()->glPrintText(TextObject::WARNING_HINTS, TextObject::WARNING, 5000,
          "Unable to allocate memory for 8 bit shadow map data");
        shadowMapSize = 0;
        return false;
      }
      glGenTextures(1, &shadowMapID);
      glBindTexture(GL_TEXTURE_2D, shadowMapID);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA8, shadowMapSize, shadowMapSize,
        0, GL_ALPHA, GL_UNSIGNED_BYTE, shadowMapData8Bit);

      //Create the 8 bit depth ramp texture
      depthRampData8Bit=new GLubyte[256];
      if(!depthRampData8Bit)
      {
        GLHelper::getGLH()->glPrintText(TextObject::WARNING_HINTS, TextObject::WARNING, 5000,
          "Unable to allocate memory for 8 bit shadow map data");
        shadowMapSize = 0;
        delete [] shadowMapData8Bit;
        shadowMapData8Bit = 0;
        return false;
      }
      // fill ramp texture
      for(int i=0; i<256; i++)
        depthRampData8Bit[i]=i;

      glGenTextures(1, &depthRampMapID);
      glBindTexture(GL_TEXTURE_1D, depthRampMapID);
      glTexImage1D(GL_TEXTURE_1D, 0, GL_ALPHA8, 256,
        0, GL_ALPHA, GL_UNSIGNED_BYTE, depthRampData8Bit);

      delete [] depthRampData8Bit;
      depthRampData8Bit = 0;
    }
  }
  return true;
}

void Light::prepareShadowTexture(bool shader)
{
  glPushMatrix();
  glLoadIdentity();
  glMultMatrixf(scaleMatrix2D);
  glMultMatrixf(light_projection_matrix);
  glMultMatrixf(light_modelview_matrix);
  glGetFloatv(GL_MODELVIEW_MATRIX, textureMatrix);
  glPopMatrix();

  if(!shader)
  {
    transposeTextureMatrix(textureMatrix);

    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
    glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
    glTexGeni(GL_Q, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);

    glTexGenfv(GL_S, GL_EYE_PLANE, &(textureMatrix[0]));
    glTexGenfv(GL_T, GL_EYE_PLANE, &(textureMatrix[4]));
    glTexGenfv(GL_R, GL_EYE_PLANE, &(textureMatrix[8]));
    glTexGenfv(GL_Q, GL_EYE_PLANE, &(textureMatrix[12]));
  }
  else
  {
    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    glMultMatrixf(textureMatrix);
    glMatrixMode(GL_MODELVIEW);
  }
}

void Light::prepare8BitShadowTexture()
{
  glPushMatrix();
  glLoadIdentity();
  glMultMatrixf(scaleMatrix2D);
  glMultMatrixf(light_projection_matrix);
  glMultMatrixf(light_modelview_matrix);
  glGetFloatv(GL_MODELVIEW_MATRIX, textureMatrix);
  glPopMatrix();

  transposeTextureMatrix(textureMatrix);

  glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
  glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
  glTexGeni(GL_Q, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

  glTexGenfv(GL_S, GL_EYE_PLANE, &(textureMatrix[0]));
  glTexGenfv(GL_T, GL_EYE_PLANE, &(textureMatrix[4]));
  glTexGenfv(GL_Q, GL_EYE_PLANE, &(textureMatrix[12]));
}

void Light::prepare1DProjection()
{
  glPushMatrix();
  glLoadIdentity();
  glMultMatrixf(scaleMatrix1D);
  glMultMatrixf(light_projection_matrix);
  glMultMatrixf(light_modelview_matrix);
  glGetFloatv(GL_MODELVIEW_MATRIX, textureMatrix1D);
  glPopMatrix();

  transposeTextureMatrix(textureMatrix1D);

  glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
  glTexGeni(GL_Q, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);

  glTexGenfv(GL_S, GL_EYE_PLANE, &(textureMatrix[0]));
  glTexGenfv(GL_Q, GL_EYE_PLANE, &(textureMatrix[12]));
}

inline void Light::transposeTextureMatrix(GLfloat* matrix)
{
  float v;
  v=matrix[1]; matrix[1]=matrix[4]; matrix[4]=v;
  v=matrix[2]; matrix[2]=matrix[8]; matrix[8]=v;
  v=matrix[3]; matrix[3]=matrix[12]; matrix[12]=v;
  v=matrix[6]; matrix[6]=matrix[9]; matrix[9]=v;
  v=matrix[7]; matrix[7]=matrix[13]; matrix[13]=v;
  v=matrix[11]; matrix[11]=matrix[14]; matrix[14]=v;
}
