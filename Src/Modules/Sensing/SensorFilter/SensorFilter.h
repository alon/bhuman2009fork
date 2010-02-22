/**
* @file SensorFilter.h
* Declaration of module SensorFilter.
* @author Colin Graf
*/

#ifndef SensorFilter_H
#define SensorFilter_H

#include "Tools/Module/Module.h"
#include "Representations/Infrastructure/SensorData.h"
#include "Representations/Configuration/SensorCalibration.h"
#include "Representations/Configuration/RobotDimensions.h"
#include "Representations/MotionControl/MotionSelection.h"
#include "Representations/MotionControl/MotionInfo.h"
#include "Representations/Sensing/RobotModel.h"
#include "Representations/Perception/GroundContactState.h"
#include "Tools/RingBufferWithSum.h"
#include "Tools/RingBuffer.h"
#include "AngleEstimator.h"

MODULE(SensorFilter)
  REQUIRES(SensorData)
  REQUIRES(RobotModel)
  REQUIRES(RobotDimensions)
  REQUIRES(SensorCalibration)
  REQUIRES(GroundContactState)
  USES(MotionSelection)
  USES(MotionInfo)
  PROVIDES_WITH_MODIFY_AND_OUTPUT(FilteredSensorData)
END_MODULE

/**
* A module for sensor data filtering.
*/
class SensorFilter : public SensorFilterBase
{
public:
  /** Default constructor. */
  SensorFilter();

private:
  /**
  * A collection of parameters for this module and the angle estimator.
  */
  class Parameters : public Streamable
  {
  private:
    /**
    * The method makes the object streamable.
    * @param in The stream from which the object is read.
    * @param out The stream to which the object is written. 
    */
    virtual void serialize(In* in, Out* out)
    {  
      STREAM_REGISTER_BEGIN();
        STREAM(processNoise);
        STREAM(gyroNoise);
        STREAM(accNoise);
        STREAM(calculatedAccNoise);
        STREAM(calculatedAccCarpetFactor);
        STREAM(calculatedAccLimit);
        STREAM(expectedGyroNoise);
        STREAM(expectedAccNoise);
        STREAM(gyroOffsetProcessNoise);
        STREAM(gyroOffsetStandNoise);
        STREAM(gyroOffsetWalkNoise);
      STREAM_REGISTER_FINISH();
    }

  public:
    /** Default constructor. */
    Parameters() {};

    Vector2<> processNoise; /**< The noise of the calculated rotation offset. (in rad/s) */
    Vector2<> gyroNoise; /**< The noise of the gyro sensors. (in rad/s) */
    Vector3<> accNoise; /**< The noise of the acceleration sensors. (in "g") */
    Vector3<> calculatedAccNoise; /**< The noise of the calculated angle. (in rad) */
    Vector2<> calculatedAccCarpetFactor; /**< A factor for modelling the difference between the calculated angle and the real one. */
    Vector2<> calculatedAccLimit; /**< Use a calculated angle up to this angle (in rad). (We use the acceleration sensors otherwise.) */
    Vector2<> expectedGyroNoise; /**< Maximum allowed deviance between expected and measured gyro values. (in rad/s) */
    Vector3<> expectedAccNoise; /**< Maximum allowed deviance between expected and measured acc values. (in "g") */
    Vector2<> gyroOffsetProcessNoise; /**< The process noise of the gyro offset estimator. */
    Vector2<> gyroOffsetStandNoise; /**< The noise between gyro measurements and the gyro offset while standing. */
    Vector2<> gyroOffsetWalkNoise; /**< The noise between gyro measurements and the gyro offset while walking. */
  };

  RotationMatrix lastFootLeft; /**< The last rotation of the left foot. */
  RotationMatrix lastFootRight; /**< The last rotation of the right foot. */

  unsigned int lastSensorDataTimeStamp; /**< The sensor data time stamp of the last frame. */
  Parameters p; /**< A set of parameters for the angle estimator. */
  AngleEstimator angleEstimator; /**< Our UKF for estimating angleX and angleY. */

  Vector3<float> lastSafeAcc; /** The last valid acceleration sensor data. */
  Vector2<float> lastSafeGyro; /** The last valid gyro sensor data. */
  Vector2<float> lastGyro; /** The last acceleration sensor data. */
  Vector3<float> lastAcc;  /** The last gyro sensor data. */
  Vector2<> lastAngle; /**< The last estimated angle. */
  int inertiaSensorDrops; /**< The amount of successive and as broken recognized gyro measurements. */

  Vector2<> gyroOffsetVariance; /**< The variance of the gyro offset estimate. */
  Vector2<> gyroOffset; /**< The gyro offset estimate. */
  Vector2<> gyroDiscalibration; /**< A crude determined discalibration velocity. */
  bool isGyroCalibrated; /**< Whether we processed at least one gyro measurement. */
  MotionRequest::Motion lastMotion;
  RingBufferWithSum<Vector2<float>, 300> gyroValues;
  double lastPositionInWalkCycle;
  bool observedFullWalkCycle;

  class GyroAvg
  {
  public:
    GyroAvg() {}
    GyroAvg(const Vector2<float>& avg, unsigned int timeStamp) : avg(avg), timeStamp(timeStamp) {}
    Vector2<float> avg;
    unsigned int timeStamp;
  };

  RingBuffer<GyroAvg, 300> gyroAvgs;

  /** Resets the internal state of the module. */
  void reset();

  /**
  * Updates the FilteredSensorData representation.
  * @param filteredSensorData The sensor data representation which is updated by this module.
  */
  void update(FilteredSensorData& filteredSensorData);

  /**
  * Updates the angle estimator and inserts the gyro and acceleration sensor measurements into the estimator.
  * @param filteredSensorData The sensor data representation to update angleX/Y, gyroX/Y/Z and accX/Y/Z.
  */
  void updateAngleEstimator(FilteredSensorData& filteredSensorData);
};

#endif
