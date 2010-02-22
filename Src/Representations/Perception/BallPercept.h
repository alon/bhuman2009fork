/**
* @file BallPercept.h
*
* Very simple representation of a seen ball
*
* @author <a href="mailto:timlaue@informatik.uni-bremen.de">Tim Laue</a>
*/

#ifndef __BallPercept_h_
#define __BallPercept_h_

#include "Tools/Streams/Streamable.h"
#include "Tools/Math/Vector2.h"

class BallPercept : public Streamable
{
  /** Streaming function
  * @param in  streaming in ...
  * @param out ... streaming out.
  */
  void serialize(In* in, Out* out);

public:
  /** Constructor */
  BallPercept() : ballWasSeen(false) {}

  /** Draws the ball*/
  void draw();

  Vector2<double> positionInImage;         /**< The position of the ball in the current image */
  double radiusInImage;                    /**< The radius of the ball in the current image */
  bool ballWasSeen;                        /**< Indicates, if the ball was seen in the current image. */
  Vector2<double> relativePositionOnField; /**< Ball position relative to the robot. */
  double validity;
};

#endif// __BallPercept_h_
