/**
 * @file RobotPose.h
 *
 * The file contains the definition of the class RobotPose.
 *
 * @author <A href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</A>
 */

#ifndef __RobotPose_h_
#define __RobotPose_h_

#include "Tools/Math/Pose2D.h"
#include "Tools/Debugging/DebugDrawings.h"

/**
* @class RobotPose
* A Pose2D with validity.
*/
class RobotPose : public Pose2D
{
protected:
  virtual void serialize(In* in, Out* out)
  {
    STREAM_REGISTER_BEGIN();
    STREAM_BASE(Pose2D);
    STREAM(validity);
    STREAM_REGISTER_FINISH();
  }

public:
  double validity;                   /**< The validity of the robot pose. */
  ColorRGBA ownTeamColorForDrawing;  /**< As the name says... */
 
  /** Constructor */
  RobotPose():validity(0.0) {}

  /** Assignment operator
  * @param other Another RobotPose
  * @return A reference to the object after the assignment
  */
  const RobotPose& operator=(const RobotPose& other)
  {
    (Pose2D&) *this = (const Pose2D&) other;
    validity = other.validity;
    return *this;
  }

  /** Cast Contructor.
  * @param otherPose A Pose2D object
  */
  RobotPose (const Pose2D& otherPose) 
  {
    (Pose2D&) *this = otherPose;
    validity = 0;
  }

  /** Assignment operator for Pose2D objects
  * @param other A Pose2D object
  * @return A reference to the object after the assignment
  */
  const RobotPose& operator=(const Pose2D& other)
  {
    (Pose2D&) *this = other;
    //validity is not set
    return *this;
  }

  /** Draws the robot pose to the field view*/
  void draw();
};

class GroundTruthRobotPose : public RobotPose 
{
public:
  /** Draws the robot pose to the field view*/
  void draw();
};

#endif //__RobotPose_h_
