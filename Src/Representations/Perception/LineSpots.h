/**
* @file LineSpots.h
* Declaration of a class that represents a spot that  indicates a line.
* @author jeff
*
*/

#ifndef __LineSpots_h_
#define __LineSpots_h_

#include "Tools/Math/Vector2.h"
#include "Tools/Debugging/DebugDrawings.h"


/**
* @class LineSpots
* This class contains all the linespots and nonlinesposts (=white regions which are no lines)
*/
class LineSpots : public Streamable
{
private:
  void serialize(In* in, Out* out)
  {
    STREAM_REGISTER_BEGIN();
    STREAM_VECTOR(spots);
    STREAM_VECTOR(nonLineSpots);
    STREAM_REGISTER_FINISH();
  }
public:

  /**
   * @class LineSpot
   * A class that represents a spot that's an indication of a line.
   */
  class LineSpot : public Streamable
  {
    void serialize(In* in, Out* out)
    {
      STREAM_REGISTER_BEGIN();
      STREAM(alpha);
      STREAM(alpha_len);
      STREAM(alpha_len2);
      STREAM(x_s);
      STREAM(y_s);
      STREAM(p1);
      STREAM(p2);
      STREAM_REGISTER_FINISH();
    }
  public:
    int x_s, /**< "schwerpunkt_x" */
        y_s; /**< "schwerpunkt_y" */
    double alpha, /**< the direction/rotation of the region    | */
           alpha_len, /**< "ausbreitung entlang alpha"         |-> Haupttraegheitsachsenbla */
           alpha_len2; /**< "ausbreitung orthogonal zu alpha"  | */
    Vector2<int> p1, p2; /**< The starting/end point of this linespot in image coordinates*/

  };

  /**
   * @class NonLineSpot
   * This class represents a white region which is no line
   */
  class NonLineSpot : public Streamable
  {
    private:
      void serialize(In* in, Out* out)
      {
        STREAM_REGISTER_BEGIN();
        STREAM(p1);
        STREAM(p2);
        STREAM(size);
        STREAM_REGISTER_FINISH();
      }
    public:
      Vector2<int> p1, p2; /**< start/end point of this spot in image coordinates */
      int size; /**< The size of the coresponding region in the image */
  };

  std::vector<LineSpot> spots; /**< All the line spots */
  std::vector<NonLineSpot> nonLineSpots; /**< All the non line spots (= white regions which are no lines)*/

  /**
  * The method draws all line spots.
  */
  void draw() const
  {
    DECLARE_DEBUG_DRAWING("representation:LineSpots:nonLineSpots", "drawingOnImage");
    COMPLEX_DRAWING("representation:LineSpots:nonLineSpots",
      for(std::vector<LineSpots::NonLineSpot>::const_iterator i = nonLineSpots.begin(); i != nonLineSpots.end(); ++i)
      {
        ARROW("representation:LineSpots:nonLineSpots", i->p1.x, i->p1.y, i->p2.x, i->p2.y, 2, Drawings::ps_solid, ColorClasses::blue);
      }
    );
 
    DECLARE_DEBUG_DRAWING("representation:LineSpots:Image", "drawingOnImage"); // Draws the LineSpots to the image
    COMPLEX_DRAWING("representation:LineSpots:Image",
      for(std::vector<LineSpots::LineSpot>::const_iterator i = spots.begin(); i != spots.end(); ++i)
      {
        LINE("representation:LineSpots:Image", i->x_s, i->y_s, i->x_s + (int)(cos(i->alpha+pi_2) * i->alpha_len2), i->y_s + (int)(sin(i->alpha+pi_2)*i->alpha_len2), 1, Drawings::ps_solid, ColorClasses::black);
        ARROW("representation:LineSpots:Image", i->p1.x, i->p1.y, i->p2.x, i->p2.y, 1, Drawings::ps_solid, ColorClasses::black);
      }
   );
  }
};
#endif //__LineSpots_h_
