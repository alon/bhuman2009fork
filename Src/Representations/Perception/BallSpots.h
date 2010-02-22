/**
* @file BallSpots.h
* Declaration of a class that represents a spot that might be an indication of a ball.
* @author <a href="mailto:jworch@informatik.uni-bremen.de">Jan-Hendrik Worch</a>
* @author <a href="mailto:ingsie@informatik.uni-bremen.de">Ingo Sieverdingbeck</a>
*
*/

#ifndef __BallSpots_h_
#define __BallSpots_h_

#include "Tools/Math/Vector2.h"
#include "BallSpot.h"

/**
* @class BallSpots
* A class that represents a spot that might be an indication of a ball.
*/
class BallSpots : public Streamable
{
private:
  void serialize(In* in, Out* out)
  {
    STREAM_REGISTER_BEGIN();
    STREAM_VECTOR(ballSpots);
    STREAM_REGISTER_FINISH();
  }
public:
  BallSpots() { ballSpots.reserve(50); }

  std::vector<BallSpot> ballSpots;

  void addBallSpot(int x, int y, double eccentricity)
  {
    BallSpot bs;
    bs.position.x = x;
    bs.position.y = y;
    bs.eccentricity = eccentricity;
    ballSpots.push_back(bs);
  }

  /**
  * The method draws all ball spots.
  */
  void draw() const
  {
    DECLARE_DEBUG_DRAWING("representation:BallSpots:Image", "drawingOnImage"); // Draws the ballspots to the image
    COMPLEX_DRAWING("representation:BallSpots:Image",
      for(std::vector<BallSpot>::const_iterator i = ballSpots.begin(); i != ballSpots.end(); ++i)
      {
        CROSS("representation:BallSpots:Image", i->position.x, i->position.y, 2, 3, Drawings::ps_solid, ColorClasses::orange);
        CROSS("representation:BallSpots:Image", i->position.x, i->position.y, 2, 1, Drawings::ps_solid, ColorClasses::black);
      }
    );
  }
};
#endif //__BallSpots_h_
