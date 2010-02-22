#include "Representations/Perception/RegionPercept.h"
#include "PointExplorer.h"
#include "Tools/Debugging/Modify.h"

ColorClasses::Color PointExplorer::getColor(int x, int y)
{
  ASSERT(x < theImage->cameraInfo.resolutionWidth && y < theImage->cameraInfo.resolutionHeight && x >= 0 && y >= 0);

  unsigned col(theImage->image[y][x].color); 
  return (ColorClasses::Color)theColorTable64->colorClasses[col >> 18 & 0x3f][col >> 10 & 0x3f][col >> 26 & 0x3f];
}

int PointExplorer::explorePoint(int x, int y, ColorClasses::Color col, int xMin, int yEnd, int yMin, int& run_end, int& explored_min_y, int& explored_max_y, bool force_detailed)
{
  int size = 0;
  if(col != ColorClasses::green || force_detailed)
  {
    run_end = runDown(x, y, col, yEnd, Drawings::ps_solid);
    explored_min_y = y;
    explored_max_y = run_end-1;

    size = (run_end - y) * parameters.gridStepSize;

    if(!(run_end - y >= parameters.minSegSize[col]))
      return -1;

    for(x -= parameters.exploreStepSize; x > xMin; x -= parameters.exploreStepSize)
    {
      if(getColor(x, explored_min_y) == col)
      {
        const int expl_run_end = runUp(x,explored_min_y, col, yMin, Drawings::ps_dot);
        if(expl_run_end < explored_min_y)
          explored_min_y = expl_run_end+1;
      }
      if(getColor(x, explored_max_y) == col)
      {
        const int expl_run_end = runDown(x, explored_max_y, col, yEnd, Drawings::ps_dot);
        if(expl_run_end > explored_max_y)
          explored_max_y = expl_run_end-1;
      }
    }
  }
  else
  {
    run_end = runDown(x, y, col, yEnd, Drawings::ps_solid);
    size = (run_end - y) * parameters.gridStepSize;
    explored_min_y = y;
    explored_max_y = run_end;
  }
  return size;
}

int PointExplorer::runDown(int x, int y, ColorClasses::Color col, int yEnd, Drawings::PenStyle draw)
{
  const int yStart = y;
  int tmp;

  for(y+=parameters.skipOffset; y < yEnd; y+=parameters.skipOffset)
  {
    if(getColor(x,y) != col)
    {
      tmp = y - parameters.skipOffset;
      for(--y; y > tmp; y--)
        if(getColor(x,y) == col)
          break;

      if(y == tmp)
      {
        y++;
        break;
      }
    }
  }
  if(y > yEnd)
    y = yEnd;

  COMPLEX_DRAWING("module:PointExplorer:runs",
        {LINE("module:PointExplorer:runs", x, yStart, x, y, 1, draw, getOnFieldDrawColor(col));}
    );
  return y;
}

int PointExplorer::findDown(int x, int y, ColorClasses::Color col, int yEnd, Drawings::PenStyle draw)
{
  const int yStart = y;
  int tmp;

  for(y+=parameters.skipOffset; y < yEnd; y+=parameters.skipOffset)
  {
    if(getColor(x,y) == col)
    {
      tmp = y - parameters.skipOffset;
      for(--y; y > tmp; y--)
        if(getColor(x,y) != col)
          break;

      if(y != tmp)
        ++y;
      break;
    }
  }
  if(y > yEnd)
    y = yEnd;

  COMPLEX_DRAWING("module:PointExplorer:runs",
        {LINE("module:PointExplorer:runs", x, yStart, x, y, 1, draw, getOnFieldDrawColor(col));}
    );
  return y;
}

int PointExplorer::findDown2(int x, int y, ColorClasses::Color col1, ColorClasses::Color col2, int yEnd, ColorClasses::Color& foundCol, Drawings::PenStyle draw)
{
  const int yStart = y;
  int tmp;
  ColorClasses::Color col = ColorClasses::none;

  for(y+=parameters.skipOffset; y < yEnd; y+=parameters.skipOffset)
  {
    col = getColor(x,y);
    if(col == col1 || col == col2)
    {
      tmp = y - parameters.skipOffset;
      for(--y; y > tmp; y--)
        if(getColor(x,y) != col)
          break;

      if(y != tmp)
        ++y;
      break;
    }
  }
  if(y > yEnd)
    y = yEnd;

  foundCol = col;
  COMPLEX_DRAWING("module:PointExplorer:runs",
        {LINE("module:PointExplorer:runs", x, yStart, x, y, 1, draw, getOnFieldDrawColor(col));}
    );
  return y;
}

int PointExplorer::runUp(int x, int y, ColorClasses::Color col, int yEnd, Drawings::PenStyle draw)
{
  const int yStart = y;
  int tmp;

  for(y-=parameters.skipOffset; y > yEnd; y-=parameters.skipOffset)
  {
    if(getColor(x,y) != col)
    {
      tmp = y + parameters.skipOffset;
      for(++y; y < tmp; y++)
        if(getColor(x,y) == col)
          break;

      if(y == tmp)
      {
        y--;
        break;
      }
    }
  }
  if(y < yEnd)
    y = yEnd;

  COMPLEX_DRAWING("module:PointExplorer:runs",
      {LINE("module:PointExplorer:runs", x, yStart, x, y, 1, draw, getOnFieldDrawColor(col));}
    );
  return y;
}

int PointExplorer::runRight(int x, int y, ColorClasses::Color col, int xMax, Drawings::PenStyle draw)
{
  const int xStart = x;
  int tmp;

  for(x += parameters.skipOffset; x < xMax; x += parameters.skipOffset)
  {
    if(getColor(x,y) != col)
    {
      tmp = x - parameters.skipOffset;
      for(--x; x > tmp; x--)
        if(getColor(x,y) == col)
          break;

      if(x == tmp)
      {
        x++;
        break;
      }
    }
  }
  if(x > xMax)
    x = xMax;

  COMPLEX_DRAWING("module:PointExplorer:runs",
      {LINE("module:PointExplorer:runs", xStart, y, x, y, 1, draw, getOnFieldDrawColor(col));}
    );
  return x;
}

void PointExplorer::initFrame(const Image *image, const ColorTable64* colorTable, int exploreStepSize, int gridStepSize, int skipOffset, int* minSegLength)
{
  theImage = image;
  theColorTable64 = colorTable;
  parameters.exploreStepSize = exploreStepSize;
  parameters.gridStepSize = gridStepSize;
  parameters.skipOffset = skipOffset;
  parameters.minSegSize = minSegLength;
  DECLARE_DEBUG_DRAWING("module:PointExplorer:runs","drawingOnImage");
}

