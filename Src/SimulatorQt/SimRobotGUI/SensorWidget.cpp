
#include <GL/glew.h>
#include <QLabel>
#include <QPainter>
#include <QApplication>
#include <QClipboard>
#include <QLocale>
#include <QGLWidget>
#include <QSettings>
#include <QMimeData>
#include <sstream>
#include <iomanip>

#include <Controller/DirectView.h>
#include <Simulation/Simulation.h>

#include "SensorWidget.h"
#include "Document.h"
#include "MenuBar.h"

class SRSensorViewWidget : public QWidget
{
public:
  QPainter painter;
  QString sensorName;
  Simulation *sim;
  int sensorPortId;
  QPen pen;
  QFont font;

  SRSensorViewWidget(const QString &sensorName, QWidget* parent=0) : QWidget( parent) 
  {
    setFocusPolicy(Qt::StrongFocus);
    this->sensorName = sensorName;
    sim = Document::document->getSimulation();
    sensorPortId = sim->getSensorPortId(std::vector<std::string>(1,sensorName.toAscii().constData()));
    pen = QPen(QColor::fromRgb(255, 0, 0));
    font = QFont ("SansSerif", 10, QFont::Bold);
  }

  void paintEvent(QPaintEvent *event)
  {
    const SensorReading& sensorReading = sim->getSensorReading(sensorPortId);
    const double& minValue = sensorReading.minValue;
    const double& maxValue = sensorReading.maxValue;
    const std::vector<int>& dimensions = sensorReading.dimensions;
    painter.begin(this);
    painter.setFont(font);
    switch(sensorReading.sensorType)
    {
    case SensorReading::doubleSensor:
      {
        double sensorValue = sensorReading.data.doubleValue;
        char str_val[32];
        sprintf(str_val, "%.03f", sensorValue);
        sensorValue -= minValue;
        sensorValue /= (maxValue-minValue);
        QBrush brush(QColor::fromRgb(sensorValue*255, sensorValue*255, sensorValue*255));
        painter.setBrush(brush);
        painter.drawRect(0,0, this->width(), this->height());
        painter.setPen(pen);
        painter.drawText(this->width()/2, this->height()/2, str_val);
        break;
      }
    case SensorReading::doubleArraySensor:
      {
        // Accelerometer, Gyroscope, PositionSensor, ...
        if(sensorReading.descriptions.size() && dimensions.size() == 1)
          paintDoubleArrayWithDescriptionsSensor(sensorReading);
        // Laser Range Finder
        else if(sensorReading.descriptions.size() == 0 && dimensions.size() == 1 &&
          sensorReading.minMaxRangeValid)
          paintDoubleArrayWithLimitsAndWithoutDescriptions(sensorReading);
        // Other stuff (e.g. depth image)
        else
          painter.drawText(0, 0, this->width(), this->height(), Qt::AlignCenter, "Not implemented yet!");
        break;
      }
    case SensorReading::boolSensor:
      {
        paintBoolSensor(sensorReading);
        break;
      }
    case SensorReading::cameraSensor:
      {
        int xsize = dimensions[0], ysize = dimensions[1];         
        const unsigned char *vals = sensorReading.data.byteArray;
        const unsigned char* end = vals + xsize * ysize * 3;
        unsigned char* buffer = new unsigned char[xsize * ysize * 4];
        unsigned char* pDest = buffer;
        for(const unsigned char* pSrc = vals; pSrc < end; pSrc += 3)
        {
          *pDest++ = pSrc[0];
          *pDest++ = pSrc[1];
          *pDest++ = pSrc[2];
          *pDest++ = 0xff;
        }
        QImage img(buffer, xsize, ysize, QImage::Format_RGB32); 
        painter.drawImage(0,0, img.scaled(this->width(), this->height()));
        delete [] buffer;
        break;
      }
    default:
      break;
    }
    painter.end();
  }

  void paintBoolSensor(const SensorReading& sensorReading)
  {
    const bool value = sensorReading.data.boolValue;
    QBrush brush(value ? QColor::fromRgb(255,255,255) : QColor::fromRgb(0,0,0));
    pen.setColor(value ? QColor::fromRgb(0,0,0) : QColor::fromRgb(255,255,255));
    painter.setPen(pen);
    painter.fillRect(0, 0, this->width(), this->height(), brush);
    painter.drawText(0, 0, this->width(), this->height(), Qt::AlignCenter, (value ? "true" : "false"));
  }

  void paintDoubleArrayWithDescriptionsSensor(const SensorReading& sensorReading)
  {
    QBrush brush(QColor::fromRgb(255, 255, 255));
    painter.setBrush(brush);
    painter.fillRect(0, 0, this->width(), this->height(), brush);
    pen.setColor(QColor::fromRgb(0, 0, 0));
    painter.setPen(pen);
    bool conversionToDegreesNeeded = sensorReading.unitForDisplay.find("°") != std::string::npos;
    for(int i = 0; i < sensorReading.dimensions[0]; i++)
    {   
      double value = conversionToDegreesNeeded ? 
        Functions::toDeg(sensorReading.data.doubleArray[i]) : sensorReading.data.doubleArray[i];
      std::stringstream valueStream;
      valueStream << std::fixed << std::setprecision(4) << value;
      painter.drawText(0, 20*i, this->width(), 20, Qt::AlignLeft, 
        (" "+sensorReading.descriptions[i]).c_str());
      painter.drawText(0, 20*i, this->width(), 20, Qt::AlignRight, 
        (valueStream.str() + " " + sensorReading.unitForDisplay + " ").c_str());
    }
  }

  void paintDoubleArrayWithLimitsAndWithoutDescriptions(const SensorReading& sensorReading)
  {
    QBrush brush(QColor::fromRgb(255,255,255));
    QBrush brush2(QColor::fromRgb(0,255,0));
    painter.fillRect(0, 0, this->width(), this->height(), brush);
    pen.setColor(QColor::fromRgb(0,0,0));
    painter.setPen(pen);
    const double* valueArray = sensorReading.data.doubleArray;
    double widthPerValue = this->width()/(double)sensorReading.dimensions[0];
    if(widthPerValue < 1.0)
      widthPerValue = 1.0;
    for(int i = 0; i < sensorReading.dimensions[0]; i++)
    {
      double factor = valueArray[i] / sensorReading.maxValue;
      painter.fillRect(i*widthPerValue, this->height()*(1-factor), widthPerValue, this->height()*factor, brush2);
      painter.drawRect(i*widthPerValue, this->height()*(1-factor), widthPerValue, this->height()*factor);
    }
  }

  QSize sizeHint () const
  {
    SRSensorWidget* sensorWidget = (SRSensorWidget*)parent();
    if(sensorWidget->sensorType != SensorReading::cameraSensor)
      return QSize(200,200); // some dummy default
    return QSize(sensorWidget->sensorDimensions[0], sensorWidget->sensorDimensions[1]);
  }
};

SRSensorWidget::SRSensorWidget(const SceneObject& object, QWidget* parent) : 
  DockWidget(object, parent), directView(0), widget(0), sensorType(SensorReading::noSensor)
{
  connect(Document::document, SIGNAL(updatedSceneGraph()), this, SLOT(updateObject()));
  connect(Document::document, SIGNAL(restoreLayoutSignal()), this, SLOT(restoreLayout()));
  connect(Document::document, SIGNAL(writeLayoutSignal()), this, SLOT(writeLayout()));
  connect(Document::document, SIGNAL(updateViewsSignal()),this, SLOT(updateView()));
  updateObject();
}

void SRSensorWidget::updateObject()
{
  QWidget* newWidget = widget;
  DirectView* newDirectView = 0;
  Simulation* simulation = Document::document->getSimulation();
  int sensorPortId = -1;
  if(simulation)
  {
    sensorPortId = simulation->getSensorPortId(std::vector<std::string>(1,object.name.toAscii().constData()));    
    if(sensorPortId >= 0)
    {
      const SensorReading& sensorReading = simulation->getSensorReading(sensorPortId);
      sensorDimensions = sensorReading.dimensions;
      sensorType = sensorReading.sensorType;
      if(sensorType == SensorReading::viewSensor)
        newDirectView = (DirectView*)simulation->getView(sensorPortId);
      else if(sensorType != SensorReading::noSensor)
        newWidget = new SRSensorViewWidget(object.name, this);
      else
        newWidget = new QLabel("noSensor");
    }    
  }

  if(newDirectView != directView)
  {
    directView = newDirectView;
    newWidget = directView ? directView->createWidget() : 0;
    if(directView)
      directView->setConfiguration(*newWidget, conf);
    if(newWidget)
      newWidget->setFocusPolicy(Qt::StrongFocus);
  }

  if(!newWidget && sensorPortId >= 0)
    newWidget = new QLabel(tr("TODO: implement me"), this);

  if(newWidget != widget)
  {
    QWidget* oldWidget = widget;
    widget = newWidget;
    setWidget(newWidget);
    setFocusProxy(newWidget);
    if(oldWidget)
      delete oldWidget;
  }
}

void SRSensorWidget::updateView()
{
  if(!isReallyVisible())
    return;
  if(directView)
    directView->updateWidget(*widget);
  else
  {
    Simulation* simulation = Document::document->getSimulation();
    int sensorPortId = simulation->getSensorPortId(std::vector<std::string>(1,object.name.toAscii().constData()));
    if(sensorPortId != -1)
    {
      const SensorReading& sensorReading = simulation->getSensorReading(sensorPortId);
      if(sensorReading.sensorType != SensorReading::noSensor) 
        widget->update();
    }
  }
}

void SRSensorWidget::restoreLayout()
{
  QSettings* settings = Document::document->getLayoutSettings();

  settings->beginGroup(object.name);

  // load configuration for direct views
  const QVariant& var(settings->value("Configuration"));
  if(var.isValid())
  {
    conf = var.toByteArray().constData();
    if(directView)
      directView->setConfiguration(*widget, conf);
  }
  
  settings->endGroup();
}

void SRSensorWidget::writeLayout()
{
  QSettings* settings = Document::document->getLayoutSettings();
  settings->beginGroup(object.name);

  if(directView)
    settings->setValue("Configuration", QByteArray(directView->getConfiguration(*widget).c_str()));

  settings->endGroup();
}

void SRSensorWidget::copy()
{
  QApplication::clipboard()->clear();
  Simulation* sim = Document::document->getSimulation();
  int sensorPortId = sim->getSensorPortId(std::vector<std::string>(1,object.name.toAscii().constData()));
  const SensorReading& sensorReading = sim->getSensorReading(sensorPortId);
  QMimeData* mineData = new QMimeData;
  setClipboardGraphics(*mineData);
  int count = 1;
  for(int i = 0; i < int(sensorReading.dimensions.size()); i++)
    count *= sensorReading.dimensions[i];
  if(count < 10000 && !directView)
    setClipboardText(*mineData);
  QApplication::clipboard()->setMimeData(mineData);  
}

void SRSensorWidget::setClipboardGraphics(QMimeData& mimeData)
{
  if(widget)
  {
    // just take a screenshot of opengl, normal or mixed widgets
    if(widget->inherits("QGLWidget"))
    {
      mimeData.setImageData(QVariant(qobject_cast<QGLWidget*>(widget)->grabFrameBuffer()));
      return;
    }
    QPixmap pixmap(QPixmap::grabWidget(widget));    
    QWidget* subWidget = widget->focusProxy();
    if(subWidget && subWidget->inherits("QGLWidget"))
    {
      QGLWidget* glWidget = qobject_cast<QGLWidget*>(subWidget);
      QPainter painter(&pixmap);
      painter.drawImage(glWidget->mapTo(widget, QPoint(0, 0)), glWidget->grabFrameBuffer());
    }
    mimeData.setImageData(QVariant(pixmap.toImage()));
  }
}

void SRSensorWidget::setClipboardText(QMimeData& mimeData)
{
  double doubleValue;
  double* pDouble = 0;
  bool deletePDouble = false;
  Simulation* pSim = Document::document->getSimulation();
  int sensorportNumber = pSim->getSensorPortId(std::vector<std::string>(1,object.name.toAscii().constData()));
  const SensorReading& sensorReading = pSim->getSensorReading(sensorportNumber);
  int dimSize[3] = {1, 1, 1};
  {
    for(int i = 0; i < 3 && i < int(sensorReading.dimensions.size()); i++)
      if(sensorReading.dimensions[i] > 0)
        dimSize[i] = sensorReading.dimensions[i];
  }
  switch(sensorReading.sensorType)
  {
  case SensorReading::boolSensor:
    {
      doubleValue = sensorReading.data.boolValue ? 1 : 0;
      pDouble = &doubleValue;
      break;
    }
  case SensorReading::doubleSensor:
    doubleValue = sensorReading.data.doubleValue;
    pDouble = &doubleValue;
    break;
  case SensorReading::cameraSensor:
    {
      pDouble = new double[dimSize[0] * dimSize[1] * dimSize[2]];
      deletePDouble = true;
      const unsigned char *vals = sensorReading.data.byteArray;
      for(int i = dimSize[0] * dimSize[1] * dimSize[2] - 1; i >= 0; --i)
        pDouble[i] = vals[i];
      break;
    }
  case SensorReading::doubleArraySensor:
    {
      pDouble = (double*)sensorReading.data.doubleArray;
      break;
    }
  case SensorReading::viewSensor:
  case SensorReading::noSensor:
    {
      break;
    }
  }
  
  if(!pDouble)
    return;

  QString text;
  const QLocale& locale(QLocale::system());
  double* pData = pDouble;
  char sBuffer[30];
  for(int y = 0; y < dimSize[1] * dimSize[2]; y++)
    for(int x = 0; x < dimSize[0]; x++)
    {      
      double d = *pData;
      sprintf(sBuffer, "%lg", d);
      for(char* str = sBuffer; *str; ++str)
        if(*str == '.')
          text += locale.decimalPoint();
        else
          text += *str;
      text += x + 1 == dimSize[0] ? '\n' : '\t';
      pData++;
    }
  mimeData.setText(text);

  if(deletePDouble)
    delete[] pDouble;
}

QMenu* SRSensorWidget::createPopupMenu()
{
  QMenu* menu = new QMenu(tr("&Sensor"));
  QAction* action;
  action = menu->addAction(QIcon(":/icons/page_copy.png"), tr("&Copy"));
  action->setShortcut(QKeySequence(QKeySequence::Copy));
  action->setStatusTip(tr("Copy the current selection's contents or view to the clipboard"));
  connect(action, SIGNAL(triggered()), this, SLOT(copy()));
  return menu;
}
