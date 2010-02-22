/**
* @file ObstacleModelProvider.cpp
*
* This file implements a module that provides information about occupied space in the robot's environment.
* It includes parts of the implementation of the PolygonLocalMapper module of the 
* autonomous wheelchair Rolland.
*
* @author <a href="mailto:Tim.Laue@dfki.de">Tim Laue</a>
* @author <a href="mailto:cman@tzi.de">Christian Mandel</a>
*/

#include "ObstacleModelProvider.h"
#include "Tools/Debugging/DebugDrawings.h"
#include "Tools/Math/Geometry.h"
#include <algorithm>


ObstacleModelProvider::ObstacleModelProvider():
lastTimePenalized(0),
lastLeftMeasurement(parameters.maxValidUSDist),
lastRightMeasurement(parameters.maxValidUSDist),
lastLeftMeasurementTime(0),
lastRightMeasurementTime(0),
polygonsComputed(false)
{
  InConfigFile stream(Global::getSettings().expandLocationFilename("obstacleModel.cfg"));
  if(stream.exists())
  {
    stream >> parameters.cellOccupiedThreshold;
  }
}

void ObstacleModelProvider::init()
{
  lastOdometry = theOdometryData;
  for(int i=0; i<CELLS_X*CELLS_Y; ++i)
  {
    cells[i].state = Cell::FREE;
  }
}

void ObstacleModelProvider::update(ObstacleModel& obstacleModel)
{
  MODIFY("parameters:ObstacleModelProvider", parameters);
  ageCellState();
  if((theRobotInfo.penalty == PENALTY_NONE) &&
     (theMotionRequest.motion != MotionRequest::specialAction) && 
     (theFrameInfo.getTimeSince(lastTimePenalized) > 3000))
  {
    moveGrid();
    occupyCells();
  }
  else if(theRobotInfo.penalty != PENALTY_NONE)
  {
    lastTimePenalized = theFrameInfo.time;
  }
  computeModel(obstacleModel);
#ifndef RELEASE
  draw();
#endif
}

void ObstacleModelProvider::computeModel(ObstacleModel& obstacleModel)
{
  const int numOfCells(CELLS_X*CELLS_Y);
  if(!polygonsComputed)
  {
    leftPoly[0]  = Vector2<double>(0,100);
    leftPoly[1]  = Vector2<double>(obstacleModel.maxValidDist,0);
    leftPoly[1].rotate(obstacleModel.outerLeftAngle);
    leftPoly[1] += leftPoly[0];
    leftPoly[2]  = Vector2<double>(obstacleModel.maxValidDist,0);
    leftPoly[2].rotate(obstacleModel.centerLeftAngle);
    leftPoly[2] += leftPoly[0];
    rightPoly[0] = Vector2<double>(0,-100);
    rightPoly[1] = Vector2<double>(obstacleModel.maxValidDist,0);
    rightPoly[1].rotate(obstacleModel.outerRightAngle);
    rightPoly[1] += rightPoly[0];
    rightPoly[2]  = Vector2<double>(obstacleModel.maxValidDist,0);
    rightPoly[2].rotate(obstacleModel.centerRightAngle);
    rightPoly[2] += rightPoly[0];
    centerLeftPoly[0] = Vector2<double>(0, 0);
    centerLeftPoly[1] = leftPoly[0];
    centerLeftPoly[2] = Vector2<double>(obstacleModel.maxValidDist,0);
    centerLeftPoly[2].rotate(obstacleModel.centerLeftAngle);
    centerLeftPoly[2] += centerLeftPoly[1];
    centerLeftPoly[3] = Vector2<double>(obstacleModel.maxValidDist,0);
    centerRightPoly[0] = rightPoly[0];
    centerRightPoly[1] = Vector2<double>(0, 0);
    centerRightPoly[2] = Vector2<double>(obstacleModel.maxValidDist,0);
    centerRightPoly[3] = Vector2<double>(obstacleModel.maxValidDist,0);
    centerRightPoly[3].rotate(obstacleModel.centerRightAngle);
    centerRightPoly[3] += centerRightPoly[0];
    polygonsComputed = true;
  }
  obstacleModel.distanceToCenterLeftObstacle = obstacleModel.maxValidDist + 1;
  obstacleModel.distanceToCenterRightObstacle = obstacleModel.maxValidDist + 1;
  obstacleModel.distanceToLeftObstacle = obstacleModel.maxValidDist + 1;
  obstacleModel.distanceToRightObstacle = obstacleModel.maxValidDist + 1;
  
  for(int i=0; i<numOfCells; ++i)
  {
    if(cells[i].state >= parameters.cellOccupiedThreshold)
    {
      Vector2<int> p = gridToWorld(Vector2<int>(i % CELLS_Y, i / CELLS_Y));
      CIRCLE("module:ObstacleModelProvider:backprojection",p.x,p.y,50,10,
        Drawings::ps_solid, ColorRGBA(255,0,0), Drawings::bs_null, ColorRGBA(255,0,0));
      Vector2<double> pDouble(p.x,p.y);
      
      if(Geometry::isPointInsideConvexPolygon(leftPoly, 3, pDouble))
      {
        const double dist = (pDouble - leftPoly[0]).abs();
        if(dist < obstacleModel.distanceToLeftObstacle)
          obstacleModel.distanceToLeftObstacle = dist;
      }
      else if(Geometry::isPointInsideConvexPolygon(rightPoly, 3, pDouble))
      {
        const double dist = (pDouble - rightPoly[0]).abs();
        if(dist < obstacleModel.distanceToRightObstacle)
          obstacleModel.distanceToRightObstacle = dist;
      }
      else if(Geometry::isPointInsideConvexPolygon(centerLeftPoly, 4, pDouble))
      {
        const double dist = pDouble.abs();
        if(dist < obstacleModel.distanceToCenterLeftObstacle)
          obstacleModel.distanceToCenterLeftObstacle = dist;
      }
      else if(Geometry::isPointInsideConvexPolygon(centerRightPoly, 4, pDouble))
      {
        const double dist = pDouble.abs();
        if(dist < obstacleModel.distanceToCenterRightObstacle)
          obstacleModel.distanceToCenterRightObstacle = dist;
      }
    }
  }
}

inline Vector2<int> ObstacleModelProvider::worldToGrid(const Vector2<int>& p) const
{
  Pose2D odoRotation(theOdometryData.rotation, 0.0, 0.0);
  Vector2<double> pDouble(p.x, p.y);
  pDouble = odoRotation*pDouble;
  pDouble /= CELL_SIZE;
  double moveX(CELLS_X/2);
  double moveY(CELLS_Y/2);
  pDouble += Vector2<double>(moveX, moveY);
  return Vector2<int>(static_cast<int>(pDouble.x), static_cast<int>(pDouble.y));
}

inline Vector2<int> ObstacleModelProvider::gridToWorld(const Vector2<int>& p) const
{
  Vector2<double> pDouble(p.x,p.y);
  double moveX(CELLS_X/2);
  double moveY(CELLS_Y/2);
  pDouble -= Vector2<double>(moveX, moveY);
  pDouble *= CELL_SIZE;
  Pose2D odoRotation(-theOdometryData.rotation, 0.0, 0.0);
  pDouble = odoRotation*pDouble;
  return Vector2<int>(static_cast<int>(pDouble.x), static_cast<int>(pDouble.y));
}

void ObstacleModelProvider::occupyCells()
{
  float m = theFilteredSensorData.data[SensorData::us];
  if(m > parameters.maxValidUSDist)
    return;
  if(m<parameters.minValidUSDist) 
    return;
 
  // Compute and draw relevant area:
  Vector2<double> measurement(m, 0.0);
  Vector2<double> measurementShort(m - 50.0, 0.0);
  Vector2<double> base, leftOfCone(measurement), rightOfCone(measurement),  
                  leftOfConeFree(measurementShort), rightOfConeFree(measurementShort); // clear this part of the grid
  if(theFilteredSensorData.usSensorType == SensorData::left)
  {
    base = parameters.usLeftPose.translation;
    leftOfCone.rotate(parameters.usOuterOpeningAngle);
    leftOfConeFree.rotate(parameters.usOuterOpeningAngle);
    rightOfCone.rotate(parameters.usInnerOpeningAngle);
    rightOfConeFree.rotate(parameters.usInnerOpeningAngle);

    leftOfCone = parameters.usLeftPose * leftOfCone;
    leftOfConeFree = parameters.usLeftPose * leftOfConeFree;
    rightOfCone = parameters.usLeftPose * rightOfCone;
    rightOfConeFree = parameters.usLeftPose * rightOfConeFree;

    lastLeftMeasurement = (int)m;
    lastLeftMeasurementTime = SystemCall::getCurrentSystemTime();
  }
  else if(theFilteredSensorData.usSensorType == SensorData::right)
  {
    base = parameters.usRightPose.translation;
    leftOfCone.rotate(-parameters.usInnerOpeningAngle);
    leftOfConeFree.rotate(-parameters.usInnerOpeningAngle);
    rightOfCone.rotate(-parameters.usOuterOpeningAngle);
    rightOfConeFree.rotate(-parameters.usOuterOpeningAngle);

    leftOfCone = parameters.usRightPose * leftOfCone;
    leftOfConeFree = parameters.usRightPose * leftOfConeFree;
    rightOfCone = parameters.usRightPose * rightOfCone;
    rightOfConeFree = parameters.usRightPose * rightOfConeFree;

    lastRightMeasurement = (int)m;
    lastRightMeasurementTime = SystemCall::getCurrentSystemTime();
  }
  else if(theFilteredSensorData.usSensorType == SensorData::leftToRight)
  {
    if(SystemCall::getTimeSince(lastLeftMeasurementTime) < 150 && lastLeftMeasurement < parameters.maxValidUSDist)
      return; // Don't fill cells, as leftToRight Measurements are only valid if left Sensor measured something

    base = parameters.usCenterPose.translation;
    leftOfCone.rotate(parameters.usCenterOpeningAngle);
    leftOfConeFree.rotate(parameters.usCenterOpeningAngle);
    rightOfCone.rotate(0);
    rightOfConeFree.rotate(0);
    leftOfCone = parameters.usCenterPose * leftOfCone;
    leftOfConeFree = parameters.usCenterPose * leftOfConeFree;
    rightOfCone = parameters.usCenterPose * rightOfCone;
    rightOfConeFree = parameters.usCenterPose * rightOfConeFree;
  }
  else if(theFilteredSensorData.usSensorType == SensorData::rightToLeft)
  {
    if(SystemCall::getTimeSince(lastRightMeasurementTime) < 150 && lastRightMeasurement < parameters.maxValidUSDist)
      return; // Don't fill cells, as rightToLeft Measurements are only valid if right Sensor measured something

    base = parameters.usCenterPose.translation;
    leftOfCone.rotate(0);
    leftOfConeFree.rotate(0);
    rightOfCone.rotate(-parameters.usCenterOpeningAngle);
    rightOfConeFree.rotate(-parameters.usCenterOpeningAngle);
    leftOfCone = parameters.usCenterPose * leftOfCone;
    leftOfConeFree = parameters.usCenterPose * leftOfConeFree;
    rightOfCone = parameters.usCenterPose * rightOfCone;
    rightOfConeFree = parameters.usCenterPose * rightOfConeFree;
  }

  Vector2<int> leftOfConeCells = worldToGrid(
    Vector2<int>(static_cast<int>(leftOfCone.x),static_cast<int>(leftOfCone.y)));
  Vector2<int> leftOfConeCellsFree = worldToGrid(
    Vector2<int>(static_cast<int>(leftOfConeFree.x),static_cast<int>(leftOfConeFree.y)));
  Vector2<int> rightOfConeCells = worldToGrid(
    Vector2<int>(static_cast<int>(rightOfCone.x),static_cast<int>(rightOfCone.y)));
  Vector2<int> rightOfConeCellsFree = worldToGrid(
    Vector2<int>(static_cast<int>(rightOfConeFree.x),static_cast<int>(rightOfConeFree.y)));
  LINE("module:ObstacleModelProvider:us", base.x, base.y, leftOfCone.x, leftOfCone.y,
    30, Drawings::ps_solid, ColorRGBA(255,0,0));
  LINE("module:ObstacleModelProvider:us", base.x, base.y, rightOfCone.x, rightOfCone.y,
    30, Drawings::ps_solid, ColorRGBA(255,0,0));
  LINE("module:ObstacleModelProvider:us", rightOfCone.x, rightOfCone.y, leftOfCone.x, leftOfCone.y,
    30, Drawings::ps_solid, ColorRGBA(255,0,0));

  // Free empty space until obstacle:
  polyPoints.clear();
  // Origin (sensor position):
  Vector2<int> p1(static_cast<int>(base.x), static_cast<int>(base.y));
  p1 = worldToGrid(p1);
  polyPoints.push_back(p1);
  // Left corner of "cone":
  polyPoints.push_back(Point(leftOfConeCellsFree, Point::NO_OBSTACLE));
  // Right corner of "cone":
  Vector2<double> ll(rightOfConeFree);
  double f1 = ll.abs(),
    f2 = f1 ? leftOfConeFree.abs() / f1 : 0;
  Vector2<double> gridPoint(ll);
  gridPoint *= f2;
  Vector2<int> gridPointInt(static_cast<int>(gridPoint.x), static_cast<int>(gridPoint.y));
  polyPoints.push_back(Point(worldToGrid(gridPointInt), polyPoints.back().flags));
  polyPoints.push_back(Point(rightOfConeCellsFree, Point::NO_OBSTACLE));
  // Sensor position again:
  polyPoints.push_back(p1);
  // Clip and fill:
  for(int j = 0; j < (int) polyPoints.size(); ++j)
    clipPointP2(p1, polyPoints[j]);
  fillScanBoundary();
  // Enter obstacle to grid:
  line(leftOfConeCells, rightOfConeCells);
}

void ObstacleModelProvider::ageCellState()
{
  for(int i=0; i<CELLS_X*CELLS_Y; ++i)
  {
    Cell& c = cells[i];
    if(c.state)
    {
      if(theFrameInfo.getTimeSince(c.lastUpdate) > parameters.cellFreeInterval)
      {
        c.state--;
        c.lastUpdate = theFrameInfo.time;
      }
    }
  }
}

void ObstacleModelProvider::moveGrid()
{
  accumulatedOdometry += theOdometryData - lastOdometry;
  lastOdometry = theOdometryData;
  // Move grid backwards in x direction (robot moves forward):
  if(accumulatedOdometry.translation.x >= CELL_SIZE)
  {
    accumulatedOdometry.translation.x -= CELL_SIZE;
    for(int y=0; y<CELLS_Y; ++y)
    {
      Cell* cStartNew = &cells[y*CELLS_X];
      Cell* cStartOld = cStartNew + 1;
      memmove(cStartNew, cStartOld, sizeof(Cell)*(CELLS_X-1));
      cStartNew[CELLS_X-1] = Cell();
    }
  }
  // Move grid forward in x direction (robot moves backwards):
  else if(accumulatedOdometry.translation.x <= -CELL_SIZE)
  {
    accumulatedOdometry.translation.x += CELL_SIZE;
    for(int y=0; y<CELLS_Y; ++y)
    {
      Cell* cStartOld = &cells[y*CELLS_X];
      Cell* cStartNew = cStartOld + 1;
      memmove(cStartNew, cStartOld, sizeof(Cell)*(CELLS_X-1));
      cStartOld[0] = Cell();
    }
  }
  // Move grid backwards in y direction (robot moves to the left):
  if(accumulatedOdometry.translation.y >= CELL_SIZE)
  {
    accumulatedOdometry.translation.y -= CELL_SIZE;
    Cell* cStartOld = &cells[CELLS_X];
    Cell* cStartNew = &cells[0];
    memmove(cStartNew, cStartOld, sizeof(Cell)*CELLS_X*(CELLS_Y-1));
    Cell* c = &cells[(CELLS_Y-1)*CELLS_X];
    for(int x=0; x<CELLS_X; ++x)
      c[x] = Cell();
  }
  // Move grid forward in y direction (robot moves to the right):
  else if(accumulatedOdometry.translation.y <= -CELL_SIZE)
  {
    accumulatedOdometry.translation.y += CELL_SIZE;
    Cell* cStartNew = &cells[CELLS_X];
    Cell* cStartOld = &cells[0];
    memmove(cStartNew, cStartOld, sizeof(Cell)*CELLS_X*(CELLS_Y-1));
    Cell* c = &cells[0];
    for(int x=0; x<CELLS_X; ++x)
      c[x] = Cell();
  }
}

void ObstacleModelProvider::clipPointP2(const Vector2<int>& p1, Point& p2) const
{
  if(p2.x < 0)
  {
    p2.y = p1.y + (p2.y - p1.y) * -p1.x / (p2.x - p1.x);
    p2.x = 0;
  }
  else if(p2.x >= CELLS_X)
  {
    p2.y = p1.y + (p2.y - p1.y) * (CELLS_X - 1 - p1.x) / (p2.x - p1.x);
    p2.x = CELLS_X - 1;
  }

  if(p2.y < 0)
  {
    p2.x = p1.x + (p2.x - p1.x) * -p1.y / (p2.y - p1.y);
    p2.y = 0;
  }
  else if(p2.y >= CELLS_Y)
  {
    p2.x = p1.x + (p2.x - p1.x) * (CELLS_Y - 1 - p1.y) / (p2.y - p1.y);
    p2.y = CELLS_Y - 1;
  }
}

void ObstacleModelProvider::fillScanBoundary()
{
  // generate boundary of polygon
  std::list<Line> lines;
  int j;
  for(j = 1; j < (int) polyPoints.size() - 1; ++j)
  {
    if(polyPoints[j - 1].y < polyPoints[j].y && polyPoints[j + 1].y < polyPoints[j].y)
      polyPoints[j].flags |= Point::PEAK;
    lines.push_back(Line(polyPoints[j - 1], polyPoints[j]));        
  }
  lines.push_back(Line(polyPoints[j - 1], polyPoints[j]));
  
  // sort the lines by their y coordinates
  lines.sort();

  // run through lines in increasing y order
  for(int y = (int) lines.front().a.y; !lines.empty(); ++y)
  {
    inter.clear();
    for(std::list<Line>::iterator it = lines.begin(); it != lines.end() && (*it).a.y <= y;)
      if((*it).b.y > y || ((*it).peak && (*it).b.y == y))
      { // line is not finished yet
        inter.push_back(int((*it).a.x));
        (*it).a.x += (*it).ils;
         ++it;
      }
      else if((*it).b.y == y && (*it).a.y == y && y != 0)
      { // horizontal line, is not drawn when y == 0, because overlapping lines clutter the map
        inter.push_back(int((*it).a.x));
        inter.push_back(int((*it).b.x));
        ++it;
      }
      else if(it != lines.begin())
      { // line is finished -> remove it
        std::list<Line>::iterator it_help = it;
        --it;
        lines.erase(it_help);
        ++it;
      }
      else
      { // special case: remove begin of list
        lines.erase(it);
        it = lines.begin();
      }

    // fill the line on an even/odd basis
    bool paint = false;
    int goalX = -1;
    std::sort(inter.begin(), inter.end());
    std::vector<int>::iterator iIt = inter.begin();
    if(iIt != inter.end())
      for(;;)
      {
        int startX = *iIt;
        if(++iIt == inter.end())
          break;
        paint ^= true;
        if(paint)
        {
          if(startX == goalX)
            ++startX;
          goalX = *iIt;
          Cell* cell = &cells[y*CELLS_X+startX];
          for(int x = startX; x <= goalX; ++x)
          {
            if((*cell).state > 0) 
              (*cell).state--;
            (*cell).lastUpdate = theFrameInfo.time;
            ++cell;
          }
        }
      }
  }
}

void ObstacleModelProvider::line(Vector2<int>& start, Vector2<int>& end) 
{
  Vector2<int> diff = end - start,
               inc(diff.x > 0 ? 1 : -1, (diff.y > 0 ? 1 : -1) * (&cells[CELLS_X] - &cells[0])),
               absDiff(abs(diff.x), abs(diff.y));
  Cell* p = &cells[start.y*CELLS_X + start.x];

  if(absDiff.y < absDiff.x)
  {
    int error = -absDiff.x;
    for(int i = 0; i <= absDiff.x; ++i)
    {
      if(p->state < Cell::MAX_VALUE)
        p->state++;
      p->lastUpdate = theFrameInfo.time;
      p += inc.x;
      error += 2 * absDiff.y;
      if(error > 0)
      {
        p += inc.y;
        error -= 2 * absDiff.x;
      }
    }
  }
  else
  {
    int error = -absDiff.y;
    for(int i = 0; i <= absDiff.y; ++i)
    {
      if(p->state < Cell::MAX_VALUE)
        p->state++;
      p->lastUpdate = theFrameInfo.time;
      p += inc.y;
      error += 2 * absDiff.x;
      if(error > 0)
      {
        p += inc.x;
        error -= 2 * absDiff.y;
      }
    }
  }
}

void ObstacleModelProvider::draw() const
{
  // Test drawing:
  DECLARE_DEBUG_DRAWING("module:ObstacleModelProvider:us", "drawingOnField");
  DECLARE_DEBUG_DRAWING("module:ObstacleModelProvider:grid", "drawingOnField");
  DECLARE_DEBUG_DRAWING("module:ObstacleModelProvider:backprojection", "drawingOnField");
  DECLARE_DEBUG_DRAWING("module:ObstacleModelProvider:pose", "drawingOnField");
  
  COMPLEX_DRAWING("module:ObstacleModelProvider:grid",
    unsigned char colorOccupiedStep(255/Cell::MAX_VALUE);
    ColorRGBA baseColor(200,200,255);
    const int numOfCells(CELLS_X*CELLS_Y);
    unsigned char cellsForDrawing[CELLS_Y*CELLS_X];
    for(int i=0; i<numOfCells; ++i)
      cellsForDrawing[i] = colorOccupiedStep*cells[i].state;
    GRID_MONO("module:ObstacleModelProvider:grid", CELL_SIZE*2, CELLS_X, CELLS_Y, baseColor, cellsForDrawing);
    int gridWidth(CELLS_X*CELL_SIZE*2);
    int gridHeight(CELLS_Y*CELL_SIZE*2);
    RECTANGLE("module:ObstacleModelProvider:grid", -gridWidth/2, -gridHeight/2, gridWidth/2, gridHeight/2,
      20, Drawings::ps_solid, ColorRGBA(0,0,100));
  );

  COMPLEX_DRAWING("module:ObstacleModelProvider:pose",
    Vector2<double> bodyPoints[4] = {Vector2<double>(55*2,90*2),
                                     Vector2<double>(-55*2,90*2),
                                     Vector2<double>(-55*2,-90*2),
                                     Vector2<double>(55*2,-90*2)};
    for(int i=0; i<4; i++)
      bodyPoints[i] = Geometry::rotate(bodyPoints[i], theOdometryData.rotation);
    Vector2<double> dirVec(200,0*2);
    dirVec.rotate(theOdometryData.rotation);
    LINE("module:ObstacleModelProvider:pose", 0, 0, dirVec.x, dirVec.y, 
      20, Drawings::ps_solid, ColorClasses::white);
    POLYGON("module:ObstacleModelProvider:pose", 4, bodyPoints, 20, Drawings::ps_solid, 
      ColorClasses::green, Drawings::bs_solid, ColorClasses::white);
    CIRCLE("module:ObstacleModelProvider:pose", 0, 0, 42*2, 1, 
      Drawings::ps_solid, ColorClasses::green, Drawings::bs_solid, ColorClasses::green);
  );
}


MAKE_MODULE(ObstacleModelProvider, Modeling)
