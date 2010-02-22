/** 
* @file CameraMatrixProvider.cpp
* This file implements a class to calculate the position of the camera for the Nao.
* @author <a href="mailto:allli@informatik.uni-bremen.de">Alexander Härtl</a>
* @author Colin Graf
*/

#include "CameraMatrixProvider.h"

MAKE_MODULE(CameraMatrixProvider, Perception);

void CameraMatrixProvider::update(RobotCameraMatrix& robotCameraMatrix)
{
  (Pose3D&)robotCameraMatrix = Pose3D();

  robotCameraMatrix.translate(0., 0., theRobotDimensions.zLegJoint1ToHeadPan);
  robotCameraMatrix.rotateZ(theFilteredJointData.angles[JointData::headPan]);
  robotCameraMatrix.rotateY(-theFilteredJointData.angles[JointData::headTilt]);
  robotCameraMatrix.translate(theRobotDimensions.xHeadTiltToCamera, 0., theRobotDimensions.zHeadTiltToCamera);
  robotCameraMatrix.rotateY(theRobotDimensions.headTiltToCameraTilt + theCameraCalibration.cameraTiltCorrection);
  robotCameraMatrix.rotateX(theCameraCalibration.cameraRollCorrection);
}

void CameraMatrixProvider::update(CameraMatrix& cameraMatrix)
{
  (Pose3D&)cameraMatrix = theInertiaMatrix;

  cameraMatrix.translate(-8., -6., -(85. + 27.));
  cameraMatrix.translate(theCameraCalibration.bodyTranslationCorrection);
  cameraMatrix.rotateY(theCameraCalibration.bodyTiltCorrection);
  cameraMatrix.rotateX(theCameraCalibration.bodyRollCorrection);
  cameraMatrix.conc(theRobotCameraMatrix);

  cameraMatrix.isValid = theInertiaMatrix.isValid && theMotionInfo.isMotionStable && 
                         theFallDownState.state == FallDownState::upright &&
                         theFrameInfo.getTimeSince(theFilteredJointData.timeStamp) < 500 &&
                         theRobotInfo.penalty == PENALTY_NONE;

  DECLARE_DEBUG_DRAWING("module:CameraMatrixProvider:calibrationHelper", "drawingOnImage");
  COMPLEX_DRAWING("module:CameraMatrixProvider:calibrationHelper", drawFieldLines(cameraMatrix); );
}

void CameraMatrixProvider::camera2image(const Vector3<>& camera, Vector2<float>& image) const
{
  const double& scale(theCameraInfo.focalLength / camera.x);
  image.x = float(theCameraInfo.opticalCenter.x - scale * camera.y);
  image.y = float(theCameraInfo.opticalCenter.y - scale * camera.z);
}

bool CameraMatrixProvider::intersectLineWithCullPlane(const Vector3<>& lineBase, const Vector3<>& lineDir, 
                                               Vector3<>& point) const
{
  if(lineDir.x == 0.)
    return false;
  point = lineBase + lineDir * ((lineBase.x - 200.) / -lineDir.x);
  return true;
}

void CameraMatrixProvider::drawFieldLines(const CameraMatrix& cameraMatrix) const
{
  Vector3<> start0C, start1C, end0C, end1C;
  Vector2<float> start0I, start1I, end0I, end1I;

  const Pose2D& robotPoseInv(theRobotPose.invert());
  const Pose3D& cameraMatrixInv(cameraMatrix.invert());
  int halfFieldLinesWidth = theFieldDimensions.fieldLinesWidth / 2;

  for(unsigned int i = 0; i < theFieldDimensions.fieldLines.lines.size(); ++i)
  {
    Pose2D relativeLine(robotPoseInv);
    relativeLine.conc(theFieldDimensions.fieldLines.lines[i].corner);
    const Vector2<>& start0(Pose2D(relativeLine).translate(0, -halfFieldLinesWidth).translation);
    const Vector2<>& end1(Pose2D(relativeLine).translate(theFieldDimensions.fieldLines.lines[i].length, halfFieldLinesWidth).translation);

    start0C = cameraMatrixInv * Vector3<>(start0.x, start0.y, 0.); // field2camera
    end1C = cameraMatrixInv * Vector3<>(end1.x, end1.y, 0.); // field2camera

    if(start0C.x <= 200 && end1C.x <= 200)
      continue;

    const Vector2<>& start1(Pose2D(relativeLine).translate(0, halfFieldLinesWidth).translation);
    const Vector2<>& end0(Pose2D(relativeLine).translate(theFieldDimensions.fieldLines.lines[i].length, -halfFieldLinesWidth).translation);

    start1C = cameraMatrixInv * Vector3<>(start1.x, start1.y, 0.); // field2camera
    end0C = cameraMatrixInv * Vector3<>(end0.x, end0.y, 0.); // field2camera

    if(start0C.x <= 200)
      intersectLineWithCullPlane(start0C, end0C - start0C, start0C);
    else if(end0C.x <= 200)
      intersectLineWithCullPlane(start0C, end0C - start0C, end0C);
    if(start1C.x <= 200)
      intersectLineWithCullPlane(start1C, end1C - start1C, start1C);
    else if(end1C.x <= 200)
      intersectLineWithCullPlane(start1C, end1C - start1C, end1C);

    camera2image(start0C, start0I);
    camera2image(end0C, end0I);
    camera2image(start1C, start1I);
    camera2image(end1C, end1I);

    LINE("module:CameraMatrixProvider:calibrationHelper", start0I.x, start0I.y, end0I.x, end0I.y, 1, Drawings::ps_solid, ColorRGBA(0,0,0));
    LINE("module:CameraMatrixProvider:calibrationHelper", start1I.x, start1I.y, end1I.x, end1I.y, 1, Drawings::ps_solid, ColorRGBA(0,0,0));
  }

  start0C = cameraMatrixInv * Vector3<>(1000, -25, 0.); // field2camera
  end0C = cameraMatrixInv * Vector3<>(0, -25, 0.); // field2camera
  camera2image(start0C, start0I);
  camera2image(end0C, end0I);

  LINE("module:CameraMatrixProvider:calibrationHelper", start0I.x, start0I.y, end0I.x, end0I.y, 1, Drawings::ps_solid, ColorClasses::blue);

  start0C = cameraMatrixInv * Vector3<>(1000, 25, 0.); // field2camera
  end0C = cameraMatrixInv * Vector3<>(0, 25, 0.); // field2camera
  camera2image(start0C, start0I);
  camera2image(end0C, end0I);

  LINE("module:CameraMatrixProvider:calibrationHelper", start0I.x, start0I.y, end0I.x, end0I.y, 1, Drawings::ps_solid, ColorClasses::blue);

  start0C = cameraMatrixInv * Vector3<>(110, 1000, 0.); // field2camera
  end0C = cameraMatrixInv * Vector3<>(110, -1000, 0.); // field2camera
  camera2image(start0C, start0I);
  camera2image(end0C, end0I);

  LINE("module:CameraMatrixProvider:calibrationHelper", start0I.x, start0I.y, end0I.x, end0I.y, 1, Drawings::ps_solid, ColorClasses::blue);
}
