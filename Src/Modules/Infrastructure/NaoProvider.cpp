/**
* @file Modules/Infrastructure/NaoProvider.cpp
* The file declares a module that provides information from the Nao via DCM.
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
*/

//#define MEASURE_DELAY

#include "NaoProvider.h"

#ifdef TARGET_ROBOT

#ifdef MEASURE_DELAY
#include "Tools/Streams/InStreams.h"
#endif
#include "Tools/Debugging/DebugDrawings.h"
#include "Tools/Settings.h"

PROCESS_WIDE_STORAGE NaoProvider* NaoProvider::theInstance = 0;

NaoProvider::NaoProvider() :
lastUSMeasurementTime(0),
lastUSSensorChangeTime(0),
lastUSSensor(0)
{
  NaoProvider::theInstance = this;

  Global::getSettings().robot = naoBody.getName();
  OUTPUT(idText, text, "Hi, I am " << Global::getSettings().robot << ".");
  OUTPUT(idRobotname, bin, Global::getSettings().robot);

#ifndef RELEASE
  for(int i = 0; i < JointData::numOfJoints; ++i)
    clippedLastFrame[i] = JointData::off;
#endif
  for(int i = 0; i < BoardInfo::numOfBoards; ++i)
  {
    lastAck[i] = 0;
    lastTimeWhenAck[i] = 0;
  }

  usOrderCntr = 0;
  usOrder[0] = 0; usOrder[1] = 1; usOrder[2] = 3; usOrder[3] = 2;
}

NaoProvider::~NaoProvider()
{
  NaoProvider::theInstance = 0;
}

void NaoProvider::prepareFrame()
{
  if(theInstance)
    theInstance->naoBody.wait();
}

void NaoProvider::send()
{
  float hardness = -1.0f;
  MODIFY("hardness", hardness);
  int dcmDelay = 20;
  MODIFY("dcmDelay", dcmDelay);
  int usDelay = 60;
  MODIFY("usDelay", usDelay);
  DEBUG_RESPONSE("module:NaoProvider:ClippingInfo",);

#ifdef MEASURE_DELAY
  OutTextFile stream("delay.log", true);
  stream << "jointRequest";
  stream << theJointRequest.angles[JointData::legLeft2];
  stream << theJointRequest.angles[JointData::legLeft3];
  stream << theJointRequest.angles[JointData::legLeft4];
  stream << endl;
#endif

  float* actuators, *usActuator;
  naoBody.openActuators(actuators, usActuator);
  int j = 0;
  for(int i = 0; i < JointData::numOfJoints; ++i)
  { 
    if(i == JointData::legRight0) // missing on Nao
      ++i;

    if(theJointRequest.angles[i] == JointData::off)
    {
      actuators[j++] = 0.0f;
      actuators[j++] = 0.0f;
    }
    else
    {
      double d = theJointRequest.angles[i] + theJointCalibration.joints[i].offset;
      if(d > theJointCalibration.joints[i].maxAngle) 
      {
        DEBUG_RESPONSE("module:NaoProvider:ClippingInfo", 
          if(clippedLastFrame[i] != d)
          {
            char tmp[64];
            sprintf(tmp, "warning: clipped joint %s at %.03f, requested %.03f.", JointData::getJointName((JointData::Joint)i), toDegrees(theJointCalibration.joints[i].maxAngle), toDegrees(theJointRequest.angles[i]));
            OUTPUT(idText, text, tmp);
            clippedLastFrame[i] = d;
          }
        ); 
        d = theJointCalibration.joints[i].maxAngle;
      }
      else if(d < theJointCalibration.joints[i].minAngle) 
      {
        DEBUG_RESPONSE("module:NaoProvider:ClippingInfo", 
          if(clippedLastFrame[i] != d)
          {
            char tmp[64];
            sprintf(tmp, "warning: clipped joint %s at %.04f, requested %.03f.", JointData::getJointName((JointData::Joint)i), toDegrees(theJointCalibration.joints[i].minAngle), toDegrees(theJointRequest.angles[i]));
            OUTPUT(idText, text, tmp);
            clippedLastFrame[i] = d;
          }
        );
        d = theJointCalibration.joints[i].minAngle;
      }
#ifndef RELEASE
      else
        clippedLastFrame[i] = JointData::off;
#endif
      actuators[j++] = float(d * theJointCalibration.joints[i].sign);
      actuators[j++] = hardness < 0.0f || hardness > 1.0f ? theJointRequest.jointHardness.hardness[i]/100.0f : hardness;
    }
  }

  LEDRequest ledRequest(theLEDRequest);
  //checkBoardState(ledRequest);

  bool on = (SystemCall::getCurrentSystemTime() / 50 & 8) != 0;
  bool fastOn = (SystemCall::getCurrentSystemTime() / 10 & 8) != 0;
  for(int i = 0; i < LEDRequest::numOfLEDs; ++i)
    actuators[j++] = (ledRequest.ledStates[i] == LEDRequest::on || 
                                (ledRequest.ledStates[i] == LEDRequest::blinking && on) ||
                                (ledRequest.ledStates[i] == LEDRequest::fastBlinking && fastOn)) 
                               ? 1.0f : 0.0f;

  if(SystemCall::getTimeSince(lastUSMeasurementTime) >= usDelay - 5) // every third frame
  {
    lastUSMeasurementTime = SystemCall::getCurrentSystemTime();
    if(SystemCall::getTimeSince(lastUSSensorChangeTime) > 150)
    {
      lastUSSensorChangeTime = lastUSMeasurementTime;
      //if(++lastUSSensor > 3) lastUSSensor = 0;
      //lastUSSensor = lastUSSensor == 0 ? 3 : 0;
      lastUSSensor = usOrder[usOrderCntr];
      if(++usOrderCntr > 3) usOrderCntr = 0;
    }
    *usActuator = float(lastUSSensor); // 0. = LeftToLeft, 1. = LeftToRight, 2. = RightToLeft, 3. = RightToRight
  }
  naoBody.closeActuators();
}

void NaoProvider::update(JointData& jointData)
{
  float* sensors = naoBody.getSensors();
  int j = 0;
  for(int i = 0; i < JointData::numOfJoints; ++i)
  {
    if(i == JointData::legRight0)
    {
      jointData.angles[i] = jointData.angles[JointData::legLeft0];
      sensorData.currents[i] = sensorData.currents[JointData::legLeft0];
      sensorData.temperatures[i] = sensorData.temperatures[JointData::legLeft0];
    }
    else
    {
      jointData.angles[i] = sensors[j++] * theJointCalibration.joints[i].sign - theJointCalibration.joints[i].offset;
      sensorData.currents[i] = short(1000 * sensors[j++]); 
      sensorData.temperatures[i] = (unsigned char) sensors[j++];
    }
  }

#ifdef MEASURE_DELAY
  OutTextFile stream("delay.log", true);
  stream << "timestamp" << SystemCall::getCurrentSystemTime();
  stream << "jointData";
  stream << jointData.angles[JointData::legLeft2];
  stream << jointData.angles[JointData::legLeft3];
  stream << jointData.angles[JointData::legLeft4];
#endif

  float currentGyroRef = 0.f;
  for(int i = 0; i < SensorData::numOfSensors; ++i)
  {
    if(i == SensorData::gyroZ)
      currentGyroRef = sensors[j++];
    else
      sensorData.data[i] = sensors[j++];
  }

  sensorData.data[SensorData::gyroX] *= theSensorCalibration.gyroXGain / 1600;
  sensorData.data[SensorData::gyroY] *= theSensorCalibration.gyroYGain / 1600;
  sensorData.data[SensorData::accX] *= theSensorCalibration.accXGain;
  sensorData.data[SensorData::accX] += theSensorCalibration.accXOffset;
  sensorData.data[SensorData::accY] *= theSensorCalibration.accYGain;
  sensorData.data[SensorData::accY] += theSensorCalibration.accYOffset;
  sensorData.data[SensorData::accZ] *= theSensorCalibration.accZGain;
  sensorData.data[SensorData::accZ] += theSensorCalibration.accZOffset;

  if(sensorData.data[SensorData::us] == 0 || SystemCall::getTimeSince(lastUSSensorChangeTime) < 110)
    sensorData.data[SensorData::us] = SensorData::off;
  else
    sensorData.data[SensorData::us] *= 1000;
  sensorData.usSensorType = SensorData::UsSensorTypes(lastUSSensor);

  sensorData.data[SensorData::fsrLFL] = sensorData.data[SensorData::fsrLFL] != 0.f ? ((1/sensorData.data[SensorData::fsrLFL] + theSensorCalibration.fsrLFLOffset) * theSensorCalibration.fsrLFLGain) : SensorData::off;
  sensorData.data[SensorData::fsrLFR] = sensorData.data[SensorData::fsrLFR] != 0.f ? ((1/sensorData.data[SensorData::fsrLFR] + theSensorCalibration.fsrLFROffset) * theSensorCalibration.fsrLFRGain) : SensorData::off;
  sensorData.data[SensorData::fsrLBL] = sensorData.data[SensorData::fsrLBL] != 0.f ? ((1/sensorData.data[SensorData::fsrLBL] + theSensorCalibration.fsrLBLOffset) * theSensorCalibration.fsrLBLGain) : SensorData::off;
  sensorData.data[SensorData::fsrLBR] = sensorData.data[SensorData::fsrLBR] != 0.f ? ((1/sensorData.data[SensorData::fsrLBR] + theSensorCalibration.fsrLBROffset) * theSensorCalibration.fsrLBRGain) : SensorData::off;
  sensorData.data[SensorData::fsrRFL] = sensorData.data[SensorData::fsrRFL] != 0.f ? ((1/sensorData.data[SensorData::fsrRFL] + theSensorCalibration.fsrRFLOffset) * theSensorCalibration.fsrRFLGain) : SensorData::off;
  sensorData.data[SensorData::fsrRFR] = sensorData.data[SensorData::fsrRFR] != 0.f ? ((1/sensorData.data[SensorData::fsrRFR] + theSensorCalibration.fsrRFROffset) * theSensorCalibration.fsrRFRGain) : SensorData::off;
  sensorData.data[SensorData::fsrRBL] = sensorData.data[SensorData::fsrRBL] != 0.f ? ((1/sensorData.data[SensorData::fsrRBL] + theSensorCalibration.fsrRBLOffset) * theSensorCalibration.fsrRBLGain) : SensorData::off;
  sensorData.data[SensorData::fsrRBR] = sensorData.data[SensorData::fsrRBR] != 0.f ? ((1/sensorData.data[SensorData::fsrRBR] + theSensorCalibration.fsrRBROffset) * theSensorCalibration.fsrRBRGain) : SensorData::off;

#ifdef MEASURE_DELAY
  stream << "sensorData";
  stream << sensorData.data[SensorData::gyroX] << sensorData.data[SensorData::gyroY] << sensorData.data[SensorData::accX] << sensorData.data[SensorData::accY] << sensorData.data[SensorData::accZ];
#endif

  for(int i = 0; i < KeyStates::numberOfKeys; ++i)
    keyStates.pressed[i] = sensors[j++] != 0;

  jointData.timeStamp = sensorData.timeStamp = SystemCall::getCurrentSystemTime();

  for(int i = 0; i < BoardInfo::numOfBoards; ++i)
  {
    boardInfo.ack[i] = (int) sensors[j++];
    boardInfo.nack[i] = (int) sensors[j++];
    boardInfo.error[i] = (int) sensors[j++];
  }

  // modify internal data structure, so checkBoardState is influenced as well
  MODIFY("representation:BoardInfo", boardInfo);

#ifndef RELEASE
  JointDataDeg jointDataDeg(jointData);
#endif
  MODIFY("representation:JointDataDeg", jointDataDeg);
}

void NaoProvider::checkBoardState(LEDRequest& ledRequest)
{
  const static LEDRequest::LED leds[BoardInfo::numOfBoards] =
  {
    LEDRequest::earsLeft72Deg, // chestBoard
    LEDRequest::earsRight72Deg, // battery
    LEDRequest::earsLeft108Deg, // usBoard
    LEDRequest::earsRight108Deg, // inertialSensor
    LEDRequest::earsLeft324Deg, // headBoard
    LEDRequest::faceRightBlue0Deg, // earLeds
    LEDRequest::earsRight324Deg, // faceBoard
    LEDRequest::earsLeft0Deg, // leftShoulderBoard
    LEDRequest::earsLeft36Deg, //leftArmBoard
    LEDRequest::earsRight0Deg, // rightShoulderBoard
    LEDRequest::earsRight36Deg, // rightArmBoard
    LEDRequest::earsLeft288Deg, // leftHipBoard
    LEDRequest::earsLeft252Deg, //leftThighBoard
    LEDRequest::earsLeft216Deg, // leftShinBoard
    LEDRequest::earsLeft180Deg, // leftFootBoard
    LEDRequest::earsRight288Deg, // rightHipBoard
    LEDRequest::earsRight252Deg, // rightThighBoard
    LEDRequest::earsRight216Deg, // rightShinBoard
    LEDRequest::earsRight180Deg // rightFootBoard
  };

  unsigned now = SystemCall::getCurrentSystemTime();
  bool camera = now - theCognitionFrameInfo.time < 2000,
       error = !camera;

  int i;
  for(i = 0; i < BoardInfo::numOfBoards; ++i)
  {
    if(lastAck[i] != boardInfo.ack[i])
    {
      lastTimeWhenAck[i] = now;
      lastAck[i] = boardInfo.ack[i];
    }
    if(now - lastTimeWhenAck[i] > 2000 || (boardInfo.error[i] && (boardInfo.error[i] & 0xf0) != 0xd0))
      error = true;
  }

  if(error)
  {
    for(i = 0; i < LEDRequest::chestRed; ++i)
      ledRequest.ledStates[i] = LEDRequest::off;

    for(i = 0; i < BoardInfo::numOfBoards; ++i)
      if(now - lastTimeWhenAck[i] > 2000 || (boardInfo.error[i] && (boardInfo.error[i] & 0xf0) != 0xd0))
        ledRequest.ledStates[leds[i]] = LEDRequest::on;
      else
        ledRequest.ledStates[leds[i]] = LEDRequest::blinking;

    for(i = LEDRequest::faceLeftBlue0Deg; i < LEDRequest::faceRightRed0Deg; ++i)
      ledRequest.ledStates[i] = camera ? LEDRequest::off : LEDRequest::blinking;

    bool ear = ledRequest.ledStates[LEDRequest::faceRightBlue0Deg] != LEDRequest::on;
    for(i = LEDRequest::faceRightBlue0Deg; i < LEDRequest::LEDRequest::earsLeft0Deg; ++i)
      ledRequest.ledStates[i] = ear ? LEDRequest::off : LEDRequest::blinking;
  }
}

void NaoProvider::finishFrame()
{
  if(theInstance)
    theInstance->send();
}


#endif // TARGET_ROBOT

MAKE_MODULE(NaoProvider, Infrastructure)
