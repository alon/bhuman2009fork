/** 
 * @file Visualization/DebugDrawing.cpp
 * Implementation of class DebugDrawing.
 *
 * @author <A href="mailto:juengel@informatik.hu-berlin.de">Matthias Jüngel</A>
 * @author <a href="mailto:Tim.Laue@dfki.de">Tim Laue</a>
 */

#include "DebugDrawing.h"
#include "Platform/GTAssert.h"
#include "Platform/SystemCall.h"


DebugDrawing::DebugDrawing()
{
  timeStamp = SystemCall::getCurrentSystemTime();
  processIdentifier = 0;
  usedSize = reservedSize = 0;
  type = 0;
  firstTip = -1;
  elements = 0;
}

const DebugDrawing& DebugDrawing::operator=(const DebugDrawing& other)
{
  reset();
  typeOfDrawing = other.typeOfDrawing;
  timeStamp = other.timeStamp;
  processIdentifier = other.processIdentifier;
  type = other.type;
  *this += other;
  return *this;
}

const DebugDrawing& DebugDrawing::operator+=(const DebugDrawing& other)
{
  int offset = usedSize;
  write(other.elements, other.usedSize);
  int i = other.firstTip;
  while(i != -1)
  {
    int address = offset + i;
    Tip& t = (Tip&) elements[address];
    i = t.next;
    t.next = firstTip;
    firstTip = address;
  }
  return *this;
}

DebugDrawing::DebugDrawing(const DebugDrawing& other)
{
  usedSize = reservedSize = 0;
  elements = 0;
  *this = other;
}

DebugDrawing::DebugDrawing(const DebugDrawing* pDebugDrawing)
{
  usedSize = reservedSize = 0;
  elements = 0;
  *this = *pDebugDrawing;
}

DebugDrawing::~DebugDrawing()
{
  if(elements)
    free(elements);
}

void DebugDrawing::reset()
{
  timeStamp = SystemCall::getCurrentSystemTime();
  processIdentifier = 0;
  firstTip = -1;
  usedSize = 0;
}

const char* DebugDrawing::getTip(int& x, int& y) const
{
  int i = firstTip;
  while(i != -1)
  {
    const Tip& t = (const Tip&) elements[i];
    long long xDiff(t.x - x);
    long long yDiff(t.y - y);
    if(xDiff * xDiff + yDiff * yDiff <= t.radius * t.radius)
    {
      x = t.x;
      y = t.y;
      return (const char*) (&t + 1);
    }
    i = t.next;
  }
  return 0;
}

void DebugDrawing::arrow(Vector2<double> start, Vector2<double> end, 
                         Drawings::PenStyle penStyle, int width, ColorRGBA color)
{
  Vector2<double> startToEnd((end.x - start.x)/4, (end.y - start.y)/4);
  Vector2<double> perpendicular(startToEnd.y, -1*startToEnd.x);
	// start to endpoint
  line((int)start.x, (int)start.y, (int)(end.x), (int)(end.y), penStyle, width, color);
	// endpoint to left and right
  line((int)(end.x), (int)(end.y), (int)(end.x - startToEnd.x + perpendicular.x), (int)(end.y - startToEnd.y + perpendicular.y), penStyle, width, color);
  line((int)(end.x), (int)(end.y), (int)(end.x - startToEnd.x - perpendicular.x), (int)(end.y - startToEnd.y - perpendicular.y), penStyle, width, color);
}

void DebugDrawing::text
(
 const char* text,
 int x,
 int y,
 int fontSize,
 ColorRGBA color
)
{
  Text element;
  element.x = x;
  element.y = y;
  element.fontSize = fontSize;
  element.penColor = color;
  element.size = strlen(text) + 1;
  write(&element, sizeof(element)); 
  write(text, element.size);
}

void DebugDrawing::tip
(
 const char* text,
 int x,
 int y,
 int radius
)
{
  Tip element;
  element.x = x;
  element.y = y;
  element.radius = radius;
  element.size = strlen(text) + 1;
  element.next = firstTip;
  firstTip = usedSize;
  write(&element, sizeof(element)); 
  write(text, element.size);
}

void DebugDrawing::line
(
 int xStart, 
 int yStart, 
 int xEnd,
 int yEnd,
 Drawings::PenStyle penStyle,
 int width,
 ColorRGBA penColor
 )
{
  Line element;
  element.xStart = xStart;
  element.yStart = yStart;
  element.xEnd = xEnd;
  element.yEnd = yEnd;
  element.penStyle = penStyle;
  element.width = width;
  element.penColor = penColor;
  write(&element, sizeof(element));
}

void DebugDrawing::line
(
 int xStart, 
 int yStart, 
 int xEnd,
 int yEnd
 )
{
  line(xStart, yStart, xEnd, yEnd, Drawings::ps_solid, 1, ColorRGBA(0,0,0));
}

void DebugDrawing::polygon
(
 const Vector2<int>* points,
 int nCount,
 int width,
 Drawings::PenStyle penStyle,
 ColorRGBA penColor,
 Drawings::FillStyle fillStyle,
 ColorRGBA fillColor
 )
{
  ASSERT(nCount <= MAX_NUMBER_OF_POINTS);
  Polygon element;
  memcpy(element.points, points, sizeof(element.points[0]) * nCount);
  element.nCount = nCount;
  element.width = width;
  element.penStyle = penStyle;
  element.penColor = penColor;
  element.fillStyle = fillStyle;
  element.fillColor = fillColor;
  write(&element, sizeof(element));
}

void DebugDrawing::gridRGBA(int cellSize, int cellsX, int cellsY, ColorRGBA* cells)
{
  ASSERT(cellsX * cellsY <= MAX_NUMBER_OF_GRID_CELLS);
  GridRGBA element;
  memcpy(element.cells, cells, sizeof(ColorRGBA) * cellsX * cellsY);
  element.cellSize = cellSize;
  element.cellsX = cellsX;
  element.cellsY = cellsY;
  write(&element, sizeof(element));
}

void DebugDrawing::gridMono(int cellSize, int cellsX, int cellsY, 
                            const ColorRGBA& baseColor, unsigned char* cells)
{
  ASSERT(cellsX * cellsY <= MAX_NUMBER_OF_GRID_CELLS);
  GridMono element;
  memcpy(element.cells, cells, sizeof(unsigned char) * cellsX * cellsY);
  element.cellSize = cellSize;
  element.cellsX = cellsX;
  element.cellsY = cellsY;
  element.baseColor = baseColor;
  write(&element, sizeof(element));
}

void DebugDrawing::dot(int x, int y, ColorRGBA penColor, ColorRGBA fillColor)
{
  Vector2<int> points[4];
  points[0].x = x - 1;
  points[0].y = y - 1;
  points[1].x = x + 1;
  points[1].y = y - 1;
  points[2].x = x + 1;
  points[2].y = y + 1;
  points[3].x = x - 1;
  points[3].y = y + 1;
  polygon(
    points,
    4,
    0,
    Drawings::ps_solid,
    penColor,
    Drawings::bs_solid,
    fillColor);
}

void DebugDrawing::largeDot(int x, int y, ColorRGBA penColor, ColorRGBA fillColor)
{
  Vector2<int> points[4];
  points[0].x = x - 25;
  points[0].y = y - 25;
  points[1].x = x + 25;
  points[1].y = y - 25;
  points[2].x = x + 25;
  points[2].y = y + 25;
  points[3].x = x - 25;
  points[3].y = y + 25;
  polygon(
    points,
    4,
    3,
    Drawings::ps_solid,
    penColor,
    Drawings::bs_solid,
    fillColor);
}

void DebugDrawing::midDot(int x, int y, ColorRGBA penColor, ColorRGBA fillColor)
{
  Vector2<int> points[4];
  points[0].x = x - 2;
  points[0].y = y - 2;
  points[1].x = x + 2;
  points[1].y = y - 2;
  points[2].x = x + 2;
  points[2].y = y + 2;
  points[3].x = x - 2;
  points[3].y = y + 2;
  polygon(
    points,
    4,
    1,
    Drawings::ps_solid,
    penColor,
    Drawings::bs_solid,
    fillColor);
}

void DebugDrawing::rectangle
(
 int left, 
 int right, 
 int top,
 int bottom,
 int width,
 Drawings::PenStyle penStyle,
 ColorRGBA penColor,
 Drawings::FillStyle fillStyle,
 ColorRGBA fillColor
 )
{
  Vector2<int> points[4];
  points[0].x = left;
  points[0].y = top;
  points[1].x = right;
  points[1].y = top;
  points[2].x = right;
  points[2].y = bottom;
  points[3].x = left;
  points[3].y = bottom;
  polygon(
    points,
    4,
    width,
    penStyle,
    penColor,
    fillStyle,
    fillColor);
}

void DebugDrawing::origin(int x, int y, float angle)
{
  Origin element;
  element.x = x;
  element.y = y;
  element.angle = angle;
  write(&element, sizeof(element));
}

bool DebugDrawing::addShapeFromQueue(InMessage& message, Drawings::ShapeType shapeType, char identifier)
{
  type = identifier;

  switch ((Drawings::ShapeType)shapeType)
  {
  case Drawings::circle:
    {
      Ellipse newCircle;
      char penWidth, penStyle, fillStyle;
      message.bin >> newCircle.x;
      message.bin >> newCircle.y;
      message.bin >> newCircle.radiusX;
      message.bin >> penWidth;
      message.bin >> penStyle;
      message.bin >> newCircle.penColor;
      message.bin >> fillStyle;
      message.bin >> newCircle.fillColor;
      newCircle.width = penWidth;
      newCircle.penStyle = (Drawings::PenStyle)penStyle;
      newCircle.fillStyle = (Drawings::FillStyle)fillStyle;
      newCircle.radiusY = newCircle.radiusX;
      newCircle.rotation = 0.0;
      write(&newCircle, sizeof(newCircle));
    }
    break;
  case Drawings::ellipse:
    {
      Ellipse newEllipse;
      char penWidth, penStyle, fillStyle;
      ColorRGBA penColor, fillColor;
      message.bin >> newEllipse.x;
      message.bin >> newEllipse.y;
      message.bin >> newEllipse.radiusX;
      message.bin >> newEllipse.radiusY;
      message.bin >> newEllipse.rotation;
      message.bin >> penWidth;
      message.bin >> penStyle;
      message.bin >> newEllipse.penColor;
      message.bin >> fillStyle;
      message.bin >> newEllipse.fillColor;
      newEllipse.width = penWidth;
      newEllipse.penStyle = (Drawings::PenStyle)penStyle;
      newEllipse.fillStyle = (Drawings::FillStyle)fillStyle;
      write(&newEllipse, sizeof(newEllipse));
    }
    break;
  case Drawings::polygon:
    {
      int numberOfPoints;
      std::string buffer;
      message.bin >> numberOfPoints >> buffer;
      InTextMemory stream(buffer.c_str(), buffer.size());
      Vector2<int>* points = new Vector2<int>[numberOfPoints];
      for(int i = 0; i < numberOfPoints; ++i)
        stream >> points[i]; 
      char penWidth, penStyle, fillStyle;
      ColorRGBA fillColor, penColor;
      message.bin >> penWidth;
      message.bin >> penStyle;
      message.bin >> penColor;
      message.bin >> fillStyle;
      message.bin >> fillColor;
      this->polygon(points, numberOfPoints, penWidth, 
        (Drawings::PenStyle) penStyle, penColor, 
        (Drawings::FillStyle) fillStyle, fillColor);
      delete [] points;
    }
    break;
  case Drawings::gridRGBA:
    {
      int cellSize, cellsX, cellsY;
      std::string buffer;
      message.bin >> cellSize >> cellsX >> cellsY >> buffer;
      const int numberOfCells(cellsX*cellsY);
      InTextMemory stream(buffer.c_str(), buffer.size());
      ColorRGBA* cells = new ColorRGBA[numberOfCells];
      for(int i = 0; i < numberOfCells; ++i)
        stream >> cells[i]; 
      this->gridRGBA(cellSize, cellsX, cellsY, cells);
      delete [] cells;
    }
    break;
  case Drawings::gridMono:
    {
      int cellSize, cellsX, cellsY;
      ColorRGBA baseColor;
      std::string buffer;
      message.bin >> cellSize >> cellsX >> cellsY >> baseColor >> buffer;
      const int numberOfCells(cellsX*cellsY);
      InTextMemory stream(buffer.c_str(), buffer.size());
      unsigned char* cells = new unsigned char[numberOfCells];
      for(int i = 0; i < numberOfCells; ++i)
        stream >> cells[i]; 
      this->gridMono(cellSize, cellsX, cellsY, baseColor, cells);
      delete [] cells;
    }
    break;
  case Drawings::line:
    {
      int x1, y1, x2, y2;
      char penWidth, penStyle;
      ColorRGBA penColor;
      message.bin >> x1;
      message.bin >> y1;
      message.bin >> x2;
      message.bin >> y2;
      message.bin >> penWidth;
      message.bin >> penStyle;
      message.bin >> penColor;
      this->line(x1, y1, x2, y2, (Drawings::PenStyle)penStyle, penWidth, penColor);
    }
    break;
  case Drawings::origin:
    {
      int x, y;
      float angle;
      message.bin >> x;
      message.bin >> y;
      message.bin >> angle;
      this->origin(x, y, angle);
    }
    break;
  case Drawings::arrow:
    {
      int x1, y1, x2, y2;
      char penWidth, penStyle;
      ColorRGBA penColor;
      message.bin >> x1;
      message.bin >> y1;
      message.bin >> x2;
      message.bin >> y2;
      message.bin >> penWidth;
      message.bin >> penStyle;
      message.bin >> penColor;
      this->arrow(Vector2<double>(x1, y1), Vector2<double>(x2, y2), (Drawings::PenStyle)penStyle, penWidth, penColor);
    }
    break;
  case Drawings::dot:
    {
      int x, y;
      ColorRGBA penColor, fillColor;
      message.bin >> x;
      message.bin >> y;
      message.bin >> penColor;
      message.bin >> fillColor;
      this->dot(x, y, penColor, fillColor);
    }
    break;
  case Drawings::midDot:
    {
      int x, y;
      ColorRGBA penColor, fillColor;
      message.bin >> x;
      message.bin >> y;
      message.bin >> penColor;
      message.bin >> fillColor;
      this->midDot(x, y, penColor, fillColor);
    }
    break;
  case Drawings::largeDot:
    {
      int x, y;
      ColorRGBA penColor, fillColor;
      message.bin >> x;
      message.bin >> y;
      message.bin >> penColor;
      message.bin >> fillColor;
      this->largeDot(x, y, penColor, fillColor);
    }
    break;
  case Drawings::text:
    {
      int x, y;
      short fontSize;
      ColorRGBA color;
      message.bin >> x;
      message.bin >> y;
      message.bin >> fontSize;
      message.bin >> color;
      std::string text;
      message.bin >> text;
      this->text(text.c_str(), x, y, fontSize, color);
    }
    break;
  case Drawings::tip:
    {
      int x, y, radius;
      message.bin >> x;
      message.bin >> y;
      message.bin >> radius;
      std::string text;
      message.bin >> text;
      this->tip(text.c_str(), x, y, radius);
    }
    break;
  }
  return true;
}

void DebugDrawing::reserve(int size)
{
  if(usedSize + size > reservedSize)
  {
    if(!reservedSize)
      reservedSize = 1024;
    while(usedSize + size > reservedSize)
      reservedSize *= 2;
    elements = (char*) realloc(elements, reservedSize);
  }
}

void DebugDrawing::write(const void* data, int size)
{
  reserve(size);
  memcpy(elements + usedSize, data, size);
  usedSize += size;
}

const DebugDrawing::Element* DebugDrawing::getNext(const Element* element) const
{
  switch(element->type)
  {
  case Element::ELLIPSE:
    element = (const Element*) ((const Ellipse*) element + 1);
    break;
  case Element::LINE:
    element = (const Element*) ((const Line*) element + 1);
    break;
  case Element::POLYGON:
    element = (const Element*) ((const Polygon*) element + 1);
    break;
  case Element::GRID_RGBA:
    element = (const Element*) ((const GridRGBA*) element + 1);
    break;
  case Element::GRID_MONO:
    element = (const Element*) ((const GridMono*) element + 1);
    break;
  case Element::TEXT:
    element = (const Element*) ((const char*) element + (sizeof(Text) + ((const Text*) element)->size));
    break;
  case Element::TIP:
    element = (const Element*) ((const char*) element + (sizeof(Tip) + ((const Tip*) element)->size));
    break;
  case Element::ORIGIN:
    element = (const Element*) ((const Origin*) element + 1);
    break;
  default:
    ASSERT(false);
  }
  return (const char*) element - elements < usedSize ? element : 0;
}

In& operator>>(In& stream,DebugDrawing& debugDrawing)
{
  debugDrawing.reset();
  int size;
  stream >> debugDrawing.typeOfDrawing 
         >> debugDrawing.timeStamp 
         >> debugDrawing.firstTip 
         >> size;
  debugDrawing.reserve(size);
  stream.read(debugDrawing.elements, size);
  debugDrawing.usedSize = size;
  return stream;
}

Out& operator<<(Out& stream, const DebugDrawing& debugDrawing)
{
  stream << debugDrawing.typeOfDrawing
         << debugDrawing.timeStamp
         << debugDrawing.firstTip
         << debugDrawing.usedSize;
  stream.write(debugDrawing.elements, debugDrawing.usedSize);
  return stream;
}
