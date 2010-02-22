/**
 * @file InverseKinematic.h
 * @author Alexander Härtl
 */

#ifndef __InverseKinematic_h_
#define __InverseKinematic_h_

#include "Tools/Math/Vector2.h"
#include "Tools/Math/Pose3D.h"
#include "Representations/Configuration/RobotDimensions.h"
#include "Tools/Range.h"


class InverseKinematic
{
public:
  /**
  * The method calculates the joint angles for the legs of the robot from a Pose3D for each leg
  * @param positionLeft The desired position (translation + rotation) of the left foots ankle point
  * @param positionRight The desired position (translation + rotation) of the right foots ankle point
  * @param jointData The instance of JointData where the resulting joints are written into  
  * @param robotDimensions The Robot Dimensions needed for calculation
  * @param ratio The ratio of 
  */
  static void calcLegJoints(const Pose3D& positionLeft, const Pose3D& positionRight, JointData& jointData, const RobotDimensions& robotDimensions, double ratio = 0.5)
  {
    calcLegJoints(positionLeft, jointData, true, robotDimensions);
    calcLegJoints(positionRight, jointData, false, robotDimensions);
    Range<double> clipping(0.0, 1.0);
    ratio = clipping.limit(ratio);
    // the hip joints of both legs must be equal, so it is computed as weighted mean and the foot positions are
    // recomputed with fixed joint0 and left open foot rotation (as possible failure)
    double joint0 = jointData.angles[JointData::legLeft0] * ratio + jointData.angles[JointData::legRight0] * (1 - ratio);
    calcLegJoints(positionLeft, jointData, joint0, true, robotDimensions);
    calcLegJoints(positionRight, jointData, joint0, false, robotDimensions);
  }

  /**
  * The method calculates the joint angles of one leg of the robot from a Pose3D
  * @param position The desired position (translation + rotation) of the foots ankle point
  * @param jointData The instance of JointData where the resulting joints are written into
  * @param left Determines if the left or right leg is calculated
  * @param robotDimensions The Robot Dimensions needed for calculation
  */
  static void calcLegJoints(const Pose3D& position, JointData& jointData, bool left, const RobotDimensions& robotDimensions)
  {
    Pose3D target(position);
    JointData::Joint firstJoint(left ? JointData::legLeft0 : JointData::legRight0);
    int sign(left ? -1 : 1);
    target.translation.y += robotDimensions.lengthBetweenLegs / 2 * sign; // translate to origin of leg
    // rotate by 45° around origin for Nao
    // calculating sqrt(2) is faster than calculating the resp. rotation matrix with getRotationX()
    static const double sqrt2_2 = sqrt(2.0) * 0.5;
    RotationMatrix rotationX_pi_4 = RotationMatrix(Vector3<double>(1, 0, 0), Vector3<double>(0, sqrt2_2, sqrt2_2 * sign), Vector3<double>(0, sqrt2_2 * -sign, sqrt2_2));
    target.translation = rotationX_pi_4 * target.translation;
    target.rotation = rotationX_pi_4 * target.rotation;

    target = target.invert(); // invert pose to get position of body relative to foot

    // use geometrical solution to compute last three joints
    double length = target.translation.abs();
    double sqrLength = length * length;
    double upperLeg = robotDimensions.upperLegLength;
    double sqrUpperLeg = upperLeg * upperLeg;
    double lowerLeg = robotDimensions.lowerLegLength;
    double sqrLowerLeg = lowerLeg * lowerLeg;
    double cosLowerLeg = (sqrLowerLeg + sqrLength - sqrUpperLeg) / (2 * lowerLeg * length);
    double cosKnee = (sqrUpperLeg + sqrLowerLeg - sqrLength) / (2 * upperLeg * lowerLeg);

    // clip for the case of unreachable position
    const Range<double> clipping(-1.0, 1.0);
    if(!clipping.isInside(cosKnee))
    {
      OUTPUT(idText, text, (left ? "Left" : "Right") << " leg too short!"); // reachability is checked and printed
      cosKnee = clipping.limit(cosKnee);
      cosLowerLeg = clipping.limit(cosLowerLeg);
    }
    double joint3 = pi - acos(cosKnee); // implicitly solve discrete ambiguousness (knee always moves forward)
    double joint4 = -acos(cosLowerLeg); 
    joint4 -= atan2(target.translation.x, Vector2<double>(target.translation.y, target.translation.z).abs());
    double joint5 = atan2(target.translation.y, target.translation.z) * sign;

    // calulate rotation matrix before hip joints
    RotationMatrix hipFromFoot;
    hipFromFoot.rotateX(joint5 * -sign);
    hipFromFoot.rotateY(-joint4 - joint3);

    // compute rotation matrix for hip from rotation before hip and desired rotation
    RotationMatrix hip = hipFromFoot.invert() * target.rotation;

    // compute joints from rotation matrix using theorem of euler angles
    // see http://www.geometrictools.com/Documentation/EulerAngles.pdf
    // this is possible because of the known order of joints (z, x, y seen from body resp. y, x, z seen from foot)
    double joint1 = asin(-hip[2].y) * -sign;
    joint1 -= pi_4; // because of the 45°-rotational construction of the Nao legs
    double joint2 = -atan2(hip[2].x, hip[2].z);
    double joint0 = atan2(hip[0].y, hip[1].y) * -sign;

    // set computed joints in jointData
    jointData.angles[firstJoint + 0] = joint0;
    jointData.angles[firstJoint + 1] = joint1;
    jointData.angles[firstJoint + 2] = joint2;
    jointData.angles[firstJoint + 3] = joint3;
    jointData.angles[firstJoint + 4] = joint4;
    jointData.angles[firstJoint + 5] = joint5;
  }

  /**
  * The method calculates the joint angles of one leg of the Nao from a Pose3D with a fixed first joint
  * This is necessary because the Nao has mechanically connected hip joints, hence not every
  * combination of foot positions can be reached and has to be recalculated with equal joint0 for both legs
  * the rotation of the foot around the z-axis through the ankle-point is left open as "failure"
  * @param position The desired position (translation + rotation) of the foots ankle point
  * @param jointData The instance of JointData where the resulting joints are written into
  * @param joint0 Fixed value for joint0 of the respective leg
  * @param left Determines if the left or right leg is calculated
  * @param robotDimensions The Robot Dimensions needed for calculation
  */
  static void calcLegJoints(const Pose3D& position, JointData& jointData, double joint0, bool left, const RobotDimensions& robotDimensions)
  {
    Pose3D target(position);
    JointData::Joint firstJoint(left ? JointData::legLeft0 : JointData::legRight0);
    const int sign(left ? -1 : 1);
    target.translation.y += robotDimensions.lengthBetweenLegs / 2 * sign; // translate to origin of leg
    target = Pose3D().rotateZ(joint0 * -sign).rotateX(pi_4 * sign).conc(target); // compute residual transformation with fixed joint0

    // use cosine theorem and arctan to compute first three joints
    double length = target.translation.abs();
    double sqrLength = length * length;
    double upperLeg = robotDimensions.upperLegLength;
    double sqrUpperLeg = upperLeg * upperLeg;
    double lowerLeg = robotDimensions.lowerLegLength;
    double sqrLowerLeg = lowerLeg * lowerLeg;
    double cosUpperLeg = (sqrUpperLeg + sqrLength - sqrLowerLeg) / (2 * upperLeg * length);
    double cosKnee = (sqrUpperLeg + sqrLowerLeg - sqrLength) / (2 * upperLeg * lowerLeg);
    // clip for the case that target position is not reachable
    const Range<double> clipping(-1.0, 1.0);
    if(!clipping.isInside(cosKnee))
    {
      OUTPUT(idText, text, (left ? "Left" : "Right") << " leg too short!");
      cosKnee = clipping.limit(cosKnee);
      cosUpperLeg = clipping.limit(cosUpperLeg);
    }
    double joint1 = atan2(target.translation.y, -target.translation.z) * sign;
    double joint2 = -acos(cosUpperLeg);
    joint2 -= atan2(target.translation.x, Vector2<double>(target.translation.y, target.translation.z).abs());
    double joint3 = pi - acos(cosKnee);
    RotationMatrix beforeFoot = RotationMatrix().rotateX(joint1 * sign).rotateY(joint2 + joint3);
    joint1 -= pi_4; // because of the strange hip of Nao

    // compute joints from rotation matrix using theorem of euler angles
    // see http://www.geometrictools.com/Documentation/EulerAngles.pdf
    // this is possible because of the known order of joints (y, x, z) where z is left open and is seen as failure
    RotationMatrix foot = beforeFoot.invert() * target.rotation;
    double joint5 = asin(-foot[2].y) * -sign * -1;
    double joint4 = -atan2(foot[2].x, foot[2].z) * -1;
    //double failure = atan2(foot[0].y, foot[1].y) * sign;

    // set computed joints in jointData
    jointData.angles[firstJoint + 0] = joint0;
    jointData.angles[firstJoint + 1] = joint1;
    jointData.angles[firstJoint + 2] = joint2;
    jointData.angles[firstJoint + 3] = joint3;
    jointData.angles[firstJoint + 4] = joint4;
    jointData.angles[firstJoint + 5] = joint5;
  }
};

#endif // __RingBuffer_h_
