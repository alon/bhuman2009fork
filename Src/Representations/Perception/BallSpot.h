/**
* @file BallSpot.h
* Declaration of a class that represents a spot that might be an indication of a ball.
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
*/

#ifndef __BallSpot_h_
#define __BallSpot_h_

#include "Tools/Math/Vector2.h"

/**
* @class BallSpot
* A class that represents a spot that might be an indication of a ball.
*/
class BallSpot : public Streamable
{
private:
  void serialize(In* in, Out* out)
  {
    STREAM_REGISTER_BEGIN();
    STREAM(position);
    STREAM(found);
    STREAM(eccentricity);
    STREAM_REGISTER_FINISH();
  }

public:
  Vector2<int> position;
  bool found;
  double eccentricity;

  BallSpot() : found(false) {}
};

#endif //__BallSpot_h_
