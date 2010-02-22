/**
* @file InertiaMatrixProvider.cpp
* Implementation of module InertiaMatrixProvider.
* @author Colin Graf
*/

#include "InertiaMatrixProvider.h"
#include "Tools/Debugging/DebugDrawings.h"

MAKE_MODULE(InertiaMatrixProvider, Sensing)

InertiaMatrixProvider::InertiaMatrixProvider() {}

void InertiaMatrixProvider::update(InertiaMatrix& inertiaMatrix)
{
  // generate rotation matrix from measured angleX and angleY
  const Vector3<double> axis(theFilteredSensorData.data[SensorData::angleX], theFilteredSensorData.data[SensorData::angleY], 0.);
  RotationMatrix inertiaRotation(axis, axis.abs());

  // calculate "center of hip" position from left foot
  Pose3D fromLeftFoot(inertiaRotation);
  fromLeftFoot.conc(theRobotModel.limbs[RobotModel::footLeft]);
  fromLeftFoot.translate(0, 0, -theRobotDimensions.heightLeg5Joint);
  fromLeftFoot.translation *= -1.;
  fromLeftFoot.rotation = inertiaRotation;

  // calculate "center of hip" position from right foot
  Pose3D fromRightFoot(inertiaRotation);
  fromRightFoot.conc(theRobotModel.limbs[RobotModel::footRight]);
  fromRightFoot.translate(0, 0, -theRobotDimensions.heightLeg5Joint);
  fromRightFoot.translation *= -1.;
  fromRightFoot.rotation = inertiaRotation;

  // get foot z-rotations
  const Pose3D& leftFootInverse(theRobotModel.limbs[RobotModel::footLeft].invert());
  const Pose3D& rightFootInverse(theRobotModel.limbs[RobotModel::footRight].invert());
  const double leftFootZRotation = leftFootInverse.rotation.getZAngle();
  const double rightFootZRotation = rightFootInverse.rotation.getZAngle();

  // determine used foot
  const bool useLeft = fromLeftFoot.translation.z > fromRightFoot.translation.z;
  
  // calculate foot span
  const Vector3<double>& newFootSpan(fromRightFoot.translation - fromLeftFoot.translation);

  // and construct the matrix
  Pose3D newInertiaMatrix;
  newInertiaMatrix.translate(newFootSpan.x / (useLeft ? 2. : -2.), newFootSpan.y / (useLeft ? 2. : -2.), 0.);
  newInertiaMatrix.conc(useLeft ? fromLeftFoot : fromRightFoot);
  newInertiaMatrix.translate(8., 6., 85. + 27.); // inertia sensor position in thorax // TODO: move these values into robotDimensions

  // calculate inertia offset
  if(inertiaMatrix.translation.z != 0.) // the last inertia matrix should be valid
  {    
    Pose3D& inertiaOffset(inertiaMatrix.inertiaOffset);
    inertiaOffset = inertiaMatrix.invert();
    inertiaOffset.translate(lastFootSpan.x / (useLeft ? 2. : -2.), lastFootSpan.y / (useLeft ? 2. : -2.), 0.);
    inertiaOffset.rotateZ(useLeft ? (leftFootZRotation - lastLeftFootZRotation) : (rightFootZRotation - lastRightFootZRotation));
    inertiaOffset.translate(newFootSpan.x / (useLeft ? -2. : 2.), newFootSpan.y / (useLeft ? -2. : 2.), 0.);
    inertiaOffset.conc(newInertiaMatrix);
  }

  // adopt new matrix and footSpan
  (Pose3D&)inertiaMatrix = newInertiaMatrix;
  lastLeftFootZRotation = leftFootZRotation;
  lastRightFootZRotation = rightFootZRotation;
  lastFootSpan = newFootSpan;

  // valid?
  inertiaMatrix.isValid = theGroundContactState.contact;

  //
  PLOT("module:InertiaMatrixProvider:footSpanX", newFootSpan.x);
  PLOT("module:InertiaMatrixProvider:footSpanY", newFootSpan.y);
  PLOT("module:InertiaMatrixProvider:footSpanZ", newFootSpan.z);
  
  PLOT("module:InertiaMatrixProvider:inertiaMatrixX", inertiaMatrix.translation.x);
  PLOT("module:InertiaMatrixProvider:inertiaMatrixY", inertiaMatrix.translation.y);
  PLOT("module:InertiaMatrixProvider:inertiaMatrixZ", inertiaMatrix.translation.z);
}

/*
void InertiaMatrixProvider::update(FilteredOdometryOffset& odometryOffset)
{
  Pose2D odometryOffset;

  if(lastInertiaMatrix.translation.z != 0.)
  {
    Pose3D odometryOffset3D(lastInertiaMatrix);
    odometryOffset3D.conc(theInertiaMatrix.inertiaOffset);
    odometryOffset3D.conc(theInertiaMatrix.invert());  
    
    odometryOffset.translation.x = odometryOffset3D.translation.x;
    odometryOffset.translation.y = odometryOffset3D.translation.y;
    odometryOffset.rotation = odometryOffset3D.rotation.getZAngle();
  }
  
  PLOT("module:InertiaMatrixProvider:odometryOffsetX", odometryOffset.translation.x);
  PLOT("module:InertiaMatrixProvider:odometryOffsetY", odometryOffset.translation.y);
  PLOT("module:InertiaMatrixProvider:odometryOffsetRotation", toDegrees(odometryOffset.rotation));

  (Pose3D&)lastInertiaMatrix = theInertiaMatrix;
}
*/
void InertiaMatrixProvider::update(OdometryData& odometryData)
{
  Pose2D odometryOffset;

  if(lastInertiaMatrix.translation.z != 0.)
  {
    Pose3D odometryOffset3D(lastInertiaMatrix);
    odometryOffset3D.conc(theInertiaMatrix.inertiaOffset);
    odometryOffset3D.conc(theInertiaMatrix.invert());  
    
    odometryOffset.translation.x = odometryOffset3D.translation.x;
    odometryOffset.translation.y = odometryOffset3D.translation.y;
    odometryOffset.rotation = odometryOffset3D.rotation.getZAngle();
  }

  PLOT("module:InertiaMatrixProvider:odometryOffsetX", odometryOffset.translation.x);
  PLOT("module:InertiaMatrixProvider:odometryOffsetY", odometryOffset.translation.y);
  PLOT("module:InertiaMatrixProvider:odometryOffsetRotation", toDegrees(odometryOffset.rotation));

  odometryData += odometryOffset;

  (Pose3D&)lastInertiaMatrix = theInertiaMatrix;
}

