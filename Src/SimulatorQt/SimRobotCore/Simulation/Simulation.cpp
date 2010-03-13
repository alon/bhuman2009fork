/**
* @file Simulation/Simulation.cpp
*
* Implementation of class Simulation
*
* @author <A href="mailto:Tim.Laue@dfki.de">Tim Laue</A>
* @author <A href="mailto:kspiess@informatik.uni-bremen.de">Kai Spiess</A>
* @author <A href="mailto:pachur@informatik.uni-bremen.de">Dennis Pachur</A>
*/

#include <iostream>

#include <math.h>

#include "Simulation.h"
#include "Actuatorport.h"
#include "../Platform/OffscreenRenderer.h"
#include "../Platform/System.h"
#include "../Controller/View.h"
#include "../Actuators/Actuator.h"
#include "../Actuators/Joint.h"
#include "../Actuators/Led.h"
#include "../Parser/SAX2Parser.h"
#include "../PhysicalObjects/PhysicalObject.h"
#include "../PhysicalObjects/ComplexShape.h"
#include "../Sensors/Bumper.h"
#include "../Sensors/InteractiveButton.h"
#include "../Tools/Debug.h"
#include "../OpenGL/Constants.h"

Simulation::Simulation(): 
currentMacro(0), objectTree(0), graphicsManager(0), timeOfLastFrameRateComputation(0), 
stepLength(1.0), resetFlag(false), simulationReady(false), offscreenRenderer(0), 
backgroundSurface(0), defaultSurface(0), physicsParameters(0), physicalWorld(0),
staticSpace(0), movableSpace(0), contactGroup(0), rootSpace(0), 
currentSpace(0), intersectionRay(0), physicsStepLength(-1.0),  useQuickSolverForPhysics(false), 
numberOfIterationsForQuickSolver (20), useAutomaticObjectDisabling(false), 
collisionDetectionMode(0), numberOfCollisionContacts(0), applyDynamicsForceFactor(1.0)
{
  simulationStatus = NOT_LOADED;
  highestMovableID = 0;
  movableIDs.push(highestMovableID);
  motionBlurMode = MOTIONBLUR_OFF;

  showStatistics = false;
  saveCubeMap = false;
  dInitODE();
}

void Simulation::setStepLength(double stepLength)
{
  this->stepLength = stepLength;
}

void Simulation::updateFrameRate()
{
  unsigned long currentTime(System::getTime());
  unsigned long timeDiff(currentTime - timeOfLastFrameRateComputation);
  //Only update frame rate once in two seconds
  if(timeDiff > 2000)
  {
    int frameDiff(simulationStep - stepAtLastFrameRateComputation);
    double frameRateDouble(double(frameDiff)/(double(timeDiff)/1000.0));
    currentFrameRate = int(floor(frameRateDouble+0.5));
    timeOfLastFrameRateComputation = currentTime;
    stepAtLastFrameRateComputation = simulationStep;
  }
}

void Simulation::addMacro(const std::string& name, SimObject* macro)
{
  macroMap[name] = macro;
}

bool Simulation::instantiateMacro(const std::string& macroName,
                                  const std::string& instanceName,
                                  SimObject* parentNode)
{
  NameToPointerMap::const_iterator pos;
  pos = macroMap.find(macroName);
  SimObject* macro;
  bool returnValue;
  if(pos != macroMap.end())
  {
    macro = pos->second;
    returnValue = true;
  }
  else
  {
    //Insert dummy object to allow the parser to continue
    macro = new SimObject();
    returnValue = false;
  }
  if(currentMacro != 0)
    macroStack.push(currentMacro);
  currentMacro = macro->clone();
  currentMacro->setName(instanceName != "" ? instanceName : macroName);
  parentNode->addChildNode(currentMacro, true);
  return returnValue;
}

void Simulation::addCurrentMacroInstanceToObjectTree(SimObject* parent)
{
  //Remove macro from its parent node:
  parent->getPointerToChildNodes()->pop_back();
  //Add the macro child nodes instead:
  unsigned int numOfChildNodes(currentMacro->getNumberOfChildNodes());
  if(numOfChildNodes == 1)
    currentMacro->getChildNode(0)->setName(currentMacro->getName());
  SimObject* childNode;
  for(unsigned int i=0; i<numOfChildNodes; i++)
  {
    childNode = currentMacro->getChildNode(i);
    parent->addChildNode(childNode, false);
  }
  //Delete the macro instance
  currentMacro->getPointerToChildNodes()->clear();
  delete currentMacro;
  if(macroStack.size())
  {
    currentMacro = macroStack.top();
    macroStack.pop();
  }
  else
  {
    currentMacro = 0;
  }
}

SimObject* Simulation::getNodeInMacro(const std::string& name) const
{
  if(name == "")
  {
    if(currentMacro->getNumberOfChildNodes())
      return currentMacro->getChildNode(0);
    else
      return currentMacro;
  }
  else
  {
    SimObject* node;
    currentMacro->findObject(node, name, false);
    return node;
  }
}

void Simulation::setBackgroundColor(const Vector3d& color)
{
  backgroundSurface->setColor((GLfloat)color.v[0], (GLfloat)color.v[1],
    (GLfloat)color.v[2], 1.0f);
}

void Simulation::renderCubeMap(const Vector3d& cameraPos,
                               const Vector3d& cameraTarget,
                               const Vector3d& cameraUpVector,
                               VisualizationParameterSet& visParams,
                               SimObject* drawingObject)
{
  // cube map has only to be build if a reflective object is chosen
  if(visParams.surfaceStyle != VisualizationParameterSet::CAMERA_SIMULATION ||
    (!graphicsManager->doEnvRendering() && !saveCubeMap))
    return;  

  // set clear color once
  glClearColor(backgroundSurface->color[0], backgroundSurface->color[1],
      backgroundSurface->color[2], backgroundSurface->color[3]);

  // cube map rendering
  for(int generateCubeMap = 6; generateCubeMap > 0; generateCubeMap--)
  {
    glClear(default_clear);

    graphicsManager->setCubeMapPerspective();
    setCamera(cameraPos, cameraTarget, cameraUpVector, visParams.snapToRoot, drawingObject, generateCubeMap);

    graphicsManager->updateLights(true, false, visParams.provideUniforms);
    graphicsManager->renderEnvironment(500.0);

    // prepare surface properties
    glCallList(graphicsManager->enableStateDL[visParams.surfaceStyle]);
    drawingObject->draw(visParams);
    glCallList(graphicsManager->disableStateDL[visParams.surfaceStyle]);

    graphicsManager->revertLightTextureUnits(visParams.provideUniforms);
    glFlush();

    // save image to cubemap file
    graphicsManager->handleCubeMapping(generateCubeMap, filename);
  }
  saveCubeMap = false;
}

void Simulation::draw(const Vector3d& cameraPos,
                      const Vector3d& cameraTarget,
                      const Vector3d& cameraUpVector,
                      unsigned int width, unsigned int height, double fovy,
                      VisualizationParameterSet& visParams,
                      const bool updateGluParameters,
                      SimObject* drawingObject,
                      const int fbo_reg)
{
  graphicsManager->beginFPSCount(fbo_reg);

  // get the motion blur mode
  MotionBlurMode mode = (visParams.surfaceStyle == VisualizationParameterSet::CAMERA_SIMULATION) ?
    getMotionBlurMode() : MOTIONBLUR_OFF;
  if(mode != MOTIONBLUR_OFF)
  {
    graphicsManager->setCamMBInfos(0, 0, -1);
    if(mode == MOTIONBLUR_COMBINED_METHOD)
    {
      clearColor = blackNoAlpha;
      graphicsManager->shader->setRunning(false);
      renderingInstructions(cameraPos, cameraTarget, cameraUpVector,
                            width, height, fovy, updateGluParameters,
                            visParams, drawingObject, fbo_reg,
                            MOTIONBLUR_12PASSES, MOTIONBLUR_VELOCITY_BUFFER_2);
      graphicsManager->shader->copyFramebufferToTexture(fbo_reg);
      graphicsManager->shader->setRunning(true);
      clearColor = grey;
      renderingInstructions(cameraPos, cameraTarget, cameraUpVector,
                            width, height, fovy, updateGluParameters,
                            visParams, drawingObject, fbo_reg,
                            MOTIONBLUR_VELOCITY_BUFFER_2, MOTIONBLUR_12PASSES);
    }
    else if(mode == MOTIONBLUR_COMBINED_METHOD_2)
    {
      clearColor = blackNoAlpha;
      graphicsManager->shader->setGlobalShader(std::string("Accumulation Buffer"), true);
      graphicsManager->shader->setPostShader(std::string("Post Accumulation Buffer"), true);
      visParams.finishRendering = false;
      renderingInstructions(cameraPos, cameraTarget, cameraUpVector,
                            width, height, fovy, updateGluParameters,
                            visParams, drawingObject, fbo_reg,
                            MOTIONBLUR_ACCUMULATION, MOTIONBLUR_VELOCITY_BUFFER_2);

      clearColor = grey;
      graphicsManager->shader->setGlobalShader(std::string("Velocity Buffer 2"), true);
      graphicsManager->shader->setPostShader(std::string("Post Velocity Buffer Comb 2"), true);
      visParams.finishRendering = true;
      renderingInstructions(cameraPos, cameraTarget, cameraUpVector,
                            width, height, fovy, updateGluParameters,
                            visParams, drawingObject, fbo_reg,
                            MOTIONBLUR_VELOCITY_BUFFER_2, MOTIONBLUR_12PASSES);
    }
    else
    {
      if(mode == MOTIONBLUR_VELOCITY_BUFFER || mode == MOTIONBLUR_VELOCITY_BUFFER_2)
        clearColor = grey;
      else
        clearColor = backgroundSurface->color;
      renderingInstructions(cameraPos, cameraTarget, cameraUpVector,
                            width, height, fovy, updateGluParameters, 
                            visParams, drawingObject, fbo_reg, mode);
    }
  }
  else
  {
    clearColor = backgroundSurface->color;
    renderingInstructions(cameraPos, cameraTarget, cameraUpVector,
                          width, height, fovy, updateGluParameters, 
                          visParams, drawingObject, fbo_reg, mode);
  }

  graphicsManager->endFPSCount(fbo_reg);
  
  // additional messages
  if(showStatistics)
  {
    GLHelper::getGLH()->glPrintText(TextObject::COMMON_HINTS, TextObject::WHITE, 1000, "Total number of defined primitives: %u",
      graphicsManager->getNumOfTotalPrimitives());
    GLHelper::getGLH()->glPrintText(TextObject::COMMON_HINTS, TextObject::WHITE, 1000, "Total number of defined vertices: %u",
      graphicsManager->getNumOfTotalVertices());
    if(selection.object)
    {
      if(selection.numPrim == 0)
        selection.numPrim = selection.object->getNumOfPrimitives();
      GLHelper::getGLH()->glPrintText(TextObject::COMMON_HINTS, TextObject::WHITE, 1000, "Number of instanced object primitives: %u",
        selection.numPrim);
      if(selection.numVert == 0)
        selection.numVert = selection.object->getNumOfVertices();
      GLHelper::getGLH()->glPrintText(TextObject::COMMON_HINTS, TextObject::WHITE, 1000, "Number of instanced object vertices: %u",
        selection.numVert);
    }
  }
    
  // render text messages
  GLHelper::getGLH()->renderText(width, height);
  graphicsManager->shader->renderInfos(width, height);
}

void Simulation::renderingInstructions(
  const Vector3d& cameraPos, const Vector3d& cameraTarget, const Vector3d& cameraUpVector,
  unsigned int width, unsigned int height, double fovy, const bool& updateGluParameters,
  VisualizationParameterSet& visParams, SimObject* drawingObject, const int fbo_reg,
  MotionBlurMode mode, MotionBlurMode skip)
{
  // get some render parameters
  visParams.mb_mode = mode;
  visParams.mb_skip = skip;
  bool useShadows = visParams.surfaceStyle == VisualizationParameterSet::TEXTURE_SHADING ||
                    visParams.surfaceStyle == VisualizationParameterSet::CAMERA_SIMULATION;
  bool skipFirstChild = (visParams.surfaceStyle != VisualizationParameterSet::CAMERA_SIMULATION &&
                         drawingObject == objectTree && graphicsManager->getEnvironment() != NULL);
  graphicsManager->getShaderUsage(visParams);
  glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);

  // render shadows
  drawShadowMaps(drawingObject, visParams);
  if(visParams.limitToRenderPass > 0)
    return;

  // render cube map
  renderCubeMap(cameraPos, cameraTarget, cameraUpVector, visParams, drawingObject);

  // prepare rendering
  graphicsManager->prepareRendering(visParams, width, height,
    (GLdouble)width/(GLdouble)height, fovy, 0.3, 500.0, fbo_reg);

  // get motion blur values and clear view
  graphicsManager->setUpMotionBlurSettings(visParams);

  // do the motion blur render passes (if motion blur is off -> 1 pass)
  for(; visParams.mb_currentPass <= visParams.mb_renderPasses; visParams.mb_currentPass++)
  {
    // Set the view
    graphicsManager->setPerspective();
    setCamera(cameraPos, cameraTarget, cameraUpVector, visParams.snapToRoot, drawingObject);
    glClear(default_clear);

    // Prepare glu-parameters for simple object selection
    if(updateGluParameters)
      prepareGluParameters();

     // update light positions
    graphicsManager->updateLights(useShadows, visParams.renderPasses == 2, visParams.provideUniforms);

    // prepare surface properties
    graphicsManager->renderEnvironment(30.0);
    glCallList(graphicsManager->enableStateDL[visParams.surfaceStyle]);

    // render
    if(drawingObject == objectTree)
      drawPlanes(visParams);
    
    drawingObject->draw(visParams, skipFirstChild);
    if(visParams.renderPasses == 2)
    {
      graphicsManager->updateLights(useShadows, false, visParams.provideUniforms);
      drawingObject->draw(visParams, skipFirstChild);
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
    graphicsManager->finishRendering(visParams, true);
  }

  // write back result if accum was used
  if(visParams.mb_useAccum)
    glAccum(GL_RETURN, 1.0f);

  glFlush();
}

void Simulation::setCamera(const Vector3d& cameraPos, const Vector3d& cameraTarget,
                           const Vector3d& cameraUpVector, const bool& snapToRoot,
                           const SimObject* drawingObject, const int& generateCubeMap)
{
  // calculate camera
  Vector3d newCamPos,
    newCamTarget,
    newCamUpVector;
  if(generateCubeMap > 0)
  {
    graphicsManager->setCubeMapCamera(generateCubeMap, graphicsManager->reflectionObject ?
      graphicsManager->reflectionObject->getPosition() : Vector3d());
    return;
  }
  else if(snapToRoot && drawingObject)
  {
    newCamPos = cameraPos;
    newCamTarget = drawingObject->getPosition();
    newCamPos -= cameraTarget;
    newCamPos += newCamTarget;
    newCamUpVector = cameraUpVector;
  }
  else
  {
    newCamPos = cameraPos;
    newCamTarget = cameraTarget;
    newCamUpVector = cameraUpVector;
  }

  //Set view
  gluLookAt(newCamPos.v[0], newCamPos.v[1], newCamPos.v[2],
    newCamTarget.v[0], newCamTarget.v[1], newCamTarget.v[2],
    newCamUpVector.v[0], newCamUpVector.v[1], newCamUpVector.v[2]);
}

void Simulation::drawShadowMaps(SimObject* rootObject, VisualizationParameterSet& visParams)
{
  if(visParams.limitToRenderPass > graphicsManager->getNumLightsOn(true))
    visParams.limitToRenderPass = 0;
  // prepare light sources and draw scene
  if(visParams.surfaceStyle != VisualizationParameterSet::TEXTURE_SHADING &&
    visParams.surfaceStyle != VisualizationParameterSet::CAMERA_SIMULATION)
  {
    graphicsManager->reenableLights();
    return;
  }
  visParams.currentRenderPass = 0;
  while(graphicsManager->prepareLightForShadowMapping(visParams))
  {
    if(visParams.limitToRenderPass >0)
    {
      visParams.currentRenderPass++;
      if(visParams.currentRenderPass == visParams.limitToRenderPass)
      {
        glColorMask(1, 1, 1, 1);
        glClear(GL_COLOR_BUFFER_BIT);
        rootObject->quickDraw();
        GLHelper::getGLH()->glPrintText(TextObject::COMMON_HINTS, TextObject::WHITE, 100, "Current view is limited to shadowmap rendering of light no %i.", 
          visParams.currentRenderPass);
        GLHelper::getGLH()->renderViewportBorder(graphicsManager->getCurrentShadowMapSize(),
          graphicsManager->getCurrentShadowMapSize());
        GLHelper::getGLH()->renderText(graphicsManager->getCurrentShadowMapSize(),
          graphicsManager->getCurrentShadowMapSize());
        glFlush();
        graphicsManager->resetShadowMapIter();
        return;
      }
    }
    else
      rootObject->quickDraw();
  }
}

void Simulation::drawPlanes(const VisualizationParameterSet& visParams)
{
  std::vector<Plane*>::iterator iter;
  for(iter = planes.begin(); iter != planes.end(); ++iter)
  {
    (*iter)->drawObject(visParams);
  }
}

void Simulation::toggleLightShowCones()
{
  graphicsManager->toggleLightShowCones();
}

void Simulation::camToLightPos(Vector3d& cameraPos, Vector3d& cameraTarget)
{
  graphicsManager->camToLightPos(cameraPos, cameraTarget);
}

void Simulation::fitCamera(Vector3d& cameraTarget, Vector3d& cameraPos, int width, int height,
                           SimObject* drawingObject)
{
  //Check size of view before computing anything:
  if((width == 0) || (height == 0))
    return;

  //Determine the center of the scene (the first object in tree):
  Vector3d center(drawingObject->getPosition());
  //Compute box around object:
  AxisAlignedBoundingBox box(drawingObject->getPosition());
  drawingObject->expandAxisAlignedBoundingBox(box);
  Vector3d boxPoints[8];
  box.getCorners(boxPoints);
  GLdouble mx, my, mz;
  double nearestx(0.0), nearesty(0.0), scalex, scaley;
  for(unsigned int i=0; i<8; i++)
  {
    gluProject(boxPoints[i].v[0], boxPoints[i].v[1], boxPoints[i].v[2],
      graphicsManager->mvmatrix, graphicsManager->projmatrix, graphicsManager->viewport,
      &mx, &my, &mz);
    scalex = fabs((mx-width)/width);
    scaley = fabs((my-height)/height);
    nearestx = (scalex > nearestx && scalex < 10) ? scalex : nearestx;
    nearesty = (scaley > nearesty && scaley < 10) ? scaley : nearesty;
  }

  Vector3d viewingvec = cameraTarget - cameraPos;
  viewingvec *= (nearestx > nearesty) ? nearestx : nearesty;
  cameraPos = cameraTarget - viewingvec;
}

void Simulation::enableStateDL(VisualizationParameterSet::SurfaceStyle style)
{
  glCallList(graphicsManager->enableStateDL[style]);
}

void Simulation::disableStateDL(VisualizationParameterSet::SurfaceStyle style)
{
  glCallList(graphicsManager->disableStateDL[style]);
}

void Simulation::precomputeSensorValues()
{
  std::vector<SensorPort>::iterator port;
  for(port = sensorPortList.begin(); port != sensorPortList.end(); ++port)
  {
    if(port->sensor->getKind() != "camera") //HACK, please improve
      port->sensor->getSensorReading(port->localPortId);
  }
}

void Simulation::prepareGluParameters()
{
  // GLU parameters for object picking
  glGetDoublev(GL_MODELVIEW_MATRIX, graphicsManager->mvmatrix);
  glGetDoublev(GL_PROJECTION_MATRIX, graphicsManager->projmatrix);
  glGetIntegerv(GL_VIEWPORT, graphicsManager->viewport);
}

void Simulation::gluProjectClick(int x, int y, Vector3d& vecFar, Vector3d& vecNear)
{
  GLdouble tx, ty, tz;

  gluUnProject((GLdouble)(x),(GLdouble)(selection.height - y), 1.0,
    graphicsManager->mvmatrix, graphicsManager->projmatrix, graphicsManager->viewport,
    &tx, &ty, &tz);

  vecFar.v[0] = tx;
  vecFar.v[1] = ty;
  vecFar.v[2] = tz;

  gluUnProject((GLdouble)(x),(GLdouble)(selection.height - y), 0.0,
    graphicsManager->mvmatrix, graphicsManager->projmatrix, graphicsManager->viewport,
    &tx, &ty, &tz);

  vecNear.v[0] = tx;
  vecNear.v[1] = ty;
  vecNear.v[2] = tz;
}

InteractiveSelectionType Simulation::selectObject(int x, int y,
                                                  const Vector3d& cameraPos, const Vector3d& cameraTarget, const Vector3d& cameraUpVector,
                                                  int width, int height, DragAndDropPlane plane, DragAndDropMode dragMode,
                                                  SimObject* simObject, bool selectButtons)
{
  //Prepare object selection:
  selection.cameraPos = cameraPos;
  selection.cameraTarget = cameraTarget;
  selection.cameraUpVector = cameraUpVector;
  selection.width = width;
  selection.height = height;
  selection.root = simObject;
  switch(plane)
  {
  case XY_PLANE:
    selection.plane = Vector3d(0,0,1);
    break;
  case XZ_PLANE:
    selection.plane = Vector3d(0,1,0);
    break;
  case YZ_PLANE:
    selection.plane = Vector3d(1,0,0);
    break;
  }
  Vector3d vecFar,
    vecNear;

  gluProjectClick(x, y, vecFar, vecNear);

  // Do intersection test:
  std::vector<Intersection*> intersections;
  AxisAlignedBoundingBox dummyBox(selection.root->getPosition());
  selection.root->expandAxisAlignedBoundingBox(dummyBox);
  bool result = selection.root->intersect(intersections, vecNear, vecFar);

  //Check, if any objects have been selected
  if(result)
  {
    //Find all intersected movables and interactive objects
    std::vector<Intersection*> intersectedInteractives;
    for(unsigned int j=0; j<intersections.size(); j++)
    {
      SimObject* intersectedObject;
      intersectedObject = intersections[j]->object;
      SimObject* pointerToInteractive = 0;
      //Don't select the root object of a view
      if(intersectedObject->isMovableOrClickable(pointerToInteractive,selectButtons) && selection.root != pointerToInteractive)
      {
        intersectedInteractives.push_back(intersections[j]);
      }
    }
    result = intersectedInteractives.size() != 0;
    if(result)
    {
      //Find nearest intersection:
      double shortestDistance = (intersectedInteractives[0]->position - selection.cameraPos).getLength();
      double idxShortest = 0;
      unsigned int k;
      for(k=1; k<intersectedInteractives.size(); k++)
      {
        double dist = (intersectedInteractives[k]->position - selection.cameraPos).getLength();
        if(dist<shortestDistance)
        {
          shortestDistance = dist;
          idxShortest = k;
        }
      }
      SimObject* dummyObject = intersectedInteractives[(int) idxShortest]->object;
      dummyObject->isMovableOrClickable(selection.object,selectButtons);
      selection.object = findRootOfMovableObject(selection.object);
      PhysicalObject* physObj = selection.object->castToPhysicalObject();
      if(physObj)
      {
        if(physObj->isGeometryTransformed())
        {
          if(compoundObjects.find(physObj->getMovableID()) != compoundObjects.end())
            compoundObjects[physObj->getMovableID()]->invertColors();
        }
        else
        {
          selection.object->invertColors();
        }
      }
      else
        selection.object->invertColors();
      Vector3d planeIntersection;
      Vector3d raydirection;
      raydirection = vecFar-vecNear;
      raydirection.normalize();
      SimGeometry::intersectRayAndPlane(vecNear, raydirection,
        selection.object->getPosition(),
        selection.plane, planeIntersection);
      selection.clickOffset = planeIntersection - selection.object->getPosition();
      GLHelper::getGLH()->glPrintText(TextObject::ALL_HINTS, TextObject::WHITE, 2000,
        "Selected object \"%s\".", selection.object->getName().c_str());
    }

    //Delete all intersection objects and clear vectors:
    for(unsigned int k=0; k<intersections.size(); k++)
      delete intersections[k];
    intersections.clear();
    intersectedInteractives.clear();
    if(result)
    {
      if(selection.object->getKind() == "interactiveButton")
      {
        selection.object->hasBeenClicked();
        return BUTTON_SELECTION;
      }
      else if(selection.object->isMovable())
      {
        bool disableGeometry = true;
        if(dragMode == APPLY_DYNAMICS)
        {
          disableGeometry = false;
          if(this->useAutomaticObjectDisabling)
            this->enablePhysics(selection.object);
        }

        this->disablePhysics(selection.object, disableGeometry);
        return OBJECT_SELECTION;
      }
    }
  }
  return NO_SELECTION;
}

void Simulation::unselectObject()
{
  PhysicalObject* physObj = selection.object->castToPhysicalObject();
  if(physObj)
  {
    if(physObj->isGeometryTransformed())
    {
      if(compoundObjects.find(physObj->getMovableID()) != compoundObjects.end())
        compoundObjects[physObj->getMovableID()]->invertColors();
    }
    else
    {
      selection.object->invertColors();
    }
  }
  else
  {
    selection.object->invertColors();
  }
  selection.object->hasBeenReleased();
  enablePhysics(selection.object);
  selection.object = 0;
  selection.numPrim = 0;
  selection.numVert = 0;
}

void Simulation::translateObject(int x, int y)
{
  std::map<int,CompoundPhysicalObject*>::iterator iter;
  for(iter = compoundObjects.begin(); iter != compoundObjects.end(); ++iter)
  {
    (*iter).second->setSynchronized(false);
  }

  assert(selection.object);
  Vector3d vecFar,
    vecNear;
  gluProjectClick(x, y, vecFar, vecNear);
  Vector3d planeIntersection;
  if(SimGeometry::intersectRayAndPlane(vecNear, vecFar, selection.object->getPosition(),
    selection.plane, planeIntersection))
  {
    Vector3d translationVec = planeIntersection - selection.object->getPosition() - selection.clickOffset;
    PhysicalObject* physObj = selection.object->castToPhysicalObject();
    if(physObj)
    {
      if(compoundObjects.find(physObj->getMovableID()) != compoundObjects.end())
      {
        if(physObj->getCompoundPhysicalObject()->isSimple())
        {
          selection.object->translateDnD(translationVec);
        }
        else
        {
          CompoundPhysicalObject* compoundObj = physObj->getCompoundPhysicalObject();
          compoundObj->translateDnD(translationVec, true);
        }
      }
      else
      {
        selection.object->translateDnD(translationVec);
      }
    }
    else
    {
      selection.object->translateDnD(translationVec);
    }
  }
}

void Simulation::rotateObject(int x, int y)
{
  std::map<int,CompoundPhysicalObject*>::iterator iter;
  for(iter = compoundObjects.begin(); iter != compoundObjects.end(); ++iter)
  {
    (*iter).second->setSynchronized(false);
  }

  assert(selection.object);
  Vector3d vecFar,
    vecNear;
  gluProjectClick(x, y, vecFar, vecNear);
  Vector3d planeIntersection;
  SimGeometry::intersectRayAndPlane(vecNear, vecFar, selection.object->getPosition(),
    selection.plane, planeIntersection);
  Vector3d clickOffset = planeIntersection - selection.object->getPosition();

  //if compound object
  PhysicalObject* physObj = selection.object->castToPhysicalObject();
  if(physObj)
  {
    if(physObj->isGeometryTransformed())
    {
      int movID = physObj->getMovableID();
      if(movID != 0)
      {
        if(compoundObjects.find(movID) != compoundObjects.end())
        {
          CompoundPhysicalObject* compObj = this->compoundObjects[movID];
          clickOffset = planeIntersection - compObj->getPosition();
        }
      }
    }
  }

  double angle;

  if(selection.plane.v[0])
    angle = atan2(clickOffset.v[2], clickOffset.v[1]) -
    atan2(selection.clickOffset.v[2], selection.clickOffset.v[1]);
  else if(selection.plane.v[1])
    angle = atan2(clickOffset.v[0], clickOffset.v[2]) -
    atan2(selection.clickOffset.v[0], selection.clickOffset.v[2]);
  else
    angle = atan2(clickOffset.v[1], clickOffset.v[0]) -
    atan2(selection.clickOffset.v[1], selection.clickOffset.v[0]);

  //if compound object
  if(physObj)
  {
    if(compoundObjects.find(physObj->getMovableID()) != compoundObjects.end())
    {
      if(physObj->getCompoundPhysicalObject()->isSimple())
      {
        selection.object->rotateAroundAxisDnD(angle, selection.plane);
      }
      else
      {
        CompoundPhysicalObject* compoundObj = physObj->getCompoundPhysicalObject();
        compoundObj->rotateAroundAxisDnD(angle, selection.plane, true);
      }
    }
    else
    {
      selection.object->rotateAroundAxisDnD(angle, selection.plane);
    }
  }
  else
  {
    selection.object->rotateAroundAxisDnD(angle, selection.plane);
  }
  selection.clickOffset = clickOffset;
}

bool Simulation::loadFile(const std::string& filename, const std::string& schemaName)
{
  this->filename = filename;
  this->schemaName = schemaName;
  closeSimulation();

  graphicsManager = new GraphicsManager(filename);

  //setup physics
  physicsParameters = new PhysicsParameterSet();
  physicalWorld = dWorldCreate();
  rootSpace = dHashSpaceCreate(0);
  intersectionRay = dCreateRay(0, 1.0);
  staticSpace = dHashSpaceCreate(rootSpace);
  movableSpace = dHashSpaceCreate(rootSpace);
  contactGroup = dJointGroupCreate(0);
  currentSpace = &staticSpace;

  parser = new SAX2Parser();
  parser->parse(filename, schemaName, this, objectTree, &errorManager,
    graphicsManager, &surfaces, &materials, &environments);
  delete parser;

  if(useQuickSolverForPhysics)
    dWorldSetQuickStepNumIterations(physicalWorld, numberOfIterationsForQuickSolver);
  offscreenRenderer = new OffscreenRenderer(graphicsManager);

  if(objectTree)
  {
    postprocessObjectTree(objectTree,"");
    collectObjectsAndMovables(objectTree);
    simulationStep = 0;
    currentFrameRate = 0;
    stepAtLastFrameRateComputation = 0;
    for(unsigned int i=0; i<actuatorList.size(); i++)
    {
      actuatorList[i]->act(true);
    }
    simulationStatus = SCENE_LOADED;

    // setup physical parameters for the world
    dWorldSetGravity(physicalWorld, 0,0,(dReal)(physicsParameters->getGravity()));
    dWorldSetERP(physicalWorld, (dReal)(physicsParameters->getErrorReductionParameter()));
    dWorldSetCFM(physicalWorld, (dReal)(physicsParameters->getConstraintForceMixing()));

    //setup parameters for automatic physical objetc disabling, if used
    if(useAutomaticObjectDisabling)
    {
      dWorldSetAutoDisableFlag(physicalWorld, 1);
      dWorldSetAutoDisableLinearThreshold(physicalWorld, (dReal)autoObjectDisableParameters.linearVelocityThreshold); 
      dWorldSetAutoDisableAngularThreshold(physicalWorld,(dReal)autoObjectDisableParameters.angularVelocityThreshold); 
      if(autoObjectDisableParameters.physicsSimulationSteps > -1)
      {
        dWorldSetAutoDisableSteps(physicalWorld, autoObjectDisableParameters.physicsSimulationSteps);
      }
      if(autoObjectDisableParameters.physicsSimulationTime > -1.0)
      {
        dWorldSetAutoDisableTime(physicalWorld,(dReal)autoObjectDisableParameters.physicsSimulationTime);
      }
    }

    // create physical objects: geometry, body, joint, motor
    createPlanePhysics();
    createStaticObjectPhysics(objectTree);
    createCompoundPhysicalObjects(objectTree);
    createCompoundObjectPhysics();
    createJointAndMotorPhysics(objectTree);
    // collision sensors
    collectCollisionSensors();
    sceneGraphChanged = true;
    simulationReady = true;
    return true;
  }
  else
  {
    delete offscreenRenderer;
    offscreenRenderer = 0;
    delete graphicsManager;
    graphicsManager = 0;
    sceneGraphChanged = false;
    return false;
  }
}

void Simulation::closeSimulation()
{
  simulationReady = false;
  {
    std::list<View*>::iterator iter;
    for(iter = viewList.begin(); iter != viewList.end(); ++iter)
      delete (*iter);
    viewList.clear();
  }
  {
    std::vector<Surface*>::iterator iter;
    for(iter = surfaces.begin(); iter != surfaces.end(); ++iter)
    {
      if((*iter) == backgroundSurface)
        backgroundSurface = 0;
      delete (*iter);
    }
    surfaces.clear();
    if(backgroundSurface)
    {
      delete backgroundSurface;
      backgroundSurface = 0;
    }
  }
  {
    std::vector<Environment*>::iterator iter;
    for(iter = environments.begin(); iter != environments.end(); ++iter)
      delete (*iter);
    environments.clear();
  }
  {
    std::vector<Material*>::iterator iter;
    for(iter = materials.begin(); iter != materials.end(); ++iter)
      delete (*iter);
    materials.clear();
  }
  {
    std::vector<Actuatorport*>::iterator iter;
    for(iter = actuatorportList.begin(); iter != actuatorportList.end(); ++iter)
      delete (*iter);
    actuatorportList.clear();
  }
  {
    sensorPortList.clear();
  }
  {
    NameToPointerMap::iterator iter;
    for(iter = macroMap.begin(); iter != macroMap.end(); ++iter)
      delete iter->second;
    macroMap.clear();
  }
  actuatorList.clear();
  if(objectTree)
  {
    delete objectTree;
    objectTree = 0;
  }
  simulationStatus = NOT_LOADED;
  selection.object = 0;
  if(graphicsManager)
  {
    delete graphicsManager;
    graphicsManager = 0;
  }
  if(offscreenRenderer)
  {
    delete offscreenRenderer;
    offscreenRenderer = 0;
  }
  {
    std::vector<Plane*>::iterator iter;
    for(iter = planes.begin(); iter != planes.end(); ++iter)
      delete (*iter);
    planes.clear();
  }
  //clean up physics
  {
    collisionSensors.clear();
  }
  if(!compoundObjects.empty())
  {
    std::map<int, CompoundPhysicalObject*>::const_iterator compoundObjIter;
    for(compoundObjIter = compoundObjects.begin(); compoundObjIter != compoundObjects.end(); ++compoundObjIter)
    {
      delete compoundObjIter->second;
    }
    compoundObjects.clear();
  }
  if(physicsParameters)
  {
    delete physicsParameters;
    physicsParameters= 0;
  }
  if(contactGroup)
  {
    dJointGroupDestroy(contactGroup);
    contactGroup = 0;
  }
  currentSpace = 0;
  if(movableSpace)
  {
    dSpaceDestroy(movableSpace);
    movableSpace = 0;
  }
  if(staticSpace)
  {
    dSpaceDestroy(staticSpace);
    staticSpace = 0;
  }
  if(rootSpace)
  {
    dSpaceDestroy(rootSpace);
    rootSpace = 0;
  }
  if(intersectionRay)
  {
    dGeomDestroy(intersectionRay);
    intersectionRay = 0;
  }
  if(physicalWorld)
  {
    dWorldDestroy(physicalWorld);
    physicalWorld = 0;
  }
  // Some remaining stuff, needed for reset from controller:
  objectList.clear();
  movableMap.clear();
  objectMap.clear();
  while(!movableIDs.empty())
    movableIDs.pop();
  highestMovableID = 0;
  movableIDs.push(highestMovableID);
}

void Simulation::resetSimulation()
{
  resetFlag = false;
  // loadFile
  if(loadFile(filename, schemaName))
  {
    //Update views
    std::list<View*>::const_iterator pos;
    for(pos = viewList.begin(); pos != viewList.end(); ++pos)
    {
      (*pos)->update();
    }
  }
}

void Simulation::readWriteNaoqi()
{
	// TODO
}

void Simulation::doSimulationStep()
{
#define NH 22
	{
		static double val = 0.2;
		static int count = 0;
		static int hinge_indices[NH];
		static bool inited_indices = false;
		count++;
		if (count % 100 == 0) val = 1.0 - val;
		//Let actuators manipulate the scene:
		if (!inited_indices) {
			std::cout << "Total sensors " << sensorPortList.size() << "\nHinges: ";
			int count_hinge = 0;
			for (unsigned int i=0; i<sensorPortList.size(); ++i)
				if (sensorPortList[i].sensor->getKind() == "hinge") {
					if (count_hinge < NH)
						hinge_indices[count_hinge++] = i;
					std::cout << i << ": " << sensorPortList[i].sensor->getName() << ", ";
				}
			std::cout << "\nHinge count: " << count_hinge << "\nActuator# " << actuatorList.size() << ": ";
			inited_indices = true;
		}
		std::cout << "Hinges position: ";
		for (unsigned int i=0; i<NH; ++i) {
			Sensor* sensor = sensorPortList[hinge_indices[i]].sensor;
			SensorReading& r = sensor->getSensorReading(0 /* port */);
			std::cout << r.data.doubleValue << ", ";
		}
		for(unsigned int i=0; i<actuatorList.size(); i++)
		{
			//std::cout << actuatorList[i]->getValue() << "->";
			actuatorList[i]->setValue(val, 0 /*port*/);
			actuatorList[i]->act();
			//std::cout << actuatorList[i]->getValue() << "; ";
		}
		std::cout << "\n";
	}
#undef NH
  //PHYSICS SIMULATION
  int physicsStepsPerSimStep = (int)(stepLength / physicsStepLength);
  double physicsSimulationStepsize = stepLength/physicsStepsPerSimStep;
  for(int i=0; i < physicsStepsPerSimStep; i++)
  {
    numberOfCollisionContacts = 0;
    applyJointFriction();
    //reset collision sensors
    if(!collisionSensors.empty())
      resetCollisionSensors();
    //collision detection
    dSpaceCollide(rootSpace, this, staticCollisionCallback);
    if(useQuickSolverForPhysics)
    {
      dWorldQuickStep(physicalWorld, (dReal)physicsSimulationStepsize);
    }
    else
    {
      dWorldStep(physicalWorld, (dReal)physicsSimulationStepsize);
    }
    //empty collision joints from collision detection
    dJointGroupEmpty(contactGroup);
  }
  //write back physical results from body/geometry to simulation object
  writeBackPhysicalResults(objectTree, motionBlurMode != MOTIONBLUR_OFF);

  //Update views
  std::list<View*>::const_iterator pos;
  for(pos = viewList.begin(); pos != viewList.end(); ++pos)
  {
    (*pos)->update();
  }

  //Set next step
  ++simulationStep;
  sceneGraphChanged = false;
  updateFrameRate();
}

SimObject* Simulation::getObjectReference(const std::string& objectName, bool fullName)
{
  if(objectName == "")
    return objectTree;

  NameToPointerMap::const_iterator pos;
  pos = objectMap.find(objectName);
  if(pos == objectMap.end())
  {
    SimObject* newObject = 0;
    objectTree->findObject(newObject, objectName, fullName);
    if(newObject)
      objectMap[objectName] = newObject;
    return newObject;
  }
  else
  {
    return pos->second;
  }
}

int Simulation::getActuatorportId(const std::vector<std::string>& parts) const
{
  int result = -1;
  for(unsigned int i=0; i<actuatorportList.size(); i++)
  {
    const std::string& currentName(actuatorportList[i]->getName());
    std::string::size_type index = 0;
    for(unsigned j = 0; j < parts.size(); ++j)
    {
      index = currentName.find(parts[j], index);
      if(index == std::string::npos)
        break;
      else
        index += parts[j].size();
    }
    if(index != std::string::npos)
    {
      result = i;
      break;
    }
  }
  return result;
}

double Simulation::getActuatorportMinValue(int id) const
{
  assert(id >= 0 && id < (int) actuatorportList.size());
  return actuatorportList[id]->getMinValue();
}

double Simulation::getActuatorportMaxValue(int id) const
{
  assert(id >= 0 && id < (int) actuatorportList.size());
  return actuatorportList[id]->getMaxValue();
}

void Simulation::setActuatorport(int id, double value)
{
  assert(id >= 0 && id < (int) actuatorportList.size());
  actuatorportList[id]->setValue(value);
}

void Simulation::setActuatorport(int id, bool value)
{
  assert(id >= 0 && id < (int) actuatorportList.size());
  actuatorportList[id]->setValue(value);
}

int Simulation::getSensorPortId(const std::vector<std::string>& parts) const
{
  int result = -1;
  for(unsigned int i=0; i<sensorPortList.size(); i++)
  {
    const std::string& currentName(sensorPortList[i].name);
    std::string::size_type index = 0;
    for(unsigned j = 0; j < parts.size(); ++j)
    {
      index = currentName.find(parts[j], index);
      if(index == std::string::npos)
        break;
      else
        index += parts[j].size();
    }
    if(index != std::string::npos)
    {
      result = i;
      break;
    }
  }
  return result;
}

SensorReading& Simulation::getSensorReading(int id)
{
  assert(id >= 0 && id < (int) sensorPortList.size());
  return sensorPortList[id].sensor->getSensorReading(sensorPortList[id].localPortId);
}

Surface* Simulation::getSurface(const std::string& name) const
{
  std::vector<Surface*>::const_iterator iter;
  for(iter = surfaces.begin(); iter != surfaces.end(); ++iter)
    if((*iter)->name == name)
      return *iter;
  return 0;
}

ShaderProgram* Simulation::getShaderProgram(const std::string& name) const
{
  return graphicsManager->shader->getShaderProgram(name);
}

Environment* Simulation::getEnvironment(const std::string& name) const
{
  std::vector<Environment*>::const_iterator iter;
  for(iter = environments.begin(); iter != environments.end(); ++iter)
    if((*iter)->getName() == name)
      return *iter;
  return 0;
}

void Simulation::addView(View* view,const std::string& name)
{
  view->setName(name);
  view->setFullName(objectTree->getFullName() + ".views." + name);
  view->addToLists(sensorPortList, actuatorportList, actuatorList);
  std::list<View*>::iterator i;
  for(i = viewList.begin(); i != viewList.end() && (*i)->getFullName() < view->getFullName(); ++i)
    ;
  viewList.insert(i, view);
  sceneGraphChanged = true;
}

View* Simulation::getView(int id)
{
  assert(id >= 0 && id < (int) sensorPortList.size());
  assert(sensorPortList[id].sensor->getKind() == "view");
  return (View*) sensorPortList[id].sensor;
}

void Simulation::getObjectDescriptionTree(std::vector<ObjectDescription>& objectDescriptionTree) const
{
  objectDescriptionTree.clear();
  if(objectTree)
  {
    objectTree->addToDescriptions(objectDescriptionTree, 0);
  }
  if(!viewList.empty())
  {
    objectDescriptionTree.push_back(ObjectDescription("", "views", objectTree->getFullName() + ".views", OBJECT_TYPE_NONE, 1));

    std::string path;
    int depth = 1;
    const std::string& pathPrefix(objectTree->getFullName() + ".views.");
    for(std::list<View*>::const_iterator pos = viewList.begin(), end = viewList.end(); pos != end; ++pos)
    {
      // shorten path
      while((*pos)->getName().compare(0, path.length(), path))
      {
        int p = path.find_last_of('.', path.length() - 2);
        path = p == -1 ? "" : path.substr(0, p + 1);
        --depth;
      }

      // lengthen path
      for(;;)
      {
        int p = (*pos)->getName().find_first_of('.', path.length() + 1);
        if(p == -1)
          break;
        ++depth;
        const std::string& name((*pos)->getName().substr(path.length(), p - path.length()));
        path = (*pos)->getName().substr(0, p + 1);

        objectDescriptionTree.push_back(ObjectDescription("", name,
          pathPrefix + path.substr(0, path.length() - 1),
          OBJECT_TYPE_NONE, depth));
      }
      (*pos)->addToDescriptions(objectDescriptionTree, depth + 1);
    }
  }
}

void Simulation::getFirstError(ErrorDescription& error)
{
  errorManager.getFirstError(error);
  errorManager.deleteFirstError();
}

void Simulation::getAllErrors(std::vector<ErrorDescription>& errors)
{
  errorManager.getAllErrors(errors);
  errorManager.deleteAllErrors();
}

void Simulation::deleteAllErrors()
{
  errorManager.deleteAllErrors();
}

void Simulation::writeBackPhysicalResults(SimObject* physObj, const bool& store_last_results)
{
  if(physObj)
  {
    ObjectList::const_iterator pos;
    ObjectList* childNodes = physObj->getPointerToChildNodes();
    for(pos = childNodes->begin(); pos != childNodes->end(); ++pos)
    {
      (*pos)->writeBackPhysicalResult(store_last_results);
      writeBackPhysicalResults(*pos, store_last_results);
    }
  }
}

void Simulation::disablePhysics(SimObject* object, bool disableGeometry)
{
  if(object)
  {
    if(object != objectTree)
    {
      PhysicalObject* physObj = object->castToPhysicalObject();
      if(physObj)
      {
        if(physObj->isGeometryTransformed())
          physObj->getCompoundPhysicalObject()->disablePhysics(disableGeometry);
        else
          physObj->disablePhysics(disableGeometry);
      }
    }

    ObjectList::const_iterator pos;
    ObjectList* childNodes = object->getPointerToChildNodes();

    for(pos = childNodes->begin(); pos != childNodes->end(); ++pos)
    {
      PhysicalObject* physObj = object->castToPhysicalObject();
      if(physObj)
      {
        if(physObj->isGeometryTransformed())
          physObj->getCompoundPhysicalObject()->disablePhysics(disableGeometry);
        else
          physObj->disablePhysics(disableGeometry);
      }
      disablePhysics(*pos, disableGeometry);
    }
  }
}

void Simulation::enablePhysics(SimObject* object)
{
  if(object)
  {
    if(object != objectTree)
    {
      PhysicalObject* physObj = object->castToPhysicalObject();
      if(physObj)
      {
        if(physObj->isGeometryTransformed())
          physObj->getCompoundPhysicalObject()->enablePhysics();
        else
          physObj->enablePhysics();
      }
    }

    ObjectList::const_iterator pos;
    ObjectList* childNodes = object->getPointerToChildNodes();

    for(pos = childNodes->begin(); pos != childNodes->end(); ++pos)
    {
      PhysicalObject* physObj = object->castToPhysicalObject();
      if(physObj)
      {
        if(physObj->isGeometryTransformed())
          physObj->getCompoundPhysicalObject()->enablePhysics();
        else
          physObj->enablePhysics();
      }
      enablePhysics(*pos);
    }
  }
}

void Simulation::staticCollisionCallback(void *pData, dGeomID geom1, dGeomID geom2)
{
  Simulation* pThis = (Simulation*) pData;
  pThis->collisionCallback(geom1,geom2);
}

void Simulation::staticIntersectionCallback(void *pData, dGeomID geom1, dGeomID geom2)
{
  Simulation* pThis = (Simulation*) pData;
  pThis->intersectionCallback(geom1,geom2);
}

void Simulation::collisionCallback(dGeomID geom1, dGeomID geom2)
{
  if(dGeomIsSpace(geom1) || dGeomIsSpace(geom2))
  {
    //colliding a space with something
    dSpaceCollide2(geom1, geom2, this, &staticCollisionCallback);
    //collide all geoms internal to the space(s)
    if(dGeomIsSpace(geom1) && dSpaceID(geom1) != staticSpace)
      dSpaceCollide((dSpaceID)geom1, this, &staticCollisionCallback);
    if(dGeomIsSpace(geom2) && dSpaceID(geom2) != staticSpace)
      dSpaceCollide((dSpaceID)geom2, this, &staticCollisionCallback);
  }
  else
  {
    dBodyID obj1Body = dGeomGetBody(geom1);
    dBodyID obj2Body = dGeomGetBody(geom2);
    short obj1Enabled = obj1Body ? dBodyIsEnabled(obj1Body) : 0;
    short obj2Enabled = obj2Body ? dBodyIsEnabled(obj2Body) : 0;
    //exit without doing anything if the two objects are disabled
    if (!obj1Enabled && !obj2Enabled)
      return;

    PhysicalObject* physObj1 = 0;
    int geomClass1 = dGeomGetClass(geom1);
    int obj1MovID = 0; 
    int obj2MovID = 0; 
    if(geomClass1 != dPlaneClass)
    {
      if(geomClass1 == dGeomTransformClass)
        physObj1 = (PhysicalObject*)dGeomGetData(dGeomTransformGetGeom(geom1));
      else
        physObj1 = (PhysicalObject*)dGeomGetData(geom1);
      obj1MovID = physObj1->getMovableID();
    }
    PhysicalObject* physObj2 = 0;
    int geomClass2 = dGeomGetClass(geom2);
    if(geomClass2 != dPlaneClass)
    {
      if(geomClass2 == dGeomTransformClass)
        physObj2 = (PhysicalObject*)dGeomGetData(dGeomTransformGetGeom(geom2));
      else
        physObj2 = (PhysicalObject*)dGeomGetData(geom2);

      obj2MovID = physObj2->getMovableID();
    }

    if(obj1MovID == 0 && obj2MovID == 0)
      return;

    // exit without doing anything if the two bodies are connected by a joint
    dBodyID b1 = dGeomGetBody(geom1);
    dBodyID b2 = dGeomGetBody(geom2);
    if(collisionDetectionMode == 0 && b1 && b2 && dAreConnected (b1,b2)) 
      return;

    // test if geom o1 and geom o2 can collide
    unsigned long colBits1;
    unsigned long colBits2;
    if(geomClass1 == dGeomTransformClass)
      colBits1 = dGeomGetCollideBits(dGeomTransformGetGeom(geom1));
    else
      colBits1 = dGeomGetCollideBits(geom1);
    if(geomClass2 == dGeomTransformClass)
      colBits2 = dGeomGetCollideBits(dGeomTransformGetGeom(geom2));
    else
      colBits2 = dGeomGetCollideBits(geom2);
    if (colBits1 == 0 || colBits2 == 0) 
      return;

    int i,n;
    const int N = 32;
    dContact contact[N];
    const double* fricTable = frictionCoefficientTable.getTable();
    int numOfMaterials = frictionCoefficientTable.getNumberOfMaterialsInTable();
    n = dCollide (geom1, geom2, N, &contact[0].geom, sizeof(dContact));
    if(n > 0)
    {
      numberOfCollisionContacts++;
      //determine friction coefficent
      double frictionCoefficient = physicsParameters->getDefaultFrictionCoefficient();
      int materialIndex1 = -1;
      int materialIndex2 = -1;
      PhysicalObject* physObj;
      if(geomClass1 != dPlaneClass)
      {
        if(geomClass1 == dGeomTransformClass)
          physObj = (PhysicalObject*)dGeomGetData(dGeomTransformGetGeom(geom1));
        else
          physObj = (PhysicalObject*)dGeomGetData(geom1);

        materialIndex1 = physObj->getMaterialIndex();
      }
      else
      {
        Plane* plane = (Plane*)dGeomGetData(geom1);
        materialIndex1 = plane->getMaterialIndex();
      }

      if(geomClass2 != dPlaneClass)
      {
        if(geomClass2 == dGeomTransformClass)
          physObj = (PhysicalObject*)dGeomGetData(dGeomTransformGetGeom(geom2));
        else
          physObj = (PhysicalObject*)dGeomGetData(geom2);

        materialIndex2 = physObj->getMaterialIndex();
      }
      else
      {
        Plane* plane = (Plane*)dGeomGetData(geom2);
        materialIndex2 = plane->getMaterialIndex();
      }

      if(materialIndex1 != -1 && materialIndex2 != -1)
      {
        frictionCoefficient = fricTable[materialIndex1*numOfMaterials + materialIndex2];
      }

      const double* restitutionCoeffTable = restitutionCoefficientTable.getTable();
      dReal restitutionCoefficient = (dReal)physicsParameters->getDefaultRestitutionCoefficient();
      if(materialIndex1 != -1 && materialIndex2 != -1)
      {
        restitutionCoefficient = dReal(restitutionCoeffTable[materialIndex1*numOfMaterials + materialIndex2]);
      }

      dReal slipFactor = (dReal)physicsParameters->getSlipFactor();
      int contactMode = physicsParameters->getContactMode();

      //create ODE contact joints
      for(i=0; i<n; i++)
      {
        contact[i].surface.mode = contactMode;
        contact[i].surface.mu = (dReal)frictionCoefficient;
        contact[i].surface.bounce = restitutionCoefficient;
        contact[i].surface.bounce_vel = (dReal)physicsParameters->getVelocityThresholdForElasticity();
        contact[i].surface.slip1 = slipFactor;
        contact[i].surface.slip2 = slipFactor;
        contact[i].surface.soft_erp = (dReal)physicsParameters->getContactSoftnessERP();
        contact[i].surface.soft_cfm = (dReal)physicsParameters->getContactSoftnessCFM();

        dJointID c = dJointCreateContact (physicalWorld, contactGroup, &contact[i]);
        dJointAttach(c, dGeomGetBody(contact[i].geom.g1), dGeomGetBody(contact[i].geom.g2));
      }

      // collision sensors
      if(!collisionSensors.empty())
      {
        dGeomID bumperGeom = geom1;
        if(geomClass1 == dGeomTransformClass)
        {
          bumperGeom = dGeomTransformGetGeom(geom1);
        }
        if(collisionSensors.find(bumperGeom) != collisionSensors.end())
        {
          if(collisionSensors[bumperGeom]->getKind() == "bumper")
          {
            Bumper* bumper = collisionSensors[bumperGeom]->castToBumper();
            if(bumper)
            {
              bumper->setValue(true);
            }
          }
          else //InteractiveButton
          {
            InteractiveButton* ibutton = collisionSensors[bumperGeom]->castToInteractiveButton();
            if(ibutton)
            {
              ibutton->hasBeenClicked();
            }
          }
        }
        bumperGeom = geom2;
        if(geomClass2 == dGeomTransformClass)
        {
          bumperGeom = dGeomTransformGetGeom(geom2);
        }
        if(collisionSensors.find(bumperGeom) != collisionSensors.end())
        {
          if(collisionSensors[bumperGeom]->getKind() == "bumper")
          {
            Bumper* bumper = collisionSensors[bumperGeom]->castToBumper();
            if(bumper)
            {
              bumper->setValue(true);
            }
          }
          else //InteractiveButton
          {
            InteractiveButton* ibutton = collisionSensors[bumperGeom]->castToInteractiveButton();
            if(ibutton)
            {
              ibutton->hasBeenClicked();
            }
          }
        }
      }

      //rolling friction
      const double* rollFrictTable = rollingFrictionCoefficientTable.getTable();
      if(materialIndex1 != -1 && materialIndex2 != -1)
      {
        frictionCoefficient = rollFrictTable[materialIndex1*numOfMaterials + materialIndex2];
      }
      else
      {
        frictionCoefficient = physicsParameters->getDefaultRollingFrictionCoefficient();
      }
      if(std::abs(frictionCoefficient) < EPSILON) // Current model for rolling friction does only affect bodies
        return;                                   // for frictions > 0.

      dGeomID geometry1 = geom1;
      if(geomClass1 == dGeomTransformClass)
      {
        geometry1 = dGeomTransformGetGeom(geom1);
        geomClass1 = dGeomGetClass(geometry1);
      }
      if(geomClass1 == dSphereClass || geomClass1 == dCCylinderClass || geomClass1 == dCylinderClass)
      {
        PhysicalObject* physObj = (PhysicalObject*)dGeomGetData(geometry1);
        if(physObj->isAffectedByRollingFriction())
        {
          if(frictionCoefficient > 0.0)
            physObj->applySimpleRollingFriction(frictionCoefficient);
        }
      }

      dGeomID geometry2 = geom2;
      if(geomClass2 == dGeomTransformClass)
      {
        geometry2 = dGeomTransformGetGeom(geom2);
        geomClass2 = dGeomGetClass(geometry2);
      }
      if(geomClass2 == dSphereClass || geomClass2 == dCCylinderClass || geomClass2 == dCylinderClass)
      {
        PhysicalObject* physObj = (PhysicalObject*)dGeomGetData(geometry2);
        if(physObj->isAffectedByRollingFriction())
        {
          if(frictionCoefficient > 0.0)
            physObj->applySimpleRollingFriction(frictionCoefficient);
        }
      }
    }
  }
}

void Simulation::intersectionCallback(dGeomID geom1, dGeomID geom2)
{
  if(dGeomIsSpace(geom1) || dGeomIsSpace(geom2))
  {
    //Does this happen? To be tested.
    dSpaceCollide2(geom1, geom2, this, &staticIntersectionCallback);
  }
  else
  {
    dContactGeom contact;
    if(dCollide(geom1, geom2, 1, &contact, sizeof(dContactGeom)))
    {
      if(contact.depth < intersectionInformation.distance)
        intersectionInformation.distance = contact.depth;
    }
  }
}

void Simulation::parseAttributeElements(const std::string& name,
                                        const AttributeSet& attributes,
                                        int line, int column,
                                        ErrorManager* errorManager)
{
  if(name == "Background")
  {
    std::string surfaceName(ParserUtilities::getValueFor(attributes,"ref"));
    backgroundSurface = getSurface(surfaceName);
    if(!backgroundSurface)
    {
      errorManager->addError("Unknown Surface",
        surfaceName + " is not a valid surface element.",
        line, column);
    }
  }
  else if(name == "Light")
  {
    std::string lightName(ParserUtilities::getValueFor(attributes,"ref"));
    switch(graphicsManager->preEnableLight(lightName))
    {
    case 1:
      errorManager->addError("Unknown Light",
        lightName + " refers to no valid LightDefinition. Defined are: "
        + graphicsManager->getNamesOfLights() + ".", line, column);
      break;
    case 2: 
      errorManager->addError("Too many Lights",
        lightName + " got no valid lightID. The maximum light number is 8.", line, column);
      break;
    default:
      break;
    }
  }
  else if(name == "GlobalAmbientLight")
  {
    ParserUtilities::toRGB(attributes, graphicsManager->getGlobalAmbientLight());
    graphicsManager->setGlobalAmbientLight();
  }
  else if(name == "Environment")
  {
    std::string environmentName(ParserUtilities::getValueFor(attributes,"ref"));
    graphicsManager->setEnvironment(getEnvironment(environmentName));
    Environment* env = graphicsManager->getEnvironment();
    if(!env)
    {
      errorManager->addError("Unknown Environment",
        "\"" + environmentName + "\" is not a valid environment element.",
        line, column);
    }
  }
  else if(name == "DefaultAppearance")
  {
    std::string surfaceName(ParserUtilities::getValueFor(attributes,"ref"));
    defaultSurface = getSurface(surfaceName);
    if(!defaultSurface)
    {
      errorManager->addError("Unknown Surface",
        surfaceName+" is not a valid surface element.",
        line, column);
    }
  }
  else if(name == "GlobalPhysicalParameters")
  {
    for(unsigned int i=0; i< attributes.size(); i++)
    {
      double value(ParserUtilities::toDouble(attributes[i].value));
      if(attributes[i].attribute == "gravity")
      {
        physicsParameters->setGravity(value);
      }
      else if(attributes[i].attribute == "erp")
      {
        physicsParameters->setErrorReductionParameter(value);
      }
      else if(attributes[i].attribute == "cfm")
      {
        physicsParameters->setConstraintForceMixing(value);
      }
      else if(attributes[i].attribute == "defaultFrictionCoefficient")
      {
        physicsParameters->setDefaultFrictionCoefficient(value);
      }
      else if(attributes[i].attribute == "defaultRollingFrictionCoefficient")
      {
        physicsParameters->setDefaultRollingFrictionCoefficient(value);
      }
      else if(attributes[i].attribute == "defaultCoefficientOfRestitution")
      {
        physicsParameters->setDefaultRestitutionCoefficient(value);
        physicsParameters->addContactMode(dContactBounce);
      }
      else if(attributes[i].attribute == "velocityThresholdForElasticity")
      {
        physicsParameters->setVelocityThresholdForElasticity(value);
      }
      else if(attributes[i].attribute == "contactSoftnessERP")
      {
        physicsParameters->setContactSoftnessERP(value);
      }
      else if(attributes[i].attribute == "contactSoftnessCFM")
      {
        physicsParameters->setContactSoftnessCFM(value);
      }
      else if(attributes[i].attribute == "slipFactor")
      {
        physicsParameters->setSlipFactor(value);
        physicsParameters->addContactMode(dContactSlip1);
        physicsParameters->addContactMode(dContactSlip2);
      }
    }
    physicsParameters->addContactMode(dContactSoftERP | dContactSoftCFM | dContactApprox1);
  }
  else if(name == "SimulationParameters")
  {
    for(unsigned int i=0; i< attributes.size(); i++)
    {
      double value(ParserUtilities::toDouble(attributes[i].value));
      if(attributes[i].attribute == "stepLength")
      {
        setStepLength(value);
      }
      else if(attributes[i].attribute == "standardLength")
      {
        standardLength = value;
      }
      else if(attributes[i].attribute == "physicsStepLength")
      {
        physicsStepLength = value;
      }
      else if(attributes[i].attribute == "collisionDetectionMode")
      {
        collisionDetectionMode = short(ParserUtilities::toInt(attributes[i].value));
      }
      else if(attributes[i].attribute == "applyDynamicsForceFactor")
      {
        applyDynamicsForceFactor = ParserUtilities::toDouble(attributes[i].value);
      }
    }
    if(physicsStepLength < 0.0 || physicsStepLength > stepLength)
    {
      physicsStepLength = stepLength;
    }
  }
  else if(name == "Plane")
  {
    planes.push_back(new Plane(attributes));
  }
  else if(name == "Appearance") // for plane; restricted by schema file
  {
    planes.back()->parseAppearance(attributes, line, column, errorManager, *this);
  }
  else if(name == "Material") // for plane; restricted by schema file
  {
    planes.back()->parseMaterial(attributes, line, column, errorManager, *this);
  }
  else if(name == "Elements" || name == "Element")
  {
    // These elements are ignored, they are just used to structure
    // the description in XML
  }
  // These are parsing functions for macro instantiation:
  else if(name == "Rotation")
  {
    Vector3d rot(ParserUtilities::toPoint(attributes));
    rot.toRad();
    currentMacro->rotate(rot, currentMacro->getPosition());
  }
  else if(name == "Translation")
  {
    currentMacro->translate(currentMacro->getParentNode()->getRotation() * ParserUtilities::toPoint(attributes));
  }
  else if(name == "LightBinding")
  {
    std::string lightName, objectName;
    for(unsigned int i=0; i< attributes.size(); i++)
    {
      if(attributes[i].attribute == "ref")
        lightName = attributes[i].value;
      else if(attributes[i].attribute == "object")
        objectName = attributes[i].value;
    }
    switch(graphicsManager->preEnableLight(lightName, objectName, parser->getCurrentNode()))
    {
    case 1:
      errorManager->addError("Unknown Light",
        lightName + " refers to no valid LightDefinition. Defined are: "
        + graphicsManager->getNamesOfLights() + ".", line, column);
      break;
    case 2: 
      errorManager->addError("Too many Lights",
        lightName + " got no valid lightID. The maximum light number is 8.", line, column);
      break;
    default:
      break;
    }
  }
  else if(name == "EnvironmentMappingBinding")
  {
    graphicsManager->reflectionObject = parser->getCurrentNode();
  }
  else if(name == "MotionBlurMethod")
  {
    MotionBlurMode method = MOTIONBLUR_VELOCITY_BUFFER_2;
    if(ParserUtilities::getValueFor(attributes,"force") == "AccumulationBuffer")
      method = MOTIONBLUR_12PASSES;
    currentMacro->setForcedMotionBlurMethod(method);
  }
  else if(name == "SubstituteSurfaces")
  {
    std::string oldSurface(ParserUtilities::getValueFor(attributes,"substitute"));
    std::string newSurface(ParserUtilities::getValueFor(attributes,"with"));
    Surface* oldSurfacePtr(0);
    Surface* newSurfacePtr(0);
    std::vector<Surface*>::const_iterator iter(surfaces.begin());
    while(iter != surfaces.end())
      if((*iter)->name == oldSurface)
      {
        oldSurfacePtr = *iter;
        break;
      }
      else
        iter++;
    if(iter == surfaces.end())
    {
      errorManager->addError("Unknown surface", "The surface "+oldSurface+" does not exist.",line,column);
      return;
    }
    iter = surfaces.begin();
    while(iter != surfaces.end())
      if((*iter)->name == newSurface)
      {
        newSurfacePtr = *iter;
        break;
      }
      else
        iter++;
    if(iter == surfaces.end())
    {
      errorManager->addError("Unknown surface", "The surface "+newSurface+" does not exist.",line,column);
      return;
    }
    currentMacro->replaceSurface(oldSurfacePtr, newSurfacePtr);
  }
  else if(name == "SubstituteAllSurfaces")
  {
    std::string newSurface(ParserUtilities::getValueFor(attributes,"with"));
    Surface* newSurfacePtr(0);
    std::vector<Surface*>::const_iterator iter(surfaces.begin());
    while(iter != surfaces.end())
      if((*iter)->name == newSurface)
      {
        newSurfacePtr = *iter;
        break;
      }
      else
        iter++;
    if(iter == surfaces.end())
    {
      errorManager->addError("Unknown surface", "The surface "+newSurface+" does not exist.",line,column);
      return;
    }
    currentMacro->replaceSurface(newSurfacePtr);
  }
  else if(name == "SubstituteShaderPrograms")
  {
    std::string oldShaderProgram(ParserUtilities::getValueFor(attributes,"substitute"));
    std::string newShaderProgram(ParserUtilities::getValueFor(attributes,"with"));
    ShaderProgram* oldShaderProgramPtr = getShaderProgram(oldShaderProgram);
    ShaderProgram* newShaderProgramPtr = getShaderProgram(newShaderProgram);

    if(!oldShaderProgramPtr)
    {
      errorManager->addError("Unknown shader program",
        "The shader program " + oldShaderProgram + " does not exist.",line,column);
      return;
    }
    if(!newShaderProgramPtr)
    {
      errorManager->addError("Unknown shader program",
        "The shader program " + newShaderProgram + " does not exist.",line,column);
      return;
    }
    currentMacro->replaceShaderProgram(oldShaderProgramPtr, newShaderProgramPtr);
  }
  else if(name == "SubstituteAllShaderPrograms")
  {
    std::string newShaderProgram(ParserUtilities::getValueFor(attributes,"with"));
    ShaderProgram* newShaderProgramPtr = getShaderProgram(newShaderProgram);
    std::vector<Surface*>::const_iterator iter(surfaces.begin());
    if(!newShaderProgramPtr)
    {
      errorManager->addError("Unknown shader program",
        "The shader program " + newShaderProgram + " does not exist.",line,column);
      return;
    }
    currentMacro->replaceShaderProgram(newShaderProgramPtr);
  }
  else if(name == "Collision")
  {
    currentMacro->changeAbilityToCollide(ParserUtilities::toBool(ParserUtilities::getValueFor(attributes,"value")));
  }
  else if(name == "QuickPhysicsSolver")
  {
    useQuickSolverForPhysics = true;
    numberOfIterationsForQuickSolver = ParserUtilities::toInt(ParserUtilities::getValueFor(attributes, "iterations"));
  }
  else if(name == "AutomaticObjectDisabling")
  {
    this->useAutomaticObjectDisabling = true;
    for(unsigned int i=0; i< attributes.size(); i++)
    {
      if(attributes[i].attribute == "linearVelocityThreshold")
      {
        autoObjectDisableParameters.linearVelocityThreshold = ParserUtilities::toDouble(attributes[i].value);
      }
      else if(attributes[i].attribute == "angularVelocityThreshold")
      {
        autoObjectDisableParameters.angularVelocityThreshold = ParserUtilities::toDouble(attributes[i].value);
      }
      else if(attributes[i].attribute == "physicsSimulationSteps")
      {
        autoObjectDisableParameters.physicsSimulationSteps = ParserUtilities::toInt(attributes[i].value);
      }
      else if(attributes[i].attribute == "physicsSimulationTime")
      {
        autoObjectDisableParameters.physicsSimulationTime = ParserUtilities::toDouble(attributes[i].value);
      }
    }
  }
  else if(name == "Scene")
  {
    for(unsigned int i=0; i< attributes.size(); i++)
    {
      if(attributes[i].attribute == "name")
      {
        objectTree->setName(attributes[i].value);
      }
      else if(attributes[i].attribute == "description")
      {
        sceneDescription = attributes[i].value;
      }
    }
  }
  else
  {
    errorManager->addUnknownElementError(name, line, column);
  }
}


void Simulation::postprocessObjectTree(SimObject* element, const std::string& path)
{
  if(!backgroundSurface)
  {
    backgroundSurface = new Surface();
    backgroundSurface->setColorv(black);
  }
  std::string name(element->getName());
  std::string fullName(path + name);
  element->setFullName(fullName);
  element->addToLists(sensorPortList, actuatorportList, actuatorList);
  element->postProcessAfterParsing();
  if((element->getKind() == "compound") && 
    (element->getParentNode() != 0) &&
    (element->getParentNode()->getMovableID() != 0))
  {
    createNewMovableID();
    element->setMovableID(getCurrentMovableID());
  }
  for(unsigned int i=0; i<element->getNumberOfChildNodes(); i++)
  {
    postprocessObjectTree(element->getChildNode(i), fullName + ".");
  }
}


void Simulation::collectObjectsAndMovables(SimObject* currentElement)
{
  //List of all objects:
  objectList.push_back(currentElement);
  //Map of all movables:
  int movID(currentElement->getMovableID());
  if(movID)
  {
    std::map<int,SimObject*>::iterator pos;
    pos = movableMap.find(movID);
    if(pos == movableMap.end())           // Not known yet, enter new id,
      movableMap[movID] = currentElement; // else case needs not to be handled.
  }
  //Enter child nodes:
  ObjectList::iterator pos;
  ObjectList* childNodes = currentElement->getPointerToChildNodes();
  for(pos = childNodes->begin(); pos != childNodes->end(); ++pos)
  {
    collectObjectsAndMovables(*pos);
  }
}


void Simulation::createStaticObjectPhysics(SimObject* object)
{
  if(object)
  {
    ObjectList::const_iterator pos;
    ObjectList* childNodes = object->getPointerToChildNodes();
    for(pos = childNodes->begin(); pos != childNodes->end(); ++pos)
    {
      if((*pos)->getMovableID() == 0 && (*pos)->castToPhysicalObject() != 0)
        (*pos)->createPhysics();
      createStaticObjectPhysics(*pos);
    }
  }
}


void Simulation::createCompoundPhysicalObjects(SimObject* object)
{
  if(object)
  {
    ObjectList::const_iterator pos;
    ObjectList* childNodes = object->getPointerToChildNodes();

    for(pos = childNodes->begin(); pos != childNodes->end(); ++pos)
    {
      int movID = (*pos)->getMovableID();
      if(movID != 0 && (*pos)->castToPhysicalObject() != 0)
      {
        //movable objects
        std::map<int, CompoundPhysicalObject*>::const_iterator compoundObjIter;
        compoundObjIter = compoundObjects.find(movID);
        if(compoundObjIter != compoundObjects.end())
        {
          compoundObjects[movID]->addObject(*pos);
        }
        else
        {
          CompoundPhysicalObject* compoundObj = new CompoundPhysicalObject();
          compoundObj->setMovableID(movID);
          compoundObj->setSimulation(this);
          compoundObj->setParent((*pos)->getParentNode());
          compoundObj->addObject(*pos);
          compoundObjects[movID] = compoundObj;
        }
      }
      createCompoundPhysicalObjects(*pos);
    }
  }

}


void Simulation::createCompoundObjectPhysics()
{
  // Create all physical objects:
  std::map<int, CompoundPhysicalObject*>::const_iterator compoundObjIter;
  for(compoundObjIter = compoundObjects.begin(); compoundObjIter != compoundObjects.end(); ++compoundObjIter)
  {
    compoundObjIter->second->createPhysics();
  }
  // Update position and rotation:
  writeBackPhysicalResults(objectTree, false);
}


void Simulation::createJointAndMotorPhysics(SimObject* object)
{
  if(object)
  {
    ObjectList::const_iterator pos;
    ObjectList* childNodes = object->getPointerToChildNodes();
    for(pos = childNodes->begin(); pos != childNodes->end(); ++pos)
    {
      (*pos)->createJointPhysics();
      (*pos)->createMotorPhysics();
      createJointAndMotorPhysics(*pos);
    }
  }
}


void Simulation::resetDynamicsForSelection()
{
  resetDynamicsForObject(selection.object);
}


void Simulation::resetDynamicsForObject(SimObject* object)
{
  if(object)
  {
    if(object != objectTree)
    {
      PhysicalObject* physObj = object->castToPhysicalObject();
      if(physObj)
      {
        dBodyID* body = (physObj)->getBody();
        if(body)
        {
          dBodySetLinearVel(*body, 0,0,0);
          dBodySetAngularVel(*body, 0,0,0);
        }
      }
    }

    ObjectList::const_iterator pos;
    ObjectList* childNodes = object->getPointerToChildNodes();

    for(pos = childNodes->begin(); pos != childNodes->end(); ++pos)
    {
      resetDynamicsForObject(*pos);
    }
  }
}


void Simulation::applyForceForSelection(int x, int y)
{
  if(!selection.object)
    return;

  Vector3d vecFar,
           vecNear;
  gluProjectClick(x, y, vecFar, vecNear);
  Vector3d planeIntersection;
  if(SimGeometry::intersectRayAndPlane(vecNear, vecFar, selection.object->getPosition(),
    selection.plane, planeIntersection))
  {
    Vector3d translationVec = planeIntersection - selection.object->getPosition() - selection.clickOffset;
    PhysicalObject* physObj = selection.object->castToPhysicalObject();
    if(physObj)
    {
      double objMass = getMassForObject(selection.object);
      double powerFactor = ((-physicsParameters->getGravity()) * objMass * applyDynamicsForceFactor);
      //      powerFactor *= objMass;
      dBodyAddForce(*(physObj->getBody()),
        dReal(translationVec.v[0] * powerFactor),
        dReal(translationVec.v[1] * powerFactor),
        dReal(translationVec.v[2] * powerFactor));
    }
  }
}

void Simulation::applyTorqueForSelection(int x, int y)
{
  assert(selection.object);
  Vector3d vecFar,
    vecNear;
  gluProjectClick(x, y, vecFar, vecNear);
  Vector3d planeIntersection;
  SimGeometry::intersectRayAndPlane(vecNear, vecFar, selection.object->getPosition(),
    selection.plane, planeIntersection);
  Vector3d clickOffset = planeIntersection - selection.object->getPosition();
  double angle;

  if(selection.plane.v[0])
    angle = atan2(clickOffset.v[2], clickOffset.v[1]) -
    atan2(selection.clickOffset.v[2], selection.clickOffset.v[1]);
  else if(selection.plane.v[1])
    angle = atan2(clickOffset.v[0], clickOffset.v[2]) -
    atan2(selection.clickOffset.v[0], selection.clickOffset.v[2]);
  else
    angle = atan2(clickOffset.v[1], clickOffset.v[0]) -
    atan2(selection.clickOffset.v[1], selection.clickOffset.v[0]);

  PhysicalObject* physObj = selection.object->castToPhysicalObject();
  if(physObj)
  {
    angle *= Functions::toRad(angle);
    double objMass = getMassForObject(selection.object);
    double powerFactor = (-physicsParameters->getGravity())*(0.0001/physicsStepLength);
    powerFactor *= objMass;
    powerFactor *= angle;
    dBodyAddTorque(*(physObj->getBody()),
      dReal(selection.plane.v[0] * powerFactor),
      dReal(selection.plane.v[1] * powerFactor),
      dReal(selection.plane.v[2] * powerFactor));
  }
}

inline double Simulation::getMassForObject(SimObject* object) const
{
  double tempMass = 0.0;
  if(object)
  {
    if(object != objectTree)
    {
      PhysicalObject* physObj = object->castToPhysicalObject();
      if(physObj)
      {
        tempMass = physObj->getMass();
      }
    }
    ObjectList::const_iterator pos;
    ObjectList* childNodes = object->getPointerToChildNodes();
    for(pos = childNodes->begin(); pos != childNodes->end(); ++pos)
    {
      tempMass += getMassForObject(*pos);
    }
  }
  return tempMass;
}

void Simulation::collectCollisionSensors()
{
  std::vector<SensorPort>::iterator pos;
  for(pos = sensorPortList.begin(); pos != sensorPortList.end(); ++pos)
  {
    Sensor* sensor = pos->sensor;
    if(sensor->getKind() == "bumper" || sensor->getKind() == "interactiveButton")
    {
      collectCollisionSensorGeometry(sensor, sensor);
    }
  }
}

void Simulation::collectCollisionSensorGeometry(Sensor* sensor, SimObject* object)
{
  ObjectList::const_iterator pos;
  ObjectList* childNodes = object->getPointerToChildNodes();
  for(pos = childNodes->begin(); pos != childNodes->end(); ++pos)
  {
    if(object->castToJoint() == 0 && object->getMovableID() == (*pos)->getMovableID())
    {
      collectCollisionSensorGeometry(sensor, *pos);

      PhysicalObject* physObj = (*pos)->castToPhysicalObject();
      if(physObj)
      {
        if(physObj->getKind() == "complex-shape")
        {
          std::vector<PhysicalObject*> *physRep = ((ComplexShape*)physObj)->getPhysicalRepresentations();
          std::vector<PhysicalObject*>::const_iterator physRepIter;
          for(physRepIter = physRep->begin(); physRepIter != physRep->end(); ++physRepIter)
          {
            collisionSensors[*((*physRepIter)->getGeometry())] = sensor;
          }
        }
        else
          collisionSensors[*(physObj->getGeometry())] = sensor;
      }
    }
  }
}

void Simulation::resetCollisionSensors()
{
  std::map<dGeomID, Sensor*>::const_iterator iter;
  for(iter = collisionSensors.begin(); iter != collisionSensors.end(); ++iter)
  {
    Bumper* bumper = iter->second->castToBumper();
    if(bumper)
      bumper->setValue(false);
  }
}

void Simulation::createPlanePhysics()
{
  std::vector<Plane*>::iterator iter;
  for(iter = planes.begin(); iter != planes.end(); ++iter)
  {
    (*iter)->createPhysics(staticSpace);
  }
}

SimObject* Simulation::findRootOfMovableObject(SimObject* object) const
{
  if(object->getKind() == "interactiveButton")
  {
    InteractiveButton* ibutton = object->castToInteractiveButton();
    if(ibutton)
      return object;
  }
  SimObject* parent = object->getParentNode();
  if(parent->getKind() == "compound" && parent != objectTree)
    parent = parent->getParentNode();
  if(parent->getMovableID() > 0 && (parent->getMovableID() == object->getMovableID() || parent->castToJoint() != 0))
  {
    return(findRootOfMovableObject(parent));
  }
  return object;
}

void Simulation::applyJointFriction()
{
  std::vector<Actuator*>::iterator pos;
  for(pos = actuatorList.begin(); pos != actuatorList.end(); ++pos)
  {
    Joint* joint = (*pos)->castToJoint();
    if(joint)
      joint->applyFriction(false);
  }
}

int Simulation::getMaterialIndex(const std::string materialName) const
{
  std::vector<Material*>::const_iterator iter(materials.begin());
  while(iter != materials.end())
  {
    if((*iter)->getName() == materialName)
    {
      return((*iter)->getIndex());
    }
    else
      iter++;
    if(iter == materials.end())
    {
      return -1;
    }
  }
  return -1;
}


void Simulation::createPhysicalCoefficientTables()
{
  std::vector<Material*>::const_iterator materialsIter;
  for(materialsIter=materials.begin(); materialsIter != materials.end(); ++materialsIter)
  {
    const std::vector<CoefficientDefinition>* coefficientDefinitions = (*materialsIter)->getFrictionCoefficients();
    std::vector<CoefficientDefinition>::const_iterator coeffDefIter;
    for(coeffDefIter = coefficientDefinitions->begin(); coeffDefIter != coefficientDefinitions->end(); ++coeffDefIter)
    {
      CoefficientTableEntry tableEntry;
      tableEntry.material1 = (*materialsIter)->getName();
      tableEntry.material2 = coeffDefIter->otherMaterial;
      tableEntry.coefficient = coeffDefIter->coefficient;

      this->frictionCoefficientTable.addCoefficientTableEntry(tableEntry);
    }

    coefficientDefinitions = (*materialsIter)->getRollingFrictionCoefficients();
    for(coeffDefIter = coefficientDefinitions->begin(); coeffDefIter != coefficientDefinitions->end(); ++coeffDefIter)
    {
      CoefficientTableEntry tableEntry;
      tableEntry.material1 = (*materialsIter)->getName();
      tableEntry.material2 = coeffDefIter->otherMaterial;
      tableEntry.coefficient = coeffDefIter->coefficient;
      rollingFrictionCoefficientTable.addCoefficientTableEntry(tableEntry);
    }

    coefficientDefinitions = (*materialsIter)->getRestitutionCoefficients();
    if(coefficientDefinitions->size() > 0)
      physicsParameters->addContactMode(dContactBounce);
    for(coeffDefIter = coefficientDefinitions->begin(); coeffDefIter != coefficientDefinitions->end(); ++coeffDefIter)
    {
      CoefficientTableEntry tableEntry;
      tableEntry.material1 = (*materialsIter)->getName();
      tableEntry.material2 = coeffDefIter->otherMaterial;
      tableEntry.coefficient = coeffDefIter->coefficient;
      restitutionCoefficientTable.addCoefficientTableEntry(tableEntry);
    }
  }
  frictionCoefficientTable.createTable(&(this->materials),this->physicsParameters->getDefaultFrictionCoefficient());
  rollingFrictionCoefficientTable.createTable(&(this->materials),this->physicsParameters->getDefaultRollingFrictionCoefficient());
  restitutionCoefficientTable.createTable(&(this->materials),this->physicsParameters->getDefaultRestitutionCoefficient());
}

bool Simulation::intersectWorldWithRay(const Vector3d& pos, const Vector3d& ray,
                                       double maxLength, double& distance)
{
  //Initialize ray:
  dGeomRaySet(intersectionRay,dReal(pos.v[0]),dReal(pos.v[1]),dReal(pos.v[2]),
    dReal(ray.v[0]),dReal(ray.v[1]),dReal(ray.v[2]));
  dGeomRaySetLength(intersectionRay,dReal(maxLength));
  intersectionInformation.distance = maxLength;
  dSpaceCollide2 (intersectionRay, (dGeomID)rootSpace, this, staticIntersectionCallback);
  distance = intersectionInformation.distance;
  return true;
}

void Simulation::registerControllerDrawingByPartOfName(const std::vector<std::string>& parts,
                                                       Controller3DDrawing* drawing)
{
  SimObject* object = getObjectReference(parts);
  object->attachDrawing(drawing);
}

void Simulation::unregisterControllerDrawingByPartOfName(const std::vector<std::string>& parts,
                                                         Controller3DDrawing* drawing)
{
  SimObject* object = getObjectReference(parts);
  object->attachDrawing(drawing);
}

SimObject* Simulation::getObjectReference(const std::vector<std::string>& partsOfName, bool fullName)
{
  SimObject* object = 0;
  objectTree->findObject(object, partsOfName);
  assert(object);
  return object;
}

void Simulation::toggleShaderInterface()
{
  if(graphicsManager)
  {
    graphicsManager->shader->toggleRunning();
  }
}

bool Simulation::getShaderInterfaceState()
{
  if(graphicsManager)
    return (bool)graphicsManager->shader->getRunning();
  return false;
}

void Simulation::reloadShader()
{
  if(graphicsManager)
  {
    graphicsManager->shader->reloadShader();
  }
}

void Simulation::switchProgram(bool previous)
{
  if(graphicsManager)
  {
    graphicsManager->shader->switchProgram(previous);
  }
}

void Simulation::switchShader(bool previous)
{
  if(graphicsManager)
  {
    graphicsManager->shader->switchShader(previous);
  }
}

void Simulation::toggleShaderShowInfos()
{
  if(graphicsManager)
    graphicsManager->shader->toggleShowInfos();
}

bool Simulation::getShaderShowInfos()
{
  if(graphicsManager)
    return graphicsManager->shader->getShowInfosState();
  return false;
}

void Simulation::switchInfo(bool previous)
{
  if(graphicsManager)
  {
    graphicsManager->shader->switchInfo(previous);
  }
}

void Simulation::definePostProgram()
{
  if(graphicsManager)
  {
    graphicsManager->shader->definePostProgram();
  }
}

bool Simulation::getPostProgramDefined()
{
  if(graphicsManager)
    return graphicsManager->shader->getPostProgramDefined();
  return false;
}

void Simulation::defineGlobalProgram()
{
  if(graphicsManager)
  {
    graphicsManager->shader->defineGlobalProgram();
  }
}

bool Simulation::checkMotionBlurCapability(const MotionBlurMode mode)
{
  if(graphicsManager)
  {
    return (graphicsManager->checkMotionBlurCapability(mode));
  }
  return false;
}

MotionBlurMode Simulation::getMotionBlurMode()
{
  return motionBlurMode;
}

void Simulation::setMotionBlurMode(MotionBlurMode mb_mode)
{
  // unset the last changes
  graphicsManager->unsetMotionBlurMode(motionBlurMode);

  // set the new
  motionBlurMode = mb_mode;
  graphicsManager->setMotionBlurMode(motionBlurMode);
}

void Simulation::toggleMultisampledFBOs()
{
  if(graphicsManager)
  {
    graphicsManager->toggleMultisampledFBOs();
  }
}

bool Simulation::getMultisampledFBOsCapability() const
{
  return graphicsManager ? graphicsManager->checkMultisampledFBOCapability() : false;
}

bool Simulation::getMultisampledFBOsState() const
{
  return graphicsManager ? graphicsManager->getMultisampledFBOsState() : false;
}

void Simulation::setDebugFBOs(const int debugState)
{
  if(graphicsManager)
    graphicsManager->shader->setDebugFBOs(debugState);
}

int Simulation::getDebugFBOsState()
{
  if(graphicsManager)
    return graphicsManager->shader->getDebugFBOsState();
  return -2;
}

void Simulation::toggleOversizedFBOs()
{
  if(graphicsManager)
    graphicsManager->shader->toggleOversizedFBOs();
}

bool Simulation::getOversizedFBOsState()
{
  if(graphicsManager)
    return graphicsManager->shader->getOversizedFBOsState();
  return false;
}

int Simulation::getAntialiasingSamples()
{
  if(graphicsManager)
    return graphicsManager->getAntialiasingSamples();
  return 0;
}

bool Simulation::getShowFPS()
{
  if(graphicsManager)
    return graphicsManager->showFPS;
  return false;
}

void Simulation::toggleShowFPS()
{
  if(graphicsManager)
    graphicsManager->showFPS ^= 1;
}

bool Simulation::getShowStatistics()
{
  return showStatistics;
}

void Simulation::toggleShowStatistics()
{
  showStatistics ^= 1;
}

void Simulation::setCubeMapGeneration()
{
  saveCubeMap = true;
}

void Simulation::moveObject(SimObject* object, const Vector3d& pos)
{
  disablePhysics(object, true);

  std::map<int,CompoundPhysicalObject*>::iterator iter;
  for(iter = compoundObjects.begin(); iter != compoundObjects.end(); ++iter)
    (*iter).second->setSynchronized(false);

  Vector3d translationVec = pos - object->getPosition();
  PhysicalObject* physObj = object->castToPhysicalObject();
  if(physObj && 
     compoundObjects.find(physObj->getMovableID()) != compoundObjects.end() &&
     !physObj->getCompoundPhysicalObject()->isSimple())
  {
    CompoundPhysicalObject* compoundObj = physObj->getCompoundPhysicalObject();
    compoundObj->translateDnD(translationVec, true);
  }
  else
    object->translateDnD(translationVec);

  enablePhysics(object);
}
  
void Simulation::moveObject(SimObject* object, const Vector3d& pos, const Vector3d& rot)
{
  disablePhysics(object, true);

  std::map<int,CompoundPhysicalObject*>::iterator iter;
  Vector3d translationVec = pos - object->getPosition();
  PhysicalObject* physObj = object->castToPhysicalObject();
  if(physObj &&
     compoundObjects.find(physObj->getMovableID()) != compoundObjects.end() &&
     !physObj->getCompoundPhysicalObject()->isSimple())
  {
    CompoundPhysicalObject* compoundObj = physObj->getCompoundPhysicalObject();

    // this is a hack for moving to the correct angles
    for(int i = 0; i < 2; ++i)
    {
      for(iter = compoundObjects.begin(); iter != compoundObjects.end(); ++iter)
        (*iter).second->setSynchronized(false);
      double angle = rot.v[0] - object->getRotation().getXAngle();
      compoundObj->rotateAroundAxisDnD(angle, Vector3d(1, 0, 0), true);

      for(iter = compoundObjects.begin(); iter != compoundObjects.end(); ++iter)
        (*iter).second->setSynchronized(false);
      angle = rot.v[1] - object->getRotation().getYAngle();
      compoundObj->rotateAroundAxisDnD(angle, Vector3d(0, 1, 0), true);

      for(iter = compoundObjects.begin(); iter != compoundObjects.end(); ++iter)
        (*iter).second->setSynchronized(false);
      angle = rot.v[2] - object->getRotation().getZAngle();
      compoundObj->rotateAroundAxisDnD(angle, Vector3d(0, 0, 1), true);
    }

    for(iter = compoundObjects.begin(); iter != compoundObjects.end(); ++iter)
      (*iter).second->setSynchronized(false);
    compoundObj->translateDnD(translationVec, true);
  }
  else
  {
    // this is a hack for moving to the correct angles
    for(int i = 0; i < 2; ++i)
    {
      for(iter = compoundObjects.begin(); iter != compoundObjects.end(); ++iter)
        (*iter).second->setSynchronized(false);
      double angle = rot.v[0] - object->getRotation().getXAngle();
      object->rotateAroundAxisDnD(angle, Vector3d(1, 0, 0));

      for(iter = compoundObjects.begin(); iter != compoundObjects.end(); ++iter)
        (*iter).second->setSynchronized(false);
      angle = rot.v[1] - object->getRotation().getYAngle();
      object->rotateAroundAxisDnD(angle, Vector3d(0, 1, 0));

      for(iter = compoundObjects.begin(); iter != compoundObjects.end(); ++iter)
        (*iter).second->setSynchronized(false);
      angle = rot.v[2] - object->getRotation().getZAngle();
      object->rotateAroundAxisDnD(angle, Vector3d(0, 0, 1));
    }

    for(iter = compoundObjects.begin(); iter != compoundObjects.end(); ++iter)
      (*iter).second->setSynchronized(false);
    object->translateDnD(translationVec);
  }

  enablePhysics(object);
}

void Simulation::changePhysicsParametersAtRunTime(const PhysicsParameterSet& newParams)
{
  //Replace local parameters:
  *physicsParameters = newParams;
  //Some parameters must be activated here:
  dWorldSetGravity(physicalWorld, 0,0,(dReal)(physicsParameters->getGravity()));
  dWorldSetERP(physicalWorld, (dReal)(physicsParameters->getErrorReductionParameter()));
  dWorldSetCFM(physicalWorld, (dReal)(physicsParameters->getConstraintForceMixing()));
  //All other parameters are taken from physicsParameters at run-time. I think ;-)
}
