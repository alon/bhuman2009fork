

#include "ObjectRenderer.h"
#include "../Simulation/Simulation.h"
#include "../Tools/SimGraphics.h"
#include "../Platform/Assert.h"
#include "../Platform/System.h"
#include "../Platform/OffscreenRenderer.h"

const double pi_2 = 3.1415926535897932384626433832795/2.0;
const double pi = 3.1415926535897932384626433832795;

ObjectRenderer::ObjectRenderer(Simulation* simulation, const std::string& object) :
  simulation(simulation), graphicsManager(simulation ? simulation->getGraphicsManager() : 0), 
  object(object), simObject(simulation ? simulation->getObjectReference(object) : 0), 
  width(0), height(0), focus(false), 
  dragging(false), dragPlane(XY_PLANE), dragMode(KEEP_DYNAMICS)
{

  resetCamera();

  fovy = 40.0;
  fbo_reg = GLHelper::getGLH()->getFBOreg();
}

ObjectRenderer::~ObjectRenderer()
{
  GLHelper::getGLH()->removeFBOreg(fbo_reg);
}

void ObjectRenderer::init(bool hasSharedDisplayLists)
{
  if(simObject)
    graphicsManager->initContext(hasSharedDisplayLists);
}

void ObjectRenderer::draw()
{
  if(simObject)
  {
    if(visParams.cameraMode == VisualizationParameterSet::PRESENTATION)
      presentationCameraView();
    simulation->draw(cameraPos, cameraTarget, cameraUpVector,
      width, height, fovy,
      visParams, focus, simObject, fbo_reg);
    glFinish();
  }
}

void ObjectRenderer::resetCamera()
{
  // set a initial camera pos (position/rotation/target)
  cameraPos.v[0] = 0;
  cameraPos.v[1] = 2;
  cameraPos.v[2] = 0;
  cameraTarget.v[0] = 0;
  cameraTarget.v[1] = 0;
  cameraTarget.v[2] = 0;
  cameraUpVector.v[0] = 0;
  cameraUpVector.v[1] = 0;
  cameraUpVector.v[2] = 1;

  // rotate a bit
  cameraPos.rotateX(0.3);
  cameraUpVector.rotateX(0.3);
  // presentation cam
  presentationCirclecounter = 0;
  presentationDistance = 2.0;
  presentationSpeed = 20000;
}

void ObjectRenderer::setFocus(bool val)
{
  focus = val;
}

bool ObjectRenderer::hasFocus()
{
  return focus;
}

void ObjectRenderer::setSize(unsigned int w, unsigned int h)
{
  width = w;
  height = h;
}

void ObjectRenderer::getSize(unsigned int& w, unsigned int& h)
{
  w = width;
  h = height;
}

Simulation* ObjectRenderer::getSimulation() const
{
  return simulation;
}

void ObjectRenderer::setSimulation(Simulation* sim)
{
  simulation = sim;
  graphicsManager = sim ? sim->getGraphicsManager() : 0;
  simObject = sim ? sim->getObjectReference(object) : 0;
}

void ObjectRenderer::setObject(const std::string& object)
{
  simObject = simulation ? simulation->getObjectReference(object) : 0;
}

Vector3d ObjectRenderer::getObjectPos()
{
  Vector3d pos;
  if (!simObject)
    return pos;
  return simObject->getPosition();
}

void ObjectRenderer::setSurfaceStyle(VisualizationParameterSet::SurfaceStyle style)
{
  visParams.surfaceStyle = style;
}

VisualizationParameterSet::SurfaceStyle ObjectRenderer::getSurfaceStyle() const
{
  return visParams.surfaceStyle;
}

void ObjectRenderer::zoom(double change)
{
  if(visParams.cameraMode == VisualizationParameterSet::TARGETCAM)
    zoomTargetCam(change);
  else if(visParams.cameraMode == VisualizationParameterSet::PRESENTATION)
    zoomPresentationCam(change);
  else
    zoomFreeCam(change);
}

void ObjectRenderer::zoomTargetCam(double change)
{
  Vector3d viewingvec = cameraTarget - cameraPos;
  if (viewingvec.getLength() > 0.05)
    viewingvec *= (1.0 - change);
  else
    viewingvec *= (1.0 + fabs(change));
  cameraPos = cameraTarget - viewingvec;
}

void ObjectRenderer::zoomFreeCam(double change)
{
  Vector3d viewingvec = cameraTarget - cameraPos;
  viewingvec.normalize();
  viewingvec *= change;
  cameraTarget += viewingvec;
  cameraPos += viewingvec;
}

void ObjectRenderer::zoomPresentationCam(double change)
{
  presentationDistance += change;
  GLHelper::getGLH()->glPrintText(TextObject::COMMON_HINTS, TextObject::WHITE, 2000,
          "Camera Distance (m): %2.2f", presentationDistance);
}

VisualizationParameterSet::CameraMode ObjectRenderer::getCameraMode() const
{
  return visParams.cameraMode;
}

void ObjectRenderer::setCameraMode(VisualizationParameterSet::CameraMode mode)
{
  visParams.cameraMode = mode;
}

void ObjectRenderer::toggleCameraMode()
{
  int mode = int(getCameraMode());
  mode++;
  if(mode > VisualizationParameterSet::PRESENTATION)
    mode = VisualizationParameterSet::TARGETCAM;
  setCameraMode(VisualizationParameterSet::CameraMode(mode));
}

void ObjectRenderer::fitCamera()
{
  if(simObject)
    simulation->fitCamera(cameraTarget, cameraPos, width, height, simObject);
  calculateUpVector();
}

void ObjectRenderer::setSnapToRoot(bool on)
{
  visParams.snapToRoot = on;
}

bool ObjectRenderer::hasSnapToRoot() const
{
  return visParams.snapToRoot;
}

void ObjectRenderer::calculateUpVector()
{
  cameraUpVector = cameraTarget - cameraPos;
  cameraUpVector.normalize();
  double zrotation(cameraUpVector.getZRotation());
  cameraUpVector.rotateZ(zrotation);
  cameraUpVector.rotateX(pi_2);
  cameraUpVector.rotateZ(-zrotation);
}

void ObjectRenderer::presentationCameraView()
{
  // update cicle rotation
  unsigned int now = System::getTime();
  unsigned int timePassed = now - presentationCirclecounter;
  if(timePassed > presentationSpeed)
  {
    presentationCirclecounter = now;
    timePassed = 0;
  }

  // update camera position
  double timepos = timePassed;
  cameraPos.v[0] = cameraTarget.v[0] + presentationDistance * sin(timepos / presentationSpeed * 2.0 * pi);
  cameraPos.v[1] = cameraTarget.v[1] + presentationDistance * cos(timepos / presentationSpeed * 2.0 * pi);
  cameraPos.v[2] = cameraTarget.v[2] + presentationDistance / 2.0;

  cameraUpVector.v[0] = 0.0;
  cameraUpVector.v[1] = 0.0;
  cameraUpVector.v[2] = 1.0;
}

void ObjectRenderer::setFovY(int f)
{
  fovy = f;
}

int ObjectRenderer::getFovY() const
{
  return int(fovy);
}

void ObjectRenderer::setDragPlane(DragAndDropPlane plane)
{
  dragPlane = plane;
}

DragAndDropPlane ObjectRenderer::getDragPlane() const
{
  return dragPlane;
}

void ObjectRenderer::setDragMode(DragAndDropMode mode)
{
  dragMode = mode;
}

DragAndDropMode ObjectRenderer::getDragMode() const
{
  return dragMode;
}

bool ObjectRenderer::startDrag(int x, int y, DragType type) 
{  
  if(!simObject)
    return false;
  
  if(dragging)
    return true;

  // get selection
  dragSelection = simulation->selectObject(x, y, cameraPos, cameraTarget, cameraUpVector, 
    width, height, dragPlane, dragMode, simObject, true);

  if(dragSelection == NO_SELECTION) // Camera Control
  {
    dragStartPos.x = dragCurrentPos.x = x;
    dragStartPos.y = dragCurrentPos.y = y;
    dragging = true;
    dragType = type;
    return true;
  }
  else // Editor Control
  {
    if(dragSelection == OBJECT_SELECTION && dragMode == RESET_DYNAMICS)
        simulation->resetDynamicsForSelection();
  
    dragging = true;
    dragType = type;
    return true;
  }  
}

bool ObjectRenderer::moveDrag(int x, int y) 
{
  if(!simObject)
    return false;
  
  if(!dragging)
    return false;

  if(dragSelection == NO_SELECTION) // Camera Control
  {
    if(visParams.cameraMode == VisualizationParameterSet::TARGETCAM)
    {
      if(dragType == DRAG_ROTATE)
      {
        // get the translation vector
        Vector3d viewingvec;
        viewingvec.v[0] = (x - dragCurrentPos.x) * 0.01;
        viewingvec.v[1] = 0;
        viewingvec.v[2] = (y - dragCurrentPos.y) * 0.01;
        viewingvec.rotateZ( -(cameraTarget - cameraPos).getZRotation() );

        // and use it
        cameraPos += viewingvec;
        cameraTarget += viewingvec;
      }
      else // if(dragType == DRAG_NORMAL)
      {
        // shift camera position by camera target (generate viewing vector)
        cameraPos -= cameraTarget;

        // reverse z-rotation
        double zrotation(cameraPos.getZRotation());
        cameraPos.rotateZ( zrotation );
        cameraUpVector.rotateZ( zrotation );

        // rotate around the x-axis
        double xrotation((double)(y - dragCurrentPos.y)/100.0);
        if (cameraUpVector.v[2] < 0.0)
          xrotation = -xrotation;
        cameraPos.rotateX( xrotation );
        cameraUpVector.rotateX( xrotation );

        // rotate to old z-rotation
        cameraPos.rotateZ( -zrotation );
        cameraUpVector.rotateZ( -zrotation );

        // add new z-rotation
        cameraPos.rotateZ( (double)(dragCurrentPos.x - x)/100.0 );
        cameraUpVector.rotateZ( (double)(dragCurrentPos.x - x)/100.0 );

        // shift camera back into position
        cameraPos += cameraTarget;
      }
    }
    else if(visParams.cameraMode == VisualizationParameterSet::FREECAM)
    {
      if(dragType == DRAG_ROTATE)
      {
        // get the viewingvec and get some angles
        Vector3d viewingvec = cameraTarget - cameraPos;
        double zrotation(viewingvec.getZRotation());
        viewingvec.rotateZ( zrotation );
        double xrotation(viewingvec.getXRotation());

        // construct a new translation vector
        viewingvec.v[0] = (x - dragCurrentPos.x) * 0.01;
        viewingvec.v[1] = (y - dragCurrentPos.y) * 0.01;
        viewingvec.v[2] = 0.0;
        viewingvec.rotateX( -xrotation );
        viewingvec.rotateZ( -zrotation );

        // and use it
        cameraPos += viewingvec;
        cameraTarget += viewingvec;
      }
      else
      {
        // shift camera target by camera position (generate viewing vector)
        cameraTarget -= cameraPos;

        // reverse z-rotation
        double zrotation(cameraTarget.getZRotation());
        cameraTarget.rotateZ( zrotation );
        cameraUpVector.rotateZ( zrotation );

        // rotate around the x-axis
        double xrotation((double)(dragCurrentPos.y - y)/100.0);
        if (cameraUpVector.v[2] < 0.0)
          xrotation = -xrotation;
        cameraTarget.rotateX( xrotation );
        cameraUpVector.rotateX( xrotation );

        // rotate to old z-rotation
        cameraTarget.rotateZ( -zrotation );
        cameraUpVector.rotateZ( -zrotation );

        // add new z-rotation
        cameraTarget.rotateZ( (double)(dragCurrentPos.x - x)/200.0 );
        cameraUpVector.rotateZ( (double)(dragCurrentPos.x - x)/200.0 );

        // shift target back into position
        cameraTarget += cameraPos;
      }
    }
    else if(visParams.cameraMode == VisualizationParameterSet::PRESENTATION)
    {
      if(dragType == DRAG_ROTATE)
      {
        // do nothing
      }
      else
      {
        presentationSpeed += (int)((y - dragCurrentPos.y) * 10);
        GLHelper::getGLH()->glPrintText(TextObject::COMMON_HINTS, TextObject::WHITE, 2000,
          "Camera Speed (circles/sec): %1.3f", 1000.0/(double)presentationSpeed);
      }
    }

    dragCurrentPos.x = x;
    dragCurrentPos.y = y;
    return true;
  }
  
  else // Editor Control
  {
    if(dragSelection == OBJECT_SELECTION)
    {
      if(dragMode == APPLY_DYNAMICS)
      {
        if(dragType == DRAG_ROTATE)
          simulation->applyTorqueForSelection(x, y);
        else
          simulation->applyForceForSelection(x, y);
      }
      else
      {
        if(dragType == DRAG_ROTATE)
          simulation->rotateObject(x, y);
        else
          simulation->translateObject(x, y);          
      }
    }
    return true;
  }
}

bool ObjectRenderer::releaseDrag(int x, int y) 
{
  if(!simObject)
    return false;
  
  if(!dragging)
    return false;

  if(dragSelection == NO_SELECTION) // Camera Control
  { 
    dragCurrentPos.x = x;
    dragCurrentPos.y = y;
    dragging = false;
    return true;
  }
  else // Editor Control
  {
    simulation->unselectObject();
    dragging = false;
    return true;
  }
}

void ObjectRenderer::setShowDetailSensors(bool show)
{
  visParams.visualizeSensors = show;
}

bool ObjectRenderer::hasShowDetailSensors() const
{
  return visParams.visualizeSensors;
}

void ObjectRenderer::setShowControllerDrawings(bool show)
{
  visParams.enableControllerDrawings = show;
}

bool ObjectRenderer::hasShowControllerDrawings() const
{
  return visParams.enableControllerDrawings;
}

void ObjectRenderer::setCamera(const Vector3d& pos, const Vector3d& target)
{
  cameraPos = pos;
  cameraTarget = target;
  calculateUpVector();
}

void ObjectRenderer::getCamera(Vector3d& pos, Vector3d& target)
{
  pos = cameraPos;
  target = cameraTarget;
}

bool ObjectRenderer::renderOffscreen(void* image)
{
  if(!simObject)
    return false;
  
  OffscreenRenderer* os = simulation->getOffscreenRenderer();
  os->prepareRendering(width, height);
  draw();
  os->finishRendering(image, width, height, OffscreenRenderer::IMAGE_RGBA);
  return true;
}

void ObjectRenderer::precomputeSensorValues()
{
  // precompute sensor data
  if(simObject && visParams.visualizeSensors)
    simulation->precomputeSensorValues();
}