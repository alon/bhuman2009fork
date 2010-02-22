/**
* @file GoalPercept.h
*
* Representation of a seen goal
*
* @author <a href="mailto:Tim.Laue@dfki.de">Tim Laue</a>
*/

#include "GoalPercept.h"
#include "Tools/Debugging/DebugDrawings.h"
#include "Tools/Debugging/DebugDrawings3D.h"


void GoalPost::serialize(In* in, Out* out)
{
  STREAM_REGISTER_BEGIN();
    STREAM(positionInImage);
    STREAM(positionOnField);
    STREAM(timeWhenLastSeen);
    STREAM_ENUMASINT(perceptionType);
    STREAM_ENUMASINT(distanceType);
  STREAM_REGISTER_FINISH();
}

void GoalPercept::serialize(In* in, Out* out)
{
  STREAM_REGISTER_BEGIN();
    STREAM_ARRAY(posts);
    STREAM_ARRAY(unknownPosts);
    STREAM_ENUMASINT(ownTeamColorForDrawing);
    STREAM(timeWhenOppGoalLastSeen);
  STREAM_REGISTER_FINISH();
}

void GoalPercept::draw()
{
  DECLARE_DEBUG_DRAWING("representation:GoalPercept:Image", "drawingOnImage");
  DECLARE_DEBUG_DRAWING("representation:GoalPercept:Field", "drawingOnField");
  DECLARE_DEBUG_DRAWING3D("representation:GoalPercept", "origin");
  TRANSLATE3D("representation:GoalPercept", 0, 0, -230);
  ColorRGBA ownColor = ColorRGBA(ownTeamColorForDrawing);
  ColorRGBA oppColor = ownTeamColorForDrawing == ColorClasses::yellow ? ColorRGBA(ColorClasses::blue) : ColorRGBA(ColorClasses::yellow);
  for(int i=0; i<NUMBER_OF_GOAL_POSTS; ++i)
  {
    const GoalPost& p = posts[i];
    ColorRGBA color = i > 1 ? ownColor : oppColor;
    if(p.perceptionType == GoalPost::SEEN_IN_IMAGE)
    { 
      CIRCLE("representation:GoalPercept:Field", p.positionOnField.x, p.positionOnField.y, 
        50, 1, Drawings::ps_solid, ColorClasses::white, Drawings::bs_solid, color);
      int barY = (i == GoalPercept::LEFT_OWN) || (i == GoalPercept::LEFT_OPPONENT) ? -700 : 700;
      LINE("representation:GoalPercept:Field", p.positionOnField.x, p.positionOnField.y,
        p.positionOnField.x, p.positionOnField.y + barY, 60, Drawings::ps_solid, color);
      MID_DOT("representation:GoalPercept:Image", p.positionInImage.x, p.positionInImage.y, 
        ColorClasses::white, color);
      LINE("representation:GoalPercept:Image", p.positionInImage.x, p.positionInImage.y, p.positionInImage.x, 0,
        5, Drawings::ps_solid, color);
      // Sorry, no access to field dimensions here, so the dimensions are hard coded
      CYLINDER3D("representation:GoalPercept", p.positionOnField.x, p.positionOnField.y, 400, 0, 0, 0, 50, 800, color);
    }
  }
  for(int i=0; i<NUMBER_OF_UNKNOWN_GOAL_POSTS; ++i)
  {
    const GoalPost& p = unknownPosts[i];
    ColorRGBA color = i > 0 ? ownColor : oppColor;
    if(p.perceptionType == GoalPost::SEEN_IN_IMAGE)
    { 
      CIRCLE("representation:GoalPercept:Field", p.positionOnField.x, p.positionOnField.y, 
        50, 1, Drawings::ps_solid, ColorRGBA(255,0,0), Drawings::bs_solid, color);
      MID_DOT("representation:GoalPercept:Image", p.positionInImage.x, p.positionInImage.y, 
        ColorRGBA(255,0,0), color);
      LINE("representation:GoalPercept:Image", p.positionInImage.x, p.positionInImage.y, p.positionInImage.x, 0,
        5, Drawings::ps_dot, color);
      // Sorry, no access to field dimensions here, so the dimensions are hard coded
      CYLINDER3D("representation:GoalPercept", p.positionOnField.x, p.positionOnField.y, 400, 0, 0, 0, 50, 800, color);
    }
  }
}
