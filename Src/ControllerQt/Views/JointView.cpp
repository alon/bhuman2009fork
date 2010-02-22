/**
* @file ControllerQt/Views/JointView.cpp
* Implementation of class JointView
* @author Colin Graf
*/

#include <QHeaderView>
#include <QApplication>
#include <QPainter>
#include <QFontMetrics>

#include "JointView.h"
#include "Platform/Thread.h"
#include "../RobotConsole.h"
#include "../Visualization/HeaderedWidget.h"

class JointWidget : public QWidget
{
public:
  JointWidget(JointView& jointView, RobotConsole& console, QHeaderView* headerView, QWidget* parent) : QWidget(parent),
    console(console), jointView(jointView), headerView(headerView), noPen(Qt::NoPen)
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
  }

  void paintEvent(QPaintEvent *event)
  {  
    painter.begin(this);
    painter.setFont(font);
    painter.setBrush(altBrush);
    painter.setPen(fontPen);
    fillBackground = false;

    paintRect = painter.window();
    paintRectField0 = QRect(headerView->sectionViewportPosition(0) + textOffset, 0, headerView->sectionSize(0) - textOffset * 2, lineSpacing);
    paintRectField1 = QRect(headerView->sectionViewportPosition(1) + textOffset, 0, headerView->sectionSize(1) - textOffset * 2, lineSpacing);
    paintRectField2 = QRect(headerView->sectionViewportPosition(2) + textOffset, 0, headerView->sectionSize(2) - textOffset * 2, lineSpacing);
    paintRectField3 = QRect(headerView->sectionViewportPosition(3) + textOffset, 0, headerView->sectionSize(3) - textOffset * 2, lineSpacing);
    paintRectField4 = QRect(headerView->sectionViewportPosition(4) + textOffset, 0, headerView->sectionSize(4) - textOffset * 2, lineSpacing);
    {
      char request[32], sensor[32], load[32], temp[32];
      SYNC_WITH(console);
      const JointData& jointData(jointView.jointData);
      const SensorData& sensorData(jointView.sensorData);
      const JointData& jointRequest(jointView.jointRequest);
      for(int i = 0; i < JointData::numOfJoints; ++i)
      {
        if(i == JointData::armLeft0 || i == JointData::armRight0 || i == JointData::legLeft0 || i == JointData::legRight0)
          newSection();
        jointRequest.angles[i] == JointData::off ? (void)strcpy(request, "off") : (void)sprintf(request, "%.1f°", toDegrees(jointRequest.angles[i]));
        jointData.angles[i] == JointData::off ? (void)strcpy(sensor, "?") : (void)sprintf(sensor, "%.1f°", toDegrees(jointData.angles[i]));
        sensorData.temperatures[i] == 0 ? (void)strcpy(load, "?") : (void)sprintf(load, "%dmA", sensorData.currents[i]);
        sensorData.temperatures[i] == 0 ? (void)strcpy(temp, "?") : (void)sprintf(temp, "%d°C", sensorData.temperatures[i]);
        print(JointData::getJointName(JointData::Joint(i)), request, sensor, load, temp);
        newBlock();
      }
    }
    painter.end();
    setMinimumHeight(paintRectField1.top());
  }

private:
  RobotConsole& console; /**< A reference to the console object. */
  JointView& jointView;
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
  QRect paintRectField3;
  QRect paintRectField4;

  void print(const char* name, const char* value1, const char* value2, const char* value3, const char* value4)
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
    painter.drawText(paintRectField3, Qt::TextSingleLine | Qt::AlignVCenter | Qt::AlignRight, value3);
    painter.drawText(paintRectField4, Qt::TextSingleLine | Qt::AlignVCenter | Qt::AlignRight, value4);
    paintRectField0.moveTop(paintRectField0.top() + lineSpacing);
    paintRectField1.moveTop(paintRectField1.top() + lineSpacing);
    paintRectField2.moveTop(paintRectField2.top() + lineSpacing);
    paintRectField3.moveTop(paintRectField3.top() + lineSpacing);
    paintRectField4.moveTop(paintRectField4.top() + lineSpacing);
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
    paintRectField3.moveTop(paintRectField3.top() + 1);
    paintRectField4.moveTop(paintRectField4.top() + 1);
    fillBackground = false;
  }

  QSize sizeHint () const { return QSize(260, 400); }
};


JointView::JointView(RobotConsole& robotConsole, const JointData& jointData, const SensorData& sensorData, const JointData& jointRequest) :
  console(robotConsole), jointData(jointData), sensorData(sensorData), jointRequest(jointRequest) {}

QWidget* JointView::createWidget()
{
  HeaderedWidget* widget = new HeaderedWidget();
  QStringList headerLabels;
  headerLabels << "Joint" << "Request" << "Sensor" << "Load" << "Temp";
  widget->setHeaderLabels(headerLabels, "lrrrr");
  QHeaderView* headerView = widget->getHeaderView();
  jointWidget = new JointWidget(*this, console, headerView, widget);
  widget->setWidget(jointWidget);  
  headerView->setMinimumSectionSize(30);
  headerView->resizeSection(0, 60);
  headerView->resizeSection(1, 50);
  headerView->resizeSection(2, 50);
  headerView->resizeSection(3, 50);
  headerView->resizeSection(4, 50);
  return widget; 
}

void JointView::updateWidget(QWidget& widget)
{
  {
    SYNC_WITH(console);
    if(jointData.timeStamp == lastUpdateTimeStamp)
      return;
    lastUpdateTimeStamp = jointData.timeStamp;
  }

  jointWidget->update();
}

std::string JointView::getConfiguration(QWidget& widget) const
{
  return std::string(((HeaderedWidget*)(&widget))->getHeaderView()->saveState().toBase64().constData());
}

void JointView::setConfiguration(QWidget& widget, const std::string& conf)
{
  ((HeaderedWidget*)(&widget))->getHeaderView()->restoreState(QByteArray::fromBase64(QByteArray(conf.c_str())));
}
