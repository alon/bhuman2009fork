/**
* @file BallModel.h
*
* Declaration of class BallModel
*
* @author <A href="mailto:timlaue@informatik.uni-bremen.de">Tim Laue</A>
*/

#ifndef BallModel_H
#define BallModel_H

#include "Tools/Streams/Streamable.h"
#include "Tools/Math/Geometry.h"
#include "Tools/Debugging/DebugDrawings.h"
#include "Representations/Modeling/RobotPose.h"


/**
 * @class BallState
 *
 * Base class for ball position and velocity.
 */
class BallState : public Streamable
{
  /** Streaming (with specifications) */
  virtual void serialize(In *in, Out *out)
  {
    STREAM_REGISTER_BEGIN();
      STREAM(position);
      STREAM(velocity);
    STREAM_REGISTER_FINISH();
  }

public: 
  
  Vector2<double> position; /**< The position of the ball relative to the robot (in mm)*/  
  Vector2<double> velocity; /**< The velocity of the ball relative to the robot (in mm/s)*/

  inline double getAngle() const
  {
    return atan2(position.y, position.x);
  }

  inline double getDistance() const
  {
    return position.abs();
  }

  inline Vector2<double> getPositionInFieldCoordinates(const RobotPose& rp) const
  {
    return Geometry::relative2FieldCoord(rp, position.x, position.y);
  }

  Vector2<double> getVelocityInFieldCoordinates(const RobotPose& rp) const
  {
    double c(rp.getCos());
    double s(rp.getSin());
    return Vector2<double>(velocity.x*c - velocity.y*s, velocity.x*s + velocity.y*c);
  }

  void setPositionAndVelocityInFieldCoordinates(const Vector2<double>& positionOnField,
                                                const Vector2<double>& velocityOnField,
                                                const RobotPose& rp)
  {
    position = Geometry::fieldCoord2Relative(rp, positionOnField);
    double c(rp.getCos());
    double s(rp.getSin());
    velocity = Vector2<double>(velocityOnField.x*c + velocityOnField.y*s,
                              -velocityOnField.x*s + velocityOnField.y*c);
  }

};


/**
 * @class BallModel
 *
 * Contains all current knowledge about the ball.
 */
class BallModel : public Streamable
{
  /** Streaming (with specifications) */
  virtual void serialize(In *in, Out *out)
  {
    STREAM_REGISTER_BEGIN();
      STREAM(lastPerception);
      STREAM(estimate);
      STREAM(lastSeenEstimate);
      STREAM(timeWhenLastSeen);
    STREAM_REGISTER_FINISH();
  }

public:
  /** Constructor */
  BallModel() : timeWhenLastSeen(0) {}
  
  BallState lastPerception; /**< The last seen position of the ball */  
  BallState estimate; /**< The state of the ball estimated from own observations;
      it is propagated even if the ball is not seen */
  BallState lastSeenEstimate; /**< The last seen estimate */  
  unsigned timeWhenLastSeen; /**< Time stamp, indicating what its name says*/

  /** Draws something*/
  void draw() 
  {
    DECLARE_DEBUG_DRAWING("representation:BallModel", "drawingOnField"); // drawing of the ball model
    {
      Vector2<double>& position(estimate.position);
      Vector2<double>& velocity(estimate.velocity);
      CIRCLE("representation:BallModel", 
        position.x, 
        position.y, 
        45, 
        1, // pen width 
        Drawings::ps_solid, 
        ColorClasses::orange,
        Drawings::bs_solid, 
        ColorClasses::orange);
      ARROW("representation:BallModel", position.x, position.y, 
        position.x + velocity.x, position.y + velocity.y, 5, 1, ColorClasses::orange);
    }
    {
      Vector2<double>& position(lastSeenEstimate.position);
      Vector2<double>& velocity(lastSeenEstimate.velocity);
      CIRCLE("representation:BallModel", 
        position.x, 
        position.y, 
        45, 
        1, // pen width 
        Drawings::ps_solid, 
        ColorRGBA(0, 0, 0, 220),
        Drawings::bs_solid, 
        ColorRGBA(255, 128, 128, 220));
      ARROW("representation:BallModel", position.x, position.y, 
        position.x + velocity.x, position.y + velocity.y, 5, 1, ColorRGBA(255, 128, 0, 220));
    }
  }
};

class GroundTruthBallModel : public BallModel 
{
public:
  /** Draws something*/
  void draw() 
  {
    DECLARE_DEBUG_DRAWING("representation:GroundTruthBallModel", "drawingOnField"); // drawing of the ground truth ball model
    const Vector2<double>& position(lastPerception.position);
    const Vector2<double>& velocity(estimate.velocity);
    CIRCLE("representation:GroundTruthBallModel", 
      position.x, 
      position.y, 
      45, 
      1, // pen width 
      Drawings::ps_solid, 
      ColorRGBA(255, 128, 0, 192),
      Drawings::bs_solid, 
      ColorRGBA(255, 128, 0, 192));
    ARROW("representation:GroundTruthBallModel", position.x, position.y, 
      position.x + velocity.x, position.y + velocity.y, 5, 1, ColorRGBA(255, 128, 0, 192));
  }
};

#endif //BallModel_H
