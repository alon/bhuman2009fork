/**
 * @file Surface.cpp
 * 
 * Definition of class Surface
 *
 * @author <A href="mailto:pachur@informatik.uni-bremen.de">Dennis Pachur</A>
 * @author <A href="mailto:timlaue@informatik.uni-bremen.de">Tim Laue</A>
 */
#include "Surface.h"

void Surface::init()
{
  // standard color: light grey
  color[0] = 0.8f;
  color[1] = 0.8f;
  color[2] = 0.8f;
  color[3] = 1.0f;

  basecolor[0] = 0.8f;
  basecolor[1] = 0.8f;
  basecolor[2] = 0.8f;
  basecolor[3] = 0.8f;

  invcolor[0] = 0.2f;
  invcolor[1] = 0.2f;
  invcolor[2] = 0.2f;
  invcolor[3] = 1.0f;
  
  // using OpenGL 2.0 standard values for material properties
  hasAmbientColor = false;

  ambientColor[0] = 0.2f;
  ambientColor[1] = 0.2f;
  ambientColor[2] = 0.2f;
  ambientColor[3] = 1.0f;

  baseAmbientColor[0] = 0.2f;
  baseAmbientColor[1] = 0.2f;
  baseAmbientColor[2] = 0.2f;
  baseAmbientColor[3] = 1.0f;

  specularColor[0] = 0.0f;
  specularColor[1] = 0.0f;
  specularColor[2] = 0.0f;
  specularColor[3] = 1.0f;

  shininess = 0.0f;

  reflectivity = 0.0f;
  reflectivityColor[0] = 0.0f;
  reflectivityColor[1] = 0.0f;
  reflectivityColor[2] = 0.0f;
  reflectivityColor[3] = reflectivity;

  //constantUpdate = false;
  //constantUpdateSize = 64;

  emissionColor[0] = 0.0f;
  emissionColor[1] = 0.0f;
  emissionColor[2] = 0.0f;
  emissionColor[3] = 1.0f;

  texGen_s_mode = GL_OBJECT_LINEAR;
  texGen_s_coeff[0] = 1.0;
  texGen_s_coeff[1] = 0.0;
  texGen_s_coeff[2] = 0.0;
  texGen_s_coeff[3] = 0.0;

  texGen_t_mode = GL_OBJECT_LINEAR;
  texGen_t_coeff[0] = 0.0;
  texGen_t_coeff[1] = 1.0;
  texGen_t_coeff[2] = 0.0;
  texGen_t_coeff[3] = 0.0;

  aEyeLinearModeExists = false;
}

void Surface::setInvertedColor(bool alphainvert)
{
  invcolor[0] = 1.0f - color[0];
  invcolor[1] = 1.0f - color[1];
  invcolor[2] = 1.0f - color[2];
  invcolor[3] = alphainvert ? 1.0f - color[3] : 1.0f;
}

void Surface::setBaseColor()
{
  basecolor[0] = color[0];
  basecolor[1] = color[1];
  basecolor[2] = color[2];
  basecolor[3] = color[3];
}

void Surface::setBaseAmbientColor()
{
  baseAmbientColor[0] = ambientColor[0];
  baseAmbientColor[1] = ambientColor[1];
  baseAmbientColor[2] = ambientColor[2];
  baseAmbientColor[3] = ambientColor[3];
}

void Surface::setColorv(const GLfloat* colorarray)
{
  // set color
  color[0] = colorarray[0];
  color[1] = colorarray[1];
  color[2] = colorarray[2];
  color[3] = colorarray[3];
  invcolor[0] = 1.0f - color[0];
  invcolor[1] = 1.0f - color[1];
  invcolor[2] = 1.0f - color[2];
  invcolor[3] = 1.0f;

  // set ambient color
  if(hasAmbientColor)
    calculateAmbientColor();
}

void Surface::setColor(const GLfloat red, const GLfloat green,
                       const GLfloat blue, const GLfloat alpha)
{
  // set color
  color[0] = red;
  color[1] = green;
  color[2] = blue;
  color[3] = alpha;
  invcolor[0] = 1.0f - red;
  invcolor[1] = 1.0f - green;
  invcolor[2] = 1.0f - blue;
  invcolor[3] = 1.0f;

  // set ambient color
  if(hasAmbientColor)
    calculateAmbientColor();
}

void Surface::calculateAmbientColor()
{
  ambientColor[0] = basecolor[0] == 0.0f ? ambientColor[0] : (baseAmbientColor[0]*color[0])/basecolor[0];
  ambientColor[1] = basecolor[1] == 0.0f ? ambientColor[1] : (baseAmbientColor[1]*color[1])/basecolor[1];
  ambientColor[2] = basecolor[2] == 0.0f ? ambientColor[2] : (baseAmbientColor[2]*color[2])/basecolor[2];
  ambientColor[3] = color[3];
}

void Surface::parseAttributeElements(const std::string& name, 
                                     const AttributeSet& attributes,
                                     int line, int column, 
                                     ErrorManager* errorManager)
{
  if(name == "Color")
  {
    ParserUtilities::toRGBA(attributes, color);
    setBaseColor();
    setInvertedColor(false);
  }
  else if(name == "AmbientColor")
  {
    ParserUtilities::toRGBA(attributes, ambientColor);
    setBaseAmbientColor();
    hasAmbientColor = true;
  }
  else if(name == "SpecularColor")
  {
    ParserUtilities::toRGBA(attributes, specularColor);
  }
  else if(name == "Shininess")
  {
    shininess = (GLfloat)ParserUtilities::toDouble(attributes[0].value);
  }
  else if(name == "EmissionColor")
  {
    ParserUtilities::toRGBA(attributes, emissionColor);
  }
  else if(name == "DiffuseTexture")
  {
    initTexture(attributes, diffuseTexture, line, column, errorManager);      
  }
  else if(name == "BorderColor")
  {
    ParserUtilities::toRGBA(attributes, diffuseTexture.getBorderColorPtr());
  }
  else if(name == "TextureGenModeS")
  {
    initTextureGenMode(attributes, texGen_s_mode, texGen_s_coeff, line, column, errorManager);
  }
  else if(name == "TextureGenModeT")
  {
    initTextureGenMode(attributes, texGen_t_mode, texGen_t_coeff, line, column, errorManager);
  }
  else if(name == "Reflectivity")
  {
    for(unsigned int i=0; i < attributes.size(); i++)
    {
      if(attributes[i].attribute == "value")
      {
        reflectivity = (GLfloat)ParserUtilities::toDouble(attributes[i].value);
        reflectivityColor[3] = reflectivity;
      }
      /*else if(attributes[i].attribute == "constantUpdate")
        constantUpdate = ParserUtilities::toBool(attributes[i].value);
      else if(attributes[i].attribute == "size")
        constantUpdateSize = ParserUtilities::toInt(attributes[i].value);*/
    }
  }
  else
  {
    errorManager->addError("Unknown Surface Parameter",
      "The provided AppearanceDefinition parameter \"" + name + "\" is unknown", line, column);
  }
}

void Surface::initTexture(const AttributeSet& attributes,
                          GLTexture& texture,
                          int line, int column, 
                          ErrorManager* errorManager)
{
  for(unsigned int i=0; i< attributes.size(); i++)
  {
    if (attributes[i].attribute == "file")
      texture.setTexName(attributes[i].value);
    else if(attributes[i].attribute == "mode")
    {
      if(attributes[i].value == "GL_MODULATE")
        texture.setMode(GL_MODULATE);
      else if(attributes[i].value == "GL_REPLACE")
        texture.setMode(GL_REPLACE);
      else if(attributes[i].value == "GL_ADD")
        texture.setMode(GL_ADD);
      else if(attributes[i].value == "GL_DECAL")
        texture.setMode(GL_DECAL);
    }
    else if (attributes[i].attribute == "wrapS")
    {
      GLenum mode = (GLenum)ParserUtilities::toTextureWrapMode(attributes[i].value);
      if(mode == GL_NONE)
        errorManager->addError("Unknown Texture Mode", "Unknown Texture Mode \""
          + attributes[i].value + "\". Try( GL_CLAMP | GL_CLAMP_TO_EDGE | GL_CLAMP_TO_BORDER | GL_MIRRORED_REPEAT | GL_REPEAT)", line, column);
      else
        texture.setWrapS(mode);
    }
    else if (attributes[i].attribute == "wrapT")
    {
      GLenum mode = (GLenum)ParserUtilities::toTextureWrapMode(attributes[i].value);
      if(mode == GL_NONE)
        errorManager->addError("Unknown Texture Mode", "Unknown Texture Mode \""
          + attributes[i].value + "\". Try( GL_CLAMP | GL_CLAMP_TO_EDGE | GL_CLAMP_TO_BORDER | GL_MIRRORED_REPEAT | GL_REPEAT)", line, column);
      else
        texture.setWrapT(mode);
    }
    else if (attributes[i].attribute == "wrapR")
    {
      GLenum mode = (GLenum)ParserUtilities::toTextureWrapMode(attributes[i].value);
      if(mode == GL_NONE)
        errorManager->addError("Unknown Texture Mode", "Unknown Texture Mode \""
          + attributes[i].value + "\". Try( GL_CLAMP | GL_CLAMP_TO_EDGE | GL_CLAMP_TO_BORDER | GL_MIRRORED_REPEAT | GL_REPEAT)", line, column);
      else
        texture.setWrapR(mode);
    }
    else if (attributes[i].attribute == "priority")
      diffuseTexture.setPriority((GLclampf)ParserUtilities::toDouble(attributes[i].value));
    else
    {
      errorManager->addError("Unknown Texture Parameter",
        "The provided texture parameter \"" + name + "\" is unknown", line, column);
    }
  }
}

void Surface::initTextureGenMode(const AttributeSet& attributes,
                                GLenum& genMode,
                                GLfloat* genCoeff,
                                int line, int column, 
                                ErrorManager* errorManager)
{
  for(unsigned int i=0; i< attributes.size(); i++)
  {
    if(attributes[i].attribute == "mode")
    {
      if(attributes[i].value == "OBJECT_LINEAR")
        genMode = GL_OBJECT_LINEAR;
      else if(attributes[i].value == "EYE_LINEAR")
      {
        genMode = GL_EYE_LINEAR;
        aEyeLinearModeExists = true;
      }
      else
      {
        errorManager->addError("Unknown Texture Generation Mode",
          "The provided texture generation mode \"" + attributes[i].value + "\" is unknown", line, column);
      }
    }
    else if(attributes[i].attribute == "s")
    {
      genCoeff[0] = (GLfloat)ParserUtilities::toDouble(attributes[i].value);
    }
    else if(attributes[i].attribute == "t")
    {
      genCoeff[1] = (GLfloat)ParserUtilities::toDouble(attributes[i].value);
    }
    else if(attributes[i].attribute == "r")
    {
      genCoeff[2] = (GLfloat)ParserUtilities::toDouble(attributes[i].value);
    }
    else if(attributes[i].attribute == "q")
    {
      genCoeff[3] = (GLfloat)ParserUtilities::toDouble(attributes[i].value);
    }
    else
    {
      errorManager->addError("Unknown Texture Generation Mode Parameter",
        "The provided texture generation mode parameter \"" + attributes[i].attribute + "\" is unknown", line, column);
    }
  }
}
