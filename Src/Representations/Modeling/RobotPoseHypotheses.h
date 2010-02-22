/**
 * @file RobotPoseHypotheses.h
 *
 * The file contains the definition of the class RobotPoseHypotheses.
 *
 * @author <A href="mailto:Tim.Laue@dfki.de">Tim Laue</A>
 */

#ifndef __RobotPoseHypotheses_h_
#define __RobotPoseHypotheses_h_

#include "Representations/Modeling/RobotPose.h"
#include "Tools/Math/Matrix2x2.h"


/**
* @class RobotPoseHypothesis
* A single hypothesis about a robot position
*/
class RobotPoseHypothesis: public RobotPose
{
private:
  virtual void serialize(In* in, Out* out)
  {
    STREAM_REGISTER_BEGIN();
      STREAM_BASE(RobotPose);
      STREAM(positionCovariance.c[0]);
      STREAM(positionCovariance.c[1]);
    STREAM_REGISTER_FINISH();
  }

public:
  /** Covariance matrix about the position uncertainty*/
  Matrix2x2<double> positionCovariance;
};


/**
* @class RobotPoseHypotheses
* A set of hypotheses
*/
class RobotPoseHypotheses: public Streamable
{
private:
  virtual void serialize(In* in, Out* out)
  {
    STREAM_REGISTER_BEGIN();
      STREAM_VECTOR(hypotheses);
    STREAM_REGISTER_FINISH();
  }

public:
  const int MAX_HYPOTHESES;                      /**< The maximum number of hypotheses */
  std::vector<RobotPoseHypothesis> hypotheses;   /**< The list of hypotheses */
 
  /** Constructor*/
  RobotPoseHypotheses():MAX_HYPOTHESES(5) {}

  /** Assignment operator
  * @param other Another RobotPoseHypotheses
  * @return A reference to the object after the assignment
  */
  const RobotPoseHypotheses& operator=(const RobotPoseHypotheses& other)
  {
    hypotheses = other.hypotheses;
    return *this;
  }

  /** Draws the hypotheses to the field view*/
  void draw();
};

#endif //__RobotPoseHypotheses_h_
