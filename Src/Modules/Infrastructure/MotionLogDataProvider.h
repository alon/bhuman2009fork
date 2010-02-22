/** 
* @file MotionLogDataProvider.h
* This file declares a module that provides data replayed from a log file.
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
*/

#ifndef __MotionLogDataProvider_h_
#define __MotionLogDataProvider_h_

#include "Tools/Module/Module.h"
#include "Representations/Infrastructure/JointData.h"
#include "Representations/Infrastructure/SensorData.h"
#include "Representations/Infrastructure/KeyStates.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/BoardInfo.h"
#include "Representations/MotionControl/OdometryData.h"
#include "Tools/MessageQueue/InMessage.h"

MODULE(MotionLogDataProvider)
  PROVIDES_WITH_MODIFY_AND_OUTPUT(JointData)
  PROVIDES_WITH_MODIFY_AND_OUTPUT(SensorData)
  PROVIDES_WITH_MODIFY_AND_OUTPUT(KeyStates)
  PROVIDES_WITH_MODIFY(FrameInfo)
  PROVIDES_WITH_MODIFY_AND_OUTPUT(OdometryData)
  PROVIDES_WITH_MODIFY(GroundTruthOdometryData)
  PROVIDES_WITH_MODIFY_AND_OUTPUT(BoardInfo)
  PROVIDES(FilteredSensorData)
  PROVIDES(FilteredJointData)
END_MODULE

class MotionLogDataProvider : public MotionLogDataProviderBase
{
private:
  PROCESS_WIDE_STORAGE_STATIC MotionLogDataProvider* theInstance; /**< Points to the only instance of this class in this process or is 0 if there is none. */
  bool frameDataComplete; /**< Were all messages of the current frame received? */
  Streamable* representationBuffer[numOfDataMessageIDs]; /**< The array of all logable representations. */
  GroundTruthOdometryData* theGroundTruthOdometryData; /** GroundTruthOdometryData has no message id, so we need a separate copy. */

#ifndef RELEASE
  OdometryData lastOdometryData;
#endif

  void update(JointData& jointData);
  void update(SensorData& sensorData);
  void update(KeyStates& keyStates);
  void update(FrameInfo& frameInfo);
  void update(OdometryData& odometryData);
  void update(GroundTruthOdometryData& odometryData);
  void update(BoardInfo& boardInfo);
  void update(FilteredSensorData&);
  void update(FilteredJointData&);

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
  MotionLogDataProvider();

  /**
  * Destructor.
  */
  ~MotionLogDataProvider();

  /** 
  * The method is called for every incoming debug message.
  * @param message An interface to read the message from the queue.
  * @return Was the message handled?
  */
  static bool handleMessage(InMessage& message);

  /** 
  * The method returns whether idProcessFinished was received.
  * @return Were all messages of the current frame received?
  */
  static bool isFrameDataComplete();
};

#endif // __MotionLogDataProvider_h_
