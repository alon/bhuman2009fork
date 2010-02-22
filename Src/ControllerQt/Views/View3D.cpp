/**
* @file ControllerQt/Views/View3D.cpp
*
* Implementation of class View3D
*
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
* @author Colin Graf
*/

#include <QGLWidget>
#include <QMouseEvent>

#include <Tools/Streams/OutStreams.h>
#include <Tools/Streams/InStreams.h>

#include "View3D.h"

class View3DWidget : public QGLWidget
{
public:
  View3DWidget(View3D& view3D) : view3D(view3D), dragging(false) {}
  
private:

  void resizeGL(int newWidth, int newHeight)
  {
    width = newWidth;
    height = newHeight;
  }

  void paintGL()
  {
    GLdouble aspect = height ? (GLdouble) width / (GLdouble) height : (GLdouble) width;

    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);
    glLineWidth(1.5); // required
    glPointSize(2.5);
    glPolygonMode(GL_FRONT,GL_LINE);
    glPolygonMode(GL_BACK,GL_LINE);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);

    if(!glIsList(View3D::cubeId) || view3D.needsUpdate())
      view3D.updateDisplayLists();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glClearColor(view3D.background.x, view3D.background.y, view3D.background.z, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(25, aspect, 1, 100);
    
    glTranslated(0.0, 0.0, -view3D.getViewDistance());
    glRotated(view3D.rotation.x, 1.0, 0.0, 0.0);
    glRotated(view3D.rotation.y, 0.0, 0.0, 1.0);

    glCallList(View3D::cubeId);
    glCallList(View3D::colorsId);

    view3D.lastBackground = view3D.background;
  }
  
  void mousePressEvent(QMouseEvent* event)
  {
    QWidget::mousePressEvent(event);
    
    if(event->button() == Qt::LeftButton || event->button() == Qt::MidButton)
    {
      dragStart = event->pos();
      dragging = true;
    }
  }
  
  void mouseReleaseEvent(QMouseEvent* event)
  {
    QWidget::mouseReleaseEvent(event);
    
    dragging = false;
  }
  
  void mouseMoveEvent(QMouseEvent* event)
  {
    QWidget::mouseMoveEvent(event);
    
    if(dragging)
    {
      QPoint diff(event->pos() - dragStart);
      dragStart = event->pos();
      view3D.rotation.y += diff.x();
      view3D.rotation.x += diff.y();
      updateGL();
    }
  }
  
  int width;
  int height;
  View3D& view3D;
  bool dragging;
  QPoint dragStart;
};

View3D::View3D(const Vector3<float>& background) : background(background) {}

QWidget* View3D::createWidget()
{
  return new View3DWidget(*this); 
}

void View3D::updateWidget(QWidget& widget)
{
  if(needsRepaint())
    ((View3DWidget*)&widget)->updateGL();
}

bool View3D::needsRepaint() const
{
  return background != lastBackground || needsUpdate();
}

std::string View3D::getConfiguration(QWidget& widget) const
{
  char configuration[100];
  OutTextMemory stream(configuration);
  stream << rotation.x << rotation.y;
  configuration[stream.getLength()] = 0;
  return configuration;
}

void View3D::setConfiguration(QWidget& widget, const std::string& configuration)
{
  InTextMemory stream(configuration.c_str(), configuration.size());
  stream >> rotation.x >> rotation.y;
}

