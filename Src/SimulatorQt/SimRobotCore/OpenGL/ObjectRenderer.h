

#ifndef ObjectRenderer_H
#define ObjectRenderer_H


/*

// Example usage of ObjectRenderer:

class MyGLWidget : public QGLWidget
{
public:

  MyGLWidget(QWidget * parent, Simulation* simulation, const std::string& object) : QGLWidget(parent),
    objectRenderer(simulation, object) {}

private:

  ObjectRenderer objectRenderer;

  void initializeGL()
  {
    objectRenderer.init(isSharing());
  }

  void paintGL()
  {
    objectRenderer.draw();
  }

  void resizeGL(int width, int height)
  { 
    objectRenderer.setSize(width, height);
  }

  void setFovSlot(int fov)
  {
    objectRenderer.setFov(fov);
  }

  // ...
};

//... 

Simulation simulation();
simulation.loadFile(...)

//...

MyGLWidget* myGLWidget = new MyGLWidget(parent, &simulation, "myobject");


*/

#include "../Tools/SimMath.h"
#include "../Simulation/APIDatatypes.h"

class Simulation;
class GraphicsManager;
class SimObject;

class ObjectRenderer
{
public:

  enum DragType
  {
    DRAG_NORMAL,
    DRAG_ROTATE
  };

  ObjectRenderer(Simulation* simulation, const std::string& object);
  ~ObjectRenderer();

  void init(bool hasSharedDisplayLists); /**< Initializes the OpenGL context. */
  void draw(); 

  void setFocus(bool hasFocus);
  bool hasFocus();
  void setSize(unsigned int width, unsigned int height);
  void getSize(unsigned int& width, unsigned int& height);
  Simulation* getSimulation() const;
  void setSimulation(Simulation* simulation);
  void setObject(const std::string& object);
  Vector3d getObjectPos();
  void setSurfaceStyle(VisualizationParameterSet::SurfaceStyle style);
  VisualizationParameterSet::SurfaceStyle getSurfaceStyle() const;
  void zoom(double change);  
  void setCameraMode(VisualizationParameterSet::CameraMode mode);
  VisualizationParameterSet::CameraMode getCameraMode() const;
  void toggleCameraMode();
  void resetCamera();  
  void fitCamera();
  void setSnapToRoot(bool on);
  bool hasSnapToRoot() const;
  void setFovY(int fovY);
  int getFovY() const;
  void setDragPlane(DragAndDropPlane plane);
  DragAndDropPlane getDragPlane() const;
  void setDragMode(DragAndDropMode mode);
  DragAndDropMode getDragMode() const;
  bool startDrag(int x, int y, DragType type);
  bool moveDrag(int x, int y);
  bool releaseDrag(int x, int y);
  void setShowDetailSensors(bool show);
  bool hasShowDetailSensors() const;
  void setShowControllerDrawings(bool show);
  bool hasShowControllerDrawings() const;
  void setCamera(const Vector3d& pos, const Vector3d& target);
  void getCamera(Vector3d& pos, Vector3d& target);
  void precomputeSensorValues();
  
  /** Renders the object into a buffer by using the offscreen renderer.
  * @param image The buffer to store the image to. This buffer should offer space for width * height * 4 bytes.
  * @return True on success.
  */
  bool renderOffscreen(void* image);

private:
  Simulation* simulation;
  GraphicsManager* graphicsManager;
  std::string object;
  SimObject* simObject;

  Vector3d cameraPos;
  Vector3d cameraTarget;
  Vector3d cameraUpVector;
  unsigned int width;
  unsigned int height;
  double fovy;
  VisualizationParameterSet visParams;
  int fbo_reg;
  bool focus;

  double presentationDistance;
  unsigned int presentationSpeed;
  unsigned int presentationCirclecounter;

  class Point
  {
  public:
    Point(int x, int y) : x(x), y(y) {}
    Point() : x(0), y(0) {}
    int x;
    int y;
  };

  bool dragging;
  DragType dragType;
  InteractiveSelectionType dragSelection;
  Point dragStartPos;
  Point dragCurrentPos;
  DragAndDropPlane dragPlane;
  DragAndDropMode dragMode;

  void zoomTargetCam(double change);
  void zoomFreeCam(double change);
  void zoomPresentationCam(double change);
  void calculateUpVector();
  void presentationCameraView();
};

#endif

