/**
* @file CameraMatrixProvider.h
* This file declares a class to calculate the position of the camera for the Nao.
* @author <a href="mailto:allli@informatik.uni-bremen.de">Alexander Härtl</a>
*/

#ifndef CameraMatrixProvider_H
#define CameraMatrixProvider_H

#include "Tools/Module/Module.h"
#include "Representations/Configuration/CameraCalibration.h"
#include "Representations/Configuration/RobotDimensions.h"
#include "Representations/Configuration/FieldDimensions.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/RobotInfo.h"
#include "Representations/Infrastructure/JointData.h"
#include "Representations/Infrastructure/SensorData.h"
#include "Representations/Infrastructure/CameraInfo.h"
#include "Representations/Perception/CameraMatrix.h"
#include "Representations/Sensing/InertiaMatrix.h"
#include "Representations/Modeling/FallDownState.h"
#include "Representations/Modeling/RobotPose.h"
#include "Representations/MotionControl/MotionInfo.h"

MODULE(CameraMatrixProvider)
  REQUIRES(FrameInfo)
  REQUIRES(RobotInfo)
  REQUIRES(CameraCalibration)
  REQUIRES(RobotDimensions)
  REQUIRES(FilteredJointData)
  REQUIRES(MotionInfo)
  REQUIRES(FallDownState)
  REQUIRES(InertiaMatrix)
  REQUIRES(RobotCameraMatrix)
  USES(FieldDimensions) // for debug drawing
  USES(RobotPose) // for debug drawing
  USES(CameraInfo) // for debug drawing
  PROVIDES_WITH_MODIFY_AND_DRAW(RobotCameraMatrix);
  PROVIDES_WITH_MODIFY_AND_OUTPUT_AND_DRAW(CameraMatrix);
END_MODULE

class CameraMatrixProvider: public CameraMatrixProviderBase
{
public:
  CameraMatrixProvider() {}

private:

  void update(CameraMatrix& cameraMatrix);
  void update(RobotCameraMatrix& robotCameraMatrix);

  void camera2image(const Vector3<>& camera, Vector2<float>& image) const;
  bool intersectLineWithCullPlane(const Vector3<>& lineBase, const Vector3<>& lineDir, 
                                                 Vector3<>& point) const;
  void drawFieldLines(const CameraMatrix& cameraMatrix) const;
};

#endif // CameraMatrixProvider_H
