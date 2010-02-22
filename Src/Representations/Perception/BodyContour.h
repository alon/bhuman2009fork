/**
* @file BodyContour.h
* The file declares a class that represents the contour of the robot's body in the image.
* The contour can be used to exclude the robot's body from image processing.
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
*/

#ifndef BodyContour_H
#define BodyContour_H

#include "Tools/Streams/Streamable.h"
#include "Tools/Math/Vector2.h"

/**
* @class BodyContour
* A class that represents the contour of the robot's body in the image.
* The contour can be used to exclude the robot's body from image processing.
*/
class BodyContour : public Streamable
{
private:
  /**
  * The method makes the object streamable.
  * @param in The stream from which the object is read (if in != 0).
  * @param out The stream to which the object is written (if out != 0).
  */
  virtual void serialize(In *in, Out *out)
  {
    STREAM_REGISTER_BEGIN();
    STREAM_VECTOR(lines);
    STREAM_REGISTER_FINISH();
  }

public:
  /** A class representing a line in 2-D space. */
  class Line : public Streamable
  {
  private:
    /**
    * The method makes the object streamable.
    * @param in The stream from which the object is read (if in != 0).
    * @param out The stream to which the object is written (if out != 0).
    */
    virtual void serialize(In *in, Out *out)
    {
      STREAM_REGISTER_BEGIN();
      STREAM(p1);
      STREAM(p2);
      STREAM_REGISTER_FINISH();
    }

  public:
    Vector2<int> p1, /**< The left point of the line. */
                 p2; /**< The right point of the line. */

    /** Default constructor. */
    Line() {}

    /**
    * Constructor.
    * @param p1 The first endpoint of the line.
    * @param p2 The second endpoint of the line.
    */
    Line(const Vector2<int> p1, const Vector2<int> p2) : p1(p1.x < p2.x ? p1 : p2), p2(p1.x < p2.x ? p2 : p1) {}

    /**
    * The method determines the x coordinate of the line for a certain y coordinate 
    * if the is any.
    * @param y The y coordinate.
    * @param x The x coordinate is returned here if it exists.
    * @return Does such an x coordinate exist?
    */
    bool xAt(int y, int& x) const
    {
      if((p1.y <= y && p2.y > y) || (p1.y > y && p2.y <= y))
      {
        x = p1.x + (p2.x - p1.x) * (y - p1.y) / (p2.y - p1.y);
        return true;
      }
      else
        return false;
    }

    /**
    * The method determines the y coordinate of the line for a certain x coordinate 
    * if the is any.
    * @param x The x coordinate.
    * @param y The y coordinate is returned here if it exists.
    * @return Does such a y coordinate exist?
    */
    bool yAt(int x, int& y) const
    {
      if(p1.x <= x && p2.x > x)
      {
        y = p1.y + (p2.y - p1.y) * (x - p1.x) / (p2.x - p1.x);
        return true;
      }
      else
        return false;
    }
  };

  std::vector<Line> lines; /**< The clipping lines. */

  /** Default constructor. */
  BodyContour() {lines.reserve(50);}

  /**
  * The method clips the bottom y coordinate of a vertical line.
  * @param x The x coordinate of the vertical line.
  * @param y The original y coordinate of the bottom of the vertical line.
  *          It will be replaced if necessary. Note that the resulting point
  *          can be outside the image!
  */
  void clipBottom(int x, int& y) const;

  /**
  * The method clips the left x coordinate of a horizonal line.
  * It only consides descending clipping lines.
  * @param x The original x coordinate of the left end of the horizontal line.
  *          It will be replaced if necessary. Note that the resulting point
  *          can be outside the image!
  * @param y The y coordinate of the horizontal line.
  */
  void clipLeft(int& x, int y) const;

  /**
  * The method clips the right x coordinate of a horizonal line.
  * It only consides ascending clipping lines.
  * @param x The original x coordinate of the right end of the horizontal line.
  *          It will be replaced if necessary. Note that the resulting point
  *          can be outside the image!
  * @param y The y coordinate of the horizontal line.
  */
  void clipRight(int& x, int y) const;

  /** Creates drawings of the contour. */
  void draw();
};

#endif //BodyContour_H
