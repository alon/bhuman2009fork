/**
* @file PerceptValidityChecker.h
*
* Declares a class for checking perceptions before their use by the self locator.
* In fact, it is intended as a collection of different methods which are used 
* by different sensor models.
*
* @author <a href="mailto:Tim.Laue@dfki.de">Tim Laue</a>
*/

#ifndef _PerceptValidityChecker_h_
#define _PerceptValidityChecker_h_

#include "Tools/Math/Pose2D.h"


/**
* @class PerceptValidityChecker
*/
class PerceptValidityChecker
{
protected:
  /** Sub-class, storing information about where
  *   the robot can confuse the goal net with lines
  */
  class GoalNetTable
  {
  private:
    /** Constants defining the resolution of the table*/
    enum {
      GOAL_NET_TABLE_RES_X = 116,
      GOAL_NET_TABLE_RES_Y = 85,
      GOAL_NET_TABLE_CELL_SIZE = 64,
      GOAL_NET_TABLE_RES_ANGLE = 32
    };
    /** A table storing the maximum valid distances for not confusing a line and a goal */
    short maxValidDistances[GOAL_NET_TABLE_RES_X][GOAL_NET_TABLE_RES_Y][GOAL_NET_TABLE_RES_ANGLE];

    const int halfFieldLength;    /**< The length of a half field */
    const int halfFieldWidth;     /**< The width of a half field */
    const int goalWidth;          /**< The width of the goal (post center to post center) */
    const int goalDepth;          /**< The depth of the goal */
    const int goalX;              /**< The x coordinate of the goal post centers */
    bool saveTableAtDestruction;  /**< Flag, in case a new table has been generated automatically*/
    
    /** Loads table from disk */
    void load();

  public:  
    /** Saves the table to the disk */
    void save();

    /** (Re)computes the table */
    void compute();

    /** Draws the table to the field view */
    void draw();

    /** looks up distance for a pose and an angle to a point
    * @param robotPose The robot's position and translation (aka sample pose)
    * @param angle The angle to the perceived point
    * @return the maximum distance that is not a part of the net
    */
    short getMaxDistanceForPoseAndAngle(const Pose2D& robotPose, double angle) const;

    /** Constructor 
    * @param halfLength The length of a half field
    * @param halfWidth The width of a half field
    * @param goalWidth The width of the goal (post center to post center)
    * @param goalDepth The depth of the goal
    * @param goalX The x coordinate of the goal post centers
    */
    GoalNetTable(int halfLength, int halfWidth, int goalWidth, int goalDepth, int goalX);

    /** Destructor*/
    ~GoalNetTable();
  };

  /** An instance of the previously declared class */
  GoalNetTable goalNetTable;
 
public:
  /** Constructor */
  PerceptValidityChecker(const FieldDimensions& theFieldDimensions):
      goalNetTable(theFieldDimensions.xPosOpponentFieldBorder, 
        theFieldDimensions.yPosLeftFieldBorder, 2*theFieldDimensions.yPosLeftGoal,
        theFieldDimensions.xPosOpponentGoal - theFieldDimensions.xPosOpponentGoalpost,
        theFieldDimensions.xPosOpponentGoalpost) 
      {}

  /** Checks, if a perceived point is valid, i.e. not confused with the goal net
  * @param robotPose The robot's position and translation (aka sample pose)
  * @param distance The distance to the perceived point
  * @param angle The angle to the perceived point
  * @return true, if point might be a part of the goal net
  */
  bool pointIsProbablyInGoalNet(const Pose2D& robotPose, short distance, double angle) const
  {
    return distance > goalNetTable.getMaxDistanceForPoseAndAngle(robotPose, angle);
  }

  /** Wrapper for saving the GoalNetTable data */
  void saveGoalNetTable() {goalNetTable.save();};

  /** Wrapper for computing the GoalNetTable data */
  void computeGoalNetTable() {goalNetTable.compute();};

  /** Draws internal representations */
  void draw() {goalNetTable.draw();};
};


#endif
