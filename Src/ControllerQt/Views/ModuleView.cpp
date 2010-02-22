/**
* @file ControllerQt/Views/ModuleView.cpp
*
* Implementation of a class to represent a view displaying the module layout of the process.
*
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
* @author Colin Graf
*/

#include <QPixmap>
#include <QFile>
#include <QDir>
#include <QProcess>
#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include <QTextStream>

#include <Platform/Thread.h>
#ifdef _WIN32
#include <Platform/File.h>
#endif

#include <algorithm>

#include "ModuleView.h"
#include "../RobotConsole.h"

class ModuleWidget : public QWidget
{
public:
  QPixmap* image;
  float scale;
  QPoint offset;

  ModuleWidget(QWidget* parent = 0, Qt::WindowFlags f = 0) : QWidget(parent, f),
    image(0), scale(1.0f), dragStart(-1, -1) 
  {
    setBackgroundRole(QPalette::Base);
  }
  
  bool changeScale(float change)
  {
    if((scale <= 0.1f && change < 0) || (scale >= 2.0f && change > 0))
      return false;
    scale += change;
    if(scale < 0.1f)
      scale = 0.1f;
    if(scale > 2.0f)
      scale = 2.0f;
    return true;
  }
  
private:
  QPainter painter;
  QPoint dragStart;
  QPoint dragStartOffset;

  void paintEvent(QPaintEvent* event)
  {
    if(!image)
      return;
    
    painter.begin(this);
    const QRect& windowRect(painter.window());
    QSizeF maxSize(windowRect.size());
    maxSize *= scale;
    QSizeF size(image->size());
    size -= QSizeF(offset.x(), offset.y());
    if(size.width() > maxSize.width())
      size.setWidth(maxSize.width());
    if(size.height() > maxSize.height())
      size.setHeight(maxSize.height());
    painter.drawPixmap(QRectF(QPointF(0, 0), size / scale), *image, QRectF(offset, size));
    painter.end();
  }
  
  void mousePressEvent(QMouseEvent* event)
  {
    QWidget::mousePressEvent(event);
    
    if((event->button() == Qt::LeftButton || event->button() == Qt::MidButton) && image)
    {
      dragStart = event->pos();
      dragStartOffset = offset;
    }
  }
  
  void mouseReleaseEvent(QMouseEvent* event)
  {
    QWidget::mouseReleaseEvent(event);
    
    dragStart = QPoint(-1, -1);
  }
  
  void mouseMoveEvent(QMouseEvent* event)
  {
    QWidget::mouseMoveEvent(event);
    
    if(dragStart.x() >= 0 && image)
    {
      const QPoint& dragPos(event->pos());
      offset = dragStartOffset + (dragStart - dragPos);
      if(offset.x() < 0)
        offset.setX(0);
      if(offset.y() < 0)
        offset.setY(0);

      if(image->width() - offset.x() < width() * scale)
        offset.setX((image->width() - width() * scale));
      if(image->height() - offset.y() < height() * scale)
        offset.setY((image->height() - height() * scale));
      update();
    }
  }
  
  void keyPressEvent(QKeyEvent* event)
  {
    if(!image)
    {
      QWidget::keyPressEvent(event);
      return;
    }
    
    switch(event->key())
    {
    case Qt::Key_PageUp:
    case Qt::Key_Plus:
      event->accept();
      changeScale(0.1f);
      update();
      break;
    case Qt::Key_PageDown:
    case Qt::Key_Minus:
      event->accept();
      changeScale(-0.1f);
      update();
      break;
    default:
      QWidget::keyPressEvent(event);
      break;
    }
  }
  
  void wheelEvent(QWheelEvent* event)
  {
    QWidget::wheelEvent(event);
    
    if(!image)
      return;
    
    changeScale(0.1 * event->delta() / 160);
    update();
  }
  
  void mouseDoubleClickEvent(QMouseEvent* event)
  {
    QWidget::mouseDoubleClickEvent(event);
    
    if(!image)
      return;
    
    scale = 1.0f;
    update();
  }
};

ModuleView::ModuleView(RobotConsole& console, char processIdentifier, const std::string& category) :
  console(console), processIdentifier(processIdentifier), category(category), 
  image(0), lastModulInfoTimeStamp(0) {}

ModuleView::~ModuleView()
{
  if(image)
    delete image;
}

QWidget* ModuleView::createWidget()
{
  ModuleWidget* widget = new ModuleWidget();
  widget->image = image;
  return widget;
}

void ModuleView::updateWidget(QWidget& widget)
{
  {
    SYNC_WITH(console);
    if(lastModulInfoTimeStamp == console.moduleInfo.timeStamp)
      return;
  }
  generateImage();
  ((ModuleWidget*)(&widget))->image = image;
  widget.update();
}

bool ModuleView::generateDotFile(const char* fileName)
{
  SYNC_WITH(console);
  const ModuleInfo& m = console.moduleInfo;
  bool success = false;
  std::ofstream stream(fileName);
  stream << "digraph G {" << std::endl;
  stream << "node [style=filled,fillcolor=lightyellow,fontname=Arial,fontsize=9,height=0.2];" << std::endl;
  stream << "concentrate = true;" << std::endl;

  for(std::list<ModuleInfo::Module>::const_iterator i = m.modules.begin(); i != m.modules.end(); ++i)
    if(i->processIdentifier == processIdentifier && (category == "" || i->category == category))
    {
      bool used = false;
      for(std::vector<std::string>::const_iterator j = i->representations.begin(); j != i->representations.end(); ++j)
      {
        std::list<ModuleInfo::Provider>::const_iterator k = std::find(m.providers.begin(), m.providers.end(), *j);
        while(k != m.providers.end() && k->selected != i->name)
          k = std::find(++k, m.providers.end(), *j);
        if(k != m.providers.end())
        {
          used = true;
          stream << j->c_str() << " [style=filled,fillcolor=\"lightblue\"];" << std::endl;
          stream << i->name << "->" << j->c_str() << " [len=2];" << std::endl;
        }
      }
      if(used)
      {
        for(std::vector<std::string>::const_iterator j = i->requirements.begin(); j != i->requirements.end(); ++j)
        {
          std::list<ModuleInfo::Provider>::const_iterator k = std::find(m.providers.begin(), m.providers.end(), *j);
          while(k != m.providers.end() && (k->selected == "" || k->processIdentifier != processIdentifier))
            k = std::find(++k, m.providers.end(), *j);
          if(k == m.providers.end())
          { // no provider in same process
            k = std::find(m.providers.begin(), m.providers.end(), *j);
            while(k != m.providers.end() && k->selected == "")
              k = std::find(++k, m.providers.end(), *j);
            if(k == m.providers.end())
              stream << j->c_str() << " [style=\"filled,dashed\",color=red,fontcolor=red];" << std::endl;
            else
              stream << j->c_str() << " [style=\"filled,dashed\",fillcolor=\"#ffdec4\"];" << std::endl;
          }
          stream << j->c_str() << "->" << i->name << " [len=2];" << std::endl;
        }
        if(i->name == "default")
          stream << i->name.c_str() << "[shape=box,fontcolor=red];" << std::endl;
        else
          stream << i->name.c_str() << "[shape=box];" << std::endl;
        success = true;
      }
    }
  
  stream << "}" << std::endl;
  return success;
}

std::string ModuleView::compress(const std::string& s) const
{
  std::string s2(s);
  if(!isalpha(s2[0]))
    s2[0] = '_';
  for(unsigned i = 1; i < s2.size(); ++i)
    if(!isalpha(s2[i]) && !isdigit(s2[i]))
      s2[i] = '_';
  return s2;
}

void ModuleView::generateImage()
{
  if(image)
  {
    delete image;
    image = 0;
  }
  
  const QString& dotName(QDir::temp().filePath("ModuleView.dot"));
  if(generateDotFile(dotName.toAscii().constData()))
  {
    const QString& pngName(QDir::temp().filePath("ModuleView.png"));
#ifdef _WIN32
    const QString& cmd(QString("\"%1\\Util\\dot-1.9.0\\dot.exe\" -Tpng -o \"%2\" \"%3\"").arg(File::getGTDir(), pngName, dotName));
#else
    const QString& cmd(QString("dot -Tpng -o \"%1\" \"%2\"").arg(pngName, dotName));
#endif
    if(QProcess::execute(cmd) == 0)
    {
      image = new QPixmap(pngName);
      if(image->isNull())
      {
        delete image;
        image = 0;
      }
      QFile::remove(pngName);
    }
    SYNC_WITH(console);
    lastModulInfoTimeStamp = console.moduleInfo.timeStamp;
  }

  QFile::remove(dotName);
}


std::string ModuleView::getConfiguration(QWidget& widget) const
{
  ModuleWidget* moduleWidget = (ModuleWidget*)&widget;
  QByteArray block;
  QDataStream out(&block, QIODevice::WriteOnly);
  out.setVersion(QDataStream::Qt_4_3);
  out << moduleWidget->offset;
  out << moduleWidget->scale;
  return std::string(block.toBase64().constData());
}

void ModuleView::setConfiguration(QWidget& widget, const std::string& conf)
{
  ModuleWidget* moduleWidget = (ModuleWidget*)&widget;
  const QByteArray& block(QByteArray::fromBase64(conf.c_str()));
  QDataStream in(block);
  in.setVersion(QDataStream::Qt_4_3);
  in >> moduleWidget->offset;
  in >> moduleWidget->scale;
  if(!moduleWidget->scale)
    moduleWidget->scale = 1.0f;
}

