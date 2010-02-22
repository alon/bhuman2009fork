/**
* @file ObstacleModel.h
*
* Declaration of class ObstacleModel
*
* @author <a href="mailto:Tim.Laue@dfki.de">Tim Laue</a>
*/

#ifndef __ObstacleModel_h_
#define __ObstacleModel_h_

#include "Tools/Streams/Streamable.h"
#include "Tools/Math/Common.h"
#include "Tools/Debugging/DebugDrawings.h"


/**
* @class ObstacleModel
*
* A class that represents the current state of the robot's environment
*/
class ObstacleModel : public Streamable
{
private:
  /** Streaming function 
  * @param in Object for streaming in the one direction
  * @param out Object for streaming in the other direction
  */
  void serialize(In* in, Out* out)
  {
    STREAM_REGISTER_BEGIN();
      STREAM(distanceToLeftObstacle);
      STREAM(distanceToCenterLeftObstacle);
      STREAM(distanceToCenterRightObstacle);
      STREAM(distanceToRightObstacle);
      STREAM_ENUMASINT(centerObstacleState);
    STREAM_REGISTER_FINISH();
  }

public:
  double distanceToLeftObstacle;
  double distanceToCenterLeftObstacle;
  double distanceToCenterRightObstacle;
  double distanceToRightObstacle;
  enum CenterObstacleState {LEFT_HALF=0, RIGHT_HALF, BOTH} centerObstacleState;

  double outerLeftAngle;
  double outerRightAngle;
  double centerLeftAngle;
  double centerRightAngle;
  double maxValidDist;

  /** Default constructor. */
  ObstacleModel():outerLeftAngle(fromDegrees(90)), outerRightAngle(fromDegrees(-90)), 
    centerLeftAngle(fromDegrees(30)), centerRightAngle(fromDegrees(-30)), maxValidDist(999)
  {
    distanceToLeftObstacle = 1000;
    distanceToRightObstacle = 1000;
    distanceToCenterLeftObstacle = 1000;
    distanceToCenterRightObstacle = 1000;
  }

  /** Function for drawing */
  void draw() 
  {
    DECLARE_DEBUG_DRAWING("representation:ObstacleModel", "drawingOnField"); 
    Vector2<double> left(distanceToLeftObstacle,0);
    Vector2<double> centerLeft(distanceToCenterLeftObstacle,0);
    Vector2<double> centerRight(distanceToCenterRightObstacle,0);
    Vector2<double> right(distanceToRightObstacle,0);
    Vector2<double> baseLeft(0,100);
    Vector2<double> baseRight(0,-100);
    // Left sector:
    Vector2<double> a(left);
    Vector2<double> b(left);
    a.rotate(outerLeftAngle);
    b.rotate(centerLeftAngle);
    a+=baseLeft;
    b+=baseLeft;
    LINE("representation:ObstacleModel", baseLeft.x, baseLeft.y, a.x, a.y, 
      30, Drawings::ps_solid, ColorClasses::robotBlue);
    LINE("representation:ObstacleModel", baseLeft.x, baseLeft.y, b.x, b.y, 
      30, Drawings::ps_solid, ColorClasses::robotBlue);
    LINE("representation:ObstacleModel", b.x, b.y, a.x, a.y, 
      30, Drawings::ps_solid, ColorClasses::robotBlue);
    // Center Left sector:
    a = centerLeft;
    b = centerLeft;
    b.rotate(centerLeftAngle);
    a+=baseLeft;
    b+=baseLeft;
    LINE("representation:ObstacleModel", baseLeft.x, baseLeft.y, a.x, a.y, 
      30, Drawings::ps_solid, ColorClasses::robotBlue);
    LINE("representation:ObstacleModel", baseLeft.x, baseLeft.y, b.x, b.y, 
      30, Drawings::ps_solid, ColorClasses::robotBlue);
    LINE("representation:ObstacleModel", b.x, b.y, a.x, a.y, 
      30, Drawings::ps_solid, ColorClasses::robotBlue);
    // Center sector:
    a = centerRight;
    b = centerRight;
    a.rotate(centerRightAngle);
    a+=baseRight;
    b+=baseRight;
    LINE("representation:ObstacleModel", baseRight.x, baseRight.y, a.x, a.y, 
      30, Drawings::ps_solid, ColorClasses::robotBlue);
    LINE("representation:ObstacleModel", baseRight.x, baseRight.y, b.x, b.y, 
      30, Drawings::ps_solid, ColorClasses::robotBlue);
    LINE("representation:ObstacleModel", b.x, b.y, a.x, a.y, 
      30, Drawings::ps_solid, ColorClasses::robotBlue);
    // Right sector:
    a = right;
    b = right;
    a.rotate(outerRightAngle);
    b.rotate(centerRightAngle);
    a+=baseRight;
    b+=baseRight;
    LINE("representation:ObstacleModel", baseRight.x, baseRight.y, a.x, a.y, 
      30, Drawings::ps_solid, ColorClasses::robotBlue);
    LINE("representation:ObstacleModel", baseRight.x, baseRight.y, b.x, b.y, 
      30, Drawings::ps_solid, ColorClasses::robotBlue);
    LINE("representation:ObstacleModel", b.x, b.y, a.x, a.y, 
      30, Drawings::ps_solid, ColorClasses::robotBlue);
  }
};

#endif //__ObstacleModel_h_
