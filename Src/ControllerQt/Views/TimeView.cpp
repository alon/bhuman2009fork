/**
* @file ControllerQt/Views/TimeView.cpp
*
* Implementation of class TimeView
*
* @author Colin Graf
*/

#include <QHeaderView>
#include <QApplication>
#include <QPainter>
#include <QFontMetrics>

#include "TimeView.h"
#include "Platform/Thread.h"
#include "../RobotConsole.h"
#include "../Visualization/HeaderedWidget.h"

class TimeWidget : public QWidget
{
public:
  TimeWidget(RobotConsole& console, const TimeInfo& info, QHeaderView* headerView, QWidget* parent) : 
    console(console), info(info), headerView(headerView), noPen(Qt::NoPen)
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
    paintRectField5 = QRect(headerView->sectionViewportPosition(5) + textOffset, 0, headerView->sectionSize(5) - textOffset * 2, lineSpacing);
    paintRectField6 = QRect(headerView->sectionViewportPosition(6) + textOffset, 0, headerView->sectionSize(6) - textOffset * 2, lineSpacing);
    {
      SYNC_WITH(console);
      
      for(TimeInfo::Infos::const_iterator i = info.infos.begin(), end = info.infos.end(); i != end; ++i)
      {
        double minTime = -1, maxTime = -1, avgTime = -1, minDelta = -1, maxDelta = -1, avgFreq = -1;
        char fminTime[100], fmaxTime[100], favgTime[100], fminDelta[100], fmaxDelta[100], favgFreq[100];
        if(info.getStatistics(i->second, minTime, maxTime, avgTime, minDelta, maxDelta, avgFreq))
        {
          sprintf(fminTime, "%.02f", minTime);
          sprintf(fmaxTime, "%.02f", maxTime);
          sprintf(favgTime, "%.02f", avgTime);
          sprintf(fminDelta, "%.02f", minDelta);
          sprintf(fmaxDelta, "%.02f", maxDelta);
          sprintf(favgFreq, "%.02f", avgFreq);
          print(i->first.c_str(), fminTime, fmaxTime, favgTime, fminDelta, fmaxDelta, favgFreq);
          newBlock();
        }
      }
    }
    painter.end();
    setMinimumHeight(paintRectField1.top());
  }


private:
  RobotConsole& console; /**< A reference to the console object. */
  const TimeInfo& info; /**< The Time info structure. */
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
  QRect paintRectField5;
  QRect paintRectField6;

  void print(const char* name, const char* value1, const char* value2, const char* value3, const char* value4, const char* value5, const char* value6)
  {
    if(fillBackground)
    {
      painter.setPen(noPen);
      painter.drawRect(paintRect.left(), paintRectField1.top(), paintRect.width(), paintRectField1.height());
      painter.setPen(fontPen);
    }
    painter.drawText(paintRectField0, Qt::TextSingleLine | Qt::AlignVCenter, name);
    painter.drawText(paintRectField1, Qt::TextSingleLine | Qt::AlignVCenter, value1);
    painter.drawText(paintRectField2, Qt::TextSingleLine | Qt::AlignVCenter, value2);
    painter.drawText(paintRectField3, Qt::TextSingleLine | Qt::AlignVCenter, value3);
    painter.drawText(paintRectField4, Qt::TextSingleLine | Qt::AlignVCenter, value4);
    painter.drawText(paintRectField5, Qt::TextSingleLine | Qt::AlignVCenter, value5);
    painter.drawText(paintRectField6, Qt::TextSingleLine | Qt::AlignVCenter, value6);
    paintRectField0.moveTop(paintRectField0.top() + lineSpacing);
    paintRectField1.moveTop(paintRectField1.top() + lineSpacing);
    paintRectField2.moveTop(paintRectField2.top() + lineSpacing);
    paintRectField3.moveTop(paintRectField3.top() + lineSpacing);
    paintRectField4.moveTop(paintRectField4.top() + lineSpacing);
    paintRectField5.moveTop(paintRectField5.top() + lineSpacing);
    paintRectField6.moveTop(paintRectField6.top() + lineSpacing);
  }
  
  void newBlock()
  {
    fillBackground = fillBackground ? false : true;
  }
};


TimeView::TimeView(RobotConsole& console, const TimeInfo& info) :
  console(console), info(info) {}

QWidget* TimeView::createWidget()
{
  HeaderedWidget* widget = new HeaderedWidget();
  QStringList headerLabels;
  headerLabels << "Module" << "Min" << "Max" << "Avg" << "MinDelta" << "MaxDelta" << "AvgFreq";
  widget->setHeaderLabels(headerLabels, "lrrrrrr");
  QHeaderView* headerView = widget->getHeaderView();
  timeWidget = new TimeWidget(console, info, headerView, widget);
  widget->setWidget(timeWidget);  
  headerView->setMinimumSectionSize(30);
  headerView->resizeSection(0, 100);
  headerView->resizeSection(1, 50);
  headerView->resizeSection(2, 50);
  headerView->resizeSection(3, 50);
  headerView->resizeSection(4, 50);
  headerView->resizeSection(5, 50);
  headerView->resizeSection(6, 50);
  return widget; 
}

void TimeView::updateWidget(QWidget& widget)
{
  {
    SYNC_WITH(console);
    if(info.timeStamp == lastTimeInfoTimeStamp)
      return;
    lastTimeInfoTimeStamp = info.timeStamp;
  }

  timeWidget->update();
}

std::string TimeView::getConfiguration(QWidget& widget) const
{
  return std::string(((HeaderedWidget*)(&widget))->getHeaderView()->saveState().toBase64().constData());
}

void TimeView::setConfiguration(QWidget& widget, const std::string& conf)
{
  ((HeaderedWidget*)(&widget))->getHeaderView()->restoreState(QByteArray::fromBase64(QByteArray(conf.c_str())));
}
