/**
* @file FieldModel.h
* The file declares a class representing the tables required for mapping
* observations of a certain class to the closest positions in the field model.
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
*/

#ifndef _FieldModel_h_
#define _FieldModel_h_

#include "Representations/Configuration/FieldDimensions.h"
#include "Representations/Perception/LinePercept.h"

/**
* @class FieldModel
* A class representing the tables required for mapping observations of a 
* certain class to the closest positions in the field model.
*/
class FieldModel
{
private:
  /**
  * The class realizes a table of closest points on the field model.
  */
  template<int xSize, int ySize, int cellSize> class ClosestPointsTable
  {
  private:
    short points[ySize][xSize][2]; /**< The table. For each [y][x] position a x- ([0]) and a y-coordinate ([1]). */

  public:
    /**
    * The method creates the table.
    * @param table The table of lines the closest points will be searched in.
    * @param orientation The orientation of the lines that are considered.
    * @param numberOfOrientations How many different orientations will be distinguished between?
    * @param minLength The minimum length of lines to be considered.
    */
    void create(const FieldDimensions::LinesTable& table,
                double orientation,
                int numberOfOrientations,
                double minLength);

    /**
    * The method creates the table.
    * @param table The table of corners the closest points will be searched in.
    */
    void create(const FieldDimensions::CornersTable& table);

    /**
    * The method returns the closest line point to the given point.
    * @param point The given point.
    * @return The line point closest to the given point.
    */
    Vector2<int> getClosestPoint(const Vector2<int>& point) const
    {
      int x = int((point.x + (xSize / 2) * cellSize) / cellSize),
          y = int((point.y + (ySize / 2) * cellSize) / cellSize);
      if(x < 0)
        x = 0;
      else if(x >= xSize)
        x = xSize - 1;
      if(y < 0)
        y = 0;
      else if(y >= ySize)
        y = ySize - 1;
      return Vector2<int>(points[y][x][0], points[y][x][1]);
    }

    /**
    * The method draws the table (by subsampling).
    */
    void draw()
    {
      for(int y = 0; y < ySize; y += 5)
        for(int x = 0; x < xSize; x += 5)
        {
          int x1 = (x - xSize / 2) * cellSize,
              y1 = (y - ySize / 2) * cellSize;
          Vector2<int> closest = getClosestPoint(Vector2<int>(x1, y1));
          LINE("module:SelfLocator:fieldModel", x1, y1, closest.x, closest.y, 1, Drawings::ps_solid, ColorClasses::black);
        }
    }

    /**
    * The method writes the table to a stream.
    * @param stream The stream the table is written to.
    */
    void write(Out& stream) const {stream.write(points, sizeof(points));}

    /**
    * The method reads the table from a stream.
    * @param stream The stream the table is read from.
    */
    void read(In& stream) {stream.read(points, sizeof(points));}
  };

  ClosestPointsTable<310, 225, 24> linePointsTables[2][2], /**< The tables line points [all, long][along, across]. */
                                   tCornersTables[4], /**< The tables for all T-corners. */
                                   lCornersTables[4]; /**< The tables for all L-corners. */
  const FieldDimensions* fieldDimensions; /**< The field dimensions. They must be set using method init(). */
  const int* maxCrossingLength; /**< The maximum length of "short" lines. Must be set using method init(). */

public:
  /**
  * Constructor. Loads the field model tables from disk.
  */
  FieldModel();

  /**
  * The method must be called before other methods are used for the first time.
  * @param fieldDimensions The field dimensions (from the blackboard).
  * @param maxCrossingLength The selflocator parameter for the maximum length of "short" lines.
  */
  void init(const FieldDimensions& fieldDimensions, const int& maxCrossingLength);

  /**
  * The method creates the field model tables and writes them to disk.
  */
  void create();

  /**
  * The method determines the point in the field line model closest to a given point.
  * @param point The point the closest point in the field line model is searched for.
  * @param neighbor A neighbor of the point. It is used to determine the orientation 
  *                 of the line. Only lines in the field line model are considered
  *                 that have a similar orientation (along/across field).
  * @param length2 The squared length of the line the point belongs to. This is used
  *                to distinguish between short and long lines in the field line model.
  * @return The matching point in the field line model.
  */
  const Vector2<int> getClosestLinePoint(const Vector2<int>& point, const Vector2<int>& neighbor, int length2) const;

  /**
  * The method determines the corner in the corner model closest to a given point.
  * @param point The point the closest corner in the field line model is searched for.
  * @param type What kind of corner is searched for (X, T, L)?
  * @param dir The direction of the first axis of the corner (ignored for X-corners).
  * @return The matching corner in the corner model.
  */
  const Vector2<int> getClosestCorner(const Vector2<int>& point, LinePercept::Intersection::IntersectionType type, double dir) const;

  /**
  * The method draws one of the field model tables. 
  * The table can be selected using get/set.
  */
  void draw();
};

#endif // _FieldModel_h_
