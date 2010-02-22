/**
* @file ControllerQt/Views/XabslView.cpp
*
* Implementation of class XabslView
*
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
* @author Colin Graf
*/

#include <QHeaderView>
#include <QApplication>
#include <QPainter>
#include <QFontMetrics>
#include <QKeyEvent>

#include "XabslView.h"
#include "Platform/Thread.h"
#include "../RobotConsole.h"
#include "../Visualization/HeaderedWidget.h"

class XabslWidget : public QWidget
{
public:
  XabslWidget(RobotConsole& console, const XabslInfo& info, QHeaderView* headerView, QWidget* parent) : 
    console(console), info(info), headerView(headerView), noPen(Qt::NoPen)
  {
    setBackgroundRole(QPalette::Base);

    font = QApplication::font();
    boldFont = font;
    boldFont.setBold(true);
    setFontPointSize(getFontPointSize());
    
    const QPalette& pal(QApplication::palette());
    altBrush = pal.alternateBase();
    fontPen.setColor(pal.text().color());
    
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setFocusPolicy(Qt::StrongFocus);
  }
  
  int getFontPointSize()
  {
    return font.pointSize();
  }
  
  void setFontPointSize(int size)
  {
    font.setPointSize(size);
    boldFont.setPointSize(size);
    QFontMetrics me(font);
    lineSpacing = me.lineSpacing() + 2;
    textOffset = me.descent() + 1;
  }

  void paintEvent(QPaintEvent *event)
  {  
    painter.begin(this);
    painter.setFont(font);
    painter.setBrush(altBrush);
    painter.setPen(fontPen);
    fillBackground = false;

    paintRect = painter.window();
    int defaultLeft = headerView->sectionViewportPosition(0) + textOffset;
    paintRectField0 = QRect(defaultLeft, 0, headerView->sectionSize(0) - textOffset * 2, lineSpacing);
    paintRectField1 = QRect(headerView->sectionViewportPosition(1) + textOffset, 0, headerView->sectionSize(1) - textOffset * 2, lineSpacing);
    {
      SYNC_WITH(console);

      print("Agent:", (info.id + " - " + info.selectedAgentName).c_str(), true);
      newBlock();

      newSection();
      print("Motion Request:", info.executedMotionRequest.c_str(), true);
      newBlock();

      newSection();
      print("Output Symbols:", 0, true);
      paintRectField0.setLeft(paintRectField0.left() + 10);
      for(std::list<XabslInfo::OutputSymbol>::const_iterator i = info.outputSymbols.begin(), end = info.outputSymbols.end(); i != end; ++i)
        if(i->show)
          print(i->name.c_str(), i->value.c_str(), false);
      paintRectField0.setLeft(defaultLeft);
      newBlock();

      newSection();
      print("Input Symbols:", 0, true);
      paintRectField0.setLeft(paintRectField0.left() + 10);
      for(std::list<XabslInfo::InputSymbol>::const_iterator i = info.inputSymbols.begin(), end = info.inputSymbols.end(); i != end; ++i)
        if(i->show)
          print(i->name.c_str(), i->value.c_str(), false);
      paintRectField0.setLeft(defaultLeft);
      newBlock();

      newSection();
      print("Option Activation Graph:", 0, true);
      newBlock();
      for(std::list<XabslInfo::ActiveOption>::const_iterator i = info.activeOptions.begin(), end = info.activeOptions.end(); i != end; ++i)
      {
        paintRectField0.setLeft(defaultLeft + 10 * (i->depth + 1));

        print(i->name.c_str(), i->durationOfActivation.c_str(), true, true);

        if(!i->parameters.empty())
        {
          int space = paintRectField0.left();
          paintRectField0.setLeft(defaultLeft + 10);
          for(std::list<XabslInfo::NameValue>::const_iterator j = i->parameters.begin(), end = i->parameters.end(); j != end; ++j)
            print(j->name.c_str(), j->value.c_str(), false);
          paintRectField0.setLeft(space);
        }

        paintRectField0.setLeft(paintRectField0.left() + 5);
        print(i->activeState.c_str(), i->durationOfStateActivation.c_str(), false, true);
        newBlock();
      }
    }
    painter.end();
    setMinimumHeight(paintRectField1.top());
  }


private:
  RobotConsole& console; /**< A reference to the console object. */
  const XabslInfo& info; /**< The Xabsl info structure. */
  QHeaderView* headerView;
  QPainter painter;
  int lineSpacing;
  int textOffset;

  QFont font;
  QFont boldFont;
  QBrush altBrush;
  QPen fontPen;
  QPen noPen;
  bool fillBackground;

  QRect paintRect;
  QRect paintRectField0;
  QRect paintRectField1;

  void print(const char* name, const char* value, bool bold = false, bool rightAlign = false)
  {
    if(fillBackground)
    {
      painter.setPen(noPen);
      painter.drawRect(paintRect.left(), paintRectField1.top(), paintRect.width(), paintRectField1.height());
      painter.setPen(fontPen);
    }
    if(name)
    {
      if(bold)
        painter.setFont(boldFont);
      painter.drawText(paintRectField0, ((!value || rightAlign) ? Qt::TextDontClip : 0) | Qt::TextSingleLine | Qt::AlignVCenter, name);
      if(bold)
        painter.setFont(font);
    }
    if(value)
      painter.drawText(paintRectField1, (rightAlign ? Qt::AlignRight : Qt::AlignLeft) | Qt::TextSingleLine | Qt::AlignVCenter, value);
    paintRectField0.moveTop(paintRectField0.top() + lineSpacing);
    paintRectField1.moveTop(paintRectField1.top() + lineSpacing);
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
    fillBackground = false;
  }

  void keyPressEvent(QKeyEvent* event)
  {
    switch(event->key())
    {
    case Qt::Key_PageUp:
    case Qt::Key_Plus:
      event->accept();
      if(getFontPointSize() < 48)
        setFontPointSize(getFontPointSize() + 1);
      update();
      break;
    case Qt::Key_PageDown:
    case Qt::Key_Minus:
      event->accept();
      if(getFontPointSize() > 3)
        setFontPointSize(getFontPointSize() - 1);
      update();
      break;
    default:
      QWidget::keyPressEvent(event);
      break;
    }
  }

  QSize sizeHint () const { return QSize(200, 500); }
};

XabslView::XabslView(RobotConsole& console, const XabslInfo& info) : 
  console(console), info(info) {}

QWidget* XabslView::createWidget()
{
  HeaderedWidget* widget = new HeaderedWidget();
  QStringList headerLabels;
  headerLabels << "Name" << "Value";
  widget->setHeaderLabels(headerLabels);
  QHeaderView* headerView = widget->getHeaderView();
  xabslWidget = new XabslWidget(console, info, headerView, widget);
  widget->setWidget(xabslWidget);
  headerView->setMinimumSectionSize(30);
  headerView->resizeSection(0, 100);
  headerView->resizeSection(1, 100);
  return widget; 
}

void XabslView::updateWidget(QWidget& widget)
{
  {
    SYNC_WITH(console);
    if(info.timeStamp == lastXabslInfoTimeStamp)
      return;
    lastXabslInfoTimeStamp = info.timeStamp;
  }

  xabslWidget->update();
}

std::string XabslView::getConfiguration(QWidget& widget) const
{
  QByteArray block;
  QDataStream out(&block, QIODevice::WriteOnly);
  out.setVersion(QDataStream::Qt_4_3);
  out << ((HeaderedWidget*)(&widget))->getHeaderView()->saveState();
  out << ((XabslWidget*)xabslWidget)->getFontPointSize();
  return std::string(block.toBase64().constData());
}

void XabslView::setConfiguration(QWidget& widget, const std::string& conf)
{
  const QByteArray& block(QByteArray::fromBase64(conf.c_str()));
  QDataStream in(block);
  in.setVersion(QDataStream::Qt_4_3);
  QByteArray headerState;
  in >> headerState;
  int fontSize;
  in >> fontSize;
  if(fontSize)
    ((XabslWidget*)xabslWidget)->setFontPointSize(fontSize);
  if(headerState.isNull())
    ((HeaderedWidget*)(&widget))->getHeaderView()->restoreState(headerState);
}

