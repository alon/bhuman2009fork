/**
* @file FieldModel.cpp
* The file implements a class representing the tables required for mapping
* observations of a certain class to the closest positions in the field model.
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
*/

#include "FieldModel.h"
#include "Tools/Debugging/Modify.h"
#include "Tools/Streams/InStreams.h"

FieldModel::FieldModel() :
fieldDimensions(0),
maxCrossingLength(0)
{
  InBinaryFile stream(Global::getSettings().expandLocationFilename("fieldModel.tab"));
  if(stream.exists())
  {
    for(int i = 0; i < 2; ++i)
      for(int j = 0; j < 2; ++j)
        linePointsTables[i][j].read(stream);
    for(int i = 0; i < 4; ++i)
      tCornersTables[i].read(stream);
    for(int i = 0; i < 4; ++i)
      lCornersTables[i].read(stream);
  }
}

void FieldModel::init(const FieldDimensions& fieldDimensions, const int& maxCrossingLength)
{
  this->fieldDimensions = &fieldDimensions;
  this->maxCrossingLength = &maxCrossingLength;
}

void FieldModel::create()
{
  OutBinaryFile stream(Global::getSettings().expandLocationFilename("fieldModel.tab"));
  for(int i = 0; i < 2; ++i)
    for(int j = 0; j < 2; ++j)
    {
      linePointsTables[i][j].create(fieldDimensions->fieldLines, (1 - j) * pi_2, 2, i * *maxCrossingLength);
      linePointsTables[i][j].write(stream);
    }
  for(int i = 0; i < 4; ++i)
  {
    tCornersTables[i].create(fieldDimensions->corners[i + FieldDimensions::tCorner0]);
    tCornersTables[i].write(stream);
  }
  for(int i = 0; i < 4; ++i)
  {
    lCornersTables[i].create(fieldDimensions->corners[i + FieldDimensions::lCorner0]);
    lCornersTables[i].write(stream);
  }
}

const Vector2<int> FieldModel::getClosestLinePoint(const Vector2<int>& point, const Vector2<int>& neighbor, int length2) const
{
  Vector2<int> diff = neighbor - point;
  return linePointsTables[length2 > *maxCrossingLength * *maxCrossingLength ? 1 : 0]
                         [std::abs(diff.y) > std::abs(diff.x) ? 1 : 0].getClosestPoint(point);
}

const Vector2<int> FieldModel::getClosestCorner(const Vector2<int>& point, LinePercept::Intersection::IntersectionType type, double dir) const
{
  if(type == LinePercept::Intersection::X)
    return fieldDimensions->corners[FieldDimensions::xCorner].getClosest(point);
  else
  {
    int index = (int) floor(dir / pi_2 + 0.5) & 3;
    return (type == LinePercept::Intersection::T ? tCornersTables[index] : lCornersTables[index]).getClosestPoint(point);
  }
}

void FieldModel::draw()
{
  DECLARE_DEBUG_DRAWING("module:SelfLocator:fieldModel", "drawingOnField"); // Draws the closest points tables
  int index = 0;
  MODIFY("module:SelfLocator:fieldModel", index);
  COMPLEX_DRAWING("module:SelfLocator:fieldModel", 
    if(index < 4)
      linePointsTables[index >> 1][index & 1].draw(); 
    else if(index < 8)
      tCornersTables[index - 4].draw();
    else
      lCornersTables[index - 8].draw();
  );
}

template<int xSize,int ySize, int cellSize> 
void FieldModel::ClosestPointsTable<xSize, ySize, cellSize>::create(
  const FieldDimensions::LinesTable& table,
  double orientation,
  int numberOfOrientations,
  double minLength)
{
  for(int y = 0; y < ySize; ++y)
    for(int x = 0; x < xSize; ++x)
    {
      Vector2<double> px((x + 0.5 - xSize / 2) * cellSize, (y + 0.5 - ySize / 2) * cellSize);
      Vector2<double> p;
      table.getClosestPoint(p, Pose2D(orientation, px), numberOfOrientations, minLength);
      points[y][x][0] = static_cast<short>(p.x);
      points[y][x][1] = static_cast<short>(p.y);
    }
}

template<int xSize,int ySize, int cellSize> 
void FieldModel::ClosestPointsTable<xSize, ySize, cellSize>::create(
  const FieldDimensions::CornersTable& table)
{
  for(int y = 0; y < ySize; ++y)
    for(int x = 0; x < xSize; ++x)
    {
      Vector2<int> px((x - xSize / 2) * cellSize + cellSize / 2, (y - ySize / 2) * cellSize + cellSize / 2),
                   p(table.getClosest(px));
      points[y][x][0] = static_cast<short>(p.x);
      points[y][x][1] = static_cast<short>(p.y);
    }
}
