/** 
* @file BH2009LocatorSymbols.h
*
* Declaration of class BH2009LocatorSymbols.
*
* @author Judith Müller
*/

#ifndef __BH2009LocatorSymbols_h_
#define __BH2009LocatorSymbols_h_

#include "../../Symbols.h"
#include "Representations/Infrastructure/TeamInfo.h"
#include "Representations/Modeling/BallModel.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Configuration/FieldDimensions.h"
#include "Representations/Perception/GroundContactState.h"

/**
* 
*
*
*/ 
class BH2009LocatorSymbols : public Symbols
{
public:
  /*
  * Constructor.
  * @param robotPose A reference to the RobotPose.
  * @param teamInfo A reference to the TeamInfo.
  * @param ballModel A reference to the ballModel.
  */
  BH2009LocatorSymbols(const RobotPose &robotPose, const OwnTeamInfo& ownTeamInfo, const FrameInfo& frameInfo, BallModel const& ballModel, FieldDimensions const& fieldDimensions, const GroundContactState& groundContactState) : 
    robotPose(robotPose),
    ownTeamInfo(ownTeamInfo),
    frameInfo(frameInfo),
    ballModel(ballModel),
    fieldDimensions(fieldDimensions),
    groundContactState(groundContactState),
    angleToOpponentGoal(0),
    angleWidthToOpponentGoal(0)
  {
    theInstance = this;
  }

  PROCESS_WIDE_STORAGE_STATIC BH2009LocatorSymbols* theInstance; /**< Points to the only instance of this class in this process or is 0 if there is none. */

  /** Updates the Locatorator symbols */
  void update();

  /** A reference to the RobotPose */
 const RobotPose &robotPose;
  
  /** A reference to the TeamInfo */
  OwnTeamInfo const& ownTeamInfo;

  /** A reference to the FrameInfo */
  const FrameInfo& frameInfo;

  /** A reference to the BallModel */
  BallModel const& ballModel;

  FieldDimensions const& fieldDimensions;

  const GroundContactState& groundContactState;

  /** registers the symbols at an engine */
  void registerSymbols(xabsl::Engine& engine);

  static double getPoseX();
  static double getPoseY();
  static double getPoseAngle();

  double angleToOpponentGoal;

  double angleWidthToOpponentGoal;  
  double angleToleranceToOpponentGoal;
 
  double angleToOwnGoal;
  double angleWidthToOwnGoal;

  /** calculates the decimal input function "distance_to" */
  static double distanceTo();
  
  /** calculates the decimal input function "angle_to" */
  static double angleTo();

  /** calculates the decimal input function "field_to_relative.x" */
  static double getFieldToRelativeX();

  /** calculates the decimal input function "field_to_relative.y" */
  static double getFieldToRelativeY();
  
  /** The parameter "x" of the functions "angle_to","distance_to","relative_to_field" */
  double x;
  
  /** The parameter "y" of the functions "angle_to","distance_to","relative_to_field" */
  double y;

};


#endif // __LocatorSymbols_h_

