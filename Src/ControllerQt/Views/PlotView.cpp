/**
* @file ControllerQt/Views/PlotView.cpp
*
* Implementation of class PlotView
*
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
* @author Colin Graf
*/

#include <QPainter>
#include <QMouseEvent>
#include <QMenu>
#include <sstream>

#include "PlotView.h"
#include "../RobotConsole.h"

class PlotWidget : public QWidget
{
public:
  PlotWidget(PlotView& plotView) : 
    blackPen(QColor(0x00, 0x00, 0x00)), grayPen(QColor(0xbb, 0xbb, 0xbb)), plotView(plotView) 
  {
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
  }

  QPen blackPen;
  QPen grayPen;

private:

  void paintEvent(QPaintEvent *event)
  {     
    painter.begin(this);
    plotView.paintAll(painter, this);
    painter.end();
  }

  void contextMenuEvent ( QContextMenuEvent * event )
  //void mouseReleaseEvent(QMouseEvent* event)
  {
    //if(event->button() == Qt::LeftButton)
    {
      event->accept();
      QMenu menu;
      QAction* unitAction = menu.addAction(tr("Show Units"));
      unitAction->setCheckable(true);
      unitAction->setChecked(plotView.drawUnits);
      QAction* legendAction = menu.addAction(tr("Show Legend"));
      legendAction->setCheckable(true);
      legendAction->setChecked(plotView.drawLegend);
      //QAction* aaAction = menu.addAction(tr("Anti-aliased"));
      //aaAction->setCheckable(true);
      //aaAction->setChecked(plotView.antialiasing);

      menu.addSeparator();
      QAction* ammAction = menu.addAction(tr("Auto-min-max"));

      QAction* userAction = menu.exec(mapToGlobal(QPoint(event->x(), event->y())));
      if(userAction == unitAction)
        plotView.drawUnits = !plotView.drawUnits;
      else if(userAction == legendAction)
        plotView.drawLegend = !plotView.drawLegend;
      //else if(userAction == aaAction)
        //plotView.antialiasing = !plotView.antialiasing;
      else if(userAction == ammAction)
        plotView.determineMinMaxValue();

      update();
    }
    //else
      //QGLWidget::mouseReleaseEvent(event);
  }

  QPainter painter; /**< The painter used for painting the plot. */
  PlotView& plotView;
};


PlotView::PlotView(RobotConsole& console, const std::string& name, 
 unsigned int plotSize, const double& minValue, const double& maxValue,
 const std::string& yUnit, const std::string& xUnit, const double& xScale) :
 drawUnits(true), drawLegend(true), antialiasing(false),
 console(console), name(name), plotSize(plotSize), plotSizeF(double(plotSize)), 
 minValue(minValue), maxValue(maxValue), valueLength(maxValue - minValue),
 yUnit(yUnit), xUnit(xUnit), xScale(xScale), lastTimeStamp(0)
{
  points = new QPointF[plotSize];
  for(unsigned int i = 0; i < plotSize; ++i)
    points[i].rx() = plotSize - 1 - i;
}

PlotView::~PlotView()
{
  delete [] points;
}

QWidget* PlotView::createWidget()
{
  return new PlotWidget(*this);
}

void PlotView::updateWidget(QWidget& widget)
{
  if(needsRepaint())
    ((PlotWidget*)(&widget))->update();
}

void PlotView::paintAll(QPainter& painter, PlotWidget* widget)
{
  if(antialiasing)
    painter.setRenderHints(QPainter::TextAntialiasing);

  // calculate margin sizes
  const double space = 3;
  double leftMargin;
  double bottomMargin;
  double topMargin;
  double rightMargin;
  double textHeight;
  const QFontMetrics& fontMetrics(painter.fontMetrics());
  {
    char buf[32];
    sprintf(buf, "%g", (maxValue - minValue) < 8. ? (minValue / 4.) : minValue);
    const QSize& bufSize(fontMetrics.size(Qt::TextSingleLine, buf));
    textHeight = bufSize.height();
    leftMargin = bufSize.width();
    bottomMargin = textHeight + space * 2;
    topMargin = space + textHeight / 2.;
    sprintf(buf, "%g", (maxValue - minValue) < 8. ? (maxValue / 4.) : maxValue);
    const QSize& bufSize2(fontMetrics.size(Qt::TextSingleLine, buf));
    if(bufSize2.width() > leftMargin)
      leftMargin = bufSize2.width();
    rightMargin = fontMetrics.size(Qt::TextSingleLine, "0").width() / 2. + space;
    leftMargin += space * 2;
    if(drawUnits)
    {
      if(!yUnit.empty())
        topMargin += textHeight + space;
      if(!xUnit.empty())
        rightMargin += fontMetrics.size(Qt::TextSingleLine, xUnit.c_str()).width() + space;
    }
  }

  // calculate size of the plot area
  const QRect& windowRect(painter.window());
  QRect plotRect(QPoint(windowRect.x() + leftMargin, windowRect.y() + topMargin), 
    QPoint(windowRect.right() - rightMargin, windowRect.bottom() - bottomMargin));
  if(!plotRect.isValid())
    return; // window too small

  // calculate step sizes to regrade the axes
  double stepY = pow(10., ceil(log10(valueLength * 20. / plotRect.height())));
  double stepX = pow(10., ceil(log10(plotSizeF * 25. / plotRect.width())));
  if(stepY * plotRect.height() / valueLength >= 40.)
    stepY /= 2.;
  if(stepY * plotRect.height() / valueLength >= 40.)
    stepY /= 2.;
  if(stepY > std::max<>(maxValue, -minValue))
    stepY = std::max<>(maxValue, -minValue);
  if(plotRect.width() * stepX / plotSizeF >= 50.)
    stepX /= 2.;
  if(plotRect.width() * stepX / plotSizeF >= 50.)
    stepX /= 2.;
  if(stepX > plotSizeF)
    stepX = plotSizeF;
  
  if((maxValue - minValue) < 8.)
  {
    char buf[32];
    sprintf(buf, "%g", stepY < fabs(maxValue) ? (maxValue - stepY) : maxValue);
    int width = fontMetrics.size(Qt::TextSingleLine, buf).width();
    sprintf(buf, "%g", stepY < fabs(minValue) ? (minValue + stepY) : minValue);
    int width2 = fontMetrics.size(Qt::TextSingleLine, buf).width();
    if(width2 > width)
      width = width2;
    width += space * 2;
    if(width < leftMargin)
    {
      plotRect.setLeft(plotRect.left() - (leftMargin - width));
      leftMargin = width;
    }
  }

  // draw axis lables
  {
    char buf[32];
    painter.setPen(widget->blackPen);

    // y
    {      
      QRectF rect(0, 0, leftMargin - space, textHeight);
      sprintf(buf, "%g", maxValue);
      rect.moveTop((valueLength - (maxValue - minValue)) * plotRect.height() / valueLength + topMargin - textHeight / 2.);
      painter.drawText(rect, Qt::AlignRight, buf);
      sprintf(buf, "%g", minValue);
      rect.moveTop((valueLength - (minValue - minValue)) * plotRect.height() / valueLength + topMargin - textHeight / 2.);
      painter.drawText(rect, Qt::AlignRight, buf);
      rect.moveTop((valueLength - (0. - minValue)) * plotRect.height() / valueLength + topMargin - textHeight / 2.);
      painter.drawText(rect, Qt::AlignRight, QString("0"));
      const double& minOffset(valueLength * 20. / plotRect.height());
      for(double pos = stepY; pos <= maxValue - minOffset; pos += stepY)
      {
        sprintf(buf, "%g", pos);
        rect.moveTop((valueLength - (pos - minValue)) * plotRect.height() / valueLength + topMargin - textHeight / 2.);
        painter.drawText(rect, Qt::AlignRight, buf);
      }
      for(double pos = -stepY; pos >= minValue + minOffset; pos -= stepY)
      {
        sprintf(buf, "%g", pos);
        rect.moveTop(((maxValue - minValue) - (pos - minValue)) * plotRect.height() / valueLength + topMargin - textHeight / 2.);
        painter.drawText(rect, Qt::AlignRight, buf);
      }
    }

    // x
    {
      QRectF rect(0, plotRect.bottom() + space, leftMargin - space, textHeight);
      sprintf(buf, "%.2f", plotSizeF * xScale);
      rect.setWidth(fontMetrics.size(Qt::TextSingleLine, buf).width());
      sprintf(buf, "%g", plotSizeF * xScale);
      rect.moveLeft((plotSizeF - plotSizeF) * plotRect.width() / plotSizeF + leftMargin - rect.width() / 2.);
      painter.drawText(rect, Qt::AlignCenter, buf);
      rect.moveLeft((plotSizeF - 0.) * plotRect.width() / plotSizeF + leftMargin - rect.width() / 2.);
      painter.drawText(rect, Qt::AlignCenter, QString("0"));
      const double& minOffset(plotSizeF * 25. / plotRect.width());      
      for(double pos = stepX; pos <= plotSizeF - minOffset; pos += stepX)
      {
        sprintf(buf, "%g", pos * xScale);
        rect.moveLeft((plotSizeF - pos) * plotRect.width() / plotSizeF + leftMargin - rect.width() / 2.);
        painter.drawText(rect, Qt::AlignCenter, buf);
      }
    }
  }

  // draw units
  if(drawUnits)
  {
    if(!yUnit.empty())
    {
      QRect rect(space, space, windowRect.width() - space, textHeight);
      painter.drawText(rect, Qt::AlignLeft, yUnit.c_str());
    }
    if(!xUnit.empty())
    {
      QRect rect(0., plotRect.bottom() + space, windowRect.width() - space, textHeight);
      painter.drawText(rect, Qt::AlignRight, xUnit.c_str());
    }
  }

  // setup plot-paint-transformation
  {
    QTransform transform;
    transform.translate(plotRect.right(), plotRect.top() + double(plotRect.height()) * maxValue / valueLength);
    transform.scale(-double(plotRect.width()) / plotSizeF, -double(plotRect.height()) / valueLength);
    painter.setTransform(transform);
  }

  // draw axes and regrade-lines
  painter.setPen(widget->grayPen);
  double twoPxX = plotSizeF * 2. / plotRect.width();
  double twoPxY = valueLength * 2. / plotRect.height();
  for(double pos = stepY; pos < maxValue; pos += stepY)
    painter.drawLine(QPointF(0., pos), QPointF(plotSizeF + twoPxX, pos));
  for(double pos = -stepY; pos > minValue; pos -= stepY)
    painter.drawLine(QPointF(0., pos), QPointF(plotSizeF + twoPxX, pos));
  for(double pos = stepX; pos < plotSizeF; pos += stepX)
    painter.drawLine(QPointF(pos, minValue - twoPxY), QPointF(pos, maxValue));
  painter.setPen(widget->blackPen);
  painter.drawLine(QPointF(0., 0.), QPointF(plotSizeF + twoPxX, 0.));
  painter.drawLine(QPointF(0., minValue - twoPxY), QPointF(0., maxValue));
  painter.drawLine(QPointF(plotSizeF, minValue - twoPxY), QPointF(plotSizeF, maxValue));
  if(minValue != 0.)
    painter.drawLine(QPointF(0., minValue), QPointF(plotSizeF + twoPxX, minValue));
  if(maxValue != 0.)
    painter.drawLine(QPointF(0., maxValue), QPointF(plotSizeF + twoPxX, maxValue));

  {
    SYNC_WITH(console);

    // draw plots
    if(antialiasing)
      painter.setRenderHints(QPainter::Antialiasing| QPainter::HighQualityAntialiasing);
    int legendWidth = 0;
    const std::list<RobotConsole::Layer>& plotList(console.plotViews[name]);
    for(std::list<RobotConsole::Layer>::const_iterator i = plotList.begin(), end = plotList.end(); 
        i != end; ++i)
    {
      const std::list<double>& list = console.plots[i->layer].points;
      int numOfPoints = std::min<>(list.size(), plotSize);
      if(numOfPoints > 1)
      {
        std::list<double>::const_iterator k = list.begin();
        for(int j = plotSize - numOfPoints; j < int(plotSize); ++j)
          points[j].ry() = *(k++);
        const ColorRGBA& color(i->color);
        painter.setPen(QColor(color.r, color.g, color.b));
        painter.drawPolyline(points + (plotSize - numOfPoints), numOfPoints);
      }
      unsigned int timeStamp = console.plots[i->layer].timeStamp;
      if(timeStamp > lastTimeStamp)
        lastTimeStamp = timeStamp;
      if(drawLegend)
      {
        int width = fontMetrics.size(Qt::TextSingleLine, i->description.c_str()).width();
        if(width > legendWidth)
          legendWidth = width;
      }
    }
    if(antialiasing)
      painter.setRenderHints(QPainter::Antialiasing| QPainter::HighQualityAntialiasing, false);


    // draw legend  
    if(drawLegend && plotList.size() > 0)
    {
      QRect legendRect(plotRect.left() + space, plotRect.top() + space, legendWidth + space * 3 + 10, space + plotList.size() * (textHeight + space));
      QRect rect(legendRect.left() + space + 10 + space, legendRect.top() + space, legendRect.width() - (space + 10 + space), textHeight);
      QLine line(legendRect.left() + space, legendRect.top() + space + textHeight / 2 + 1, legendRect.left() + space + 10, legendRect.top() + space + textHeight / 2 + 1);
      painter.setTransform(QTransform());
      painter.setPen(widget->blackPen);
      painter.setBrush(QBrush(QColor(0xff, 0xff, 0xff, 0x99)));
      painter.drawRect(legendRect);
      for(std::list<RobotConsole::Layer>::const_iterator i = plotList.begin(), end = plotList.end(); 
          i != end; ++i)
      {
        painter.setPen(widget->blackPen);
        painter.drawText(rect, Qt::AlignLeft, i->description.c_str());
        const ColorRGBA& color(i->color);
        painter.setPen(QColor(color.r, color.g, color.b));
        painter.drawLine(line);
        rect.moveTop(rect.top() + textHeight + space);
        line.translate(0, textHeight + space);
      }
    }
  }
}

void PlotView::determineMinMaxValue()
{
  SYNC_WITH(console);
  bool started = false;
  const std::list<RobotConsole::Layer>& plotList(console.plotViews[name]);
  for(std::list<RobotConsole::Layer>::const_iterator i = plotList.begin(), end = plotList.end(); i != end; ++i)
  {
    const std::list<double>& list = console.plots[i->layer].points;
    int numOfPoints = std::min<>(list.size(), plotSize);
    if(numOfPoints > 1)
    {
      std::list<double>::const_iterator k = list.begin();
      for(int j = plotSize - numOfPoints; j < int(plotSize); ++j)
      {
        const double& value(*(k++));
        if(started)
        {
          if(value < minValue)
            minValue = value;
          else if(value > maxValue)
            maxValue = value;
        }
        else
        {
          minValue = value;
          maxValue = minValue + 0.00001;
          started = true;
        }
      }
    }
  }

  if(started)
  {
    int precision = int(ceil(log10(maxValue - minValue))) - 1;
    double rounder = pow(10., precision);
    minValue = floor(minValue / rounder) * rounder;
    maxValue = ceil(maxValue / rounder) * rounder;
    valueLength = maxValue - minValue;
  }
}

bool PlotView::needsRepaint() const
{
  SYNC_WITH(console);
  for(std::list<RobotConsole::Layer>::const_iterator i = console.plotViews[name].begin(), end = console.plotViews[name].end(); 
      i != end; ++i)
    if(console.plots[i->layer].timeStamp > lastTimeStamp)
      return true;
  return false;
}

std::string PlotView::getConfiguration(QWidget& widget) const
{
  std::stringstream ss;
  bool dontDrawUnits = !drawUnits, dontDrawLegend = !drawLegend;
  ss << dontDrawUnits << ' ' << dontDrawLegend << ' '<< antialiasing;
  return ss.str().c_str();
}

void PlotView::setConfiguration(QWidget& widget, const std::string& conf)
{
  std::stringstream ss(conf);
  bool dontDrawUnits, dontDrawLegend;
  ss >> dontDrawUnits >> dontDrawLegend >> antialiasing;
  drawUnits = !dontDrawUnits;
  drawLegend = !dontDrawLegend;
}
