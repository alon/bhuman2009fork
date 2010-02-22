/** 
* @file PaintMethods.cpp
* Implementation of class PaintMethods.
*
* @author <A href="mailto:juengel@informatik.hu-berlin.de">Matthias Jüngel</A>
* @author Colin Graf
*/

#include <QPainter>

#include "PaintMethods.h"
#include "DebugDrawing.h"

void PaintMethods::paintDebugDrawing(QPainter& painter, const DebugDrawing& debugDrawing, const QTransform& baseTrans)
{
  static QBrush brush(Qt::SolidPattern);
  static QBrush noBrush(Qt::NoBrush);
  static QPen pen;
  static QPen noPen(Qt::NoPen);

  for(const DebugDrawing::Element* e = debugDrawing.getFirst(); e; e = debugDrawing.getNext(e))
    switch(e->type)
    {
    case DebugDrawing::Element::POLYGON:
      {
        const DebugDrawing::Polygon& element = *(const DebugDrawing::Polygon*) e;

        // select brush
        if(element.fillStyle == Drawings::bs_solid)
        {
          brush.setColor(QColor(element.fillColor.r, element.fillColor.g, element.fillColor.b, element.fillColor.a));
          painter.setBrush(brush);
        }
        else
          painter.setBrush(noBrush);

        // select pen
        if(element.penStyle != Drawings::ps_null)
        {
          pen.setColor(QColor(element.penColor.r, element.penColor.g, element.penColor.b, element.penColor.a));
          // A line width of zero indicates a cosmetic pen. This means that the pen width is always drawn one pixel wide, independent of the transformation set on the painter.
          pen.setWidth(element.width == 1 ? 0 : element.width);
          switch (element.penStyle)
          {
          case Drawings::ps_dash:
            pen.setStyle(Qt::DashLine);
            break;
          case Drawings::ps_dot:
            pen.setStyle(Qt::DotLine);
            break;
          case Drawings::ps_solid:
          default:
            pen.setStyle(Qt::SolidLine);
          }
          painter.setPen(pen);
        }
        else
          painter.setPen(noPen);

        // copy vector2 to QPoints
        static QPoint points[16];
        for(int n = element.nCount - 1; n >= 0 ; --n)
          points[n] = QPoint(element.points[n].x, element.points[n].y);

        painter.drawPolygon(points, element.nCount);
        break;
      }
    case DebugDrawing::Element::GRID_RGBA:
      {
        const DebugDrawing::GridRGBA& element = *(const DebugDrawing::GridRGBA*) e;
        const int totalWidth(element.cellsX * element.cellSize);
        const int totalHeight(element.cellsY * element.cellSize);
        for(int y=0; y<element.cellsY; ++y)
        {
          for(int x=0; x<element.cellsX; ++x)
          {
            int startX(x*element.cellSize - totalWidth/2);
            int startY(y*element.cellSize - totalHeight/2);
            int c(y*element.cellsX + x);
            brush.setColor(QColor(element.cells[c].r, element.cells[c].g, 
                                  element.cells[c].b, element.cells[c].a));
            pen.setColor(QColor(element.cells[c].r, element.cells[c].g, 
                                element.cells[c].b, element.cells[c].a));
            pen.setWidth(1);
            painter.setBrush(brush);
            painter.setPen(pen);
            painter.drawRect(startX, startY, element.cellSize-1, element.cellSize-1);
          }
        }
        break;
      }
    case DebugDrawing::Element::GRID_MONO:
      {
        const DebugDrawing::GridMono& element = *(const DebugDrawing::GridMono*) e;
        const int totalWidth(element.cellsX * element.cellSize);
        const int totalHeight(element.cellsY * element.cellSize);
        for(int y=0; y<element.cellsY; ++y)
        {
          for(int x=0; x<element.cellsX; ++x)
          {
            int startX(x*element.cellSize - totalWidth/2);
            int startY(y*element.cellSize - totalHeight/2);
            int c(y*element.cellsX + x);
            ColorRGBA col(element.baseColor * (1.0 - (static_cast<double>(element.cells[c]) / 255.0)));
            brush.setColor(QColor(col.r, col.g, col.b, 255));
            pen.setColor(QColor(col.r, col.g, col.b, 255));
            pen.setWidth(1);
            painter.setBrush(brush);
            painter.setPen(pen);
            painter.drawRect(startX, startY, element.cellSize-1, element.cellSize-1);
          }
        }
        break;
      }
    case DebugDrawing::Element::ELLIPSE:
      {
        const DebugDrawing::Ellipse& element = *(const DebugDrawing::Ellipse*) e;

        // select brush
        if(element.fillStyle == Drawings::bs_solid)
        {
          brush.setColor(QColor(element.fillColor.r, element.fillColor.g, element.fillColor.b, element.fillColor.a));
          painter.setBrush(brush);
        }
        else
          painter.setBrush(noBrush);

        // select pen
        if(element.penStyle != Drawings::ps_null)
        {
          pen.setColor(QColor(element.penColor.r, element.penColor.g, element.penColor.b, element.penColor.a));
          // A line width of zero indicates a cosmetic pen. This means that the pen width is always drawn one pixel wide, independent of the transformation set on the painter.
          pen.setWidth(element.width == 1 ? 0 : element.width);
          switch (element.penStyle)
          {
          case Drawings::ps_dash:
            pen.setStyle(Qt::DashLine);
            break;
          case Drawings::ps_dot:
            pen.setStyle(Qt::DotLine);
            break;
          case Drawings::ps_solid:
          default:
            pen.setStyle(Qt::SolidLine);
          }
          painter.setPen(pen);
        }
        else
          painter.setPen(noPen);

        if(element.rotation != 0.0)
        {
          QTransform trans(painter.transform());
          QTransform transBack(painter.transform());
          trans.translate(qreal(element.x), qreal(element.y));
          trans.rotateRadians(qreal(element.rotation));
          painter.setTransform(trans);
          painter.drawEllipse(-element.radiusX, -element.radiusY, 2*element.radiusX, 2*element.radiusY);
          painter.setTransform(transBack);
        }
        else 
        {
          painter.drawEllipse(element.x-element.radiusX, element.y-element.radiusY, 2*element.radiusX, 2*element.radiusY);
        }
        break;
      }
    case DebugDrawing::Element::LINE:
      {
        const DebugDrawing::Line& element = *(const DebugDrawing::Line*) e;

        if(element.penStyle != Drawings::ps_null)
        {
          pen.setColor(QColor(element.penColor.r, element.penColor.g, element.penColor.b, element.penColor.a));
          // A line width of zero indicates a cosmetic pen. This means that the pen width is always drawn one pixel wide, independent of the transformation set on the painter.
          pen.setWidth(element.width == 1 ? 0 : element.width);
          switch (element.penStyle)
          {
          case Drawings::ps_dash:
            pen.setStyle(Qt::DashLine);
            break;
          case Drawings::ps_dot:
            pen.setStyle(Qt::DotLine);
            break;
          case Drawings::ps_solid:
          default:
            pen.setStyle(Qt::SolidLine);
          }
          painter.setPen(pen);

          painter.drawLine(element.xStart, element.yStart, element.xEnd, element.yEnd);
        }
        break;
      }
    case DebugDrawing::Element::ORIGIN:
      {
        const DebugDrawing::Origin& element = *(const DebugDrawing::Origin*) e;
        QTransform trans(baseTrans);
        trans.translate(qreal(element.x), qreal(element.y));
        trans.rotateRadians(qreal(element.angle));
        painter.setTransform(trans);
        break;
      }

    case DebugDrawing::Element::TEXT:
      {
        const DebugDrawing::Text& element = *(const DebugDrawing::Text*) e;
        
        pen.setColor(QColor(element.penColor.r, element.penColor.g, element.penColor.b, element.penColor.a));
        painter.setPen(pen);

        QTransform trans(painter.transform());
        const QPoint& pos(trans.map(QPoint(element.x, element.y)));
        painter.resetTransform();
        painter.drawText(pos, (const char*) (&element + 1));
        painter.setTransform(trans);

        break;
      }
    default:
      break;
    }
}
