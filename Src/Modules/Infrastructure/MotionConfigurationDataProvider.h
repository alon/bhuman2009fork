/** 
* @file MotionConfigurationDataProvider.h
* This file declares a module that provides data loaded from configuration files.
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
*/

#ifndef __MotionConfigurationDataProvider_h_
#define __MotionConfigurationDataProvider_h_

#include "Tools/Module/Module.h"
#include "Tools/MessageQueue/InMessage.h"
#include "Representations/Configuration/RobotName.h"
#include "Representations/Configuration/JointCalibration.h"
#include "Representations/Configuration/SensorCalibration.h"
#include "Representations/Configuration/RobotDimensions.h"
#include "Representations/Configuration/MassCalibration.h"
#include "Representations/Infrastructure/SensorData.h"

MODULE(MotionConfigurationDataProvider)
  USES(SensorData)
  PROVIDES_WITH_MODIFY(RobotName)
  PROVIDES_WITH_MODIFY(JointCalibration)
  PROVIDES_WITH_MODIFY(SensorCalibration)
  PROVIDES_WITH_MODIFY(RobotDimensions)
  PROVIDES_WITH_MODIFY(MassCalibration)
END_MODULE

class MotionConfigurationDataProvider : public MotionConfigurationDataProviderBase
{
private:
  std::string robot;
  JointCalibration* theJointCalibration;
  SensorCalibration* theSensorCalibration;
  RobotDimensions* theRobotDimensions;
  MassCalibration* theMassCalibration;
  double fsrOffsets[8], 
         fsrFullSum,
         fsrSumFactorLeft,
         fsrSumFactorRight;

  bool loadedJointCalibration,
       loadedSensorCalibration,
       loadedRobotDimensions,
       loadedBalanceParameters,
       loadedMassCalibration;

  void update(RobotName& robotName);
  void update(JointCalibration& jointCalibration);
  void update(SensorCalibration& sensorCalibration);
  void update(RobotDimensions& robotDimensions);
  void update(MassCalibration& massCalibration);

  void readIfRequired();
  void readJointCalibration();
  void readSensorCalibration();
  void readRobotDimensions();
  void readMassCalibration();

public:
  /**
  * Default constructor.
  */
  MotionConfigurationDataProvider();

  /**
  * Destructor.
  */
  ~MotionConfigurationDataProvider();
};

#endif // __MotionConfigurationDataProvider_h_
