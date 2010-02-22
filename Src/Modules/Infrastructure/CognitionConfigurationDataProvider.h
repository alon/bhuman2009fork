/** 
* @file CognitionConfigurationDataProvider.h
* This file declares a module that provides data loaded from configuration files.
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
*/

#ifndef __CognitionConfigurationDataProvider_h_
#define __CognitionConfigurationDataProvider_h_

#include "Tools/Module/Module.h"
#include "Tools/MessageQueue/InMessage.h"
#include "Representations/Configuration/RobotName.h"
#include "Representations/Configuration/FieldDimensions.h"
#include "Representations/Configuration/ColorTable64.h"
#include "Representations/Configuration/CameraSettings.h"
#include "Representations/Configuration/CameraCalibration.h"
#include "Representations/Configuration/RobotDimensions.h"
#include "Representations/Configuration/BehaviorConfiguration.h"

MODULE(CognitionConfigurationDataProvider)
  REQUIRES(RobotName)
  PROVIDES_WITH_DRAW(FieldDimensions)
  PROVIDES_WITH_OUTPUT(ColorTable64)
  PROVIDES_WITH_MODIFY(CameraSettings)
  PROVIDES_WITH_MODIFY(CameraCalibration)
  PROVIDES_WITH_MODIFY(RobotDimensions)
  PROVIDES_WITH_MODIFY(BehaviorConfiguration)
END_MODULE

class CognitionConfigurationDataProvider : public CognitionConfigurationDataProviderBase
{
private:
  PROCESS_WIDE_STORAGE_STATIC CognitionConfigurationDataProvider* theInstance; /**< Points to the only instance of this class in this process or is 0 if there is none. */

  FieldDimensions* theFieldDimensions;
  ColorTable64* theColorTable64;
  CameraSettings* theCameraSettings;
  CameraCalibration* theCameraCalibration;
  RobotDimensions* theRobotDimensions;
  BehaviorConfiguration* theBehaviorConfiguration;
  bool loadedFieldDimensions,
       loadedColorTable64,
       loadedCameraSettings,
       loadedCameraCalibration,
       loadedRobotDimensions,
       loadedBehaviorConfiguration;

  void update(FieldDimensions& fieldDimensions);
  void update(ColorTable64& colorTable64);
  void update(CameraSettings& cameraSettings);
  void update(CameraCalibration& cameraCalibration);
  void update(RobotDimensions& robotDimensions);
  void update(BehaviorConfiguration& behaviorConfiguration);

  void readIfRequired();
  void readFieldDimensions();
  void readColorTable64();
  void readCameraSettings();
  void readCameraCalibration();
  void readRobotDimensions();
  void readBehaviorConfiguration();

  /** 
  * The method is called for every incoming debug message by handleMessage.
  * @param message An interface to read the message from the queue.
  * @return Was the message handled?
  */
  bool handleMessage2(InMessage& message);

public:
  /**
  * Default constructor.
  */
  CognitionConfigurationDataProvider();

  /**
  * Destructor.
  */
  ~CognitionConfigurationDataProvider();

  /** 
  * The method is called for every incoming debug message.
  * @param message An interface to read the message from the queue.
  * @return Was the message handled?
  */
  static bool handleMessage(InMessage& message);
};

#endif // __CognitionConfigurationDataProvider_h_
