/**
* @file ShaderProgram.cpp
* 
* Definition of opengl glsl shader program class
*
* @author <A href="mailto:pachur@informatik.uni-bremen.de">Dennis Pachur</A>
*/

#include "ShaderProgram.h"
#include <PhysicalObjects/PhysicalObject.h>
#include <OpenGL/Shader.h>
#include <OpenGL/GLHelper.h>
#include <OpenGL/FramebufferObject.h>
#include <sstream>
#include <fstream>
#include <algorithm>

ShaderProgram::ShaderProgram(const std::string& name_)
: numLines(0), id(0), log(0), build(GL_FALSE), isFilter(GL_FALSE),
  numFboTextures(-1), numActiveFboTextures(-1), numStorageTextures(-1), numTextures(-1)
{
  name = name_;
  RPuniformsSwapped = false;
  PUuniformsSwapped = false;
  MRT_count = 0;
  MRT_clearcount = 0;
  extension = NULL;
}

ShaderProgram::ShaderProgram()
: numLines(0), id(0), log(NULL), secondlog(NULL), build(GL_TRUE), isFilter(GL_FALSE),
  numFboTextures(-1), numActiveFboTextures(-1), numStorageTextures(-1), numTextures(-1)
{
  name = "none";
  RPuniformsSwapped = false;
  PUuniformsSwapped = false;
  MRT_count = 0;
  MRT_clearcount = 0;
  extension = NULL;
}

ShaderProgram::~ShaderProgram()
{
  destroy();
  attachedShader.clear();
  filters.clear();
  uniforms.clear();

  if(MRT_count > 0)
  {
    delete [] bufferArray;
    MRT_index.clear();
  }
  if(MRT_clearcount > 0)
  {
    delete [] clearbufferArray;
    MRT_clearindex.clear();
  }
}

void ShaderProgram::destroy()
{
  if(log)
    delete[] log;

  for(int i=0; i < numLines; i++)
    delete[] description[i];

  if(id != 0 && glIsProgram(id))
    glDeleteProgram(id);

  numLines = 0;
  id = 0;
  log = NULL;
  secondlog = NULL;
  build = GL_FALSE;
  isFilter = GL_FALSE;
  numActiveFboTextures = -1; 
  numFboTextures = -1;
  numTextures = -1;
}

std::string& ShaderProgram::getName()
{
  return name;
}

void ShaderProgram::attachShader(Shader* shader)
{
  attachedShader.push_back(shader);
}

void ShaderProgram::attachFilter(ShaderProgram* filter)
{
  filters.push_back(filter);
}

void ShaderProgram::attachUniform(const Uniform uniform, const Uniform swapTarget,
                                  const UniformType type, GLTexture* tex, const bool& ignore,
                                  const GLenum texFilter)
{
  uniforms.push_back(UniformInfo(uniform, type, swapTarget, tex, ignore, texFilter));
}

int ShaderProgram::create()
{
  if(name == "none")
  {
    build = GL_TRUE;
    return 0;
  }
  else if(build == GL_TRUE)
    return 0;

  // create attached filter
  std::vector<ShaderProgram*>::iterator pIter;
  for(pIter = filters.begin(); pIter != filters.end(); pIter++)
  {
    if((*pIter)->create() > 0)
    {
      secondlog = (*pIter)->log;
      return 2;
    }
  }

  // read description
  std::stringstream filename;
  filename << "Shader/";
  filename << "Descriptions.info";
  std::ifstream infile;
  infile.open(filename.str().c_str() , std::ifstream::in );
  if(infile.is_open())
  {
    GLchar* testline = new GLchar[160];
    GLboolean beginDescription = GL_FALSE;
    numLines = 0;
    while(infile.good())
    {
      infile.getline(testline, 80);
      if(numLines >= 20)
        break;
      else if(!beginDescription && strcmp(name.c_str(), testline) == 0)
        beginDescription = GL_TRUE;
      else if(beginDescription && strcmp(name.c_str(), testline) !=0)
      {
        description[numLines] = new GLchar[80];
        strcpy(description[numLines++], testline);
      }
      else if(strcmp(name.c_str(), testline) == 0)
        break;
    }
    delete[] testline;
    infile.close();
  }
  else
  {
    GLHelper::getGLH()->glPrintText(TextObject::WARNING_HINTS, TextObject::WARNING, 5000,
      "The shader description file \"%s\" is not accessable.", filename.str().c_str());
    description[0] = new GLchar[29];
    strcpy(description[0], "description file is missing\n");
    numLines = 1;
  }

  id = glCreateProgram();

  // attach shader
  GLboolean working = GL_FALSE;
  std::vector<Shader*>::iterator asIter;
  for(asIter = attachedShader.begin(); asIter != attachedShader.end(); asIter++)
  {
    if((*asIter)->usable())
    {
      glAttachShader(id, (*asIter)->getId());
      working = GL_TRUE;
    }
  }

  if(!working)
  {
    GLHelper::getGLH()->glPrintMarkedText(TextObject::WARNING_HINTS, name.c_str(), TextObject::WARNING, 5000,
      "WARNING: No Shader have been attached to Program \"%s\".", name.c_str());
    log = new GLchar[46];
    strcpy(log, "no shader were attached during link process.\n");
    return 1;
  }

  GLint linked;
  glLinkProgram(id);
  glGetProgramiv(id, GL_LINK_STATUS, &linked);
  if(!linked)
  {
    GLint length;
    glGetProgramiv(id, GL_INFO_LOG_LENGTH, &length);
    log = new GLchar[length];
    glGetProgramInfoLog(id, length, &length, log);
    GLHelper::getGLH()->glPrintMarkedText(TextObject::WARNING_HINTS, name.c_str(), TextObject::WARNING, 5000,
      "WARNING: Compilation errors of Program \"%s\" have been occured.", name.c_str());
    return 1;
  }

  glUseProgram(id);

  std::stringstream uniformlogstr;
  int linelog = 0;
  for(UniformInfosIter iter = uniforms.begin();
    iter != uniforms.end(); iter++)
  {
    if(iter->ignore)
      continue;
    iter->glID = glGetUniformLocation(id, (const GLchar*)UniformName[iter->ID].c_str());
    if(iter->glID == -1)
    {
      uniformlogstr << "WARNING: Could not find uniform location of \"";
      uniformlogstr << UniformName[iter->ID];
      uniformlogstr << "\".\n";
      linelog++;
    }
  }
  if(uniformlogstr.str().size() > 0)
  {
    log = new GLchar[uniformlogstr.str().size() + linelog ];
    strcpy(log, uniformlogstr.str().c_str());
    GLHelper::getGLH()->glPrintMarkedText(TextObject::WARNING_HINTS, name.c_str(), TextObject::WARNING, 5000,
      "WARNING: Uniform variable errors of Program \"%s\" have been occured.", name.c_str());
    return 1;
  }
  glUseProgram(0);

  build = GL_TRUE;
  return 0;
}

GLuint ShaderProgram::getId() const
{
  return id;
}

GLboolean ShaderProgram::usable() const
{
  return build;
}

void ShaderProgram::setFilter(const bool filterFlag)
{
  isFilter = filterFlag;
}

std::vector<ShaderProgram*>& ShaderProgram::getFilters()
{
  return filters;
}

void ShaderProgram::printInfo(ProgramInfo info, GLuint posy)
{
  GLHelper* glhelper = GLHelper::getGLH();
  GLuint posx = 260;
  switch(info)
  {
  case PROGRAM_LOG:
    {
      if(!log)
      {
        glColor3f(GLfloat(0.3), GLfloat(1.0), GLfloat(0.3));
        glhelper->glPrintf(false, posx, posy, "no log available");
        return;
      }
      glColor3f(GLfloat(1.0), GLfloat(0.3), GLfloat(0.3));
      std::string logstr(log);
      std::string message;
      for(int loc = logstr.find("\n", 0); loc != int(std::string::npos); )
      {
        message = logstr.substr(0, loc);
        glhelper->glPrintf(false, posx, posy, "%s", message.c_str());
        logstr = logstr.substr(loc+1, logstr.size());
        loc = logstr.find("\n", 0);
        posy -= 20;
      }
    }
    break;
  case PROGRAM_DESCRIPTION:
    glColor3f(GLfloat(0.7), GLfloat(0.7), GLfloat(0.7));
    if(numLines > 0)
    {
      for(int i = 0; i < numLines; i++)
      {
        glhelper->glPrintf(false, posx, posy, "%s", description[i]);
        posy -= 20;
      }
    }
    else
      glhelper->glPrintf(false, posx, posy, "no description available");
    break;
  case PROGRAM_SHADER:
    if(attachedShader.empty())
      glhelper->glPrintf(false, posx, posy, "no shader attached");
    else
    {
      for(std::vector<Shader*>::iterator sIter = attachedShader.begin();
        sIter != attachedShader.end(); sIter++)
      {
        if((*sIter)->usable())
          glColor3f(GLfloat(0.3), GLfloat(1.0), GLfloat(0.3));
        else
          glColor3f(GLfloat(1.0), GLfloat(0.3), GLfloat(0.3));
        glhelper->glPrintf(false, posx, posy, "%s", (*sIter)->getName().c_str());
        posy -= 20;
      }
    }
    break;
  case PROGRAM_UNIFORMS:
    if(uniforms.empty())
      glhelper->glPrintf(false, posx, posy, "no uniforms defined");
    else
    {
      for(UniformInfosIter iter = uniforms.begin();
        iter != uniforms.end(); iter++)
      {
        if(iter->glID != -1 || iter->ignore)
          glColor3f(GLfloat(0.3), GLfloat(1.0), GLfloat(0.3));
        else
          glColor3f(GLfloat(1.0), GLfloat(0.3), GLfloat(0.3));
        glhelper->glPrintf(false, posx, posy, "%s", UniformName[iter->ID].c_str());
        posy -= 20;
      }
    }
    break;
  case PROGRAM_FILTERS:
  {
    // print attached filter infos
    if(filters.empty())
      glhelper->glPrintf(false, posx, posy, "none");
    else
    {
      std::vector<ShaderProgram*>::iterator pIter;
      for(pIter = filters.begin(); pIter != filters.end(); pIter++)
      {
        std::string filterName = (*pIter)->getName();
        if((*pIter)->usable())
            glColor3f(GLfloat(0.3), GLfloat(1.0), GLfloat(0.3));
          else
            glColor3f(GLfloat(1.0), GLfloat(0.3), GLfloat(0.3));
        glhelper->glPrintf(false, posx, posy, "%s", filterName.c_str());
        posy -= 20;
      }
      // filter log available ?
      if(secondlog)
      {
        glColor3f(GLfloat(1.0), GLfloat(0.3), GLfloat(0.3));
        std::string logstr(secondlog);
        std::string message;
        for(int loc = logstr.find("\n", 0); loc != int(std::string::npos); )
        {
          message = logstr.substr(0, loc);
          glhelper->glPrintf(false, posx, posy, "%s", message.c_str());
          logstr = logstr.substr(loc+1, logstr.size());
          loc = logstr.find("\n", 0);
          posy -= 20;
        }
      }
    }
    break;
  }
  default:
    break;
  }
}

void ShaderProgram::handleUniforms(const PhysicalObject* object, int textureOffset) const
{
  for(ConstUniformInfosIter iter = uniforms.begin();
    iter != uniforms.end(); iter++)
  {
    if(iter->ID == Uniform_ObjectMass)
    {
      if(object)
        glUniform1f(iter->glID, (float)object->getMass());
    }
    else if(iter->ID == Uniform_ModelMatrix)
    {
      if(object)
      {
        GLfloat modelmatrix[16];
        GLHelper::getGLH()->setMatrix(object->getPosition(), object->getRotation(), modelmatrix);
        glUniformMatrix4fv(iter->glID, 1, GL_TRUE, modelmatrix);
      }
    }
    else if(iter->ID >= Uniform_Texture0 &&
      iter->ID <= Uniform_Texture7)
    {
      /*Uniform uID = iter->ID;
      if((PUuniformsSwapped && iter->type == UniformType_FBO_PhysicUpdate_Swap && iter->swapID != -1) ||
         (RPuniformsSwapped && iter->type == UniformType_FBO_RenderPass_Swap && iter->swapID != -1))
        uID = iter->swapID;*/
      glUniform1i(getUniformGLID(iter->ID), iter->ID);
    }
    else if(iter->ID == Uniform_Apply_Texture)
    {
      if(object)
        glUniform1i(iter->glID, object->getSurface()->diffuseTexture.textureID != 0);
    }
    else if(iter->ID == Uniform_Random)
    {
      float randomValue = (float)rand()/(float)RAND_MAX;
      glUniform1f(iter->glID, randomValue);
    }
  }
}

int ShaderProgram::getNumTextures(const UniformType restriction)
{
  // if already calculated the numbers, just return them
  if(numTextures >= 0 || countTextures())
  {
    switch(restriction)
    {
    case UniformType_FBO:
      return numFboTextures;
    case UniformType_FBO_PhysicUpdate_Swap:
    case UniformType_FBO_RenderPass_Swap:
      return numActiveFboTextures;
    case UniformType_Storage:
      return numStorageTextures;
    default:
      return numTextures;
    }
  }

  return -1;
}

void ShaderProgram::getTexFilter(std::vector<GLenum>& texFilter)
{
  std::map<Uniform, GLenum> texFilterMap;
  for(UniformInfosIter iter = uniforms.begin();iter != uniforms.end(); iter++)
  {
    if(iter->type == UniformType_FBO ||
      iter->type == UniformType_FBO_PhysicUpdate_Swap ||
      iter->type == UniformType_FBO_RenderPass_Swap)
      texFilterMap[iter->ID] = iter->texFilter;
  }

  for(std::map<Uniform, GLenum>::iterator iter = texFilterMap.begin();
    iter != texFilterMap.end(); iter++)
    texFilter.push_back(iter->second);
}

bool ShaderProgram::countTextures()
{
  // sum up unique textures ids of filter
  std::set<Uniform> textures;
  std::vector<ShaderProgram*>::iterator fIter;

  this->getTextureIDs(textures);
  for(fIter = filters.begin(); fIter != filters.end(); fIter++)
    (*fIter)->getTextureIDs(textures);
  numTextures = textures.size();

  textures.clear();
  this->getFboTextureIDs(textures);
  for(fIter = filters.begin(); fIter != filters.end(); fIter++)
    (*fIter)->getFboTextureIDs(textures);
  numFboTextures = textures.size();

  textures.clear();
  this->getActiveFboTextureIDs(textures);
  for(fIter = filters.begin(); fIter != filters.end(); fIter++)
    (*fIter)->getActiveFboTextureIDs(textures);
  numActiveFboTextures = textures.size();

  textures.clear();
  this->getStorageTextureIDs(textures);
  for(fIter = filters.begin(); fIter != filters.end(); fIter++)
    (*fIter)->getStorageTextureIDs(textures);
  numStorageTextures = textures.size();

  textures.clear();

  return true;
}

void ShaderProgram::getTextureIDs(std::set<Uniform>& textures)
{
  for(UniformInfosIter iter = uniforms.begin();iter != uniforms.end(); iter++)
  {
    if(iter->type != UniformType_Value)
      textures.insert(iter->ID);
  }
}

void ShaderProgram::getFboTextureIDs(std::set<Uniform>& textures)
{
  for(UniformInfosIter iter = uniforms.begin();iter != uniforms.end(); iter++)
  {
    if(iter->type == UniformType_FBO ||
      iter->type == UniformType_FBO_PhysicUpdate_Swap ||
      iter->type == UniformType_FBO_RenderPass_Swap)
      textures.insert(iter->ID);
  }
}

void ShaderProgram::getActiveFboTextureIDs(std::set<Uniform>& textures)
{
  for(UniformInfosIter iter = uniforms.begin();iter != uniforms.end(); iter++)
  {
    if(iter->type == UniformType_FBO)
      textures.insert(iter->ID);
  }
}

void ShaderProgram::getStorageTextureIDs(std::set<Uniform>& textures)
{
  for(UniformInfosIter iter = uniforms.begin();iter != uniforms.end(); iter++)
  {
    if(iter->type == UniformType_Storage)
      textures.insert(iter->ID);
  }
}

int ShaderProgram::getUniformGLID(const Uniform uni) const
{
  for(ConstUniformInfosIter iter = uniforms.begin();
    iter != uniforms.end(); iter++)
  {
    if(iter->ID == uni)
      return iter->glID;
  }
  return -1;
}

void ShaderProgram::bindTexture(FramebufferObject* fbo, int i)
{
  int idFBOtex = 0;
  for(ConstUniformInfosIter iter = uniforms.begin();
    iter != uniforms.end(); iter++)
  {
    if(iter->type == UniformType_Value)
      continue;

    if(i-- > 0)
    {
      if(iter->type == UniformType_FBO ||
        iter->type == UniformType_FBO_PhysicUpdate_Swap ||
        iter->type == UniformType_FBO_RenderPass_Swap)
        idFBOtex++;
      continue;
    }

    switch(iter->type)
    {
    case UniformType_FBO:
    case UniformType_FBO_PhysicUpdate_Swap:
    case UniformType_FBO_RenderPass_Swap:
    case UniformType_Storage:
      fbo->bindTexture(idFBOtex);
      break;
    case UniformType_Noise:
    case UniformType_TextureFile:
      iter->tex->BindTexture();
      break;
    default:
      break;
    }
    return;
  }
}

void ShaderProgram::unbindTexture(FramebufferObject* fbo, int i)
{
  for(ConstUniformInfosIter iter = uniforms.begin();
    iter != uniforms.end(); iter++)
  {
    if(iter->type == UniformType_Value)
      continue;

    if(i-- > 0)
      continue;

    switch(iter->type)
    {
    case UniformType_FBO:
    case UniformType_FBO_PhysicUpdate_Swap:
    case UniformType_FBO_RenderPass_Swap:
      break;
    case UniformType_Noise:
    case UniformType_TextureFile:
      iter->tex->UnBindTexture(false);
      break;
    default:
      break;
    }
    return;
  }
}

void ShaderProgram::bindTextures(FramebufferObject* fbo)
{
  for(ConstUniformInfosIter iter = uniforms.begin();
    iter != uniforms.end(); iter++)
  {
    if(iter->ignore)
      continue;

    switch(iter->type)
    {
    case UniformType_Value:
      break;
    case UniformType_FBO_PhysicUpdate_Swap:
      glActiveTexture(GL_TEXTURE0 + iter->ID);
      fbo->bindTexture(PUuniformsSwapped && iter->swapID != -1 ? iter->swapID : iter->ID);
      break;
    case UniformType_FBO_RenderPass_Swap:
      glActiveTexture(GL_TEXTURE0 + iter->ID);
      fbo->bindTexture(RPuniformsSwapped && iter->swapID != -1 ? iter->swapID : iter->ID);
      break;
    case UniformType_FBO:
    case UniformType_Storage:
      {
        bool texBound = false;
        if(PUuniformsSwapped)
        {
          for(ConstUniformInfosIter iter2 = uniforms.begin();
            iter2 != uniforms.end(); iter2++)
          {
            if(iter2->swapID == iter->ID)
            {
              glActiveTexture(GL_TEXTURE0 + iter->ID);
              fbo->bindTexture(iter2->ID);
              texBound = true;
              break;
            }
          }
        }
        if(!texBound)
        {
          glActiveTexture(GL_TEXTURE0 + iter->ID);
          fbo->bindTexture(iter->ID);
        }
        break;
        }
    case UniformType_Noise:
    case UniformType_TextureFile:
      glActiveTexture(GL_TEXTURE0 + iter->ID);
      iter->tex->BindTexture();
      break;
    }
  }
  glActiveTexture(GL_TEXTURE0);
}

void ShaderProgram::unbindTextures(FramebufferObject* fbo)
{
  //int idFBOtex = 0;
  for(ConstUniformInfosIter iter = uniforms.begin();
    iter != uniforms.end(); iter++)
  {
    switch(iter->type)
    {
    case UniformType_Value:
      break;
    case UniformType_FBO:
    case UniformType_FBO_PhysicUpdate_Swap:
    case UniformType_FBO_RenderPass_Swap:
      glActiveTexture(GL_TEXTURE0 + iter->ID);
      glBindTexture(GL_TEXTURE_2D, 0);
      break;
    case UniformType_Noise:
    case UniformType_TextureFile:
      glActiveTexture(GL_TEXTURE0 + iter->ID);
      iter->tex->UnBindTexture(false);
      break;
    default:
      break;
    }
  }
  glActiveTexture(GL_TEXTURE0);
}

void ShaderProgram::setMRT(int bitmask, int clearbitmask)
{
  // delete previous buffer arrays
  if(MRT_count > 0)
  {
    delete [] bufferArray;
    MRT_index.clear();
  }
  if(MRT_clearcount > 0)
  {
    delete [] clearbufferArray;
    MRT_clearindex.clear();
  }

  // clamp the bitfield
  bitmask =  std::min<int>(std::max<int>(0, bitmask), 255);
  clearbitmask = std::min<int>(std::max<int>(0, clearbitmask), 255);

  // read the bitfield
  MRT_clearindex.clear();
  int i, bit;
  for(i=0, bit=1; i < 8; i++, bit <<= 1)
  {
    if(bitmask & bit)
      MRT_index.push_back(i);
    if(clearbitmask & bit)
      MRT_clearindex.push_back(i);
  }
  MRT_count = MRT_index.size();
  MRT_clearcount = MRT_clearindex.size();

  // create the buffer arrays
  if(MRT_count > 0)
    bufferArray = new GLenum[MRT_count];
  if(MRT_clearcount > 0)
    clearbufferArray = new GLenum[MRT_clearcount];
}

void ShaderProgram::activateDrawBuffers(GLenum* colorBufferArray)
{
  if(MRT_count != 0)
  {
    // resolve the buffer values
    for(int i=0; i < MRT_count; i++)
      bufferArray[i] = colorBufferArray[MRT_index[i]];
    
    // set the draw buffers
    glDrawBuffers(MRT_count, bufferArray);
  }
}

void ShaderProgram::optionalClearDrawBuffers(GLenum* colorBufferArray)
{
  if(MRT_clearcount != 0)
  {
    // resolve the buffer values
    for(int i=0; i < MRT_clearcount; i++)
      clearbufferArray[i] = colorBufferArray[MRT_clearindex[i]];
    
    glDrawBuffers(MRT_clearcount, clearbufferArray);
    glClear(default_clear);
  }
}

void ShaderProgram::setExtension(ShaderProgram* prog)
{
  extension = prog;
}

ShaderProgram* ShaderProgram::getExtension()
{
  return extension;
}

std::vector<int>& ShaderProgram::getMRTindex()
{
  return MRT_index;
}

std::vector<int>& ShaderProgram::getMRTclearindex()
{
  return MRT_clearindex;
}
