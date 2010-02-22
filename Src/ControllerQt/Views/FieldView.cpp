/**
* @file ControllerQt/Views/FieldView.cpp
*
* Implementation of class FieldView
*
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
* @author Colin Graf
*/

#include <QWidget>
#include <QPainter>
#include <QApplication>
#include <QMouseEvent>
#include <QResizeEvent>

#include <Representations/Configuration/FieldDimensions.h>
#include <Platform/Thread.h>

#include "FieldView.h"
#include "../RobotConsole.h"
#include "../Visualization/PaintMethods.h"

class FieldWidget : public QWidget
{
public:
  FieldWidget(RobotConsole& console, const std::string& name) :
    console(console), name(name),
    lastDrawingsTimeStamp(0), zoom(1.f)
  {
    fieldDimensions.load();
    setMouseTracking(true);
  }

  void update()
  {
    if(needsRepaint())
      QWidget::update();
  }

private:
  RobotConsole& console;
  const std::string& name;
  unsigned int lastDrawingsTimeStamp;
  QPainter painter;
  FieldDimensions fieldDimensions; /**< The field dimensions. */
  float scale;
  float zoom;
  QPoint offset;
  QPoint dragStart;
  QPoint dragStartOffset;

  void paintEvent(QPaintEvent *event)
  {     
    painter.begin(this);

    const QSize& size(this->size());
    int viewWidth = int(fieldDimensions.xPosOpponentFieldBorder) * 2;
    int viewHeight = int(fieldDimensions.yPosLeftFieldBorder) * 2;
    float xScale = float(size.width()) / viewWidth;
    float yScale = float(size.height()) / viewHeight;
    scale = xScale < yScale ? xScale : yScale;
    scale *= zoom;
    painter.setTransform(QTransform(scale, 0, 0, -scale, size.width() / 2. - offset.x()*scale, size.height() / 2. - offset.y()*scale));

    {
      SYNC_WITH(console);      
      paintDrawings();
    }

    painter.end();
  }

  void paintDrawings()
  {
    const QTransform baseTrans(painter.transform());
    const std::list<std::string>& drawings(console.fieldViews[name]);
    for(std::list<std::string>::const_iterator i = drawings.begin(), end = drawings.end(); i != end; ++i)
    {
      const DebugDrawing& debugDrawing(console.fieldDrawings[*i]);
      PaintMethods::paintDebugDrawing(painter, debugDrawing, baseTrans);
      if(debugDrawing.timeStamp > lastDrawingsTimeStamp)
        lastDrawingsTimeStamp = debugDrawing.timeStamp;
    }
    painter.setTransform(baseTrans);
  }

  bool needsRepaint() const
  {
    SYNC_WITH(console);
    const std::list<std::string>& drawings(console.fieldViews[name]);
    for(std::list<std::string>::const_iterator i = drawings.begin(), end = drawings.end(); i != end; ++i)
    {
      const DebugDrawing& debugDrawing(console.fieldDrawings[*i]);
      if(debugDrawing.timeStamp > lastDrawingsTimeStamp)
        return true;
    }
    return false;
  }

  void window2viewport(QPoint& point)
  {
    const QSize& size(this->size());
    point = QPoint((point.x() - size.width() / 2) / scale, (point.y() - size.height() / 2) / scale);
    point += offset;
  }

  void mouseMoveEvent(QMouseEvent* event)
  {
    if(dragStart.x() > 0)
    {
      const QPoint& dragPos(event->pos());
      offset = dragStartOffset + (dragStart - dragPos) / scale;
      QWidget::update();
    }

    {
      SYNC_WITH(console);
      if(!console.colorTableHandler.active)
      {
        QPoint pos(event->pos());
        window2viewport(pos);
        const char* text = 0;
        const std::list<std::string>& drawings(console.fieldViews[name]);
        for(std::list<std::string>::const_iterator i = drawings.begin(), end = drawings.end(); i != end; ++i)
        {
          text = console.fieldDrawings[*i].getTip(pos.rx(), pos.ry());
          if(text)
            break;
        }
        if(text)
          setToolTip(QString(text));
        else
          setToolTip(QString());
      }
    }
  }

  void keyPressEvent(QKeyEvent* event)
  {
    switch(event->key())
    {
    case Qt::Key_PageUp:
    case Qt::Key_Plus:
      event->accept();
      if(zoom < 4.f)
        zoom += 0.1f;
      if(zoom > 4.f)
        zoom = 4.f;
      QWidget::update();
      break;
    case Qt::Key_PageDown:
    case Qt::Key_Minus:
      event->accept();
      if(zoom > 0.1f)
        zoom -= 0.1f;
      QWidget::update();
      break;
    case Qt::Key_Down:
      event->accept();
      offset += QPoint(0,100);
      QWidget::update();
      break;
    case Qt::Key_Up:
      event->accept();
      offset += QPoint(0,-100);
      QWidget::update();
      break;
    case Qt::Key_Left:
      event->accept();
      offset += QPoint(-100,0);
      QWidget::update();
      break;
    case Qt::Key_Right:
      event->accept();
      offset += QPoint(100, 0);
      QWidget::update();
      break;
    default:
      QWidget::keyPressEvent(event);
      break;
    }
  }

  void wheelEvent(QWheelEvent* event)
  {
    QWidget::wheelEvent(event);

    zoom += 0.1 * event->delta()/120;
    if(zoom > 4.f)
      zoom = 4.f;
    else if(zoom < 0.1f)
      zoom = 0.1f;
    QWidget::update();
  }

  void mousePressEvent(QMouseEvent *event)
  {
    QWidget::mousePressEvent(event);

    if(event->button() == Qt::LeftButton || event->button() == Qt::MidButton)
    {
      dragStart = event->pos();
      dragStartOffset = offset;
    }
  }

  void mouseReleaseEvent(QMouseEvent *event)
  {
    QWidget::mouseReleaseEvent(event);

    dragStart = QPoint(-1, -1);
  }

  void mouseDoubleClickEvent(QMouseEvent* event)
  {
    QWidget::mouseDoubleClickEvent(event);
    zoom = 1;
    offset = QPoint(0,0);
    QWidget::update();
  }

  QSize sizeHint () const { return QSize(fieldDimensions.xPosOpponentFieldBorder * 2 / 10, fieldDimensions.yPosLeftFieldBorder * 2 / 10); }

  friend class FieldView;
};

FieldView::FieldView(RobotConsole& console, const std::string& name) : 
  console(console), name(name) {}

QWidget* FieldView::createWidget()
{
  return new FieldWidget(console, name);
}

void FieldView::updateWidget(QWidget& widget)
{
  ((FieldWidget*)(&widget))->update();
}

std::string FieldView::getConfiguration(QWidget& widget) const
{
  FieldWidget* fieldWidget = (FieldWidget*)&widget;
  QByteArray block;
  QDataStream out(&block, QIODevice::WriteOnly);
  out.setVersion(QDataStream::Qt_4_3);
  out << fieldWidget->zoom;
  out << fieldWidget->offset;
  return std::string(block.toBase64().constData());
}

void FieldView::setConfiguration(QWidget& widget, const std::string& conf)
{
  FieldWidget* fieldWidget = (FieldWidget*)&widget;
  const QByteArray& block(QByteArray::fromBase64(conf.c_str()));
  QDataStream in(block);
  in.setVersion(QDataStream::Qt_4_3);
  float zoom;
  in >> zoom;
  if(zoom)
    fieldWidget->zoom = zoom;
  QPoint offset;
  in >> offset;
  fieldWidget->offset = offset;
}
