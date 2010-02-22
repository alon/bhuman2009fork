/**
* @file FieldDimensions.cpp
*
* Some useful functions regarding field dimensions.
*
* @author Max Risler
*/

#include "FieldDimensions.h"
#include "Tools/Debugging/Modify.h"
#include "Tools/Settings.h"
#include "Tools/Streams/InStreams.h"
#include <limits>

FieldDimensions::FieldDimensions()
: Boundary<double>(0,0)
{
}

void FieldDimensions::load()
{
  int* const valuesInt[]={
    &xPosOpponentFieldBorder, &xPosOpponentGoal, &xPosOpponentGoalpost,
    &xPosOpponentGroundline, &xPosOpponentSideCorner, &xPosOpponentPenaltyArea, &xPosHalfWayLine,
    &xPosOwnPenaltyArea, &xPosOwnSideCorner, &xPosOwnGroundline, &xPosOwnGoalpost,
    &xPosOwnGoal, &xPosOwnFieldBorder,
    &yPosLeftFieldBorder, &yPosLeftSideline, &yPosLeftGroundline,
    &yPosLeftPenaltyArea, &yPosLeftGoal, &yPosCenterGoal, &yPosRightGoal,
    &yPosRightPenaltyArea, &yPosRightGroundline, &yPosRightSideline, &yPosRightFieldBorder,
    &centerCircleRadius, &goalHeight, &fieldLinesWidth, &goalPostRadius, 
    &xPosThrowInPointOpponentHalf, &xPosThrowInPointCenter, 
    &xPosThrowInPointOwnHalf, &ballRadius, &ballFriction
  };
  const char* const namesInt[]={
    "xPosOpponentFieldBorder", "xPosOpponentGoal", "xPosOpponentGoalpost",
    "xPosOpponentGroundline", "xPosOpponentSideCorner", "xPosOpponentPenaltyArea", "xPosHalfWayLine",
    "xPosOwnPenaltyArea", "xPosOwnSideCorner", "xPosOwnGroundline", "xPosOwnGoalpost",
    "xPosOwnGoal", "xPosOwnFieldBorder",
    "yPosLeftFieldBorder", "yPosLeftSideline", "yPosLeftGroundline",
    "yPosLeftPenaltyArea", "yPosLeftGoal", "yPosCenterGoal", "yPosRightGoal",
    "yPosRightPenaltyArea", "yPosRightGroundline", "yPosRightSideline", "yPosRightFieldBorder",
    "centerCircleRadius",
    "goalHeight", "fieldLinesWidth", "goalPostRadius",
    "xPosThrowInPointOpponentHalf", "xPosThrowInPointCenter", 
    "xPosThrowInPointOwnHalf", "ballRadius", "ballFriction"
  };

  const int numOfValuesInt = sizeof(valuesInt)/sizeof(int*);
  ASSERT(sizeof(namesInt)/sizeof(char*) == numOfValuesInt);

  bool initializedInt[numOfValuesInt];

  for (int i = 0; i < numOfValuesInt; initializedInt[i++] = false);

  readDimensions(valuesInt, namesInt, initializedInt, numOfValuesInt);
  readLines(valuesInt, namesInt, initializedInt, numOfValuesInt);
  readCorners(valuesInt, namesInt, initializedInt, numOfValuesInt);

  add(Vector2<double>(xPosOpponentFieldBorder,yPosLeftFieldBorder));
  add(Vector2<double>(xPosOwnFieldBorder,yPosRightFieldBorder));
}

void FieldDimensions::readDimensions(int* const* valuesInt, const char* const* namesInt, bool* initializedInt, int numOfValuesInt)
{
  InConfigFile file(Global::getSettings().expandLocationFilename("field.cfg"), "dimensions");
  std::string buf;
  while(file.exists() && !file.eof())
  {
    file >> buf;
    int i;
    for (i = 0; i < numOfValuesInt; i++)
      if(buf == namesInt[i])
      {
        if (initializedInt[i])
        {
          ASSERT(false);
        }
        file >> buf;
        *valuesInt[i] = readValue(buf, valuesInt, namesInt, initializedInt, numOfValuesInt);
        initializedInt[i] = true;
        break;
      }
    ASSERT(i < numOfValuesInt);
  }
  int i;
  for (i = 0; i < numOfValuesInt; i++)
    if (!initializedInt[i])
    {
      ASSERT(false);
    }
}

void FieldDimensions::readLines(const int* const* valuesInt, const char* const* namesInt, const bool* initializedInt, int numOfValuesInt)
{
  LinesTable* linesTables[] = {&carpetBorder, &fieldBorder, &fieldLines};
  const char* linesTableNames[] = {"carpetBorder", "fieldBorder", "fieldLines"};
  const int numOfLinesTables = sizeof(linesTables)/sizeof(LinesTable*);
  ASSERT(sizeof(linesTableNames)/sizeof(char*) == numOfLinesTables);

  int i;
  for(i = 0; i < numOfLinesTables; i++)
  {
    InConfigFile file(Global::getSettings().expandLocationFilename("field.cfg"), linesTableNames[i]);

    while(file.exists() && !file.eof())
    {
      std::string buf;
      file >> buf;
      if (buf == "(")
      {
        Vector2<double> p1,p2;
        file >> buf;
        p1.x = readValue(buf, valuesInt, namesInt, initializedInt, numOfValuesInt);
        file >> buf;
        p1.y = readValue(buf, valuesInt, namesInt, initializedInt, numOfValuesInt);
        file >> buf;
        if (buf != ")")
        {
          ASSERT(false);
        }
        file >> buf;
        if (buf != "-")
        {
          ASSERT(false);
        }
        file >> buf;
        if (buf != "(")
        {
          ASSERT(false);
        }
        file >> buf;
        p2.x = readValue(buf, valuesInt, namesInt, initializedInt, numOfValuesInt);
        file >> buf;
        p2.y = readValue(buf, valuesInt, namesInt, initializedInt, numOfValuesInt);
        file >> buf;
        if (buf != ")")
        {
          ASSERT(false);
        }

        linesTables[i]->push(p1, p2);
      } 
      else if (buf == "circle")
      {
        Vector2<double> center;
        double radius;
        int numOfSegments;
        file >> buf;
        if (buf != "(")
        {
          ASSERT(false);
        }
        file >> buf;
        center.x = readValue(buf, valuesInt, namesInt, initializedInt, numOfValuesInt);
        file >> buf;
        center.y = readValue(buf, valuesInt, namesInt, initializedInt, numOfValuesInt);
        file >> buf;
        radius = readValue(buf, valuesInt, namesInt, initializedInt, numOfValuesInt);
        file >> numOfSegments;
        file >> buf;
        if (buf != ")")
        {
          ASSERT(false);
        }

        linesTables[i]->pushCircle(center, radius, numOfSegments);
      }
      else if (buf == "quartercircle")
      {
        Vector2<double> center;
        double radius;
        int numOfSegments;
        int angle;
        file >> buf;
        if (buf != "(")
        {
          ASSERT(false);
        }
        file >> buf;
        center.x = readValue(buf, valuesInt, namesInt, initializedInt, numOfValuesInt);
        file >> buf;
        center.y = readValue(buf, valuesInt, namesInt, initializedInt, numOfValuesInt);
        file >> buf;
        radius = readValue(buf, valuesInt, namesInt, initializedInt, numOfValuesInt);
        file >> numOfSegments;
        file >> angle;
        file >> buf;
        if (buf != ")")
        {
          ASSERT(false);
        }

        linesTables[i]->pushQuarterCircle(center, radius, numOfSegments, angle);
      }
      else
      {
        ASSERT(false);
      }
    }
  }
}

void FieldDimensions::readCorners(const int* const* valuesInt, const char* const* namesInt, const bool* initializedInt, int numOfValuesInt)
{
  for(int i = 0; i < numOfCornerClasses; ++i)
  {
    std::string buf;
    InConfigFile file(Global::getSettings().expandLocationFilename("field.cfg"), getCornerClassName((CornerClass) i));
    while(file.exists() && !file.eof())
    {
      Vector2<int> corner;
      file >> buf;
      corner.x = readValue(buf, valuesInt, namesInt, initializedInt, numOfValuesInt);
      file >> buf;
      corner.y = readValue(buf, valuesInt, namesInt, initializedInt, numOfValuesInt);
      corners[i].push_back(corner);
    }
  }
}

int FieldDimensions::readValue(const std::string& buf, const int* const* values, const char* const* names, const bool* initialized, int numOfValues) const
{
  std::string n = buf;
  bool neg = false;
  if (buf[0] == '-')
  {
    neg=true;
    n = n.substr(1);
  }
  for (int j = 0; j < numOfValues; j++)
    if (n == names[j])
    {
      if (!initialized[j])
      {
        ASSERT(false);
      }
      return (neg) ? (-*values[j]) : (*values[j]);
    }
  return (int)strtol(buf.c_str(),(char**)NULL,0);
}

Pose2D FieldDimensions::randomPoseOnField() const
{ 
  Pose2D pose;
  do
    pose = Pose2D::random(x,y,Range<double>(-pi,pi));
  while(!isInsideField(pose.translation));
  return pose;
}

Pose2D FieldDimensions::randomPoseOnCarpet() const
{ 
  Pose2D pose;
  do
    pose = Pose2D::random(x,y,Range<double>(-pi,pi));
  while(!isInsideCarpet(pose.translation));
  return pose;
}

void FieldDimensions::draw() const
{
  drawLines();
  drawCorners();
}

void FieldDimensions::drawLines() const
{
  DECLARE_DEBUG_DRAWING("field lines", "drawingOnField");
  COMPLEX_DRAWING("field lines",
    int lineWidth = fieldLinesWidth;
    ColorRGBA lineColor(192,192,192);

    for(std::vector<LinesTable::Line>::const_iterator i = fieldLines.lines.begin(); i != fieldLines.lines.end(); ++i)
    {
      Vector2<double> source = i->corner.translation;
      Pose2D target(i->corner);
      target.translate(i->length, 0);
      LINE("field lines", source.x, source.y, target.translation.x, target.translation.y, lineWidth, Drawings::ps_solid, lineColor);
    }
  );
}

void FieldDimensions::drawPolygons(RoboCup::uint32 ownColor) const
{
  DECLARE_DEBUG_DRAWING("field polygons", "drawingOnField");
  COMPLEX_DRAWING("field polygons",
    Vector2<double>* points = new Vector2<double>[carpetBorder.lines.size()];
    for (unsigned i = 0; i < carpetBorder.lines.size(); ++i)
    {  
      points[i] = carpetBorder.lines[i].corner.translation;
    }
    POLYGON("field polygons", (int) carpetBorder.lines.size(), points, 
            1, Drawings::ps_solid, ColorRGBA(0, 180, 0), Drawings::bs_solid, ColorRGBA(0, 140, 0));
    delete [] points;

    ColorRGBA own = ownColor == TEAM_BLUE ? ColorRGBA(100,100,255) : ColorRGBA(ColorClasses::yellow);
    ColorRGBA opp = ownColor != TEAM_BLUE ? ColorRGBA(100,100,255) : ColorRGBA(ColorClasses::yellow);

    CIRCLE("field polygons", xPosOwnGoalpost, yPosLeftGoal, 50, 1, Drawings::ps_solid,
      own, Drawings::bs_solid, own);
    CIRCLE("field polygons", xPosOwnGoalpost, yPosRightGoal, 50, 1, Drawings::ps_solid,
      own, Drawings::bs_solid, own);
    own.a = 100;
    Vector2<double> goal[4];
    goal[0] = Vector2<double>(xPosOwnGoalpost, yPosLeftGoal);
    goal[1] = Vector2<double>(xPosOwnGoalpost, yPosRightGoal);
    goal[2] = Vector2<double>(xPosOwnGoal, yPosRightGoal);
    goal[3] = Vector2<double>(xPosOwnGoal, yPosLeftGoal);
    POLYGON("field polygons", 4, goal, 1, Drawings::ps_solid, own, Drawings::bs_solid, own);
    
    CIRCLE("field polygons", xPosOpponentGoalpost, yPosLeftGoal, 50, 1, Drawings::ps_solid,
      opp, Drawings::bs_solid, opp);
    CIRCLE("field polygons", xPosOpponentGoalpost, yPosRightGoal, 50, 1, Drawings::ps_solid,
      opp, Drawings::bs_solid, opp);
    opp.a = 100;
    goal[0] = Vector2<double>(xPosOpponentGoalpost, yPosLeftGoal);
    goal[1] = Vector2<double>(xPosOpponentGoalpost, yPosRightGoal); 
    goal[2] = Vector2<double>(xPosOpponentGoal, yPosRightGoal); 
    goal[3] = Vector2<double>(xPosOpponentGoal, yPosLeftGoal);
    POLYGON("field polygons", 4, goal, 1, Drawings::ps_solid, opp, Drawings::bs_solid, opp);
  );
}

void FieldDimensions::drawCorners() const
{
  DECLARE_DEBUG_DRAWING("field corners", "drawingOnField");
  CornerClass c = xCorner;
  MODIFY_ENUM("fieldDimensions:cornerClass", c, numOfCornerClasses, &getCornerClassName);
  COMPLEX_DRAWING("field corners",
    for(CornersTable::const_iterator i = corners[c].begin(); i != corners[c].end(); ++i)
      LARGE_DOT("field corners", i->x, i->y, ColorRGBA(255,255,255), ColorRGBA(255,255,255));
  );
}

void FieldDimensions::LinesTable::push(const Pose2D& p, double l)
{
  LinesTable::Line line;
  line.corner = p;
  line.length = l;
  lines.push_back(line);
}

void FieldDimensions::LinesTable::push(const Vector2<double>& s, const Vector2<double>& e)
{
  Vector2<double> d = e - s;
  push(Pose2D(d.angle(), s), d.abs());
}

void FieldDimensions::LinesTable::pushCircle(const Vector2<double>& center, double radius, int numOfSegments)
{
  Vector2<double> p1, p2;
  for (double a = 0; a <= pi_4; a += pi2/numOfSegments)
  {
    p1 = Vector2<double>(sin(a), cos(a)) * radius;
    if (a > 0)
    {
      push(center + p1, center + p2);
      push(center + Vector2<double>(p1.x,-p1.y), center + Vector2<double>(p2.x,-p2.y));
      push(center + Vector2<double>(-p1.x,p1.y), center + Vector2<double>(-p2.x,p2.y));
      push(center - p1, center - p2);
      push(center + Vector2<double>(p1.y,p1.x), center + Vector2<double>(p2.y,p2.x));
      push(center + Vector2<double>(p1.y,-p1.x), center + Vector2<double>(p2.y,-p2.x));
      push(center + Vector2<double>(-p1.y,p1.x), center + Vector2<double>(-p2.y,p2.x));
      push(center + Vector2<double>(-p1.y,-p1.x), center + Vector2<double>(-p2.y,-p2.x));
    }
    p2 = p1;
  }
}

void FieldDimensions::LinesTable::pushQuarterCircle(const Vector2<double>& center, double radius, int numOfSegments, int angle)
{
  Vector2<double> p1, p2;
  for (double a = 0; a <= pi_4; a += pi2/numOfSegments)
  {
    p1 = Vector2<double>(sin(a), cos(a)) * radius;
    if (a > 0)
    {
      switch(angle)
      {
        case 0:
          push(center + p1, center + p2);
          push(center + Vector2<double>(p1.y,p1.x), center + Vector2<double>(p2.y,p2.x));
          break;
        case 1:
          push(center + Vector2<double>(-p1.x,p1.y), center + Vector2<double>(-p2.x,p2.y));
          push(center + Vector2<double>(-p1.y,p1.x), center + Vector2<double>(-p2.y,p2.x));
          break;
        case 2:
          push(center - p1, center - p2);
          push(center + Vector2<double>(-p1.y,-p1.x), center + Vector2<double>(-p2.y,-p2.x));
          break;
        case 3:
          push(center + Vector2<double>(p1.x,-p1.y), center + Vector2<double>(p2.x,-p2.y));
          push(center + Vector2<double>(p1.y,-p1.x), center + Vector2<double>(p2.y,-p2.x));
          break;
      }
    }
    p2 = p1;
  }
}

void FieldDimensions::LinesTable::doubleSided(double width, const FieldDimensions::LinesTable& single)
{
  for(std::vector<Line>::const_iterator i = single.lines.begin(); i != single.lines.end(); ++i)
  {
    push(i->corner + 
          Pose2D(Vector2<double>(-width/2,width/2)), 
          i->length + width);
    push(i->corner + 
          Pose2D(pi, Vector2<double>(i->length,0)) + 
          Pose2D(Vector2<double>(-width/2,width/2)), 
          i->length + width);
  }
}

bool FieldDimensions::LinesTable::isInside(const Vector2<double>& v) const
{
  //note:
  //This function assumes that the point (0,0) is inside and
  //that for any point inside the area the line to (0,0) belongs to the area too.

  Geometry::Line testLine(v, -v);
  for(std::vector<Line>::const_iterator i = lines.begin(); i != lines.end(); ++i)
  {
    double factor;
    Geometry::Line border(i->corner, i->length);
    if (Geometry::getIntersectionOfRaysFactor(border, testLine, factor))
      return false;
  }
  return true;
}

double FieldDimensions::LinesTable::clip(Vector2<double>& v) const
{
  if(isInside(v))
    return 0;
  else
  {
    Vector2<double> old = v,
                          v2;
    double minDist = 100000;
    for(std::vector<Line>::const_iterator i = lines.begin(); i != lines.end(); ++i)
    {
      Vector2<double> diff = (Pose2D(old) - i->corner).translation;
      if(diff.x < 0)
        v2 = i->corner.translation;

      else if(diff.x > i->length)
        v2 = (i->corner + Pose2D(Vector2<double>(i->length,0))).translation;
      else
        v2 = (i->corner + Pose2D(Vector2<double>(diff.x,0))).translation;
      double dist = (old - v2).abs();
      if(minDist > dist)
      {
        minDist = dist;
        v = v2;
      }
    }
    return (v - old).abs();
  }
}

bool FieldDimensions::LinesTable::getClosestPoint(Vector2<double>& vMin, const Pose2D& p, int numberOfRotations, double minLength) const
{
  int trueNumberOfRotations = numberOfRotations;
  if(numberOfRotations == 2)
    numberOfRotations = 4;

  // target angle -> target index
  double r = p.rotation / pi2 * numberOfRotations + 0.5;
  if(r < 0)
    r += numberOfRotations;
  int targetRot = int(r);
  ASSERT(targetRot >= 0 && targetRot < numberOfRotations);
  targetRot %= trueNumberOfRotations;
  Vector2<double> v2;
  double minDist = 100000;
  for(std::vector<Line>::const_iterator i = lines.begin(); i != lines.end(); ++i)
    if(i->length >= minLength)
    {
      // angle -> index
      double r = (i->corner.rotation + pi_2) / pi2 * numberOfRotations + 0.5;
      if(r < 0)
        r += numberOfRotations;
      else if(r >= numberOfRotations)
        r -= numberOfRotations;
      int rot = int(r);
      ASSERT(rot >= 0 && rot < numberOfRotations);
      rot %= trueNumberOfRotations;

      // index must be target index
      if(rot == targetRot)
      {
        Vector2<double> diff = (p - i->corner).translation;
        if(diff.x < 0)
          v2 = i->corner.translation;
        else if(diff.x > i->length)
          v2 = (i->corner + Pose2D(Vector2<double>(i->length,0))).translation;
        else
          v2 = (i->corner + Pose2D(Vector2<double>(diff.x,0))).translation;
        Vector2<double> vDiff = v2 - p.translation;
        double dist = vDiff.abs();
        if(minDist > dist)
        {
          minDist = dist;
          vMin = v2;
        }
      }
    }
  return (minDist<100000);
}

double FieldDimensions::LinesTable::getDistance(const Pose2D& pose) const
{
  double minDist = 100000;
  for(std::vector<Line>::const_iterator i = lines.begin(); i != lines.end(); ++i)
  {
    Vector2<double> v1 = (i->corner - pose).translation,
                    v2 = (i->corner + Pose2D(Vector2<double>(i->length,0)) 
                          - pose).translation;
    if(v1.y < 0 && v2.y > 0)
    {
      double dist = v1.x + (v2.x - v1.x) * -v1.y / (v2.y - v1.y);
      if(dist >= 0 && dist < minDist)
        minDist = dist;
    }
  }
  return minDist == 100000 ? -1 : minDist;
}

void FieldDimensions::LinesTable::draw(const ColorRGBA& color, bool drawNormals) const
{
  for(std::vector<Line>::const_iterator i = lines.begin(); i != lines.end(); ++i)
  {
    Vector2<double> s = i->corner.translation;
    Vector2<double> p = (i->corner + Pose2D(Vector2<double>(i->length,0))).translation;
    LINE("field lines",
       (int) s.x,
       (int) s.y,
       (int) p.x,
       (int) p.y,
       1, Drawings::ps_solid, color);
    if(drawNormals)
    {
      p = (i->corner + Pose2D(Vector2<double>(i->length/2,0))).translation;
      Vector2<double> p2 = (i->corner + Pose2D(Vector2<double>(i->length/2,100))).translation;
      LINE("field lines",
         (int) p2.x,
         (int) p2.y,
         (int) p.x,
         (int) p.y,
         1, Drawings::ps_solid, color);    
    }
  }
}

const Vector2<int>& FieldDimensions::CornersTable::getClosest(const Vector2<int>& p) const
{
  ASSERT(!empty());
  int maxDistance2 = std::numeric_limits<int>().max();
  const Vector2<int>* closest = 0;
  for(const_iterator i = begin(); i != end(); ++i)
  {
    Vector2<int> diff = p - *i;
    int distance2 = diff * diff;
    if(maxDistance2 > distance2)
    {
      maxDistance2 = distance2;
      closest = &*i;
    }
  }
  return *closest;
}

const Vector2<double> FieldDimensions::CornersTable::getClosest(const Vector2<double>& p) const
{
  Vector2<int> closest = getClosest(Vector2<int>((int) p.x, (int) p.y));
  return Vector2<double>(closest.x, closest.y);
}

const char* FieldDimensions::getCornerClassName(FieldDimensions::CornerClass cornerClass)
{
  switch(cornerClass)
  {
  case xCorner: return "xCorner";
  case tCorner0: return "tCorner0";
  case tCorner90: return "tCorner90";
  case tCorner180: return "tCorner180";
  case tCorner270: return "tCorner270";
  case lCorner0: return "lCorner0";
  case lCorner90: return "lCorner90";
  case lCorner180: return "lCorner180";
  case lCorner270: return "lCorner270";
  default: return "UNKNOWN";
  }
}

void FieldDimensions::serialize(In *in, Out *out)
{
  STREAM_REGISTER_BEGIN();
  STREAM(xPosOpponentFieldBorder);
  STREAM(xPosOpponentGoal);
  STREAM(xPosOpponentGoalpost);
  STREAM(xPosOpponentGroundline);
  STREAM(xPosOpponentSideCorner);
  STREAM(xPosOpponentPenaltyArea);
  STREAM(xPosHalfWayLine);
  STREAM(xPosOwnPenaltyArea);
  STREAM(xPosOwnSideCorner);
  STREAM(xPosOwnGroundline);
  STREAM(xPosOwnGoalpost);
  STREAM(xPosOwnGoal);
  STREAM(xPosOwnFieldBorder);
  STREAM(yPosLeftFieldBorder);
  STREAM(yPosLeftSideline);
  STREAM(yPosLeftGroundline);
  STREAM(yPosLeftPenaltyArea);
  STREAM(yPosLeftGoal);
  STREAM(yPosCenterGoal);
  STREAM(yPosRightGoal);
  STREAM(yPosRightPenaltyArea);
  STREAM(yPosRightGroundline);
  STREAM(yPosRightSideline);
  STREAM(yPosRightFieldBorder);
  STREAM(centerCircleRadius);
  STREAM(goalHeight);
  STREAM(ballRadius);
  STREAM(ballFriction);
  STREAM(fieldLinesWidth);
  STREAM(xPosThrowInPointOpponentHalf);
  STREAM(xPosThrowInPointCenter);
  STREAM(xPosThrowInPointOwnHalf);
  STREAM(fieldLines);
  STREAM(carpetBorder);
  STREAM(fieldBorder);
  STREAM_ARRAY(corners);
  STREAM_REGISTER_FINISH();
}


