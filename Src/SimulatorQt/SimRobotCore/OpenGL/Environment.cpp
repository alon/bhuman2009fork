/**
* @file Environment.cpp
* 
* Definition of class Environment
*
* @author <A href="mailto:pachur@informatik.uni-bremen.de">Dennis Pachur</A>
*/
#include "Environment.h"
#include <Tools/SimGraphics.h>
#include <OpenGL/GLHelper.h>
#include <OpenGL/Constants.h>

Environment::Environment(const AttributeSet& attributes, GraphicsManager* gm) 
{
  name = ParserUtilities::getValueFor(attributes, "name");
  graphicsManager = gm;
  cubeMapGenMode = GL_NORMAL_MAP;
  dlhandle = 0;
  environment_mapping_supported = false;
  lightsHaveBeenInitialized = false;
}

Environment::~Environment()
{
  lights.clear();
  texIDhandles.clear();
  if(dlhandle != 0)
    glDeleteLists(dlhandle, 1);
}

void Environment::parseAttributeElements(const std::string& name, 
                                         const AttributeSet& attributes,
                                         int line, int column, 
                                         ErrorManager* errorManager)
{
  if(name == "CubeMap")
  {
    std::string cubemapmode = ParserUtilities::getValueFor(attributes,"mode");
    if(cubemapmode == "REFLECTION_MAP")
      cubeMapGenMode = GL_REFLECTION_MAP;
    cubeMap.setUseAsDirectTextures(ParserUtilities::toBool(
      ParserUtilities::getValueFor(attributes,"useAsDirectTextures")));
  }
  else if(name == "SimpleCubeMap")
  {
    std::string cubemapmode = ParserUtilities::getValueFor(attributes,"mode");
    if(cubemapmode == "REFLECTION_MAP")
      cubeMapGenMode = GL_REFLECTION_MAP;
    cubeMap.setSingleTexture();
    cubeMap.setUseAsDirectTextures(ParserUtilities::toBool(
      ParserUtilities::getValueFor(attributes,"useAsDirectTextures")));
  }
  else if(name == "PosX_Texture")
  {
    cubeMap.setCubeMapTexture(GL_TEXTURE_CUBE_MAP_POSITIVE_X,
      ParserUtilities::getValueFor(attributes,"file"));
  }
  else if(name == "PosX_Texture")
  {
    cubeMap.setCubeMapTexture(GL_TEXTURE_CUBE_MAP_POSITIVE_X,
      ParserUtilities::getValueFor(attributes,"file"));
  }
  else if(name == "NegX_Texture")
  {
    cubeMap.setCubeMapTexture(GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
      ParserUtilities::getValueFor(attributes,"file"));
  }
  else if(name == "PosY_Texture")
  {
    cubeMap.setCubeMapTexture(GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
      ParserUtilities::getValueFor(attributes,"file"));
  }
  else if(name == "NegY_Texture")
  {
    cubeMap.setCubeMapTexture(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
      ParserUtilities::getValueFor(attributes,"file"));
  }
  else if(name == "PosZ_Texture")
  {
    cubeMap.setCubeMapTexture(GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
      ParserUtilities::getValueFor(attributes,"file"));
  }
  else if(name == "NegZ_Texture")
  {
    cubeMap.setCubeMapTexture(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
      ParserUtilities::getValueFor(attributes,"file"));
  }
  else if(name == "Single_Texture")
  {
    cubeMap.setCubeMapTexture(GL_TEXTURE_CUBE_MAP,
      ParserUtilities::getValueFor(attributes,"file"));
  }
  else if(name == "Light")
  {
    std::string lightname = ParserUtilities::getValueFor(attributes,"ref");
    if(lightname.size() > 0)
      lights.push_back(lightname);
  }
  else
  {
    errorManager->addError("Unknown Environment Parameter",
      "The provided EnvironmentDefinition parameter \"" + name + "\" is unknown", line, column);
  }
}

bool Environment::initEnvironment()
{
  // Environment has to be initialized once per render context
  cubeMap.initCubeMap();
  if(cubeMap.posxTexture.getWidth() != graphicsManager->GLSETTINGS_CubeMap_Size &&
    graphicsManager->reflectionObject)
  {
    GLHelper::getGLH()->glPrintText(TextObject::WARNING_HINTS, TextObject::WARNING, 5000, "Your Cube Map Generation Size varies from the loaded textures. Static Environment mapping will be disabled.");
    cubeMap.changeTexSizes(graphicsManager->GLSETTINGS_CubeMap_Size);
  }
  if(!GLEW_ARB_texture_cube_map)
    GLHelper::getGLH()->glPrintText(TextObject::WARNING_HINTS, TextObject::WHITE, 5000, "Cube maps are not supported by your hardware. Environment mapping will be disabled.");

  // but lights only once
  if(!lightsHaveBeenInitialized)
  {
    std::vector<std::string>::const_iterator iter;
    for(iter = lights.begin(); iter != lights.end(); iter++)
    {
      switch(graphicsManager->preEnableLight(*iter))
      {
      case 1:
        GLHelper::getGLH()->glPrintText(TextObject::WARNING_HINTS, TextObject::WARNING, 5000,
          "Unknown Light. %s refers to no valid LightDefinition.", iter->c_str());
        break;
      case 2: 
        GLHelper::getGLH()->glPrintText(TextObject::WARNING_HINTS, TextObject::WARNING, 5000,
          "Too many Lights. %s got no valid lightID. The maximum light number is 8.", iter->c_str());
        break;
      default:
        break;
      }
    }
    lightsHaveBeenInitialized = true;
  }

  return true;
}

void Environment::disableEnvironment()
{
  std::vector<std::string>::const_iterator iter;
  for(iter = lights.begin(); iter != lights.end(); iter++)
    graphicsManager->disableLight(*iter);
}

void Environment::setInvMVMatrixAsTextureMatrix()
{
  glGetDoublev(GL_MODELVIEW_MATRIX, mvmatrix);

  texmatrix[0] = mvmatrix[0];
  texmatrix[1] = mvmatrix[4];
  texmatrix[2] = mvmatrix[8];
  texmatrix[3] = mvmatrix[12];

  texmatrix[4] = mvmatrix[1];
  texmatrix[5] = mvmatrix[5];
  texmatrix[6] = mvmatrix[9];
  texmatrix[7] = mvmatrix[13];

  texmatrix[8] = mvmatrix[2];
  texmatrix[9] = mvmatrix[6];
  texmatrix[10] = mvmatrix[10];
  texmatrix[11] = mvmatrix[14];

  texmatrix[12] = -mvmatrix[0]*mvmatrix[3] -mvmatrix[4]*mvmatrix[7] -mvmatrix[8]*mvmatrix[11];
  texmatrix[13] = -mvmatrix[1]*mvmatrix[3] -mvmatrix[5]*mvmatrix[7] -mvmatrix[9]*mvmatrix[11];
  texmatrix[14] = -mvmatrix[2]*mvmatrix[3] -mvmatrix[6]*mvmatrix[7] -mvmatrix[10]*mvmatrix[11];
  texmatrix[15] = mvmatrix[15];

  glMatrixMode(GL_TEXTURE);
  glLoadMatrixd(texmatrix);
  glMatrixMode(GL_MODELVIEW);
}

void Environment::enableEvironmentMapping(GLfloat* reflectivity,
                                          unsigned int textureUnitOffset)
{
  if(!GLEW_VERSION_1_3)
    return;
  glActiveTexture(GL_TEXTURE0 + ++textureUnitOffset);
  setInvMVMatrixAsTextureMatrix();
  glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, reflectivity);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
  glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_INTERPOLATE);
  glTexEnvf(GL_TEXTURE_ENV, GL_SRC0_RGB, GL_TEXTURE);
  glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
  glTexEnvf(GL_TEXTURE_ENV, GL_SRC1_RGB, GL_PREVIOUS);
  glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);
  glTexEnvf(GL_TEXTURE_ENV, GL_SRC2_RGB, GL_CONSTANT);
  glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND2_RGB, GL_SRC_ALPHA);
  glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, cubeMapGenMode);
  glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, cubeMapGenMode);
  glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, cubeMapGenMode);
  glEnable(GL_TEXTURE_GEN_S);
  glEnable(GL_TEXTURE_GEN_T);
  glEnable(GL_TEXTURE_GEN_R);
  glEnable(GL_TEXTURE_CUBE_MAP);
  glActiveTexture(GL_TEXTURE0 + --textureUnitOffset);
}

void Environment::disableEvironmentMapping(unsigned int textureUnitOffset)
{
  if(!GLEW_VERSION_1_3)
    return;
  glActiveTexture(GL_TEXTURE0 + ++textureUnitOffset);
  glMatrixMode(GL_TEXTURE);
  glLoadIdentity();
  glMatrixMode(GL_MODELVIEW);
  glDisable(GL_TEXTURE_GEN_S);
  glDisable(GL_TEXTURE_GEN_T);
  glDisable(GL_TEXTURE_GEN_R);
  glDisable(GL_TEXTURE_CUBE_MAP);
  glActiveTexture(GL_TEXTURE0 + --textureUnitOffset);
}

void Environment::render()
{
  if(cubeMap.getUseAsDirectTextures())
    glCallList(dlhandle);
}

void Environment::createDL()
{
  if((dlhandle == 0 || !glIsList(dlhandle)) && cubeMap.getUseAsDirectTextures())
  {
    if(dlhandle == 0)
      dlhandle = glGenLists(1);
    glNewList(dlhandle, GL_COMPILE);
    {
      glEnableClientState(GL_VERTEX_ARRAY);
      glVertexPointer(3, GL_FLOAT, 0, environmentCubeVertices);
      if(texIDhandles.empty())
        texIDhandles = cubeMap.getTextureIDs();
      if(texIDhandles.size() > 0)
      {
        if(GLEW_VERSION_1_3)
          glActiveTexture(GL_TEXTURE0);
        glEnable(GL_TEXTURE_2D);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(2, GL_FLOAT, 0, environmentCubeTexCoords);

        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, nocolor);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);

        if(texIDhandles.size() == 1)
        {
          glBindTexture(GL_TEXTURE_2D, texIDhandles[0]);
          glDrawArrays(GL_QUADS, 0, 24);
        }
        else
        {
          glBindTexture(GL_TEXTURE_2D, texIDhandles[0]);
          glDrawArrays(GL_QUADS, 0, 4);
          glBindTexture(GL_TEXTURE_2D, texIDhandles[1]);
          glDrawArrays(GL_QUADS, 4, 4);
          glBindTexture(GL_TEXTURE_2D, texIDhandles[2]);
          glDrawArrays(GL_QUADS, 8, 4);
          glBindTexture(GL_TEXTURE_2D, texIDhandles[3]);
          glDrawArrays(GL_QUADS, 12, 4);
          glBindTexture(GL_TEXTURE_2D, texIDhandles[4]);
          glDrawArrays(GL_QUADS, 16, 4);
          glBindTexture(GL_TEXTURE_2D, texIDhandles[5]);
          glDrawArrays(GL_QUADS, 20, 4);
        }

        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glDisable(GL_TEXTURE_2D);
      }
      else
        glDrawArrays(GL_QUADS, 0, 24);
      glDisableClientState(GL_VERTEX_ARRAY);
    }
    glEndList();
  }
}
