/**
* @file BallPercept.cpp
*
* Very simple representation of a seen ball
*
* @author <a href="mailto:timlaue@informatik.uni-bremen.de">Tim Laue</a>
*/

#include "BallPercept.h"
#include "Tools/Debugging/DebugDrawings.h"
#include "Tools/Debugging/DebugDrawings3D.h"

void BallPercept::serialize(In* in, Out* out)
{
  STREAM_REGISTER_BEGIN();
    STREAM(positionInImage);
    STREAM(radiusInImage);
    STREAM(ballWasSeen);
    STREAM(relativePositionOnField);
    //STREAM(validity);
  STREAM_REGISTER_FINISH();
}

void BallPercept::draw()
{
  DECLARE_DEBUG_DRAWING("representation:BallPercept:Image", "drawingOnImage");
  DECLARE_DEBUG_DRAWING("representation:BallPercept:Field", "drawingOnField");
  DECLARE_DEBUG_DRAWING3D("representation:BallPercept", "origin");
  TRANSLATE3D("representation:BallPercept", 0, 0, -230);
  if(ballWasSeen)
  {
    CIRCLE("representation:BallPercept:Image", 
      positionInImage.x, 
      positionInImage.y, 
      radiusInImage, 
      2, // pen width 
      Drawings::ps_solid, 
      ColorClasses::black,
      Drawings::bs_solid, 
      ColorRGBA(255,128,64,100));
    CIRCLE("representation:BallPercept:Field", 
      relativePositionOnField.x, 
      relativePositionOnField.y, 
      45, 
      1, // pen width 
      Drawings::ps_solid, 
      ColorClasses::orange,
      Drawings::bs_null, 
      ColorClasses::orange);
    // Sorry, no access to field dimensions here, so ball radius is hard coded
    SPHERE3D("representation:BallPercept", relativePositionOnField.x, relativePositionOnField.y, 43, 43, ColorClasses::orange);
  }
}
