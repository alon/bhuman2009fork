/**
* @file LinePercept.h
* Implementation of a class that represents the fieldline percepts
* @author jeff
*/

#include "LinePercept.h"
#include "Tools/Debugging/DebugDrawings.h"
#include "Tools/Debugging/DebugDrawings3D.h"
#include "Tools/Math/Geometry.h"

void LinePercept::serialize(In* in, Out* out)
{
  std::vector<Line> lines;
  std::vector<LineSegment> singleSegs;
  if(out)
  {
    lines.reserve(this->lines.size());
    for(std::list<Line>::const_iterator i = this->lines.begin();i != this->lines.end(); ++i)
      lines.push_back(*i);
    singleSegs.reserve(this->singleSegs.size());
    for(std::list<LineSegment>::const_iterator i = this->singleSegs.begin();i != this->singleSegs.end(); ++i)
      singleSegs.push_back(*i);
  }
  STREAM_REGISTER_BEGIN();
  STREAM_VECTOR(lines);
  STREAM_VECTOR(intersections);
  STREAM_VECTOR(singleSegs);
  STREAM(circle);
  STREAM_REGISTER_FINISH();
  if(in)
  {
    this->lines.clear();
    for(std::vector<Line>::const_iterator i = lines.begin();i != lines.end(); ++i)
      this->lines.push_back(*i);
    this->singleSegs.clear();
    for(std::vector<LineSegment>::const_iterator i = singleSegs.begin();i != singleSegs.end(); ++i)
      this->singleSegs.push_back(*i);
  }
}

void LinePercept::LineSegment::serialize(In* in, Out* out)
{
  STREAM_REGISTER_BEGIN();
  STREAM(p1);
  STREAM(p2);
  STREAM(p1Img);
  STREAM(p2Img);
  STREAM(alpha);
  STREAM(d);
  STREAM_REGISTER_FINISH();
}
 
void LinePercept::Line::serialize(In* in, Out* out)
{
  STREAM_REGISTER_BEGIN();
  STREAM(first);
  STREAM(last);
  STREAM_VECTOR(segments);
  STREAM(alpha);
  STREAM(d);
  STREAM(midLine);
  STREAM_REGISTER_FINISH();
}

Vector2<int> LinePercept::Line::calculateClosestPointOnLine(const Vector2<int>& p) const
{
  const Vector2<double> p_double = Vector2<double>(p.x, p.y);
  const Vector2<double> normale = Vector2<double>(cos(alpha+pi), sin(alpha+pi)); 
  const Vector2<double> offset = normale * calculateDistToLine(p);
  return p + Vector2<int>((int)offset.x, (int)offset.y);
}

void LinePercept::CircleSpot::serialize(In* in, Out* out)
{
  STREAM_REGISTER_BEGIN();
  STREAM(pos);
  STREAM(found);
  STREAM_REGISTER_FINISH();
}

void LinePercept::Intersection::serialize(In* in, Out* out)
{
  STREAM_REGISTER_BEGIN();
  STREAM(pos);
  STREAM_ENUMASINT(type);
  STREAM(dir1);
  STREAM(dir2);
  STREAM_REGISTER_FINISH();
}

int LinePercept::getClosestLine(Vector2<int> point, Line& retLine) const
{
  std::list<LinePercept::Line>::const_iterator closestLine = lines.end();
  int minDist = -1;
  for(std::list<LinePercept::Line>::const_iterator l1 = lines.begin(); l1 != lines.end(); l1++)
  {
    const int dist = (int)fabs(l1->calculateDistToLine(point));
    if(dist < minDist || minDist == -1)
    {
      closestLine = l1;
      minDist = dist;
    }
  }

  if(minDist != -1)
    retLine = *closestLine;
  return minDist;
}

void LinePercept::clear()
{
  lines.clear();
  singleSegs.clear();
  intersections.clear();
  circle.found = false;
}

void LinePercept::drawOnField(const FieldDimensions& theFieldDimensions, int circleBiggerThanSpecified) const
{
  DECLARE_DEBUG_DRAWING("representation:LinePercept:Field", "drawingOnField");
  COMPLEX_DRAWING("representation:LinePercept:Field",
    for(std::list<Line>::const_iterator line = lines.begin(); line != lines.end(); line++)
    {
      const Drawings::PenStyle pen = line->midLine ? Drawings::ps_dash : Drawings::ps_solid;
      LINE("representation:LinePercept:Field", line->first.x, line->first.y, line->last.x, line->last.y, 15, pen, ColorClasses::red);
      ARROW("representation:LinePercept:Field", line->first.x, line->first.y, line->first.x + cos(line->alpha-pi_2)*100, line->first.y + sin(line->alpha-pi_2) * 100, 15, pen, ColorClasses::robotBlue);
      CROSS("representation:LinePercept:Field", line->first.x, line->first.y, 10, 5, pen, ColorClasses::red);
    }
    for(std::vector<Intersection>::const_iterator inter = intersections.begin(); inter != intersections.end(); inter++)
    {
       switch(inter->type)
       {
       case Intersection::X:
         LINE("representation:LinePercept:Field", inter->pos.x - inter->dir1.x * 100, inter->pos.y - inter->dir1.y * 100,
                                                          inter->pos.x + inter->dir1.x * 100, inter->pos.y + inter->dir1.y * 100,
                                                          15, Drawings::ps_solid, ColorClasses::blue);
         LINE("representation:LinePercept:Field", inter->pos.x - inter->dir2.x * 100, inter->pos.y - inter->dir2.y * 100,
                                                          inter->pos.x + inter->dir2.x * 100, inter->pos.y + inter->dir2.y * 100,
                                                          15, Drawings::ps_solid, ColorClasses::blue);
         break;
       case Intersection::T:
         LINE("representation:LinePercept:Field", inter->pos.x, inter->pos.y,
                                                          inter->pos.x + inter->dir1.x * 100, inter->pos.y + inter->dir1.y * 100,
                                                          15, Drawings::ps_solid, ColorClasses::blue);
         LINE("representation:LinePercept:Field", inter->pos.x - inter->dir2.x * 100, inter->pos.y - inter->dir2.y * 100,
                                                          inter->pos.x + inter->dir2.x * 100, inter->pos.y + inter->dir2.y * 100,
                                                          15, Drawings::ps_solid, ColorClasses::blue);
         break;
       case Intersection::L:
         LINE("representation:LinePercept:Field", inter->pos.x, inter->pos.y,
                                                          inter->pos.x + inter->dir1.x * 100, inter->pos.y + inter->dir1.y * 100,
                                                          15, Drawings::ps_solid, ColorClasses::blue);
         LINE("representation:LinePercept:Field", inter->pos.x , inter->pos.y,
                                                          inter->pos.x + inter->dir2.x * 100, inter->pos.y + inter->dir2.y * 100,
                                                          15, Drawings::ps_solid, ColorClasses::blue);
         break;
       }
    }
    if(circle.found)
    {
      CROSS("representation:LinePercept:Field", circle.pos.x, circle.pos.y,40, 40, Drawings::ps_solid, ColorClasses::robotBlue);
      CIRCLE("representation:LinePercept:Field", circle.pos.x, circle.pos.y, theFieldDimensions.centerCircleRadius+circleBiggerThanSpecified, 30, Drawings::ps_solid, ColorClasses::blue, Drawings::bs_null, ColorClasses::robotBlue);
    }
    for(std::list<LineSegment>::const_iterator seg = singleSegs.begin(); seg != singleSegs.end(); seg++)
      ARROW("representation:LinePercept:Field", seg->p1.x, seg->p1.y, seg->p2.x, seg->p2.y, 10, Drawings::ps_solid, ColorClasses::orange);
  );

}

void LinePercept::drawOnImage(const CameraMatrix& theCameraMatrix, const CameraInfo& theCameraInfo, const FieldDimensions& theFieldDimensions, int circleBiggerThanSpecified, const ImageCoordinateSystem& theImageCoordinateSystem) const
{
  DECLARE_DEBUG_DRAWING("representation:LinePercept:Image", "drawingOnImage");
  COMPLEX_DRAWING("representation:LinePercept:Image",
    for(std::list<Line>::const_iterator line = lines.begin(); line != lines.end(); line++)
    {
      Vector2<int> p1;
      Vector2<int> p2;
      if(Geometry::calculatePointInImage(line->first, theCameraMatrix, theCameraInfo, p1) &&
         Geometry::calculatePointInImage(line->last, theCameraMatrix, theCameraInfo, p2))
      {
        const Drawings::PenStyle pen = line->midLine ? Drawings::ps_dash : Drawings::ps_solid;
        Vector2<double> uncor1 = theImageCoordinateSystem.fromCorrectedApprox(p1);
        Vector2<double> uncor2 = theImageCoordinateSystem.fromCorrectedApprox(p2);
        LINE("representation:LinePercept:Image", uncor1.x, uncor1.y, uncor2.x, uncor2.y, 3, pen, ColorClasses::red);
      }
    }

    if(circle.found)
    {
      Vector2<int> p1;
      if(Geometry::calculatePointInImage(circle.pos, theCameraMatrix, theCameraInfo, p1))
      {
        CROSS("representation:LinePercept:Image", p1.x, p1.y, 5, 3, Drawings::ps_solid, ColorClasses::robotBlue);
      }
      const double stepSize = 0.4;
      for(double i = 0; i < pi*2; i += stepSize) 
      {
        Vector2<int> p1;
        Vector2<int> p2;
        if(Geometry::calculatePointInImage(circle.pos + Vector2<int>(theFieldDimensions.centerCircleRadius+circleBiggerThanSpecified, 0).rotate(i), theCameraMatrix, theCameraInfo, p1) &&
           Geometry::calculatePointInImage(circle.pos + Vector2<int>(theFieldDimensions.centerCircleRadius+circleBiggerThanSpecified, 0).rotate(i+stepSize), theCameraMatrix, theCameraInfo, p2))
        {
          Vector2<double> uncor1 = theImageCoordinateSystem.fromCorrectedApprox(p1);
          Vector2<double> uncor2 = theImageCoordinateSystem.fromCorrectedApprox(p2);
          LINE("representation:LinePercept:Image", uncor1.x, uncor1.y, uncor2.x, uncor2.y, 3, Drawings::ps_solid, ColorClasses::blue);
        }
      }

    }

    for(std::list<LineSegment>::const_iterator seg = singleSegs.begin(); seg != singleSegs.end(); seg++)
    {
      Vector2<int> p1;
      Vector2<int> p2;
      if(Geometry::calculatePointInImage(seg->p1, theCameraMatrix, theCameraInfo, p1) &&
         Geometry::calculatePointInImage(seg->p2, theCameraMatrix, theCameraInfo, p2))
      {
        Vector2<double> uncor1 = theImageCoordinateSystem.fromCorrectedApprox(p1);
        Vector2<double> uncor2 = theImageCoordinateSystem.fromCorrectedApprox(p2);
        LINE("representation:LinePercept:Image", uncor1.x, uncor1.y, uncor2.x, uncor2.y, 2, Drawings::ps_solid, ColorClasses::orange);
      }
    }

   for(std::vector<Intersection>::const_iterator inter = intersections.begin(); inter != intersections.end(); inter++)
   {
      Vector2<int> p1;
      Vector2<int> p2;
      Vector2<int> p3;
      Vector2<int> p4;
      const Vector2<int> dir1Int = Vector2<int>(int(inter->dir1.x*100), int(inter->dir1.y*100));
      const Vector2<int> dir2Int = Vector2<int>(int(inter->dir2.x*100), int(inter->dir2.y*100));
      switch(inter->type)
      {
      case Intersection::X:
        Geometry::calculatePointInImage(inter->pos - dir1Int, theCameraMatrix, theCameraInfo, p1);
        Geometry::calculatePointInImage(inter->pos + dir1Int, theCameraMatrix, theCameraInfo, p2);
        Geometry::calculatePointInImage(inter->pos - dir2Int, theCameraMatrix, theCameraInfo, p3);
        Geometry::calculatePointInImage(inter->pos + dir2Int, theCameraMatrix, theCameraInfo, p4);
        break;
      case Intersection::T:
        Geometry::calculatePointInImage(inter->pos, theCameraMatrix, theCameraInfo, p1);
        Geometry::calculatePointInImage(inter->pos + dir1Int, theCameraMatrix, theCameraInfo, p2);
        Geometry::calculatePointInImage(inter->pos - dir2Int, theCameraMatrix, theCameraInfo, p3);
        Geometry::calculatePointInImage(inter->pos + dir2Int, theCameraMatrix, theCameraInfo, p4);
        break;
      case Intersection::L:
        Geometry::calculatePointInImage(inter->pos, theCameraMatrix, theCameraInfo, p1);
        Geometry::calculatePointInImage(inter->pos + dir1Int, theCameraMatrix, theCameraInfo, p2);
        Geometry::calculatePointInImage(inter->pos, theCameraMatrix, theCameraInfo, p3);
        Geometry::calculatePointInImage(inter->pos + dir2Int, theCameraMatrix, theCameraInfo, p4);
        break;
      }

      Vector2<double> uncor1 = theImageCoordinateSystem.fromCorrectedApprox(p1);
      Vector2<double> uncor2 = theImageCoordinateSystem.fromCorrectedApprox(p2);
      Vector2<double> uncor3 = theImageCoordinateSystem.fromCorrectedApprox(p3);
      Vector2<double> uncor4 = theImageCoordinateSystem.fromCorrectedApprox(p4);
      ARROW("representation:LinePercept:Image", uncor1.x, uncor1.y, uncor2.x, uncor2.y,
                                                       3, Drawings::ps_solid, ColorClasses::robotBlue);
      ARROW("representation:LinePercept:Image", uncor3.x, uncor3.y, uncor4.x, uncor4.y,
                                                       3, Drawings::ps_solid, ColorClasses::blue);
   }
  );
}

void LinePercept::drawIn3D(const FieldDimensions& theFieldDimensions, int circleBiggerThanSpecified) const
{
  DECLARE_DEBUG_DRAWING3D("representation:LinePercept", "origin");
  TRANSLATE3D("representation:LinePercept", 0, 0, -210);
  COMPLEX_DRAWING3D("representation:LinePercept",
    for(std::list<Line>::const_iterator line = lines.begin(); line != lines.end(); line++)
      LINE3D("representation:LinePercept", line->first.x, line->first.y, 0, line->last.x, line->last.y, 0, 2, ColorClasses::red);
    for(std::vector<Intersection>::const_iterator inter = intersections.begin(); inter != intersections.end(); inter++)
    {
       switch(inter->type)
       {
       case Intersection::X:
         LINE3D("representation:LinePercept", inter->pos.x - inter->dir1.x * 100, inter->pos.y - inter->dir1.y * 100, 0,
                                              inter->pos.x + inter->dir1.x * 100, inter->pos.y + inter->dir1.y * 100, 0,
                                              2, ColorClasses::blue);
         LINE3D("representation:LinePercept", inter->pos.x - inter->dir2.x * 100, inter->pos.y - inter->dir2.y * 100, 0,
                                              inter->pos.x + inter->dir2.x * 100, inter->pos.y + inter->dir2.y * 100, 0,
                                              2, ColorClasses::blue);
         break;
       case Intersection::T:
         LINE3D("representation:LinePercept", inter->pos.x, inter->pos.y, 0, 
                                              inter->pos.x + inter->dir1.x * 100, inter->pos.y + inter->dir1.y * 100, 0,
                                              2, ColorClasses::blue);
         LINE3D("representation:LinePercept", inter->pos.x - inter->dir2.x * 100, inter->pos.y - inter->dir2.y * 100, 0,
                                              inter->pos.x + inter->dir2.x * 100, inter->pos.y + inter->dir2.y * 100, 0,
                                              2, ColorClasses::blue);
         break;
       case Intersection::L:
         LINE3D("representation:LinePercept", inter->pos.x, inter->pos.y, 0,
                                              inter->pos.x + inter->dir1.x * 100, inter->pos.y + inter->dir1.y * 100, 0,
                                              2, ColorClasses::blue);
         LINE3D("representation:LinePercept", inter->pos.x , inter->pos.y, 0,
                                              inter->pos.x + inter->dir2.x * 100, inter->pos.y + inter->dir2.y * 100, 0,
                                              2, ColorClasses::blue);
         break;
       }
    }
    if(circle.found)
    {
      Vector2<double> v1(circle.pos.x + theFieldDimensions.centerCircleRadius+circleBiggerThanSpecified, circle.pos.y);
      for(int i = 1; i < 33; ++i)
      {
        const double angle(i * pi2 / 32);
        Vector2<double> v2(circle.pos.x + cos(angle) * (theFieldDimensions.centerCircleRadius+circleBiggerThanSpecified), 
                           circle.pos.y + sin(angle) * (theFieldDimensions.centerCircleRadius+circleBiggerThanSpecified));
        LINE3D("representation:LinePercept", v1.x, v1.y, 0, v2.x, v2.y, 0, 2, ColorClasses::blue);
        v1 = v2;
      }
    }
    for(std::list<LineSegment>::const_iterator seg = singleSegs.begin(); seg != singleSegs.end(); seg++)
      LINE3D("representation:LinePercept", seg->p1.x, seg->p1.y, 0, seg->p2.x, seg->p2.y, 0, 2, ColorClasses::orange);
  );
}
