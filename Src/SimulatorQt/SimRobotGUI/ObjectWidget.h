

#ifndef ObjectWidget_H
#define ObjectWidget_H

#include <OpenGL/ObjectRenderer.h>

#include "DockWidget.h"

class QGLWidget;

class ObjectWidget : public DockWidget
{
  Q_OBJECT

public:
  ObjectWidget(const SceneObject& object, QWidget* parent);

  QMenu* createPopupMenu();

private slots:
  void updateObject();
  void restoreLayout();
  void writeLayout();
  void updateView();

  void copy();
  void setSurfaceStyle(int style);
  void setCameraMode(int mode);
  void setFovY(int fovy);
  void setDragPlane(int plane);
  void setDragMode(int mode);
  void resetCamera();
  void toggleCameraMode();
  void fitCamera();
  void snapToRoot();
  void showDetailSensors();
  void showControllerDrawings();

private:
  ObjectRenderer objectRenderer;
  QGLWidget* objectGlWidget;

  QAction* showSensorsAct;
  QAction* showDrawingsAct;
};

#endif