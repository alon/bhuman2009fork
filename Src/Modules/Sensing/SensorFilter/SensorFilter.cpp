/**
* @file SensorFilter.cpp
* Implementation of module SensorFilter.
* @author Colin Graf
*/

#include "SensorFilter.h"
#include "Tools/Debugging/DebugDrawings.h"
#include "Representations/MotionControl/SpecialActionRequest.h"
#include "Tools/Math/Matrix.h"

MAKE_MODULE(SensorFilter, Sensing)

SensorFilter::SensorFilter()
{
  p.processNoise = Vector2<>(fromDegrees(5.), fromDegrees(5.));
  p.gyroNoise = Vector2<>(fromDegrees(0.5), fromDegrees(0.5));
  p.accNoise = Vector3<>(0.002, 0.002, 0.002);
  p.calculatedAccNoise = Vector3<>(0.001, 0.001, 0.001);
  p.calculatedAccCarpetFactor = Vector2<>(0., 0.3);// 0.45;
  p.calculatedAccLimit = Vector2<>(fromDegrees(10.), fromDegrees(20.));

  p.expectedGyroNoise = Vector2<>(1., 1.);
  p.expectedAccNoise = Vector3<>(1., 1., 1.);

  p.gyroOffsetProcessNoise = Vector2<>(0.05 / 50., 0.05 / 50.);
  p.gyroOffsetStandNoise = Vector2<>(0.01, 0.01);
  p.gyroOffsetWalkNoise = Vector2<>(0.1, 0.1);

  reset();
}

void SensorFilter::reset()
{
  lastSensorDataTimeStamp = 0;
  lastSafeAcc = Vector3<float>(0.f, 0.f, -1.f);
  Vector2<float> lastSafeGyro = Vector2<float>();
  Vector2<float> lastGyro = Vector2<float>();
  Vector3<float> lastAcc = Vector3<float>();
  Vector2<> lastAngle = Vector2<>();
  inertiaSensorDrops = 1000;
  gyroOffsetVariance = Vector2<>();
  gyroOffset = Vector2<>();
  gyroDiscalibration = Vector2<>();
  isGyroCalibrated = false;
  angleEstimator.setUncertainty(p.processNoise * 3.);
  lastMotion = MotionRequest::specialAction;
  lastPositionInWalkCycle = 0;
  gyroValues.init();
  gyroAvgs.init();
}

void SensorFilter::update(FilteredSensorData& filteredSensorData)
{
  MODIFY("module:SensorFilter:parameters", p);

  if(theSensorData.timeStamp < lastSensorDataTimeStamp)
    reset();

  if(theSensorData.timeStamp == lastSensorDataTimeStamp)
    return;

  // copy sensor data (without touching acc, gyro and angle sensor values)
  Vector3<float> lastAcc(filteredSensorData.data[SensorData::accX], filteredSensorData.data[SensorData::accY], filteredSensorData.data[SensorData::accZ]);
  Vector3<float> lastGyro(filteredSensorData.data[SensorData::gyroX], filteredSensorData.data[SensorData::gyroY], filteredSensorData.data[SensorData::gyroZ]);
  Vector2<float> lastAngle(filteredSensorData.data[SensorData::angleX], filteredSensorData.data[SensorData::angleY]);
  (SensorData&)filteredSensorData = theSensorData;
  filteredSensorData.data[SensorData::accX] = lastAcc.x;
  filteredSensorData.data[SensorData::accY] = lastAcc.y;
  filteredSensorData.data[SensorData::accZ] = lastAcc.z;
  filteredSensorData.data[SensorData::gyroX] = lastGyro.x;
  filteredSensorData.data[SensorData::gyroY] = lastGyro.y;
  filteredSensorData.data[SensorData::gyroZ] = lastGyro.z;
  filteredSensorData.data[SensorData::angleX] = lastAngle.x;
  filteredSensorData.data[SensorData::angleY] = lastAngle.y;

  // improved angleX/Y, gyroX/Y/Z
  updateAngleEstimator(filteredSensorData);

  //
  PLOT("module:SensorFilter:rawAngleX", toDegrees(double(theSensorData.data[SensorData::angleX])));
  PLOT("module:SensorFilter:rawAngleY", toDegrees(double(theSensorData.data[SensorData::angleY])));

  PLOT("module:SensorFilter:rawAccX", double(theSensorData.data[SensorData::accX]));
  PLOT("module:SensorFilter:rawAccY", double(theSensorData.data[SensorData::accY]));
  PLOT("module:SensorFilter:rawAccZ", double(theSensorData.data[SensorData::accZ]));
    
  PLOT("module:SensorFilter:rawGyroX", toDegrees(double(theSensorData.data[SensorData::gyroX])));
  PLOT("module:SensorFilter:rawGyroY", toDegrees(double(theSensorData.data[SensorData::gyroY])));
  PLOT("module:SensorFilter:rawGyroZ", toDegrees(double(theSensorData.data[SensorData::gyroZ])));

  PLOT("module:SensorFilter:angleX", toDegrees(double(filteredSensorData.data[SensorData::angleX])));
  PLOT("module:SensorFilter:angleY", toDegrees(double(filteredSensorData.data[SensorData::angleY])));

  PLOT("module:SensorFilter:accX", double(filteredSensorData.data[SensorData::accX]));
  PLOT("module:SensorFilter:accY", double(filteredSensorData.data[SensorData::accY]));
  PLOT("module:SensorFilter:accZ", double(filteredSensorData.data[SensorData::accZ]));
  
  PLOT("module:SensorFilter:gyroX", toDegrees(double(filteredSensorData.data[SensorData::gyroX])));
  PLOT("module:SensorFilter:gyroY", toDegrees(double(filteredSensorData.data[SensorData::gyroY])));
  PLOT("module:SensorFilter:gyroZ", toDegrees(double(filteredSensorData.data[SensorData::gyroZ])));

  PLOT("module:SensorFilter:us", filteredSensorData.data[SensorData::us]);

  lastSensorDataTimeStamp = theSensorData.timeStamp;
}

void SensorFilter::updateAngleEstimator(FilteredSensorData& filteredSensorData)
{
  if(lastSensorDataTimeStamp > 0 && theSensorData.timeStamp > lastSensorDataTimeStamp)
  {
    const double timeScale = (theSensorData.timeStamp - lastSensorDataTimeStamp) / 1000.;
    const double timeScaleTimeScale = timeScale * timeScale;

    // process update; based on calculated rotation offset
    const Pose3D& footLeft(theRobotModel.limbs[RobotModel::footLeft]);
    const Pose3D& footRight(theRobotModel.limbs[RobotModel::footRight]);
    const Pose3D& footLeftInvert(footLeft.invert());
    const Pose3D& footRightInvert(footLeft.invert());
    Matrix2x2<> processNoise(
        p.processNoise.x * p.processNoise.x * timeScaleTimeScale, 0.,
        0., p.processNoise.y * p.processNoise.y * timeScaleTimeScale);
    
    Vector3<> offset;
    AngleEstimator::RotationMatrix calculatedRotation;
    if(footLeftInvert.translation.z > footRightInvert.translation.z)
    {
      calculatedRotation = footLeft.rotation.invert();
      offset = ((const AngleEstimator::RotationMatrix&)(lastFootLeft * calculatedRotation)).getAngleAxis();
    }
    else
    {
      calculatedRotation = footRight.rotation.invert();
      offset = ((const AngleEstimator::RotationMatrix&)(lastFootRight * calculatedRotation)).getAngleAxis();      
    }
    if(fabs(footLeftInvert.translation.z - footRightInvert.translation.z) < 3.) // use interpolation between the calculated rotation of each leg
    {
      const Vector3<> calRot((((const AngleEstimator::RotationMatrix&)footLeft.rotation.invert()).getAngleAxis() + ((const AngleEstimator::RotationMatrix&)footRight.rotation.invert()).getAngleAxis()) * 0.5);
      calculatedRotation = AngleEstimator::RotationMatrix(calRot);
    }

    angleEstimator.processUpdate(offset, processNoise);
    lastFootLeft = footLeft.rotation;
    lastFootRight = footRight.rotation;

    // make sure acc and gyro sensor values are not broken
    const Vector2<float>& expectedGyro(lastGyro);
    const Vector3<float>& expectedAcc(lastAcc);
    Vector2<float> gyro(theSensorData.data[SensorData::gyroX], theSensorData.data[SensorData::gyroY]);
    Vector3<float> acc(theSensorData.data[SensorData::accX], theSensorData.data[SensorData::accY], theSensorData.data[SensorData::accZ]);
    if(fabs(gyro.x - expectedGyro.x) > p.expectedGyroNoise.x ||
       fabs(gyro.y - expectedGyro.y) > p.expectedGyroNoise.y ||
       fabs(acc.x - expectedAcc.x) > p.expectedAccNoise.x ||
       fabs(acc.y - expectedAcc.y) > p.expectedAccNoise.y ||
       fabs(acc.z - expectedAcc.z) > p.expectedAccNoise.z )
    {
      //if(inertiaSensorDrops == 0)
        //OUTPUT(idText, text, "Dropped broken inertia sensor data");
      gyro.x = SensorData::off;
      gyro.y = SensorData::off;
      acc.x = SensorData::off;
      acc.y = SensorData::off;
      acc.z = SensorData::off;
      inertiaSensorDrops++;
    }
    else
      inertiaSensorDrops = 0;

    // continuous gyro auto-calibration stuff
    DECLARE_PLOT("module:SensorFilter:gyroAvgX");
    DECLARE_PLOT("module:SensorFilter:gyroOffsetX");
    if((lastMotion == MotionRequest::stand && gyroValues.getNumberOfEntries() >= 50) || // ~1 sec
       (lastMotion == MotionRequest::walk && gyroValues.getNumberOfEntries() > 1 && theMotionInfo.positionInWalkCycle < lastPositionInWalkCycle && observedFullWalkCycle))
    {
      Vector2<float> gyroAvg(gyroValues.getSum() / float(gyroValues.getNumberOfEntries()));
      PLOT("module:SensorFilter:gyroAvgX", gyroAvg.x);
      gyroValues.init();

      if(gyroAvgs.getNumberOfEntries() > 0)
      {
        const GyroAvg& oldestGyroAvg(gyroAvgs.getEntry(gyroAvgs.getNumberOfEntries() - 1));
        gyroDiscalibration.x = (gyroAvg.x - oldestGyroAvg.avg.x) / (theSensorData.timeStamp - oldestGyroAvg.timeStamp) * theRobotDimensions.motionCycleTime * 1000.;
        gyroDiscalibration.y = (gyroAvg.y - oldestGyroAvg.avg.y) / (theSensorData.timeStamp - oldestGyroAvg.timeStamp) * theRobotDimensions.motionCycleTime * 1000.;
      }
      gyroAvgs.add(GyroAvg(gyroAvg, theSensorData.timeStamp));

      // update the gyro offset filter
      if(!isGyroCalibrated)
      {
        // initialize the filters
        gyroOffset = Vector2<>(gyroAvg.x, gyroAvg.y);
        gyroOffsetVariance = Vector2<>(p.gyroOffsetStandNoise.x * p.gyroOffsetStandNoise.x, p.gyroOffsetStandNoise.y * p.gyroOffsetStandNoise.y);
        isGyroCalibrated = true;
      }
      else
      {
        const Vector2<>& noise(theMotionSelection.targetMotion == MotionRequest::stand ? p.gyroOffsetStandNoise : p.gyroOffsetWalkNoise);
        Vector2<> kalmanGain(
          gyroOffsetVariance.x / (gyroOffsetVariance.x + noise.x * noise.x),
          gyroOffsetVariance.y / (gyroOffsetVariance.y + noise.y * noise.y));
        gyroOffset += Vector2<>(
          kalmanGain.x * (gyroAvg.x - gyroOffset.x),
          kalmanGain.y * (gyroAvg.y - gyroOffset.y));
        gyroOffsetVariance -= Vector2<>(
          kalmanGain.x * gyroOffsetVariance.x,
          kalmanGain.y * gyroOffsetVariance.y);
      }
      PLOT("module:SensorFilter:gyroOffsetX", gyroOffset.x);
    }
    else if(lastMotion == MotionRequest::walk  && theMotionInfo.positionInWalkCycle < lastPositionInWalkCycle && !observedFullWalkCycle)
    {
      observedFullWalkCycle = true;
      gyroValues.init();
    }

    if(theMotionSelection.targetMotion != lastMotion || // motion change
      theMotionSelection.targetMotion != theMotionInfo.executedMotionRequest.motion ||  // interpolating
      !theGroundContactState.contact)
    {
      observedFullWalkCycle = false;
      gyroValues.init();
    }
    else
    {
      if(theMotionSelection.targetMotion == MotionRequest::stand)
      {
        if(gyro.x != SensorData::off && gyro.y != SensorData::off && acc.z != SensorData::off && acc.z < -0.9)
          gyroValues.add(gyro);
      }
      else if(theMotionSelection.targetMotion == MotionRequest::walk)
      {
        if(fabs(theMotionSelection.walkRequest.speed.translation.y) < 10 && 
          fabs(theMotionSelection.walkRequest.speed.translation.x) < 20 && 
          fabs(theMotionSelection.walkRequest.speed.rotation) < 0.15)
        {
          if(gyro.x != SensorData::off && gyro.y != SensorData::off && acc.z != SensorData::off && acc.z < -0.8)
            gyroValues.add(gyro);
          else if(inertiaSensorDrops <= 1 && lastSafeAcc.z < -0.8)
            gyroValues.add(lastSafeGyro);
          else
            observedFullWalkCycle = false;
        }
        else
          observedFullWalkCycle = false;
      }
    }
    lastMotion = theMotionSelection.targetMotion;
    lastPositionInWalkCycle = theMotionInfo.positionInWalkCycle;
    MODIFY("module:SensorFilter:gyroOffset", gyroOffset);
    MODIFY("module:SensorFilter:gyroDiscalibration", gyroDiscalibration);
    //gyroOffset += gyroDiscalibration;
    gyroOffsetVariance += Vector2<>(p.gyroOffsetProcessNoise.x * p.gyroOffsetProcessNoise.x, p.gyroOffsetProcessNoise.y * p.gyroOffsetProcessNoise.y);

    // gyro sensor update
    if(gyro.x != SensorData::off && gyro.y != SensorData::off && isGyroCalibrated)
    {
      Vector2<> scaledGyro(gyro.x - (gyroOffset.x + gyroDiscalibration.x), gyro.y - (gyroOffset.y + gyroDiscalibration.y));
      scaledGyro *= timeScale;
      Matrix2x2<> dynamicNoise(
        p.gyroNoise.x * p.gyroNoise.x * timeScaleTimeScale, 0.,
        0., p.gyroNoise.y * p.gyroNoise.y * timeScaleTimeScale);
      angleEstimator.gyroSensorUpdate(scaledGyro, dynamicNoise);
    }

    // fake acceleration sensor update
    if(fabs(lastAngle.x) < p.calculatedAccLimit.x && fabs(lastAngle.y) < p.calculatedAccLimit.y)
    {
      Vector3<> acc(calculatedRotation * Vector3<>(0., 0., -1.));
      acc.x *= -1. * (1. - p.calculatedAccCarpetFactor.x);
      acc.y *= -1. * (1. - p.calculatedAccCarpetFactor.y);
      acc.x += theSensorCalibration.accXOffset;
      acc.y += theSensorCalibration.accYOffset;
      Vector3<> noise(p.calculatedAccNoise);
      Matrix3x3<> accNoise(
        noise.x * noise.x, 0., 0.,
        0., noise.y * noise.y, 0.,
        0., 0., noise.z * noise.z);
      angleEstimator.accSensorUpdate(acc, accNoise);
    }

    // acceleration sensor update
    else if(acc.x != SensorData::off && acc.y != SensorData::off && acc.z != SensorData::off)
    {
      Vector3<> accf(acc.x, acc.y, acc.z);
      Vector3<> noise(p.accNoise);
      Matrix3x3<> accNoise(
        noise.x * noise.x, 0., 0.,
        0., noise.y * noise.y, 0.,
        0., 0., noise.z * noise.z);
      angleEstimator.accSensorUpdate(accf, accNoise);
    }

    // get angle and gyro from the estimator
    lastAngle = angleEstimator.getAngles();
    filteredSensorData.data[SensorData::angleX] = float(lastAngle.x);
    filteredSensorData.data[SensorData::angleY] = float(lastAngle.y);
    //filteredSensorData.data[SensorData::angleX] = theSensorData.data[SensorData::angleX];
    //filteredSensorData.data[SensorData::angleY] = theSensorData.data[SensorData::angleY];
    const Vector3<>& newGyro(angleEstimator.getAngleOffset() / timeScale);
    filteredSensorData.data[SensorData::gyroX] = float(newGyro.x);
    filteredSensorData.data[SensorData::gyroY] = float(newGyro.y);
    filteredSensorData.data[SensorData::gyroZ] = float(newGyro.z);

    // try not to provide broken acc sensor values
    filteredSensorData.data[SensorData::accX] = (acc.x != SensorData::off) ? acc.x : lastSafeAcc.x;
    filteredSensorData.data[SensorData::accY] = (acc.y != SensorData::off) ? acc.y : lastSafeAcc.y;
    filteredSensorData.data[SensorData::accZ] = (acc.z != SensorData::off) ? acc.z : lastSafeAcc.z;

    // backup gyro and acc values
    if(gyro.x != SensorData::off)
      lastGyro = lastSafeGyro = gyro;
    if(acc.x != SensorData::off)
      lastAcc = lastSafeAcc = acc;
    if(inertiaSensorDrops > 3)
    {
      lastGyro = Vector2<float>(theSensorData.data[SensorData::gyroX], theSensorData.data[SensorData::gyroY]);
      lastAcc = Vector3<float>(theSensorData.data[SensorData::accX], theSensorData.data[SensorData::accY], theSensorData.data[SensorData::accZ]);
    }
  }
}
