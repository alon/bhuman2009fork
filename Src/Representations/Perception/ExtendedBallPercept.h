/**
* @file ExtendedBallPercept.h
*
* Very simple representation of a seen ball
*
* @author <a href="mailto:timlaue@informatik.uni-bremen.de">Tim Laue</a>
* @author <a href="mailto:ingsie@informatik.uni-bremen.de">Ingo Sieverdingbeck</a>
*/

#ifndef __ExtendedBallPercept_h
#define __ExtendedBallPercept_h_

#include "Tools/Streams/Streamable.h"
#include "Tools/Math/Vector2.h"

class ExtendedBallPercept : public Streamable
{
  /** Streaming function
  * @param in  streaming in ...
  * @param out ... streaming out.
  */
  void serialize(In* in, Out* out);

public:
  /** Constructor */
  ExtendedBallPercept() :
  positionInImage(0,0),
  radiusInImage(0),
  relativePositionOnField(0,0),
  projectedBallRadius(0),
  levenbergMarquardtResult(false),
  couldBeBallPart(false),
  couldBeRobotPart(false),
  isValidBallPercept(true),
  validity(-1)
  {}

  /** Draws the ball*/
  void draw() const;

  Vector2<double> positionInImage;         /**< The position of the ball in the current image */
  double radiusInImage;                    /**< The radius of the ball in the current image */
  
  Vector2<double> relativePositionOnField; /**< Ball position relative to the robot. */
  double projectedBallRadius;

  bool levenbergMarquardtResult;
  bool couldBeBallPart;
  bool couldBeRobotPart;
  bool isValidBallPercept;

  Vector2<int> centerTopLeft;
  Vector2<int> centerDownRight;
  Vector2<int> middleTopLeft;
  Vector2<int> middleDownRight;
  Vector2<int> outerTopLeft;
  Vector2<int> outerDownRight;
  // 0=COUNTER; 1=ORANGE; 2=GREEN; 3=WHITE; 4=RED; 5=YELLOW; 6=BLUE; 7=NONE; 8=OTHER
  int centerArea[9];
  int middleArea[9];
  int outerArea[9];
  double centerResult;
  double middleResult;
  double outerResult;
  double distanceCheckResult;
  double validity;

  double deviationOfBallPoints; // the average deviation of non-border ballpoints from the calculated circle
  double durabilityOfBallPoints; // 0..1; factor considering the relation of nearGreen points, nearYellow points and hardEdge points to the number of non-border points
  double eccentricity;
};

#endif// __ExtendedBallPercept_h_
