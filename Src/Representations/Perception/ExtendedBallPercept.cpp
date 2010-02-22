/**
* @file ExtendedBallPercept.cpp
*
* Very simple representation of a seen ball
*
* @author <a href="mailto:timlaue@informatik.uni-bremen.de">Tim Laue</a>
* @author <a href="mailto:ingsie@informatik.uni-bremen.de">Ingo Sieverdingbeck</a>
*/

#include "ExtendedBallPercept.h"
#include "Tools/Debugging/DebugDrawings.h"

void ExtendedBallPercept::serialize(In* in, Out* out)
{
  STREAM_REGISTER_BEGIN();
    STREAM(positionInImage);
    STREAM(radiusInImage);
    STREAM(relativePositionOnField);
    STREAM(projectedBallRadius);
    STREAM(levenbergMarquardtResult);
    STREAM(couldBeBallPart);
    STREAM(couldBeRobotPart);
    STREAM(isValidBallPercept);
    STREAM(centerTopLeft);
    STREAM(centerDownRight);
    STREAM(middleTopLeft);
    STREAM(middleDownRight);
    STREAM(outerTopLeft);
    STREAM(outerDownRight);
    STREAM_ARRAY(centerArea);
    STREAM_ARRAY(middleArea);
    STREAM_ARRAY(outerArea);
    STREAM(centerResult);
    STREAM(middleResult);
    STREAM(outerResult);
    STREAM(outerResult);
    STREAM(distanceCheckResult);
    STREAM(validity);
    STREAM(deviationOfBallPoints);
    STREAM(durabilityOfBallPoints);
    STREAM(eccentricity);
  STREAM_REGISTER_FINISH();
}

void ExtendedBallPercept::draw() const
{
  DECLARE_DEBUG_DRAWING("representation:ExtendedBallPercepts:Image", "drawingOnImage"); // drawing of representation BallPercept
  DECLARE_DEBUG_DRAWING("representation:ExtendedballPercepts:Field", "drawingOnField"); // drawing of representation BallPercept
  if(isValidBallPercept)
  {
    CIRCLE("representation:ExtendedBallPercepts:Image", 
      positionInImage.x, 
      positionInImage.y, 
      radiusInImage, 
      2, // pen width 
      Drawings::ps_solid, 
      ColorClasses::black,
      Drawings::bs_solid, 
      ColorRGBA(255,128,64,100));
    CIRCLE("representation:ExtendedBallPercepts:Field", 
      relativePositionOnField.x, 
      relativePositionOnField.y, 
      45, 
      1, // pen width 
      Drawings::ps_solid, 
      ColorClasses::orange,
      Drawings::bs_null, 
      ColorClasses::orange);
  }
}
