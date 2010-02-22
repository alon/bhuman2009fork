/**
* @file ShaderInterface.cpp
* 
* Definition of an interface to shader definitions and the shader and program classes
*
* @author <A href="mailto:pachur@informatik.uni-bremen.de">Dennis Pachur</A>
*/ 

#include "ShaderInterface.h"
#include <list>
#include <PhysicalObjects/PhysicalObject.h>
#include <Tools/SimGraphics.h>
#include <OpenGL/FramebufferObject.h>
#include <OpenGL/Shader.h>
#include <time.h>

ShaderInterface::ShaderInterface(GraphicsManager* sg)
: initialized(GL_FALSE), usable(GL_FALSE), postShaderUsable(true),
  globalShaderUsable(true), running(false),
  showInfos(false), programInfoState(PROGRAM_LOG), shaderInfoState(SHADER_LOG),
  simgraphics(sg)
{
  ShaderProgram* program = new ShaderProgram();
  programs["none"] = program;
  runningProgram = programs.find("none");
  currentProgram = programs.end();
  previousProgram = programs.end();
  nextProgram = programs.end();
  initialProgram = programs.end();
  postProgram = programs.end();
  initialProgram = programs.end();
  fboInit = clock();

  fbo_debug = -2;
  fbo_storage = false;
  fbo_oversized = false;
  fbo_antialiased = false;
  fbo_samples = 0;
  currentFBO = -1;
  original_width = 0;
  original_height = 0;
  original_fov = 0;
}

ShaderInterface::~ShaderInterface()
{
  initialized = GL_FALSE;

  std::map<std::string, Shader*>::iterator sIter;
  for(sIter = shaders.begin(); sIter != shaders.end(); sIter++)
    delete sIter->second;
  shaders.clear();

  std::map<std::string, ShaderProgram*>::iterator pIter;
  for(pIter = programs.begin(); pIter != programs.end(); pIter++)
    delete pIter->second;
  programs.clear();

  for(pIter = filters.begin(); pIter != filters.end(); pIter++)
    delete pIter->second;
  filters.clear();

  std::vector<GLTexture*>::iterator texIter;
  for(texIter = staticTextures.begin(); texIter != staticTextures.end(); texIter++)
    delete *texIter;
  staticTextures.clear();

  destroyFramebufferObjects();
}

void ShaderInterface::createShader(std::string& name, GLenum& type, std::string& filename)
{
  Shader* shader = new Shader(name, type, filename);
  shaders[name] = shader;
}

void ShaderInterface::createProgram(const std::string& name, const std::string& extensionName, const bool isFilter)
{
  ShaderProgram* program = new ShaderProgram(name);
  program->setFilter(isFilter);
  if(programs.find(extensionName) != programs.end())
    program->setExtension(programs[extensionName]);
  if(isFilter)
    filters[name] = program;
  else
    programs[name] = program;
}

void ShaderInterface::attachShader(const std::string& pname, const std::string& sname)
{
  if(programs.find(pname) != programs.end())
  {
    if(shaders.find(sname) != shaders.end())
      programs[pname]->attachShader(shaders[sname]);
  }
  else if(filters.find(pname) != filters.end())
  {
    if(shaders.find(sname) != shaders.end())
      filters[pname]->attachShader(shaders[sname]);
  }
}

void ShaderInterface::attachFilter(const std::string& pname, const std::string& fname)
{
  if(programs.find(pname) != programs.end())
  {
    if(filters.find(fname) != filters.end())
      programs[pname]->attachFilter(filters[fname]);
  }
}

void ShaderInterface::addUniform(const std::string& pname,
                                 const std::string& uname,
                                 std::string& swapTarget,
                                 const std::string& type,
                                 const std::string& file,
                                 const bool& ignore,
                                 const std::string& filter)
{
  // find ids
  Uniform uniformID = Uniform_Undefined,
          swapID = Uniform_Undefined;
  for(int i = 0; i < NumUniqueUniforms; i++)
  {
    std::string un = UniformName[i];
    if(un == uname)
      uniformID = (Uniform)i;
    if(un == swapTarget)
      swapID = (Uniform)i;
  }

  UniformType uniType = UniformType_Value;
  GLTexture* tex = 0;
  if(type == "FBO")
    uniType = UniformType_FBO;
  else if(type == "FBOPhysicUpdateSwap")
    uniType = UniformType_FBO_PhysicUpdate_Swap;
  else if(type == "FBORenderPassSwap")
    uniType = UniformType_FBO_RenderPass_Swap;
  else if(type == "File")
  {
    uniType = UniformType_TextureFile;
    tex = getTexture(file);
  }
  else if(type == "Noise")
  {
    uniType = UniformType_Noise;
    tex = getTexture("Noise");
  }
  else if(type == "Storage")
    uniType = UniformType_Storage;

  GLenum texFilter = GL_LINEAR;
  if(filter == "nearest")
    texFilter = GL_NEAREST;

  if(programs.find(pname) != programs.end())
    programs[pname]->attachUniform(uniformID, swapID, uniType, tex, ignore, texFilter);
  else if(filters.find(pname) != filters.end())
    filters[pname]->attachUniform(uniformID, swapID, uniType, tex, ignore, texFilter);
}

void ShaderInterface::setMRT(const std::string& pname, int bitmask, int clearbitmask)
{
  if(programs.find(pname) != programs.end())
    programs[pname]->setMRT(bitmask, clearbitmask);
  else if(filters.find(pname) != filters.end())
    filters[pname]->setMRT(bitmask, clearbitmask);
}

void ShaderInterface::handleUniforms(PhysicalObject* object, bool usePostProgram)
{
  ProgIter thisProg = usePostProgram ? postProgram : runningProgram;

  int textureOffset = 0;
  UniformInfos unis = thisProg->second->getUniforms();
  for(ConstUniformInfosIter iter = unis.begin();
    iter != unis.end(); iter++)
  {
    if(iter->ID == Uniform_Num_Lights)
      glUniform1i(iter->glID, simgraphics->getNumLightsOn());
    else if(iter->ID == Uniform_ImageDimensions)
    {
      int dimensions[2]; // x is width
      simgraphics->getCurrentImageDimensions(dimensions[1], dimensions[0]);
      glUniform2i(iter->glID, dimensions[0], dimensions[1]);
    }
    else if(iter->ID == Uniform_ExposureTime)
      glUniform1f(iter->glID, simgraphics->getExposureTime());
    else if(iter->ID == Uniform_ShadowMap)
      glUniform1i(iter->glID, ++textureOffset);
  }
  thisProg->second->handleUniforms(object, textureOffset);
}

void ShaderInterface::init()
{
  if(initialized)
    return;

  srand(30757);
  initialized = GL_TRUE;

  // check for shader accesibility
  if(GLEW_ARB_fragment_program && GLEW_ARB_fragment_shader
    && GLEW_ARB_vertex_program && GLEW_ARB_vertex_shader)
    usable = GL_TRUE;
  else
  {
    GLHelper::getGLH()->glPrintMarkedText(TextObject::WARNING_HINTS, "shader", TextObject::WARNING, 5000,
      "WARNING: Your graphics card has no shader capabilities.");
    return;
  }

  // check if shader and programs are defined
  if(programs.size() == 1)
  {
    GLHelper::getGLH()->glPrintMarkedText(TextObject::WARNING_HINTS, "Shader", TextObject::WARNING, 5000, "WARNING: No Program defined.");
    return;
  }
  else if(shaders.size() == 0)
  {
    GLHelper::getGLH()->glPrintMarkedText(TextObject::WARNING_HINTS, "Shader", TextObject::WARNING, 5000, "WARNING: No Shader defined.");
    return;
  }

  currentProgram = programs.find("none");
  currentShader = shaders.end();
  createShadersAndPrograms();

  if(initialProgram != programs.end() &&
    initialProgram != programs.find("none") &&
    initialProgram->second->usable())
  {
    runningProgram = initialProgram;
    currentProgram = initialProgram;
  }
  else
    runningProgram = programs.find("none");

  previousProgram = getPreviousProgram(currentProgram);
  nextProgram = getNextProgram(currentProgram);

  if(currentShader == shaders.end())
  {
    currentShader = shaders.begin();
    previousShader = getPreviousShader(currentShader);
    nextShader = getNextShader(currentShader);
  }

  // fbo are not combinable with accumulation buffers
  if(!postShaderUsable)
    postProgram = programs.end();
}

void ShaderInterface::reloadShader(const bool killFBOs)
{
  // destroy shader
  std::map<std::string, Shader*>::iterator sIter;
  for(sIter = shaders.begin(); sIter != shaders.end(); sIter++)
    sIter->second->destroy();

  // destroy programs
  std::map<std::string, ShaderProgram*>::iterator pIter;
  for(pIter = programs.begin(); pIter != programs.end(); pIter++)
    pIter->second->destroy();

  // destroy filters
  for(pIter = filters.begin(); pIter != filters.end(); pIter++)
    pIter->second->destroy();

  // destroy frame buffer objects
  if(killFBOs)
    destroyFramebufferObjects();

  // and recreate the stuff
  createShadersAndPrograms();
}

void ShaderInterface::createShadersAndPrograms()
{
  // create shader
  std::map<std::string, Shader*>::iterator sIter;
  for(sIter = shaders.begin(); sIter != shaders.end(); sIter++)
  {
    if(!sIter->second->create())
    {
      shaderInfoState = SHADER_LOG;
      showInfos = true;
      currentShader = sIter;
      previousShader = getPreviousShader(currentShader);
      nextShader = getNextShader(currentShader);
    }
  }

  // create programs
  std::map<std::string, ShaderProgram*>::iterator pIter;
  for(pIter = programs.begin(); pIter != programs.end(); pIter++)
  {
    int status = pIter->second->create();
    if(status > 0)
    {
      programInfoState = status == 1 ? PROGRAM_LOG : PROGRAM_FILTERS;
      showInfos = true;
      currentProgram = pIter;
      previousProgram = getPreviousProgram(currentProgram);
      nextProgram = getNextProgram(currentProgram);
    }
  }
}

ProgIter ShaderInterface::getPreviousProgram(ProgIter iter)
{
  if(programs.size() == 0)
    return programs.end();

  if(iter == programs.begin())
    iter = programs.end();
  iter--;


  return iter;
}

ProgIter ShaderInterface::getNextProgram(ProgIter iter)
{
  if(programs.size() == 0)
    return programs.end();

  iter++;
  if(iter == programs.end())
    iter = programs.begin();

  return iter;
}

ShadIter ShaderInterface::getPreviousShader(ShadIter iter)
{
  if(shaders.size() == 0)
    return shaders.end();

  if(iter == shaders.begin())
    iter = shaders.end();
  iter--;

  return iter;
}

ShadIter ShaderInterface::getNextShader(ShadIter iter)
{
  if(shaders.size() == 0)
    return shaders.end();

  iter++;
  if(iter == shaders.end())
    iter = shaders.begin();

  return iter;
}

void ShaderInterface::toggleRunning()
{
  if(!initialized)
    init();
  if(usable)
  {
    if(!postShaderUsable || !globalShaderUsable)
    {
      GLHelper::getGLH()->glPrintText(TextObject::WARNING_HINTS, TextObject::WARNING, 5000,
        "WARNING: ShaderInterface cannot be toggled. Disable other features first.");
      return;
    }
    running ^= 1;
  }

  if(fbos.size() > 0 && !running)
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
  if(GLEW_VERSION_1_3)
    glActiveTexture(GL_TEXTURE0);
}

void ShaderInterface::setRunning(bool state)
{
  if(!initialized)
    init();
  if(usable)
    running = state;

  if(fbos.size() > 0 && !running)
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

void ShaderInterface::switchProgram(const GLboolean previous)
{
  if(previous)
    currentProgram = getPreviousProgram(currentProgram);
  else
    currentProgram = getNextProgram(currentProgram);

  previousProgram = getPreviousProgram(currentProgram);
  nextProgram = getNextProgram(currentProgram);
}

void ShaderInterface::switchShader(const GLboolean previous)
{
  if(previous)
    currentShader = getPreviousShader(currentShader);
  else
    currentShader = getNextShader(currentShader);

  previousShader = getPreviousShader(currentShader);
  nextShader = getNextShader(currentShader);
}

void ShaderInterface::defineGlobalProgram(const bool immediate_usage)
{
  if(!globalShaderUsable && !immediate_usage)
  {
    GLHelper::getGLH()->glPrintText(TextObject::WARNING_HINTS, TextObject::WARNING, 5000,
      "WARNING: Global Shader are currently not accessable. Disable other features first.");
    return;
  }

  if(immediate_usage)
  {
    if(!initialProgram->second->usable())
    {
      GLHelper::getGLH()->glPrintText(TextObject::WARNING_HINTS, TextObject::WARNING, 5000,
        "WARNING: Current Shader Program is not usable. See log for more infos.");
      return;
    }
    runningProgram = initialProgram;
    return;
  }

  if(currentProgram == runningProgram)
  {
    runningProgram = programs.find("none");
    return;
  }

  if(!currentProgram->second->usable())
  {
    GLHelper::getGLH()->glPrintText(TextObject::WARNING_HINTS, TextObject::WARNING, 5000,
      "WARNING: Current Shader Program is not usable. See log for more infos.");
    return;
  }

  runningProgram = currentProgram;
}

void ShaderInterface::definePostProgram()
{
  if(!postShaderUsable)
  {
    GLHelper::getGLH()->glPrintText(TextObject::WARNING_HINTS, TextObject::WARNING, 5000,
      "WARNING: Post Shader are currently not accessable. Disable other features first.");
    return;
  }

  destroyFramebufferObjects();
  fboInit = clock();

  if(currentProgram == postProgram
    || currentProgram->first == "none")
  {
    postProgram = programs.end();
    return;
  }

  if(!currentProgram->second->usable())
  {
    GLHelper::getGLH()->glPrintText(TextObject::WARNING_HINTS, TextObject::WARNING, 5000,
      "WARNING: Current Shader Program is not usable. See log for more infos.");
    return;
  }

  postProgram = currentProgram;
}

bool ShaderInterface::getPostProgramDefined()
{
  return (postProgram != programs.end());
}

void ShaderInterface::renderInfos(GLuint width, GLuint height) const
{
  if(!running || !showInfos)
    return;

  GLuint posx = 10,
         posy = height-20;

  GLHelper* glhelper = GLHelper::getGLH();

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);
  glColor4d(1.0, 1.0, 1.0, 0.6);
  glhelper->glPrintf(true, posx, posy, "Program");
  posx += 250;
  glColor4d(1.0, 1.0, 1.0, programInfoState == PROGRAM_LOG ? 0.6 : 0.4);
  glhelper->glPrintf(true, posx, posy, "LOG");
  glColor4d(1.0, 1.0, 1.0, programInfoState == PROGRAM_DESCRIPTION ? 0.6 : 0.4);
  posx += 50;
  glhelper->glPrintf(true, posx, posy, "DESCRIPTION");
  glColor4d(1.0, 1.0, 1.0, programInfoState == PROGRAM_SHADER ? 0.6 : 0.4);
  posx += 130;
  glhelper->glPrintf(true, posx, posy, "SHADER");
  glColor4d(1.0, 1.0, 1.0, programInfoState == PROGRAM_UNIFORMS ? 0.6 : 0.4);
  posx += 80;
  glhelper->glPrintf(true, posx, posy, "UNIFORMS");
  glColor4d(1.0, 1.0, 1.0, programInfoState == PROGRAM_FILTERS ? 0.6 : 0.4);
  posx += 100;
  glhelper->glPrintf(true, posx, posy, "FILTERS");
  posx = 10;
  posy = height-200;
  glColor4d(1.0, 1.0, 1.0, 0.6);
  glhelper->glPrintf(true, posx, posy, "Shader");
  posx += 250;
  glColor4d(1.0, 1.0, 1.0, shaderInfoState == SHADER_LOG ? 0.6 : 0.4);
  glhelper->glPrintf(true, posx, posy, "LOG");
  glColor4d(1.0, 1.0, 1.0, shaderInfoState == SHADER_SOURCE ? 0.6 : 0.4);
  posx += 50;
  glhelper->glPrintf(true, posx, posy, "SOURCE");
  posx = 20;
  posy = height-50;
  ProgIter thisProg;
  for(int i=0; i < 3; i++)
  {
    if(i==0) thisProg = previousProgram;
    else if(i==1) thisProg = currentProgram;
    else thisProg = nextProgram;

    if(thisProg == runningProgram)
      glColor4d(0.0, 1.0, 0.0, 0.2+(0.4*(i%2)));
    else if(thisProg == postProgram)
      glColor4d(1.0, 0.8, 0.0, 0.6);
    else if(thisProg->second->usable())
      glColor4d(1.0, 1.0, 1.0, 0.2+(0.4*(i%2)));
    else
      glColor4d(1.0, 0.0, 0.0, 0.2+(0.4*(i%2)));
    glhelper->glPrintf(true, posx, posy, "%s", thisProg->second->getName().c_str());

    if(i==1)
    {
      glColor3d(0.7, 0.7, 0.7);
      thisProg->second->printInfo(programInfoState, posy+20);
    }
    posy -= 30;
  }
  ShadIter thisShad;
  posy = height-230;
  for(int i=0; i < 3; i++)
  {
    if(i==0) thisShad = previousShader;
    else if(i==1) thisShad = currentShader;
    else thisShad = nextShader;

    if(thisShad->second->usable())
      glColor4d(1.0, 1.0, 1.0, 0.2+(0.4*(i%2)));
    else
      glColor4d(1.0, 0.0, 0.0, 0.2+(0.4*(i%2)));
    glhelper->glPrintf(true, posx, posy, "%s", thisShad->second->getName().c_str());

    if(i==1)
    {
      glColor3d(0.7, 0.7, 0.7);
      thisShad->second->printInfo(shaderInfoState, posy+20);
    }
    posy -= 30;
  }
  glDisable(GL_BLEND);
}

void ShaderInterface::toggleShowInfos()
{
  showInfos ^= 1;
}

bool ShaderInterface::getShowInfosState() const
{
  return showInfos;
}

void ShaderInterface::switchInfo(const GLboolean previous)
{
  int info = (int)programInfoState;
  if(previous)
  {
    info++;
    info%=5;
  }
  else
  {
    if(info == 0)
      info = 4;
    else
      info--;
  }
  programInfoState = (ProgramInfo)info;
  shaderInfoState = (ShaderInfo)(info%2);
}

ShaderProgram* ShaderInterface::getShaderProgram(const std::string& name)
{
  ProgIter ptest = programs.find(name);
  if(ptest != programs.end())
    return ptest->second;
  else
    return 0;
}

bool ShaderInterface::enableGlobalShader(const std::string& pname)
{
  if(running)
  {
    ProgIter ptest = programs.find(pname);
    if(ptest != programs.end())
    {
      if(ptest->second->usable())
      {
        runningProgram = ptest;
        glUseProgram(runningProgram->second->getId());
        return true;
      }
    }
  }
  return false;
}

void ShaderInterface::setGlobalShader(const std::string& pname, const bool immediate_usage)
{
  ProgIter ptest = programs.find(pname);
  ProgIter plast = runningProgram;
  if(ptest != programs.end())
    initialProgram = ptest;

  if(immediate_usage && plast != ptest)
    defineGlobalProgram(true);
}

void ShaderInterface::setPostShader(const std::string& pname, const bool immediate_usage)
{
  if(immediate_usage && pname == "")
  {
    postProgram = programs.end();
    destroyFramebufferObjects();
  }
  else
  {
    ProgIter ptest = programs.find(pname);
    ProgIter plast = postProgram;
    if(ptest != programs.end())
      postProgram = ptest;
    fboInit = clock();
  }
}

void ShaderInterface::enableLocalShader(const std::string& pname)
{
  if(running)
  {
    ProgIter ptest = programs.find(pname);
    if(ptest != programs.end())
    {
      if(ptest->second->usable())
        glUseProgram(ptest->second->getId());
      else
        GLHelper::getGLH()->glPrintMarkedText(TextObject::WARNING_HINTS, ptest->second->getName().c_str(), TextObject::WARNING, 5000,
          "WARNING: Program \"%s\" is locally used but did not compile without failures!",
          ptest->second->getName().c_str());
    }
  }
}

void ShaderInterface::enableLocalShader(PhysicalObject* obj) const
{
  if(running)
  {
    if(obj->getShaderProgram()->usable())
    {
      glUseProgram(obj->getShaderProgram()->getId());
      obj->getShaderProgram()->handleUniforms(obj);
    }
  }
}

void ShaderInterface::disableLocalShader() const
{
  if(running)
    glUseProgram(runningProgram->second->getId());
}

bool ShaderInterface::initFramebufferObject(const int fbo_reg)
{
  // check capabilities
  if(!GLEW_EXT_framebuffer_object)
    return false;

  ShaderProgram* prog = postProgram->second->getExtension();
  if(prog == NULL)
    prog = postProgram->second;

  int numTex = prog->getNumTextures(UniformType_FBO);
  if(numTex <= 0)
  {
    GLHelper::getGLH()->glPrintText(TextObject::WARNING_HINTS, TextObject::WARNING, 5000,
        "WARNING: selected post program has no color buffer attachments!");
    return false;
  }
  std::vector<GLenum> texFilter;
  prog->getTexFilter(texFilter);

  bool useStorage = (prog->getNumTextures(UniformType_Storage) > 0) ? true : false;

  // create requested frame buffer object
  fbos[fbo_reg] = new FramebufferObject(
    numTex, texFilter, fbo_oversized, fbo_antialiased, fbo_samples, useStorage);
  return true;
}

void ShaderInterface::destroyFramebufferObjects()
{
  if(!GLEW_EXT_framebuffer_object)
    return;

  std::map<int, FramebufferObject*>::iterator fboIter;
  for(fboIter = fbos.begin(); fboIter != fbos.end(); fboIter++)
    delete fboIter->second;
  fbos.clear();
  currentFBO = -1;
}

void ShaderInterface::preparePostProduction(GLuint& width, GLuint& height,
                                            GLdouble& fov, const int fbo_reg)
{
  // save original render settings (needed for oversized fbos)
  original_width = width;
  original_height = height;
  original_fov = fov;

  // global shader init 
  glUseProgram(runningProgram->second->getId());

  // check if a post program has been selected: if so, prepare the frame buffer objects
  if(postProgram == programs.end())
    return;

  // check fbo
  if(fbos.find(fbo_reg) == fbos.end())
  {
    if(!initFramebufferObject(fbo_reg))
    {
      // if we tried creation longer than a minute: abort
      if(fboInit - clock() > 1000)
      {
        GLHelper::getGLH()->glPrintText(TextObject::WARNING_HINTS, TextObject::WARNING, 5000,
        "WARNING: preparing post production failed: fbo initialization went wrong");
        postProgram = programs.end();
      }
      return;
    }
  }

  // choose render destination
  currentFBO = fbo_reg;
  FramebufferObject* fbo = fbos[currentFBO];
  if(!fbo->update(width, height))
  {
    GLHelper::getGLH()->glPrintText(TextObject::WARNING_HINTS, TextObject::WARNING, 500,
      "WARNING: preparing post production failed: fbo not initialized");
    destroyFramebufferObjects();
    postProgram = programs.end();
    return;
  }

  // set the new render resolution (if oversized fbos are used)
  if(fbo_oversized)
  {
    width = (GLuint)((float)(width)*1.4f);
    height = (GLuint)((float)(height)*1.4f);
    fov *= 1.6;
  }

  // choose render target
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo->getID());
  postProgram->second->optionalClearDrawBuffers(fbo->getColorBuffersExt());
  fbo->handleTextures(postProgram->second->getMRTclearindex());
  runningProgram->second->activateDrawBuffers(fbo->getColorBuffersExt());
  runningProgram->second->bindTextures(fbo);
}

void ShaderInterface::drawPostSurface(const bool isObjectWindow)
{
  // disable global program
  glUseProgram(0);

  // if no fbo selected or no post program active: return
  if(currentFBO == -1 || postProgram == programs.end())
    return;

  // reselect render destination
  FramebufferObject* fbo = fbos[currentFBO];
  if(!fbo->usable())
    return;

  // if a filtered image is requested: do filter post processing
  std::vector<ShaderProgram*> kernels = postProgram->second->getFilters();
  std::vector<ShaderProgram*>::iterator kIter;
  for(kIter = kernels.begin(); kIter != kernels.end(); kIter++)
  {
    fbo->handleTextures(runningProgram->second->getMRTindex());
    applyKernel(*kIter);
  }
  fbo->handleTextures(runningProgram->second->getMRTindex());
  
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
  // reset draw and read targets if pixel copy has used them
  if(fbo->getAntialising())
  {
    glReadBuffer(GL_BACK);
    glDrawBuffer(GL_BACK);
  }

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

  // if renderbuffer debug is enabled
  // switch to multiple viewport drawing
  // else resize if oversized fbos are used
  int numTextures = postProgram->second->getNumTextures();
  if(fbo_debug != -2 && numTextures > 1)
  {
    drawDebugPostSurfaces();
    if(fbo_debug > -1)
      return;
  }
  else if(fbo_oversized)
    setOriginalViewport(isObjectWindow);

  // set up textures for post drawing usage
  postProgram->second->bindTextures(fbo);

  // set up render environment
  glEnable(GL_POLYGON_SMOOTH);
  glDisable(GL_DEPTH_TEST);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
	glOrtho(0.0, 1.0, 0.0, 1.0, 0.0, 0.2);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
  glTranslatef(0.0, 0.0, GLfloat(-0.1));

  // load and run post program
  glUseProgram(postProgram->second->getId());
  handleUniforms(NULL, true);
  
  // render post surface
  if(fbo_oversized)
  {
    // 20% of the rendered image is the oversize part
    // and should not be rendered in the final image
    glBegin(GL_QUADS);
      glTexCoord2f(0.205f, 0.205f); glVertex2d(0.0, 0.0);
      glTexCoord2f(0.795f, 0.205f); glVertex2d(1.0, 0.0);
      glTexCoord2f(0.795f, 0.795f); glVertex2d(1.0, 1.0);
      glTexCoord2f(0.205f, 0.795f); glVertex2d(0.0, 1.0);
    glEnd();
  }
  else
  {
    glBegin(GL_QUADS);
      glTexCoord2f(0.0f, 0.0f); glVertex2d(0.0, 0.0);
      glTexCoord2f(1.0f, 0.0f); glVertex2d(1.0, 0.0);
      glTexCoord2f(1.0f, 1.0f); glVertex2d(1.0, 1.0);
      glTexCoord2f(0.0f, 1.0f); glVertex2d(0.0, 1.0);
    glEnd();
  }

  glUseProgram(0);

  glDisable(GL_POLYGON_SMOOTH);
  glEnable(GL_DEPTH_TEST);

  // revert texture units
  postProgram->second->unbindTextures(fbo);

  // if debugging, pop viewport attrib back again
  if(fbo_debug == -1 && numTextures > 1)
    glPopAttrib();
}

void ShaderInterface::drawDebugPostSurfaces()
{
  // render one texture at a time
  glActiveTexture(GL_TEXTURE0);
  glEnable(GL_TEXTURE_2D);

  // set up render environment
  glEnable(GL_POLYGON_SMOOTH);
  glDisable(GL_DEPTH_TEST);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
	glOrtho(0.0, 1.0, 0.0, 1.0, 0.0, 0.2);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
  glTranslatef(0.0, 0.0, GLfloat(-0.1));

  glPushAttrib(GL_VIEWPORT_BIT);

  // render one texture per surface
  int renderSurfaces = postProgram->second->getNumTextures();
  GLsizei surfaceWidth = original_width/renderSurfaces;
  GLsizei surfaceHeight = original_height/renderSurfaces;

  FramebufferObject* fbo = fbos[currentFBO];
  if(fbo_debug > -1)
  {
    glViewport(0, 0, original_width, original_height);
    postProgram->second->bindTexture(fbo, fbo_debug);

    // render post surface
    glBegin(GL_QUADS);
      glTexCoord2f(0.0f, 0.0f); glVertex2d(0.01, 0.01);
      glTexCoord2f(1.0f, 0.0f); glVertex2d(0.99, 0.01);
      glTexCoord2f(1.0f, 1.0f); glVertex2d(0.99, 0.99);
      glTexCoord2f(0.0f, 1.0f); glVertex2d(0.01, 0.99);
    glEnd();

    postProgram->second->unbindTexture(fbo, fbo_debug);
  }
  else
  {
    for(int i = 0; i < renderSurfaces; i++)
    {
      glViewport(i * surfaceWidth, original_height - surfaceHeight,
                 surfaceWidth, surfaceHeight);

      postProgram->second->bindTexture(fbo, i);

      // render post surface
      glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex2d(0.01, 0.01);
        glTexCoord2f(1.0f, 0.0f); glVertex2d(0.99, 0.01);
        glTexCoord2f(1.0f, 1.0f); glVertex2d(0.99, 0.99);
        glTexCoord2f(0.0f, 1.0f); glVertex2d(0.01, 0.99);
      glEnd();

      postProgram->second->unbindTexture(fbo, i);
    }

    // set viewport for final image
    glViewport(surfaceWidth/2, 0,
               original_width - surfaceWidth, original_height - surfaceHeight);
  }
  glDisable(GL_POLYGON_SMOOTH);
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_TEXTURE_2D);
}

void ShaderInterface::drawPostSurface()
{
  // if no fbo selected or no post program active: return
  if(currentFBO == -1 || postProgram == programs.end())
    return;

  // reselect render destination
  FramebufferObject* fbo = fbos[currentFBO];
  if(!fbo->usable())
    return;

  // disable global program
  glUseProgram(0);

  // if a filtered image is requested: do filter post processing
  std::vector<ShaderProgram*> kernels = postProgram->second->getFilters();
  std::vector<ShaderProgram*>::iterator kIter;
  for(kIter = kernels.begin(); kIter != kernels.end(); kIter++)
  {
    fbo->handleTextures(runningProgram->second->getMRTindex());
    applyKernel(*kIter);
  }
  fbo->handleTextures(runningProgram->second->getMRTindex());

  // reset draw and read targets if pixel copy has used them
  if(fbo->getAntialising())
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo->getID());

  postProgram->second->activateDrawBuffers(fbo->getColorBuffersExt());

  glClear(GL_COLOR_BUFFER_BIT);
  glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

  // set up textures for post drawing usage
  postProgram->second->bindTextures(fbo);

  // set up render environment
  glEnable(GL_POLYGON_SMOOTH);
  glDisable(GL_DEPTH_TEST);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
	glOrtho(0.0, 1.0, 0.0, 1.0, 0.0, 0.2);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
  glTranslatef(0.0, 0.0, GLfloat(-0.1));

  // load and run post program
  glUseProgram(postProgram->second->getId());
  handleUniforms(NULL, true);
  
  // render post surface
  glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex2d(0.0, 0.0);
    glTexCoord2f(1.0f, 0.0f); glVertex2d(1.0, 0.0);
    glTexCoord2f(1.0f, 1.0f); glVertex2d(1.0, 1.0);
    glTexCoord2f(0.0f, 1.0f); glVertex2d(0.0, 1.0);
  glEnd();
  
  glDisable(GL_POLYGON_SMOOTH);
  glEnable(GL_DEPTH_TEST);

  // revert texture units
  postProgram->second->unbindTextures(fbo);

  // copy multisample results
  fbo->handleTextures(postProgram->second->getMRTindex());
  if(fbo->getAntialising())
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo->getID());

  // reenable global shader
  glUseProgram(runningProgram->second->getId());
}

bool ShaderInterface::getMultisampledFBOsState()
{
  return fbo_antialiased;
}

void ShaderInterface::toggleMultisampledFBOsState(int samples)
{
  fbo_antialiased ^= 1;
  fbo_samples = samples;
  std::map<int, FramebufferObject*>::iterator fboIter;
  for(fboIter = fbos.begin(); fboIter != fbos.end(); fboIter++)
    fboIter->second->setAntialising(fbo_antialiased, fbo_samples);
}

void ShaderInterface::setDebugFBOs(const int debugState)
{
  fbo_debug = debugState;
}

int ShaderInterface::getDebugFBOsState() const
{
  return fbo_debug;
}

int ShaderInterface::getPreviousMVPMID()
{
  if(runningProgram->second->getName() != "none")
    return runningProgram->second->getUniformGLID(Uniform_Previous_ModelViewProjectionMatrix);
  return -1;
}

void ShaderInterface::applyKernel(ShaderProgram* filter)
{
  // set up render environment
  glEnable(GL_POLYGON_SMOOTH);
  glDisable(GL_DEPTH_TEST);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
	glOrtho(0.0, 1.0, 0.0, 1.0, 0.0, 0.2);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
  glTranslatef(0.0, 0.0, GLfloat(-0.1));

  FramebufferObject* fbo = fbos[currentFBO];
  filter->bindTextures(fbo);

  glUseProgram(filter->getId());
  filter->handleUniforms(NULL);
  
  // render post surface
  glBegin(GL_QUADS);
  glTexCoord2f(0.0f, 0.0f); glVertex2d(0.0, 0.0);
  glTexCoord2f(1.0f, 0.0f); glVertex2d(1.0, 0.0);
  glTexCoord2f(1.0f, 1.0f); glVertex2d(1.0, 1.0);
  glTexCoord2f(0.0f, 1.0f); glVertex2d(0.0, 1.0);
  glEnd();

  glUseProgram(0);

  filter->unbindTextures(fbo);
}

void ShaderInterface::toggleOversizedFBOs()
{
  fbo_oversized ^= 1;
  
  std::map<int, FramebufferObject*>::iterator fboIter;
  for(fboIter = fbos.begin(); fboIter != fbos.end(); fboIter++)
  {
    fboIter->second->setOversize(fbo_oversized);
    fboIter->second->update(original_width, original_height, true);
  }
}

bool ShaderInterface::getOversizedFBOsState()
{
  return fbo_oversized;
}

void ShaderInterface::setOriginalViewport(const bool isObjectWindow)
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(original_fov, simgraphics->currentAspect,
                 simgraphics->currentNearClip, simgraphics->currentFarClip);
  glViewport(0, 0, original_width, original_height);
  glMatrixMode(GL_MODELVIEW);
  
  // if object window: update selection matrices
  if(isObjectWindow)
  {
    glGetDoublev(GL_MODELVIEW_MATRIX, simgraphics->mvmatrix);
    glGetDoublev(GL_PROJECTION_MATRIX, simgraphics->projmatrix);
    glGetIntegerv(GL_VIEWPORT, simgraphics->viewport);
  }
}

void ShaderInterface::swapRPRenderTargets()
{
  if(postProgram == programs.end() ||
    currentFBO == -1)
    return;

  postProgram->second->RPuniformsSwapped ^= 1;

  if(fbos[currentFBO]->RPswapped == postProgram->second->RPuniformsSwapped)
    return;

  std::map<int, FramebufferObject*>::iterator fboIter;
  UniformInfos unis = postProgram->second->getUniforms();
  for(ConstUniformInfosIter uniIter = unis.begin();
    uniIter != unis.end(); uniIter++)
  {
    if(uniIter->type != UniformType_FBO_RenderPass_Swap)
      continue;
    fbos[currentFBO]->swapAttachments(uniIter->ID, uniIter->swapID);
  }
  fbos[currentFBO]->RPswapped ^= 1;
  
  runningProgram->second->activateDrawBuffers(fbos[currentFBO]->getColorBuffersExt());
}

void ShaderInterface::swapPURenderTargets()
{
  if(postProgram == programs.end() ||
    currentFBO == -1)
    return;

  postProgram->second->PUuniformsSwapped ^= 1;
  
  if(fbos[currentFBO]->PUswapped == postProgram->second->PUuniformsSwapped)
    return;

  std::map<int, FramebufferObject*>::iterator fboIter;
  UniformInfos unis = postProgram->second->getUniforms();
  for(ConstUniformInfosIter uniIter = unis.begin();
    uniIter != unis.end(); uniIter++)
  {
    if(uniIter->type != UniformType_FBO_PhysicUpdate_Swap)
      continue;
    fbos[currentFBO]->swapAttachments(uniIter->ID, uniIter->swapID);
  }
  fbos[currentFBO]->PUswapped ^= 1; 
}

GLTexture* ShaderInterface::getTexture(const std::string& filename)
{
  // check for a existing texture with the same name
  std::vector<GLTexture*>::iterator texIter;
  for(texIter = staticTextures.begin(); texIter != staticTextures.end(); texIter++)
  {
    if((*texIter)->getTexName() == filename)
      return *texIter;
  }

  GLTexture* tex = new GLTexture();
  tex->setTexName(filename);

  // special case: color and noise texture
  if(filename == "Noise")
    tex->setWidth(128);

  staticTextures.push_back(tex);
  return tex;
}

void ShaderInterface::copyFramebufferToTexture(const int fbo_reg)
{
  if(fbos.find(fbo_reg) != fbos.end())
    fbos[fbo_reg]->storeFramebuffer();
}
