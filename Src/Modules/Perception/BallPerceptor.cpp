/**
* @file BallPerceptor.cpp
* This file declares a module that provides the ball percept based on the
* biggest orange segment found by the PointsPerceptor. It is based on the
* BallSpecialist in GT2005.
* @author <A href="mailto:juengel@informatik.hu-berlin.de">Matthias Juengel</A>
* @author Max Risler
* @author <A href="mailto:brunn@sim.tu-darmstadt.de">Ronnie Brunn</A>
* @author <A href="mailto:mkunz@sim.tu-darmstadt.de">Michael Kunz</A>
* @author <a href="mailto:walter.nistico@uni-dortmund.de">Walter Nistico</a>
* @author <a href="mailto:judy@informatik.uni-bremen.de">Judith Müller</a>
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
* @author <a href="mailto:ingsie@informatik.uni-bremen.de">Ingo Sieverdingbeck</a>
* @author <a href="mailto:jworch@informatik.uni-bremen.de">Jan-Hendrik Worch</a>
*/

#include "BallPerceptor.h"
#include "Tools/Streams/InStreams.h"
#include "Tools/Debugging/DebugDrawings.h"
#include "Tools/Math/Geometry.h"
#include "Tools/Math/Matrix_nxn.h"
#include "Tools/ImageProcessing/BresenhamLineScan.h"


BallPerceptor::BallPerceptor():
ballPointsAtImageBorder(false),
calculatedBallRadius(0)
{
  InConfigFile stream("ballPerceptor.cfg", "default");
  ASSERT(stream.exists());
  stream >> parameters;
}

void BallPerceptor::update(BallPercept& ballPercept)
{
  calculatedBallRadius = 0;
  ballPointsAtImageBorder = false;
  MODIFY("parameters:BallPerceptor",parameters);
  ExtendedBallPercepts testPercepts;
  update(testPercepts);
  ExtendedBallPercept bestPercept;
  if (testPercepts.extendedBallPercepts.size() >= 1)
  {
    bestPercept = testPercepts.extendedBallPercepts[0];
    for (unsigned int i = 1; i < testPercepts.extendedBallPercepts.size(); i++)
    {
      if (testPercepts.extendedBallPercepts[i].isValidBallPercept)
      {
        if (testPercepts.extendedBallPercepts[i].validity > bestPercept.validity)
        {
          bestPercept = testPercepts.extendedBallPercepts[i];
        }
      }
    }
    ballPercept.ballWasSeen = bestPercept.isValidBallPercept;
    ballPercept.positionInImage = bestPercept.positionInImage;
    ballPercept.radiusInImage = bestPercept.radiusInImage;
    ballPercept.relativePositionOnField = bestPercept.relativePositionOnField;
  }
  else
  {
    ballPercept.ballWasSeen = false;
  }
}

void BallPerceptor::update(ExtendedBallPercepts& extendedBallPercepts)
{
  DECLARE_DEBUG_DRAWING("module:BallPerceptor:ball","drawingOnImage");
  DECLARE_DEBUG_DRAWING("module:BallPerceptor:recalculate","drawingOnImage");
  DECLARE_DEBUG_DRAWING("module:BallPerceptor:boxes","drawingOnImage");
  DECLARE_DEBUG_DRAWING("module:BallPerceptor:seenRadiusAndCenterOfBall","drawingOnImage");
  extendedBallPercepts.extendedBallPercepts.clear();
  if((!theBallSpots.ballSpots.empty()) && theCameraMatrix.isValid)
  {
    for (std::vector<BallSpot>::const_iterator i = theBallSpots.ballSpots.begin(); i != theBallSpots.ballSpots.end(); ++i)
    {
      ExtendedBallPercept extendedBallPercept;
      searchBall(*i, extendedBallPercept);
      
      if (extendedBallPercept.levenbergMarquardtResult)
      {
        checkCurrentBall(extendedBallPercept);
        CIRCLE("module:BallPerceptor:seenRadiusAndCenterOfBall", extendedBallPercept.positionInImage.x, extendedBallPercept.positionInImage.y, extendedBallPercept.radiusInImage, 2, Drawings::ps_solid, ColorClasses::red, Drawings::ps_dot, ColorClasses::orange);

        if (extendedBallPercept.isValidBallPercept)
          calculateValidityFirstRun(extendedBallPercept);
      }
      if (parameters.minValidityFirstRun < extendedBallPercept.validity && extendedBallPercept.validity < parameters.minValiditySecondRun)
      {
        evaluateColorClasses(extendedBallPercept);
        if (extendedBallPercept.isValidBallPercept)
        {
          calculateValiditySecondRun(extendedBallPercept);
        }
      }
      if (extendedBallPercept.isValidBallPercept)
        extendedBallPercepts.extendedBallPercepts.push_back(extendedBallPercept);
    }
  }
}

void BallPerceptor::calculateValidityFirstRun(ExtendedBallPercept& extendedBallPercept)
{
  double numOfMeasures = 0;
  double result = 0;
  if (extendedBallPercept.validity==-1)
  {
    result += extendedBallPercept.durabilityOfBallPoints * parameters.quantifierDurabilityOfBallPoints;
    numOfMeasures += parameters.quantifierDurabilityOfBallPoints;
    
    result += extendedBallPercept.eccentricity * parameters.quantifierEccentricity;
    numOfMeasures += parameters.quantifierEccentricity;
    
    result += extendedBallPercept.distanceCheckResult * parameters.quantifierDistance;
    numOfMeasures += parameters.quantifierDistance;
    
    if (numOfMeasures)
      result /= numOfMeasures;
    if (result < parameters.minValidityFirstRun)
      extendedBallPercept.isValidBallPercept = false;
    else
      extendedBallPercept.validity = result;
  }
}

void BallPerceptor::calculateValiditySecondRun(ExtendedBallPercept& extendedBallPercept)
{
  double numOfMeasures = 0;
  double result = 0;
 
  result += extendedBallPercept.durabilityOfBallPoints * parameters.quantifierDurabilityOfBallPoints;
  numOfMeasures += parameters.quantifierDurabilityOfBallPoints;
 
  result += extendedBallPercept.eccentricity * parameters.quantifierEccentricity;
  numOfMeasures += parameters.quantifierEccentricity;
  
  result += extendedBallPercept.distanceCheckResult * parameters.quantifierDistance;
  numOfMeasures += parameters.quantifierDistance;

  result += extendedBallPercept.centerResult * parameters.quantifierInner;
  numOfMeasures += parameters.quantifierInner;
  // not used yet
    //result += extendedBallPercept.middleResult * parameters.quantifierMiddle;
    //numOfMeasures += parameters.quantifierMiddle;
  result += extendedBallPercept.outerResult * parameters.quantifierOuter;
  numOfMeasures += parameters.quantifierOuter;

  // not used yet
  //result += extendedBallPercept.deviationOfBallPoints;
  if (numOfMeasures)
    result /= numOfMeasures;
  //OUTPUT(idText, text, "result: " << result);
  if (result < parameters.minValiditySecondRun)
    extendedBallPercept.isValidBallPercept = false;
  else
    extendedBallPercept.validity = result;
  //OUTPUT(idText, text, "centerResult: " << extendedBallPercept.centerResult << " outerResult: " << extendedBallPercept.outerResult << " result: " << result);
  //OUTPUT(idText, text, "deviationOfBallPoints: " << extendedBallPercept.deviationOfBallPoints);
  //OUTPUT(idText, text, "*******************************");
}


void BallPerceptor::evaluateColorClasses(ExtendedBallPercept& extendedBallPercept)
{
  if (extendedBallPercept.isValidBallPercept)
  {
    checkSquares(extendedBallPercept);
    // 0=COUNTER; 1=ORANGE; 2=GREEN; 3=WHITE; 4=RED; 5=YELLOW; 6=BLUE; 7=NONE; 8=OTHER
    extendedBallPercept.middleResult = 1;
    if (extendedBallPercept.centerArea[0] > 0)
    {
      double numOfPixelInArea = extendedBallPercept.centerArea[0];
      double orangeRatio = extendedBallPercept.centerArea[1] / numOfPixelInArea;
      //double redRatio = extendedBallPercept.centerArea[4] / numOfPixelInArea;
      double yellowRatio = extendedBallPercept.centerArea[5] / numOfPixelInArea;
      double otherRatio = ( extendedBallPercept.centerArea[2] + extendedBallPercept.centerArea[3] + extendedBallPercept.centerArea[4] + extendedBallPercept.centerArea[6] + extendedBallPercept.centerArea[8] ) / numOfPixelInArea;
      double noneRatio = extendedBallPercept.centerArea[7] / numOfPixelInArea;
      double greenRatio = extendedBallPercept.centerArea[2] / numOfPixelInArea;
      if ((orangeRatio >= parameters.evalBallCenterOrangeMinPercent) &&
          /**(redRatio <= parameters.evalBallCenterRedMaxPercent) &&*/
          (yellowRatio <= parameters.evalBallCenterYellowMaxPercent) &&
          (otherRatio <= parameters.evalBallCenterOtherMaxPercent) &&
          (noneRatio <= parameters.evalBallCenterNoneMaxPercent))
      {
        extendedBallPercept.centerResult = orangeRatio;
      }
      else
      {
        //OUTPUT(idText, text, "extendedBallPercept.centerResult: " << extendedBallPercept.centerResult);
        if ((orangeRatio >= parameters.evalBallPartOrangeMinPercent) &&
            (greenRatio <= parameters.evalBallPartGreenMaxPercent))
        {
          extendedBallPercept.centerResult = orangeRatio;
          extendedBallPercept.couldBeBallPart = true;
        }
        else
        {
          extendedBallPercept.centerResult = 0;
          extendedBallPercept.isValidBallPercept = false;
        }
      }
    }
    else
    {
      extendedBallPercept.centerResult = 0;
      extendedBallPercept.isValidBallPercept = false;
    }
    if (extendedBallPercept.outerArea[0] > 0)
    {
      double numOfPixelInArea = extendedBallPercept.outerArea[0];
      double orangeRatio = extendedBallPercept.outerArea[1] / numOfPixelInArea;
      //double redRatio = extendedBallPercept.outerArea[4] / numOfPixelInArea;
      if (orangeRatio <= parameters.evalOuterOrangeMaxPercent)
      {
        extendedBallPercept.outerResult = 1 - orangeRatio;
      }
      else
      {
        if (extendedBallPercept.couldBeBallPart)
        {
          extendedBallPercept.outerResult = 1 - orangeRatio;
        }
        else
        {
          extendedBallPercept.outerResult = 0.2;
          extendedBallPercept.isValidBallPercept = false;
        }
      }
      /**if (redRatio >= parameters.evalOuterRobotPartRedMinPercent)
      {
        extendedBallPercept.outerResult -= 0.1;
        extendedBallPercept.couldBeRobotPart = true;
      }*/
    }
    else
    {    
      extendedBallPercept.outerResult = 0.2;
      extendedBallPercept.isValidBallPercept = false;
    }
  }
    //OUTPUT(idText, text, "extendedBallPercept.outerResult: " << extendedBallPercept.outerResult);
}


void BallPerceptor::searchBall(const BallSpot& ballSpot, ExtendedBallPercept& extendedBallPercept)
{
  BallPointList ballPoints;
  Vector2<int> center;
  double radius;
  int countOrange = 0;
  int countAmbiguous = 0;
  int maxOrangePerLine = 0;
  int countPixel = 0;
  Vector2<int> ballStart = ballSpot.position;
  extendedBallPercept.eccentricity = ballSpot.eccentricity;

  scanForBallPoints(ballStart, ballPoints, countAmbiguous, countOrange, maxOrangePerLine, countPixel);

  //ignore ball if not enough points are orange
  int numberOfSoftEdgePoints = 0;
  int numberOfPointsInYellow = 0;

  for(int i = 0; i < ballPoints.number; i++)
  {
    if (ballPoints[i].yellowIsClose && !ballPoints[i].atBorder) numberOfPointsInYellow++;
    if (!ballPoints[i].hardEdge && !ballPoints[i].atBorder) numberOfSoftEdgePoints++;
  }

  if ((countOrange > countAmbiguous) && 
    (countOrange * 6 > countPixel) &&                          // >  16.66% orange
    (numberOfPointsInYellow * 4 <= ballPoints.number * 3)      // <= 75% with Yellow close
    )
  {
    considerBallPoints(ballPoints, extendedBallPercept);

    //try only points near green with hard edge first
    BallPointList testPoints;
    for(int i = 0; i < ballPoints.number; i++)
    {
      if (ballPoints[i].greenIsClose && ballPoints[i].hardEdge) 
        testPoints.add(ballPoints[i]);
    }
    if (
      testPoints.number * 2 >= ballPoints.number &&
      computeBallInImageLevenbergMarquardt(testPoints, center, radius) &&
      checkIfPointsAreInsideBall(ballPoints, center, radius, extendedBallPercept))
    {
      extendedBallPercept.positionInImage.x = center.x;
      extendedBallPercept.positionInImage.y = center.y;
      extendedBallPercept.radiusInImage = radius;
      extendedBallPercept.levenbergMarquardtResult = true;
    }
    else
    {
      //now all points if not at border with hard edge
      testPoints.number = 0;
      for(int i = 0; i < ballPoints.number; i++)
      {
        if (!ballPoints[i].atBorder && ballPoints[i].hardEdge) 
          testPoints.add(ballPoints[i]);
      }
      if (computeBallInImageLevenbergMarquardt(testPoints, center, radius) &&
          checkIfPointsAreInsideBall(ballPoints, center, radius, extendedBallPercept))
      {
        extendedBallPercept.positionInImage.x = center.x;
        extendedBallPercept.positionInImage.y = center.y;
        extendedBallPercept.radiusInImage = radius;
        extendedBallPercept.levenbergMarquardtResult = true;
      }
      else
      {
        //now all points if not at border
        testPoints.number = 0;
        for(int i = 0; i < ballPoints.number; i++)
        {
          if (!ballPoints[i].atBorder) 
            testPoints.add(ballPoints[i]);
        }
        if (computeBallInImageLevenbergMarquardt(testPoints, center, radius) &&
            checkIfPointsAreInsideBall(ballPoints, center, radius, extendedBallPercept))
        {
          extendedBallPercept.positionInImage.x = center.x;
          extendedBallPercept.positionInImage.y = center.y;
          extendedBallPercept.radiusInImage = radius;
          extendedBallPercept.levenbergMarquardtResult = true;
        }
        else
        {
          //take all points if nothing else works
          if (computeBallInImageLevenbergMarquardt(ballPoints, center, radius))
          {
            extendedBallPercept.positionInImage.x = center.x;
            extendedBallPercept.positionInImage.y = center.y;
            extendedBallPercept.radiusInImage = radius;
            extendedBallPercept.levenbergMarquardtResult = true;
            calculateDeviationOfBallPoints(ballPoints, center, radius, extendedBallPercept);
          }
          else
          {
            extendedBallPercept.levenbergMarquardtResult = false;
          }
        }
      }
    }
  }
  if (!extendedBallPercept.levenbergMarquardtResult)
    extendedBallPercept.isValidBallPercept = false;
}


void BallPerceptor::BallPointList::add(const BallPoint& ballPoint)
{
  if(number < maxNumberOfPoints)
  {
    ballPoints[number++] = ballPoint;
    DOT("module:BallPerceptor:ball", ballPoint.x, ballPoint.y,
      (ballPoint.hardEdge) ? ColorClasses::blue : ColorClasses::orange, 
      (ballPoint.atBorder) ? ColorClasses::black :
      (ballPoint.greenIsClose) ? ColorClasses::green :
      (ballPoint.yellowIsClose) ? ColorClasses::yellow :
      ColorClasses::white
      );
  }
}


void BallPerceptor::scanForBallPoints
(
 const Vector2<int>& ballStart,
 BallPointList& ballPoints,
 int& countAmbiguous, 
 int& countOrange,
 int& maxOrangePerLine,
 int& countPixel
 )
{
  // search for ball variables
  BallPoint north;
  BallPoint east;
  BallPoint south;
  BallPoint west;

  BallPoint start;
  start = ballStart;
  Vector2<int>step;
  BallPoint destination;
  BallPoint start2;

  DOT("module:BallPerceptor:ball", start.x, start.y, ColorClasses::black, ColorClasses::white);
  //find north
  step.x = 0; step.y = -1;

  findEndOfBall(start, step, north, countAmbiguous, countOrange, maxOrangePerLine, countPixel);
  if(north.atBorder)
  {
    start2 = north - step;
    //find east
    step.x = 1; step.y = 0;
    if(findEndOfBall(start2, step, destination, countAmbiguous, countOrange, maxOrangePerLine, countPixel))
    {
      ballPoints.add(destination);
    }
    //find west
    step.x = -1; step.y = 0;
    if(findEndOfBall(start2, step, destination, countAmbiguous, countOrange, maxOrangePerLine, countPixel))
    {
      ballPoints.add(destination);
    }
  }
  else
  {
    ballPoints.add(north);
  }

  //find east
  step.x = 1; step.y = 0;
  findEndOfBall(start, step, east, countAmbiguous, countOrange, maxOrangePerLine, countPixel);
  if(east.atBorder)
  {
    start2 = east - step;
    //find north
    step.x = 0; step.y = -1;
    if(findEndOfBall(start2, step, destination, countAmbiguous, countOrange, maxOrangePerLine, countPixel))
    {
      ballPoints.add(destination);
    }
    //find south
    step.x = 0; step.y = 1;
    if(findEndOfBall(start2, step, destination, countAmbiguous, countOrange, maxOrangePerLine, countPixel))
    {
      ballPoints.add(destination);
    }
  }
  else
  {
    ballPoints.add(east);
  }

  //find south
  step.x = 0; step.y = 1;
  findEndOfBall(start, step, south, countAmbiguous, countOrange, maxOrangePerLine, countPixel);
  if(south.atBorder)
  {
    start2 = south - step;
    //find east
    step.x = 1; step.y = 0;
    if(findEndOfBall(start2, step, destination, countAmbiguous, countOrange, maxOrangePerLine, countPixel))
    {
      ballPoints.add(destination);
    }
    //find west
    step.x = -1; step.y = 0;
    if(findEndOfBall(start2, step, destination, countAmbiguous, countOrange, maxOrangePerLine, countPixel))
    {
      ballPoints.add(destination);
    }
  }
  else
  {
    ballPoints.add(south);
  }

  //find west
  step.x = -1; step.y = 0;
  findEndOfBall(start, step, west, countAmbiguous, countOrange, maxOrangePerLine, countPixel);
  if(west.atBorder)
  {
    start2 = west - step;
    //find north
    step.x = 0; step.y = -1;
    if(findEndOfBall(start2, step, destination, countAmbiguous, countOrange, maxOrangePerLine, countPixel))
    {
      ballPoints.add(destination);
    }
    //find south
    step.x = 0; step.y = 1;
    if(findEndOfBall(start2, step, destination, countAmbiguous, countOrange, maxOrangePerLine, countPixel))
    {
      ballPoints.add(destination);
    }
  }
  else
  {
    ballPoints.add(west);
  }

  if( (south.y - north.y) > (east.x - west.x) )
  {
    if ((north.y + south.y) / 2 != start.y)
    {
      start.y = (north.y + south.y) / 2;
      //find east
      step.x = 1; step.y = 0;
      findEndOfBall(start, step, east, countAmbiguous, countOrange, maxOrangePerLine, countPixel);
      if(!east.atBorder)
      {
        ballPoints.add(east);
      }
      //find west
      step.x = -1; step.y = 0;
      findEndOfBall(start, step, west, countAmbiguous, countOrange, maxOrangePerLine, countPixel);
      if (!west.atBorder)
      {
        ballPoints.add(west);
      }
      start.x = (west.x + east.x) / 2;
    }
  }
  else
  {
    if ((west.x + east.x) / 2 != start.x)
    {
      start.x = (west.x + east.x) / 2;
      //find north
      step.x = 0; step.y = -1;
      findEndOfBall(start, step, north, countAmbiguous, countOrange, maxOrangePerLine, countPixel);
      if(!north.atBorder)
      {
        ballPoints.add(north);
      }
      //find south
      step.x = 0; step.y = 1;
      findEndOfBall(start, step, south, countAmbiguous, countOrange, maxOrangePerLine, countPixel);
      if(!south.atBorder)
      {
        ballPoints.add(south);
      }
      start.y = (north.y + south.y) / 2;
    }
  }

  // find in diagonal
  for (step.x = -1; step.x <= 1; step.x += 2)
  {
    for (step.y = -1; step.y <= 1; step.y += 2)
    {
      findEndOfBall(start, step, destination, countAmbiguous, countOrange, maxOrangePerLine, countPixel);
      if (!destination.atBorder)
      {
        ballPoints.add(destination);
      }
    }
  }
}


bool BallPerceptor::findEndOfBall
(
 const BallPoint& start,
 const Vector2<int>& step,
 BallPoint& destination,
 int& countAmbiguous,
 int& countOrange,
 int& maxOrangePerLine,
 int& countPixel
 )
{
  /*
  stopColors are colors indicating the end of the ball:
  green, yellow, skyblue
  more than 3 pixels of a stopColor indicate the end of the ball
  */
  int stopColorCounter = 0;
  int currentOrange = 0;
  int currentAmbiguous = 0;
  int len = 0;
  int stopLen = 0;

  ColorClasses::Color currentColorClass;

  Vector2<int> firstStopColor = start;
  unsigned char currentOrangeSim = 0;
  Vector2<int> lastPoint = start;
  destination = start;
  destination.greenIsClose = false;
  destination.yellowIsClose = false;
  destination.atBorder = false;
  destination.hardEdge = true;

  bool isOrange = false;

  bool goOn = true;

  while(goOn)
  {
    lastPoint = destination;
    destination += step;
    if(destination.x < 0 || destination.x >= theImage.cameraInfo.resolutionWidth ||
       destination.y < 0 || destination.y >= theImage.cameraInfo.resolutionHeight) 
    {
      if (stopColorCounter == 0)
      {
        countPixel += len;
        destination.atBorder = true;
        ballPointsAtImageBorder = true;
      }
      else
      {
        destination = firstStopColor;
        countPixel += stopLen;
        destination.atBorder = false;
      }
      goOn = false;
    }
    else
    {
      currentColorClass = (ColorClasses::Color) theColorTable64.colorClasses
        [theImage.image[destination.y][destination.x].y >> 2]
        [theImage.image[destination.y][destination.x].cb >> 2]
        [theImage.image[destination.y][destination.x].cr >> 2];

      // counting all orange pixels on the scanned horz./vert./diag. lines
      len++;
      if ( currentColorClass == ColorClasses::orange )
      {
        currentOrange++;
      }
      if (currentColorClass == ColorClasses::yellow)
      {
        currentAmbiguous++;
      }

      LINE("module:BallPerceptor:ball",destination.x,destination.y,destination.x + 1,destination.y,1,Drawings::ps_solid,ColorClasses::green);

      if (currentColorClass == ColorClasses::orange)
        isOrange = true;
      else
      {
        currentOrangeSim = getSimilarityToOrange(
          theImage.image[destination.y][destination.x].y,
          theImage.image[destination.y][destination.x].cb,
          theImage.image[destination.y][destination.x].cr
          );
        isOrange = currentOrangeSim > 30;
      }
      if(currentColorClass == ColorClasses::green || 
        currentColorClass == ColorClasses::yellow ||
        currentColorClass == ColorClasses::blue ||
        !isOrange
        )
      {
        if (stopColorCounter == 0)
        {
          firstStopColor = destination;
          stopLen = len;
        }
        if (currentColorClass == ColorClasses::green) destination.greenIsClose = true;
        if (currentColorClass == ColorClasses::yellow) destination.yellowIsClose = true;
        stopColorCounter++;

        LINE("module:BallPerceptor:ball",destination.x,destination.y,destination.x + 1,destination.y,1,Drawings::ps_solid,ColorRGBA(255,0,0));

        if(stopColorCounter > parameters.stopColorCounter) 
        {
          destination = firstStopColor;
          countPixel += stopLen;
          goOn = false;
        }
      }
      else
      {
        destination.greenIsClose = false;
        destination.yellowIsClose = false;
        stopColorCounter = 0;
      }
    } 
  }
  destination -= step;

  // compute sum of all orange pixels and max-pixels-per-line
  countOrange += currentOrange;
  countAmbiguous += currentAmbiguous;
  maxOrangePerLine = std::max ( maxOrangePerLine, currentOrange);
  if (destination.greenIsClose) destination.yellowIsClose = false;

  if (!destination.atBorder)
  {
    unsigned char lastGoodColorOrangeSim = getSimilarityToOrange(
      theImage.image[destination.y][destination.x].y,
      theImage.image[destination.y][destination.x].cb,
      theImage.image[destination.y][destination.x].cr
      );
    destination.hardEdge = (2 * currentOrangeSim < lastGoodColorOrangeSim);
  }
  else 
    destination.hardEdge = false;

  if (!destination.atBorder)
  {
    if (destination.greenIsClose)
    {
      LINE("module:BallPerceptor:ball",destination.x,destination.y,destination.x + 1,destination.y,1,Drawings::ps_solid,ColorClasses::blue);
    }
    else
      if (destination.yellowIsClose)
      {
        LINE("module:BallPerceptor:ball",destination.x,destination.y,destination.x + 1,destination.y,1,Drawings::ps_solid,ColorRGBA(0,128,255));
      }
      else
      {
        LINE("module:BallPerceptor:ball",destination.x,destination.y,destination.x + 1,destination.y,1,Drawings::ps_solid,ColorRGBA(255,128,128));
      }
  }
  return true;
}

bool BallPerceptor::computeBallInImageLevenbergMarquardt( 
  const BallPointList& ballPoints,
  Vector2<int>& center,
  double& radius)
{
  if(!ballPointsAtImageBorder)
  {
    double best = 0;
    for (int i = 0; i < ballPoints.number; ++i)
      for (int j = 0; j < ballPoints.number;  ++j)
      {
        if (i == j)
          continue;
        double dist = sqrt((double) (ballPoints[i].x - ballPoints[j].x) * (ballPoints[i].x - ballPoints[j].x) + (ballPoints[i].y - ballPoints[j].y) * (ballPoints[i].y - ballPoints[j].y));
        if (dist > best)
        {
          best = dist;
          center.x = (ballPoints[i].x - ballPoints[j].x)/2;
          center.x = ballPoints[i].x - center.x;
          center.y = (ballPoints[i].y- ballPoints[j].y)/2;
          center.y = ballPoints[i].y - center.y;
          radius = dist/2;
        }
      }
    if (best)
      return true;
    
    return false;
  }
  else if (ballPointsAtImageBorder || radius < 2)
  {
    if (ballPoints.number < 3)
      return false;
    int Mx(0), My(0), Mxx(0), Myy(0), Mxy(0), Mz(0), Mxz(0), Myz(0);
    for (int i = 0; i < ballPoints.number; ++i)
    {
      int x = ballPoints[i].x;
      int y = ballPoints[i].y;
      int xx = x*x;
      int yy = y*y;
      int z = xx + yy;
      Mx += x;
      My += y;
      Mxx += xx;
      Myy += yy;
      Mxy += x*y;
      Mz += z;
      Mxz += x*z;
      Myz += y*z;
    }

    // Construct and solve matrix (might fail and throw exception).
    // Result will be center and radius of ball in theImage.
    try
    {
      double matrix_values[9] = 
      {
        static_cast<double>(Mxx), static_cast<double>(Mxy), static_cast<double>(Mx),
        static_cast<double>(Mxy), static_cast<double>(Myy), static_cast<double>(My),
        static_cast<double>(Mx),  static_cast<double>(My),  static_cast<double>(ballPoints.number)
      };
      Matrix_nxn<double, 3> M(matrix_values);

      Vector_n<double, 3> v;
      v[0] = static_cast<double>(-Mxz);
      v[1] = static_cast<double>(-Myz);
      v[2] = static_cast<double>(-Mz);

      Vector_n<double, 3> BCD(M.solve(v));
      center.x = static_cast<int>(-BCD[0]/2.0);
      center.y = static_cast<int>(-BCD[1]/2.0);
      double radicand = BCD[0]*BCD[0]/4.0 + BCD[1]*BCD[1]/4.0 - BCD[2];
      if (radicand < 0.0)
        return false;
      radius = sqrt(radicand);
    }
    catch (...)
    {
      return false;
    }
    return true;
  }
  return false;
}


bool BallPerceptor::checkIfPointsAreInsideBall(const BallPointList& ballPoints, Vector2<int>& center, double radius, ExtendedBallPercept& extendedBallPercept)
{
  double d;
  int numberOfNonBorderBallPoints = 0;
  double dev = 0;
  for(int i = 0; i < ballPoints.number; i++)
  {
    d = Geometry::distance(center, ballPoints[i]);
    if ((d > radius * 1.1) && (fabs(d-radius) > 5))
    {
      return false; 
    }
    if (!ballPoints[i].atBorder)
    {
      numberOfNonBorderBallPoints++;
      dev += fabs(d-radius);
    }
  }
  extendedBallPercept.deviationOfBallPoints = (numberOfNonBorderBallPoints ? dev / numberOfNonBorderBallPoints : 0);
  return true;
}


void BallPerceptor::calculateDeviationOfBallPoints(const BallPointList& ballPoints, Vector2<int>& center, double radius, ExtendedBallPercept& extendedBallPercept)
{
  double d;
  int numberOfNonBorderBallPoints = 0;
  double dev = 0;
  for(int i = 0; i < ballPoints.number; i++)
  {
    d = Geometry::distance(center, ballPoints[i]);
    if (!ballPoints[i].atBorder)
    {
      numberOfNonBorderBallPoints++;
      dev += fabs(d-radius);
    }
  }
  extendedBallPercept.deviationOfBallPoints = (numberOfNonBorderBallPoints ? dev / numberOfNonBorderBallPoints : 0);
}


void BallPerceptor::considerBallPoints(const BallPointList& ballPoints, ExtendedBallPercept& extendedBallPercept)
{
  int numberOfNearGreenPoints = 0;
  int numberOfNearYellowPoints = 0;
  int numberOfHardEdgePoints = 0;
  int numberOfNonBorderPoints = 0;

  for (int i=0; i<ballPoints.number; i++)
  {
    if (!ballPoints[i].atBorder)
    {
      if (ballPoints[i].hardEdge)
      {
        numberOfHardEdgePoints++;
      }
      if (ballPoints[i].greenIsClose)
      {
        numberOfNearGreenPoints++;
      }
      if (ballPoints[i].yellowIsClose)
      {
        numberOfNearYellowPoints++;
      }
      numberOfNonBorderPoints++;
    }
  }
  if (numberOfNonBorderPoints>0)
  {
    extendedBallPercept.durabilityOfBallPoints=1.0;
    if (!(numberOfHardEdgePoints>=3 && numberOfNearGreenPoints*2>numberOfNonBorderPoints)) 
      extendedBallPercept.durabilityOfBallPoints *= 0.8;
    if (numberOfHardEdgePoints*3<numberOfNonBorderPoints*2) // less than 2/3 hard edge
      extendedBallPercept.durabilityOfBallPoints *= 0.9;
    if (numberOfHardEdgePoints*3<numberOfNonBorderPoints)   // less than 1/3 hard edge
      extendedBallPercept.durabilityOfBallPoints *= 0.9;
    if (numberOfNearGreenPoints*4<numberOfNonBorderPoints)   // less than 1/4 near green
      extendedBallPercept.durabilityOfBallPoints *= 0.5;
    if (numberOfNearGreenPoints==0)                           // no points near green
      extendedBallPercept.durabilityOfBallPoints *= 0.5;
    if (numberOfNearYellowPoints*2>numberOfNonBorderPoints) // more than 1/2 near yellow
      extendedBallPercept.durabilityOfBallPoints *= 0.9;
    if (numberOfNonBorderPoints<4)                            // only 1-3 non border points 
      extendedBallPercept.durabilityOfBallPoints *= 0.5;
  }
  else
  {// there are only border ball points, so the ball is right in front of the camera, and the durability can stay 1.0
    extendedBallPercept.durabilityOfBallPoints=1.0;
  }
}


void BallPerceptor::checkCurrentBall
(
 ExtendedBallPercept& extendedBallPercept
 )
{
  Vector2<int> center;
  center.x = int(extendedBallPercept.positionInImage.x);
  center.y = int(extendedBallPercept.positionInImage.y);
  double radius = extendedBallPercept.radiusInImage;
  // test if ball is below horizon
  double factor = theImage.cameraInfo.focalLengthInv;
  Vector3<double> vectorToCenter(1, 
                                 (theImage.cameraInfo.opticalCenter.x - center.x) * factor, 
                                 (theImage.cameraInfo.opticalCenter.y - center.y) * factor);
  Vector3<double> vectorToCenterWorld = theCameraMatrix.rotation * vectorToCenter;

  //Is the point above the horizon ? - return
  if(vectorToCenterWorld.z < 1 * factor)
  {
    double percentOfOrange = 1.0; // big balls are already scanned enough. They are supposed to have nearly 100% orange inside
    if (radius <= 12.0)
    {//small ghost balls can easily appear on an orange fringe of a pink/yellow landmark
      // this because the scan follows along the fringe; this new scan starts from the 
      // hypothetical center of the "ghost" ball, which in such case lies inside the landmark
      // and hence produces a high number of "non orange" points; a true ball however,
      // should have enough orange inside
      Vector2<int> current(center), clippedCenter(center);
      int orangeCount = 0;
      int totalCount = 0;
      double scanAngle = 0;
      ColorClasses::Color currentColor;
      Geometry::clipPointInsideRectangle(Vector2<int>(0,0), Vector2<int>(theImage.cameraInfo.resolutionWidth - 1, theImage.cameraInfo.resolutionHeight - 1), clippedCenter);
      currentColor = (ColorClasses::Color) theColorTable64.colorClasses
        [theImage.image[clippedCenter.y][clippedCenter.x].y >> 2]
        [theImage.image[clippedCenter.y][clippedCenter.x].cb >> 2]
        [theImage.image[clippedCenter.y][clippedCenter.x].cr >> 2];
      if(currentColor == ColorClasses::orange)
        orangeCount++;
      totalCount++;
      for (int sector=0; sector<8; sector++)
      {
        BresenhamLineScan scan(clippedCenter, scanAngle, theImage.cameraInfo);
        scan.init();
        current = clippedCenter;
        int steps = (sector%2) ? (int)(radius/sqrt(2.0)) : (int)(radius);
        for (int i=0; i<steps && i<scan.numberOfPixels; i++)
        {
          scan.getNext(current);
          currentColor = (ColorClasses::Color) theColorTable64.colorClasses
            [theImage.image[current.y][current.x].y >> 2]
            [theImage.image[current.y][current.x].cb >> 2]
            [theImage.image[current.y][current.x].cr >> 2];
          if(currentColor == ColorClasses::orange)
            orangeCount++;
          totalCount++;
          DOT("module:BallPerceptor:ball", current.x, current.y, ColorRGBA(128, 128, 128), currentColor);
        }
        scanAngle += pi_4;
      }
      if (orangeCount*3 < totalCount) //not enough orange to be ball
        return;
      percentOfOrange = (double)orangeCount/totalCount;
    }

    Vector2<double> correctedCenter = theImageCoordinateSystem.toCorrected(center);
    Vector2<double>angles;
    Geometry::calculateAnglesForPoint(Vector2<int>(int(correctedCenter.x + 0.5), int(correctedCenter.y + 0.5)),
                                      theCameraMatrix, theImage.cameraInfo, angles);
    double radiusAsAngle = Geometry::pixelSizeToAngleSize(radius, theImage.cameraInfo);

    computeOffsetToBall(center,
      angles,
      radius,
      radiusAsAngle, 
      theCameraMatrix.translation, 
      theCameraMatrix.isValid,
      extendedBallPercept);
  }
  else
    extendedBallPercept.isValidBallPercept = false;
}

void BallPerceptor::checkSquares(ExtendedBallPercept& extendedBallPercept)
{
  double radius = extendedBallPercept.radiusInImage;
  Vector2<int> centerTopLeft;
  centerTopLeft.x = int(extendedBallPercept.positionInImage.x);
  centerTopLeft.y = int(extendedBallPercept.positionInImage.y);
  Vector2<int> centerDownRight = centerTopLeft;
  Vector2<int> middleTopLeft = centerTopLeft;
  Vector2<int> middleDownRight = centerTopLeft;
  Vector2<int> outerTopLeft = centerTopLeft;
  Vector2<int> outerDownRight = centerTopLeft;
  int imageWidth = theImage.cameraInfo.resolutionWidth - 1;
  int imageHeight= theImage.cameraInfo.resolutionHeight - 1;
  int* targetArea;
  
  middleTopLeft.x = int(floor(middleTopLeft.x - radius)); 
  middleTopLeft.y = int(floor(middleTopLeft.y - radius));
  if (middleDownRight.x <= 0)
  {
    middleDownRight.x = 0;
  }
  if (middleDownRight.y <= 0)
  {
    middleDownRight.y = 0;
  }
  if (middleDownRight.x > imageWidth)
  {
    middleDownRight.x = imageWidth;
  }
  if (middleDownRight.y > imageHeight)
  {
    middleDownRight.y = imageHeight;
  }
  middleDownRight.x = int(ceil(middleDownRight.x + radius)); 
  middleDownRight.y = int(ceil(middleDownRight.y + radius));
  if (middleDownRight.x <= 0)
  {
    middleDownRight.x = 0;
  }
  if (middleDownRight.y <= 0)
  {
    middleDownRight.y = 0;
  }
  if (middleDownRight.x > imageWidth)
  {
    middleDownRight.x = imageWidth;
  }
  if (middleDownRight.y > imageHeight)
  {
    middleDownRight.y = imageHeight;
  }
  
  if (middleDownRight.x - middleTopLeft.x > calculatedBallRadius*2.2)
  {
    extendedBallPercept.isValidBallPercept = false;
    return;
  }

  centerTopLeft.x = int(floor(centerTopLeft.x - sqrt(0.5)*radius)); 
  centerTopLeft.y = int(floor(centerTopLeft.y - sqrt(0.5)*radius));
  if (centerTopLeft.x <= 0)
  {
    centerTopLeft.x = 0;
  }
  if (centerTopLeft.y <= 0)
  {
    centerTopLeft.y = 0;
  }
  if (centerTopLeft.x > imageWidth )
  {
    centerTopLeft.x = imageWidth;
  }
  if (centerTopLeft.y > imageHeight)
  {
    centerTopLeft.y = imageHeight;
  }
  centerDownRight.x = int(ceil(centerDownRight.x + sqrt(0.5)*radius)); 
  centerDownRight.y = int(ceil(centerDownRight.y + sqrt(0.5)*radius));
  if (centerDownRight.x <= 0)
  {
    centerDownRight.x = 0;
  }
  if (centerDownRight.y <= 0)
  {
    centerDownRight.y = 0;
  }
  if (centerDownRight.x > imageWidth)
  {
    centerDownRight.x = imageWidth;
  }
  if (centerDownRight.y > imageHeight)
  {
    centerDownRight.y = imageHeight;
  }

  outerTopLeft.x = int(floor(outerTopLeft.x - sqrt(2.)*radius)); 
  outerTopLeft.y = int(floor(outerTopLeft.y - sqrt(2.)*radius));
  if (outerTopLeft.x <= 0)
  {
    outerTopLeft.x = 0;
  }
  if (outerTopLeft.y <= 0)
  {
    outerTopLeft.y = 0;
  }
  if (outerTopLeft.x > imageWidth)
  {
    outerTopLeft.x = imageWidth;
  }
  if (outerTopLeft.y > imageHeight)
  {
    outerTopLeft.y = imageHeight;
  }
  outerDownRight.x = int(ceil(outerDownRight.x + sqrt(2.)*radius)); 
  outerDownRight.y = int(ceil(outerDownRight.y + sqrt(2.)*radius));
  if (outerDownRight.x <= 0)
  {
    outerDownRight.x = 0;
  }
  if (outerDownRight.y <= 0)
  {
    outerDownRight.y = 0;
  }
  if (outerDownRight.x > imageWidth)
  {
    outerDownRight.x = imageWidth;
  }
  if (outerDownRight.y > imageHeight)
  {
    outerDownRight.y = imageHeight;
  }

  RECTANGLE("module:BallPerceptor:boxes", centerTopLeft.x, centerTopLeft.y, centerDownRight.x, centerDownRight.y, 1, Drawings::ps_solid, ColorClasses::red);
  RECTANGLE("module:BallPerceptor:boxes", middleTopLeft.x, middleTopLeft.y, middleDownRight.x, middleDownRight.y, 1, Drawings::ps_solid, ColorClasses::yellow);
  RECTANGLE("module:BallPerceptor:boxes", outerTopLeft.x, outerTopLeft.y, outerDownRight.x, outerDownRight.y, 1, Drawings::ps_solid, ColorClasses::green);

  extendedBallPercept.centerTopLeft = centerTopLeft;
  extendedBallPercept.centerDownRight = centerDownRight;
  extendedBallPercept.middleTopLeft = middleTopLeft;
  extendedBallPercept.middleDownRight = middleDownRight;
  extendedBallPercept.outerTopLeft = outerTopLeft;
  extendedBallPercept.outerDownRight = outerDownRight;

  for (int i = 0; i <= 8; i++)
  {
    extendedBallPercept.centerArea[i] = 0;
    extendedBallPercept.middleArea[i] = 0;
    extendedBallPercept.outerArea[i] = 0;
  }
  ColorClasses::Color currentColorClass;
  
  int stepWidth = (outerDownRight.x - outerTopLeft.x + 1) / parameters.maxScanPoints + 1;
  for (int x = outerTopLeft.x; x <= outerDownRight.x; x += stepWidth)
  { 
    int clippedY = outerDownRight.y;
    theBodyContour.clipBottom(x,clippedY);
    #ifndef RELEASE
      if (outerDownRight.y != clippedY)
      {
        DOT("module:BallPerceptor:boxes", x, clippedY, ColorClasses::black, ColorClasses::black);
      }
    #endif
    for (int y = outerTopLeft.y; y <= clippedY; y += stepWidth)
    {
      if ((x >= centerTopLeft.x && x <= centerDownRight.x)
          && ( y >= centerTopLeft.y && y <= centerDownRight.y))
      {
        targetArea = &(extendedBallPercept.centerArea[0]);
      }
      else
      {
        if ((x >= middleTopLeft.x && x <= middleDownRight.x)
            && ( y >= middleTopLeft.y && y <= middleDownRight.y))
        {
          targetArea = &(extendedBallPercept.middleArea[0]);
        }
        else
        {
          targetArea = &(extendedBallPercept.outerArea[0]);
        }
      }
      currentColorClass = (ColorClasses::Color) theColorTable64.colorClasses
        [theImage.image[y][x].y >> 2]
        [theImage.image[y][x].cb >> 2]
        [theImage.image[y][x].cr >> 2];
      targetArea[0]++; 
      // 0=COUNTER; 1=ORANGE; 2=GREEN; 3=WHITE; 4=RED; 5=YELLOW; 6=BLUE; 7=NONE; 8=OTHER
      switch (currentColorClass)
      {
        case ColorClasses::orange: targetArea[1]++; break;
        case ColorClasses::green: targetArea[2]++; break;
        case ColorClasses::white: targetArea[3]++; break;
        case ColorClasses::red: targetArea[4]++; break;
        case ColorClasses::yellow: targetArea[5]++; break;
        case ColorClasses::blue: targetArea[6]++; break;
        case ColorClasses::none: targetArea[7]++; break;
        default: targetArea[5]++;
      }
    }
  }
}

void BallPerceptor::checkBallDistance(ExtendedBallPercept& extendedBallPercept)
{
  double radiusAsAngle = Geometry::pixelSizeToAngleSize(extendedBallPercept.radiusInImage, theImage.cameraInfo);
  double distance = Geometry::getBallDistanceByAngleSize((int) 2 * theFieldDimensions.ballRadius, 2 * radiusAsAngle);

  Geometry::Circle c;
  Geometry::calculateBallInImage(extendedBallPercept.relativePositionOnField, theCameraMatrix, theImage.cameraInfo, theFieldDimensions.ballRadius, c);
  calculatedBallRadius = c.radius;
  DRAWTEXT("module:BallPerceptor:recalculate", extendedBallPercept.positionInImage.x + c.radius+5, extendedBallPercept.positionInImage.y, 400, ColorClasses::white, extendedBallPercept.radiusInImage << " " << c.radius);
  DRAWTEXT("module:BallPerceptor:recalculate", extendedBallPercept.positionInImage.x + c.radius+5, extendedBallPercept.positionInImage.y+20, 400, ColorClasses::white, c.center.x << " " << c.center.y);
  if (distance < 7500)
  {
    if (extendedBallPercept.radiusInImage > 0 && c.center.x > 0 && c.center.x < theImage.cameraInfo.resolutionWidth && c.center.y > 0 && c.center.y < theImage.cameraInfo.resolutionHeight) 
    {
      CIRCLE("module:BallPerceptor:recalculate", c.center.x, c.center.y, c.radius, 2, Drawings::ps_solid, ColorClasses::red, Drawings::bs_solid, ColorClasses::orange);
      double factorRadiusDifference = 1 - (std::abs(extendedBallPercept.radiusInImage-c.radius)) / extendedBallPercept.radiusInImage;
      double factorCenterXdifference = 1 - fabs(extendedBallPercept.positionInImage.x - c.center.x) / theImage.cameraInfo.resolutionWidth;
      double factorCenterYdifference = 1 - fabs(extendedBallPercept.positionInImage.y - c.center.y) / theImage.cameraInfo.resolutionHeight;
      extendedBallPercept.distanceCheckResult = factorRadiusDifference * factorCenterXdifference * factorCenterYdifference;
    }
    //center of ball might be out of the image, for balls at imageborder
    else
    {
      extendedBallPercept.distanceCheckResult = parameters.distanceResultCenterOutOfImage;
    }
  }
  else
    extendedBallPercept.isValidBallPercept = false;

}


void BallPerceptor::computeOffsetToBall(
 const Vector2<int>& centerInPixel,
 const Vector2<double>& centerAsAngles,
 double radiusInPixel,
 double radiusAsAngle,
 const Vector3<double>& translationOfCamera,
 bool isCameraMatrixValid,
 ExtendedBallPercept& extendedBallPercept)
{
  if (extendedBallPercept.isValidBallPercept)
  {
    double ballRadius = theFieldDimensions.ballRadius;

    // Bearing based ball offset:
    Vector2<double> bbo; // bearing based
    const double distanceBearingBased = centerAsAngles.y < 0 ? (translationOfCamera.z - ballRadius) / tan(-centerAsAngles.y) : 10000;
    bbo.x = translationOfCamera.x + distanceBearingBased * cos(centerAsAngles.x);
    bbo.y = translationOfCamera.y + distanceBearingBased * sin(centerAsAngles.x);

    // Size based ball offset:
    Vector2<double> sbo; 
    const double distanceFromCameraToBallCenter = Geometry::getBallDistanceByAngleSize(int(2.0 * ballRadius), 2.0 * radiusAsAngle);
    double distanceFromCameraToBallCenterOnGround;
    if(distanceFromCameraToBallCenter > fabs(translationOfCamera.z - ballRadius))
      distanceFromCameraToBallCenterOnGround = sqrt(sqr(distanceFromCameraToBallCenter) - sqr(translationOfCamera.z  - ballRadius));
    else
      distanceFromCameraToBallCenterOnGround = 0;
    sbo.x = translationOfCamera.x + distanceFromCameraToBallCenterOnGround * cos(centerAsAngles.x);
    sbo.y = translationOfCamera.y + distanceFromCameraToBallCenterOnGround * sin(centerAsAngles.x);

    Vector2<double> o; // offset
    const double sbDist(sbo.abs());
    const double bbDist(bbo.abs());
    const double SIZE_BASED_ONLY_DIST(800.0);
    const double BEARING_BASED_ONLY_DIST(400.0);
    //In some cases, use size based computation only:
    if(!isCameraMatrixValid || centerAsAngles.y >= 0 || bbDist > SIZE_BASED_ONLY_DIST) 
      o = sbo;
    //At medium distance, interpolate between both offsets:
    else if(bbDist >= BEARING_BASED_ONLY_DIST)
    {
      double factor = (SIZE_BASED_ONLY_DIST - sbDist) / (SIZE_BASED_ONLY_DIST - BEARING_BASED_ONLY_DIST);
      o = bbo * factor + sbo * (1.0 - factor);
    }
    //In all remainung cases (ball is near, no other problems), use bearing based distance only:
    else 
      o = bbo;
    // Finally, create the percept
    extendedBallPercept.relativePositionOnField = o;
    checkBallDistance(extendedBallPercept);
    if (extendedBallPercept.isValidBallPercept)
      extendedBallPercept.isValidBallPercept =calculatedBallRadius * 0.7 && radiusInPixel < calculatedBallRadius * 1.1;
    PLOT("module:BallPerceptor:ballAngleRadian", atan2(o.y, o.x));
    PLOT("module:BallPerceptor:ballAngleDegree", toDegrees(atan2(o.y, o.x)));
  }
}

MAKE_MODULE(BallPerceptor, Perception)
