/**
* @file BodyContour.h
* The file implements a class that represents the contour of the robot's body in the image.
* The contour can be used to exclude the robot's body from image processing.
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
*/

#include "BodyContour.h"
#include "Tools/Debugging/DebugDrawings.h"
#include "Tools/Debugging/DebugDrawings3D.h"

void BodyContour::clipBottom(int x, int& y) const
{
  int yIntersection;
  for(std::vector<Line>::const_iterator i = lines.begin(); i != lines.end(); ++i)
    if(i->yAt(x, yIntersection) && yIntersection < y)
      y = yIntersection;
}

void BodyContour::clipLeft(int& x, int y) const
{
  int xIntersection;
  for(std::vector<Line>::const_iterator i = lines.begin(); i != lines.end(); ++i)
    if(i->p1.y > i->p2.y)
    {
      if(i->xAt(y, xIntersection) && xIntersection > x)
        x = xIntersection;
      else if(i->p2.y <= y && i->p2.x > x) // below a segment, clip anyway
        x = i->p2.x;
    }
}

void BodyContour::clipRight(int& x, int y) const
{
  int xIntersection;
  for(std::vector<Line>::const_iterator i = lines.begin(); i != lines.end(); ++i)
    if(i->p1.y < i->p2.y)
    {
      if(i->xAt(y, xIntersection) && xIntersection < x)
        x = xIntersection;
      else if(i->p1.y <= y && i->p1.x < x) // below a segment, clip anyway
        x = i->p1.x;
    }
}

void BodyContour::draw()
{
  DECLARE_DEBUG_DRAWING("representation:BodyContour", "drawingOnImage");
  COMPLEX_DRAWING("representation:BodyContour",
    for(std::vector<Line>::const_iterator i = lines.begin(); i != lines.end(); ++i)
      LINE("representation:BodyContour", i->p1.x, i->p1.y, i->p2.x, i->p2.y, 1, 
           Drawings::ps_solid, ColorRGBA(255,0,0));
  );
}
