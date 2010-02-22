/**
* @file PerceptValidityChecker.cpp
*
* Implements a class for checking perceptions before their use by the self locator.
* In fact, it is intended as a collection of different methods which are used 
* by different sensor models.
*
* @author <a href="mailto:Tim.Laue@dfki.de">Tim Laue</a>
*/

#include "Representations/Configuration/FieldDimensions.h"
#include "Tools/Settings.h"
#include "Tools/Debugging/Debugging.h"
#include "Tools/Streams/InStreams.h"
#include "Tools/Streams/OutStreams.h"
#include "PerceptValidityChecker.h"
#include <limits>


bool intersectLines(const Vector2<double>& p1, const Vector2<double>& v1,
                    const Vector2<double>& p2, const Vector2<double>& v2,
                    Vector2<double>& q)
{
  if(v1.y * v2.x == v1.x * v2.y)
  {
    return false;
  }
  q.x = p1.x + v1.x * (p1.y * v2.x - p2.y * v2.x + (-p1.x + p2.x) * v2.y)
    / (-v1.y * v2.x + v1.x * v2.y);
  q.y = p1.y + v1.y * (-p1.y * v2.x + p2.y * v2.x + (p1.x - p2.x) * v2.y)
    / (v1.y * v2.x - v1.x * v2.y);
  return true;
}


void PerceptValidityChecker::GoalNetTable::load()
{
  InBinaryFile stream(Global::getSettings().expandLocationFilename("goalNet.tab"));
  if(!stream.exists()) // In case of error
  {
    OUTPUT(idText, text, "ERROR: goalNet.tab cannot be opened. Computing new table, saving at destruction.");
    saveTableAtDestruction = true;
    compute();
  }
  else
  {
    int fieldLengthInFile, fieldWidthInFile, xResInFile, yResInFile, 
      angleResInFile, cellSizeInFile;
    stream >> fieldLengthInFile >> fieldWidthInFile >> xResInFile 
      >> yResInFile >> angleResInFile >> cellSizeInFile;
    if((fieldLengthInFile == halfFieldLength) && (fieldWidthInFile == halfFieldWidth) &&
       (xResInFile == GOAL_NET_TABLE_RES_X) && (yResInFile == GOAL_NET_TABLE_RES_Y) &&
       (angleResInFile == GOAL_NET_TABLE_RES_ANGLE) && (cellSizeInFile == GOAL_NET_TABLE_CELL_SIZE))
    {
      stream.read(maxValidDistances, sizeof(maxValidDistances));
    }
    else
    {
      OUTPUT(idText, text, "ERROR: goalNet.tab contains different field size and parameters than"
        <<" the current version of the software. Computing new table, saving at destruction.");
      saveTableAtDestruction = true;
      compute();
    }
  }
}

void PerceptValidityChecker::GoalNetTable::save()
{
  OutBinaryFile stream(Global::getSettings().expandLocationFilename("goalNet.tab"));
  int dummyXRes(GOAL_NET_TABLE_RES_X), dummyYRes(GOAL_NET_TABLE_RES_Y), 
    dummyAngleRes(GOAL_NET_TABLE_RES_ANGLE), dummyCellSize(GOAL_NET_TABLE_CELL_SIZE);
  stream << halfFieldLength << halfFieldWidth << dummyXRes << dummyYRes
    << dummyAngleRes << dummyCellSize;
  stream.write(maxValidDistances, sizeof(maxValidDistances));
}

void PerceptValidityChecker::GoalNetTable::compute()
{
  // Prepare list of goal segments to intersect rays with:
  typedef std::vector< std::pair< Vector2<double>,Vector2<double> > > GoalSegmentsList;
  GoalSegmentsList segList;
    // Opponent goal
  segList.push_back(std::make_pair(Vector2<double>(goalX+goalDepth,-goalWidth/2),Vector2<double>(goalX+goalDepth, goalWidth/2)));
  segList.push_back(std::make_pair(Vector2<double>(goalX,-goalWidth/2),Vector2<double>(goalX+goalDepth,-goalWidth/2)));
  segList.push_back(std::make_pair(Vector2<double>(goalX, goalWidth/2),Vector2<double>(goalX+goalDepth, goalWidth/2)));
    // Own goal
  segList.push_back(std::make_pair(Vector2<double>(-goalX-goalDepth,-goalWidth/2),Vector2<double>(-goalX-goalDepth, goalWidth/2)));
  segList.push_back(std::make_pair(Vector2<double>(-goalX,-goalWidth/2),Vector2<double>(-goalX-goalDepth,-goalWidth/2)));
  segList.push_back(std::make_pair(Vector2<double>(-goalX, goalWidth/2),Vector2<double>(-goalX-goalDepth, goalWidth/2)));
  
  // Iterate over all table elements:
  const int halfCell(GOAL_NET_TABLE_CELL_SIZE / 2);
  const double angleOffset = pi / GOAL_NET_TABLE_RES_ANGLE;
  for(int x=0; x<GOAL_NET_TABLE_RES_X; ++x)
  {
    const int xPos = x*GOAL_NET_TABLE_CELL_SIZE + halfCell - halfFieldLength;
    for(int y=0; y<GOAL_NET_TABLE_RES_Y; ++y)
    {
      const int yPos = y*GOAL_NET_TABLE_CELL_SIZE + halfCell  - halfFieldWidth;
      Vector2<double> lineStart(xPos, yPos);
      for(int a=0; a<GOAL_NET_TABLE_RES_ANGLE; ++a)
      {
        maxValidDistances[x][y][a] = std::numeric_limits<short>::max();
        double maxDistAsDouble(maxValidDistances[x][y][a]);
        const double angle = (pi2 / GOAL_NET_TABLE_RES_ANGLE) * a + angleOffset;
        Vector2<double> ray(std::numeric_limits<short>::max(),0);
        ray.rotate(angle);
        Vector2<double> lineEnd(lineStart);
        lineEnd += ray;
        GoalSegmentsList::const_iterator seg;
        for(seg = segList.begin(); seg != segList.end(); ++seg)
        {
          if(Geometry::checkIntersectionOfLines(lineStart, lineEnd, seg->first, seg->second))
          {
            Vector2<double> intersectionPosition;
            const Vector2<double> v1 = (lineEnd - lineStart);
            const Vector2<double> v2 = (seg->second - seg->first);
            intersectLines(lineStart, v1, seg->first, v2, intersectionPosition);
            const double distance = (lineStart - intersectionPosition).abs();
            if(distance < maxDistAsDouble)
            {
              maxDistAsDouble = distance;
              maxValidDistances[x][y][a] = static_cast<short>(distance);
            }
          }
        }
      }
    }
  }
}

short PerceptValidityChecker::GoalNetTable::getMaxDistanceForPoseAndAngle(
  const Pose2D& robotPose, double angle) const
{
  const double sectorSize(pi2 / GOAL_NET_TABLE_RES_ANGLE);
  const int xIndex = (static_cast<int>(robotPose.translation.x) + halfFieldLength) / GOAL_NET_TABLE_CELL_SIZE;
  const int yIndex = (static_cast<int>(robotPose.translation.y) + halfFieldWidth) / GOAL_NET_TABLE_CELL_SIZE;
  double totalAngle(robotPose.rotation + angle);
  while(totalAngle < 0.0)
    totalAngle += pi2;
  while(totalAngle >= pi2)
    totalAngle -= pi2;
  int angleIndex = static_cast<int>(totalAngle / sectorSize);
  return maxValidDistances[xIndex][yIndex][angleIndex];
}

PerceptValidityChecker::GoalNetTable::GoalNetTable(
  int halfLength, int halfWidth, int goalWidth, int goalDepth, int goalX):
halfFieldLength(halfLength), halfFieldWidth(halfWidth), goalWidth(goalWidth),
goalDepth(goalDepth), goalX(goalX), saveTableAtDestruction(false) 
{
  if((2*halfWidth > GOAL_NET_TABLE_CELL_SIZE * GOAL_NET_TABLE_RES_Y) ||
     (2*halfLength > GOAL_NET_TABLE_CELL_SIZE * GOAL_NET_TABLE_RES_X))
  {
    OUTPUT(idText, text, "Warning! Field size is not completely covered by GoalNetTable! Please adjust parameters!");
  }
  load();
}

PerceptValidityChecker::GoalNetTable::~GoalNetTable()
{
  if(saveTableAtDestruction)
  {
    OUTPUT(idText, text, "Automatic saving of goalNet.tab."); 
    save();
  }
}

void PerceptValidityChecker::GoalNetTable::draw()
{
  const int halfCell(GOAL_NET_TABLE_CELL_SIZE / 2);
  for(int x=0; x<GOAL_NET_TABLE_RES_X; ++x)
  {
    const int xPos = x*GOAL_NET_TABLE_CELL_SIZE + halfCell - halfFieldLength;
    for(int y=0; y<GOAL_NET_TABLE_RES_Y; ++y)
    {
      // Centers of cells:
      const int yPos = y*GOAL_NET_TABLE_CELL_SIZE + halfCell  - halfFieldWidth;
      CIRCLE("module:SelfLocator:perceptValidityChecker", xPos, yPos, GOAL_NET_TABLE_CELL_SIZE / 6,
        1, Drawings::ps_solid, ColorClasses::black, Drawings::bs_solid, ColorClasses::black);
      // Cycle through all directions over time (one per second):
      int direction = (SystemCall::getCurrentSystemTime() / 1000) % GOAL_NET_TABLE_RES_ANGLE;
      if(maxValidDistances[x][y][direction] < std::numeric_limits<short>::max())
      {
        double offset = pi / GOAL_NET_TABLE_RES_ANGLE;
        double angle = (pi2 / GOAL_NET_TABLE_RES_ANGLE) * direction + offset;
        Vector2<double> dummyLine(maxValidDistances[x][y][direction],0);
        dummyLine.rotate(angle);
        LINE("module:SelfLocator:perceptValidityChecker", xPos, yPos, xPos + dummyLine.x, yPos + dummyLine.y,
          10, Drawings::ps_solid, ColorClasses::red);
      }
    }
  }
}
