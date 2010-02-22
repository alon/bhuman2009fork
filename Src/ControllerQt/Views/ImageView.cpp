/**
* @file ControllerQt/Views/ImageView.cpp
*
* Implementation of class ImageView
*
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
* @author Colin Graf
*/

#include <QGLWidget>
#include <QPainter>
#include <QApplication>
#include <QMouseEvent>

#include <Representations/Infrastructure/Image.h>
#include <Platform/Thread.h>
#include <GL/gl.h>

#include "ImageView.h"
#include "../RobotConsole.h"
#include "../Visualization/PaintMethods.h"

class ImageWidget : public QGLWidget
{
public:
  ImageWidget(RobotConsole& console, const std::string& background, const std::string& name, bool segmented) : QGLWidget(QGLFormat(/*QGL::SampleBuffers*/)),
    console(console),
    background(background),
    name(name),
    segmented(segmented), 
    imageData(0), imageWidth(0), imageHeight(0),
    lastImageTimeStamp(0), lastColorTableTimeStamp(0), lastDrawingsTimeStamp(0),
    widgetWidth(0), widgetHeight(0),
    dragStart(-1, -1), zoom(1.f) 
  {
    setMouseTracking(true);
  }

  ~ImageWidget()
  {
    if(imageData)
      delete[] imageData;
  }
  
  void update()
  {
    if(needsRepaint())
      QGLWidget::update();
  }

private:
  RobotConsole& console;
  const std::string& background;
  const std::string& name;
  bool segmented;
  char* imageData;
  int imageWidth;
  int imageHeight;
  unsigned int lastImageTimeStamp;
  unsigned int lastColorTableTimeStamp;
  unsigned int lastDrawingsTimeStamp;
  int widgetWidth;
  int widgetHeight;
  QPainter painter;
  QPoint dragStart;
  QPoint dragPos;
  float zoom;

  void resizeGL(int width, int height)
  {
    widgetWidth = width;
    widgetHeight = height;
  }

  void paintEvent(QPaintEvent *event)
  {
    painter.begin(this);
    
    {
      int width, height;

      SYNC_WITH(console);

      const Image* image = 0;
      RobotConsole::Images::const_iterator i = console.images.find(background);
      if(i != console.images.end())
        image = i->second.image;
      if(image)
      {
        width = image->cameraInfo.resolutionWidth;
        height = image->cameraInfo.resolutionHeight;
      }
      else
      {
        width = cameraResolutionWidth;
        height = cameraResolutionHeight;
      }

      float scale, xOffset, yOffset;
      {
        float xScale = float(widgetWidth) / float(width);
        float yScale = float(widgetHeight) / float(height);
        scale = xScale < yScale ? xScale : yScale;
        scale *= zoom;
        xOffset = (float(widgetWidth) - float(width) * scale) * 0.5f;
        yOffset = (float(widgetHeight) - float(height) * scale) * 0.5f;
      }

      if(image)
        paintImage(*image, xOffset, yOffset, scale);
      else
        lastImageTimeStamp = 0;
      
      painter.setTransform(QTransform(scale, 0, 0, scale, xOffset, yOffset));
      paintDrawings();
    }

    if(dragStart.x() >= 0)
    {
      QPen pen(QColor(128, 128, 128, 128));
      pen.setWidth(1);
      painter.setPen(pen);
      painter.setBrush(QBrush(Qt::NoBrush));
      painter.drawRect(QRect(dragStart, dragPos));
    }
    
    painter.end();
  }

  void paintDrawings()
  {
    const QTransform baseTrans(painter.transform());
    const std::list<std::string>& drawings(console.imageViews[name]);
    for(std::list<std::string>::const_iterator i = drawings.begin(), end = drawings.end(); i != end; ++i)
    {
      const DebugDrawing& debugDrawing(console.imageDrawings[*i]);
      PaintMethods::paintDebugDrawing(painter, debugDrawing, baseTrans);
      if(debugDrawing.timeStamp > lastDrawingsTimeStamp)
        lastDrawingsTimeStamp = debugDrawing.timeStamp;
    }
    painter.setTransform(baseTrans);
  }

  void copyImage(const Image& srcImage)
  {
    int width = srcImage.cameraInfo.resolutionWidth;
    int height = srcImage.cameraInfo.resolutionHeight;

    static const int factor1 = 29016;
    static const int factor2 = 5662;
    static const int factor3 = 22972;
    static const int factor4 = 11706;
    char* p = imageData;
    int r, g, b;
    int yImage, uImage, vImage;
    for(int y = height - 1; y >= 0; y--) 
    {
      const Image::Pixel* cur = &srcImage.image[y][0];
      const Image::Pixel* end = cur + width;
      for(; cur < end; cur++)
      {        
        yImage = int(cur->y) << 14;
        uImage = int(cur->cr) - 128;
        vImage = int(cur->cb) - 128;

        r = (yImage + factor3 * uImage) >> 14;
        g = (yImage - factor2 * vImage - factor4 * uImage) >> 14;
        b = (yImage + factor1 * vImage) >> 14;

        *p++ = r < 0 ? 0 : (r > 255 ? 255 : r);
        *p++ = g < 0 ? 0 : (g > 255 ? 255 : g);
        *p++ = b < 0 ? 0 : (b > 255 ? 255 : b);
      }
    }
  }

  void copyImageSegmented(const Image& srcImage)
  {
    int width = srcImage.cameraInfo.resolutionWidth;
    int height = srcImage.cameraInfo.resolutionHeight;
    static const int colorClassColors[] = {
      0x7f7f7f, // none
      0xff7f00, // orange
      0xffff00, // yellow
      0x0000ff, // blue
      0xffffff, // white
      0x00ff00, // green
      0x000000, // black
      0xff0000, // red
      0x00ffff, // robotBlue
    };
    char* p = imageData;
    unsigned char (*colorClasses)[64][64](console.colorTableHandler.colorTable.colorClasses);
    for(int y = height - 1; y >= 0; y--) 
    {
      const Image::Pixel* cur = &srcImage.image[y][0];
      const Image::Pixel* end = cur + width;
      for(; cur < end; cur++)
      {
        unsigned char clrclass = colorClasses[cur->y >> 2][cur->cb >> 2][cur->cr >> 2];
        if(clrclass > 8)
          clrclass = 0;
        int color = colorClassColors[clrclass];
        *(p++) = color >> 16;
        *(p++) = color >> 8;
        *(p++) = color;
      }
    }

  }

  void paintImage(const Image& srcImage, float x, float y, float scale)
  {
    int width = srcImage.cameraInfo.resolutionWidth;
    int height = srcImage.cameraInfo.resolutionHeight;

    // make sure we have a buffer
    if(imageWidth != width || imageHeight != height)
    {
      if(imageData)
        delete[] imageData;
      imageWidth = width;
      imageHeight = height;
      imageData = new char[imageWidth * imageHeight * 3];
    }

    if(srcImage.timeStamp != lastImageTimeStamp || (segmented && console.colorTableHandler.timeStamp != lastColorTableTimeStamp))
    {
      if(segmented)
        copyImageSegmented(srcImage);
      else
        copyImage(srcImage);
      
      lastImageTimeStamp = srcImage.timeStamp;
      if(segmented)
        lastColorTableTimeStamp = console.colorTableHandler.timeStamp;
    }
    
    //glPushAttrib(GL_ALL_ATTRIB_BITS);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    //glMatrixMode(GL_MODELVIEW);
    //glPushMatrix();

    //glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, (GLdouble) widgetWidth, 0.0, (GLdouble) widgetHeight);
    //glMatrixMode(GL_MODELVIEW);
    //glLoadIdentity();

    glRasterPos2f(x, y);
    glPixelZoom(scale, scale);
    glDrawPixels(imageWidth, imageHeight, GL_RGB, GL_UNSIGNED_BYTE, imageData);

    //glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    //glMatrixMode(GL_MODELVIEW);
    //glPopMatrix();
    //glPopAttrib();
  }

  bool needsRepaint() const
  {
    SYNC_WITH(console);
    const std::list<std::string>& drawings(console.imageViews[name]);
    for(std::list<std::string>::const_iterator i = drawings.begin(), end = drawings.end(); i != end; ++i)
    {
      const DebugDrawing& debugDrawing(console.imageDrawings[*i]);
      if(debugDrawing.timeStamp > lastDrawingsTimeStamp)
        return true;
    }
    Image* image = 0;
    RobotConsole::Images::const_iterator j = console.images.find(background);
    if(j != console.images.end())
      image = j->second.image;
    return (image && 
           (image->timeStamp != lastImageTimeStamp ||
            (segmented && console.colorTableHandler.timeStamp != lastColorTableTimeStamp))) ||
           (!image && lastImageTimeStamp);
  }

  void window2viewport(QPoint& point)
  {
    float scale, xOffset, yOffset;
    {
      float xScale = float(widgetWidth) / float(imageWidth);
      float yScale = float(widgetHeight) / float(imageHeight);
      scale = xScale < yScale ? xScale : yScale;
      scale *= zoom;
      xOffset = (float(widgetWidth) - float(imageWidth) * scale) * 0.5f;
      yOffset = (float(widgetHeight) - float(imageHeight) * scale) * 0.5f;
    }
    point = QPoint((point.x() - xOffset) / scale, (point.y() - yOffset) / scale);
  }

  void mousePressEvent(QMouseEvent* event)
  {
    QGLWidget::mousePressEvent(event);
    
    if(event->button() == Qt::LeftButton || event->button() == Qt::MidButton)
    {
      bool active;
      {
        SYNC_WITH(console);
        active = console.colorTableHandler.active;
      }
      if(!active)
      {
        //QApplication::beep();
        return;
      }
      dragStart = event->pos();
      window2viewport(dragStart);
      dragPos = dragStart;
      QGLWidget::update();
    }
  }

  void mouseMoveEvent(QMouseEvent* event)
  {
    QGLWidget::mouseMoveEvent(event);
    
    if(dragStart.x() >= 0)
    {
      dragPos = event->pos();
      window2viewport(dragPos);
      QGLWidget::update();
      return;
    }

    {
      SYNC_WITH(console);
      if(!console.colorTableHandler.active)
      {
        QPoint pos(event->pos());
        window2viewport(pos);
        const char* text = 0;
        const std::list<std::string>& drawings(console.imageViews[name]);
        for(std::list<std::string>::const_iterator i = drawings.begin(), end = drawings.end(); i != end; ++i)
        {
          text = console.imageDrawings[*i].getTip(pos.rx(), pos.ry());
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


  void mouseReleaseEvent(QMouseEvent* event)
  {
    QGLWidget::mouseReleaseEvent(event);
    
    if(dragStart.x() < 0)
      return;

    Qt::KeyboardModifiers keys = QApplication::keyboardModifiers();
    {
      SYNC_WITH(console);

      Image* image = 0;
      RobotConsole::Images::const_iterator i = console.images.find("raw image");
      if(i != console.images.end())
        image = i->second.image;

      if(image)
      {
        if(keys & Qt::ControlModifier && !(keys & Qt::ShiftModifier))
          console.colorTableHandler.undo();
        else 
        {
          if(dragStart == dragPos) // just a click
          {
            if((keys & Qt::ShiftModifier) && (keys & Qt::ControlModifier))
              console.colorTableHandler.clearPixel(*image, Vector2<int>(dragStart.x(), dragStart.y()));
            else if(keys & Qt::ShiftModifier)
              console.colorTableHandler.selectedColorClass = 
                console.colorTableHandler.colorTable.getColorClass(image->image[dragStart.y()][dragStart.x()].y,
                                                                   image->image[dragStart.y()][dragStart.x()].cb,
                                                                   image->image[dragStart.y()][dragStart.x()].cr);
            else
              console.colorTableHandler.addPixel(*image, Vector2<int>(dragStart.x(), dragStart.y()));
          }
          else if((keys & Qt::ShiftModifier) && (keys & Qt::ControlModifier))
            console.colorTableHandler.clearRectangle(*image, Vector2<int>(std::min<>(dragStart.x(), dragPos.x()), std::min<>(dragStart.y(), dragPos.y())),
                                                             Vector2<int>(std::max<>(dragStart.x(), dragPos.x()), std::max<>(dragStart.y(), dragPos.y())));
          else if(keys & Qt::ShiftModifier)
            console.colorTableHandler.fillRectangle(*image, Vector2<int>(std::min<>(dragStart.x(), dragPos.x()), std::min<>(dragStart.y(), dragPos.y())),
                                                          Vector2<int>(std::max<>(dragStart.x(), dragPos.x()), std::max<>(dragStart.y(), dragPos.y())));
          else if(!(keys & Qt::ShiftModifier))
            console.colorTableHandler.addRectangle(*image, Vector2<int>(std::min<>(dragStart.x(), dragPos.x()), std::min<>(dragStart.y(), dragPos.y())),
                                                           Vector2<int>(std::max<>(dragStart.x(), dragPos.x()), std::max<>(dragStart.y(), dragPos.y())));          
        }
      }
    }

    dragStart = QPoint(-1, -1);
    QGLWidget::update();
  }

  void keyPressEvent(QKeyEvent* event)
  {
    switch(event->key())
    {
    case Qt::Key_PageUp:
    case Qt::Key_Plus:
      event->accept();
      if(zoom < 1.f)
        zoom += 0.1f;
      if(zoom > 1.f)
        zoom = 1.f;
      QGLWidget::update();
      break;
    case Qt::Key_PageDown:
    case Qt::Key_Minus:
      event->accept();
      if(zoom > 0.1f)
        zoom -= 0.1f;
      QGLWidget::update();
      break;
    default:
      QGLWidget::keyPressEvent(event);
      break;
    }
  }

  QSize sizeHint () const { return QSize(cameraResolutionWidth, cameraResolutionHeight); }

  friend class ImageView;
};

ImageView::ImageView(RobotConsole& console, const std::string& background, const std::string& name, bool segmented) :
  console(console),
  background(background), name(name),
  segmented(segmented) {}

QWidget* ImageView::createWidget()
{
  return new ImageWidget(console, background, name, segmented);
}

void ImageView::updateWidget(QWidget& widget)
{
  ((ImageWidget*)(&widget))->update();
}

std::string ImageView::getConfiguration(QWidget& widget) const
{
  ImageWidget* imageWidget = (ImageWidget*)&widget;
  QByteArray block;
  QDataStream out(&block, QIODevice::WriteOnly);
  out.setVersion(QDataStream::Qt_4_3);
  out << imageWidget->zoom;
  return std::string(block.toBase64().constData());
}

void ImageView::setConfiguration(QWidget& widget, const std::string& conf)
{
  ImageWidget* imageWidget = (ImageWidget*)&widget;
  const QByteArray& block(QByteArray::fromBase64(conf.c_str()));
  QDataStream in(block);
  in.setVersion(QDataStream::Qt_4_3);
  float zoom;
  in >> zoom;
  if(zoom)
    imageWidget->zoom = zoom;
}
