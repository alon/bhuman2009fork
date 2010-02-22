/**
* @file LinePerceptor.cpp
* @author jeff
*/

#include "LinePerceptor.h"
#include "Tools/Debugging/DebugDrawings.h"
#include "Tools/Streams/InStreams.h"
#include "Tools/Math/Geometry.h"
#include "Tools/Team.h"
#include "Tools/Debugging/ReleaseOptions.h"

LinePerceptor::LinePerceptor()
{
  InConfigFile stream("linePerceptor.cfg", "default");
  ASSERT(stream.exists());
  stream >> parameters;
  InConfigFile streamC("linePerceptor.cfg", "expCircle");
  ASSERT(streamC.exists());
  streamC >> circleParams;
  InConfigFile streamN("linePerceptor.cfg", "nonLine");
  ASSERT(streamN.exists());
  streamN >> nonLineParams;
  InConfigFile streamB("linePerceptor.cfg", "banSector");
  ASSERT(streamB.exists());
  streamB >> banSectorParams;
}

void LinePerceptor::update(LinePercept& linePercept)
{
  DECLARE_DEBUG_DRAWING("module:LinePerceptor:LineSegments", "drawingOnField");
  DECLARE_DEBUG_DRAWING("module:LinePerceptor:NonLineSegments", "drawingOnField");
  DECLARE_DEBUG_DRAWING("module:LinePerceptor:LineSegmentsImg", "drawingOnImage");
  DECLARE_DEBUG_DRAWING("module:LinePerceptor:banSectors", "drawingOnField");
  DECLARE_DEBUG_DRAWING("module:LinePerceptor:banSectorsImg", "drawingOnImage");
  DECLARE_DEBUG_DRAWING("module:LinePerceptor:Lines1", "drawingOnField");
  DECLARE_DEBUG_DRAWING("module:LinePerceptor:CircleSpots", "drawingOnField");
  DECLARE_DEBUG_DRAWING("module:LinePerceptor:CircleSpots2", "drawingOnField");
  DECLARE_DEBUG_DRAWING("module:LinePerceptor:CircleSpotsImg", "drawingOnImage");
  DECLARE_DEBUG_DRAWING("module:LinePerceptor:Lines2", "drawingOnField");
  DECLARE_DEBUG_DRAWING("module:LinePerceptor:Intersections", "drawingOnField");
  //Nao Masserfassungsgeraet
  DECLARE_DEBUG_DRAWING("module:LinePerceptor:naoMeter", "drawingOnImage");
  COMPLEX_DRAWING("module:LinePerceptor:naoMeter",
    Vector2<int> ppf;
    Vector2<int> ppf2;
    Vector2<double> pp2 = theImageCoordinateSystem.toCorrected(Vector2<int>(260, 200));
    Vector2<double> pp1 = theImageCoordinateSystem.toCorrected(Vector2<int>(160, 200));
    MODIFY("pp1", pp1);
    MODIFY("pp2", pp2);
    Geometry::calculatePointOnField((int)pp1.x, (int)pp1.y, theCameraMatrix, theCameraInfo, ppf);
    Geometry::calculatePointOnField((int)pp2.x, (int)pp2.y, theCameraMatrix, theCameraInfo, ppf2);
    LINE("module:LinePerceptor:naoMeter", pp1.x, pp1.y, pp2.x, pp2.y, 2, Drawings::ps_solid, ColorClasses::black);
    LINE("module:LinePerceptor:naoMeter", pp1.x, pp1.y-5, pp1.x, pp1.y+5, 2, Drawings::ps_solid, ColorClasses::black);
    LINE("module:LinePerceptor:naoMeter", pp2.x, pp2.y-5, pp2.x, pp2.y+5, 2, Drawings::ps_solid, ColorClasses::black);
    DRAWTEXT("module:LinePerceptor:naoMeter", pp1.x+10, pp1.y+20, 250, ColorClasses::black, "Nao sagt: " << (ppf-ppf2).abs() << "mm");
  );

  MODIFY("parameters:LinePerceptor",parameters);
  MODIFY("parameters:LinePerceptorCircle",circleParams);
  MODIFY("parameters:LinePerceptorNonLine",nonLineParams);
  MODIFY("parameters:LinePerceptorBanSector",banSectorParams);

  STOP_TIME_ON_REQUEST("clearPercept",
  lineSegs.clear();
  linePercept.clear();
  );
  banSectors.clear();

  STOP_TIME_ON_REQUEST("createLineSegments" ,createLineSegments(linePercept.singleSegs););
  STOP_TIME_ON_REQUEST("createLines", createLines(linePercept.lines, linePercept.singleSegs););
  STOP_TIME_ON_REQUEST("analyzeSingleSegments", analyzeSingleSegments(linePercept.singleSegs, linePercept.circle, linePercept.lines););
  STOP_TIME_ON_REQUEST("analyzeLines", analyzeLines(linePercept.lines, linePercept.intersections, linePercept.circle, linePercept.singleSegs););

  STOP_TIME_ON_REQUEST("drawLinePercept", 
    linePercept.drawOnField(theFieldDimensions, parameters.circleBiggerThanSpecified); 
    linePercept.drawOnImage(theCameraMatrix, theCameraInfo, theFieldDimensions, parameters.circleBiggerThanSpecified, theImageCoordinateSystem);
    linePercept.drawIn3D(theFieldDimensions, parameters.circleBiggerThanSpecified);
  );
  
  if(Global::getReleaseOptions().linePercept)
  {
    TEAM_OUTPUT(idLinePercept, bin, linePercept);
  }
}

void LinePerceptor::createBanSectors()
{
  for(std::vector<LineSpots::NonLineSpot>::const_iterator spot = theLineSpots.nonLineSpots.begin(); spot != theLineSpots.nonLineSpots.end(); spot++)
  {
    const Vector2<double> p1_cor = theImageCoordinateSystem.toCorrected(spot->p1),
                          p2_cor = theImageCoordinateSystem.toCorrected(spot->p2);
    Vector2<double> pf1, pf2;
    if(!Geometry::calculatePointOnFieldHacked((int)p1_cor.x, (int)p1_cor.y, theCameraMatrix, theCameraInfo, pf1))
    {
      //CROSS("module:LinePerceptor:LineSegmentsImg", ((spot->p1+spot->p2)/2).x, ((spot->p1+spot->p2)/2).y,4, 2, Drawings::ps_solid, ColorClasses::orange);
      //continue;
    }
    if(!Geometry::calculatePointOnFieldHacked((int)p2_cor.x, (int)p2_cor.y, theCameraMatrix, theCameraInfo, pf2))
    {
      pf2 = pf1 * 100;
        
      CROSS("module:LinePerceptor:LineSegmentsImg", ((spot->p1+spot->p2)/2).x, ((spot->p1+spot->p2)/2).y,4, 2, Drawings::ps_solid, ColorClasses::orange);
      //continue;
    }

    ARROW("module:LinePerceptor:NonLineSegments", pf1.x, pf1.y, pf2.x, pf2.y, 30, Drawings::ps_solid, ColorClasses::orange);

    double alpha = (pf1.angle() + pf2.angle()) / 2;

    while(alpha > pi_2)
      alpha -= pi;
    while(alpha < -pi_2)
      alpha += pi;

    const int dist = (int)pf1.abs();
    const int dist2 = (int)pf2.abs();

    for(std::list<BanSector>::iterator s = banSectors.begin(); s != banSectors.end(); s++)
    {
      if(s->alphaLeft < alpha && s->alphaRight > alpha)
      {
        if((dist > s->start && dist < s->end) ||
           (dist2 > s->start && dist2 < s->end))
        {
          if(s->start > dist)
            s->start = dist;
          if(s->end < dist2)
            s->end = dist2;
          if(alpha - banSectorParams.angleStepSize < s->alphaLeft)
            s->alphaLeft = alpha - banSectorParams.angleStepSize;
          if(alpha + banSectorParams.angleStepSize > s->alphaRight)
            s->alphaRight = alpha + banSectorParams.angleStepSize;
          s->counter++;
          goto continueOuter;
        }
      }
    }
    BanSector sector;
    sector.alphaLeft = alpha - banSectorParams.angleStepSize;
    sector.alphaRight = alpha + banSectorParams.angleStepSize;
    sector.start = dist;
    sector.end = dist2;
    sector.counter = 1;
    banSectors.push_back(sector);
continueOuter:
    ;
  }

  for(std::list<BanSector>::iterator s = banSectors.begin(); s != banSectors.end(); s++)
  {
   COMPLEX_DRAWING("module:LinePerceptor:banSectors",
        Vector2<int> p1((int)(cos(s->alphaLeft) * s->start), (int)(sin(s->alphaLeft) * s->start));
        Vector2<int> p2((int)(cos(s->alphaRight) * s->start), (int)(sin(s->alphaRight) * s->start));
        Vector2<int> p3((int)(cos(s->alphaLeft) * s->end), (int)(sin(s->alphaLeft) * s->end));
        Vector2<int> p4((int)(cos(s->alphaRight) * s->end), (int)(sin(s->alphaRight) * s->end));
        LINE("module:LinePerceptor:banSectors", p1.x, p1.y, p2.x, p2.y, 20, Drawings::ps_solid, ColorClasses::blue);
        LINE("module:LinePerceptor:banSectors", p1.x, p1.y, p3.x, p3.y, 20, Drawings::ps_solid, ColorClasses::blue);
        LINE("module:LinePerceptor:banSectors", p2.x, p2.y, p4.x, p4.y, 20, Drawings::ps_solid, ColorClasses::blue);
        LINE("module:LinePerceptor:banSectors", p3.x, p3.y, p4.x, p4.y, 20, Drawings::ps_solid, ColorClasses::blue);
    );

  }

  std::list<BanSector>::iterator nexts2;
  for(std::list<BanSector>::iterator s1 = banSectors.begin(); s1 != banSectors.end(); s1++)
  {
    nexts2 = s1;
    nexts2++;
    for(std::list<BanSector>::iterator s2 = banSectors.begin(); s2 != banSectors.end(); s2 = nexts2)
    {
      nexts2 = s2;
      nexts2++;

      if(s2 == s1)
        continue;

      if((s1->alphaLeft > s2->alphaLeft && s1->alphaLeft < s2->alphaRight) ||
         (s1->alphaRight > s2->alphaLeft && s1->alphaRight < s2->alphaRight) ||
         (s2->alphaLeft > s1->alphaLeft && s2->alphaLeft < s1->alphaRight) ||
         (s2->alphaRight > s1->alphaLeft && s2->alphaRight < s1->alphaRight) )
      {
         if((s1->start > s2->start && s1->start < s2->end) || 
            (s2->start > s1->start && s2->start < s1->end))
         {
           if(s2->alphaLeft < s1->alphaLeft) 
             s1->alphaLeft = s2->alphaLeft;
           if(s2->alphaRight > s1->alphaRight) 
             s1->alphaRight = s2->alphaRight;
           if(s2->start < s1->start)
             s1->start = s2->start;
           if(s2->end > s1->end)
             s1->end = s2->end;
           s1->counter += s2->counter;
           nexts2 = banSectors.erase(s2);
         }
      }
    }
  }

  std::list<BanSector>::iterator nexts;
  for(std::list<BanSector>::iterator s = banSectors.begin(); s != banSectors.end(); s = nexts)
  {
    nexts = s;
    nexts++;
    if(s->counter < banSectorParams.minSectorCounter)
    {
      COMPLEX_DRAWING("module:LinePerceptor:banSectors",
        Vector2<int> p1((int)(cos(s->alphaLeft) * s->start), (int)(sin(s->alphaLeft) * s->start));
        Vector2<int> p2((int)(cos(s->alphaRight) * s->start), (int)(sin(s->alphaRight) * s->start));
        Vector2<int> mid = (p1 + p2)/2;
        CROSS("module:LinePerceptor:banSectors", mid.x, mid.y, 80, 40, Drawings::ps_solid, ColorClasses::red);
      );
      nexts = banSectors.erase(s);
      continue;
    }

    COMPLEX_DRAWING("module:LinePerceptor:banSectors",
        Vector2<int> p1((int)(cos(s->alphaLeft) * s->start), (int)(sin(s->alphaLeft) * s->start));
        Vector2<int> p2((int)(cos(s->alphaRight) * s->start), (int)(sin(s->alphaRight) * s->start));
        Vector2<int> p3((int)(cos(s->alphaLeft) * s->end), (int)(sin(s->alphaLeft) * s->end));
        Vector2<int> p4((int)(cos(s->alphaRight) * s->end), (int)(sin(s->alphaRight) * s->end));
        LINE("module:LinePerceptor:banSectors", p1.x, p1.y, p2.x, p2.y, 20, Drawings::ps_dash, ColorClasses::red);
        LINE("module:LinePerceptor:banSectors", p1.x, p1.y, p3.x, p3.y, 20, Drawings::ps_dash, ColorClasses::red);
        LINE("module:LinePerceptor:banSectors", p2.x, p2.y, p4.x, p4.y, 20, Drawings::ps_dash, ColorClasses::red);
        LINE("module:LinePerceptor:banSectors", p3.x, p3.y, p4.x, p4.y, 20, Drawings::ps_dash, ColorClasses::red);
        Vector2<int> pmid = (p1 + p2)/2 + p3/2;
        DRAWTEXT("module:LinePerceptor:banSectors", pmid.x, pmid.y, 150, ColorClasses::black, s->counter);

        /*Vector2<double> p1cor = theImageCoordinateSystem.fromCorrectedApprox(p1);
        Vector2<double> p2cor = theImageCoordinateSystem.fromCorrectedApprox(p2);
        Vector2<double> p3cor = theImageCoordinateSystem.fromCorrectedApprox(p3);
        Vector2<double> p4cor = theImageCoordinateSystem.fromCorrectedApprox(p4);*/
        Vector2<double> p1cor = Vector2<double>(p1.x, p1.y);
        Vector2<double> p2cor = Vector2<double>(p2.x, p2.y);
        Vector2<double> p3cor = Vector2<double>(p3.x, p3.y);
        Vector2<double> p4cor = Vector2<double>(p4.x, p4.y);

        Vector2<int> p1Img;
        Vector2<int> p2Img;
        Vector2<int> p3Img;
        Vector2<int> p4Img;

        Geometry::calculatePointInImage(Vector2<int>((int)p1cor.x, (int)p1cor.y), theCameraMatrix, theCameraInfo, p1Img);
        Geometry::calculatePointInImage(Vector2<int>((int)p2cor.x, (int)p2cor.y), theCameraMatrix, theCameraInfo, p2Img);
        Geometry::calculatePointInImage(Vector2<int>((int)p3cor.x, (int)p3cor.y), theCameraMatrix, theCameraInfo, p3Img);
        Geometry::calculatePointInImage(Vector2<int>((int)p4cor.x, (int)p4cor.y), theCameraMatrix, theCameraInfo, p4Img);

        LINE("module:LinePerceptor:banSectorsImg", p1Img.x, p1Img.y, p2Img.x, p2Img.y, 2, Drawings::ps_dot, ColorClasses::red);
        LINE("module:LinePerceptor:banSectorsImg", p1Img.x, p1Img.y, p3Img.x, p3Img.y, 2, Drawings::ps_dot, ColorClasses::red);
        LINE("module:LinePerceptor:banSectorsImg", p2Img.x, p2Img.y, p4Img.x, p4Img.y, 2, Drawings::ps_dot, ColorClasses::red);
        LINE("module:LinePerceptor:banSectorsImg", p3Img.x, p3Img.y, p4Img.x, p4Img.y, 2, Drawings::ps_dot, ColorClasses::red);
    );
  }

}

void LinePerceptor::createLineSegments(std::list<LinePercept::LineSegment>& singleSegs)
{
  createBanSectors();

  for(std::vector<LineSpots::LineSpot>::const_iterator spot = theLineSpots.spots.begin(); spot != theLineSpots.spots.end(); spot++)
  {
    if(spot->alpha_len/spot->alpha_len2 > parameters.minWidthRatio)
    {
      //transform to field coordinates
      const Vector2<double> p1_cor = theImageCoordinateSystem.toCorrected(spot->p1),
                            p2_cor = theImageCoordinateSystem.toCorrected(spot->p2);
      Vector2<int> pf1, pf2;
      if(!Geometry::calculatePointOnField((int)p1_cor.x, (int)p1_cor.y, theCameraMatrix, theCameraInfo, pf1))
      {
        CROSS("module:LinePerceptor:LineSegmentsImg", ((spot->p1+spot->p2)/2).x, ((spot->p1+spot->p2)/2).y,4, 2, Drawings::ps_solid, ColorClasses::orange);
        continue;
      }
      if(!Geometry::calculatePointOnField((int)p2_cor.x, (int)p2_cor.y, theCameraMatrix, theCameraInfo, pf2))
      {
        CROSS("module:LinePerceptor:LineSegmentsImg", ((spot->p1+spot->p2)/2).x, ((spot->p1+spot->p2)/2).y,4, 2, Drawings::ps_solid, ColorClasses::orange);
        continue;
      }

      //hmmmmm at least in some special cases (corrupted log file) this happens and causes
      //other parts of this module to crash. So if it happens, just ignore the spot
      //this seems to happen if the camera matrix gets messed up
      if(pf1 == pf2)
      {
        CROSS("module:LinePerceptor:LineSegmentsImg", ((spot->p1+spot->p2)/2).x, ((spot->p1+spot->p2)/2).y,4, 2, Drawings::ps_solid, ColorClasses::robotBlue);
        continue;
      }

      if(pf1 * pf1 > sqr(parameters.maxLineDistance) || pf2 * pf2 > sqr(parameters.maxLineDistance))
      {
        CROSS("module:LinePerceptor:LineSegments", ((pf1+pf2)/2).x, ((pf1+pf2)/2).y,20, 20, Drawings::ps_solid, ColorClasses::red);
        continue;
      }


      if(fabs(fabs(spot->alpha) - pi_2) < nonLineParams.maxAlphaDiff && spot->alpha_len / spot->alpha_len2 > nonLineParams.minWidthRatio && (pf1 - pf2).squareAbs() > sqr(nonLineParams.minLineLength))
      {
        ARROW("module:LinePerceptor:LineSegmentsImg", spot->p1.x, spot->p1.y, spot->p2.x, spot->p2.y, 3, Drawings::ps_dash, ColorClasses::red);
        ARROW("module:LinePerceptor:NonLineSegments", pf1.x, pf1.y, pf2.x, pf2.y, 30, Drawings::ps_dash, ColorClasses::red);

        double alpha = (pf1.angle() + pf2.angle()) / 2;

        while(alpha >= pi_2)
          alpha -= pi;
        while(alpha < -pi_2)
          alpha += pi;

        bool con = false;
        const int dist = ((pf1 + pf2)/2).squareAbs();
        for(std::list<BanSector>::iterator s = banSectors.begin(); s != banSectors.end(); s++)
        {
          if((alpha > s->alphaLeft && alpha < s->alphaRight) ||
              fabs(alpha - s->alphaLeft) < banSectorParams.maxLineAngleDiff ||
              fabs(alpha - s->alphaRight) < banSectorParams.maxLineAngleDiff)
          {
            if(dist > sqr(s->start))
            {
              ARROW("module:LinePerceptor:LineSegmentsImg", spot->p1.x, spot->p1.y, spot->p2.x, spot->p2.y, 3, Drawings::ps_dash, ColorClasses::yellow);
              con = true;
              break;
            }
          }
        }
        if(con)
          continue;
      }

      
      LinePercept::LineSegment s;
      s.p1 = pf1;
      s.p2 = pf2;
      s.p1Img = spot->p1;
      s.p2Img = spot->p2;

      Vector2<int> diff = pf2 - pf1;
      //hmmm here once again we need a magic offset of 90 degree
      s.alpha = diff.angle() + pi_2;
      //normalize alpha
      while(s.alpha < 0)
        s.alpha += pi;
      while(s.alpha > pi)
        s.alpha -= pi;
      const double d1 = s.p1.x * cos(s.alpha) + s.p1.y * sin(s.alpha),
                   d2 = s.p2.x * cos(s.alpha) + s.p2.y * sin(s.alpha);
      s.d = (d1 + d2) / 2.0f;
      lineSegs.push_back(s);

      ARROW("module:LinePerceptor:LineSegmentsImg", spot->p1.x, spot->p1.y, spot->p2.x, spot->p2.y, 1, Drawings::ps_solid, ColorClasses::blue);
      COMPLEX_DRAWING("module:LinePerceptor:LineSegments",
        ARROW("module:LinePerceptor:LineSegments", pf1.x, pf1.y, pf2.x, pf2.y, 15, Drawings::ps_solid, ColorClasses::blue);
        /*
        DRAWTEXT("module:LinePerceptor:LineSegments", (pf1.x+pf2.x)/2, (pf1.y+pf2.y)/2+50, 10, ColorClasses::black, s.alpha);
        DRAWTEXT("module:LinePerceptor:LineSegments", (pf1.x+pf2.x)/2, (pf1.y+pf2.y)/2, 10, ColorClasses::black, s.d);*/
      );
 
    }
  }
}

void LinePerceptor::createLines(std::list<LinePercept::Line>& lines, std::list<LinePercept::LineSegment>& singleSegs)
{
  //Hough Transformation fuer (ganz) arme....
  while(lineSegs.size() > 0)
  {
    //pick a segment...
    LinePercept::LineSegment seg = *lineSegs.begin();
    lineSegs.erase(lineSegs.begin());

    ARROW("module:LinePerceptor:Lines1", seg.p1.x, seg.p1.y, seg.p2.x, seg.p2.y, 15, Drawings::ps_solid, ColorClasses::white);

    //collect supporters...
    std::vector<std::list<LinePercept::LineSegment>::iterator> supporters;
    int maxSegmentLength = 0;
    for(std::list<LinePercept::LineSegment>::iterator other = lineSegs.begin(); other != lineSegs.end(); other++)
    {
      if((fabs(other->alpha - seg.alpha) < parameters.maxAlphaDiff &&
         fabs(other->d - seg.d) < parameters.maxDDiff))
      {
        const int sqr_length = (other->p1-other->p2).squareAbs();
        if(sqr_length > maxSegmentLength)
          maxSegmentLength = sqr_length;
        supporters.push_back(other);
      }
      else if((fabs(fabs(other->alpha - seg.alpha) - pi) < parameters.maxAlphaDiff &&
         fabs(other->d + seg.d) < parameters.maxDDiff))
      {
        const int sqr_length = (other->p1-other->p2).squareAbs();
        if(sqr_length > maxSegmentLength)
          maxSegmentLength = sqr_length;
        //make supporters all look into the same direction (alpha in [0...pi])
        if(other->alpha > seg.alpha)
          other->alpha -= pi;
        else
          other->alpha += pi;
        other->d *= -1;
        supporters.push_back(other);
      }
    }
    maxSegmentLength = static_cast<int>(sqrt(static_cast<double>(maxSegmentLength)));

    //if you have enough supporters, you become a line
    if((int)supporters.size() >= parameters.minSupporters && maxSegmentLength > parameters.minLineStartLength)
    {
      COMPLEX_DRAWING("module:LinePerceptor:Lines1",
        CROSS("module:LinePerceptor:Lines1", (seg.p1.x+seg.p2.x)/2, (seg.p1.y+seg.p2.y)/2,20, 20, Drawings::ps_solid, ColorClasses::red);
        DRAWTEXT("module:LinePerceptor:Lines1", seg.p1.x+50, seg.p1.y+100, 10, ColorClasses::black, (int)supporters.size());
      );
      LinePercept::Line l;
      double d = seg.d, alpha = seg.alpha;
      l.dead = false;
      l.midLine = false;
      l.segments.push_back(seg);
      for(std::vector<std::list<LinePercept::LineSegment>::iterator>::const_iterator sup = supporters.begin(); sup != supporters.end(); sup++)
      {
        ARROW("module:LinePerceptor:Lines1", (*sup)->p1.x, (*sup)->p1.y, (*sup)->p2.x, (*sup)->p2.y, 15, Drawings::ps_solid, ColorClasses::red);
        ARROW("module:LinePerceptor:Lines1", seg.p1.x, seg.p1.y, (*sup)->p1.x, (*sup)->p1.y, 5, Drawings::ps_solid, ColorClasses::robotBlue);
        d += (*sup)->d;
        alpha += (*sup)->alpha;
        l.segments.push_back(*(*sup));
      }
      for(std::vector<std::list<LinePercept::LineSegment>::iterator>::const_reverse_iterator sup = supporters.rbegin(); sup != supporters.rend(); sup++)
        lineSegs.erase(*sup);
      l.d = d / ((int)supporters.size()+1);
      l.alpha = alpha / ((int)supporters.size()+1);
      lines.push_back(l); 
    }
    else
      singleSegs.push_back(seg);
  }
}

void LinePerceptor::getFirstAndLastOfLine(LinePercept::Line& line, Vector2<int>& first, Vector2<int>& last, bool updateLine)
{
  Vector2<int> p_ref=line.segments.at(0).p1;
  first=p_ref;
  last=p_ref;
  double first_dist=0, last_dist=0;
  Vector2<int> alphaDir = Vector2<int>(0,0); 
  std::vector<LinePercept::LineSegment>::const_iterator prevSeg;
  for(std::vector<LinePercept::LineSegment>::const_iterator seg = line.segments.begin(); seg != line.segments.end(); seg++)
  {
      if(seg != line.segments.begin())
      {
        ARROW("module:LinePerceptor:Lines2", ((seg->p1 + seg->p2)/2).x, ((seg->p1 + seg->p2)/2).y,((prevSeg->p1 + prevSeg->p2)/2).x, ((prevSeg->p1 + prevSeg->p2)/2).y , 5, Drawings::ps_solid, ColorClasses::yellow);
        alphaDir += (seg->p1 + seg->p2)/2 - (prevSeg->p1 + prevSeg->p2)/2;
      }

      const Vector2<int> diffp1 = seg->p1 - p_ref,
                         diffp2 = seg->p2 - p_ref;

      //if dist(p1, p_ref) > dist(first, p_ref) and dist(first, p1) < dist(p1, p_ref
      //-->means if p1 is farer away from p_ref as first and is on the same side of p_ref as first
      if(diffp1.squareAbs() > first_dist && (seg->p1 - first).squareAbs() <= diffp1.squareAbs())
      {
          first = seg->p1;
          first_dist = diffp1.squareAbs();
      }
      else if(diffp1.squareAbs() > last_dist && (seg->p1 - first).squareAbs() > (p_ref - first).squareAbs() && (seg->p1 - last).squareAbs() <= diffp1.squareAbs())
      {
        last = seg->p1;
        last_dist = diffp1.squareAbs();
      }
      if(diffp2.squareAbs() > first_dist && (seg->p2 - first).squareAbs() <= diffp2.squareAbs())
      {
          first = seg->p2;
          first_dist = diffp2.squareAbs();
      }
      else if(diffp2.squareAbs() > last_dist && (seg->p2 - first).squareAbs() > (p_ref - first).squareAbs() && (seg->p2 - last).squareAbs() <= diffp2.squareAbs())
      {
        last = seg->p2;
        last_dist = diffp2.squareAbs();
      }
      prevSeg = seg;
  }

  if(updateLine)
  {
    if((int) line.segments.size() > 1)
    {
      alphaDir /= (int)line.segments.size()-1;
      line.alpha = alphaDir.angle()+pi_2;
      while(line.alpha < 0)
        line.alpha += pi;
      while(line.alpha > pi)
        line.alpha -= pi;

      line.d = 0;
      const double c = cos(line.alpha),
                   s = sin(line.alpha);
      for(std::vector<LinePercept::LineSegment>::const_iterator seg = line.segments.begin(); seg != line.segments.end(); seg++)
        line.d += seg->p1.x * c + seg->p1.y * s;
      line.d /= (int)line.segments.size();
    }
    else
    {
      line.alpha = prevSeg->alpha;
      line.d = prevSeg->d;
    }
  }

  first = line.calculateClosestPointOnLine(first);
  last = line.calculateClosestPointOnLine(last);
}

void LinePerceptor::analyzeLines(std::list<LinePercept::Line>& lines, std::vector<LinePercept::Intersection>& intersections, LinePercept::CircleSpot& circle, std::list<LinePercept::LineSegment>& singleSegs)
{
  //the points first and last are the two points on the line which
  //have the greatest distance to each other ("endpoints")
  for(std::list<LinePercept::Line>::iterator line = lines.begin(); line != lines.end(); line++)
    getFirstAndLastOfLine(*line, line->first, line->last);

  
  //delete lines if their circleSpot is near the found circle
  if(circle.found)
  {
    std::vector<std::list<LinePercept::Line>::iterator> toDelete;
    for(std::list<LinePercept::Line>::iterator l1 = lines.begin(); l1 != lines.end(); l1++)
    {
      Vector2<int> line_mid = (l1->first + l1->last)/2;
      CROSS("module:LinePerceptor:CircleSpots", line_mid.x, line_mid.y, 30, 30, Drawings::ps_solid, ColorClasses::green);
      Vector2<int> bla = Vector2<int>(line_mid + (l1->first-l1->last).rotate(pi_2).normalize(theFieldDimensions.centerCircleRadius+parameters.circleBiggerThanSpecified));
      CROSS("module:LinePerceptor:CircleSpots", bla.x, bla.y, 30, 30, Drawings::ps_solid, ColorClasses::green);
      if((bla - circle.pos).squareAbs() < sqr(parameters.maxLineCircleDist))
        toDelete.push_back(l1);
      else 
      {
        Vector2<int> bla = Vector2<int>(line_mid + (l1->first-l1->last).rotate(-pi_2).normalize(theFieldDimensions.centerCircleRadius+parameters.circleBiggerThanSpecified));
        CROSS("module:LinePerceptor:CircleSpots", bla.x, bla.y, 30, 30, Drawings::ps_solid, ColorClasses::green);
        if((bla - circle.pos).squareAbs() < sqr(parameters.maxLineCircleDist))
          toDelete.push_back(l1);
      }

    }
    for(std::vector<std::list<LinePercept::Line>::iterator>::iterator t = toDelete.begin(); t != toDelete.end(); t++)
      lines.erase(*t);
  }
  
  //find lines which are allmost parallel
  std::vector<std::list<LinePercept::Line>::iterator> toDelete;
  for(std::list<LinePercept::Line>::iterator line = lines.begin(); line != lines.end(); line++)
  {
    if(line->dead)
      continue;
    std::list<LinePercept::Line>::iterator other = line;
    other++;
    for(; other != lines.end(); other++)
    {
      if(other->dead)
        continue;

      double alphaDiff = line->alpha - other->alpha;
      while(alphaDiff < - pi_2)
        alphaDiff += pi;
      while(alphaDiff > pi_2)
        alphaDiff -= pi;
      //if endpoints of the other line are close to the line
      if( ((fabs(line->calculateDistToLine(other->first)) < parameters.maxLineUniteDist &&
            fabs(line->calculateDistToLine(other->last)) < parameters.maxLineUniteDist)
          ||
           (fabs(other->calculateDistToLine(line->first)) < parameters.maxLineUniteDist &&
            fabs(other->calculateDistToLine(line->last)) < parameters.maxLineUniteDist)
          )
         &&
          fabs(alphaDiff) < parameters.maxLineUniteAlphaDiff
        )
      {
        line->segments.insert(line->segments.end(), other->segments.begin(), other->segments.end());
        line->d = (line->d + other->d) / 2;
        if(line->alpha - other->alpha > pi_2)
          other->alpha += pi;
        else if(other->alpha - line->alpha > pi_2)
          other->alpha -= pi;
        line->alpha = (line->alpha + other->alpha) / 2;
        if(line->alpha < 0)
          line->alpha += pi;
        getFirstAndLastOfLine(*line, (*line).first, (*line).last);
        toDelete.push_back(other);
        other->dead = true;

      }
    }
  }
  for(std::vector<std::list<LinePercept::Line>::iterator>::iterator t = toDelete.begin(); t != toDelete.end(); t++)
    lines.erase(*t);

  //add singleSegments where the start and end pos is close to a line to the line
  std::vector<std::list<LinePercept::LineSegment>::iterator> ttoDelete;
  for(std::list<LinePercept::LineSegment>::iterator seg = singleSegs.begin(); seg != singleSegs.end(); seg++)
  {
    for(std::list<LinePercept::Line>::iterator line = lines.begin(); line != lines.end(); line++)
    {

      if(fabs(seg->p1.x * cos(line->alpha) + seg->p1.y * sin(line->alpha) - line->d) < parameters.maxLineSingleSegDist &&
         fabs(seg->p2.x * cos(line->alpha) + seg->p2.y * sin(line->alpha) - line->d) < parameters.maxLineSingleSegDist)
      {
        double firstToLast = (line->last - line->first).abs();
        const Vector2<int> segMid = (seg->p1 + seg->p2)/2;
        CROSS("module:LinePerceptor:Lines2", segMid.x, segMid.y,30, 20, Drawings::ps_solid, ColorClasses::yellow);
        if( (firstToLast < (line->last - segMid).abs() || firstToLast < (line->first - segMid).abs()) )
        {
          //seg is not between first and last
          const double minToLine = (line->last - segMid).abs() > (line->first - segMid).abs() ? (line->first - segMid).abs() : (line->last - segMid).abs();
          
          COMPLEX_DRAWING("module:LinePerceptor:Lines2",
            DRAWTEXT("module:LinePerceptor:Lines2", segMid.x+20, segMid.y, 10, ColorClasses::black, minToLine);
          );

          if(minToLine > parameters.maxLineSingleSegDist2)
            continue;
          CROSS("module:LinePerceptor:Lines2", segMid.x, segMid.y,30, 20, Drawings::ps_solid, ColorClasses::red);
        }
        else
          CROSS("module:LinePerceptor:Lines2", segMid.x, segMid.y,30, 20, Drawings::ps_solid, ColorClasses::blue);

        line->segments.push_back(*seg);
        ttoDelete.push_back(seg);
        getFirstAndLastOfLine(*line, line->first, line->last, false);
        break;
      }
    }
  }
  for(std::vector<std::list<LinePercept::LineSegment>::iterator>::iterator d = ttoDelete.begin(); d != ttoDelete.end(); d++)
    singleSegs.erase(*d);
  

  //delete lines which do not "hard cover" (length / sum(segemnts.length)) enough
  toDelete.clear();
  for(std::list<LinePercept::Line>::iterator l1 = lines.begin(); l1 != lines.end(); l1++)
  {
    int hardcover = 0;
    for(std::vector<LinePercept::LineSegment>::iterator seg = l1->segments.begin(); seg != l1->segments.end(); seg++)
      hardcover += (seg->p1 - seg->p2).abs();
    if(hardcover/(double)(l1->first - l1->last).abs() < parameters.minHardcover)
      toDelete.push_back(l1);
  }
  for(std::vector<std::list<LinePercept::Line>::iterator>::iterator t = toDelete.begin(); t != toDelete.end(); t++)
  {
    for(std::vector<LinePercept::LineSegment>::iterator seg = (*(*t)).segments.begin(); seg != (*(*t)).segments.end(); seg++)
      singleSegs.push_back(*seg);
    lines.erase(*t);
  }
  
  //find intersections
  for(std::list<LinePercept::Line>::iterator l1 = lines.begin(); l1 != lines.end(); l1++)
  {
    std::list<LinePercept::Line>::iterator l2 = l1;
    for(l2++; l2 != lines.end(); l2++)
    {
      double alphaDiff = l1->alpha - l2->alpha;
      while(alphaDiff < -pi_2)
        alphaDiff += pi;
      while(alphaDiff > pi_2)
        alphaDiff -= pi;
      if(fabs(alphaDiff) < parameters.minIntersectionAlphaDiff)
        continue;

      if((l1->first - l1->last).squareAbs() < sqr(parameters.minIntersectionLength) &&
         (l2->first - l2->last).squareAbs() < sqr(parameters.minIntersectionLength))
        continue;

      //zwei hessesche normaleformen gleichsetzen und aufloesen, dann kommt das bei raus
      const double zaehler = l1->d - (l2->d * cos(l1->alpha)/cos(l2->alpha)),
                   nenner = sin(l1->alpha) - (sin(l2->alpha)*cos(l1->alpha)/cos(l2->alpha));
      const double y_s = zaehler/nenner,
                   x_s = (l1->d - y_s * sin(l1->alpha))/cos(l1->alpha);

      if(y_s == y_s && x_s == x_s)//intersection exists -> not paralel || ident
      {
        const Vector2<int> s_p = Vector2<int>((int)x_s, (int)y_s);
        //this is some freay stuff which determines in which relation the
        //point s_p is to l1->first/last and l2->first/last given s_p is the
        //intersectionpoint of l1 and l2
        //distToLx = ( -min(dist(sp,last/first)) if in between,  )
        //           (  min(dist(s_p,last/first) else)           )
        double spToFirst = (s_p - l1->first).abs(),
                     spToLast = (s_p - l1->last).abs(),
                     firstToLast = (l1->first - l1->last).abs();
        double distToL1=0, distToL2=0;
        if(spToFirst < firstToLast && spToLast < firstToLast)
          //sp is between first and last
          distToL1 = - ( spToFirst > spToLast ? spToLast : spToFirst);
        else if(spToFirst >= firstToLast)
          //sp is closer to last
          distToL1 = spToLast;
        else if(spToLast >= firstToLast)
          //sp is closer to first
          distToL1 = spToFirst;
        else
          ASSERT(false);
        spToFirst = (s_p - l2->first).abs(),
        spToLast = (s_p - l2->last).abs(),
        firstToLast = (l2->first - l2->last).abs();
        if(spToFirst < firstToLast && spToLast < firstToLast)
          //sp is between first and last
          distToL2 = - ( spToFirst > spToLast ? spToLast : spToFirst);
        else if(spToFirst >= firstToLast)
          //sp is closer to last
          distToL2 = spToLast;
        else if(spToLast >= firstToLast)
          //sp is closer to first
          distToL2 = spToFirst;
        else
          ASSERT(false);
        //end freaky stuff

        LinePercept::Intersection inter;
        inter.pos = Vector2<int>((int)x_s, (int)y_s);
        Vector2<int> t1 = l1->first - l1->last,
                     t2 = l2->first - l2->last;
        //this checks whether the intersection point is closer to first
        //or to last and if it is closer to first we need to flip the
        //direction
        if((l1->first - inter.pos).squareAbs() < (l1->last - inter.pos).squareAbs())
          t1 = l1->last - l1->first;
        if((l2->first - inter.pos).squareAbs() < (l2->last - inter.pos).squareAbs())
          t2 = l2->last - l2->first;
        //this is the heading of the intersection (to l1 and l2)
        Vector2<double> dirL1 = Vector2<double>(t1.x, t1.y).normalize(),
                     dirL2 = Vector2<double>(t2.x, t2.y).normalize();


        if(distToL1 < -parameters.minTToEnd && distToL2 < -parameters.minTToEnd)
        {
          ARROW("module:LinePerceptor:Intersections", x_s, y_s, l1->last.x, l1->last.y, 5, Drawings::ps_solid, ColorClasses::yellow);
          ARROW("module:LinePerceptor:Intersections", x_s, y_s, l2->last.x, l2->last.y, 5, Drawings::ps_solid, ColorClasses::yellow);
          //this is a X
          inter.type = LinePercept::Intersection::X;
          inter.dir1 = dirL1;
          inter.dir2 = dirL2;
          intersections.push_back(inter);
        }
        else if((distToL1 < -parameters.minTToEnd && distToL2 < parameters.maxTFromEnd) ||
                (distToL2 < -parameters.minTToEnd && distToL1 < parameters.maxTFromEnd))
        {
          ARROW("module:LinePerceptor:Intersections", x_s, y_s, l1->last.x, l1->last.y, 5, Drawings::ps_solid, ColorClasses::yellow);
          ARROW("module:LinePerceptor:Intersections", x_s, y_s, l2->last.x, l2->last.y, 5, Drawings::ps_solid, ColorClasses::yellow);
          //this is a T
          inter.type = LinePercept::Intersection::T;
          if(distToL2 < -parameters.minTToEnd && distToL1 < parameters.maxTFromEnd)
          {
            //l2 is the intersected line (the upper part of the T)
            inter.dir1 = dirL1;
            inter.dir2 = dirL2;
          }
          else
          {
            //l1 is the intersected line (the upper part of the T)
            inter.dir1 = dirL2;
            inter.dir2 = dirL1;
          }
          intersections.push_back(inter);
        }
        else if(distToL1 < parameters.maxTFromEnd && distToL2 < parameters.maxTFromEnd)
        {
          ARROW("module:LinePerceptor:Intersections", x_s, y_s, l1->last.x, l1->last.y, 5, Drawings::ps_solid, ColorClasses::yellow);
          ARROW("module:LinePerceptor:Intersections", x_s, y_s, l2->last.x, l2->last.y, 5, Drawings::ps_solid, ColorClasses::yellow);
          //this is a L
          inter.type = LinePercept::Intersection::L;
          inter.dir1 = dirL1;
          inter.dir2 = dirL2;
          intersections.push_back(inter);
        }
      }
    }
  }

  //find "mittellinie"
  if(circle.found)
  {
    std::list<LinePercept::Line>::iterator closestLine;
    int minDist = -1;
    for(std::list<LinePercept::Line>::iterator l1 = lines.begin(); l1 != lines.end(); l1++)
    {
      const int dist = (int)fabs(l1->calculateDistToLine(circle.pos));
      if( dist < parameters.maxMidLineToCircleDist &&
         (dist < minDist || minDist == -1) &&
         (l1->first - l1->last).squareAbs() > sqr(parameters.minMidLineLength))
      {
        closestLine = l1;
        minDist = dist;
      }
    }

    if(minDist != -1)
    {
      closestLine->midLine = true;
      circle.pos = closestLine->calculateClosestPointOnLine(circle.pos);

      //intersections
      const Vector2<int> midLineDir = (closestLine->first - closestLine->last).normalize(theFieldDimensions.centerCircleRadius + parameters.circleBiggerThanSpecified);
      LinePercept::Intersection inter;

      inter.pos = circle.pos + midLineDir;
      inter.dir1 = Vector2<double>(midLineDir.x, midLineDir.y).normalize();
      inter.dir2 = inter.dir1;
      inter.dir2.rotateLeft();
      inter.type = LinePercept::Intersection::X;
      intersections.push_back(inter);

      inter.pos = circle.pos - midLineDir;
      inter.dir1 = Vector2<double>(midLineDir.x, midLineDir.y).normalize();
      inter.dir2 = inter.dir1;
      inter.dir2.rotateLeft();
      inter.type = LinePercept::Intersection::X;
      intersections.push_back(inter);
    }
  }
}

void LinePerceptor::analyzeSingleSegments(std::list<LinePercept::LineSegment>& singleSegs, LinePercept::CircleSpot& circle, std::list<LinePercept::Line>& lines)
{

  std::list<LinePercept::CircleSpot> circleSpots;
  std::list<LinePercept::CircleSpot> circleSpots2;
  LinePercept::CircleSpot spot;

  std::list<LinePercept::LineSegment>::iterator seg2;
  for(std::list<LinePercept::LineSegment>::iterator seg = singleSegs.begin(); seg != singleSegs.end(); seg++)
  {
    ARROW("module:LinePerceptor:CircleSpots", seg->p1.x, seg->p1.y, seg->p2.x, seg->p2.y, 20, Drawings::ps_solid, ColorClasses::robotBlue);

    const Vector2<int> seg_dir = seg->p1 - seg->p2;
    const Vector2<int> seg_mid = (seg->p1 + seg->p2)/2;
    const Vector2<int> seg_norm = Vector2<int>(seg_dir.x, seg_dir.y).rotateLeft();

    const Vector2<int> spot1 = seg_mid + (seg->p1-seg->p2).rotate(pi_2).normalize(theFieldDimensions.centerCircleRadius+parameters.circleBiggerThanSpecified);
    spot.pos = spot1;
    spot.iterator = seg;
    LINE("module:LinePerceptor:CircleSpots2", spot.pos.x, spot.pos.y, seg_mid.x, seg_mid.y, 5, Drawings::ps_solid, ColorClasses::yellow);
    CROSS("module:LinePerceptor:CircleSpots2", spot.pos.x, spot.pos.y,20, 20, Drawings::ps_solid, ColorClasses::yellow);
    circleSpots2.push_back(spot);
    const Vector2<int> spot2 = seg_mid + (seg->p1-seg->p2).rotate(-pi_2).normalize(theFieldDimensions.centerCircleRadius+parameters.circleBiggerThanSpecified);
    spot.pos = spot2;
    spot.iterator = seg;
    LINE("module:LinePerceptor:CircleSpots2", spot.pos.x, spot.pos.y, seg_mid.x, seg_mid.y, 5, Drawings::ps_solid, ColorClasses::yellow);
    CROSS("module:LinePerceptor:CircleSpots2", spot.pos.x, spot.pos.y,20, 20, Drawings::ps_solid, ColorClasses::yellow);
    circleSpots2.push_back(spot);

    if(seg_dir.squareAbs() < sqr(circleParams.minSegmentLength) || (seg->p1Img - seg->p2Img).squareAbs() < sqr(circleParams.minSegmentImgLength))
      continue;

    LINE("module:LinePerceptor:CircleSpots", seg_mid.x, seg_mid.y, seg_mid.x + seg_norm.x, seg_mid.y + seg_norm.y, 20, Drawings::ps_solid, ColorClasses::orange);

    seg2 = seg;
    seg2++;
    for(; seg2 != singleSegs.end(); seg2++)
    {
      const Vector2<int> seg2_dir = seg2->p1 - seg2->p2;
      if(seg2_dir.squareAbs() < sqr(circleParams.minSegmentLength) || (seg2->p1Img - seg2->p2Img).squareAbs() < sqr(circleParams.minSegmentImgLength))
        continue;

      if((seg->p1 - seg2->p1).squareAbs() < sqr(circleParams.maxNgbhDist) ||
         (seg->p1 - seg2->p2).squareAbs() < sqr(circleParams.maxNgbhDist) ||
         (seg->p2 - seg2->p1).squareAbs() < sqr(circleParams.maxNgbhDist) ||
         (seg->p2 - seg2->p2).squareAbs() < sqr(circleParams.maxNgbhDist))
      {
        const Vector2<int> seg2_mid = (seg2->p1 + seg2->p2)/2;
        LINE("module:LinePerceptor:CircleSpots", seg_mid.x, seg_mid.y, seg2_mid.x, seg2_mid.y, 20, Drawings::ps_solid, ColorClasses::red);
        const Vector2<int> seg2_norm = Vector2<int>(seg2_dir.x, seg2_dir.y).rotateLeft();

        const Vector2<int> p1 = seg_mid;
        const Vector2<int> p2 = seg_mid + seg_norm;;
        const Vector2<int> p3 = seg2_mid;
        const Vector2<int> p4 = seg2_mid + seg2_norm;

        const int zaehler1 = (p4.x - p3.x)*(p1.y-p3.y) - (p4.y-p3.y)*(p1.x-p3.x);
        const int nenner1 = (p4.y-p3.y)*(p2.x-p1.x) - (p4.x-p3.x)*(p2.y-p1.y);
        double X1factor = zaehler1/(double)nenner1;

        const int zaehler2 = (p2.x-p1.x)*(p1.y-p3.y) - (p2.y-p1.y)*(p1.x-p3.x);
        const int nenner2 = (p4.y-p3.y)*(p2.x-p1.x) - (p4.x-p3.x)*(p2.y-p1.y);
        const double X2factor = zaehler2/(double)nenner2;

        const Vector2<int> t = p2 - p1;
        const Vector2<int> inter = p1 + Vector2<int>((int)(t.x*X1factor), (int)(t.y*X1factor));

        if(fabs(fabs(seg_norm.abs() * X1factor) - theFieldDimensions.centerCircleRadius+parameters.circleBiggerThanSpecified) > circleParams.maxRadiusError || fabs(fabs(seg2_norm.abs() * X2factor) - theFieldDimensions.centerCircleRadius+parameters.circleBiggerThanSpecified) > circleParams.maxRadiusError)
          continue;

        const int X1Sign = X1factor > 0 ? 1 : -1;
        const int X2Sign = X2factor > 0 ? 1 : -1;
        const Vector2<int> i1 = seg_mid + Vector2<int>(seg_norm).normalize((theFieldDimensions.centerCircleRadius+parameters.circleBiggerThanSpecified) * X1Sign);
        const Vector2<int> i2 = seg2_mid + Vector2<int>(seg2_norm).normalize((theFieldDimensions.centerCircleRadius+parameters.circleBiggerThanSpecified) * X2Sign);

        //CROSS("module:LinePerceptor:CircleSpots", inter.x, inter.y, 40, 20, Drawings::ps_solid, ColorClasses::red);
        CROSS("module:LinePerceptor:CircleSpots", i1.x, i1.y, 40, 20, Drawings::ps_solid, ColorClasses::blue);
        CROSS("module:LinePerceptor:CircleSpots", i2.x, i2.y, 40, 20, Drawings::ps_solid, ColorClasses::blue);
        LINE("module:LinePerceptor:CircleSpots", seg_mid.x, seg_mid.y, inter.x, inter.y, 10, Drawings::ps_solid, ColorClasses::orange);
        LINE("module:LinePerceptor:CircleSpots", seg2_mid.x, seg2_mid.y, inter.x, inter.y, 10, Drawings::ps_solid, ColorClasses::orange);
        spot.pos = i1;
        circleSpots.push_back(spot);
        spot.pos = i2;
        circleSpots.push_back(spot);
      }
    }
  }
  
  //Hough Transformation fuer (ganz) arme ;-)
  for(std::list<LinePercept::CircleSpot>::iterator spot_iter = circleSpots.begin(); spot_iter != circleSpots.end(); spot_iter++)
  {
    spot = *spot_iter;
    Vector2<int> center(0,0);
  
    std::vector<std::list<LinePercept::CircleSpot>::iterator> supporters;

    for(std::list<LinePercept::CircleSpot>::iterator other = circleSpots.begin(); other != circleSpots.end(); other++)
    {
      if((other->pos - spot.pos).squareAbs() < sqr(circleParams.maxSupporterDist))
      {
        supporters.push_back(other);
        center += other->pos;
      }
    }

    if((int)supporters.size() >= circleParams.minSupporters)
    {
      center /= supporters.size();

      //collect second round of supporters
      for(std::list<LinePercept::CircleSpot>::iterator other = circleSpots2.begin(); other != circleSpots2.end(); other++)
        if((other->pos - center).squareAbs() < sqr(circleParams.maxSupporterDist2))
          singleSegs.erase(other->iterator);
      
      circle.pos = center;
      circle.found = true;
      circle.lastSeen = theFrameInfo.time;
      CIRCLE("module:LinePerceptor:CircleSpots", circle.pos.x, circle.pos.y, circleParams.maxSupporterDist, 30, Drawings::ps_solid, ColorClasses::blue, Drawings::bs_null, ColorClasses::robotBlue);
      CIRCLE("module:LinePerceptor:CircleSpots", circle.pos.x, circle.pos.y, circleParams.maxSupporterDist2, 30, Drawings::ps_solid, ColorClasses::blue, Drawings::bs_null, ColorClasses::robotBlue);
      break;
    }
  }

  //a single segment is assumed to be a line if it's size is sufficent (and it's not part of the circle)
  std::vector<std::list<LinePercept::LineSegment>::iterator> toDelete;
  for(std::list<LinePercept::LineSegment>::iterator seg = singleSegs.begin(); seg != singleSegs.end(); seg++)
  {
    if((seg->p1 - seg->p2).squareAbs() > sqr(parameters.minLineSingleRegionLength))
    {
      LinePercept::Line l;
      l.d = seg->d;
      l.alpha = seg->alpha;
      l.segments.push_back(*seg);
      l.dead = false;
      l.midLine = false;
      lines.push_back(l);
      toDelete.push_back(seg);
    }
  }
  for(std::vector<std::list<LinePercept::LineSegment>::iterator>::iterator d = toDelete.begin(); d != toDelete.end(); d++)
    singleSegs.erase(*d);
  
}

MAKE_MODULE(LinePerceptor, Perception)

