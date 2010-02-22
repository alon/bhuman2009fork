/**
* @file RobotModel.cpp
* Implementation of class RobotModel.
* @author Alexander Härtl
*/

#include "RobotModel.h"
#include "Tools/Debugging/DebugDrawings.h"
#include "Tools/Debugging/DebugDrawings3D.h"

RobotModel::RobotModel(const JointData& joints, const RobotDimensions& robotDimensions, const MassCalibration& massCalibration)
{
  setJointData(joints, robotDimensions, massCalibration);
}

void RobotModel::setJointData(const JointData& joints, const RobotDimensions& robotDimensions, const MassCalibration& massCalibration)
{
  // compute direct kinematic chain for bilateral limbs 
  for(int side = 0; side < 2; side++)
  {
    bool left = side == 0;
    int sign = left ? -1 : 1;
    Limb upperLeg = left ? upperLegLeft : upperLegRight;
    JointData::Joint leg0 = left ? JointData::legLeft0 : JointData::legRight0;
    Limb upperArm = left ? upperArmLeft : upperArmRight;
    JointData::Joint arm0 = left ? JointData::armLeft0 : JointData::armRight0;

    limbs[upperLeg + 0] = Pose3D(0, robotDimensions.lengthBetweenLegs / 2.0 * -sign, 0)
                                             .rotateX(-pi_4 * sign)
                                             .rotateZ(joints.angles[leg0 + 0] * sign)
                                             .rotateX((joints.angles[leg0 + 1] + pi_4) * sign)
                                             .rotateY(joints.angles[leg0 + 2]);

    limbs[upperLeg + 1] = Pose3D(limbs[upperLeg + 0])
                                             .translate(0, 0, -robotDimensions.upperLegLength)
                                             .rotateY(joints.angles[leg0 + 3]);

    limbs[upperLeg + 2] = Pose3D(limbs[upperLeg + 1])
                                             .translate(0, 0, -robotDimensions.lowerLegLength)
                                             .rotateY(joints.angles[leg0 + 4])
                                             .rotateX(joints.angles[leg0 + 5] * sign);

    limbs[upperArm + 0] = Pose3D(robotDimensions.armOffset.x, robotDimensions.armOffset.y * -sign, robotDimensions.armOffset.z)
                                             .rotateY(-joints.angles[arm0 + 0])
                                             .rotateX(joints.angles[arm0 + 1] * -sign);

    limbs[upperArm + 1] = Pose3D(limbs[upperArm + 0])
                                             .translate(0, robotDimensions.upperArmLength * -sign, 0)
                                             .rotateY(joints.angles[arm0 + 2])
                                             .rotateX(joints.angles[arm0 + 3] * -sign);
  }

  // compute unilateral limbs (only head)
  limbs[head] = Pose3D(0, 0, robotDimensions.zLegJoint1ToHeadPan)
                                             .rotateZ(joints.angles[JointData::headPan])
                                             .rotateY(-joints.angles[JointData::headTilt]);

  // calculate center of mass
  const MassCalibration::MassInfo& torso(massCalibration.masses[MassCalibration::torso]);
  // initialize accumulators
  centerOfMass = torso.offset * torso.mass;
  totalMass = torso.mass;
  for(int i = 0; i < numOfLimbs; i++)
  {
    const MassCalibration::MassInfo& limb(massCalibration.masses[i]);
    totalMass += limb.mass;
    centerOfMass += (limbs[i] * limb.offset) * limb.mass;
  }
  centerOfMass /= totalMass;
}

void RobotModel::draw()
{
  DECLARE_DEBUG_DRAWING3D("representation:RobotModel", "origin");
  COMPLEX_DRAWING3D("representation:RobotModel",
    for(int i = 0; i < numOfLimbs; ++i)
    {
      const Pose3D& p = limbs[i];
      const Vector3<double>& v = p.translation;
      const Vector3<double> v1 = p * Vector3<double>(200, 0, 0);
      const Vector3<double> v2 = p * Vector3<double>(0, 200, 0);
      const Vector3<double> v3 = p * Vector3<double>(0, 0, 200);
      LINE3D("representation:RobotModel", v.x, v.y, v.z, v1.x, v1.y, v1.z, 1, ColorRGBA(255, 0, 0));
      LINE3D("representation:RobotModel", v.x, v.y, v.z, v2.x, v2.y, v2.z, 1, ColorRGBA(0, 255, 0));
      LINE3D("representation:RobotModel", v.x, v.y, v.z, v3.x, v3.y, v3.z, 1, ColorRGBA(0, 0, 255));
    }
  );
}
