/**
* @file ControllerQt/Views/SensorView.cpp
*
* Implementation of class SensorView
*
* @author of the original sensorview <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
* @author Jeff
* @author Colin Graf
*/

#include <QHeaderView>
#include <QApplication>
#include <QPainter>
#include <QFontMetrics>

#include "SensorView.h"
#include "../RobotConsole.h"
#include "../Visualization/HeaderedWidget.h"


class SensorWidget : public QWidget
{
public:
  SensorWidget(SensorView& sensorView, RobotConsole &c, QHeaderView* headerView, QWidget* parent) : 
    console(c), sensorView(sensorView), headerView(headerView), noPen(Qt::NoPen)
  {
    setBackgroundRole(QPalette::Base);
    const QFontMetrics& fontMetrics(QApplication::fontMetrics());
    lineSpacing = fontMetrics.lineSpacing() + 2;
    textOffset = fontMetrics.descent() + 1;

    font = QApplication::font();
    
    const QPalette& pal(QApplication::palette());
    altBrush = pal.alternateBase();
    fontPen.setColor(pal.text().color());
    
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    for(int j = 0; j < 2; ++j)
      for(int i = 0; i < SensorData::numOfUsSensorTypes; ++i)
        usDistances[j][i] = SensorData::off;
  }

  enum Type
  {
    distance,
    pressure,
    acceleration,
    batteryLevel,
    angularSpeed,
    ratio,
    unknown,
    angle2,
    usLDistance,
    usLtRDistance,
    usRtLDistance,
    usRDistance,
    end
  };
  struct ViewInfo
  {
    SensorData::Sensor sensor; /**< The id of the sensor. */
    Type type;  /**< The type of measurements the sensor takes. */
    bool endOfSection; /**< Is this entry the last in a group of sensors? */
  };

  bool formatSensor(Type type, float inValue, bool filtered, char* value) const
  {
    switch(type)
    {
      case usLDistance: inValue = usDistances[filtered ? 1 : 0][SensorData::left]; break;
      case usLtRDistance: inValue = usDistances[filtered ? 1 : 0][SensorData::leftToRight]; break;
      case usRtLDistance: inValue = usDistances[filtered ? 1 : 0][SensorData::rightToLeft]; break;
      case usRDistance: inValue = usDistances[filtered ? 1 : 0][SensorData::right]; break;
      default: break;
    }
    if(inValue == SensorData::off)
    {
      strcpy(value, "?");
      return true;
    }
    switch(type)
    {
      case SensorWidget::distance:
        sprintf(value, "%.1fmm", inValue);
        break;
      case SensorWidget::pressure:
        sprintf(value, "%.1fg", inValue * 1000);
        break;
      case SensorWidget::acceleration:
        sprintf(value, "%.1fmg", inValue * 1000);
        break;
      case SensorWidget::batteryLevel:
        sprintf(value, "%.1fV", inValue);
        break;
      case SensorWidget::angularSpeed:
        sprintf(value, "%.1f°/s", inValue);
        break;
      case SensorWidget::ratio:
        sprintf(value, "%.1f%%", inValue * 100);
        break;
      case SensorWidget::angle2:
        sprintf(value, "%.1f°", toDegrees(inValue));
        break;
      case SensorWidget::usLDistance:
        sprintf(value, "%.0fmm", inValue);
        break;
      case SensorWidget::usLtRDistance:
        sprintf(value, "%.0fmm", inValue);
        break;
      case SensorWidget::usRtLDistance:
        sprintf(value, "%.0fmm", inValue);
        break;
      case SensorWidget::usRDistance:
        sprintf(value, "%.0fmm", inValue);
        break;
      default:
        return false;
    }
    return true;
  }

  const char* getSensorName(const ViewInfo& vi) const
  {
    switch(vi.type)
    {
      case usLDistance: return "usLeft";
      case usLtRDistance: return "usLeftToRight";
      case usRtLDistance: return "usRightToLeft";
      case usRDistance: return "usRight";
      default: return SensorData::getSensorName(vi.sensor);
    }
  }

  void paintEvent(QPaintEvent *event)
  {  
    static const ViewInfo viewInfoNao[] =  {
      {SensorData::gyroX,    angularSpeed, false},
      {SensorData::gyroY,    angularSpeed, true},

      {SensorData::accX,    acceleration, false},
      {SensorData::accY,    acceleration, false},
      {SensorData::accZ,    acceleration, true},

      {SensorData::batteryLevel, ratio, true},

      {SensorData::fsrLFL,  pressure, false},
      {SensorData::fsrLFR,  pressure, false},
      {SensorData::fsrLBL,  pressure, false},
      {SensorData::fsrLBR,  pressure, false},
      {SensorData::fsrRFL,  pressure, false},
      {SensorData::fsrRFR,  pressure, false},
      {SensorData::fsrRBL,  pressure, false},
      {SensorData::fsrRBR,  pressure, true},

      {SensorData::us,  usLDistance,   false},
      {SensorData::us,  usLtRDistance, false},
      {SensorData::us,  usRtLDistance, false},
      {SensorData::us,  usRDistance,   true},

      {SensorData::angleX,  angle2, false},
      {SensorData::angleY,  angle2, false},
      {SensorData::numOfSensors, end, true}
    };

    painter.begin(this);
    painter.setFont(font);
    painter.setBrush(altBrush);
    painter.setPen(fontPen);
    fillBackground = false;

    paintRect = painter.window();
    paintRectField0 = QRect(headerView->sectionViewportPosition(0) + textOffset, 0, headerView->sectionSize(0) - textOffset * 2, lineSpacing);
    paintRectField1 = QRect(headerView->sectionViewportPosition(1) + textOffset, 0, headerView->sectionSize(1) - textOffset * 2, lineSpacing);
    paintRectField2 = QRect(headerView->sectionViewportPosition(2) + textOffset, 0, headerView->sectionSize(2) - textOffset * 2, lineSpacing);
    {
      char value[32];
      char filtered[32];
      SYNC_WITH(console);
      const SensorData& sensorData(sensorView.sensorData);
      const SensorData& filteredSensorData(sensorView.filteredSensorData);

      if(sensorData.data[SensorData::us] != SensorData::off)
        usDistances[0][sensorData.usSensorType] = sensorData.data[SensorData::us];
      if(filteredSensorData.data[SensorData::us] != SensorData::off)
        usDistances[1][sensorData.usSensorType] = filteredSensorData.data[SensorData::us];

      for(int i = 0;; ++i)
      {
        const ViewInfo &vi = viewInfoNao[i];
        if(vi.type == SensorWidget::end)
          break;
        
        formatSensor(vi.type, sensorData.data[vi.sensor], false, value);
        formatSensor(vi.type, filteredSensorData.data[vi.sensor], true, filtered);

        print(getSensorName(vi), value, filtered);
        newBlock();
        if(vi.endOfSection)
          newSection();
      }
    }
    painter.end();
    setMinimumHeight(paintRectField1.top());
  }

private:
  RobotConsole &console;
  SensorView& sensorView;
  QHeaderView* headerView;

  QPainter painter;
  int lineSpacing;
  int textOffset;

  QFont font;
  QBrush altBrush;
  QPen fontPen;
  QPen noPen;
  bool fillBackground;

  QRect paintRect;
  QRect paintRectField0;
  QRect paintRectField1;
  QRect paintRectField2;

  float usDistances[2][SensorData::numOfUsSensorTypes];

  void print(const char* name, const char* value1, const char* value2)
  {
    if(fillBackground)
    {
      painter.setPen(noPen);
      painter.drawRect(paintRect.left(), paintRectField1.top(), paintRect.width(), paintRectField1.height());
      painter.setPen(fontPen);
    }
    painter.drawText(paintRectField0, Qt::TextSingleLine | Qt::AlignVCenter, name);
    painter.drawText(paintRectField1, Qt::TextSingleLine | Qt::AlignVCenter | Qt::AlignRight, value1);
    painter.drawText(paintRectField2, Qt::TextSingleLine | Qt::AlignVCenter | Qt::AlignRight, value2);
    paintRectField0.moveTop(paintRectField0.top() + lineSpacing);
    paintRectField1.moveTop(paintRectField1.top() + lineSpacing);
    paintRectField2.moveTop(paintRectField2.top() + lineSpacing);
  }
  
  void newBlock()
  {
    fillBackground = fillBackground ? false : true;
  }

  void newSection()
  {
    painter.drawLine(paintRect.left(), paintRectField1.top(), paintRect.width(), paintRectField1.top());
    paintRectField0.moveTop(paintRectField0.top() + 1);
    paintRectField1.moveTop(paintRectField1.top() + 1);
    paintRectField2.moveTop(paintRectField2.top() + 1);
    fillBackground = false;
  }

  QSize sizeHint () const { return QSize(160, 350); }
};

SensorView::SensorView(RobotConsole& robotConsole, const SensorData& sensorData, const SensorData& filteredSensorData) : 
  console(robotConsole), sensorData(sensorData), filteredSensorData(filteredSensorData) {};

QWidget* SensorView::createWidget()
{
  HeaderedWidget* widget = new HeaderedWidget();
  QStringList headerLabels;
  headerLabels << "Sensor" << "Value" << "Filtered";
  widget->setHeaderLabels(headerLabels, "lrr");
  QHeaderView* headerView = widget->getHeaderView();
  sensorWidget = new SensorWidget(*this, console, headerView, widget);
  widget->setWidget(sensorWidget);  
  headerView->setMinimumSectionSize(30);
  headerView->resizeSection(0, 60);
  headerView->resizeSection(1, 50);
  headerView->resizeSection(2, 50);
  return widget; 
}

void SensorView::updateWidget(QWidget& widget)
{
  {
    SYNC_WITH(console);
    if(sensorData.timeStamp == lastUpdateTimeStamp)
      return;
    lastUpdateTimeStamp = sensorData.timeStamp;
  }

  sensorWidget->update();
}

std::string SensorView::getConfiguration(QWidget& widget) const
{
  return std::string(((HeaderedWidget*)(&widget))->getHeaderView()->saveState().toBase64().constData());
}

void SensorView::setConfiguration(QWidget& widget, const std::string& conf)
{
  ((HeaderedWidget*)(&widget))->getHeaderView()->restoreState(QByteArray::fromBase64(QByteArray(conf.c_str())));
}

