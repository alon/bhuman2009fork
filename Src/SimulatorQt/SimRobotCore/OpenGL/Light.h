/**
* @file Light.h
* 
* Definition of class Light
*
* @author <A href="mailto:pachur@informatik.uni-bremen.de">Dennis Pachur</A>
*/

#ifndef LIGHT_H_
#define LIGHT_H_

#include <Parser/ParserUtilities.h>
#include <Tools/Errors.h>
#include <GL/glew.h>

class SimObject;

class Light
{
protected:
  /** A possible object the light is bound to */
  const SimObject* boundObject;

  /** The matrix of the shadow map texture coordinates */
  GLfloat textureMatrix[16];
  GLfloat textureMatrix1D[16];

  /** Transpose the texture matrix
  * @param matrix A pointer to the texture matrix to process
  */
  void transposeTextureMatrix(GLfloat* matrix);

public:
  std::string name;
  GLenum lightID;

  GLfloat ambientColor[4];
  GLfloat diffuseColor[4];
  GLfloat specularColor[4];

  GLfloat position[4];
  GLfloat boundPosition[4];

  GLfloat constantAttenuation;
  GLfloat linearAttenuation;
  GLfloat quadraticAttenuation;

  GLfloat spotCutoff;
  GLfloat spotExponent;
  GLfloat spotDirection[3];
  GLfloat boundSpotDirection[3];

  /** The texture number given by OpenGL */
  GLuint shadowMapID;
  GLuint depthRampMapID;

  /** The size of the shadow map */
  unsigned int shadowMapSize;

  /** Data for 8-bite shadow mapping (no hardware support) */
  GLboolean hardwareShadowing;
  GLubyte* shadowMapData8Bit;
  GLubyte* depthRampData8Bit;

  /** The clipping planes of the shadow map rendering */
  GLuint shadowMapFarClippingPlane,
    shadowMapNearClippingPlane;

  /* The intensity of the casted shadows */
  GLfloat shadowMapIntensity[4];

  /** The modelview matrix from the lights view */
  GLfloat light_projection_matrix[16];
  GLfloat light_modelview_matrix[16];

  /** standard constructor */
  Light(const AttributeSet& attributes);

  /** standard destructor */
  ~Light();

  /** Parses the attributes of the light object which are
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

  /** Bind light to an object
  * simObject The pointer to the object
  */
  void setObjectBinding(const SimObject* simObject);

  /** Update the light position */
  void updateLightPosition();

  /** Do the transformations for the shadow mapping */
  bool updateBoundObjectTransformation();

  /** Draw the lights cone
  * @param qobj The GLU q object for rendering advanced primitives
  */
  void drawLightCones(GLUquadric* qobj);

  /** create a shadow map for stroring depth information
  * @return If the routine could finish
  */
  bool createShadowMap();

  /** set up texture generation parameters */
  void prepareShadowTexture(bool shader);
  void prepare8BitShadowTexture();
  void prepare1DProjection();

  /** get the spot direction as vector3d
  * @param The vector to assign the spot direction
  */
  void getSpotDirectionVector(Vector3d& vec);

  /** get the position as vector3d
  * @param The vector to assign the position
  */
  void getPositionVector(Vector3d& vec);

  /** get the lights target as vector3d
  * @param The vector to assign the target
  */
  void getTargetVector(Vector3d& vec);

  /** Set the ambient color
  * @param red The new red value
  * @param green The new green value
  * @param blue The new blue value
  * @param alpha The new alpha value (1.0)
  */
  void setAmbientColor(GLfloat& red, GLfloat& green, GLfloat& blue, GLfloat& alpha);

  /** Set the ambient color
  * @param red The new red value
  * @param green The new green value
  * @param blue The new blue value
  * @param alpha The new alpha value (1.0)
  */
  void setDiffuseColor(GLfloat& red, GLfloat& green, GLfloat& blue, GLfloat& alpha);

  /** Set the specular color
  * @param red The new red value
  * @param green The new green value
  * @param blue The new blue value
  * @param alpha The new alpha value (1.0)
  */
  void setSpecularColor(GLfloat& red, GLfloat& green, GLfloat& blue, GLfloat& alpha);

  /** Set the position
  * @param x The new x coordinate
  * @param y The new y coordinate
  * @param z The new z coordinate
  * @param w The new w coordinate (1.0)
  */
  void setPosition(GLfloat& x, GLfloat& y, GLfloat& z, GLfloat& w);

  /** Set the constant attenuation
  * @param value The new value
  */
  void setConstantAttenuation(GLfloat& value)
  {constantAttenuation = value;};

  /** Set the linear attenuation
  * @param value The new value
  */
  void setLinearAttenuation(GLfloat& value)
  {linearAttenuation = value;};

  /** Set the quadratic attenuation
  * @param value The new value
  */
  void setQuadraticAttenuation(GLfloat& value)
  {quadraticAttenuation = value;};

  /** Set the spot cutoff
  * @param value The new value
  */
  void setSpotCutoff(GLfloat& value)
  {spotCutoff = value > 180.0f ? 180.0f : value;};

  /** Set the spot exponent
  * @param value The new value
  */
  void setSpotExponent(GLfloat& value)
  {spotExponent = value;};

  /** Set the spot direction
  * @param x The x component of the direction
  * @param y The z component of the direction
  * @param z The y component of the direction
  */
  void setSpotDirection(GLfloat& x, GLfloat& y, GLfloat& z);

  const GLfloat* getConstantAttenuation()
  { return &constantAttenuation; };

  const GLfloat* getLinearAttenuation()
  { return &linearAttenuation; };

  const GLfloat* getQuadraticAttenuation()
  { return &quadraticAttenuation; };

  const GLfloat* getSpotCutoff()
  { return &spotCutoff; };

  const GLfloat* getSpotExponent()
  { return &spotExponent; };
};

#endif //LIGHT_H_
