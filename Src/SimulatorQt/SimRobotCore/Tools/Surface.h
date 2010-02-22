/**
 * @file Surface.h
 * 
 * Definition of class Surface
 *
 * @author <A href="mailto:pachur@informatik.uni-bremen.de">Dennis Pachur</A>
 * @author <A href="mailto:timlaue@informatik.uni-bremen.de">Tim Laue</A>
 */ 

#ifndef SURFACE_H_
#define SURFACE_H_

#include "../Tools/Errors.h"
#include "../Parser/ParserUtilities.h"
#include "../OpenGL/GLtexture.h"

/**
* @class Surface
* A class describing the surface of a drawable object
*/
class Surface
{
private:
  /** Helper Function to init the arrays */
  void init();
  
  /** Helper Function to set the inverted color
  * @param alphainvert If the alpha value should be inverted to
  */
  void setInvertedColor(bool alphainvert);
  
  /** Sets the base color of the surface (can be set only once)*/
  void setBaseColor();

  /** Sets the base ambient color of the surface (can be set only once)*/
  void setBaseAmbientColor();

  /** calculate an ambient color to fit the change of the diffuse color */
  void calculateAmbientColor();

  /**
  * Initialize a texture (helper function for parser)
  * @param attributes The attributes of the element
  * @param texture The texture object to initialize
  * @param line The in the scene description
  * @param column The column in the scene description
  * @param errorManager An object managing errors
  */
  void initTexture(const AttributeSet& attributes,
                   GLTexture& texture,
                   int line, int column, 
                   ErrorManager* errorManager);

  /**
  * Initialize a texture generation mode (helper function for parser)
  * @param attributes The attributes of the element
  * @param genMode The reference of the tex gen mode
  * @param genCoeff The reference of the array of coeffizients
  * @param lin The in the scene description
  * @param column The column in the scene description
  * @param errorManager An object managing errors
  */
  void initTextureGenMode(const AttributeSet& attributes,
                          GLenum& genMode,
                          GLfloat* genCoeff,
                          int line, int column, 
                          ErrorManager* errorManager);

protected:
  /** The base colors */
  GLfloat basecolor[4];
  GLfloat baseAmbientColor[4];

public:
  /** The name of the surface*/
  std::string name;
  /** The color */
  GLfloat color[4];
  GLfloat invcolor[4];
  /** Flag if a seperate Ambient Color has been defined */
  GLboolean hasAmbientColor;
  /** The ambient color of material */
  GLfloat ambientColor[4];
  /** The specular color of material */
  GLfloat specularColor[4];
  /** The shininess of material */
  GLfloat shininess;
  /** The reflectivity of material */
  GLfloat reflectivity;
  GLfloat reflectivityColor[4];
  ///** Dynamic reflectivity values */
  //GLboolean constantUpdate;
  //GLuint    constantUpdateSize;
  /** The emission color of material */
  GLfloat emissionColor[4];
  /** The diffuse texture file*/
  GLTexture diffuseTexture;
  /** Choice of GL_TEXTURE_GEN_MODE S and its coefficients */
  GLenum texGen_s_mode;
  GLfloat texGen_s_coeff[4];
  /** Choice of GL_TEXTURE_GEN_MODE T and its coefficients */
  GLenum texGen_t_mode;
  GLfloat texGen_t_coeff[4];
  /** Choice of GL_TEXTURE_GEN_MODE R and its coefficients */
  GLenum texGen_r_mode;
  GLfloat texGen_r_coeff[4];
  /** Flag if a Eye Linear Mode exists */
  GLboolean aEyeLinearModeExists;
  
  /** Constructor */
  Surface()
  {
    init();
  }

  /** Constructor (Parser) */
  Surface(const AttributeSet& attributes) 
  {
    name = ParserUtilities::getValueFor(attributes, "name");
    init();
  }

  /** Sets the color of the surface
  * @param colorarray A point to the float[4]
  */
  void setColorv(const GLfloat* colorarray);

  /** Sets the color of the surface
  * @param red The red value of the color
  * @param green The green value of the color
  * @param blue The blue value of the color
  * @param alpha The alpha value of the color
  */
  void setColor(const GLfloat red, const GLfloat green,
                 const GLfloat blue, const GLfloat alpha);

  /** Get the base color of the surface */
  const GLfloat* getBaseColor()
  { return basecolor; }

  /** Get the base ambient color of the surface */
  const GLfloat* getBaseAmbientColor()
  { return baseAmbientColor; }

  /** Get the shininess
  * @return A pointer the shininess
  */
  GLfloat* getShininess()
  { return &shininess; }

  /**
  * Parses the attributes of a simulated object which are
  * defined in seperate elements
  * @param name The name of the attribute element
  * @param attributes The attributes of the element
  * @param line The line in the scene description
  * @param column The column in the scene description
  * @param errorManager An object managing errors
  */
  void parseAttributeElements(const std::string& name, 
                              const AttributeSet& attributes,
                              int line, int column, 
                              ErrorManager* errorManager);
};

#endif //SURFACE_H_
