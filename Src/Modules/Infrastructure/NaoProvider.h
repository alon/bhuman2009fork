/**
* @file Modules/Infrastructure/NaoProvider.h
* The file declares a module that provides information from the Nao via DCM.
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
*/

#ifndef __NaoProvider_H__
#define __NaoProvider_H__


#include "Tools/Module/Module.h"
#include "Representations/Configuration/JointCalibration.h"
#include "Representations/Infrastructure/JointData.h"
#include "Representations/Infrastructure/SensorData.h"
#include "Representations/Configuration/SensorCalibration.h"
#include "Representations/Infrastructure/KeyStates.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/LEDRequest.h"
#include "Representations/Infrastructure/BoardInfo.h"

MODULE(NaoProvider)
  REQUIRES(JointCalibration)
  REQUIRES(JointData)
  REQUIRES(LEDRequest)
  REQUIRES(SensorCalibration)
  REQUIRES(CognitionFrameInfo)
  PROVIDES_WITH_MODIFY_AND_OUTPUT(JointData)
  PROVIDES_WITH_MODIFY_AND_OUTPUT(SensorData)
  PROVIDES_WITH_MODIFY_AND_OUTPUT(KeyStates)
  PROVIDES_WITH_MODIFY(FrameInfo)
  PROVIDES_WITH_OUTPUT(BoardInfo) // MODIFY is also available, but defined elsewhere
  USES(JointRequest) // Will be accessed in send()
END_MODULE


#ifdef TARGET_ROBOT

/**
* @class NaoProvider
* A module that provides information from the Nao.
*/
class NaoProvider : public NaoProviderBase
{
private:
  PROCESS_WIDE_STORAGE_STATIC NaoProvider* theInstance; /**< The only instance of this module. */

  NaoBody naoBody;
  SensorData sensorData; /**< The last sensor data received. */
  KeyStates keyStates; /**< The last key states received. */
  BoardInfo boardInfo; /**< Information about the connection to all boards of the robot. */
  unsigned lastUSMeasurementTime; /**< Last time when sonar was fired. */
  unsigned lastUSSensorChangeTime; /**< The last time we changed us sensor side. */
  int lastUSSensor;
  int usOrder[4];
  int usOrderCntr;
  int lastAck[BoardInfo::numOfBoards]; 
  unsigned lastTimeWhenAck[BoardInfo::numOfBoards]; 

#ifndef RELEASE
  double clippedLastFrame[JointData::numOfJoints]; /**< Array that indicates whether a certain joint value was clipped in the last frame (and what was the value)*/
#endif

  void update(JointData& jointData);
  void update(SensorData& sensorData) {sensorData = this->sensorData;}
  void update(KeyStates& keyStates) {keyStates = this->keyStates;}
  void update(FrameInfo& frameInfo) {frameInfo.time = theJointData.timeStamp;}
  void update(BoardInfo& boardInfo) {boardInfo = this->boardInfo;}

  /**
  * The function sends a command to the Nao.
  */
  void send();

  /**
  * The method overwrites the LEDRequest to indicate problems with 
  * Nao's microcontroller boards.
  * @param ledRequest The request that might be overwritten.
  */
  void checkBoardState(LEDRequest& ledRequest);

public:
  /**
  * Constructor.
  */
  NaoProvider();

  /**
  * Destructor.
  */
  ~NaoProvider();

  /**
  * The method is called by process Motion to send the requests to the Nao.
  */
  static void finishFrame();

  static void prepareFrame();
};

#else 
//TARGET_ROBOT not defined here (Simulator).


class NaoProvider : public NaoProviderBase
{
private:
  void update(JointData& jointData) {}
  void update(SensorData& sensorData) {}
  void update(KeyStates& keyStates) {}
  void update(FrameInfo& frameInfo) {}
  void update(BoardInfo& boardInfo) {}
  void send();
public:
  NaoProvider() {}
  ~NaoProvider() {}
  static void finishFrame() {}
  static void prepareFrame() {}
};


#endif


#endif  //__NaoProvider_h_
