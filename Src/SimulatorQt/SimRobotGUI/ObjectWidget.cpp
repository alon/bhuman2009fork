
#include <QMouseEvent>
#include <QGLWidget>
#include <QApplication>
#include <QSettings>
#include <QMenu>
#include <QClipboard>
#include <QSignalMapper>

#include "ObjectWidget.h"

class ObjectGLWidget : public QGLWidget
{
public:
  ObjectGLWidget(ObjectRenderer& renderer, QWidget* parent) : QGLWidget(parent, objectGLWidgetList.isEmpty() ? 0 : objectGLWidgetList.first()),
    objectRenderer(renderer)
  {
    objectGLWidgetList.append(this);
    setFocusPolicy(Qt::StrongFocus);

    objectRenderer.setFocus(hasFocus());
  }

  ~ObjectGLWidget()
  {
    objectGLWidgetList.removeAll(this);
  }

private:
  static QList<ObjectGLWidget*> objectGLWidgetList;

  ObjectRenderer& objectRenderer;

  void initializeGL()
  {
    objectRenderer.init(isSharing());
  }

  void paintGL()
  {
    objectRenderer.precomputeSensorValues();
    makeCurrent();
    objectRenderer.draw();
  }

  void resizeGL(int width, int height)
  {
    objectRenderer.setSize(width, height);
  }

  void focusInEvent(QFocusEvent * event)
  {
    QGLWidget::focusInEvent(event);

    objectRenderer.setFocus(true);
  }

  void focusOutEvent(QFocusEvent * event)
  {
    QGLWidget::focusOutEvent(event);

    objectRenderer.setFocus(true);
  }

  void mouseMoveEvent(QMouseEvent* event)
  {
    QGLWidget::mouseMoveEvent(event);

    if(objectRenderer.moveDrag(event->x(), event->y()))
    {
      event->accept();
      updateGL();
    }
  }

  void mousePressEvent(QMouseEvent* event)
  {
    QGLWidget::mousePressEvent(event);

    if(!objectRenderer.hasFocus())
      return;
    updateGL();

    if(event->button() == Qt::LeftButton || event->button() == Qt::MidButton)
      if(objectRenderer.startDrag(event->x(), event->y(), QApplication::keyboardModifiers() & Qt::ShiftModifier ? ObjectRenderer::DRAG_ROTATE : ObjectRenderer::DRAG_NORMAL))
      {
        event->accept();
        updateGL();
      }
  }

  void mouseReleaseEvent(QMouseEvent* event)
  {
    QGLWidget::mouseReleaseEvent(event);

    if(objectRenderer.releaseDrag(event->x(), event->y()))
    {
      event->accept();
      updateGL();
    }
    /*
    else if(event->button() == Qt::RightButton)
    {

    } */
  }


  void keyPressEvent(QKeyEvent* event)
  {
    switch(event->key())
    {
    case Qt::Key_PageUp:
    case Qt::Key_Plus:
      event->accept();
      objectRenderer.zoom(0.05);
      updateGL();
      break;

    case Qt::Key_PageDown:
    case Qt::Key_Minus:
      event->accept();
      objectRenderer.zoom(-0.05);
      updateGL();
      break;

    default:
      QGLWidget::keyPressEvent(event);
      break;
    }
  }

  void wheelEvent(QWheelEvent* event)
  {
    if(event->orientation() == Qt::Vertical)
    {
      objectRenderer.zoom(-0.05 * event->delta() / 160);
      updateGL();
      event->accept();
    }
    else
      QGLWidget::wheelEvent(event);
  }

  QSize sizeHint () const { return QSize(480, 320); }

};


QList<ObjectGLWidget*> ObjectGLWidget::objectGLWidgetList;

ObjectWidget::ObjectWidget(const SceneObject& object, QWidget* parent) : DockWidget(object, parent),
  objectRenderer(Document::document->getSimulation(), object.name.toAscii().constData()), objectGlWidget(0),
  showSensorsAct(0), showDrawingsAct(0)
{
  connect(Document::document, SIGNAL(updatedSceneGraph()), this, SLOT(updateObject()));
  connect(Document::document, SIGNAL(restoreLayoutSignal()), this, SLOT(restoreLayout()));
  connect(Document::document, SIGNAL(writeLayoutSignal()), this, SLOT(writeLayout()));
  connect(Document::document, SIGNAL(updateViewsSignal()), this, SLOT(updateView()));
  updateObject();
}

void ObjectWidget::updateObject()
{
  Simulation* sim = Document::document->getSimulation();
  if(objectRenderer.getSimulation() != sim || (!objectGlWidget && sim))
  {
    objectRenderer.setSimulation(sim);
    QWidget* oldWidget = objectGlWidget;
    objectGlWidget = sim ? new ObjectGLWidget(objectRenderer, this) : 0;
    setWidget(objectGlWidget);
    setFocusProxy(objectGlWidget);
    if(oldWidget)
      delete oldWidget;
  }
  else
    objectRenderer.setObject(object.name.toAscii().constData());
}

void ObjectWidget::restoreLayout()
{
  QSettings* settings = Document::document->getLayoutSettings();
  settings->beginGroup(object.name);

  objectRenderer.setSurfaceStyle(VisualizationParameterSet::SurfaceStyle(settings->value("SurfaceStyle", int(objectRenderer.getSurfaceStyle())).toInt()));
  objectRenderer.setCameraMode(VisualizationParameterSet::CameraMode(settings->value("CameraMode", int(objectRenderer.getCameraMode())).toInt()));
  objectRenderer.setSnapToRoot(settings->value("SnapToRoot", objectRenderer.hasSnapToRoot()).toBool());
  objectRenderer.setFovY(settings->value("FovY", objectRenderer.getFovY()).toInt());
  objectRenderer.setDragPlane(DragAndDropPlane(settings->value("DragPlane", int(objectRenderer.getDragPlane())).toInt()));
  objectRenderer.setDragMode(DragAndDropMode(settings->value("DragMode", int(objectRenderer.getDragMode())).toInt()));
  objectRenderer.setShowDetailSensors(settings->value("ShowDetailSensors", objectRenderer.hasShowDetailSensors()).toBool());
  objectRenderer.setShowControllerDrawings(settings->value("ShowControllerDrawings", objectRenderer.hasShowControllerDrawings()).toBool());

  Vector3d pos, target;
  objectRenderer.getCamera(pos, target);

  pos.v[0] = settings->value("cameraPosX", pos.v[0]).toDouble();
  pos.v[1] = settings->value("cameraPosY", pos.v[1]).toDouble();
  pos.v[2] = settings->value("cameraPosZ", pos.v[2]).toDouble();
  target.v[0] = settings->value("cameraTargetX", target.v[0]).toDouble();
  target.v[1] = settings->value("cameraTargetY", target.v[1]).toDouble();
  target.v[2] = settings->value("cameraTargetZ", target.v[2]).toDouble();

  objectRenderer.setCamera(pos, target);

  settings->endGroup();
}

void ObjectWidget::writeLayout()
{
  QSettings* settings = Document::document->getLayoutSettings();
  settings->beginGroup(object.name);

  settings->setValue("SurfaceStyle", int(objectRenderer.getSurfaceStyle()));
  settings->setValue("CameraMode", int(objectRenderer.getCameraMode()));
  settings->setValue("SnapToRoot", objectRenderer.hasSnapToRoot());
  settings->setValue("FovY", objectRenderer.getFovY());
  settings->setValue("DragPlane", int(objectRenderer.getDragPlane()));
  settings->setValue("DragMode", int(objectRenderer.getDragMode()));
  settings->setValue("ShowDetailSensors", objectRenderer.hasShowDetailSensors());
  settings->setValue("ShowControllerDrawings", objectRenderer.hasShowControllerDrawings());

  Vector3d pos, target;
  objectRenderer.getCamera(pos, target);

  settings->setValue("cameraPosX", pos.v[0]);
  settings->setValue("cameraPosY", pos.v[1]);
  settings->setValue("cameraPosZ", pos.v[2]);
  settings->setValue("cameraTargetX", target.v[0]);
  settings->setValue("cameraTargetY", target.v[1]);
  settings->setValue("cameraTargetZ", target.v[2]);

  settings->endGroup();
}

void ObjectWidget::updateView()
{
  if(isReallyVisible() && objectGlWidget)
    objectGlWidget->updateGL();
}

QMenu* ObjectWidget::createPopupMenu()
{
  QMenu* menu = new QMenu(tr("&Object"));
  QAction* action;
  action = menu->addAction(QIcon(":/icons/page_copy.png"), tr("&Copy"));
  action->setShortcut(QKeySequence(QKeySequence::Copy));
  action->setStatusTip(tr("Copy the current selection's contents or view to the clipboard"));
  connect(action, SIGNAL(triggered()), this, SLOT(copy()));

  menu->addSeparator();
  
  QMenu* subMenu = menu->addMenu(tr("&Drag and Drop"));
  action = subMenu->menuAction();
  action->setIcon(QIcon(":/icons/DragPlane.png"));
  action->setStatusTip(tr("Select the drag and drop dynamics mode and plane along which operations are performed"));
  QActionGroup* actionGroup = new QActionGroup(subMenu);
  QSignalMapper* signalMapper = new QSignalMapper(subMenu);
  connect(signalMapper, SIGNAL(mapped(int)), SLOT(setDragPlane(int)));
  action = subMenu->addAction(tr("X/Y Plane"));
  actionGroup->addAction(action);
  signalMapper->setMapping(action, XY_PLANE);
  action->setShortcut(QKeySequence(Qt::Key_Z));
  action->setCheckable(true);
  connect(action, SIGNAL(triggered()), signalMapper, SLOT(map()));
  action = subMenu->addAction(tr("X/Z Plane"));
  actionGroup->addAction(action);
  signalMapper->setMapping(action, XZ_PLANE);
  action->setShortcut(QKeySequence(Qt::Key_Y));
  action->setCheckable(true);
  connect(action, SIGNAL(triggered()), signalMapper, SLOT(map()));
  action = subMenu->addAction(tr("Y/Z Plane"));
  actionGroup->addAction(action);
  signalMapper->setMapping(action, YZ_PLANE);
  action->setShortcut(QKeySequence(Qt::Key_X));
  action->setCheckable(true);
  connect(action, SIGNAL(triggered()), signalMapper, SLOT(map()));
  qobject_cast<QAction*>(signalMapper->mapping(objectRenderer.getDragPlane()))->setChecked(true);
  subMenu->addSeparator();
  actionGroup = new QActionGroup(subMenu);
  signalMapper = new QSignalMapper(subMenu);
  connect(signalMapper, SIGNAL(mapped(int)), SLOT(setDragMode(int)));
  action = subMenu->addAction(tr("&Keep Dynamics"));
  actionGroup->addAction(action);
  signalMapper->setMapping(action, KEEP_DYNAMICS);
  action->setShortcut(QKeySequence(Qt::Key_8));
  action->setCheckable(true);
  connect(action, SIGNAL(triggered()), signalMapper, SLOT(map()));
  action = subMenu->addAction(tr("&Reset Dynamics"));
  actionGroup->addAction(action);
  signalMapper->setMapping(action, RESET_DYNAMICS);
  action->setShortcut(QKeySequence(Qt::Key_9));
  action->setCheckable(true);
  connect(action, SIGNAL(triggered()), signalMapper, SLOT(map()));
  action = subMenu->addAction(tr("&Apply Dynamics"));
  actionGroup->addAction(action);
  signalMapper->setMapping(action, APPLY_DYNAMICS);
  action->setShortcut(QKeySequence(Qt::Key_0));
  action->setCheckable(true);
  connect(action, SIGNAL(triggered()), signalMapper, SLOT(map()));
  qobject_cast<QAction*>(signalMapper->mapping(objectRenderer.getDragMode()))->setChecked(true);

  menu->addSeparator();

  subMenu = menu->addMenu(tr("&Camera"));
  action = subMenu->menuAction();
  action->setIcon(QIcon(":/icons/camera.png"));
  action->setStatusTip(tr("Select different camera modes for displaying the scene"));
  actionGroup = new QActionGroup(subMenu);
  signalMapper = new QSignalMapper(subMenu);
  connect(signalMapper, SIGNAL(mapped(int)), SLOT(setCameraMode(int)));
  action = subMenu->addAction(tr("&Target Mode"));
  action->setCheckable(true);
  actionGroup->addAction(action);
  signalMapper->setMapping(action, VisualizationParameterSet::TARGETCAM);
  connect(action, SIGNAL(triggered()), signalMapper, SLOT(map()));
  action = subMenu->addAction(tr("&Free Mode"));
  action->setCheckable(true);
  actionGroup->addAction(action);
  signalMapper->setMapping(action, VisualizationParameterSet::FREECAM);
  connect(action, SIGNAL(triggered()), signalMapper, SLOT(map()));
  action = subMenu->addAction(tr("&Presentation Mode"));
  action->setCheckable(true);
  actionGroup->addAction(action);
  signalMapper->setMapping(action, VisualizationParameterSet::PRESENTATION);
  connect(action, SIGNAL(triggered()), signalMapper, SLOT(map()));
  qobject_cast<QAction*>(signalMapper->mapping(objectRenderer.getCameraMode()))->setChecked(true);
  subMenu->addSeparator();
  action = subMenu->addAction(tr("&Reset"));
  action->setShortcut(QKeySequence(Qt::Key_R));
  connect(action, SIGNAL(triggered()), this, SLOT(resetCamera()));
  action = subMenu->addAction(tr("&Toggle"));
  action->setShortcut(QKeySequence(Qt::Key_T));
  connect(action, SIGNAL(triggered()), this, SLOT(toggleCameraMode()));
  action = subMenu->addAction(tr("&Fit"));
  action->setShortcut(QKeySequence(Qt::Key_F));
  connect(action, SIGNAL(triggered()), this, SLOT(fitCamera()));
  subMenu->addSeparator();
  action = subMenu->addAction(tr("&Snap To Root"));
  action->setCheckable(true);
  action->setChecked(objectRenderer.hasSnapToRoot());
  connect(action, SIGNAL(triggered()), this, SLOT(snapToRoot()));

  subMenu = menu->addMenu(tr("&Vertical Opening Angle"));
  action = subMenu->menuAction();
  action->setIcon(QIcon(":/icons/opening_angle.png"));
  actionGroup = new QActionGroup(subMenu);
  signalMapper = new QSignalMapper(subMenu);
  connect(signalMapper, SIGNAL(mapped(int)), SLOT(setFovY(int)));
  action = subMenu->addAction(tr("&20°"));
  actionGroup->addAction(action);
  signalMapper->setMapping(action, 20);
  action->setShortcut(QKeySequence(Qt::Key_1));
  action->setCheckable(true);
  connect(action, SIGNAL(triggered()), signalMapper, SLOT(map()));
  action = subMenu->addAction(tr("&40°"));
  actionGroup->addAction(action);
  signalMapper->setMapping(action, 40);
  action->setShortcut(QKeySequence(Qt::Key_2));
  action->setCheckable(true);
  connect(action, SIGNAL(triggered()), signalMapper, SLOT(map()));
  action = subMenu->addAction(tr("&60°"));
  actionGroup->addAction(action);
  signalMapper->setMapping(action, 60);
  action->setShortcut(QKeySequence(Qt::Key_3));
  action->setCheckable(true);
  connect(action, SIGNAL(triggered()), signalMapper, SLOT(map()));
  action = subMenu->addAction(tr("&80°"));
  actionGroup->addAction(action);
  signalMapper->setMapping(action, 80);
  action->setShortcut(QKeySequence(Qt::Key_4));
  action->setCheckable(true);
  connect(action, SIGNAL(triggered()), signalMapper, SLOT(map()));
  action = subMenu->addAction(tr("100°"));
  actionGroup->addAction(action);
  signalMapper->setMapping(action, 100);
  action->setShortcut(QKeySequence(Qt::Key_5));
  action->setCheckable(true);
  connect(action, SIGNAL(triggered()), signalMapper, SLOT(map()));
  action = subMenu->addAction(tr("120°"));
  actionGroup->addAction(action);
  signalMapper->setMapping(action, 120);
  action->setShortcut(QKeySequence(Qt::Key_6));
  action->setCheckable(true);
  connect(action, SIGNAL(triggered()), signalMapper, SLOT(map()));
  qobject_cast<QAction*>(signalMapper->mapping(objectRenderer.getFovY()))->setChecked(true);

  subMenu = menu->addMenu(tr("&Surface Rendering"));
  actionGroup = new QActionGroup(subMenu);
  signalMapper = new QSignalMapper(subMenu);
  connect(signalMapper, SIGNAL(mapped(int)), SLOT(setSurfaceStyle(int)));
  action = subMenu->menuAction();
  action->setIcon(QIcon(":/icons/layers.png"));
  action->setStatusTip(tr("Select different rendering techniques for displaying the scene"));
  action = subMenu->addAction(tr("Appearance &Wire Frame"));
  actionGroup->addAction(action);
  action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_W));
  action->setCheckable(true);
  signalMapper->setMapping(action, VisualizationParameterSet::WIREFRAME);
  connect(action, SIGNAL(triggered()), signalMapper, SLOT(map()));
  action = subMenu->addAction(tr("Appearance &Flat Shading"));
  actionGroup->addAction(action);
  action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_F));
  action->setCheckable(true);
  signalMapper->setMapping(action, VisualizationParameterSet::FLAT_SHADING);
  connect(action, SIGNAL(triggered()), signalMapper, SLOT(map()));
  action = subMenu->addAction(tr("Appearance &Smooth Shading"));
  actionGroup->addAction(action);
  action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_M));
  action->setCheckable(true);
  signalMapper->setMapping(action, VisualizationParameterSet::SMOOTH_SHADING);
  connect(action, SIGNAL(triggered()), signalMapper, SLOT(map()));
  action = subMenu->addAction(tr("Appearance &Texture Shading"));
  actionGroup->addAction(action);
  action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_T));
  action->setCheckable(true);
  signalMapper->setMapping(action, VisualizationParameterSet::TEXTURE_SHADING);
  connect(action, SIGNAL(triggered()), signalMapper, SLOT(map()));
  action = subMenu->addAction(tr("Appearance &Camera Simulation"));
  actionGroup->addAction(action);
  action->setCheckable(true);
  signalMapper->setMapping(action, VisualizationParameterSet::CAMERA_SIMULATION);
  connect(action, SIGNAL(triggered()), signalMapper, SLOT(map()));
  subMenu->addSeparator();
  action = subMenu->addAction(tr("Physics &Wire Frame"));
  actionGroup->addAction(action);
  action->setCheckable(true);
  signalMapper->setMapping(action, VisualizationParameterSet::PHYSICS_WIREFRAME);
  connect(action, SIGNAL(triggered()), signalMapper, SLOT(map()));
  action = subMenu->addAction(tr("Physics &Flat Shading"));
  actionGroup->addAction(action);
  action->setCheckable(true);
  signalMapper->setMapping(action, VisualizationParameterSet::PHYSICS_FLAT);
  connect(action, SIGNAL(triggered()), signalMapper, SLOT(map()));
  action = subMenu->addAction(tr("Physics &Smooth Shading"));
  actionGroup->addAction(action);
  action->setCheckable(true);  
  signalMapper->setMapping(action, VisualizationParameterSet::PHYSICS_SMOOTH);
  connect(action, SIGNAL(triggered()), signalMapper, SLOT(map()));
  subMenu->addSeparator();
  action = subMenu->addAction(tr("Mixed Mode &1 (Shaded Physics)"));
  actionGroup->addAction(action);
  action->setCheckable(true);
  signalMapper->setMapping(action, VisualizationParameterSet::MIXED_PHYSICS);
  connect(action, SIGNAL(triggered()), signalMapper, SLOT(map()));
  action = subMenu->addAction(tr("Mixed Mode &2 (Shaded Appearance)"));
  actionGroup->addAction(action);
  action->setCheckable(true);
  signalMapper->setMapping(action, VisualizationParameterSet::MIXED_APPEARANCE);
  connect(action, SIGNAL(triggered()), signalMapper, SLOT(map()));
  qobject_cast<QAction*>(signalMapper->mapping(objectRenderer.getSurfaceStyle()))->setChecked(true);

  menu->addSeparator();

  if(!showSensorsAct)
  {
    showSensorsAct = new QAction(QIcon(":/icons/transmit_go.png"), tr("Show &Sensors"), this);
    showSensorsAct->setStatusTip(tr("Show the values of the sensors in the scene view"));
    showSensorsAct->setCheckable(true);
    showSensorsAct->setChecked(objectRenderer.hasShowDetailSensors());
    connect(showSensorsAct, SIGNAL(triggered()), this, SLOT(showDetailSensors()));
  }
  menu->addAction(showSensorsAct);

  if(!showDrawingsAct)
  {
    showDrawingsAct = new QAction(QIcon(":/icons/chart_line.png"), tr("Show Controller &Drawings"), this);
    showDrawingsAct->setStatusTip(tr("Show the controller 3D drawings in the scene view"));
    showDrawingsAct->setCheckable(true);
    showDrawingsAct->setChecked(objectRenderer.hasShowControllerDrawings());
    connect(showDrawingsAct, SIGNAL(triggered()), this, SLOT(showControllerDrawings()));
  }
  menu->addAction(showDrawingsAct);

  return menu;
}

void ObjectWidget::copy()
{
  QApplication::clipboard()->clear();
  if(objectGlWidget)
    QApplication::clipboard()->setPixmap(QPixmap::fromImage(objectGlWidget->grabFrameBuffer()));
}

void ObjectWidget::setSurfaceStyle(int style)
{
  objectRenderer.setSurfaceStyle(VisualizationParameterSet::SurfaceStyle(style));
  if(objectGlWidget)
    objectGlWidget->updateGL();
}

void ObjectWidget::setCameraMode(int mode)
{
  objectRenderer.setCameraMode(VisualizationParameterSet::CameraMode(mode));
  if(objectGlWidget)
    objectGlWidget->updateGL();
}

void ObjectWidget::setFovY(int fovy)
{
  objectRenderer.setFovY(fovy);
  if(objectGlWidget)
    objectGlWidget->updateGL();
}

void ObjectWidget::setDragPlane(int plane)
{
  objectRenderer.setDragPlane(DragAndDropPlane(plane));
  if(objectGlWidget)
    objectGlWidget->updateGL();
}

void ObjectWidget::setDragMode(int mode)
{
  objectRenderer.setDragMode(DragAndDropMode(mode));
  if(objectGlWidget)
    objectGlWidget->updateGL();
}

void ObjectWidget::resetCamera()
{
  objectRenderer.resetCamera();
  if(objectGlWidget)
    objectGlWidget->updateGL();
}

void ObjectWidget::toggleCameraMode()
{
  objectRenderer.toggleCameraMode();
  if(objectGlWidget)
    objectGlWidget->updateGL();
}

void ObjectWidget::fitCamera()
{
  objectRenderer.fitCamera();
  if(objectGlWidget)
    objectGlWidget->updateGL();
}

void ObjectWidget::snapToRoot()
{
  objectRenderer.setSnapToRoot(!objectRenderer.hasSnapToRoot());
  if(objectGlWidget)
    objectGlWidget->updateGL();
}

void ObjectWidget::showDetailSensors()
{
  objectRenderer.setShowDetailSensors(!objectRenderer.hasShowDetailSensors());
  if(objectGlWidget)
    objectGlWidget->updateGL();
}

void ObjectWidget::showControllerDrawings()
{
  objectRenderer.setShowControllerDrawings(!objectRenderer.hasShowControllerDrawings());
  if(objectGlWidget)
    objectGlWidget->updateGL();
}

