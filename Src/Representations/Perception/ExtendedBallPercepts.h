/**
* @file ExtendedBallPercepts.h
* @author <a href="mailto:ingsie@informatik.uni-bremen.de">Ingo Sieverdingbeck</a>
*/

#ifndef __ExtendedBallPercepts_h_
#define __ExtendedBallPercepts_h_

#include "Tools/Math/Vector2.h"
#include "ExtendedBallPercept.h"

class ExtendedBallPercepts : public Streamable
{
private:
  void serialize(In* in, Out* out)
  {
    STREAM_REGISTER_BEGIN();
    STREAM_VECTOR(extendedBallPercepts);
    STREAM_REGISTER_FINISH();
  }
public:
  ExtendedBallPercepts() { extendedBallPercepts.reserve(10); }


  std::vector<ExtendedBallPercept> extendedBallPercepts;

  /**
  * The method draws all ball percepts.
  */
  void draw() const
  {
    DECLARE_DEBUG_DRAWING("representation:ExtendedBallPercepts:Image", "drawingOnImage"); // Draws the ballpercepts to the image
    COMPLEX_DRAWING("representation:ExtendedBallPercepts:Image",
      for(std::vector<ExtendedBallPercept>::const_iterator i = extendedBallPercepts.begin(); i != extendedBallPercepts.end(); ++i)
      {
        i->draw();
      }
    );
  }
};
#endif //__ExtendedBallPercepts_h_
