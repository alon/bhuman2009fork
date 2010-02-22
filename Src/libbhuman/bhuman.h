/**
* @file bhuman.h
* Declatation of shared data proved by libbhuman.
* @author Colin Graf
*/
 
#ifndef LIBBHUMAN_H__
#define LIBBHUMAN_H__

#define LBH_SEM_NAME "/bhuman_sem"
#define LBH_MEM_NAME "/bhuman_mem"
#define LBH_TRACE_MSG_LENGTH 256
#define LBH_TRACE_SIZE 16

#ifdef __cplusplus
extern "C"
{
#endif

static const char* lbhSensorNames[] = {
  
  "Device/SubDeviceList/HeadYaw/Position/Sensor/Value",
  "Device/SubDeviceList/HeadYaw/ElectricCurrent/Sensor/Value",
  "Device/SubDeviceList/HeadYaw/Temperature/Sensor/Value",
  "Device/SubDeviceList/HeadPitch/Position/Sensor/Value",
  "Device/SubDeviceList/HeadPitch/ElectricCurrent/Sensor/Value",
  "Device/SubDeviceList/HeadPitch/Temperature/Sensor/Value",
  "Device/SubDeviceList/LShoulderPitch/Position/Sensor/Value",
  "Device/SubDeviceList/LShoulderPitch/ElectricCurrent/Sensor/Value",
  "Device/SubDeviceList/LShoulderPitch/Temperature/Sensor/Value",
  "Device/SubDeviceList/LShoulderRoll/Position/Sensor/Value",
  "Device/SubDeviceList/LShoulderRoll/ElectricCurrent/Sensor/Value",
  "Device/SubDeviceList/LShoulderRoll/Temperature/Sensor/Value",
  "Device/SubDeviceList/LElbowYaw/Position/Sensor/Value",
  "Device/SubDeviceList/LElbowYaw/ElectricCurrent/Sensor/Value",
  "Device/SubDeviceList/LElbowYaw/Temperature/Sensor/Value",
  "Device/SubDeviceList/LElbowRoll/Position/Sensor/Value",
  "Device/SubDeviceList/LElbowRoll/ElectricCurrent/Sensor/Value",
  "Device/SubDeviceList/LElbowRoll/Temperature/Sensor/Value",
  "Device/SubDeviceList/RShoulderPitch/Position/Sensor/Value",
  "Device/SubDeviceList/RShoulderPitch/ElectricCurrent/Sensor/Value",
  "Device/SubDeviceList/RShoulderPitch/Temperature/Sensor/Value",
  "Device/SubDeviceList/RShoulderRoll/Position/Sensor/Value",
  "Device/SubDeviceList/RShoulderRoll/ElectricCurrent/Sensor/Value",
  "Device/SubDeviceList/RShoulderRoll/Temperature/Sensor/Value",
  "Device/SubDeviceList/RElbowYaw/Position/Sensor/Value",
  "Device/SubDeviceList/RElbowYaw/ElectricCurrent/Sensor/Value",
  "Device/SubDeviceList/RElbowYaw/Temperature/Sensor/Value",
  "Device/SubDeviceList/RElbowRoll/Position/Sensor/Value",
  "Device/SubDeviceList/RElbowRoll/ElectricCurrent/Sensor/Value",
  "Device/SubDeviceList/RElbowRoll/Temperature/Sensor/Value",
  "Device/SubDeviceList/LHipYawPitch/Position/Sensor/Value",
  "Device/SubDeviceList/LHipYawPitch/ElectricCurrent/Sensor/Value",
  "Device/SubDeviceList/LHipYawPitch/Temperature/Sensor/Value",
  "Device/SubDeviceList/LHipRoll/Position/Sensor/Value",
  "Device/SubDeviceList/LHipRoll/ElectricCurrent/Sensor/Value",
  "Device/SubDeviceList/LHipRoll/Temperature/Sensor/Value",
  "Device/SubDeviceList/LHipPitch/Position/Sensor/Value",
  "Device/SubDeviceList/LHipPitch/ElectricCurrent/Sensor/Value",
  "Device/SubDeviceList/LHipPitch/Temperature/Sensor/Value",
  "Device/SubDeviceList/LKneePitch/Position/Sensor/Value",
  "Device/SubDeviceList/LKneePitch/ElectricCurrent/Sensor/Value",
  "Device/SubDeviceList/LKneePitch/Temperature/Sensor/Value",
  "Device/SubDeviceList/LAnklePitch/Position/Sensor/Value",
  "Device/SubDeviceList/LAnklePitch/ElectricCurrent/Sensor/Value",
  "Device/SubDeviceList/LAnklePitch/Temperature/Sensor/Value",
  "Device/SubDeviceList/LAnkleRoll/Position/Sensor/Value",
  "Device/SubDeviceList/LAnkleRoll/ElectricCurrent/Sensor/Value",
  "Device/SubDeviceList/LAnkleRoll/Temperature/Sensor/Value",
  "Device/SubDeviceList/RHipRoll/Position/Sensor/Value",
  "Device/SubDeviceList/RHipRoll/ElectricCurrent/Sensor/Value",
  "Device/SubDeviceList/RHipRoll/Temperature/Sensor/Value",
  "Device/SubDeviceList/RHipPitch/Position/Sensor/Value",
  "Device/SubDeviceList/RHipPitch/ElectricCurrent/Sensor/Value",
  "Device/SubDeviceList/RHipPitch/Temperature/Sensor/Value",
  "Device/SubDeviceList/RKneePitch/Position/Sensor/Value",
  "Device/SubDeviceList/RKneePitch/ElectricCurrent/Sensor/Value",
  "Device/SubDeviceList/RKneePitch/Temperature/Sensor/Value",
  "Device/SubDeviceList/RAnklePitch/Position/Sensor/Value",
  "Device/SubDeviceList/RAnklePitch/ElectricCurrent/Sensor/Value",
  "Device/SubDeviceList/RAnklePitch/Temperature/Sensor/Value",
  "Device/SubDeviceList/RAnkleRoll/Position/Sensor/Value",
  "Device/SubDeviceList/RAnkleRoll/ElectricCurrent/Sensor/Value",
  "Device/SubDeviceList/RAnkleRoll/Temperature/Sensor/Value",
  
  "Device/SubDeviceList/InertialSensor/GyrX/Sensor/Value", // 63
  "Device/SubDeviceList/InertialSensor/GyrY/Sensor/Value",
  "Device/SubDeviceList/InertialSensor/GyrRef/Sensor/Value",
  "Device/SubDeviceList/InertialSensor/AccX/Sensor/Value",
  "Device/SubDeviceList/InertialSensor/AccY/Sensor/Value",
  "Device/SubDeviceList/InertialSensor/AccZ/Sensor/Value",
  "Device/SubDeviceList/Battery/Charge/Sensor/Value", // 69
  "Device/SubDeviceList/LFoot/FSR/FrontLeft/Sensor/Value",
  "Device/SubDeviceList/LFoot/FSR/FrontRight/Sensor/Value",
  "Device/SubDeviceList/LFoot/FSR/RearLeft/Sensor/Value",
  "Device/SubDeviceList/LFoot/FSR/RearRight/Sensor/Value",
  "Device/SubDeviceList/RFoot/FSR/FrontLeft/Sensor/Value",
  "Device/SubDeviceList/RFoot/FSR/FrontRight/Sensor/Value",
  "Device/SubDeviceList/RFoot/FSR/RearLeft/Sensor/Value",
  "Device/SubDeviceList/RFoot/FSR/RearRight/Sensor/Value",
  "Device/SubDeviceList/US/Sensor/Value",
  "Device/SubDeviceList/InertialSensor/AngleX/Sensor/Value",
  "Device/SubDeviceList/InertialSensor/AngleY/Sensor/Value",
  "Device/SubDeviceList/RFoot/Bumper/Right/Sensor/Value", // 81
  "Device/SubDeviceList/RFoot/Bumper/Left/Sensor/Value",
  "Device/SubDeviceList/LFoot/Bumper/Right/Sensor/Value",
  "Device/SubDeviceList/LFoot/Bumper/Left/Sensor/Value",
  "Device/SubDeviceList/ChestBoard/Button/Sensor/Value",
  "Device/DeviceList/ChestBoard/Ack",
  "Device/DeviceList/ChestBoard/Nack",
  "Device/DeviceList/ChestBoard/Error",
  "Device/DeviceList/Battery/Ack",
  "Device/DeviceList/Battery/Nack",
  "Device/DeviceList/Battery/Error",
  "Device/DeviceList/USBoard/Ack",
  "Device/DeviceList/USBoard/Nack",
  "Device/DeviceList/USBoard/Error",
  "Device/DeviceList/InertialSensor/Ack",
  "Device/DeviceList/InertialSensor/Nack",
  "Device/DeviceList/InertialSensor/Error",
  "Device/DeviceList/HeadBoard/Ack",
  "Device/DeviceList/HeadBoard/Nack",
  "Device/DeviceList/HeadBoard/Error",
  "Device/DeviceList/EarLeds/Ack",
  "Device/DeviceList/EarLeds/Nack",
  "Device/DeviceList/EarLeds/Error",
  "Device/DeviceList/FaceBoard/Ack",
  "Device/DeviceList/FaceBoard/Nack",
  "Device/DeviceList/FaceBoard/Error",
  "Device/DeviceList/LeftShoulderBoard/Ack",
  "Device/DeviceList/LeftShoulderBoard/Nack",
  "Device/DeviceList/LeftShoulderBoard/Error",
  "Device/DeviceList/LeftArmBoard/Ack",
  "Device/DeviceList/LeftArmBoard/Nack",
  "Device/DeviceList/LeftArmBoard/Error",
  "Device/DeviceList/RightShoulderBoard/Ack",
  "Device/DeviceList/RightShoulderBoard/Nack",
  "Device/DeviceList/RightShoulderBoard/Error",
  "Device/DeviceList/RightArmBoard/Ack",
  "Device/DeviceList/RightArmBoard/Nack",
  "Device/DeviceList/RightArmBoard/Error",
  "Device/DeviceList/LeftHipBoard/Ack",
  "Device/DeviceList/LeftHipBoard/Nack",
  "Device/DeviceList/LeftHipBoard/Error",
  "Device/DeviceList/LeftThighBoard/Ack",
  "Device/DeviceList/LeftThighBoard/Nack",
  "Device/DeviceList/LeftThighBoard/Error",
  "Device/DeviceList/LeftShinBoard/Ack",
  "Device/DeviceList/LeftShinBoard/Nack",
  "Device/DeviceList/LeftShinBoard/Error",
  "Device/DeviceList/LeftFootBoard/Ack",
  "Device/DeviceList/LeftFootBoard/Nack",
  "Device/DeviceList/LeftFootBoard/Error",
  "Device/DeviceList/RightHipBoard/Ack",
  "Device/DeviceList/RightHipBoard/Nack",
  "Device/DeviceList/RightHipBoard/Error",
  "Device/DeviceList/RightThighBoard/Ack",
  "Device/DeviceList/RightThighBoard/Nack",
  "Device/DeviceList/RightThighBoard/Error",
  "Device/DeviceList/RightShinBoard/Ack",
  "Device/DeviceList/RightShinBoard/Nack",
  "Device/DeviceList/RightShinBoard/Error",
  "Device/DeviceList/RightFootBoard/Ack",
  "Device/DeviceList/RightFootBoard/Nack",
  "Device/DeviceList/RightFootBoard/Error"
};

static const char* lbhActuatorNames[] = {
  "HeadYaw/Position/Actuator/Value", // 0
  "HeadYaw/Hardness/Actuator/Value",
  "HeadPitch/Position/Actuator/Value", // 2
  "HeadPitch/Hardness/Actuator/Value",
  "LShoulderPitch/Position/Actuator/Value", // 4
  "LShoulderPitch/Hardness/Actuator/Value",
  "LShoulderRoll/Position/Actuator/Value", // 6
  "LShoulderRoll/Hardness/Actuator/Value",
  "LElbowYaw/Position/Actuator/Value", // 8
  "LElbowYaw/Hardness/Actuator/Value",
  "LElbowRoll/Position/Actuator/Value", // 10
  "LElbowRoll/Hardness/Actuator/Value",
  "RShoulderPitch/Position/Actuator/Value", // 12
  "RShoulderPitch/Hardness/Actuator/Value",
  "RShoulderRoll/Position/Actuator/Value", // 14
  "RShoulderRoll/Hardness/Actuator/Value",
  "RElbowYaw/Position/Actuator/Value", // 16
  "RElbowYaw/Hardness/Actuator/Value",
  "RElbowRoll/Position/Actuator/Value", // 18
  "RElbowRoll/Hardness/Actuator/Value",
  "LHipYawPitch/Position/Actuator/Value", // 20
  "LHipYawPitch/Hardness/Actuator/Value",
  "LHipRoll/Position/Actuator/Value", // 22
  "LHipRoll/Hardness/Actuator/Value",
  "LHipPitch/Position/Actuator/Value", // 24
  "LHipPitch/Hardness/Actuator/Value",
  "LKneePitch/Position/Actuator/Value", // 26
  "LKneePitch/Hardness/Actuator/Value",
  "LAnklePitch/Position/Actuator/Value", // 28
  "LAnklePitch/Hardness/Actuator/Value",
  "LAnkleRoll/Position/Actuator/Value", // 30
  "LAnkleRoll/Hardness/Actuator/Value",
  "RHipRoll/Position/Actuator/Value", // 32
  "RHipRoll/Hardness/Actuator/Value",
  "RHipPitch/Position/Actuator/Value", // 34
  "RHipPitch/Hardness/Actuator/Value",
  "RKneePitch/Position/Actuator/Value", // 36
  "RKneePitch/Hardness/Actuator/Value",
  "RAnklePitch/Position/Actuator/Value", // 38
  "RAnklePitch/Hardness/Actuator/Value",
  "RAnkleRoll/Position/Actuator/Value", // 40
  "RAnkleRoll/Hardness/Actuator/Value",
  
  "Face/Led/Red/Left/0Deg/Actuator/Value", // 42
  "Face/Led/Red/Left/45Deg/Actuator/Value",
  "Face/Led/Red/Left/90Deg/Actuator/Value", // 44
  "Face/Led/Red/Left/135Deg/Actuator/Value",
  "Face/Led/Red/Left/180Deg/Actuator/Value", // 46
  "Face/Led/Red/Left/225Deg/Actuator/Value",
  "Face/Led/Red/Left/270Deg/Actuator/Value", // 48
  "Face/Led/Red/Left/315Deg/Actuator/Value",
  "Face/Led/Green/Left/0Deg/Actuator/Value", // 50
  "Face/Led/Green/Left/45Deg/Actuator/Value",
  "Face/Led/Green/Left/90Deg/Actuator/Value", // 52
  "Face/Led/Green/Left/135Deg/Actuator/Value",
  "Face/Led/Green/Left/180Deg/Actuator/Value", // 54
  "Face/Led/Green/Left/225Deg/Actuator/Value",
  "Face/Led/Green/Left/270Deg/Actuator/Value", // 56
  "Face/Led/Green/Left/315Deg/Actuator/Value",
  "Face/Led/Blue/Left/0Deg/Actuator/Value", // 58
  "Face/Led/Blue/Left/45Deg/Actuator/Value",
  "Face/Led/Blue/Left/90Deg/Actuator/Value", // 60
  "Face/Led/Blue/Left/135Deg/Actuator/Value",
  "Face/Led/Blue/Left/180Deg/Actuator/Value", // 62
  "Face/Led/Blue/Left/225Deg/Actuator/Value",
  "Face/Led/Blue/Left/270Deg/Actuator/Value", // 64
  "Face/Led/Blue/Left/315Deg/Actuator/Value",
  "Face/Led/Red/Right/0Deg/Actuator/Value", // 66
  "Face/Led/Red/Right/45Deg/Actuator/Value",
  "Face/Led/Red/Right/90Deg/Actuator/Value", // 68
  "Face/Led/Red/Right/135Deg/Actuator/Value",
  "Face/Led/Red/Right/180Deg/Actuator/Value", // 70
  "Face/Led/Red/Right/225Deg/Actuator/Value",
  "Face/Led/Red/Right/270Deg/Actuator/Value", // 72
  "Face/Led/Red/Right/315Deg/Actuator/Value",
  "Face/Led/Green/Right/0Deg/Actuator/Value", // 74
  "Face/Led/Green/Right/45Deg/Actuator/Value",
  "Face/Led/Green/Right/90Deg/Actuator/Value", // 76
  "Face/Led/Green/Right/135Deg/Actuator/Value",
  "Face/Led/Green/Right/180Deg/Actuator/Value", // 78
  "Face/Led/Green/Right/225Deg/Actuator/Value",
  "Face/Led/Green/Right/270Deg/Actuator/Value", // 80
  "Face/Led/Green/Right/315Deg/Actuator/Value",
  "Face/Led/Blue/Right/0Deg/Actuator/Value", // 82
  "Face/Led/Blue/Right/45Deg/Actuator/Value",
  "Face/Led/Blue/Right/90Deg/Actuator/Value", // 84
  "Face/Led/Blue/Right/135Deg/Actuator/Value",
  "Face/Led/Blue/Right/180Deg/Actuator/Value", // 86
  "Face/Led/Blue/Right/225Deg/Actuator/Value",
  "Face/Led/Blue/Right/270Deg/Actuator/Value", // 88
  "Face/Led/Blue/Right/315Deg/Actuator/Value",
  "Ears/Led/Left/36Deg/Actuator/Value", // 90
  "Ears/Led/Left/72Deg/Actuator/Value",
  "Ears/Led/Left/108Deg/Actuator/Value",
  "Ears/Led/Left/144Deg/Actuator/Value",
  "Ears/Led/Left/180Deg/Actuator/Value",
  "Ears/Led/Left/216Deg/Actuator/Value",
  "Ears/Led/Left/252Deg/Actuator/Value",
  "Ears/Led/Left/288Deg/Actuator/Value",
  "Ears/Led/Left/324Deg/Actuator/Value",
  "Ears/Led/Left/0Deg/Actuator/Value",
  "Ears/Led/Right/0Deg/Actuator/Value",
  "Ears/Led/Right/36Deg/Actuator/Value",
  "Ears/Led/Right/72Deg/Actuator/Value",
  "Ears/Led/Right/108Deg/Actuator/Value",
  "Ears/Led/Right/144Deg/Actuator/Value",
  "Ears/Led/Right/180Deg/Actuator/Value",
  "Ears/Led/Right/216Deg/Actuator/Value",
  "Ears/Led/Right/252Deg/Actuator/Value",
  "Ears/Led/Right/288Deg/Actuator/Value",
  "Ears/Led/Right/324Deg/Actuator/Value",
  "ChestBoard/Led/Red/Actuator/Value",
  "ChestBoard/Led/Green/Actuator/Value",
  "ChestBoard/Led/Blue/Actuator/Value",
  "LFoot/Led/Red/Actuator/Value",
  "LFoot/Led/Green/Actuator/Value",
  "LFoot/Led/Blue/Actuator/Value",
  "RFoot/Led/Red/Actuator/Value",
  "RFoot/Led/Green/Actuator/Value",
  "RFoot/Led/Blue/Actuator/Value",
};

static const char* lbhUSActuatorNames[] = {
  "US/Actuator/Value"
};

enum
{
  lbhNumOfSensors = sizeof(lbhSensorNames) / sizeof(*lbhSensorNames),
  lbhNumOfActuators = sizeof(lbhActuatorNames) / sizeof(*lbhActuatorNames),
  lbhNumOfUSActuators = sizeof(lbhUSActuatorNames) / sizeof(*lbhUSActuatorNames),
};

enum BHState
{
  ok = 0,
  abnormalTermination = 1,
  sigINT = 2,
  sigQUIT = 3,
  sigILL = 4,
  sigABRT = 6,
  sigFPE = 8,
  sigKILL = 9,
  sigSEGV = 11,
  sigPIPE = 13,
  sigALRM = 14,
  sigTERM = 15,
};

struct LbhData
{
  volatile int readingSensors; /**< Index of sensor data reserved for reading. */
  volatile int actualSensors; /**< Index of sensor data that is the most actual. */
  volatile int readingActuators; /**< Index of actuator commands reserved for reading. */
  volatile int actualActuators; /**< Index of actuator command that are the most actual. */

  char robotName[24]; /* Device/DeviceList/ChestBoard/BodyNickName */
  float sensors[3][lbhNumOfSensors];
  float actuators[3][lbhNumOfActuators];
  float usActuator[3]; /* US/Actuator/Value */

  BHState state;
  bool release;

  int cognitionTraceCounter;
  char cognitionTrace[LBH_TRACE_MSG_LENGTH*LBH_TRACE_SIZE];
  int motionTraceCounter;
  char motionTrace[LBH_TRACE_MSG_LENGTH*LBH_TRACE_SIZE];
};

#ifdef TARGET_ROBOT
extern LbhData *lbhData; //Main.cpp
#endif

#define lbh_pi_2 1.5707963267948966f
static const double sitDownAngles[21] = { 
    -0.0135177,
  -( 0.043983),
  -( 0.780565-lbh_pi_2),
    -1.42764+lbh_pi_2,
     0.126611,
    -0.25758,
  -( 0.780528-lbh_pi_2),
  -(-1.43436+lbh_pi_2),
  -( 0.135423),
  -(-0.250192),
    -0.00440383,
  -( 0.00831337),
    -1.00943,
     2.17113,
    -1.20265,
  -( 0.00354111),
     0.00174092,
    -1.01021,
     2.17649,
    -1.20632,
    -0.00542875 };

#ifdef __cplusplus
}
#endif

#endif
