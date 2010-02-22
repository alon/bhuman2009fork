/**
* @file ObstacleModelProvider.h
*
* This file declares a module that provides information about occupied space in the robot's environment.
* It includes parts of the implementation of the PolygonLocalMapper module of the 
* autonomous wheelchair Rolland.
*
* @author <a href="mailto:Tim.Laue@dfki.de">Tim Laue</a>
* @author <a href="mailto:cman@tzi.de">Christian Mandel</a>
*/

#ifndef __ObstacleModelProvider_h_
#define __ObstacleModelProvider_h_

#include "Representations/Modeling/ObstacleModel.h"
#include "Representations/MotionControl/MotionRequest.h"
#include "Representations/MotionControl/OdometryData.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/RobotInfo.h"
#include "Representations/Infrastructure/SensorData.h"
#include "Tools/Module/Module.h"


MODULE(ObstacleModelProvider)
  REQUIRES(OdometryData)
  REQUIRES(FrameInfo)
  REQUIRES(FilteredSensorData)
  USES(MotionRequest)
  USES(RobotInfo)
  PROVIDES_WITH_MODIFY_AND_DRAW(ObstacleModel)
END_MODULE


/**
* @class ObstacleModelProvider
*
* A module for computing the occupied space in the robot's environment
*/
class ObstacleModelProvider: public ObstacleModelProviderBase
{
private:
  /**
  * @class Parameters
  * The parameters of the module
  */
  class Parameters: public Streamable
  {
  private:
    /**
    * The method makes the object streamable.
    * @param in The stream from which the object is read 
    * @param out The stream to which the object is written 
    */
    virtual void serialize(In* in, Out* out)
    {
      STREAM_REGISTER_BEGIN();
        STREAM(cellFreeInterval);
        STREAM(usOuterOpeningAngle);
        STREAM(usInnerOpeningAngle);
        STREAM(usCenterOpeningAngle);
        STREAM(maxValidUSDist);
        STREAM(minValidUSDist);
        STREAM(usRightPose);
        STREAM(usLeftPose);
        STREAM(usCenterPose);
      STREAM_REGISTER_FINISH();
    }

  public:
    int cellFreeInterval;                /**< Time (in ms) after which cell value becomes decreased */
    double usOuterOpeningAngle;          /**< Opening angle of US sensor towards side of robot */
    double usInnerOpeningAngle;          /**< Opening angle of US sensor towards center of robot */
    double usCenterOpeningAngle;         /**< Opening angle of Center US sensor to each side */
    int maxValidUSDist;                  /**< Maximum measured distance which is considered for the model */
    int minValidUSDist;                  /**< Minimum measured distance which is considered for the model */
    int cellOccupiedThreshold;           /**< If a cell state is higher than or equal to this value, it is considered as occupied */
    Pose2D usRightPose;                  /**< Position and orientation of right US sensor*/
    Pose2D usLeftPose;                   /**< Position and orientation of left US sensor*/
    Pose2D usCenterPose;                 /**< Position and orientation of (virtual) center US sensor */
    
    /** Constructor */
    Parameters():cellFreeInterval(3000), maxValidUSDist(999), minValidUSDist(200), 
      cellOccupiedThreshold(3)
    {
      usInnerOpeningAngle = fromDegrees(30);
      usOuterOpeningAngle = fromDegrees(70);
      usCenterOpeningAngle = fromDegrees(30);
      usRightPose.translation = Vector2<double>(53.7,-34.1);
      usLeftPose.translation  = Vector2<double>(53.7, 34.1);
      usCenterPose.translation = Vector2<double>(53.7, 0);
      usRightPose.rotation = atan2(-0.34, 0.93);
      usLeftPose.rotation  = atan2( 0.34, 0.93);
      usCenterPose.rotation = 0.0;
    }
  };

  /** 
  * @class Cell
  * A cell within the modeled occupancy grid
  */
  class Cell
  {
  public:
    int lastUpdate;                           /** The point of time of the last change of the state*/
    enum State {FREE = 0, MAX_VALUE = 3};     /** Minimum and maximum value of state */
    int state;                                /** The state of the cell, i.e. the number of positive measurements within the last few frames*/

    /** Constructor */
    Cell():lastUpdate(0),state(FREE)
    {}
  };

  /**
  * @class Point
  * The class represents a grid point together with clipping information.
  */
  class Point : public Vector2<int>
  {
  public:
    /**
    * The enumeration represents different edges of the map 
    * to which scan points can be clipped, and further options.
    */
    enum Flags
    {
      NONE, 
      NO_OBSTACLE = 16,
      PEAK = 32
    };
    int flags; /**< The set of edges to which the point was clipped, and whether it results from a castor wheel. */

    /**
    * Default constructor.
    */
    Point() : flags(NONE) {}

    /**
    * Constructor.
    * @param x The x coordinate of the point.
    * @param y The y coordinate of the point.
    * @param flags The set of edges to which the point was clipped, and whether 
    *              it results from a castor wheel.
    */
    Point(int x, int y, int flags = 0) : Vector2<int>(x, y), flags(flags) {}

    /**
    * Constructor.
    * @param v The x and y coordinates of the point.
    * @param flags The set of edges to which the point was clipped, and whether 
    *              it results from a castor wheel.
    */
    Point(const Vector2<int>& v, int flags = 0) : Vector2<int>(v), flags(flags) {}
  };

  /**
  * @class Line
  * This class represents a single line-segment out of a polyline.
  */
  class Line
  {
  public:
    /** 
    * Constructor.
    * @param ai The first point of the line-segment.
    * @param bi The last point of the line-segment.
    */
    Line(const Point& ai, const Point& bi)
    : peak(ai.flags & Point::PEAK || bi.flags & Point::PEAK)
    { 
      if(ai.y <= bi.y)
      {
        a = Vector2<float>( (float)ai.x, (float)ai.y ); 
        b = Vector2<float>( (float)bi.x, (float)bi.y );
      }
      else
      {
        b = Vector2<float>( (float)ai.x, (float)ai.y ); 
        a = Vector2<float>( (float)bi.x, (float)bi.y );
      }
      ils = b.y != a.y ? (b.x - a.x) / (b.y - a.y) : 0;
    }

    /**
    * Operator that compares a given line with this line. 
    * @param other This line is compared to other.
    * @return Is the y coordinate of this line smaller than the one of the other line?
    */
    bool operator<(const Line& other) const
    {
      return a.y < other.a.y;
    }

    Vector2<float> a; /**< First point of this line. */
    Vector2<float> b; /**< Last point of this line. */
    float ils; /**< Inverse slope of this line. */
    bool peak; /**< Is this line part of an upper peak? */
  };

public:
  /** Constructor */
  ObstacleModelProvider();

private:
  /** Dimensions of the grid */
  enum {CELL_SIZE = 60,
        CELLS_X = 40,
        CELLS_Y = 40};

  Pose2D accumulatedOdometry;                 /**< Storing odometry differences for grid update */
  Pose2D lastOdometry;                        /**< Odometry value at last execution */
  unsigned lastTimePenalized;                 /**< Last point of time the robot was penalized */
  Parameters parameters;                      /**< The parameters of this module */
  Cell cells[CELLS_Y*CELLS_X];                /**< The grid */
  std::vector<Point> polyPoints;              /**< Vector of obstacle points to become processed. Clipped and filtered. */
  std::vector<int> inter;                     /**< The intersections per line. */
  int lastLeftMeasurement, lastRightMeasurement; /**< Last Measurements for left and right Sensors */
  int lastLeftMeasurementTime, lastRightMeasurementTime; /**< Last time the last[Left|Right]Measurement values have been updated */
  Vector2<double> leftPoly[3];                /**< Polygon for left output value */
  Vector2<double> rightPoly[3];               /**< Polygon for right output value */
  Vector2<double> centerLeftPoly[4];          /**< Polygon for center/left output value */
  Vector2<double> centerRightPoly[4];         /**< Polygon for center/right output value */
  bool polygonsComputed;                      /**< Flag to assure that polygons are only computed once */

  /** Executes this module
  * @param obstacleModel The data structure that is filled by this module
  */
  void update(ObstacleModel& obstacleModel);

  /** Initialization after construction */
  void init();

  /** Enter obstacles to the grid */
  void occupyCells();

  /** Ages all cells, i.e. decreases the obstacle value in each cell */
  void ageCellState();

  /** Called by occupyCells, frees the space between the sensor and the obstacle */
  void fillScanBoundary();

  /** Clips point to the grid 
  * @param p1 The position from which the measurement is done
  * @param p2 The measured point
  */
  void clipPointP2(const Vector2<int>& p1, Point& p2) const;

  /** Converts coordinates in the local coordinate system to grid coordinates
  * @param p The position of a point
  * @return The grid coordinates
  */
  Vector2<int> worldToGrid(const Vector2<int>& p) const;

  /** Converts coordinates in grid coordinates to the local coordinate system
  * @param p The position within the grid
  * @return A position in local robot coordinates
  */
  Vector2<int> gridToWorld(const Vector2<int>& p) const;

  /** Enters an obstacle line to the grid
  * @param start The start of the line
  * @param end The end of the line
  */
  void line(Vector2<int>& start, Vector2<int>& end);

  /** Computes the model data from the grid
  * @param obstacleModel The model
  */
  void computeModel(ObstacleModel& obstacleModel);

  /** Moves the grid cells according to the odometry differences*/
  void moveGrid();

  /** Do debug drawings*/
  void draw() const;
};

#endif// __ObstacleModelProvider_h_
