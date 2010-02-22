/**
* @file Camera.cpp
* 
* Implementation of class Camera
*
* @author <A href="mailto:Tim.Laue@dfki.de">Tim Laue</A>
* @author <A href="mailto:pachur@informatik.uni-bremen.de">Dennis Pachur</A>
*/

#include "Camera.h"
#include "../Platform/OffscreenRenderer.h"
#include "../Simulation/Simulation.h"
#include "../Tools/SimGraphics.h"
#include "../OpenGL/Constants.h"


Camera::Camera():ImagingSensor()
{
  cycle_order_offset = -1;
  fbo_reg = GLHelper::getGLH()->getFBOreg();
  sensorReading.data.byteArray = 0;
}

Camera::Camera(const AttributeSet& attributes):ImagingSensor(attributes)
{
  cycle_order_offset = -1;
  fbo_reg = GLHelper::getGLH()->getFBOreg();
  sensorReading.data.byteArray = 0;
  sensorReading.minValue = 0;
  sensorReading.maxValue = 255;
  sensorReading.sensorType = SensorReading::cameraSensor;
  sensorReading.portName = "image"; 
}

Camera::~Camera()
{
  if(sensorReading.data.byteArray)
    delete [] sensorReading.data.byteArray;
  GLHelper::getGLH()->removeFBOreg(fbo_reg);
}

void Camera::addToLists(std::vector<SensorPort>& sensorPortList,
                        std::vector<Actuatorport*>& actuatorportList,
                        std::vector<Actuator*>& actuatorList) 
{
  sensorReading.fullSensorName = fullName;
  sensorPortList.push_back(SensorPort(fullName+".image",OffscreenRenderer::IMAGE,this));
}

void Camera::addToDescriptions(std::vector<ObjectDescription>& objectDescriptionTree,
                               int depth) 
{
  Sensor::addToDescriptions(objectDescriptionTree, depth);  

  //Add Sensorport:
  ObjectDescription sensorportDesc;
  sensorportDesc.name = "image";
  sensorportDesc.fullName = fullName + ".image";
  sensorportDesc.depth = depth + 1;
  sensorportDesc.type = OBJECT_TYPE_SENSORPORT;
  objectDescriptionTree.push_back(sensorportDesc);
}

void Camera::setResolution(int x, int y)
{
  if(sensorReading.data.byteArray)
    delete [] sensorReading.data.byteArray;
  sensorReading.dimensions.clear();
  sensorReading.dimensions.push_back(x);
  sensorReading.dimensions.push_back(y);
  sensorReading.dimensions.push_back(3);
  resolutionX = x;
  resolutionY = y;
  sensorReading.data.byteArray = 0;
}

void Camera::renderCubeMap(VisualizationParameterSet& visParams)
{
  // cube map has only to be build if a reflective object is chosen
  if(!graphicsManager->doEnvRendering())
    return;  

  // set clear color once
  Surface* backgroundSurface = simulation->getBackgroundSurface();
  glClearColor(backgroundSurface->color[0], backgroundSurface->color[1],
      backgroundSurface->color[2], backgroundSurface->color[3]);

  // cube map rendering
  for(int generateCubeMap = 6; generateCubeMap > 0; generateCubeMap--)
  {
    glClear(default_clear);
    graphicsManager->setCubeMapPerspective();
    setCamera(visParams, generateCubeMap);
    graphicsManager->updateLights(true, false, visParams.provideUniforms);
    graphicsManager->renderEnvironment(maxRange);
    // prepare surface properties
    glCallList(graphicsManager->enableStateDL[visParams.surfaceStyle]);
    rootNode->draw(visParams);
    glCallList(graphicsManager->disableStateDL[visParams.surfaceStyle]);
    graphicsManager->revertLightTextureUnits(visParams.provideUniforms);
    glFlush();
    // save image to cubemap file
    graphicsManager->handleCubeMapping(generateCubeMap, simulation->getFilename());
  }
}

void Camera::renderingInstructions(VisualizationParameterSet& visParams)
{
  // set some values
  graphicsManager->getShaderUsage(visParams);
  // prepare shadow maps
  while(graphicsManager->prepareLightForShadowMapping(visParams))
    rootNode->quickDraw();
  // prepare cube map
  graphicsManager->getShaderUsage(visParams);
  renderCubeMap(visParams);
  // prepare rendering
  glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
  graphicsManager->prepareRendering(visParams, resolutionX, resolutionY,
    aspect, Functions::toDeg(angleY), minRange, maxRange, fbo_reg);
  // get motion blur values and clear view
  graphicsManager->setUpMotionBlurSettings(visParams);
  // do the motion blur render passes (if motion blur is off -> 1 pass)
  for(; visParams.mb_currentPass <= visParams.mb_renderPasses; visParams.mb_currentPass++)
  {
    // set the view
    graphicsManager->setPerspective();
    setCamera(visParams);
    glClear(default_clear);
    // update light positions
    graphicsManager->updateLights(true, visParams.renderPasses == 2, visParams.provideUniforms);
    // prepare surface properties
    graphicsManager->renderEnvironment(maxRange);
    glCallList(graphicsManager->enableStateDL[visParams.surfaceStyle]);
    // render
    rootNode->draw(visParams);
    if(visParams.renderPasses == 2)
    {
      graphicsManager->updateLights(true, false, visParams.provideUniforms);
      rootNode->draw(visParams);
    }
    // undo surface properties
    glCallList(graphicsManager->disableStateDL[visParams.surfaceStyle]);
    graphicsManager->revertLightTextureUnits(visParams.provideUniforms);
    // load to accumulation buffer, blend with accumulation buffer or just do nothing (default)
    if(visParams.mb_useAccum && visParams.mb_currentPass == 1)
      glAccum(GL_LOAD, 1.0f/visParams.mb_renderPasses);
    else if(visParams.mb_useAccum)
      glAccum(GL_ACCUM, 1.0f/visParams.mb_renderPasses);
    // render post production and swap render targets if multi pass rendering with fbo usage
    graphicsManager->finishRendering(visParams, false);
  }
  // write back result if accum was used
  if(visParams.mb_useAccum)
    glAccum(GL_RETURN, 1.0f);
  glFlush(); 
}

void Camera::setCamera(const VisualizationParameterSet& visParams, const int generateCubeMap)
{
  // set the camera
  if(generateCubeMap > 0)
  {
    graphicsManager->setCubeMapCamera(generateCubeMap, graphicsManager->reflectionObject ?
      graphicsManager->reflectionObject->getPosition() : Vector3d());
    return;
  }
  else
  {
    Vector3d forwardVec(1.0,0.0,0.0);
    Vector3d upVec(0.0,0.0,1.0);

    // if the camera is bound to a physical object and motion blur is active
    // the camera needs to be transformed across the render passes
    if(visParams.mb_mode != MOTIONBLUR_OFF)
    {
      if(visParams.mb_mode == MOTIONBLUR_6PASSES ||
          visParams.mb_mode == MOTIONBLUR_12PASSES ||
          visParams.mb_mode == MOTIONBLUR_24PASSES ||
          visParams.mb_mode == MOTIONBLUR_ACCUMULATION)
      {
        if(cycle_order_offset >= 0 && visParams.mb_renderPasses > 1)
        {
          float pos_offset = cycle_order_offset -
            ((float)(visParams.mb_currentPass-1.0f)/(visParams.mb_renderPasses-1.0f))*graphicsManager->getExposure2Physic();
          GLHelper::getGLH()->setInterpolatedCameraLookAt( previous_positions, previous_rotations, pos_offset);
        }
        else
          GLHelper::getGLH()->setMatrix(position, rotation);
      }
      else if(visParams.mb_mode == MOTIONBLUR_VELOCITY_BUFFER ||
        visParams.mb_mode == MOTIONBLUR_VELOCITY_BUFFER_2 ||
        visParams.mb_mode == MOTIONBLUR_VELOCITY_BUFFER_2_GEO)
      {
        forwardVec.rotate(rotation);
        upVec.rotate(rotation);
        gluLookAt(position.v[0], position.v[1], position.v[2], 
                  position.v[0] + forwardVec.v[0],
                  position.v[1] + forwardVec.v[1], 
                  position.v[2] + forwardVec.v[2],
                  upVec.v[0], upVec.v[1], upVec.v[2]);
      }
    }
    else
    {
      forwardVec.rotate(rotation);
      upVec.rotate(rotation);
      gluLookAt(position.v[0], position.v[1], position.v[2], 
                position.v[0] + forwardVec.v[0],
                position.v[1] + forwardVec.v[1], 
                position.v[2] + forwardVec.v[2],
                upVec.v[0], upVec.v[1], upVec.v[2]);
    }
  }
}

SensorReading& Camera::getSensorReading(int localPortId)
{ 
  int simulationStep(simulation->getSimulationStep());
  if(!osRenderer)
    osRenderer = simulation->getOffscreenRenderer();
  if(!sensorReading.data.byteArray)
  {
    sensorReading.data.byteArray = new unsigned char[sensorReading.dimensions[0] *
      sensorReading.dimensions[1] * sensorReading.dimensions[2]];
  }
  unsigned char* imageData = const_cast<unsigned char*>(sensorReading.data.byteArray);
  if(exposureTime > 0.0)
  {
    // currently physicupdates are rewritten at the end of the simstep and not
    // at the end of the pyhsic update...
    double simStepLength = simulation->getStepLength();
    double timePast = (double)(simulationStep - lastComputationStep)*simStepLength;
    if(timePast >= exposureTime)
    {
      osRenderer->prepareRendering(resolutionX, resolutionY);
      graphicsManager->setExposure(exposureTime, simStepLength);
      render();
      osRenderer->finishRendering(imageData, resolutionX, resolutionY, OffscreenRenderer::IMAGE);
      lastComputationStep = simulationStep;
    }
  }
  else if(simulationStep>lastComputationStep)
  {
    osRenderer->prepareRendering(resolutionX, resolutionY);
    render();
    osRenderer->finishRendering(imageData, resolutionX, resolutionY, OffscreenRenderer::IMAGE);
    lastComputationStep = simulationStep;
  }
  return sensorReading;
}

SimObject* Camera::clone() const
{
  Camera* newCamera= new Camera();
  newCamera->copyStandardMembers(this);
  newCamera->sensorReading = sensorReading;
  if(sensorReading.data.byteArray)
  {
    int size = resolutionX * resolutionY * 8;
    newCamera->sensorReading.data.byteArray = new unsigned char[size];
  }
  newCamera->childNodes.clear();
  std::list<SimObject*>::const_iterator pos;
  for(pos = childNodes.begin(); pos != childNodes.end(); ++pos)
  {
    SimObject* childNode = (*pos)->clone();
    newCamera->addChildNode(childNode, false);
  }
  SimObject* newObject = newCamera;
  return newObject;
}

void Camera::writeBackPhysicalResult(const bool& store_last_results)
{
  if(!store_last_results)
    return;

  // store the positions...
  cycle_order_offset = (++cycle_order_offset)%12;
  previous_positions[cycle_order_offset] = position;
  previous_rotations[cycle_order_offset] = rotation;
}

void Camera::render()
{
  graphicsManager->beginFPSCount(fbo_reg);

  // set the visual parameters  
  VisualizationParameterSet visParams;
  visParams.drawForSensor = true;
  visParams.surfaceStyle = VisualizationParameterSet::CAMERA_SIMULATION;

  visParams.mb_mode = simulation->getMotionBlurMode();
  if(visParams.mb_mode != MOTIONBLUR_OFF)
  {
    graphicsManager->setCamMBInfos(previous_positions, previous_rotations, cycle_order_offset);
    if(visParams.mb_mode == MOTIONBLUR_COMBINED_METHOD)
    {
      clearColor = blackNoAlpha;
      graphicsManager->shader->setRunning(false);
      visParams.mb_mode = MOTIONBLUR_12PASSES;
      visParams.mb_skip = MOTIONBLUR_VELOCITY_BUFFER_2;
      renderingInstructions(visParams);
      graphicsManager->shader->copyFramebufferToTexture(fbo_reg);
      graphicsManager->shader->setRunning(true);
      clearColor = grey;
      visParams.mb_mode = MOTIONBLUR_VELOCITY_BUFFER_2;
      visParams.mb_skip = MOTIONBLUR_12PASSES;
      renderingInstructions(visParams);
    }
    else if(visParams.mb_mode == MOTIONBLUR_COMBINED_METHOD_2)
    {
      clearColor = blackNoAlpha;
      graphicsManager->shader->setGlobalShader(std::string("Accumulation Buffer"), true);
      graphicsManager->shader->setPostShader(std::string("Post Accumulation Buffer"), true);
      visParams.mb_mode = MOTIONBLUR_ACCUMULATION;
      visParams.mb_skip = MOTIONBLUR_VELOCITY_BUFFER_2;
      visParams.finishRendering = false;
      renderingInstructions(visParams);

      clearColor = grey;
      graphicsManager->shader->setGlobalShader(std::string("Velocity Buffer 2"), true);
      graphicsManager->shader->setPostShader(std::string("Post Velocity Buffer Comb 2"), true);
      visParams.mb_mode = MOTIONBLUR_VELOCITY_BUFFER_2;
      visParams.mb_skip = MOTIONBLUR_12PASSES;
      visParams.finishRendering = true;
      renderingInstructions(visParams);
    }
    else
    {
      if(visParams.mb_mode == MOTIONBLUR_VELOCITY_BUFFER ||
        visParams.mb_mode == MOTIONBLUR_VELOCITY_BUFFER_2 ||
        visParams.mb_mode == MOTIONBLUR_VELOCITY_BUFFER_2_GEO)
        clearColor = grey;
      else
        clearColor = simulation->getBackgroundSurface()->color;
      renderingInstructions(visParams);
    }
  }
  else
  {
    clearColor = simulation->getBackgroundSurface()->color;
    renderingInstructions(visParams);
  }
  graphicsManager->endFPSCount(fbo_reg);
}
