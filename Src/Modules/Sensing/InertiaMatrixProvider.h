/**
* @file InertiaMatrixProvider.h
* Declaration of module InertiaMatrixProvider.
* @author Colin Graf
*/

#ifndef InertiaMatrixProvider_H
#define InertiaMatrixProvider_H

#include "Tools/Module/Module.h"
#include "Representations/Infrastructure/SensorData.h"
#include "Representations/Sensing/RobotModel.h"
#include "Representations/MotionControl/OdometryData.h"
#include "Representations/Sensing/InertiaMatrix.h"
#include "Representations/Perception/GroundContactState.h"
#include "Representations/Configuration/RobotDimensions.h"

MODULE(InertiaMatrixProvider)
  REQUIRES(FilteredSensorData)
  REQUIRES(RobotModel)
  REQUIRES(RobotDimensions)
  REQUIRES(GroundContactState)
  PROVIDES(InertiaMatrix)
  USES(InertiaMatrix)
  PROVIDES_WITH_MODIFY_AND_OUTPUT(OdometryData)
END_MODULE

/**
* @class InertiaMatrixProvider
* A module that provides the (estimated) position and velocity of the inertia board.
*/
class InertiaMatrixProvider : public InertiaMatrixProviderBase
{
public:
  /** 
  * Default constructor.
  */
  InertiaMatrixProvider();

private:
  double lastLeftFootZRotation; /**< The last z-rotation of the left foot. */
  double lastRightFootZRotation; /**< The last z-rotation of the right foot. */

  Vector3<double> lastFootSpan; /**< The last span between both feet. */
  Pose3D lastInertiaMatrix; /**< The last inertia matrix for calculating the odometry offset. */

  /** Updates the InertiaMatrix representation.
  * @param inertiaMatrix The inertia matrix representation which is updated by this module.
  */
  void update(InertiaMatrix& inertiaMatrix);

  /** Updates the OdometryData representation.
  * @param odometryData The odometry data representation which is updated by this module.
  */
  void update(OdometryData& odometryData);


};

#endif
