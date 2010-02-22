/**
* @file Environment.h
* 
* Definition of class Environment
*
* @author <A href="mailto:pachur@informatik.uni-bremen.de">Dennis Pachur</A>
*/ 

#ifndef ENVIRONMENT_H_
#define ENVIRONMENT_H_

#include <Tools/Errors.h>
#include <Parser/ParserUtilities.h>
#include "CubeMap.h"

class GraphicsManager;

class Environment
{
private:
  /** The texture coordinate generation mode */
  GLenum cubeMapGenMode;

  /** The name of the environment */
  std::string name;

  /** Displaylist handle */
  GLuint dlhandle;

  /** A flag if environment mapping is supported */
  GLboolean environment_mapping_supported;

  /** Vector with texture ids from cube map */
  std::vector<GLuint> texIDhandles;

  /** A list of referenced lights */
  std::vector<std::string> lights;

  /** A pointer to the GraphicsManager */
  GraphicsManager* graphicsManager;

  /** The current camera position */
  Vector3d camPosition;

  /** Some Matrices */
  GLdouble texmatrix[16], mvmatrix[16];

  /** lights have to be initialized once */
  bool lightsHaveBeenInitialized;

  /** Set texture matrix as inverse modelview matrix*/
  void setInvMVMatrixAsTextureMatrix();

public:
  Environment(const AttributeSet& attributes, GraphicsManager* gm);

  ~Environment();

  /** The surrounding setting */
  CubeMap cubeMap;

  /** Get the environments name
  * @return The environments name
  */
  std::string getName()
  { return name; };

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

  /**
  * Initialize the environment
  * @return true if the settings have been loaded, false if the environment has
  *         been initialized before
  */
  bool initEnvironment();

  /**
  * Disable the environment
  */
  void disableEnvironment();

  /**
  * Render the environment
  */
  void render();

  /**
  * Create a display list for faster rendering
  */
  void createDL();

  /**
  * Enables the next texture unit and prepares it for environment mapping
  * @param reflectivity The reflectivity of the material
  * @param The texture unit offset which has to be used
  */
  void enableEvironmentMapping(GLfloat* reflectivity,
    unsigned int textureUnitOffset);

  /**
  * Disables environment mapping
  * @param The texture unit offset which has to be used
  */
  void disableEvironmentMapping(unsigned int textureUnitOffset);
};

#endif //ENVIRONMENT_H_
