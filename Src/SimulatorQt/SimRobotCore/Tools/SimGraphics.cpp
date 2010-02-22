/**
* @file Tools/SimGraphics.cpp
* 
* Implementation of class GraphicsManager and several datatypes
*
* @author <A href="mailto:pachur@informatik.uni-bremen.de">Dennis Pachur</A>
* @author <A href="mailto:timlaue@informatik.uni-bremen.de">Tim Laue</A>
*/ 

#include <cstring>
#include <cstdlib> //atoi
#include <sstream>
#include <cfloat> // DBL_MAX 
#include <cstdio>

#include "SimGraphics.h"
#include "SimGeometry.h"
#include "../Parser/ParserUtilities.h"
#include "../Simulation/SimObject.h"
#include "../OpenGL/Constants.h"
#include "../Platform/Assert.h"

void VertexList::addVertex(const AttributeSet& attributes)
{
  const std::string name(ParserUtilities::getValueFor(attributes, "name"));
  Vertex vertex(ParserUtilities::toVertex(attributes));
  vertices[name] = vertex;
}

bool VertexList::getVertex(const std::string& name, Vertex& vertex)
{
  NameToVertexMap::const_iterator pos;
  pos = vertices.find(name);
  if(pos == vertices.end())
  {
    return false;
  }
  else
  {
    vertex = pos->second;
    return true;
  }
}

bool VertexList::getVertexNormal(const std::string& name, Vector3d& vertexNormal)
{
  NameToVectorMap::const_iterator pos;
  pos = vertexNormals.find(name);
  if(pos == vertexNormals.end())
  {
    return false;
  }
  else
  {
    vertexNormal = pos->second;
    return true;
  }
}

void VertexList::addSurfaceNormal(const std::string& name, const Vector3d& normal)
{
  vertexNormals[name] += normal;
  counter[name]++;
}

void VertexList::flipVertexNormals()
{
  for(NameToVectorMap::iterator iter = vertexNormals.begin();
    iter != vertexNormals.end(); iter++)
  {
    iter->second *= -1.0;
  }
}

void VertexList::calculateVertexNormals()
{
  for(NameToVectorMap::iterator iter = vertexNormals.begin();
    iter != vertexNormals.end(); iter++)
  {
    iter->second /= counter[iter->first];
    counter[iter->first] = 1;
  }
}

void VertexList::clearVertexNormals()
{
  vertexNormals.clear();
}

void GraphicsPrimitiveDescription::compileDisplayList()
{
  displayListHandle = glGenLists(1);
  glNewList(displayListHandle, GL_COMPILE);
  {
    if(type == TRIANGLE_STRIP || type == POLYGON
      || type == QUAD || type == TRIANGLE)
    {
      glEnableClientState(GL_NORMAL_ARRAY);
      glNormalPointer(GL_DOUBLE, 0, vertexNormals);

      switch(numTextureCoordinates)
      {
      case 2:
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(2, GL_DOUBLE, 0, textureCoordinates);
        break;
      case 1:
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(1, GL_DOUBLE, 0, textureCoordinates);
        break;
      default:
        break;
      }

      glEnableClientState(GL_VERTEX_ARRAY);
      glVertexPointer(3, GL_DOUBLE, 0, vertices);

      if(type == TRIANGLE_STRIP)
        glDrawArrays(GL_TRIANGLE_STRIP, 0, numberOfVertices);
      else if(type == POLYGON)
        glDrawArrays(GL_POLYGON, 0, numberOfVertices);
      else if(type == QUAD)
        glDrawArrays(GL_QUADS, 0, numberOfVertices);
      else if(type == TRIANGLE)
        glDrawArrays(GL_TRIANGLES, 0, numberOfVertices);

      if(numTextureCoordinates > 0)
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
      glDisableClientState(GL_VERTEX_ARRAY);
      glDisableClientState(GL_NORMAL_ARRAY);
    }
    else if(type == GRAPHICS_BOX)
    {
      // top
      glBegin (GL_QUADS);
      glNormal3d (0,0,1);
      glVertex3d (vertices[0],vertices[1],vertices[2]);
      glVertex3d (vertices[3],vertices[4],vertices[5]);
      glVertex3d (vertices[6],vertices[7],vertices[8]);
      glVertex3d (vertices[9],vertices[10],vertices[11]);
      glEnd();
      // back
      glBegin (GL_QUADS);
      glNormal3d(0,1,0);
      glVertex3d (vertices[15],vertices[16],vertices[17]);
      glVertex3d (vertices[12],vertices[13],vertices[14]);
      glVertex3d (vertices[9],vertices[10],vertices[11]);
      glVertex3d (vertices[6],vertices[7],vertices[8]);
      glEnd();
      // bottom
      glBegin (GL_QUADS);
      glNormal3d(0,0,-1);
      glVertex3d (vertices[12],vertices[13],vertices[14]);
      glVertex3d (vertices[15],vertices[16],vertices[17]);
      glVertex3d (vertices[18],vertices[19],vertices[20]);
      glVertex3d (vertices[21],vertices[22],vertices[23]);
      glEnd();
      // right
      glBegin (GL_QUADS);
      glNormal3d (-1,0,0);
      glVertex3d (vertices[0],vertices[1],vertices[2]);
      glVertex3d (vertices[9],vertices[10],vertices[11]);
      glVertex3d (vertices[12],vertices[13],vertices[14]);
      glVertex3d (vertices[21],vertices[22],vertices[23]);
      glEnd();
      // front
      glBegin (GL_QUADS);
      glNormal3d(0,-1,0);
      glVertex3d (vertices[21],vertices[22],vertices[23]);
      glVertex3d (vertices[18],vertices[19],vertices[20]);
      glVertex3d (vertices[3],vertices[4],vertices[5]);
      glVertex3d (vertices[0],vertices[1],vertices[2]);
      glEnd();
      // left
      glBegin (GL_QUADS);
      glNormal3d(1,0,0);
      glVertex3d (vertices[18],vertices[19],vertices[20]);
      glVertex3d (vertices[15],vertices[16],vertices[17]);
      glVertex3d (vertices[6],vertices[7],vertices[8]);
      glVertex3d (vertices[3],vertices[4],vertices[5]);
      glEnd();
    }
    else if(type == GRAPHICS_SPHERE)
    {
      gluSphere(graphicsManager->q, vertices[0], 16, 16);
    }
    else if(type == GRAPHICS_CYLINDER)
    {
      double radius(vertices[0]);
      double height(vertices[1]);
      glTranslated(0,0,-(height*0.5));
      gluCylinder(graphicsManager->q, radius, radius, height, 16, 1);
      glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
      gluDisk(graphicsManager->q, 0, radius, 16, 1);
      glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
      glTranslated(0,0,height);
      gluDisk(graphicsManager->q, 0, radius, 16, 1);
    }
    else if(type == GRAPHICS_CAPPED_CYLINDER)
    {
      double radius(vertices[0]);
      double height(vertices[1]);
      glTranslated(0,0,-(height*0.5));
      gluCylinder(graphicsManager->q, radius, radius, height, 16, 1);
      gluSphere(graphicsManager->q, radius, 16, 16);
      glTranslated(0,0,height);
      gluSphere(graphicsManager->q, radius, 16, 16);
    }
  }
  glEndList();
}


void GraphicsPrimitiveDescription::destroyDisplayList()
{
  glDeleteLists(displayListHandle, 1);
}


void GraphicsPrimitiveDescription::callDisplayList()
{
  glCallList(displayListHandle);
}


void GraphicsPrimitiveDescription::expandAxisAlignedBoundingBox(AxisAlignedBoundingBox& box,
                                                                const Vector3d& position,
                                                                const Matrix3d& rotation) const
{
  Vector3d* vertex = (Vector3d*)&vertices[0];
  Vector3d* end = (Vector3d*)&vertices[numberOfVertices*3];
  /** The number of vertices*/
  while(vertex != end)
  {
    Vector3d p(*vertex);
    p.rotate(rotation);
    p += position;
    box.expand(p);
    vertex++;
  }
}

GraphicsManager::GraphicsManager(const std::string& filename) : 
  glewInitialized(false), showLightCones(false), firstShadowRendering(true), copyTexImage(false), currentVertexList(0),
  displayListsHaveBeenCompiled(false), nextFreeTextureID(1),
  numOfTotalPrimitives(0), numOfTotalVertices(0), numOfTotalPrimitivesIsSet(false),
  numOfTotalVerticesIsSet(false), environment(0), lastScreenshotID(0), currentTextureUnitOffset(0)
{
  shader = new ShaderInterface(this);
  q = gluNewQuadric();
  currentGraphicsPrimitive.graphicsManager = this;

  globalAmbientLight[0] = 0.0;
  globalAmbientLight[1] = 0.0;
  globalAmbientLight[2] = 0.0;
  globalAmbientLight[3] = 1.0;

  // default OpenGL settings (can be reassigned by parsing a OpenGLSettings definition
  GLSETTINGS_LightModel_twoSided = GL_TRUE;
  GLSETTINGS_LightModel_localCameraViewer = GL_TRUE;
  GLSETTINGS_LightModel_localObjectViewer = GL_FALSE;
  GLSETTINGS_Culling_face = GL_NONE;
  GLSETTINGS_Culling_front = GL_CCW;
  GLSETTINGS_Material_face = GL_FRONT_AND_BACK;
  GLSETTINGS_Disable_WGL_Share_Lists = GL_FALSE;
  GLSETTINGS_PolygonOffset_factor = 0.0f;
  GLSETTINGS_PolygonOffset_units = 0.0f;
  GLSETTINGS_CubeMap_CamPos = Vector3d(0.0, 0.0, 0.0);
  GLSETTINGS_CubeMap_Size = 512;
  GLSETTINGS_CubeMap_ClipNear = 0.01;
  GLSETTINGS_CubeMap_ClipFar = 30.0;
  GLSETTINGS_CubeMap_jitterEnd = 8;
  GLSETTINGS_CubeMap_jitterIndex = 42;
  GLSETTINGS_CubeMap_UpdateRate = 1;
  GLSETTINGS_CubeMap_UpdateRate = 0;
  GLSETTINGS_Shader_InitASAP = GL_FALSE;
  GLSETTINGS_Filename = filename;
  GLSETTINGS_UseBaseColors = GL_FALSE;
  GLSETTINGS_Antialiasing_Samples = 0;

  showFPS = false;

  reflectionObject = 0;
  exposure2Physic = 1.0f;
}

GraphicsManager::~GraphicsManager()
{
  // get the opengl contexts to operate on them
  //contextSharingManager.switchToBaseContext();
  glFinish();

  std::map<std::string, VertexList*>::iterator iter;
  for(iter = vertexLists.begin(); iter != vertexLists.end(); ++iter)
    delete (iter->second);

  std::vector<GraphicsPrimitiveDescription>::iterator graIter;
  for(graIter = primitives.begin(); graIter != primitives.end(); ++graIter)
  {
    graIter->destroyDisplayList();
    if(graIter->vertices)
      delete[] graIter->vertices;
    if(graIter->vertexNormals)
      delete[] graIter->vertexNormals;
    if(graIter->textureCoordinates)
      delete[] graIter->textureCoordinates;
    if(graIter->normals)
      delete[] graIter->normals;
  }

  gluDeleteQuadric(q);

  disableAllLights();
  std::vector<Light*>::iterator lightIter;
  for(lightIter = lights.begin(); lightIter != lights.end(); ++lightIter)
    delete *lightIter;
  lights.clear();

  std::map<int, FPS_Counter*>::iterator fpsIter;
  for(fpsIter = fpsCounter.begin(); fpsIter != fpsCounter.end(); fpsIter++)
    delete fpsIter->second;
  fpsCounter.clear();

  if(shader)
    delete shader;

  GLHelper::destroy();
  GLTexture::ClearTexRegister();

  std::map<VisualizationParameterSet::SurfaceStyle, GLuint>::iterator stateDLit;
  for(stateDLit = enableStateDL.begin(); stateDLit != enableStateDL.end(); ++stateDLit)
    if(glIsList(stateDLit->second))
      glDeleteLists(stateDLit->second, 1);
  for(stateDLit = disableStateDL.begin(); stateDLit != disableStateDL.end(); ++stateDLit)
    if(glIsList(stateDLit->second))
      glDeleteLists(stateDLit->second, 1);
}

bool GraphicsManager::doEnvRendering()
{
  if(!reflectionObject)
    return false;

  GLSETTINGS_CubeMap_UpdateCycle++;
  if(GLSETTINGS_CubeMap_UpdateCycle >= GLSETTINGS_CubeMap_UpdateRate)
  {
    // reset update cycle
    GLSETTINGS_CubeMap_UpdateCycle = 0;
    return true;
  }
  return false;
}

// Helper for counting fps
void GraphicsManager::beginFPSCount(const int fbo_reg)
{
  if(showFPS)
  {
    if(fpsCounter.find(fbo_reg) == fpsCounter.end())
      fpsCounter[fbo_reg] = new FPS_Counter();
    fpsCounter[fbo_reg]->begin();
  }
}

void GraphicsManager::endFPSCount(const int fbo_reg)
{
  if(showFPS)
  {
    fpsCounter[fbo_reg]->end();
    std::stringstream sstr;
    sstr << fbo_reg;
    GLHelper::getGLH()->glPrintMarkedText(TextObject::COMMON_HINTS, sstr.str().c_str(), TextObject::WHITE, 1000, "fbo_reg %i: current FPS: %3.1f   average FPS: %3.1f   perframe: %2.3fms   possible FPS: %4.1f",
      fbo_reg, fpsCounter[fbo_reg]->current, fpsCounter[fbo_reg]->average, fpsCounter[fbo_reg]->frametime, fpsCounter[fbo_reg]->possible);
  }
}

unsigned int GraphicsManager::getNumOfTotalPrimitives()
{
  if(!numOfTotalPrimitivesIsSet)
  {
    numOfTotalPrimitives = primitives.size();
    numOfTotalPrimitivesIsSet = true;
  }
  return numOfTotalPrimitives;
}

unsigned int GraphicsManager::getNumOfTotalVertices()
{
  if(!numOfTotalVerticesIsSet)
  {
    std::vector<GraphicsPrimitiveDescription>::iterator iter;
    for(iter = primitives.begin(); iter != primitives.end(); iter++)
    {
      switch(iter->type)
      {
      case GRAPHICS_BOX:
        numOfTotalVertices += 24;
        break;
      case GRAPHICS_CYLINDER:
        numOfTotalVertices += 66;
        break;
      case GRAPHICS_CAPPED_CYLINDER:
        numOfTotalVertices += 288;
        break;
      case GRAPHICS_SPHERE:
        numOfTotalVertices += 256;
        break;
      default:
        numOfTotalVertices += iter->numberOfVertices;
        break;
      }
    }
    numOfTotalVerticesIsSet = true;
  }
  return numOfTotalVertices;
}

unsigned int GraphicsManager::getNumVertForCS(GraphicsPrimitiveHandle gph) const
{
  return primitives[gph].numberOfVertices;
}

void GraphicsManager::parseOpenGLSettings(const std::string& name, 
                                          const AttributeSet& attributes,
                                          int line, int column, 
                                          ErrorManager* errorManager)
{
  if(name == "LightModel")
  {
    for(unsigned int i=0; i< attributes.size(); i++)
    {
      if(attributes[i].attribute == "twoSided")
      {
        if(ParserUtilities::toBool(attributes[i].value))
          GLSETTINGS_LightModel_twoSided = GL_TRUE;
        else
          GLSETTINGS_LightModel_twoSided = GL_FALSE;
      }
      else if(attributes[i].attribute == "localCameraViewer")
      {
        if(ParserUtilities::toBool(attributes[i].value))
          GLSETTINGS_LightModel_localCameraViewer = GL_TRUE;
        else
          GLSETTINGS_LightModel_localCameraViewer = GL_FALSE;
      }
      else if(attributes[i].attribute == "localObjectViewer")
      {
        if(ParserUtilities::toBool(attributes[i].value))
          GLSETTINGS_LightModel_localObjectViewer = GL_TRUE;
        else
          GLSETTINGS_LightModel_localObjectViewer = GL_FALSE;
      }
    }
  }
  else if(name == "Antialiasing")
  {
     for(unsigned int i=0; i< attributes.size(); i++)
     {
       if(attributes[i].attribute == "samples")
         GLSETTINGS_Antialiasing_Samples = atoi(attributes[i].value.c_str());
     }
  }
  else if(name == "UseBaseColors")
  {
    GLSETTINGS_UseBaseColors = GL_TRUE;
  }
  else if(name == "Culling")
  {
    for(unsigned int i=0; i< attributes.size(); i++)
    {
      if(attributes[i].attribute == "face")
      {
        if(attributes[i].value == "FRONT")
        {
          GLSETTINGS_Culling_face = GL_FRONT;
          GLSETTINGS_Material_face = GL_BACK;
        }
        else if(attributes[i].value == "BACK")
        {
          GLSETTINGS_Culling_face = GL_BACK;
          GLSETTINGS_Material_face = GL_FRONT;
        }
        else if(attributes[i].value == "NONE")
        {
          GLSETTINGS_Culling_face = GL_NONE;
          GLSETTINGS_Material_face = GL_FRONT_AND_BACK;
        }
        else
        {
          errorManager->addError("Unknown OpenGL Setting",
            "The provided OpenGL face culling setting \"" + name + "\" is unknown. ( FRONT | BACK | NONE) ", line, column);
        }
      }
      else if(attributes[i].attribute == "front")
      {
        if(attributes[i].value == "CW")
          GLSETTINGS_Culling_front = GL_CW;
        else if(attributes[i].value == "CCW")
          GLSETTINGS_Culling_front = GL_CCW;
        else
        {
          errorManager->addError("Unknown OpenGL Setting",
            "The provided OpenGL culling front setting \"" + name + "\" is unknown. ( CW | CCW ) ", line, column);
        }
      }
    }
  }
  else if(name == "Hints")
  {
    for(unsigned int i=0; i< attributes.size(); i++)
    {
      if(attributes[i].attribute == "display")
      {
        if(attributes[i].value == "ALL")
          GLHelper::getGLH()->setMaxMessageType(TextObject::ALL_HINTS);
        else if(attributes[i].value == "COMMON")
          GLHelper::getGLH()->setMaxMessageType(TextObject::COMMON_HINTS);
        else if(attributes[i].value == "WARNINGS")
          GLHelper::getGLH()->setMaxMessageType(TextObject::WARNING_HINTS);
        else if(attributes[i].value == "NONE")
          GLHelper::getGLH()->setMaxMessageType(TextObject::NO_HINTS);
      }
    }
  }
  else if(name == "DisableWGLShareLists")
  {
    GLSETTINGS_Disable_WGL_Share_Lists = GL_TRUE;
  }
  else if(name == "PolygonOffset")
  {
    for(unsigned int i=0; i< attributes.size(); i++)
    {
      if(attributes[i].attribute == "factor")
        GLSETTINGS_PolygonOffset_factor = (GLfloat)ParserUtilities::toDouble(attributes[i].value);
      else if(attributes[i].attribute == "units")
        GLSETTINGS_PolygonOffset_units = (GLfloat)ParserUtilities::toDouble(attributes[i].value);
    }
  }
  else if(name == "CubeMapGeneration")
  {
    for(unsigned int i=0; i< attributes.size(); i++)
    {
      if(attributes[i].attribute == "posx")
        GLSETTINGS_CubeMap_CamPos.v[0] = ParserUtilities::toDouble(attributes[i].value);
      else if(attributes[i].attribute == "posy")
        GLSETTINGS_CubeMap_CamPos.v[1] = ParserUtilities::toDouble(attributes[i].value);
      else if(attributes[i].attribute == "posz")
        GLSETTINGS_CubeMap_CamPos.v[2] = ParserUtilities::toDouble(attributes[i].value);
      else if(attributes[i].attribute == "size")
      {
        GLSETTINGS_CubeMap_Size = ParserUtilities::toInt(attributes[i].value);
      }
      else if(attributes[i].attribute == "clipNear")
      {
        GLSETTINGS_CubeMap_ClipNear = ParserUtilities::toDouble(attributes[i].value);
      }
      else if(attributes[i].attribute == "clipFar")
      {
        GLSETTINGS_CubeMap_ClipFar = ParserUtilities::toDouble(attributes[i].value);
      }
      else if(attributes[i].attribute == "update")
      {
        GLSETTINGS_CubeMap_UpdateRate = ParserUtilities::toInt(attributes[i].value);
      }
    }
  }
  else if(name == "ShaderDef")
  {
    std::string name, file;
    GLenum type;
    for(unsigned int i=0; i< attributes.size(); i++)
    {
      if(attributes[i].attribute == "name")
        name = attributes[i].value;
      else if(attributes[i].attribute == "file")
        file = attributes[i].value;
      else if(attributes[i].attribute == "type")
      {
         if(attributes[i].value == "VERTEX")
           type = GL_VERTEX_SHADER;
         else if(attributes[i].value == "FRAGMENT")
           type = GL_FRAGMENT_SHADER;
         else if(attributes[i].value == "GEOMETRY")
           type = GL_GEOMETRY_SHADER_EXT;
      }
    }
    shader->createShader(name, type, file);
  }
  else if(name == "FilterDef")
  {
    std::string extensionName;
    for(unsigned int i=0; i< attributes.size(); i++)
    {
      if(attributes[i].attribute == "name")
        lastProgramName = attributes[i].value;
      else if(attributes[i].attribute == "extension")
        extensionName = attributes[i].value;
    }
    shader->createProgram(lastProgramName, extensionName, true);
  }
  else if(name == "Program")
  {
    std::string extensionName;
    for(unsigned int i=0; i< attributes.size(); i++)
    {
      if(attributes[i].attribute == "name")
        lastProgramName = attributes[i].value;
      else if(attributes[i].attribute == "extension")
        extensionName = attributes[i].value;
    }
    shader->createProgram(lastProgramName, extensionName);
  }
  else if(name == "Shader")
  {
    for(unsigned int i=0; i< attributes.size(); i++)
    {
      if(attributes[i].attribute == "ref")
        shader->attachShader(lastProgramName, attributes[i].value);
    }
  }
  else if(name == "Filter")
  {
    for(unsigned int i=0; i< attributes.size(); i++)
    {
      if(attributes[i].attribute == "ref")
        shader->attachFilter(lastProgramName, attributes[i].value);
    }
  }
  else if(name == "Uniform")
  {
    std::string uniName, swapName, type, file, filter;
    bool ignore = false;
    for(unsigned int i=0; i< attributes.size(); i++)
    {
      if(attributes[i].attribute == "ref")
        uniName = attributes[i].value;
      else if(attributes[i].attribute == "swap")
        swapName = attributes[i].value;
      else if(attributes[i].attribute == "type")
        type = attributes[i].value;
      else if(attributes[i].attribute == "file")
        file = attributes[i].value;
      else if(attributes[i].attribute == "ignore")
        ignore = ParserUtilities::toBool(attributes[i].value);
      else if(attributes[i].attribute == "filter")
        filter = attributes[i].value;
    }
    shader->addUniform(lastProgramName, uniName, swapName, type, file, ignore, filter);
  }
  else if(name == "MRT")
  {
    int bitmask = 0, clearbitmask = 0;
    for(unsigned int i=0; i< attributes.size(); i++)
    {
      if(attributes[i].attribute == "bitmask")
        bitmask = ParserUtilities::toInt(attributes[i].value);
      else if(attributes[i].attribute == "clearbitmask")
        clearbitmask = ParserUtilities::toInt(attributes[i].value);
    }
    shader->setMRT(lastProgramName, bitmask, clearbitmask);
  }
  else if(name == "ShaderInterface")
  {
    for(unsigned int i=0; i< attributes.size(); i++)
    {
      if(attributes[i].attribute == "startOn")
        GLSETTINGS_Shader_InitASAP = ParserUtilities::toBool(attributes[i].value);
      else if(attributes[i].attribute == "showInfos" &&
        ParserUtilities::toBool(attributes[i].value))
        shader->toggleShowInfos();
      else if(attributes[i].attribute == "initialShader")
        shader->setGlobalShader(attributes[i].value);
      else if(attributes[i].attribute == "initialPostShader" &&
        attributes[i].value != "none")
        shader->setPostShader(attributes[i].value);
    }
  }
  else
  {
    errorManager->addError("Unknown OpenGL Setting",
      "The provided OpenGL setting \"" + name + "\" is unknown.", line, column);
  }
}

void GraphicsManager::initContext(bool hasSharedDisplayLists)
{
  if(!glewInitialized)
  {
    glewInitialized = true;
    glewError = glewInit();
    if(glewError != GLEW_OK)
    {
      /* Problem: glewInit failed, something is seriously wrong. */
      glewErrorString = (char*)glewGetErrorString(glewError);
    }
  }

  if(glewError != GLEW_OK)
  {
    GLHelper::getGLH()->glPrintText(TextObject::WARNING_HINTS, TextObject::WARNING, 5000, "Warning: Glew could not initialized: %s", glewErrorString.c_str());
  }

  if(!hasSharedDisplayLists)
  {
    // do the init job
    displayListsHaveBeenCompiled = false;
    init();

    // print standard grafics information
    GLHelper* glhelper = GLHelper::getGLH();
    char* glInfo = (char*)glGetString(GL_VERSION);
    glhelper->glPrintText(TextObject::ALL_HINTS, TextObject::WHITE, 5000, "Using OpenGL Version: %s", glInfo);
    glInfo = (char*)glGetString(GL_VENDOR);
    glhelper->glPrintText(TextObject::ALL_HINTS, TextObject::WHITE, 5200, "Company of OpenGL implementation: %s", glInfo);
    glInfo = (char*)glGetString(GL_RENDERER);
    glhelper->glPrintText(TextObject::ALL_HINTS, TextObject::WHITE, 5400, "Identifier of Renderer: %s", glInfo);

    // check multisampling capabilities
    if(GLEW_VERSION_1_3)
    {
      int max_samples;
      glGetIntegerv(GL_SAMPLES, &max_samples);
      if(max_samples < GLSETTINGS_Antialiasing_Samples)
      {
        glhelper->glPrintText(TextObject::WARNING_HINTS, TextObject::WARNING, 7500, "WARNING: You have selected %i antialiasing samples!", GLSETTINGS_Antialiasing_Samples);
        glhelper->glPrintText(TextObject::WARNING_HINTS, TextObject::WARNING, 7500, "WARNING: But your hardware supports only %i samples!", max_samples);
      }
      // if framebuffer objects are available -> check framebuffer samples too
      if(GLEW_VERSION_2_0)
      {
        max_samples = 0;
        glGetIntegerv(GL_MAX_SAMPLES_EXT, &max_samples);
        if(max_samples < GLSETTINGS_Antialiasing_Samples)
        {
          glhelper->glPrintText(TextObject::WARNING_HINTS, TextObject::WARNING, 7500, "WARNING: You have selected %i antialiasing samples!", GLSETTINGS_Antialiasing_Samples);
          glhelper->glPrintText(TextObject::WARNING_HINTS, TextObject::WARNING, 7500, "WARNING: But your hardware supports only %i samples for use in framebuffer objects!", max_samples);
        }
      }
    }

    // check right shadow configuration and if not so display a help message
    if(GLSETTINGS_Culling_face == GL_NONE && shadowMappingIsUsed())
      glhelper->glPrintText(TextObject::WARNING_HINTS, TextObject::WARNING, 7500, "WARNING: You have to use culling to avoid selfshadowing artifacts.");
    return;
  }
  else
  {
    GLHelper::getGLH()->glPrintText(TextObject::COMMON_HINTS, TextObject::LIGHTGREY, 2500, "Context has shared DisplayLists.");
    init();
    return;
  }
}

void GraphicsManager::init()
{
  // enable global states which will not be modified later on
  glClearDepth(1.0f);
  glDepthFunc(GL_LEQUAL);
  glEnable(GL_DEPTH_TEST);

  glClearAccum(0.0, 0.0, 0.0, 0.0);

  glEnable(GL_COLOR_MATERIAL);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  if(GLSETTINGS_Culling_face != GL_NONE)
  {
    glEnable(GL_CULL_FACE);
    glCullFace(GLSETTINGS_Culling_face);
    glFrontFace(GLSETTINGS_Culling_front);
  }
  else
    glDisable(GL_CULL_FACE);

  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbientLight);
  glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GLSETTINGS_LightModel_twoSided);

  // enable environment
  if(environment)
  {
    if(environment->initEnvironment())
    {
      if(reflectionObject && !GLEW_ARB_texture_cube_map)
        reflectionObject = 0;
    }
  }
  else if(reflectionObject)
  {
    GLHelper::getGLH()->glPrintText(TextObject::WARNING_HINTS, TextObject::WARNING, 7500, "Warning: There is no environment present. Dynamic environment mapping is disabled");
    reflectionObject = 0;
  }

  // enable lights
  enableLights();

  // check if display lists have been compiled
  checkDisplayListRequirements();

   // reset texture units (User may have closed the window and left changed states behind)
  if(GLEW_VERSION_1_3)
  {
    int max_units;
    glGetIntegerv(GL_MAX_TEXTURE_UNITS, &max_units);
    for(int i = 0; i < max_units && max_units > 0; i++)
    {
      glActiveTexture(GL_TEXTURE0 + i);
      glDisable(GL_TEXTURE_2D);
    }
    glActiveTexture(GL_TEXTURE0);
  }
  glDisable(GL_TEXTURE_2D);

  // init shader interface if forced
  if(GLSETTINGS_Shader_InitASAP)
  {
    shader->setRunning(true);
    GLSETTINGS_Shader_InitASAP = GL_FALSE;
  }
}

void GraphicsManager::checkDisplayListRequirements()
{
  if(!displayListsHaveBeenCompiled)
  {
    // State changes display lists
    compileStateChangeDisplayLists();
    // Create font display lists
    GLHelper::getGLH()->CreateBaseFont();
    // Primitive display lists
    std::vector<GraphicsPrimitiveDescription>::iterator graIter;
    for(graIter = primitives.begin(); graIter != primitives.end(); ++graIter)
    {
      graIter->compileDisplayList();
    }
    // create environment dl
    if(environment)
      environment->createDL();
    displayListsHaveBeenCompiled = true;
  }
}

void GraphicsManager::compileStateChangeDisplayLists()
{
  // Display List to enable WIREFRAME mode
  enableStateDL[VisualizationParameterSet::WIREFRAME] = glGenLists(1);
  glNewList(enableStateDL[VisualizationParameterSet::WIREFRAME], GL_COMPILE);
  enableWireframeMode();
  glEndList();

  // Display List to disable WIREFRAME mode
  disableStateDL[VisualizationParameterSet::WIREFRAME] = glGenLists(1);
  glNewList(disableStateDL[VisualizationParameterSet::WIREFRAME], GL_COMPILE);
  disableWireframeMode();
  glEndList();

  // Display List to enable FLAT_SHADING mode
  enableStateDL[VisualizationParameterSet::FLAT_SHADING] = glGenLists(1);
  glNewList(enableStateDL[VisualizationParameterSet::FLAT_SHADING], GL_COMPILE);
  enableFlatShadingMode();
  glEndList();

  // Display List to disable FLAT_SHADING mode
  disableStateDL[VisualizationParameterSet::FLAT_SHADING] = glGenLists(1);
  glNewList(disableStateDL[VisualizationParameterSet::FLAT_SHADING], GL_COMPILE);
  disableFlatShadingMode();
  glEndList();

  // Display List to enable SMOOTH_SHADING mode
  enableStateDL[VisualizationParameterSet::SMOOTH_SHADING] = glGenLists(1);
  glNewList(enableStateDL[VisualizationParameterSet::SMOOTH_SHADING], GL_COMPILE);
  enableSmoothShadingMode();
  glEndList();

  // Display List to disable SMOOTH_SHADING mode
  disableStateDL[VisualizationParameterSet::SMOOTH_SHADING] = glGenLists(1);
  glNewList(disableStateDL[VisualizationParameterSet::SMOOTH_SHADING], GL_COMPILE);
  disableSmoothShadingMode();
  glEndList();

  // Display List to enable TEXTURE_SHADING mode
  enableStateDL[VisualizationParameterSet::TEXTURE_SHADING] = glGenLists(1);
  glNewList(enableStateDL[VisualizationParameterSet::TEXTURE_SHADING], GL_COMPILE);
  enableTextureShadingMode();
  glEndList();

  // Display List to disable TEXTURE_SHADING mode
  disableStateDL[VisualizationParameterSet::TEXTURE_SHADING] = glGenLists(1);
  glNewList(disableStateDL[VisualizationParameterSet::TEXTURE_SHADING], GL_COMPILE);
  disableTextureShadingMode();
  glEndList();

  // Display List to enable TEXTURE_SHADING mode
  enableStateDL[VisualizationParameterSet::CAMERA_SIMULATION] = glGenLists(1);
  glNewList(enableStateDL[VisualizationParameterSet::CAMERA_SIMULATION], GL_COMPILE);
  enableCameraSimulationMode();
  glEndList();

  // Display List to disable TEXTURE_SHADING mode
  disableStateDL[VisualizationParameterSet::CAMERA_SIMULATION] = glGenLists(1);
  glNewList(disableStateDL[VisualizationParameterSet::CAMERA_SIMULATION], GL_COMPILE);
  disableCameraSimulationMode();
  glEndList();

  // Display List to enable SHADOW_MAPPING mode
  enableStateDL[VisualizationParameterSet::SHADOW_MAPPING] = glGenLists(1);
  glNewList(enableStateDL[VisualizationParameterSet::SHADOW_MAPPING], GL_COMPILE);
  enableShadowMappingMode();
  glEndList();

  // Display List to disable SHADOW_MAPPING mode
  disableStateDL[VisualizationParameterSet::SHADOW_MAPPING] = glGenLists(1);
  glNewList(disableStateDL[VisualizationParameterSet::SHADOW_MAPPING], GL_COMPILE);
  disableShadowMappingMode();
  glEndList();

  // Display Lists for PHYSICS_WIREFRAM (derived from WIREFRAME)
  enableStateDL[VisualizationParameterSet::PHYSICS_WIREFRAME]
  = enableStateDL[VisualizationParameterSet::WIREFRAME];
  disableStateDL[VisualizationParameterSet::PHYSICS_WIREFRAME]
  = disableStateDL[VisualizationParameterSet::WIREFRAME];

  // Display Lists for PHYSICS_FLAT (derived from FLAT_SHADING)
  enableStateDL[VisualizationParameterSet::PHYSICS_FLAT]
  = enableStateDL[VisualizationParameterSet::FLAT_SHADING];
  disableStateDL[VisualizationParameterSet::PHYSICS_FLAT]
  = disableStateDL[VisualizationParameterSet::FLAT_SHADING];

  // Display Lists for PHYSICS_SMOOTH (derived from SMOOTH_SHADING)
  enableStateDL[VisualizationParameterSet::PHYSICS_SMOOTH]
  = enableStateDL[VisualizationParameterSet::SMOOTH_SHADING];
  disableStateDL[VisualizationParameterSet::PHYSICS_SMOOTH]
  = disableStateDL[VisualizationParameterSet::SMOOTH_SHADING];

  // Display Lists for MIXED_PHYSICS (derived from SMOOTH_SHADING)
  enableStateDL[VisualizationParameterSet::MIXED_PHYSICS]
  = enableStateDL[VisualizationParameterSet::SMOOTH_SHADING];
  disableStateDL[VisualizationParameterSet::MIXED_PHYSICS]
  = disableStateDL[VisualizationParameterSet::SMOOTH_SHADING];

  // Display Lists for MIXED_APPEARANCE (derived from SMOOTH_SHADING)
  enableStateDL[VisualizationParameterSet::MIXED_APPEARANCE]
  = enableStateDL[VisualizationParameterSet::SMOOTH_SHADING];
  disableStateDL[VisualizationParameterSet::MIXED_APPEARANCE]
  = disableStateDL[VisualizationParameterSet::SMOOTH_SHADING];
}

void GraphicsManager::enableWireframeMode()
{
  glLineWidth(1.0);
  glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
  glShadeModel(GL_FLAT);
}

void GraphicsManager::disableWireframeMode()
{
  glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
}

void GraphicsManager::enableFlatShadingMode()
{
  glShadeModel(GL_FLAT);
}

void GraphicsManager::disableFlatShadingMode()
{
}

void GraphicsManager::enableSmoothShadingMode()
{
  glShadeModel(GL_SMOOTH);
  glEnable(GL_POLYGON_SMOOTH);
  glEnable(GL_LIGHTING);
  if(GLSETTINGS_LightModel_localObjectViewer)
    glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
}

void GraphicsManager::disableSmoothShadingMode()
{
  glDisable(GL_POLYGON_SMOOTH);
  glDisable(GL_LIGHTING);
  if(GLSETTINGS_LightModel_localObjectViewer)
    glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_FALSE);
}

void GraphicsManager::enableTextureShadingMode()
{
  glShadeModel(GL_SMOOTH);
  glEnable(GL_POLYGON_SMOOTH);
  glEnable(GL_LINE_SMOOTH);
  glEnable(GL_LIGHTING);
  if(GLSETTINGS_LightModel_localObjectViewer)
    glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
}

void GraphicsManager::disableTextureShadingMode()
{
  glDisable(GL_POLYGON_SMOOTH);
  glDisable(GL_LINE_SMOOTH);
  glDisable(GL_LIGHTING);
  if(GLSETTINGS_LightModel_localObjectViewer)
    glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_FALSE);
}

void GraphicsManager::enableCameraSimulationMode()
{
  // prepare scene: draw environment
  glShadeModel(GL_SMOOTH);
  glEnable(GL_POLYGON_SMOOTH);
  glEnable(GL_LINE_SMOOTH);
  glEnable(GL_LIGHTING);
  if(!GLSETTINGS_LightModel_localCameraViewer)
    glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_FALSE);
}

void GraphicsManager::disableCameraSimulationMode()
{
  glDisable(GL_POLYGON_SMOOTH);
  glDisable(GL_LINE_SMOOTH);
  glDisable(GL_LIGHTING);
  if(!GLSETTINGS_LightModel_localCameraViewer)
    glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
}

void GraphicsManager::enableShadowMappingMode()
{
  //Draw back faces into the shadow map
  if(GLSETTINGS_Culling_face == GL_NONE)
    glEnable(GL_CULL_FACE);
  if(GLSETTINGS_Culling_face != GL_BACK)
    glCullFace(GL_BACK);
  else
    glCullFace(GL_FRONT);

  if(GLSETTINGS_PolygonOffset_factor != 0.0f &&
    GLSETTINGS_PolygonOffset_units != 0.0f)
  {
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(GLSETTINGS_PolygonOffset_factor, GLSETTINGS_PolygonOffset_units);
  }

  glGetIntegerv(GL_VIEWPORT, viewport);
  glShadeModel(GL_FLAT);
  glColorMask(0, 0, 0, 0);
}

void GraphicsManager::disableShadowMappingMode()
{
  if(GLSETTINGS_Culling_face == GL_NONE)
    glDisable(GL_CULL_FACE);
  else
    glCullFace(GLSETTINGS_Culling_face);

  if(GLSETTINGS_PolygonOffset_factor != 0.0f &&
    GLSETTINGS_PolygonOffset_units != 0.0f)
    glDisable(GL_POLYGON_OFFSET_FILL);

  glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
  glShadeModel(GL_SMOOTH);
  glColorMask(1, 1, 1, 1);
}

unsigned char GraphicsManager::preEnableLight(const std::string& name, const std::string& objName, SimObject* simObject)
{
  Light* light = getLightByName(name);
  if(!light)
    return 1;
  light->lightID = getUnusedLightID();
  if(light->lightID == GL_NONE)
    return 2;
  if(simObject)
  {
    if(!objName.empty() && simObject->getName() != objName)
    {
      SimObject* obj;
      simObject->findObject(obj, objName, false);
      if(obj)
      {
        light->setObjectBinding(obj);
        return 0;
      }
    }
    light->setObjectBinding(simObject);
  }
  return 0;
}

unsigned char GraphicsManager::preEnableLight(const std::string& name)
{
  return preEnableLight(name, "", 0);
}

void GraphicsManager::reenableLights()
{
  for(std::vector<Light*>::iterator iter = lights.begin(); iter != lights.end(); ++iter)
  {
    if((*iter)->lightID != GL_NONE)
    {
      glEnable((*iter)->lightID);
    }
  }
}

void GraphicsManager::dimmLights()
{
  for(std::vector<Light*>::iterator iter = lights.begin(); iter != lights.end(); ++iter)
  {
    if((*iter)->lightID != GL_NONE)
    {
      glLightfv((*iter)->lightID, GL_AMBIENT, (*iter)->ambientColor);
      glLightfv((*iter)->lightID, GL_DIFFUSE, nocolor);
      glLightfv((*iter)->lightID, GL_SPECULAR, black);
    }
  }
}

void GraphicsManager::undimmLights()
{
  for(std::vector<Light*>::iterator iter = lights.begin(); iter != lights.end(); ++iter)
  {
    if((*iter)->lightID != GL_NONE)
    {
      glLightfv((*iter)->lightID, GL_AMBIENT, (*iter)->ambientColor);
      glLightfv((*iter)->lightID, GL_DIFFUSE, (*iter)->diffuseColor);
      glLightfv((*iter)->lightID, GL_SPECULAR, (*iter)->specularColor);
    }
  }
}

void GraphicsManager::enableLights()
{
  for(std::vector<Light*>::iterator iter = lights.begin(); iter != lights.end(); ++iter)
  {
    if((*iter)->lightID != GL_NONE)
    {
      glLightfv((*iter)->lightID, GL_AMBIENT, (*iter)->ambientColor);
      glLightfv((*iter)->lightID, GL_DIFFUSE, (*iter)->diffuseColor);
      glLightfv((*iter)->lightID, GL_SPECULAR, (*iter)->specularColor);
      glLightfv((*iter)->lightID, GL_POSITION, (*iter)->position);
      glLightfv((*iter)->lightID, GL_CONSTANT_ATTENUATION, (*iter)->getConstantAttenuation());
      glLightfv((*iter)->lightID, GL_LINEAR_ATTENUATION, (*iter)->getLinearAttenuation());
      glLightfv((*iter)->lightID, GL_QUADRATIC_ATTENUATION, (*iter)->getQuadraticAttenuation());
      glLightfv((*iter)->lightID, GL_SPOT_CUTOFF, (*iter)->getSpotCutoff());
      glLightfv((*iter)->lightID, GL_SPOT_DIRECTION, (*iter)->spotDirection);
      glLightfv((*iter)->lightID, GL_SPOT_EXPONENT, (*iter)->getSpotExponent());
      glEnable((*iter)->lightID);
      // if shadow map has to be build: build one
      if(!(*iter)->createShadowMap())
      {
        if(!GLEW_ARB_multitexture || !GLEW_ARB_texture_env_combine)
        {
          GLHelper::getGLH()->glPrintText(TextObject::WARNING_HINTS, TextObject::WARNING, 10000, "ERROR: Software Shadow Mapping not possible! GL_ARB_multitexture and GL_ARB_texture_env_combine are not supported.");
          int max_units;
          glGetIntegerv(GL_MAX_TEXTURE_UNITS, &max_units);
          GLHelper::getGLH()->glPrintText(TextObject::WARNING_HINTS, TextObject::WARNING, 10000, "ERROR: GL_MAX_TEXTURE_UNITS: %i", max_units);
        }
        if(!GLEW_ARB_depth_texture || !GLEW_ARB_shadow)
          GLHelper::getGLH()->glPrintText(TextObject::WARNING_HINTS, TextObject::WARNING, 10000, "ERROR: Hardware Shadow Mapping not possible! GL_ARB_depth_texture and GL_ARB_shadow are not supported.");
      }
    }
  }
  // create shadow map iterator
  shadowMapIter = lights.begin();
  firstShadowRendering = true;
}

void GraphicsManager::disableLight(const std::string& name)
{
  Light* light = getLightByName(name);
  glDisable(light->lightID);
  light->lightID = GL_NONE;
}

void GraphicsManager::disableAllLights()
{
  for(std::vector<Light*>::iterator iter = lights.begin(); iter != lights.end(); ++iter)
  {
    if((*iter)->lightID != GL_NONE)
    {
      glDisable((*iter)->lightID);
      (*iter)->lightID = GL_NONE;
    }
  }
}

void GraphicsManager::defineLight(Light* light)
{
  lights.push_back(light);
}

Light* GraphicsManager::getLightByName(const std::string& name)
{
  Light* light = NULL;
  for(std::vector<Light*>::iterator iter = lights.begin(); iter != lights.end(); ++iter)
  {
    if(strcmp(name.c_str(), (*iter)->name.c_str()) == 0)
      light = *iter;
  }
  return light;
}

GLenum GraphicsManager::getUnusedLightID()
{
  bool ids[8] = { false, false, false, false,
    false, false, false, false };

  for(std::vector<Light*>::iterator iter = lights.begin(); iter != lights.end(); ++iter)
  {
    switch((*iter)->lightID)
    {
    case GL_LIGHT0: ids[0] = true; break;
    case GL_LIGHT1: ids[1] = true; break;
    case GL_LIGHT2: ids[2] = true; break;
    case GL_LIGHT3: ids[3] = true; break;
    case GL_LIGHT4: ids[4] = true; break;
    case GL_LIGHT5: ids[5] = true; break;
    case GL_LIGHT6: ids[6] = true; break;
    case GL_LIGHT7: ids[7] = true; break;
    default:
      break;
    }
  }
  for(int i=0; i<8; ++i)
  {
    if(!ids[i])
     return GL_LIGHT0+i;
  }
  // do not enable light!
  return GL_NONE;
}

int GraphicsManager::getNumLightsOn(bool onlyShadowMapped) const
{
  int i(0);
  if(onlyShadowMapped)
  {
    for(std::vector<Light*>::const_iterator iter = lights.begin(); iter != lights.end(); ++iter)
    {
      if((*iter)->lightID != GL_NONE && (*iter)->shadowMapID != GL_NONE)
        i++;
    }
  }
  else
  {
    for(std::vector<Light*>::const_iterator iter = lights.begin(); iter != lights.end(); ++iter)
    {
      if((*iter)->lightID != GL_NONE)
        i++;
    }
  }
  return i;
}

std::string GraphicsManager::getNamesOfLights() const
{
  std::string names;
  for(std::vector<Light*>::const_iterator iter = lights.begin();;)
  {
    names += (*iter)->name;
    iter++;
    if (iter == lights.end())
      return names;
    names += ", ";
  }
  return names;
}

void GraphicsManager::updateLights(bool useShadows, bool dimmPass, bool shader)
{
  std::vector<Light*>::iterator iter;
  for(iter = lights.begin(); iter != lights.end(); ++iter)
  {
    if ((*iter)->lightID != GL_NONE)
    {
      (*iter)->updateLightPosition();
      if(showLightCones)
        (*iter)->drawLightCones(q);
      if((*iter)->shadowMapID != GL_NONE && useShadows)
      {
        if((*iter)->hardwareShadowing)
        {
          if(GLEW_VERSION_1_3)
            glActiveTexture(GL_TEXTURE0 + ++currentTextureUnitOffset);
          glBindTexture(GL_TEXTURE_2D, (*iter)->shadowMapID);
          (*iter)->prepareShadowTexture(shader);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

          glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

          if(!shader)
          {
            glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, (*iter)->shadowMapIntensity);
            glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
            glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_INTERPOLATE);
            glTexEnvf(GL_TEXTURE_ENV, GL_SRC0_RGB, GL_TEXTURE);
            glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
            glTexEnvf(GL_TEXTURE_ENV, GL_SRC1_RGB, GL_PREVIOUS);
            glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);
            glTexEnvf(GL_TEXTURE_ENV, GL_SRC2_RGB, GL_CONSTANT);
            glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND2_RGB, GL_SRC_ALPHA);

            glEnable(GL_TEXTURE_GEN_S);
            glEnable(GL_TEXTURE_GEN_T);
            glEnable(GL_TEXTURE_GEN_R);
            glEnable(GL_TEXTURE_GEN_Q);
            glEnable(GL_TEXTURE_2D);
          }

          // switch back for normal texture mapping
          if(GLEW_VERSION_1_3)
            glActiveTexture(GL_TEXTURE0);
        }
        else
        {
          if(dimmPass)
            continue;
          // first shadow texture unit
          if(GLEW_VERSION_1_3)
            glActiveTexture(GL_TEXTURE0 + ++currentTextureUnitOffset);
          glEnable(GL_TEXTURE_2D);
          glBindTexture(GL_TEXTURE_2D, (*iter)->shadowMapID);
          (*iter)->prepare8BitShadowTexture();
          glEnable(GL_TEXTURE_GEN_S);
          glEnable(GL_TEXTURE_GEN_T);
          glEnable(GL_TEXTURE_GEN_Q);

          glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
          glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_REPLACE);
          glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA, GL_TEXTURE);
          glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_ALPHA, GL_SRC_ALPHA);
          glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_REPLACE);
          glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_REPLACE);
          glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_PRIMARY_COLOR);
          glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
          glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_REPLACE);

          // second shadow texture unit
          if(GLEW_VERSION_1_3)
            glActiveTexture(GL_TEXTURE0 + ++currentTextureUnitOffset);
          glEnable(GL_TEXTURE_1D);
          glBindTexture(GL_TEXTURE_1D, (*iter)->depthRampMapID);
          (*iter)->prepare1DProjection();
          glEnable(GL_TEXTURE_GEN_S);
          glEnable(GL_TEXTURE_GEN_Q);

          glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
          glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA, GL_PREVIOUS);
          glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_ALPHA, GL_SRC_ALPHA);
          glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_ADD_SIGNED);
          glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_ALPHA, GL_TEXTURE);
          glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
          glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_PREVIOUS);
          glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
          glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_REPLACE);

          //Use alpha test to reject shadowed fragments
          glAlphaFunc(GL_GEQUAL, 0.5f);
          glEnable(GL_ALPHA_TEST);
        }
      }
    }
  }
}

void GraphicsManager::revertLightTextureUnits(bool shader)
{
  while(currentTextureUnitOffset > 0)
  {
    if(GLEW_VERSION_1_3)
      glActiveTexture(GL_TEXTURE0 + currentTextureUnitOffset--);
    if(!shader)
    {
      glDisable(GL_TEXTURE_GEN_S);
      glDisable(GL_TEXTURE_GEN_T);
      glDisable(GL_TEXTURE_GEN_R);
      glDisable(GL_TEXTURE_GEN_Q);
      glDisable(GL_TEXTURE_2D);
    }
    else
    {
      glMatrixMode(GL_TEXTURE);
      glLoadIdentity();
      glMatrixMode(GL_MODELVIEW);
    }
  }
  glDisable(GL_ALPHA_TEST);
}

void GraphicsManager::camToLightPos(Vector3d& cameraPos, Vector3d& cameraTarget) const
{
  int currentLight(0), targetLight(0), foundCamAtLight(0);
  Vector3d lightPosition;
  std::vector<Light*>::const_iterator iter;
  for(iter = lights.begin(); iter != lights.end(); ++iter)
  {
    if ((*iter)->lightID != GL_NONE)
    {
      currentLight++;
      (*iter)->getPositionVector(lightPosition);
      if(cameraPos == lightPosition)
      {
        foundCamAtLight = currentLight;
      }
      if(targetLight == 0)
      {
        targetLight = currentLight;
      }
      if(currentLight > foundCamAtLight && foundCamAtLight != 0)
      {
        targetLight = currentLight;
        break;
      }
    }
  }
  currentLight = 0;
  for(iter = lights.begin(); iter != lights.end(); ++iter)
  {
    if ((*iter)->lightID != GL_NONE)
    {
      currentLight++;
      if(targetLight == currentLight)
      {
        (*iter)->getPositionVector(cameraPos);
        (*iter)->getTargetVector(cameraTarget);
        return;
      }
    }
  }
}

unsigned int GraphicsManager::getCurrentShadowMapSize()
{
  return (*shadowMapIter)->shadowMapSize;
}

bool GraphicsManager::prepareLightForShadowMapping(VisualizationParameterSet& visParams)
{
  if(firstShadowRendering)
  {
    // prepare render settings
    glCallList(enableStateDL[VisualizationParameterSet::SHADOW_MAPPING]);
    copyTexImage = false;
  }
  if(copyTexImage)
  {
    if((*shadowMapIter)->hardwareShadowing)
    {
      glBindTexture(GL_TEXTURE_2D, (*shadowMapIter)->shadowMapID);
      glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 0, 0,
                      (*shadowMapIter)->shadowMapSize, (*shadowMapIter)->shadowMapSize, 0);
      visParams.renderPasses = 1;
    }
    else
    {
      glReadPixels(0, 0, (*shadowMapIter)->shadowMapSize, (*shadowMapIter)->shadowMapSize,
                   GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, (*shadowMapIter)->shadowMapData8Bit);
      glBindTexture(GL_TEXTURE_2D, (*shadowMapIter)->shadowMapID);
      glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, (*shadowMapIter)->shadowMapSize,
        (*shadowMapIter)->shadowMapSize, GL_ALPHA, GL_UNSIGNED_BYTE, (*shadowMapIter)->shadowMapData8Bit);
      visParams.renderPasses = 2;
    }
    copyTexImage = false;
    ++shadowMapIter;
  }
  for(firstShadowRendering = false; lights.size() > 0 && shadowMapIter != lights.end(); ++shadowMapIter)
  {
    if((*shadowMapIter)->shadowMapID != GL_NONE)
    {
      // prepare light
      glViewport(0, 0, (*shadowMapIter)->shadowMapSize,
                       (*shadowMapIter)->shadowMapSize);
      glClear(GL_DEPTH_BUFFER_BIT);

      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      gluPerspective((*shadowMapIter)->spotCutoff*1.5, 1,
        (*shadowMapIter)->shadowMapNearClippingPlane,
        (*shadowMapIter)->shadowMapFarClippingPlane);
      glGetFloatv(GL_PROJECTION_MATRIX, (*shadowMapIter)->light_projection_matrix);
      
      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();
      if((*shadowMapIter)->updateBoundObjectTransformation())
        gluLookAt((*shadowMapIter)->boundPosition[0], 
                  (*shadowMapIter)->boundPosition[1],
                  (*shadowMapIter)->boundPosition[2],
                  (*shadowMapIter)->boundPosition[0] + (*shadowMapIter)->boundSpotDirection[0],
                  (*shadowMapIter)->boundPosition[1] + (*shadowMapIter)->boundSpotDirection[1],
                  (*shadowMapIter)->boundPosition[2] + (*shadowMapIter)->boundSpotDirection[2],
                  0, 0, 1);
      else
        gluLookAt((*shadowMapIter)->position[0], 
                  (*shadowMapIter)->position[1],
                  (*shadowMapIter)->position[2],
                  (*shadowMapIter)->position[0] + (*shadowMapIter)->spotDirection[0],
                  (*shadowMapIter)->position[1] + (*shadowMapIter)->spotDirection[1],
                  (*shadowMapIter)->position[2] + (*shadowMapIter)->spotDirection[2],
                  0, 0, 1);
      glGetFloatv(GL_MODELVIEW_MATRIX, (*shadowMapIter)->light_modelview_matrix);
      copyTexImage = true;
      return true;
    }
  }
  // reset iter for next run through
  shadowMapIter = lights.begin();
  // undo render settings

  glCallList(disableStateDL[VisualizationParameterSet::SHADOW_MAPPING]);
  firstShadowRendering = true;
  return false;
}

void GraphicsManager::resetShadowMapIter()
{
  shadowMapIter = lights.begin();
  firstShadowRendering = true;
  copyTexImage = false;
}

bool GraphicsManager::shadowMappingIsUsed()
{
  std::vector<Light*>::iterator iter;
  for(iter = lights.begin(); iter != lights.end(); ++iter)
  {
    if((*iter)->lightID != GL_NONE&&
       (*iter)->shadowMapID != GL_NONE)
       return true;
  }
  return false;
}

Light* GraphicsManager::getLightByID(GLenum lightID)
{
  std::vector<Light*>::iterator iter;
  for(iter = lights.begin(); iter != lights.end(); ++iter)
  {
    if ((*iter)->lightID == lightID)
    {
      return *iter;
    }
  }
  return NULL;
}

void GraphicsManager::getCurrentImageDimensions(int& height, int& width) const
{
  height = currentHeight;
  width = currentWidth;
}

double GraphicsManager::distanceToFarthestPoint(const std::list<GraphicsPrimitiveHandle>& handles)
{
  std::list<GraphicsPrimitiveHandle>::const_iterator handle(handles.begin());
  double maxQuadDist(0.0);
  while(handle != handles.end())
  {
    unsigned int currentHandle(static_cast<unsigned int>(*(handle++)));
    double* vertices = primitives[currentHandle].vertices;
    int numOfPoints(primitives[currentHandle].numberOfVertices);
    int maxIndex(3*numOfPoints);
    for(int i=0; i<maxIndex;)
    {
      Vector3d p;
      p.v[0] = vertices[i++]; p.v[1] = vertices[i++]; p.v[2] = vertices[i++];
      double quadDist(p.getQuadLength());
      if(quadDist > maxQuadDist)
        maxQuadDist = quadDist;
    }
  }
  return sqrt(maxQuadDist);
}

bool GraphicsManager::intersectSetOfPrimitivesWithRay(const Vector3d& rayPosition, const Vector3d& ray,
                                                      const Vector3d& objPosition, const Matrix3d& objRotation,
                                                      const std::list<GraphicsPrimitiveHandle>& handles, Vector3d& intersection)
{
  Vector3d transPos, transRay, nearestIntersection;
  bool intersectionFound(false);
  double quadDistToNearestIntersection(DBL_MAX);
  SimGeometry::transformRayToObjectAtOrigin(rayPosition, ray, objPosition, objRotation, transPos, transRay);
  std::list<GraphicsPrimitiveHandle>::const_iterator handle(handles.begin());
  while(handle != handles.end())
  {
    Vector3d pos;
    unsigned int index(static_cast<unsigned int>(*(handle++)));
    if(SimGeometry::intersectRayAndGraphicsPrimitive(transPos, transRay, primitives[index], pos))
    {
      intersectionFound = true;
      double currentQuadDist((rayPosition-pos).getQuadLength());
      if(currentQuadDist < quadDistToNearestIntersection)
      {
        quadDistToNearestIntersection = currentQuadDist;
        nearestIntersection = pos;
      }
    }
  }
  intersection = nearestIntersection;
  intersection.rotate(objRotation);
  intersection += objPosition;
  return intersectionFound;
}


void GraphicsManager::setActiveVertexList(const std::string& listName)
{
  currentVertexList = vertexLists[listName];
}


void GraphicsManager::newVertexList(const std::string& name, unsigned char numTextureCoordinates)
{
  VertexList* newVertexList = new VertexList(numTextureCoordinates);
  vertexLists[name] = newVertexList;
  setActiveVertexList(name);
}



// ******************************
// Funcions for adding new primitives:


GraphicsPrimitiveHandle GraphicsManager::createNewBox(const Vector3d corners[])
{
  GraphicsPrimitiveHandle returnValue = static_cast<GraphicsPrimitiveHandle>(primitives.size());
  currentGraphicsPrimitive.type = GRAPHICS_BOX;
  currentGraphicsPrimitive.numberOfVertices = 8;
  currentGraphicsPrimitive.vertices = new double[24];
  int index(0);
  // Store vertex data (just needed for later conversion to display list)
  for(int i=0; i<8; i++)
  {
    currentGraphicsPrimitive.vertices[index++] = corners[i].v[0];
    currentGraphicsPrimitive.vertices[index++] = corners[i].v[1];
    currentGraphicsPrimitive.vertices[index++] = corners[i].v[2];
  }
  primitives.push_back(currentGraphicsPrimitive);
  return returnValue;
}


GraphicsPrimitiveHandle GraphicsManager::createNewSphere(double radius)
{
  GraphicsPrimitiveHandle returnValue = static_cast<GraphicsPrimitiveHandle>(primitives.size());
  currentGraphicsPrimitive.type = GRAPHICS_SPHERE;
  currentGraphicsPrimitive.numberOfVertices = 1;
  currentGraphicsPrimitive.vertices = new double[1];
  currentGraphicsPrimitive.vertices[0] = radius;
  primitives.push_back(currentGraphicsPrimitive);
  return returnValue;
}


GraphicsPrimitiveHandle GraphicsManager::createNewCylinder(double radius, double height, bool capped)
{
  GraphicsPrimitiveHandle returnValue = static_cast<GraphicsPrimitiveHandle>(primitives.size());
  currentGraphicsPrimitive.type = capped ? GRAPHICS_CAPPED_CYLINDER : GRAPHICS_CYLINDER;
  currentGraphicsPrimitive.numberOfVertices = 2;
  currentGraphicsPrimitive.vertices = new double[2];
  currentGraphicsPrimitive.vertices[0] = radius;
  currentGraphicsPrimitive.vertices[1] = height;
  primitives.push_back(currentGraphicsPrimitive);
  return returnValue;
}

void GraphicsManager::createNewQuadPrePrimitive(const AttributeSet& attributes, bool flipSurfaceNormal)
{
  currentPrePrimitive.type = QUAD;
  if(!flipSurfaceNormal)
  {
    currentPrePrimitive.vertexNames.push_back(ParserUtilities::getValueFor(attributes, "v1"));
    currentPrePrimitive.vertexNames.push_back(ParserUtilities::getValueFor(attributes, "v2"));
    currentPrePrimitive.vertexNames.push_back(ParserUtilities::getValueFor(attributes, "v3"));
    currentPrePrimitive.vertexNames.push_back(ParserUtilities::getValueFor(attributes, "v4"));
  }
  else
  {
    currentPrePrimitive.vertexNames.push_back(ParserUtilities::getValueFor(attributes, "v4"));
    currentPrePrimitive.vertexNames.push_back(ParserUtilities::getValueFor(attributes, "v3"));
    currentPrePrimitive.vertexNames.push_back(ParserUtilities::getValueFor(attributes, "v2"));
    currentPrePrimitive.vertexNames.push_back(ParserUtilities::getValueFor(attributes, "v1"));
  }
  currentPrePrimitiveHasBeenFinished();
}

void GraphicsManager::createNewTrianglePrePrimitive(const AttributeSet& attributes, bool flipSurfaceNormal)
{
  currentPrePrimitive.type = TRIANGLE;
  if(!flipSurfaceNormal)
  {
    currentPrePrimitive.vertexNames.push_back(ParserUtilities::getValueFor(attributes, "v1"));
    currentPrePrimitive.vertexNames.push_back(ParserUtilities::getValueFor(attributes, "v2"));
    currentPrePrimitive.vertexNames.push_back(ParserUtilities::getValueFor(attributes, "v3"));
  }
  else
  {
    currentPrePrimitive.vertexNames.push_back(ParserUtilities::getValueFor(attributes, "v3"));
    currentPrePrimitive.vertexNames.push_back(ParserUtilities::getValueFor(attributes, "v2"));
    currentPrePrimitive.vertexNames.push_back(ParserUtilities::getValueFor(attributes, "v1"));
  }
  currentPrePrimitiveHasBeenFinished();
}

void GraphicsManager::createNewPrePrimitive(PrimitiveType type_)
{
  currentPrePrimitive.type = type_;
}

void GraphicsManager::addVertexToCurrentPrePrimitive(const std::string& vertexName)
{
  currentPrePrimitive.vertexNames.push_back(vertexName);
}

void GraphicsManager::currentPrePrimitiveHasBeenFinished()
{
  Vertex firstVertex, secondVertex, thirdVertex;
  Vector3d surfaceNormal;

  if(currentPrePrimitive.vertexNames.size() == 3)
    currentPrePrimitive.type = TRIANGLE;
  else if(currentPrePrimitive.vertexNames.size() == 4)
    currentPrePrimitive.type = QUAD;

  std::vector<std::string>::const_iterator iter,
    firstVertex_ptr, secondVertex_ptr, thirdVertex_ptr;
  for(iter = currentPrePrimitive.vertexNames.begin();
    iter != currentPrePrimitive.vertexNames.end(); iter++)
  {
    if(iter == currentPrePrimitive.vertexNames.begin())
    {
      if(!currentVertexList->getVertex(*iter, firstVertex))
        GLHelper::getGLH()->glPrintText(TextObject::WARNING_HINTS, TextObject::WARNING, 5000, "Could not resolve reference of vertex \"%s\"", iter->c_str());
      firstVertex_ptr = iter;
      iter++;
      if(!currentVertexList->getVertex(*iter, secondVertex))
        GLHelper::getGLH()->glPrintText(TextObject::WARNING_HINTS, TextObject::WARNING, 5000, "Could not resolve reference of vertex \"%s\"", iter->c_str());
      secondVertex_ptr = iter;
      iter++;
      if(!currentVertexList->getVertex(*iter, thirdVertex))
        GLHelper::getGLH()->glPrintText(TextObject::WARNING_HINTS, TextObject::WARNING, 5000, "Could not resolve reference of vertex \"%s\"", iter->c_str());
      thirdVertex_ptr = iter;
    }
    else
    {
      firstVertex = secondVertex;
      firstVertex_ptr = secondVertex_ptr;
      secondVertex = thirdVertex;
      secondVertex_ptr = thirdVertex_ptr;
      currentVertexList->getVertex(*iter, thirdVertex);
      thirdVertex_ptr = iter;
    }
    SimGeometry::normalVectorOfPlane(firstVertex.pos, secondVertex.pos, thirdVertex.pos, surfaceNormal);
    surfaceNormal.normalize();
    if(currentPrePrimitive.surfaceNormals.empty())
    {
      currentPrePrimitive.surfaceNormals.push_back(surfaceNormal);
      currentPrePrimitive.surfaceNormals.push_back(surfaceNormal);
      currentPrePrimitive.surfaceNormals.push_back(surfaceNormal);
    }
    else
      currentPrePrimitive.surfaceNormals.push_back(surfaceNormal);
    currentVertexList->addSurfaceNormal(*firstVertex_ptr, surfaceNormal);
    currentVertexList->addSurfaceNormal(*secondVertex_ptr, surfaceNormal);
    currentVertexList->addSurfaceNormal(*thirdVertex_ptr, surfaceNormal);
  }
  preprimitives.push_back(currentPrePrimitive);
  currentPrePrimitive.vertexNames.clear();
  currentPrePrimitive.surfaceNormals.clear();
}

void GraphicsManager::flipVertexNormals()
{
  currentVertexList->flipVertexNormals();
}

void GraphicsManager::calculateVertexNormals()
{
  currentVertexList->calculateVertexNormals();
}

void GraphicsManager::clearVertexNormals()
{
  currentVertexList->clearVertexNormals();
}

void GraphicsManager::combinePrePrimitives()
{
  PreGraphicsPrimitiveDescription currentQuadsPrePrimitives,
                                  currentTrianglesPrePrimitives;
  currentQuadsPrePrimitives.type = QUAD;
  currentTrianglesPrePrimitives.type = TRIANGLE;
  std::vector<PreGraphicsPrimitiveDescription> oldpreprimitives;
  oldpreprimitives = preprimitives;
  preprimitives.clear();

  std::vector<PreGraphicsPrimitiveDescription>::iterator iter;
  for(iter = oldpreprimitives.begin(); iter != oldpreprimitives.end(); iter++)
  {
    if(iter->type == QUAD)
    {
      for(unsigned int i=0; i < iter->vertexNames.size(); i++)
      {
        currentQuadsPrePrimitives.surfaceNormals.push_back(iter->surfaceNormals[i]);
        currentQuadsPrePrimitives.vertexNames.push_back(iter->vertexNames[i]);
      }
    }
    else if(iter->type == TRIANGLE)
    {
      for(unsigned int i=0; i < iter->vertexNames.size(); i++)
      {
        currentTrianglesPrePrimitives.surfaceNormals.push_back(iter->surfaceNormals[i]);
        currentTrianglesPrePrimitives.vertexNames.push_back(iter->vertexNames[i]);
      }
    }
    else
    {
      preprimitives.push_back(*iter);
    }
  }

  if(currentQuadsPrePrimitives.vertexNames.size() > 0)
    preprimitives.push_back(currentQuadsPrePrimitives);
  if(currentTrianglesPrePrimitives.vertexNames.size() > 0)
    preprimitives.push_back(currentTrianglesPrePrimitives);
}

void GraphicsManager::flipVertexOrder()
{
  std::vector<PreGraphicsPrimitiveDescription>::iterator iter;
  for(iter = preprimitives.begin(); iter != preprimitives.end(); iter++)
  {
    std::vector<std::string> newVertNames;
    newVertNames.resize(iter->vertexNames.size());
    unsigned int reverseCounter = iter->vertexNames.size() -1;
    for(unsigned int i = 0; i < iter->vertexNames.size(); i++, reverseCounter--)
      newVertNames[i] = iter->vertexNames[reverseCounter];
    iter->vertexNames.clear();
    iter->vertexNames = newVertNames;
  }
}

GraphicsPrimitiveHandle GraphicsManager::convertPrePrimitive(bool averageNormals)
{
  GraphicsPrimitiveHandle gHandle = static_cast<GraphicsPrimitiveHandle>(primitives.size());
  std::vector<PreGraphicsPrimitiveDescription>::iterator iter = preprimitives.begin();

  currentGraphicsPrimitive.type = iter->type;
  currentGraphicsPrimitive.numberOfVertices = iter->vertexNames.size();
  currentGraphicsPrimitive.vertices = new double[currentGraphicsPrimitive.numberOfVertices*3];
  currentGraphicsPrimitive.vertexNormals  = new double[currentGraphicsPrimitive.numberOfVertices*3];
  switch(currentVertexList->getNumTextureCoordinates())
  {
  case 2:
    currentGraphicsPrimitive.textureCoordinates  = new double[currentGraphicsPrimitive.numberOfVertices*2];
    currentGraphicsPrimitive.numTextureCoordinates = 2;
    break;
  case 1:
    currentGraphicsPrimitive.textureCoordinates  = new double[currentGraphicsPrimitive.numberOfVertices];
    currentGraphicsPrimitive.numTextureCoordinates = 1;
    break;
  default:
    currentGraphicsPrimitive.numTextureCoordinates = 0;
    break;
  }
  currentGraphicsPrimitive.normals = new double[currentGraphicsPrimitive.numberOfVertices*3];
  int index(0);

  // Convert vertex data:
  Vertex newVertex;
  Vector3d newVertexNormal, newSurfaceNormal;
  std::vector<std::string>::const_iterator vertexName = iter->vertexNames.begin();
  for(unsigned int numOfVertexName = 0; vertexName != iter->vertexNames.end(); ++vertexName, ++numOfVertexName)
  {
    currentVertexList->getVertex(*vertexName, newVertex);
    currentVertexList->getVertexNormal(*vertexName, newVertexNormal);

    currentGraphicsPrimitive.vertices[index]  = newVertex.pos.v[0];
    if(currentGraphicsPrimitive.numTextureCoordinates > 0)
      currentGraphicsPrimitive.textureCoordinates[index -
      index/3*(3-currentGraphicsPrimitive.numTextureCoordinates)] = newVertex.tex.s;
    currentGraphicsPrimitive.vertexNormals[index] =
      averageNormals ? newVertexNormal.v[0] : iter->surfaceNormals[numOfVertexName].v[0];
    currentGraphicsPrimitive.normals[index] = iter->surfaceNormals[numOfVertexName].v[0];
    index++;

    currentGraphicsPrimitive.vertices[index]  = newVertex.pos.v[1];
    if(currentGraphicsPrimitive.numTextureCoordinates > 0)
      currentGraphicsPrimitive.textureCoordinates[index -
      index/3*(3-currentGraphicsPrimitive.numTextureCoordinates)] = newVertex.tex.t;
    currentGraphicsPrimitive.vertexNormals[index] = 
      averageNormals ? newVertexNormal.v[1] : iter->surfaceNormals[numOfVertexName].v[1];
    currentGraphicsPrimitive.normals[index] = iter->surfaceNormals[numOfVertexName].v[1];
    index++;

    currentGraphicsPrimitive.vertices[index]  = newVertex.pos.v[2];
    currentGraphicsPrimitive.vertexNormals[index] =
      averageNormals ? newVertexNormal.v[2] : iter->surfaceNormals[numOfVertexName].v[2];
    currentGraphicsPrimitive.normals[index] = iter->surfaceNormals[numOfVertexName].v[2];
    index++;
  }

  primitives.push_back(currentGraphicsPrimitive);
  currentGraphicsPrimitive.vertices = 0;
  currentGraphicsPrimitive.vertexNormals = 0;
  currentGraphicsPrimitive.textureCoordinates = 0;
  currentGraphicsPrimitive.numTextureCoordinates = 0;
  currentGraphicsPrimitive.normals = 0;
  preprimitives.erase(preprimitives.begin());

  return gHandle;
}

// ******************************
// Drawing functions:


void GraphicsManager::drawPrimitive(GraphicsPrimitiveHandle handle, 
                                    const Vector3d& position, const Matrix3d& rotation, 
                                    Surface& surface, bool inverted, bool drawForSensor,
                                    const VisualizationParameterSet& visParams)
{
  unsigned int index(static_cast<unsigned int>(handle));
  prepareObjectDrawing(position, rotation, surface, inverted, drawForSensor,
    primitives[index].numTextureCoordinates == 0, visParams);
  primitives[index].callDisplayList();
  finishObjectDrawing(surface, primitives[index].numTextureCoordinates == 0);
}


void GraphicsManager::drawPrimitives(const std::list<GraphicsPrimitiveHandle>& handles, 
                                     const Vector3d& position, const Matrix3d& rotation, 
                                     Surface& surface, bool inverted, bool drawForSensor,
                                     const VisualizationParameterSet& visParams)
{
  std::list<GraphicsPrimitiveHandle>::const_iterator handle(handles.begin());
  if(handle != handles.end())
  {
    unsigned int index(static_cast<unsigned int>(*handle));
    prepareObjectDrawing(position, rotation, surface, inverted, drawForSensor,
      primitives[index].numTextureCoordinates == 0, visParams);
  }
  else
  {
    return;
  }
  while(handle != handles.end())
  {
    unsigned int index(static_cast<unsigned int>(*handle));
    primitives[index].callDisplayList();
    ++handle;
  }
  unsigned int index(static_cast<unsigned int>(*(--handle)));
  finishObjectDrawing(surface, primitives[index].numTextureCoordinates == 0);
}

void GraphicsManager::quickDrawPrimitive(GraphicsPrimitiveHandle handle, 
                                         const Vector3d& position, const Matrix3d& rotation, 
                                         Surface& surface)
{
  unsigned int index(static_cast<unsigned int>(handle));
  prepareQuickObjectDrawing(position, rotation, surface);
  primitives[index].callDisplayList();
  finishQuickObjectDrawing();
}


void GraphicsManager::quickDrawPrimitives(const std::list<GraphicsPrimitiveHandle>& handles, 
                                          const Vector3d& position, const Matrix3d& rotation, 
                                          Surface& surface)
{
  std::list<GraphicsPrimitiveHandle>::const_iterator handle(handles.begin());
  if(handle != handles.end())
  {
    prepareQuickObjectDrawing(position, rotation, surface);
  }
  else
  {
    return;
  }
  while(handle != handles.end())
  {
    unsigned int index(static_cast<unsigned int>(*handle));
    primitives[index].callDisplayList();
    ++handle;
  }
  finishQuickObjectDrawing();
}

void GraphicsManager::prepareObjectDrawing(const Vector3d& position, const Matrix3d& rotation, 
                                           Surface& surface, bool inverted, bool drawForSensor,
                                           bool generateTexCoordinates,
                                           const VisualizationParameterSet& visParams)
{
  // Set color (material properties)
  if(surface.hasAmbientColor)
  {
    glColorMaterial(GLSETTINGS_Material_face, GL_DIFFUSE);
    if(GLSETTINGS_UseBaseColors)
      glMaterialfv(GLSETTINGS_Material_face, GL_AMBIENT, surface.getBaseAmbientColor());
    else
      glMaterialfv(GLSETTINGS_Material_face, GL_AMBIENT, surface.ambientColor);
  }
  else
    glColorMaterial(GLSETTINGS_Material_face, GL_AMBIENT_AND_DIFFUSE);

  if(inverted && !drawForSensor)
    glColor4fv(surface.invcolor);
  else if(GLSETTINGS_UseBaseColors)
    glColor4fv(surface.getBaseColor());
  else
    glColor4fv(surface.color);

  if(surface.color[3] < 1.0)
  {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  }

  glMaterialfv(GLSETTINGS_Material_face, GL_SPECULAR, surface.specularColor);
  glMaterialfv(GLSETTINGS_Material_face, GL_SHININESS, surface.getShininess());
  glMaterialfv(GLSETTINGS_Material_face, GL_EMISSION, surface.emissionColor);

  // Set Texture
  if(texturingIsEnabled)
  {
    // if texture is bound but tex coordinates have to be generated
    if(surface.diffuseTexture.BindTexture())
    {
      if(generateTexCoordinates)
      {
        if(surface.aEyeLinearModeExists)
        {
          glPushMatrix();
          glLoadIdentity();

          glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, surface.texGen_s_mode);
          glTexGenfv(GL_S, GL_OBJECT_PLANE, surface.texGen_s_coeff);
          glTexGenfv(GL_S, GL_EYE_PLANE, surface.texGen_s_coeff);

          glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, surface.texGen_t_mode);
          glTexGenfv(GL_T, GL_OBJECT_PLANE, surface.texGen_t_coeff);
          glTexGenfv(GL_T, GL_EYE_PLANE, surface.texGen_t_coeff);

          glPopMatrix();
        }
        else
        {
          glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
          glTexGenfv(GL_S, GL_OBJECT_PLANE, surface.texGen_s_coeff);

          glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
          glTexGenfv(GL_T, GL_OBJECT_PLANE, surface.texGen_t_coeff);
        }

        glEnable(GL_TEXTURE_GEN_S);
        glEnable(GL_TEXTURE_GEN_T);
      }
      if(surface.diffuseTexture.HasAlpha())
      {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      }
    }
    // environment mapping
    if(surface.reflectivity > 0.0f && environment)
    {
      environment->enableEvironmentMapping(surface.reflectivityColor,
        currentTextureUnitOffset);
    }
  }
  //Go to position and rotation:
  // cycle order is negative if the physics are disabled
  glPushMatrix();
  if((visParams.mb_mode == MOTIONBLUR_6PASSES ||
    visParams.mb_mode == MOTIONBLUR_12PASSES ||
    visParams.mb_mode == MOTIONBLUR_24PASSES ||
    visParams.mb_mode == MOTIONBLUR_ACCUMULATION) &&
    cycle_order_offset >= 0 && visParams.mb_renderPasses > 1)
  {
    float pos_offset = cycle_order_offset -
      ((float)(visParams.mb_currentPass-1.0f)/(visParams.mb_renderPasses-1.0f))*exposure2Physic;
    GLHelper::getGLH()->setInterpolatedMatrix( previous_positions,
                                               previous_rotations,
                                               pos_offset);
  }
  else if(visParams.mb_mode == MOTIONBLUR_VELOCITY_BUFFER ||
    visParams.mb_mode == MOTIONBLUR_VELOCITY_BUFFER_2 ||
    visParams.mb_mode == MOTIONBLUR_VELOCITY_BUFFER_2_GEO)
  {
    // handle movement of camera
    if(cam_cycle_order_offset >=0)
    {
      glLoadIdentity();
      // slightly different cam pos update (in order to ignore gluLookAt re-implementation)
      int pos_offset = cam_cycle_order_offset - (int)exposure2Physic;
      if(pos_offset < 0)
        pos_offset = 12 + pos_offset;
      Vector3d forwardVec(1.0,0.0,0.0);
      Vector3d upVec(0.0,0.0,1.0);
      forwardVec.rotate(*(cam_previous_rotations + pos_offset));
      upVec.rotate(*(cam_previous_rotations + pos_offset));
      gluLookAt((cam_previous_positions + pos_offset)->v[0],
                (cam_previous_positions + pos_offset)->v[1],
                (cam_previous_positions + pos_offset)->v[2],
                (cam_previous_positions + pos_offset)->v[0] + forwardVec.v[0],
                (cam_previous_positions + pos_offset)->v[1] + forwardVec.v[1], 
                (cam_previous_positions + pos_offset)->v[2] + forwardVec.v[2],
                    upVec.v[0], upVec.v[1], upVec.v[2]);
    }
    // handle movement of objects
    if(cycle_order_offset >= 0)
    {
      float pos_offset = cycle_order_offset - exposure2Physic;
      GLHelper::getGLH()->setInterpolatedMatrix( previous_positions,
                                                 previous_rotations,
                                                 pos_offset);
    }
    else
      GLHelper::getGLH()->setMatrix(position, rotation);

    setupPreviousMVPM();
    // previous model view proj is same as current of static scene
    if(cycle_order_offset >=0 || cam_cycle_order_offset >=0)
    {
      glPopMatrix();
      glPushMatrix();
      GLHelper::getGLH()->setMatrix(position, rotation);
    }
  }
  else
    GLHelper::getGLH()->setMatrix(position, rotation);
}


inline void GraphicsManager::finishObjectDrawing(Surface& surface, bool generateTexCoordinates)
{
  if(texturingIsEnabled)
  {
    // unset environment mapping
    if(surface.reflectivity > 0.0f && environment)
      environment->disableEvironmentMapping(currentTextureUnitOffset);
    // unset texture generation
    surface.diffuseTexture.UnBindTexture(generateTexCoordinates);
    if(surface.diffuseTexture.HasAlpha())
      glDisable(GL_BLEND);
  }
  
  if(surface.color[3] < 1.0f)
    glDisable(GL_BLEND);

  //Restore old position and rotation
  glPopMatrix();
}

inline void GraphicsManager::prepareQuickObjectDrawing(const Vector3d& position, const Matrix3d& rotation, 
                                                       Surface& surface)
{
  //Go to position and rotation:
  glPushMatrix();
  GLHelper::getGLH()->setMatrix(position, rotation);
}

inline void GraphicsManager::finishQuickObjectDrawing()
{
  //Restore old position and rotation
  glPopMatrix();
}

void GraphicsManager::renderEnvironment(double farClippingPlaneDistance)
{
  if(environment)
  {
    if(farClippingPlaneDistance < ecSize*1.5)
    {
      GLHelper* glhelper = GLHelper::getGLH();
      glhelper->glPrintText(TextObject::WARNING_HINTS, TextObject::WARNING, 5000, "WARNING: A cameras far clipping plane is set to %3.2f.", farClippingPlaneDistance);
      glhelper->glPrintText(TextObject::WARNING_HINTS, TextObject::RED, 5000, "WARNING: Parts of the Environment Cube will not be visible.");
      glhelper->glPrintText(TextObject::WARNING_HINTS, TextObject::RED, 5000, "WARNING: Increase the distance to at least %3.2f.", ecSize*1.5);
    }
    environment->render();
  }
}

const GraphicsPrimitiveDescription* GraphicsManager::getGraphicsPrimitive(GraphicsPrimitiveHandle handle) const
{
  const unsigned int index(static_cast<unsigned int>(handle));
  const GraphicsPrimitiveDescription* primitive = &(primitives[index]);
  return primitive;
}

void GraphicsManager::saveScreenshot(unsigned int width, unsigned int height,
                                     std::string& name, bool generateFilename)
{
  FILE * file;

  char filename[64];

  //first calculate the filename to save to
  if(generateFilename)
  {
    for(int i=lastScreenshotID; i<10000; i++)
    {
      sprintf(filename, "%s%04d.tga", name.substr(0, name.size()-4).c_str(), i);
      
      //try opening this file - if not possible, use this filename
      file=fopen(filename, "rb");

      if(!file)
      {
        break;
      }
      
      //otherwise, the file exists, try next, except if this is the last one
      fclose(file);

      if(i==999)
      {
        GLHelper::getGLH()->glPrintText(TextObject::WARNING_HINTS, TextObject::WARNING, 5000, "Unable to generate a screenshot identifier.");
        return;
      }
    }
  }
  else
    sprintf(filename, "%s.tga", name.c_str());

  GLHelper::getGLH()->glPrintText(TextObject::COMMON_HINTS, TextObject::WHITE, 5000, "%s \"%s\".", "Saving image to", filename);
  
  GLubyte   TGAheader[12]={0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0}; //Uncompressed TGA header
  GLubyte   infoHeader[6];

  unsigned char * data=new unsigned char[4*width*height];
  if(!data)
  {
    GLHelper::getGLH()->glPrintText(TextObject::WARNING_HINTS, TextObject::WARNING, 5000, "Unable to allocate memory for screen data");
    return;
  }

  //read in the screen data
  glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);

  //data needs to be in BGR format
  //swap b and r
  for(unsigned int i=0; i<width*height*4; i+=4)
  { 
    //repeated XOR to swap bytes 0 and 2
    data[i] ^= data[i+2] ^= data[i] ^= data[i+2];
  }
  
  //open the file
  file = fopen(filename, "wb");

  //save header
  VERIFY(fwrite(TGAheader, 1, sizeof(TGAheader), file) > 0);

  //save info header
  infoHeader[0]=(width & 0x00FF);
  infoHeader[1]=(width & 0xFF00) >> 8;
  infoHeader[2]=(height & 0x00FF);
  infoHeader[3]=(height & 0xFF00) >> 8;
  infoHeader[4]=32;
  infoHeader[5]=0;

  //save info header
  VERIFY(fwrite(infoHeader, 1, sizeof(infoHeader), file) > 0);

  //save the image data
  VERIFY(fwrite(data, 1, width*height*4, file) > 0);
  
  fclose(file);

  //free data
  delete [] data;
  
  GLHelper::getGLH()->glPrintText(TextObject::COMMON_HINTS, TextObject::LIGHTGREEN, 5000, "%s \"%s\".", "Image saved to", filename);
  return;
}

void GraphicsManager::accFrustrum(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top,
                                  GLdouble zNear, GLdouble zFar, GLdouble pixdx, GLdouble pixdy,
                                  GLdouble eyedx, GLdouble eyedy, GLdouble focus)
{
  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);

  GLdouble xwsize = right - left;
  GLdouble ywsize = top - bottom;
  GLdouble dx = -(pixdx*xwsize/(GLdouble)viewport[2] + eyedx*zNear/focus);
  GLdouble dy = -(pixdy*ywsize/(GLdouble)viewport[3] + eyedy*zNear/focus);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glFrustum(left + dx, right + dx, bottom + dy, top + dy, zNear, zFar);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslated(-eyedx, -eyedy, 0.0);
}

void GraphicsManager::accPerspective(GLdouble zNear, GLdouble zFar,
                                     GLdouble pixdx, GLdouble pixdy, GLdouble eyedx, GLdouble eyedy,
                                     GLdouble focus)
{
  GLdouble fov2 = ((currentFOV*M_PI) / 180.0) / 2.0;

  GLdouble top = zNear / (cos(fov2) / sin(fov2));
  GLdouble bottom = -top;
  GLdouble right = top * currentAspect;
  GLdouble left = -right;

  accFrustrum(left, right, bottom, top, zNear, zFar, pixdx, pixdy, eyedx, eyedy, focus);
}

void GraphicsManager::setPerspective()
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(currentFOV, currentAspect, currentNearClip, currentFarClip);
  glViewport(0, 0, currentWidth, currentHeight);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void GraphicsManager::setCubeMapPerspective()
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(90.0, 1.0, GLSETTINGS_CubeMap_ClipNear, GLSETTINGS_CubeMap_ClipFar);
  glViewport(0, 0, GLSETTINGS_CubeMap_Size, GLSETTINGS_CubeMap_Size);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void GraphicsManager::setCubeMapCamera(const int generateCubeMap, const Vector3d& pos)
{
  Vector3d newCamPos = (pos != Vector3d()) ? pos : GLSETTINGS_CubeMap_CamPos,
    newCamUpVector, newCamTarget;
  switch(generateCubeMap)
  {
  case 6:
    newCamTarget = newCamPos + Vector3d(0.0, 0.0, -1.0);
    newCamUpVector = Vector3d(0.0, -1.0, 0.0);
    break;
  case 5:
    newCamTarget = newCamPos + Vector3d(0.0, 0.0, 1.0);
    newCamUpVector = Vector3d(0.0, -1.0, 0.0);
    break;
  case 4:
    newCamTarget = newCamPos + Vector3d(0.0, -1.0, 0.0);
    newCamUpVector = Vector3d(0.0, 0.0, -1.0);
    break;
  case 3:
    newCamTarget = newCamPos + Vector3d(0.0, 1.0, 0.0);
    newCamUpVector = Vector3d(0.0, 0.0, 1.0);
    break;
  case 2:
    newCamTarget = newCamPos + Vector3d(-1.0, 0.0, 0.0);
    newCamUpVector = Vector3d(0.0, -1.0, 0.0);
    break;
  case 1:
    newCamTarget = newCamPos + Vector3d(1.0, 0.0, 0.0);
    newCamUpVector = Vector3d(0.0, -1.0, 0.0);
    break;
  default:
    newCamTarget = Vector3d(0.0, 0.0, 0.0);
    newCamUpVector = Vector3d(0.0, 0.0, 1.0);
    break;
  }
  gluLookAt(newCamPos.v[0], newCamPos.v[1], newCamPos.v[2],
    newCamTarget.v[0], newCamTarget.v[1], newCamTarget.v[2],
    newCamUpVector.v[0], newCamUpVector.v[1], newCamUpVector.v[2]);
}

void GraphicsManager::handleCubeMapping(int cubeMapSide, const std::string& filename)
{
  if(reflectionObject)
  {
    // cubeMapSize: 1=posx, 2=negx, 3=posy, 4=negy, 5=posz, 6=negz
    glCopyTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + (cubeMapSide-1), 0, GL_RGB,
        0, 0, GLSETTINGS_CubeMap_Size, GLSETTINGS_CubeMap_Size, 0);
  }
  else
  {
    std::string name;
    std::string suffix[] = {"_posx", "_negx", "_posy", "_negy", "_posz", "_negz"};
    name = filename.substr(0, filename.size()-4) + suffix[cubeMapSide-1];
    saveScreenshot(GLSETTINGS_CubeMap_Size, GLSETTINGS_CubeMap_Size, name, false);
  }
}

bool GraphicsManager::checkMotionBlurCapability(const MotionBlurMode mode)
{
  switch(mode)
  {
  case MOTIONBLUR_OFF:
    return true;
  case MOTIONBLUR_6PASSES:
  case MOTIONBLUR_12PASSES:
  case MOTIONBLUR_24PASSES:
    if(GLEW_VERSION_1_1)
      return true;
    break;
  case MOTIONBLUR_VELOCITY_BUFFER:
    if(GLEW_VERSION_2_0 &&
      GLEW_EXT_framebuffer_object &&
      shader &&
      shader->getShaderProgram("Post Velocity Buffer") != NULL &&
      shader->getShaderProgram("Velocity Buffer") != NULL)
    {
      GLint maxbuffers;
      glGetIntegerv(GL_MAX_DRAW_BUFFERS, &maxbuffers);
      if(maxbuffers >= 3)
        return true;
    }
    break;
  case MOTIONBLUR_VELOCITY_BUFFER_2:
    if(GLEW_VERSION_2_0 &&
      GLEW_EXT_framebuffer_object &&
      shader &&
      shader->getShaderProgram("Post Velocity Buffer 2") != NULL &&
      shader->getShaderProgram("Velocity Buffer 2") != NULL)
    {
      GLint maxbuffers;
      glGetIntegerv(GL_MAX_DRAW_BUFFERS, &maxbuffers);
      if(maxbuffers >= 3)
        return true;
    }
    break;
  case MOTIONBLUR_VELOCITY_BUFFER_2_GEO:
    if(GLEW_VERSION_2_0 &&
      GLEW_EXT_framebuffer_object &&
      shader &&
      shader->getShaderProgram("Post Velocity Buffer 2") != NULL &&
      shader->getShaderProgram("Velocity Buffer 2 Geo") != NULL)
    {
      GLint maxbuffers;
      glGetIntegerv(GL_MAX_DRAW_BUFFERS, &maxbuffers);
      if(maxbuffers >= 3)
        return true;
    }
    break;
  case MOTIONBLUR_COMBINED_METHOD:
    if(GLEW_VERSION_2_0 &&
      GLEW_EXT_framebuffer_object &&
      shader &&
      shader->getShaderProgram("Post Velocity Buffer Comb") != NULL &&
      shader->getShaderProgram("Velocity Buffer 2") != NULL)
    {
      GLint maxbuffers;
      glGetIntegerv(GL_MAX_DRAW_BUFFERS, &maxbuffers);
      if(maxbuffers >= 3)
        return true;
    }
    break;
  case MOTIONBLUR_COMBINED_METHOD_2:
    if(GLEW_VERSION_2_0 &&
      GLEW_EXT_framebuffer_object &&
      shader &&
      shader->getShaderProgram("Post Velocity Buffer Comb 2") != NULL &&
      shader->getShaderProgram("Velocity Buffer 2") != NULL &&
      shader->getShaderProgram("Post Accumulation Buffer") != NULL &&
      shader->getShaderProgram("Accumulation Buffer") != NULL)
    {
      GLint maxbuffers;
      glGetIntegerv(GL_MAX_DRAW_BUFFERS, &maxbuffers);
      if(maxbuffers >= 4)
        return true;
    }
    break;
  default:
    break;
  }
  return false;
}

void GraphicsManager::setUpMotionBlurSettings(VisualizationParameterSet& visParams)
{
  // standard values
  visParams.mb_currentPass = 1;

  // mode specific values
  switch(visParams.mb_mode)
  {
  case MOTIONBLUR_6PASSES:
    visParams.mb_renderPasses = 6.0f;
    visParams.mb_useAccum = true;
    break;
  case MOTIONBLUR_12PASSES:
    visParams.mb_renderPasses = 12.0f;
    visParams.mb_useAccum = true;
    break;
  case MOTIONBLUR_24PASSES:
    visParams.mb_renderPasses = 24.0f;
    visParams.mb_useAccum = true;
    break;
  case MOTIONBLUR_ACCUMULATION:
    visParams.mb_renderPasses = 12.0f;
    visParams.mb_useAccum = false;
    break;
  default:
    visParams.mb_renderPasses = 1.0f;
    visParams.mb_useAccum = false;
    break;
  }
}

void GraphicsManager::setMBInfos(Vector3d* pp, Matrix3d* pr, const int& coo)
{
  previous_positions = pp;
  previous_rotations = pr;
  cycle_order_offset = coo;
}

void GraphicsManager::setCamMBInfos(Vector3d* cam_pp, Matrix3d* cam_pr, const int& cam_coo)
{
  cam_previous_positions = cam_pp;
  cam_previous_rotations = cam_pr;
  cam_cycle_order_offset = cam_coo;
}

void GraphicsManager::unsetMotionBlurMode(const MotionBlurMode motionBlurMode)
{
  // mode specific values
  switch(motionBlurMode)
  {
  case MOTIONBLUR_6PASSES:
  case MOTIONBLUR_12PASSES:
  case MOTIONBLUR_24PASSES:
     shader->postShaderUsage(true);
     break;
  case MOTIONBLUR_VELOCITY_BUFFER:
  case MOTIONBLUR_VELOCITY_BUFFER_2:
  case MOTIONBLUR_VELOCITY_BUFFER_2_GEO:
  case MOTIONBLUR_COMBINED_METHOD:
  case MOTIONBLUR_COMBINED_METHOD_2:
    shader->setGlobalShader(std::string("none"), true);
    shader->setPostShader(std::string(""), true);
    shader->postShaderUsage(true);
    shader->globalShaderUsage(true);
    break;
  default:
    break;
  }
}

void GraphicsManager::setMotionBlurMode(const MotionBlurMode motionBlurMode)
{
  switch(motionBlurMode)
  {
  case MOTIONBLUR_6PASSES:
  case MOTIONBLUR_12PASSES:
  case MOTIONBLUR_24PASSES:
     shader->postShaderUsage(false);
     break;
  case MOTIONBLUR_VELOCITY_BUFFER:
    if(!shader->getRunning())
      shader->setRunning(true);
    shader->setGlobalShader(std::string("Velocity Buffer"), true);
    shader->setPostShader(std::string("Post Velocity Buffer"), true);
    shader->postShaderUsage(false);
    shader->globalShaderUsage(false);
    break;
  case MOTIONBLUR_VELOCITY_BUFFER_2:
    if(!shader->getRunning())
      shader->setRunning(true);
    shader->setGlobalShader(std::string("Velocity Buffer 2"), true);
    shader->setPostShader(std::string("Post Velocity Buffer 2"), true);
    shader->postShaderUsage(false);
    shader->globalShaderUsage(false);
    break;
  case MOTIONBLUR_VELOCITY_BUFFER_2_GEO:
    if(!shader->getRunning())
      shader->setRunning(true);
    shader->setGlobalShader(std::string("Velocity Buffer 2 Geo"), true);
    shader->setPostShader(std::string("Post Velocity Buffer 2"), true);
    shader->postShaderUsage(false);
    shader->globalShaderUsage(false);
    break;
  case MOTIONBLUR_COMBINED_METHOD:
    if(!shader->getRunning())
      shader->setRunning(true);
    shader->setGlobalShader(std::string("Velocity Buffer 2"), true);
    shader->setPostShader(std::string("Post Velocity Buffer Comb"), true);
    shader->postShaderUsage(false);
    shader->globalShaderUsage(false);
    break;
  case MOTIONBLUR_COMBINED_METHOD_2:
    if(!shader->getRunning())
      shader->setRunning(true);
    shader->postShaderUsage(false);
    shader->globalShaderUsage(false);
    break;
  default:
    break;
  }
}

bool GraphicsManager::getMultisampledFBOsState() const
{
  return shader->getMultisampledFBOsState();
}

bool GraphicsManager::checkMultisampledFBOCapability() const
{
  if(GLEW_EXT_framebuffer_multisample)
    return true;
  return false;
}

void GraphicsManager::toggleMultisampledFBOs()
{
  shader->toggleMultisampledFBOsState(GLSETTINGS_Antialiasing_Samples);
}

void GraphicsManager::setupPreviousMVPM()
{
  GLfloat proj[16];
  GLfloat modl[16];
  GLfloat mvpm[16];

  // Get The Current PROJECTION Matrix From OpenGL
  glGetFloatv( GL_PROJECTION_MATRIX, proj );
  // Get The Current MODELVIEW Matrix From OpenGL
  glGetFloatv( GL_MODELVIEW_MATRIX, modl );

  // Combine The Two Matrices (Multiply Projection By Modelview)
  mvpm[ 0] = modl[ 0] * proj[ 0] + modl[ 1] * proj[ 4] + modl[ 2] * proj[ 8] + modl[ 3] * proj[12];
  mvpm[ 1] = modl[ 0] * proj[ 1] + modl[ 1] * proj[ 5] + modl[ 2] * proj[ 9] + modl[ 3] * proj[13];
  mvpm[ 2] = modl[ 0] * proj[ 2] + modl[ 1] * proj[ 6] + modl[ 2] * proj[10] + modl[ 3] * proj[14];
  mvpm[ 3] = modl[ 0] * proj[ 3] + modl[ 1] * proj[ 7] + modl[ 2] * proj[11] + modl[ 3] * proj[15];
  mvpm[ 4] = modl[ 4] * proj[ 0] + modl[ 5] * proj[ 4] + modl[ 6] * proj[ 8] + modl[ 7] * proj[12];
  mvpm[ 5] = modl[ 4] * proj[ 1] + modl[ 5] * proj[ 5] + modl[ 6] * proj[ 9] + modl[ 7] * proj[13];
  mvpm[ 6] = modl[ 4] * proj[ 2] + modl[ 5] * proj[ 6] + modl[ 6] * proj[10] + modl[ 7] * proj[14];
  mvpm[ 7] = modl[ 4] * proj[ 3] + modl[ 5] * proj[ 7] + modl[ 6] * proj[11] + modl[ 7] * proj[15];
  mvpm[ 8] = modl[ 8] * proj[ 0] + modl[ 9] * proj[ 4] + modl[10] * proj[ 8] + modl[11] * proj[12];
  mvpm[ 9] = modl[ 8] * proj[ 1] + modl[ 9] * proj[ 5] + modl[10] * proj[ 9] + modl[11] * proj[13];
  mvpm[10] = modl[ 8] * proj[ 2] + modl[ 9] * proj[ 6] + modl[10] * proj[10] + modl[11] * proj[14];
  mvpm[11] = modl[ 8] * proj[ 3] + modl[ 9] * proj[ 7] + modl[10] * proj[11] + modl[11] * proj[15];
  mvpm[12] = modl[12] * proj[ 0] + modl[13] * proj[ 4] + modl[14] * proj[ 8] + modl[15] * proj[12];
  mvpm[13] = modl[12] * proj[ 1] + modl[13] * proj[ 5] + modl[14] * proj[ 9] + modl[15] * proj[13];
  mvpm[14] = modl[12] * proj[ 2] + modl[13] * proj[ 6] + modl[14] * proj[10] + modl[15] * proj[14];
  mvpm[15] = modl[12] * proj[ 3] + modl[13] * proj[ 7] + modl[14] * proj[11] + modl[15] * proj[15];

  // load matrix into uniform location
  glUniformMatrix4fv(shader->getPreviousMVPMID(), 1, GL_FALSE, mvpm);
}

void GraphicsManager::setExposure(const double exposureTime, const double simStepLength)
{
  this->exposureTime = (float)exposureTime;
  exposure2Physic = (float)(exposureTime/simStepLength);
  // be aware that the position buffer is limited to 12 older calculations
  if(exposure2Physic > 11.0f)
  {
    if(11.0f < exposure2Physic)
      exposure2Physic = 11.0f;
    GLHelper::getGLH()->glPrintText(TextObject::WARNING_HINTS, TextObject::WARNING, 500,
      "WARNING: too many physic updates per exposure setting! (exposure=%1.3f physic=%1.3f)",
      exposureTime, simStepLength);
  }

}

int GraphicsManager::getAntialiasingSamples()
{
  return GLSETTINGS_Antialiasing_Samples;
}

void GraphicsManager::getShaderUsage(VisualizationParameterSet& visParams)
{
  if(shader->getRunning() &&
    visParams.surfaceStyle == VisualizationParameterSet::CAMERA_SIMULATION)
    visParams.provideUniforms = true;
  else
    visParams.provideUniforms = false;
}

void GraphicsManager::prepareRendering(const VisualizationParameterSet& visParams, GLuint width, GLuint height, GLdouble aspect,
                                       GLdouble fov, GLdouble nearClip, GLdouble farClip, const int fbo_reg)
{
  // prepare the post production and set a global shader (if used)
  if(shader->getRunning() &&
    visParams.surfaceStyle == VisualizationParameterSet::CAMERA_SIMULATION)
    shader->preparePostProduction(width, height, fov, fbo_reg);

  // init the render values
  currentWidth = width;
  currentHeight = height;
  currentFOV = fov;
  currentAspect = aspect;
  currentNearClip = nearClip;
  currentFarClip = farClip;

  // enable texturing
  texturingIsEnabled = (visParams.surfaceStyle == VisualizationParameterSet::CAMERA_SIMULATION ||
                        visParams.surfaceStyle == VisualizationParameterSet::TEXTURE_SHADING);
}

void GraphicsManager::finishRendering(const VisualizationParameterSet& visParams, const bool isObjectWindow)
{
  if(!shader->getRunning() ||
    visParams.surfaceStyle != VisualizationParameterSet::CAMERA_SIMULATION)
    return;

  // draw post surface
  if(visParams.mb_currentPass == visParams.mb_renderPasses &&
    visParams.finishRendering)
  {
    shader->drawPostSurface(isObjectWindow);
    shader->swapPURenderTargets();
  }
  else
  {
    shader->drawPostSurface();
    shader->swapRPRenderTargets();
  }
}
