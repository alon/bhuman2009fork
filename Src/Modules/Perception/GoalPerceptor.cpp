/**
* @file GoalPerceptor.cpp
*
* Simple GoalLocator implementation.
*
* @author Andrik Rieskamp
*/

#include "GoalPerceptor.h"
#include "Tools/Math/AngleTables.h"
#include "Tools/Streams/InStreams.h"
#include "Tools/Team.h"
#include "Representations/Perception/GoalPercept.h"
#include <limits>

using namespace std;


GoalPerceptor::GoalPerceptor()
 : lastExecution(0)
{
  InConfigFile stream("goalPerceptor.cfg", "default");
  ASSERT(stream.exists());
  stream >> parameters;
}


void GoalPerceptor::init()
{
  lastOdometry = theOdometryData;
}


void GoalPerceptor::update(GoalPercept& goalPercept)
{
  DECLARE_DEBUG_DRAWING("module:GoalPerceptor:field", "drawingOnField"); // Draws the internal representations of the goal locator
  DECLARE_DEBUG_DRAWING("module:GoalPerceptor:image", "drawingOnImage"); // Draws the internal representations of the goal locator
  DECLARE_DEBUG_DRAWING("module:GoalPerceptor:width", "drawingOnImage"); // Draws the internal representations of the goal locator
  DECLARE_DEBUG_DRAWING("module:GoalPerceptor:green", "drawingOnImage"); // Draws the internal representations of the goal locator

  DECLARE_PLOT("module:GoalPerceptor:bluePoleDistance0");
  DECLARE_PLOT("module:GoalPerceptor:bluePoleDistance0byHead");
  DECLARE_PLOT("module:GoalPerceptor:bluePoleDistance0byFoot");
  DECLARE_PLOT("module:GoalPerceptor:bluePoleDistance1");
  DECLARE_PLOT("module:GoalPerceptor:bluePoleDistance1byHead");
  DECLARE_PLOT("module:GoalPerceptor:bluePoleDistance1byFoot");

  DECLARE_PLOT("module:GoalPerceptor:yellowPoleDistance0");
  DECLARE_PLOT("module:GoalPerceptor:yellowPoleDistance0byHead");
  DECLARE_PLOT("module:GoalPerceptor:yellowPoleDistance0byFoot");
  DECLARE_PLOT("module:GoalPerceptor:yellowPoleDistance1");
  DECLARE_PLOT("module:GoalPerceptor:yellowPoleDistance1byHead");
  DECLARE_PLOT("module:GoalPerceptor:yellowPoleDistance1byFoot");
  
  MODIFY("module:GoalPerceptor:parameters", parameters);

  preExecution(goalPercept);
  motionUpdate(goalPercept);
  if(!theCameraMatrix.isValid)
    return;
  
  int minSegSize[ColorClasses::numOfColors];
  for(int i=0; i<ColorClasses::numOfColors;i++)
    minSegSize[i] = 0;
  pointExplorer.initFrame(&theImage, &theColorTable64, 0, 0, 4, minSegSize);
  detectGoal(oppColor, oppPolePercepts);
  detectGoal(ownColor, ownPolePercepts);
  updateRepresentation(goalPercept, oppPolePercepts, ownPolePercepts);
  postExecution();
  
  TEAM_OUTPUT(idTeamMateGoalPercept, bin, goalPercept);
  DEBUG_RESPONSE("representation:GoalPercept", OUTPUT(idGoalPercept, bin, goalPercept); );
}


void GoalPerceptor::preExecution(GoalPercept& goalPercept)
{
  oppColor = theOwnTeamInfo.teamColor == TEAM_RED ? ColorClasses::blue : ColorClasses::yellow;
  ownColor = theOwnTeamInfo.teamColor == TEAM_RED ? ColorClasses::yellow : ColorClasses::blue;
  odometryOffset = theOdometryData - lastOdometry;
  goalPercept.ownTeamColorForDrawing = ownColor;
  for(unsigned int i=0; i<GoalPercept::NUMBER_OF_UNKNOWN_GOAL_POSTS; ++i)
    goalPercept.unknownPosts[i].perceptionType = GoalPost::NEVER_SEEN; // temporary fix
}

void GoalPerceptor::motionUpdate(GoalPercept& goalPercept)
{
  for(int i=0; i<GoalPercept::NUMBER_OF_GOAL_POSTS; i++)
  {
    goalPercept.posts[i].positionOnField -= Vector2<int>((int) odometryOffset.translation.x, (int) odometryOffset.translation.y);
    goalPercept.posts[i].positionOnField.rotate(-odometryOffset.rotation);
    goalPercept.posts[i].perceptionType = GoalPost::CALCULATED;
  }
}

void GoalPerceptor::detectGoal(ColorClasses::Color goalColor, PolePercept polePercepts[])
{
  // Initialize two new percepts
  PolePercept polePercept1;
  polePercept1.color = goalColor;
  polePercept1.yImageTop = theImage.cameraInfo.resolutionHeight-1;
  PolePercept polePercept2;
  polePercept2.color = goalColor;
  polePercept2.yImageTop = theImage.cameraInfo.resolutionHeight-1;
  polePercepts[0] = polePercept1;
  polePercepts[1] = polePercept2;

  // build accumulator of horizontal segment percepts
  vector<int> verticalAcc (theImage.cameraInfo.resolutionWidth, 0);
  for(unsigned int i=0; i<theRegionPercept.horizontalPostSegments.size(); i++)
  {
    RegionPercept::Segment segment = theRegionPercept.horizontalPostSegments[i];
    if(segment.color == goalColor && segment.length > 3 && segment.length < 100)
    {
      for(int i=segment.x; i<segment.x+segment.length; i++)
        verticalAcc[i]++;
    }
  }
  
  // find best x-coordinate from accumulator
  for(int p=0; p<MAX_NUMBER_OF_PERCEPTS; p++)
  {
    for(int j=0; j<(int) theImage.cameraInfo.resolutionWidth; j++)
    {
      int points = verticalAcc[j];
      if(points > 4)
      {
        if(points >= parameters.PoleAccThreshold && points >= polePercepts[p].accumulatorMatchSize)
        {
          if(p==0 || abs(j-polePercepts[0].xImage) >= parameters.PoleAccMinDistance)
          {
            polePercepts[p].xImage = j;
            polePercepts[p].xImageTop = j;
            polePercepts[p].xImageBottom = j;
            polePercepts[p].accumulatorMatchSize = points;
          }
        }
      }
    }
  }

  // Calculate distance to horizon
  cameraHeight = int(theCameraMatrix.translation.z + 0.5);
  double pan = theCameraMatrix.rotation.getZAngle();
  if(pan >= pi)
    pan -= pi2;
  else if(pan < -pi)
    pan += pi2;
  origin = Pose2D(pan, theCameraMatrix.translation.x, theCameraMatrix.translation.y);
  RotationMatrix r(theCameraMatrix.rotation.invert());
  r.rotateZ(pan);
  double tilt = r.getYAngle();
  r.rotateY(-tilt);
  double roll = r.getXAngle();
  double factor = theImage.cameraInfo.focalLengthInv * (1 << 20);
  rotCos = int(cos(-roll) * factor + 0.5);
  rotSin = int(sin(-roll) * factor + 0.5);
  tiltCos2 = int(cos(-tilt) * (1 << 20) + 0.5);
  tiltSin2 = int(sin(-tilt) * (1 << 20) + 0.5);
  tiltCos = tiltCos2 >> 10;
  tiltSin = tiltSin2 >> 10;

  int yHorizon = theImageCoordinateSystem.fromHorizonBased(Vector2<double>()).y,
      yHorizonClipped = std::min(theImage.cameraInfo.resolutionHeight - 1, std::max(0, yHorizon)),
      yHorizonCorrected = (int) theImageCoordinateSystem.toCorrected(Vector2<int>(0, yHorizonClipped)).y;

  yHorizon += yHorizonClipped - yHorizonCorrected;
  yHorizon = std::max(0, yHorizon);

  int numberOfPercepts = 0;
  if(polePercepts[0].xImage != 0)
    numberOfPercepts++;
  if(polePercepts[1].xImage != 0)
    numberOfPercepts++;

  for(int p=0; p<numberOfPercepts; p++)
  {
    int segmentsOnPole = 0;
    vector<RegionPercept::Segment> segmentsOnPoleVector;
    for(int i=theRegionPercept.horizontalPostSegments.size()-1; i>=0; i--)
    {
      RegionPercept::Segment segment = theRegionPercept.horizontalPostSegments[i];
      if(segment.color == goalColor)
      {
        // from all segments with intersections find the segment with greatest and lowest y-coordinate
        // calculate percentage of coverage with correct colored segments
        if((segment.x <= polePercepts[p].xImage)
          && (segment.x + segment.length >= polePercepts[p].xImage))
        {
          segmentsOnPoleVector.push_back(segment);
          segmentsOnPole++;
          if(segment.y > polePercepts[p].yImageBottom && (polePercepts[p].yImageBottom == 0 || (segment.y - polePercepts[p].yImageBottom) <= 20))
            polePercepts[p].yImageBottom = segment.y;
          if(segment.y < polePercepts[p].yImageTop)
            polePercepts[p].yImageTop = segment.y;
        }
      }
    }
    double segmentsOnPoleMax = (double) (polePercepts[p].yImageBottom - polePercepts[p].yImageTop) / parameters.gridStepSize;
    if((segmentsOnPole / segmentsOnPoleMax) < 0.8)
    {
      polePercepts[p].discarded = true;
      continue; // next percept
    }

    // find the most accurate footpoint (look into the image for this because it could not be on a scanline)
    int xCorrection = 0;
    while(polePercepts[p].yImageBottom + 1 < theImage.cameraInfo.resolutionHeight 
      && polePercepts[p].xImage + xCorrection > 2 
      && polePercepts[p].xImage + xCorrection + 2 < theImage.cameraInfo.resolutionWidth)
    {
      if(convertColorAt(polePercepts[p].xImage + xCorrection, polePercepts[p].yImageBottom+1) == goalColor)
      {
        polePercepts[p].yImageBottom++;
        continue;
      }
      if(convertColorAt(polePercepts[p].xImage + xCorrection + 1, polePercepts[p].yImageBottom+1) == goalColor)
      {
        polePercepts[p].yImageBottom++;
        xCorrection++;
        continue;
      }
      if(convertColorAt(polePercepts[p].xImage + xCorrection + 2, polePercepts[p].yImageBottom+1) == goalColor)
      {
        polePercepts[p].yImageBottom++;
        xCorrection+=2;
        continue;
      }
      if(convertColorAt(polePercepts[p].xImage + xCorrection - 1, polePercepts[p].yImageBottom+1) == goalColor)
      {
        polePercepts[p].yImageBottom++;
        xCorrection--;
        continue;
      }
      if(convertColorAt(polePercepts[p].xImage + xCorrection - 2, polePercepts[p].yImageBottom+1) == goalColor)
      {
        polePercepts[p].yImageBottom++;
        xCorrection-=2;
        continue;
      }
      break;
    }
    
    polePercepts[p].xImageBottom += xCorrection;
    
    int toRight = 0;
    while(polePercepts[p].xImageBottom + toRight < theImage.cameraInfo.resolutionHeight
      && convertColorAt(polePercepts[p].xImageBottom + toRight + 1, polePercepts[p].yImageBottom) == goalColor)
    {
      toRight++;
    }

    int toLeft = 0;
    while(polePercepts[p].xImageBottom + toLeft < theImage.cameraInfo.resolutionHeight
      && convertColorAt(polePercepts[p].xImageBottom + toLeft - 1, polePercepts[p].yImageBottom) == goalColor)
    {
      toLeft--;
    }

    polePercepts[p].xImageBottom += (toRight + toLeft)/2;

    if(polePercepts[p].yImageBottom >= theImage.cameraInfo.resolutionHeight - parameters.pixelsToTreatPoleAsNear 
      || polePercepts[p].xImageBottom <= 2 
      || polePercepts[p].xImageBottom >= theImage.cameraInfo.resolutionWidth - 2)
      polePercepts[p].footVisible = false;

    // check whether percepted pole footpoint is below horizon
    if(polePercepts[p].yImageBottom < yHorizon - 5) // TODO
    {
      polePercepts[p].discarded = true;
      continue; // next percept
    }
    
    // find more accurate top y-coordinate
    xCorrection = 0;
    while(polePercepts[p].yImageTop > 0
      && polePercepts[p].xImage + xCorrection > 2 
      && polePercepts[p].xImage + xCorrection + 2 < theImage.cameraInfo.resolutionWidth)
    {
      if(convertColorAt(polePercepts[p].xImage + xCorrection, polePercepts[p].yImageTop-1) == goalColor)
      {
        polePercepts[p].yImageTop--;
        continue;
      }
      if(convertColorAt(polePercepts[p].xImage + xCorrection + 1, polePercepts[p].yImageTop-1) == goalColor)
      {
        polePercepts[p].yImageTop--;
        xCorrection++;
        continue;
      }
      if(convertColorAt(polePercepts[p].xImage + xCorrection + 2, polePercepts[p].yImageTop-1) == goalColor)
      {
        polePercepts[p].yImageTop--;
        xCorrection+=2;
        continue;
      }
      if(convertColorAt(polePercepts[p].xImage + xCorrection - 1, polePercepts[p].yImageTop-1)  == goalColor)
      {
        polePercepts[p].yImageTop--;
        xCorrection--;
        continue;
      }
      if(convertColorAt(polePercepts[p].xImage + xCorrection - 2, polePercepts[p].yImageTop-1) == goalColor)
      {
        polePercepts[p].yImageTop--;
        xCorrection-=2;
        continue;
      }

      break;
    }

    polePercepts[p].xImageTop += xCorrection;
    
    /*
    // check whether percepted pole has > 80% goalColor pixels
    double xd = (double) polePercepts[p].xImageTop;
    int yd = polePercepts[p].yImageTop;
    int height = polePercepts[p].yImageBottom - polePercepts[p].yImageTop;
    int pixelCorrectCount = 0;
    int widthDiff = polePercepts[p].xImageBottom - polePercepts[p].xImageTop;
    while(yd <= polePercepts[p].yImageBottom)
    {
      unsigned int pixelColor = theImage.image[yd][(int) (xd + 0.5)].color;
      ColorClasses::Color pixelColorConverted = (ColorClasses::Color) theColorTable64.colorClasses[pixelColor >> 18 & 0x3f][pixelColor >> 10 & 0x3f][pixelColor >> 26 & 0x3f];

      if(pixelColorConverted == goalColor)
        pixelCorrectCount++;

      xd = polePercepts[p].xImageTop + ((double)((yd - polePercepts[p].yImageTop)*widthDiff))/((double)height);
      yd++;
    }
    if(((double) pixelCorrectCount) / ((double) height) < 0.80)
      polePercepts[p].discarded = true;
    */
    
    // compare width of segments with expected width
    int expectedWidth = calculateExpectedPixelWidth(Vector2<int>(polePercepts[p].xImageBottom, polePercepts[p].yImageBottom));
    LINE("module:GoalPerceptor:width", polePercepts[p].xImageBottom - expectedWidth/2, polePercepts[p].yImageBottom, polePercepts[p].xImageBottom + expectedWidth / 2, polePercepts[p].yImageBottom, 2, Drawings::ps_solid, ColorClasses::red);
    
    if(goalColor == ColorClasses::blue)
    {
      int maxAcceptedWidth = (int)(expectedWidth * 1.7 + 0.5);
      int minAcceptedWidth = (int)(expectedWidth * 0.6 + 0.5);
      int segmentsOnPoleVectorAccepted = 0;

      for(unsigned int i=0; i<segmentsOnPoleVector.size(); i++)
      {
        RegionPercept::Segment segment = segmentsOnPoleVector[i];
        if(segment.length >= minAcceptedWidth && segment.length <= maxAcceptedWidth)
          segmentsOnPoleVectorAccepted++;
      }
      
      if(segmentsOnPoleVectorAccepted / ((double) segmentsOnPoleMax) < 0.6)
      {
        polePercepts[p].discarded = true;
        continue; // next percept
      }
    }

    // check whether percepted pole is a based on green/white/orange
    /*
    short wrongColoredPixels = 0;
    short scannedPixels = 0;
    for(int i=polePercepts[p].yImageBottom; i<theImage.cameraInfo.resolutionHeight && i<polePercepts[p].yImageBottom+10; i++)
    {
      scannedPixels++;
      ColorClasses::Color colorConverted = convertColorAt(polePercepts[p].xImageBottom, i);
      if(colorConverted != ColorClasses::white && colorConverted != ColorClasses::green && colorConverted != ColorClasses::orange)
        wrongColoredPixels++;
    }


    if((wrongColoredPixels / ((double) scannedPixels)) >= .5)
    {
      polePercepts[p].discarded = true;
      polePercepts[p].footVisible = false;
      continue; // next percept
    }
    */

    int firstGreen = theImage.cameraInfo.resolutionHeight;
    for(int i=-1; i<2; i++)
    {
      int x = polePercepts[p].xImageBottom + i * expectedWidth / 2;
      if(x >= 0 && x < theImage.cameraInfo.resolutionWidth)
      {
        int firstGreenCurrentScan = pointExplorer.findDown(x, polePercepts[p].yImageBottom, ColorClasses::green, theImage.cameraInfo.resolutionHeight, Drawings::ps_dot);
        if(firstGreenCurrentScan < firstGreen)
          firstGreen = firstGreenCurrentScan;
      }
    }

    if(firstGreen > polePercepts[p].yImageBottom + 10 || firstGreen == theImage.cameraInfo.resolutionHeight)
    {
      polePercepts[p].discarded = true;
      continue; // next percept
    }
    else
      LINE("module:GoalPerceptor:green", polePercepts[p].xImageBottom - expectedWidth/2, firstGreen, polePercepts[p].xImageBottom + expectedWidth / 2, firstGreen, 2, Drawings::ps_solid, ColorClasses::green);

    if(polePercepts[p].yImageTop <= 5)
    {
      polePercepts[p].headVisible = false;
    }

    // calculate width of pole
    int leftScanned = 0;
    int rightScanned = 0;

    int y = (polePercepts[p].yImageBottom - polePercepts[p].yImageTop) / 2 + polePercepts[p].yImageTop;
    // scan to left
    for(int x = polePercepts[p].xImage; x >= 0 && x >= polePercepts[p].xImage - 75; x--)
    {
      if(convertColorAt(x, y) == goalColor)
        leftScanned++;
    }
    // scan to right
    for(int x = polePercepts[p].xImage; x < theImage.cameraInfo.resolutionWidth && x <= polePercepts[p].xImage + 75; x++)
    {
      if(convertColorAt(x, y) == goalColor)
        rightScanned++;
    }

    polePercepts[p].width = leftScanned + rightScanned;
  }

  // if we see both poles
  if(polePercepts[0].xImage != 0 && polePercepts[1].xImage != 0 && !polePercepts[0].discarded && !polePercepts[1].discarded)
  {
    polePercepts[0].timeWhenLastSeen = theFrameInfo.time;
    polePercepts[1].timeWhenLastSeen = theFrameInfo.time;
    polePercepts[0].seenInThisFrame = true;
    polePercepts[1].seenInThisFrame = true;

    if(polePercepts[0].headVisible && polePercepts[1].headVisible)
    {
      int x1 = polePercepts[0].xImageTop;
      int x2 = polePercepts[1].xImageTop;
      int y1 = polePercepts[0].yImageTop;
      int y2 = polePercepts[1].yImageTop;
      LINE("module:GoalPerceptor:image", x1, y1, x2, y2, 5, Drawings::bs_solid, ColorClasses::white);
    }
    
    if(polePercepts[0].xImage < polePercepts[1].xImage)
    {
      polePercepts[0].poleSide = PolePercept::LEFT;
      polePercepts[1].poleSide = PolePercept::RIGHT;
    }
    else
    {
      polePercepts[0].poleSide = PolePercept::RIGHT;
      polePercepts[1].poleSide = PolePercept::LEFT;
    }
    pointToField(polePercepts[0].xImageBottom, polePercepts[0].yImageBottom, polePercepts[0].onField);
    pointToField(polePercepts[1].xImageBottom, polePercepts[1].yImageBottom, polePercepts[1].onField);
    Vector2<double> lAddon(theFieldDimensions.goalPostRadius, 0);
    lAddon.rotate(polePercepts[0].onField.angle());
    polePercepts[0].onField += lAddon;
    Vector2<double> rAddon(theFieldDimensions.goalPostRadius, 0);
    rAddon.rotate(polePercepts[1].onField.angle());
    polePercepts[1].onField += rAddon;
  }
  // if we see only one pole
  else if(polePercepts[0].xImage != 0 && !polePercepts[0].discarded)
  {
    bool poleIdentified = false;
    int left  = 0;
    int right = 0;

    // crossbar detection
    if(polePercepts[0].headVisible)
    {
      //bool crossbarDetected = false;
      int xScanOffset = 150;
      int yScanTop = polePercepts[0].yImageTop - 30;
      int yScanBottom = polePercepts[0].yImageTop + 30;
      if(polePercepts[0].xImage < xScanOffset)
        xScanOffset = polePercepts[0].xImage;
      if(polePercepts[0].xImage >= theImage.cameraInfo.resolutionWidth - xScanOffset)
        xScanOffset = theImage.cameraInfo.resolutionWidth - polePercepts[0].xImage - 1;

      if(xScanOffset > 25)
      {
        //LINE("module:GoalPerceptor:image", polePercepts[0].xImageTop - xScanOffset, yScanTop, polePercepts[0].xImageTop + xScanOffset, yScanTop, 1, Drawings::bs_solid, ColorClasses::white);
        //LINE("module:GoalPerceptor:image", polePercepts[0].xImageTop - xScanOffset, yScanBottom, polePercepts[0].xImageTop + xScanOffset, yScanBottom, 1, Drawings::bs_solid, ColorClasses::white);
        
        // new crossbar detection
        for(unsigned int i=0; i<theRegionPercept.verticalPostSegments.size(); i++)
        {
          RegionPercept::Segment segment = theRegionPercept.verticalPostSegments[i];
          
          if(segment.color == goalColor && segment.length > 3 && segment.length < 50 && abs(polePercepts[0].xImageTop - segment.x) <= xScanOffset 
            && ((segment.y >= yScanTop && segment.y <= yScanBottom) || (segment.y - segment.length >= yScanTop && segment.y - segment.length <= yScanBottom)))
          {
            if(segment.x > polePercepts[0].xImageTop)
              right++;
            else
              left++;
          }
        }
      }

      // if the crossbar has not been identified clearly
      if(abs(right - left) > 3)
      {
        poleIdentified = true;
        COMPLEX_DRAWING("module:GoalPerceptor:image", {
          int x2 = (left > right ? polePercepts[0].xImageTop - 40 : polePercepts[0].xImageTop + 40);
          if(x2 < 0) x2 = 0;
          if(x2 >= theImage.cameraInfo.resolutionWidth) x2 = theImage.cameraInfo.resolutionWidth - 1;
          ARROW("module:GoalPerceptor:image", polePercepts[0].xImageTop, polePercepts[0].yImageTop, x2, polePercepts[0].yImageTop, 1, Drawings::bs_solid, ColorClasses::white);
        });
      }
      // try to find small part of crossbar
      else
      {
        left = 0;
        right = 0;
        int xls = polePercepts[0].xImageTop - polePercepts[0].width * 2;
        int xrs = polePercepts[0].xImageTop + polePercepts[0].width * 2;
        if(xls >= 0 && xrs < theImage.cameraInfo.resolutionWidth && polePercepts[0].headVisible)
        {
          int yss = polePercepts[0].yImageTop - 40;
          int yse = polePercepts[0].yImageTop + 40;
          if(yss < 0) yss = 0;
          if(yse >= theImage.cameraInfo.resolutionHeight) yse = theImage.cameraInfo.resolutionHeight - 1;

          for(int y = yss; y <= yse; y++)
          {
            if(convertColorAt(xls, y) == goalColor)
              left++;
            if(convertColorAt(xrs, y) == goalColor)
              right++;
          }

          LINE("module:GoalPerceptor:image", xls, yss, xls, yse, 1, Drawings::bs_solid, goalColor);
          LINE("module:GoalPerceptor:image", xrs, yss, xrs, yse, 1, Drawings::bs_solid, goalColor);

          if(left + right != 0)
          {
            poleIdentified = true;
            ARROW("module:GoalPerceptor:image", polePercepts[0].xImageTop, polePercepts[0].yImageTop, right > left ? xrs : xls, polePercepts[0].yImageTop, 1, Drawings::ps_solid, ColorClasses::white);
          }
        }
      }
    }

    polePercepts[0].timeWhenLastSeen = theFrameInfo.time;
    polePercepts[0].seenInThisFrame = true;
    pointToField(polePercepts[0].xImageBottom, polePercepts[0].yImageBottom, polePercepts[0].onField);
    Vector2<double> addon(theFieldDimensions.goalPostRadius, 0.0);
    addon.rotate(polePercepts[0].onField.angle());
    polePercepts[0].onField += addon;

    // if the pole was identified update pole position
    if(poleIdentified)
    {
      if(right > left)
      {
        polePercepts[0].poleSide = PolePercept::LEFT;
      }
      else
      {
        polePercepts[0].poleSide = PolePercept::RIGHT;
      }
    }
    else
      polePercepts[0].poleSide = PolePercept::UNDEFINED;
  }

  // discard misplaced poles
  Vector3<> field;
  for(int p = 0; p < numberOfPercepts; ++p)
    if(!polePercepts[p].discarded)
    {
      double distance = polePercepts[p].onField.abs(); 
      double distanceByHead, distanceByFoot;
      bool headOverHorizon, footUnderHorizon;

      {        
        const Vector2<>& corrected(theImageCoordinateSystem.toCorrected(Vector2<int>(polePercepts[p].xImageTop, polePercepts[p].yImageTop)));
        headOverHorizon = Geometry::calculatePointOnField(corrected, double(theFieldDimensions.goalHeight), theCameraMatrix, theImage.cameraInfo, field);
        distanceByHead = Vector2<>(field.x, field.y).abs();
      }
      {
        const Vector2<>& corrected(theImageCoordinateSystem.toCorrected(Vector2<int>(polePercepts[p].xImageBottom, polePercepts[p].yImageBottom)));
        footUnderHorizon = Geometry::calculatePointOnField(corrected, 0, theCameraMatrix, theImage.cameraInfo, field);
        distanceByFoot = Vector2<>(field.x, field.y).abs();
      }

      if(polePercepts[p].headVisible && !headOverHorizon)
        polePercepts[p].discarded2 = true;

      if(polePercepts[p].headVisible && headOverHorizon && (fabs(distance - distanceByHead) / std::max<>(distance, distanceByHead)) > parameters.allowedDistanceNoise)
        polePercepts[p].discarded2 = true;

      if(!polePercepts[p].headVisible && headOverHorizon && ((distanceByHead - distance) / std::max<>(distance, distanceByHead)) <  -parameters.allowedDistanceNoise)
        polePercepts[p].discarded2 = true;

      if(polePercepts[p].footVisible && !footUnderHorizon)
        polePercepts[p].discarded2 = true;

      if(polePercepts[p].footVisible && footUnderHorizon && (fabs(distance - distanceByFoot) / std::max<>(distance, distanceByFoot)) > parameters.allowedDistanceNoise)
        polePercepts[p].discarded2 = true;


      if(polePercepts[p].discarded2)
        polePercepts[p].discarded = true;

#ifndef RELEASE
      if(polePercepts[p].headVisible)
      {
        if(goalColor == ColorClasses::blue)
        {
          switch(p)
          {
          case 0:
            PLOT("module:GoalPerceptor:bluePoleDistance0", distance);
            PLOT("module:GoalPerceptor:bluePoleDistance0byHead", distanceByHead);
            PLOT("module:GoalPerceptor:bluePoleDistance0byFoot", distanceByFoot);
            break;
          case 1:
            PLOT("module:GoalPerceptor:bluePoleDistance1", distance);
            PLOT("module:GoalPerceptor:bluePoleDistance1byHead", distanceByHead);
            PLOT("module:GoalPerceptor:bluePoleDistance1byFoot", distanceByFoot);
            break;
          }
        }
        else
        {
          switch(p)
          {
          case 0:
            PLOT("module:GoalPerceptor:yellowPoleDistance0", distance);
            PLOT("module:GoalPerceptor:yellowPoleDistance0byHead", distanceByHead);
            PLOT("module:GoalPerceptor:yellowPoleDistance0byFoot", distanceByFoot);
            break;
          case 1:
            PLOT("module:GoalPerceptor:yellowPoleDistance1", distance);
            PLOT("module:GoalPerceptor:yellowPoleDistance1byHead", distanceByHead);
            PLOT("module:GoalPerceptor:yellowPoleDistance1byFoot", distanceByFoot);
            break;
          }
        }
      }
#endif
    }

  // draw poles
  for(int p=0; p<numberOfPercepts; p++)
  {
    ColorClasses::Color goalDrawColor = goalColor;
    if(polePercepts[p].discarded)
    {
      goalDrawColor = ColorClasses::red;
    }
    if(polePercepts[p].discarded2)
    {
      goalDrawColor = ColorClasses::green;
    }

    LINE("module:GoalPerceptor:image", polePercepts[p].xImageBottom, polePercepts[p].yImageBottom,
      polePercepts[p].xImageTop, polePercepts[p].yImageTop, 5, polePercepts[p].poleSide == PolePercept::UNDEFINED ? Drawings::ps_dot : Drawings::ps_solid, goalDrawColor);

    if(polePercepts[p].footVisible)
    {
      DOT("module:GoalPerceptor:image", polePercepts[p].xImageBottom, polePercepts[p].yImageBottom, ColorClasses::orange, ColorClasses::orange);
    }
    if(polePercepts[p].headVisible)
    {
      DOT("module:GoalPerceptor:image", polePercepts[p].xImageTop, polePercepts[p].yImageTop, ColorClasses::orange, ColorClasses::orange);
    }
  }

  // If a pole is completely visible distance can be calculated from height
  for(int p=0; p<numberOfPercepts; p++)
  {
    if(!polePercepts[p].discarded && polePercepts[p].headVisible && polePercepts[p].footVisible && polePercepts[p].poleSide != PolePercept::UNDEFINED)
    {
      const PolePercept& pp = polePercepts[p];
      Vector2<double> anglesBottom;
      Geometry::calculateAnglesForPoint(Vector2<int>(pp.xImageBottom, pp.yImageBottom), theCameraMatrix, theImage.cameraInfo, anglesBottom);
      Vector2<double> anglesTop;
      Geometry::calculateAnglesForPoint(Vector2<int>(pp.xImageTop, pp.yImageTop), theCameraMatrix, theImage.cameraInfo, anglesTop);
      if(anglesTop.y != anglesBottom.y && polePercepts[p].onField.abs())
      {
        double distanceCorrected = theFieldDimensions.goalHeight / (tan(anglesTop.y) - tan(anglesBottom.y));
        Vector2<double> addon (distanceCorrected - polePercepts[p].onField.abs(), 0);
        addon.rotate(polePercepts[p].onField.angle());
        polePercepts[p].onField+=addon;
        polePercepts[p].distanceType = GoalPost::HEIGHT_BASED;
      }
    }
    else if(polePercepts[p].xImage != 0 && !polePercepts[p].footVisible)
      polePercepts[p].distanceType = GoalPost::IS_CLOSER;
  }
}

void GoalPerceptor::updateRepresentation(GoalPercept& goalPercept, PolePercept oppPercepts[], PolePercept ownPercepts[])
{
  // if we see two complete goals -> discard both
  if(oppPercepts[0].seenInThisFrame && !oppPercepts[0].discarded && oppPercepts[0].poleSide != PolePercept::UNDEFINED
    && oppPercepts[1].seenInThisFrame && !oppPercepts[1].discarded && oppPercepts[1].poleSide != PolePercept::UNDEFINED
    && ownPercepts[0].seenInThisFrame && !ownPercepts[0].discarded && ownPercepts[0].poleSide != PolePercept::UNDEFINED
    && ownPercepts[1].seenInThisFrame && !ownPercepts[1].discarded && ownPercepts[1].poleSide != PolePercept::UNDEFINED)
  {
  }
  // if we see opp goal complete -> discard own goal
  else if(oppPercepts[0].seenInThisFrame && !oppPercepts[0].discarded && oppPercepts[0].poleSide != PolePercept::UNDEFINED
    && oppPercepts[1].seenInThisFrame && !oppPercepts[1].discarded && oppPercepts[1].poleSide != PolePercept::UNDEFINED)
  {
    if(oppPercepts[0].poleSide == PolePercept::LEFT)
    {
      createGoalPercept(goalPercept, 0, oppPercepts[0]);
      createGoalPercept(goalPercept, 1, oppPercepts[1]);
    }
    else
    {
      createGoalPercept(goalPercept, 1, oppPercepts[0]);
      createGoalPercept(goalPercept, 0, oppPercepts[1]);
    }
  }
  // if we see own goal complete -> discard opp goal
  else if(ownPercepts[0].seenInThisFrame && !ownPercepts[0].discarded && ownPercepts[0].poleSide != PolePercept::UNDEFINED
    && ownPercepts[1].seenInThisFrame && !ownPercepts[1].discarded && ownPercepts[1].poleSide != PolePercept::UNDEFINED)
  {
    if(ownPercepts[0].poleSide == PolePercept::LEFT)
    {
      createGoalPercept(goalPercept, 2, ownPercepts[0]);
      createGoalPercept(goalPercept, 3, ownPercepts[1]);
    }
    else
    {
      createGoalPercept(goalPercept, 3, ownPercepts[0]);
      createGoalPercept(goalPercept, 2, ownPercepts[1]);
    }
  }
  // if we see opp goal nearly complete -> discard own goal
  else if(oppPercepts[0].seenInThisFrame && !oppPercepts[0].discarded
    && oppPercepts[1].seenInThisFrame && !oppPercepts[1].discarded
    && (oppPercepts[0].poleSide != PolePercept::UNDEFINED || oppPercepts[1].poleSide != PolePercept::UNDEFINED))
  {
    if(oppPercepts[0].poleSide == PolePercept::UNDEFINED)
      createUnknownGoalPercept(goalPercept, 0, oppPercepts[0]);
    else if(oppPercepts[0].poleSide == PolePercept::LEFT)
      createGoalPercept(goalPercept, 0, oppPercepts[0]);
    else
      createGoalPercept(goalPercept, 1, oppPercepts[0]);
     
    if(oppPercepts[1].poleSide == PolePercept::UNDEFINED)
      createUnknownGoalPercept(goalPercept, 0, oppPercepts[1]);
    else if(oppPercepts[1].poleSide == PolePercept::LEFT)
      createGoalPercept(goalPercept, 0, oppPercepts[1]);
    else
      createGoalPercept(goalPercept, 1, oppPercepts[1]);
  }
  // if we see own goal nearly complete -> discard opp goal
  else if(ownPercepts[0].seenInThisFrame && !ownPercepts[0].discarded
    && ownPercepts[1].seenInThisFrame && !ownPercepts[1].discarded
    && (ownPercepts[0].poleSide != PolePercept::UNDEFINED || ownPercepts[1].poleSide != PolePercept::UNDEFINED))
  {
    if(ownPercepts[0].poleSide == PolePercept::UNDEFINED)
      createUnknownGoalPercept(goalPercept, 1, ownPercepts[0]);
    else if(ownPercepts[0].poleSide == PolePercept::LEFT)
      createGoalPercept(goalPercept, 2, ownPercepts[0]);
    else
      createGoalPercept(goalPercept, 3, ownPercepts[0]);
     
    if(ownPercepts[1].poleSide == PolePercept::UNDEFINED)
      createUnknownGoalPercept(goalPercept, 1, ownPercepts[1]);
    else if(ownPercepts[1].poleSide == PolePercept::LEFT)
      createGoalPercept(goalPercept, 2, ownPercepts[1]);
    else
      createGoalPercept(goalPercept, 3, ownPercepts[1]);
  }
  // left opp
  else if(oppPercepts[0].seenInThisFrame && !oppPercepts[1].discarded
    && (!ownPercepts[0].seenInThisFrame || ownPercepts[0].discarded)
    && (!ownPercepts[1].seenInThisFrame || ownPercepts[1].discarded))
  {
    if(oppPercepts[0].poleSide == PolePercept::UNDEFINED)
      createUnknownGoalPercept(goalPercept, 0, oppPercepts[0]);
    else if(oppPercepts[0].poleSide == PolePercept::LEFT)
      createGoalPercept(goalPercept, 0, oppPercepts[0]);
    else
      createGoalPercept(goalPercept, 1, oppPercepts[0]);
  }
  // right opp
  else if(oppPercepts[1].seenInThisFrame && !oppPercepts[1].discarded
    && (!ownPercepts[0].seenInThisFrame || ownPercepts[0].discarded)
    && (!ownPercepts[1].seenInThisFrame || ownPercepts[1].discarded))
  {
    if(oppPercepts[1].poleSide == PolePercept::UNDEFINED)
      createUnknownGoalPercept(goalPercept, 0, oppPercepts[1]);
    else if(oppPercepts[1].poleSide == PolePercept::LEFT)
      createGoalPercept(goalPercept, 0, oppPercepts[1]);
    else
      createGoalPercept(goalPercept, 1, oppPercepts[1]);
  }
  // left own
  else if(ownPercepts[0].seenInThisFrame && !ownPercepts[0].discarded
    && (!oppPercepts[0].seenInThisFrame || oppPercepts[0].discarded)
    && (!oppPercepts[1].seenInThisFrame || oppPercepts[1].discarded))
  {
    if(ownPercepts[0].poleSide == PolePercept::UNDEFINED)
      createUnknownGoalPercept(goalPercept, 1, ownPercepts[0]);
    else if(ownPercepts[0].poleSide == PolePercept::LEFT)
      createGoalPercept(goalPercept, 2, ownPercepts[0]);
    else
      createGoalPercept(goalPercept, 3, ownPercepts[0]);
  }
  // right own
  else if(ownPercepts[1].seenInThisFrame && !ownPercepts[1].discarded
    && (!oppPercepts[0].seenInThisFrame || oppPercepts[0].discarded)
    && (!oppPercepts[1].seenInThisFrame || oppPercepts[1].discarded))
  {
    if(ownPercepts[1].poleSide == PolePercept::UNDEFINED)
      createUnknownGoalPercept(goalPercept, 1, ownPercepts[1]);
    else if(ownPercepts[1].poleSide == PolePercept::LEFT)
      createGoalPercept(goalPercept, 2, ownPercepts[1]);
    else
      createGoalPercept(goalPercept, 3, ownPercepts[1]);
  }

  if(goalPercept.posts[0].timeWhenLastSeen == theFrameInfo.time || goalPercept.posts[1].timeWhenLastSeen == theFrameInfo.time)
    goalPercept.timeWhenOppGoalLastSeen = theFrameInfo.time;

  if(goalPercept.posts[2].timeWhenLastSeen == theFrameInfo.time || goalPercept.posts[3].timeWhenLastSeen == theFrameInfo.time)
    goalPercept.timeWhenOwnGoalLastSeen = theFrameInfo.time;
}

void GoalPerceptor::createGoalPercept(GoalPercept& goalPercept, int index, PolePercept percept)
{
  goalPercept.posts[index].distanceType     = percept.distanceType;
  goalPercept.posts[index].perceptionType   = GoalPost::SEEN_IN_IMAGE;
  goalPercept.posts[index].positionInImage  = Vector2<int> (percept.xImageBottom, percept.yImageBottom);
  goalPercept.posts[index].positionOnField  = Vector2<int> ((int) percept.onField.x, (int) percept.onField.y);
  goalPercept.posts[index].timeWhenLastSeen = percept.timeWhenLastSeen;
}

void GoalPerceptor::createUnknownGoalPercept(GoalPercept& goalPercept, int index, PolePercept percept)
{
  goalPercept.unknownPosts[index].distanceType     = percept.distanceType;
  goalPercept.unknownPosts[index].perceptionType   = GoalPost::SEEN_IN_IMAGE;
  goalPercept.unknownPosts[index].positionInImage  = Vector2<int> (percept.xImageBottom, percept.yImageBottom);
  goalPercept.unknownPosts[index].positionOnField  = Vector2<int> ((int) percept.onField.x, (int) percept.onField.y);
  goalPercept.unknownPosts[index].timeWhenLastSeen = percept.timeWhenLastSeen;
}

void GoalPerceptor::postExecution()
{
  lastExecution = theFrameInfo.time;
  lastOdometry = theOdometryData;
}

void GoalPerceptor::draw()
{
}

void GoalPerceptor::pointToField(int x, int y, Vector2<double>& p) const
{
  Vector2<int> v = theImageCoordinateSystem.toCorrectedCenteredNeg(x, y);
  int b0 = (v.x * rotSin + v.y * rotCos) >> 10,
  b1 = tiltCos2 + b0 * tiltSin,
  b2 = v.x * rotCos - v.y * rotSin,
  b3 = tiltSin2 - b0 * tiltCos;
  if (b3 == 0) b3 = 1;
  p.x = (double) (b1 * cameraHeight / b3);
  p.y = (double) (b2 * cameraHeight / b3);
  p = (origin + Pose2D(p)).translation;
}

int GoalPerceptor::calculateExpectedPixelWidth(Vector2<int> posImg)
{
  Vector2<double> pos;
  if(!Geometry::calculatePointOnFieldHacked(posImg.x, posImg.y, theCameraMatrix, theImage.cameraInfo, pos))
    return -1;
  Vector2<double> p1 = pos + Vector2<double>(0, theFieldDimensions.goalPostRadius);
  Vector2<double> p2 = pos - Vector2<double>(0, theFieldDimensions.goalPostRadius);

  Vector2<int> p1Img, p2Img;

  if(!Geometry::calculatePointInImage(Vector2<int>((int)p1.x, (int)p1.y), theCameraMatrix, theImage.cameraInfo, p1Img))
    return -1;
  if(!Geometry::calculatePointInImage(Vector2<int>((int)p2.x, (int)p2.y), theCameraMatrix, theImage.cameraInfo, p2Img))
    return -1;

  Vector2<double> p1Uncor = theImageCoordinateSystem.fromCorrectedApprox(p1Img);
  Vector2<double> p2Uncor = theImageCoordinateSystem.fromCorrectedApprox(p2Img);

  return (int)fabs(p1Uncor.x - p2Uncor.x);
}

ColorClasses::Color GoalPerceptor::convertColorAt(int x, int y)
{
  unsigned color(theImage.image[y][x].color);
  return (ColorClasses::Color) theColorTable64.colorClasses[color >> 18 & 0x3f][color >> 10 & 0x3f][color >> 26 & 0x3f];
}

MAKE_MODULE(GoalPerceptor, Perception)
