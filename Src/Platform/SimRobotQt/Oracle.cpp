/**
 * @file Platform/SimRobotQt/Oracle.cpp
 * Implementation of class Oracle for SimRobotQt.
 * @author Max Risler
 * @author Colin Graf
 */

#include <Simulation/SimObject.h>
#include "Oracle.h"
#include "RoboCupCtrl.h"
#include "../SystemCall.h"
#include "../hash_map.h"
#include "Representations/Modeling/RobotPose.h"
#include "Representations/Modeling/BallModel.h"
#include "Representations/MotionControl/OdometryData.h"

USING_HASH_MAP

static const char* translate[] =
{
  "headPan", "HeadYaw",
  "headTilt", "HeadPitch",
  "armRight0", "RShoulderPitch",
  "armRight1", "RShoulderRoll",
  "armRight2", "RElbowYaw",
  "armRight3", "RElbowRoll",
  "armLeft0", "LShoulderPitch",
  "armLeft1", "LShoulderRoll",
  "armLeft2", "LElbowYaw",
  "armLeft3", "LElbowRoll",
  "legRight0", "RHipYawPitch",
  "legRight1", "RHipRoll",
  "legRight2", "RHipPitch",
  "legRight3", "RKneePitch",
  "legRight4", "RAnklePitch",
  "legRight5", "RAnkleRoll",
  "legLeft0", "LHipYawPitch",
  "legLeft1", "LHipRoll",
  "legLeft2", "LHipPitch",
  "legLeft3", "LKneePitch",
  "legLeft4", "LAnklePitch",
  "legLeft5", "LAnkleRoll",
  "accelerometer", "accelerometer",
  "gyroscope", "gyro",
  "sonarCenterRight", "US-TopRight",
  "sonarRight", "US-BottomRight",
  "sonarCenterLeft", "US-TopLeft",
  "sonarLeft", "US-BottomLeft",
  "camera", "camera",
  "rFSR1", "RFsrFL",
  "rFSR2", "RFsrFR",
  "rFSR3", "RFsrBR",
  "rFSR4", "RFsrBL",
  "lFSR1", "LFsrFL",
  "lFSR2", "LFsrFR",
  "lFSR3", "LFsrBR",
  "lFSR4", "LFsrBL",
  "rFSRBump", "RFoot",
  "lFSRBump", "LFoot",
  "rFoot", "RFOOT_COORD_MACRO34",
  "lFoot", "LFOOT_COORD_MACRO45"
};

Oracle::Oracle() :
blue(false), me(0), leftFoot(0), rightFoot(0), ball(0), imageYUV422x2(0), naoV3(false) 
{}

Oracle::~Oracle()
{
  if(imageYUV422x2)
    delete[] imageYUV422x2;
}

void Oracle::init()
{
  ASSERT(me == 0);

  hash_map<std::string, const char*> names;
  for(size_t i = 0; i < sizeof(translate) / sizeof(translate[0]); i += 2)
    names[translate[i]] = translate[i + 1];
  const std::string& robotName(RoboCupCtrl::getRobotFullName());

  // get controller
  VERIFY(ctrl = RoboCupCtrl::getController());
  
  // get the robot
  VERIFY(me = ctrl->getObjectReference(robotName));
  blue = me->getName()[5] < '5';

  me->findObject(rightFoot, "rFoot", false);
  if(!rightFoot)
  {
    naoV3 = true;
    me->findObject(rightFoot, names["rFoot"], false);
  }

  me->findObject(leftFoot, "lFoot", false);
  if(!leftFoot)
    me->findObject(leftFoot, names["lFoot"], false);

  // get the ball
  const SimObject* balls = ctrl->getObjectReference("RoboCup.balls");
  if(balls->getNumberOfChildNodes() > 0)
    ball = balls->getChildNode(0);

  // get joint-ids
  std::vector<std::string> parts;
  parts.resize(2);
  parts[0] = robotName;
  for(int i = 0; i < JointData::numOfJoints; ++i)
  {
    std::string angleName(JointData::getJointName(JointData::Joint(i)));
    parts[1] = angleName;
    spJoints[i] = ctrl->getSensorportId(parts);
    if(spJoints[i] == -1)
    {
      angleName = names[angleName];
      parts[1] = angleName;
      spJoints[i] = ctrl->getSensorportId(parts);
    }
    parts[1] += ".angle";
    apJoints[i] = ctrl->getActuatorportId(parts);
    parts[1] = angleName + ".switch";
    psJoints[i] = ctrl->getActuatorportId(parts);
  }

  // get sensor-ids
  parts.resize(3);
  parts[1] = "camera";
  parts[2] = "image";
  spCamera = ctrl->getSensorportId(parts);
  if(spCamera == -1)
  {
    parts[1] = names["camera"];
    spCamera = ctrl->getSensorportId(parts);
  }
  parts.resize(2);
  parts[1] = "accelerometer";
  spAcc = ctrl->getSensorportId(parts);
  if(spAcc == -1)
  {
    parts[1] = names["accelerometer"];
    spAcc = ctrl->getSensorportId(parts);
  }
  parts[1] = "gyroscope";
  spGyro = ctrl->getSensorportId(parts);
  if(spGyro == -1)
  {
    parts[1] = names["gyroscope"];
    spGyro = ctrl->getSensorportId(parts);
  }
  parts[1] = "sonarLeft";
  spUsLeft = ctrl->getSensorportId(parts);
  if(spUsLeft == -1)
  {
    parts[1] = names["sonarLeft"];
    spUsLeft = ctrl->getSensorportId(parts);
  }
  parts[1] = "sonarRight";
  spUsRight = ctrl->getSensorportId(parts);
  if(spUsRight == -1)
  {
    parts[1] = names["sonarRight"];
    spUsRight = ctrl->getSensorportId(parts);
  }
  parts[1] = "sonarCenterLeft";
  spUsCenterLeft = ctrl->getSensorportId(parts);
  if(spUsCenterLeft == -1)
  {
    parts[1] = names["sonarCenterLeft"];
    spUsCenterLeft = ctrl->getSensorportId(parts);
  }
  parts[1] = "sonarCenterRight";
  spUsCenterRight = ctrl->getSensorportId(parts);
  if(spUsCenterRight == -1)
  {
    parts[1] = names["sonarCenterRight"];
    spUsCenterRight = ctrl->getSensorportId(parts);
  }

  // get FSR-ids
  static const char* fsrs[8] =
  {
    "lFSR1", "lFSR2", "lFSR3", "lFSR4",
    "rFSR1", "rFSR2", "rFSR3", "rFSR4"
  };
  for(int i = 0; i < 8; ++i)
  {
    parts[1] = fsrs[i];
    spFSR[i] = ctrl->getSensorportId(parts);
    if(spFSR[i] == -1)
    {
      parts[1] = names[fsrs[i]];
      spFSR[i] = ctrl->getSensorportId(parts);
    }
  }

  static const char* bumpers[2] = {"lFSRBump", "rFSRBump"};
  for(int i = 0; i < 2; ++i)
  {
    parts[1] = bumpers[i];
    spFSRBump[i] = ctrl->getSensorportId(parts);
    if(spFSRBump[i] == -1)
    {
      parts[1] = names[bumpers[i]];
      spFSRBump[i] = ctrl->getSensorportId(parts);
    }
  }

  parts[1] = "CameraSelect.angle";
  int apCameraSelect = ctrl->getActuatorportId(parts);
  if(apCameraSelect != -1)
    ctrl->setActuatorport(apCameraSelect, fromDegrees(40));

  // load calibration
  InConfigFile stream(Global::getSettings().expandRobotFilename("jointCalibration.cfg"));
  ASSERT(stream.exists());
  stream >> jointCalibration;
}

void Oracle::getRobotPose(RobotPose& robotPose) const
{
  if(rightFoot && leftFoot)
  {
    Pose2D leftPose, rightPose;
    getPose2D(leftFoot, leftPose);
    getPose2D(rightFoot, rightPose);
    getPose2D(me, (Pose2D&)robotPose);
    robotPose.translation = (leftPose.translation + rightPose.translation) / 2.;
    robotPose.translation *= 1000.;
    if(blue)
      robotPose = Pose2D(pi) + robotPose;
  }
  else
  {
    ASSERT(me);
    getPose2D(me, robotPose);
    robotPose.translation *= 1000.;
    if(blue)
      robotPose = Pose2D(pi) + robotPose;
  }
}

void Oracle::getOdometryData(const RobotPose& robotPose, OdometryData& odometryData) const
{
  ASSERT(me);
  (Pose2D&)odometryData = blue ? (Pose2D(pi) + robotPose) : (const Pose2D&)robotPose;
}

void Oracle::getBallModel(const RobotPose& robotPose, BallModel& ballModel)
{
  ASSERT(me);
  if(ball)
  {
    unsigned int timeWhenLastSeen = SystemCall::getCurrentSystemTime();
    Pose2D pose;
    getPose2D(ball, pose);
    pose.translation *= 1000.;
    if(blue)
      pose = Pose2D(pi) + pose;
    Vector2<double> velocity((pose.translation - lastBallPosition) / (timeWhenLastSeen - ballModel.timeWhenLastSeen) * 1000.);
    ballModel.lastPerception.setPositionAndVelocityInFieldCoordinates(pose.translation, velocity, robotPose);
    ballModel.estimate = ballModel.lastPerception;
    ballModel.lastSeenEstimate = ballModel.lastPerception;
    ballModel.timeWhenLastSeen = timeWhenLastSeen;
    lastBallPosition = pose.translation;
  }
}

void Oracle::getImage(Image& image)
{
  ASSERT(me);
  if(spCamera != -1)
  {
    const SensorReading& imageReading = ctrl->getSensorReading(spCamera);
    const int w = imageReading.dimensions[0];
    const int h = imageReading.dimensions[1];
    if(imageYUV422x2 == 0)
      imageYUV422x2 = new unsigned char[w*h*8];
    /** TODO: Convert image here! */

    int w3 = w * 3, w2 = w * 2;
    unsigned char* src = (unsigned char*)imageReading.data.byteArray;
    unsigned char* srcLineEnd = src;
    Image::Pixel* destBegin = (Image::Pixel*)imageYUV422x2;
    Image::Pixel* dest;
    int r, g, b, yy, uu, vv;
    for(int y = 0; y < h; ++y)
    {
      for(srcLineEnd += w3, dest = destBegin + y * w2; src < srcLineEnd; )
      {
        b = *src++;
        g = *src++;
        r = *src++;
        yy =          306 * r,
        uu = 130560 + 512 * r - 429 * g,
        vv = 130560 - 173 * r - 339 * g + 512 * b;
        yy += 601 * g + 117 * b;
        uu -= 83 * b;
        // yy =          306 * r + 601 * g + 117 * b // && 0<=r<=255, 0<=g<=255, 0<=b<=255
        // => max = 261120, min = 0
        // uu = 130048 + 512 * r - 429 * g -  83 * b // && 0<=r<=255, 0<=g<=255, 0<=b<=255
        // => max = 260 608, min = -512
        // vv = 130048 - 173 * r - 339 * g + 512 * b // && 0<=r<=255, 0<=g<=255, 0<=b<=255
        // => max = 260 608, min = -512
        // I suggest using 130560 instead of 130048 to get rid of these tests:
        //if(yy < 0) yy = 0; else if(yy > 261120) yy = 261120;
        //if(uu < 0) uu = 0; else if(uu > 261120) uu = 261120;
        //if(vv < 0) vv = 0; else if(vv > 261120) vv = 261120;
        dest->y = dest->yCbCrPadding = (unsigned char)(yy >> 10);
        dest->cr = (unsigned char)(uu >> 10);
        dest->cb = (unsigned char)(vv >> 10);
        ++dest;
      }
    }
    image.setImage(imageYUV422x2);
  }
  image.timeStamp = SystemCall::getCurrentSystemTime();
}

void Oracle::getAndSetJointData(const JointData& jointRequest, JointData& jointData) const
{
  ASSERT(me);
  for(int i = 0; i < JointData::numOfJoints; ++i)
  {
    // Get angles      
    if(spJoints[i] != -1)
    {
      const SensorReading& actualAngle = ctrl->getSensorReading(spJoints[i]);
      jointData.angles[i] = actualAngle.data.doubleValue * jointCalibration.joints[i].sign - jointCalibration.joints[i].offset;
    }

    // Set angles
    const double& targetAngle(jointRequest.angles[i]);
    if(targetAngle != JointData::off && 
       targetAngle != JointData::ignore &&
       spJoints[i] != -1) // if joint does exist
      ctrl->setActuatorport(apJoints[i], (targetAngle + jointCalibration.joints[i].offset) * jointCalibration.joints[i].sign);
  }
  jointData.timeStamp = SystemCall::getCurrentSystemTime();
}

void Oracle::getSensorData(SensorData& sensorData) const
{
  ASSERT(me);

  // Battery
  sensorData.data[SensorData::batteryLevel] = 1.0f;

  // Acceleration sensors
  const SensorReading& acceleration = ctrl->getSensorReading(spAcc);
  sensorData.data[SensorData::accX] = float(acceleration.data.doubleArray[0] / 9.81);
  sensorData.data[SensorData::accY] = float(acceleration.data.doubleArray[1] / 9.81);
  sensorData.data[SensorData::accZ] = float(acceleration.data.doubleArray[2] / 9.81);

  // Gyro
  const SensorReading& rotationSpeed = ctrl->getSensorReading(spGyro);
  sensorData.data[SensorData::gyroX] = float(rotationSpeed.data.doubleArray[0]);
  sensorData.data[SensorData::gyroY] = float(rotationSpeed.data.doubleArray[1]);
  sensorData.data[SensorData::gyroZ] = 0.; //float(rotationSpeed.data.doubleArray[2]); // nao style
  if(blue)
  {
    sensorData.data[SensorData::gyroX] *= -1.f;
    sensorData.data[SensorData::gyroY] *= -1.f;
  }

  if(spFSR[0] != -1)
  {
    const SensorReading& pressure0 = ctrl->getSensorReading(spFSR[0]);
    sensorData.data[SensorData::fsrLFL] = float(pressure0.data.doubleArray[2]);
    const SensorReading& pressure1 = ctrl->getSensorReading(spFSR[1]);
    sensorData.data[SensorData::fsrLFR] = float(pressure1.data.doubleArray[2]);
    const SensorReading& pressure2 = ctrl->getSensorReading(spFSR[2]);
    sensorData.data[SensorData::fsrLBL] = float(pressure2.data.doubleArray[2]);
    const SensorReading& pressure3 = ctrl->getSensorReading(spFSR[3]);
    sensorData.data[SensorData::fsrLBR] = float(pressure3.data.doubleArray[2]);
    const SensorReading& pressure4 = ctrl->getSensorReading(spFSR[4]);
    sensorData.data[SensorData::fsrRFL] = float(pressure4.data.doubleArray[2]);
    const SensorReading& pressure5 = ctrl->getSensorReading(spFSR[5]);
    sensorData.data[SensorData::fsrRFR] = float(pressure5.data.doubleArray[2]);
    const SensorReading& pressure6 = ctrl->getSensorReading(spFSR[6]);
    sensorData.data[SensorData::fsrRBL] = float(pressure6.data.doubleArray[2]);
    const SensorReading& pressure7 = ctrl->getSensorReading(spFSR[7]);
    sensorData.data[SensorData::fsrRBR] = float(pressure7.data.doubleArray[2]);
  }
  else if(spFSRBump[0] != -1)
  {
    const SensorReading& contactLeft = ctrl->getSensorReading(spFSRBump[0]);
    const SensorReading& contactRight = ctrl->getSensorReading(spFSRBump[1]);
    if(contactLeft.data.boolValue && contactRight.data.boolValue)
      for(int i = 0; i < 8; i++)
        sensorData.data[SensorData::fsrLFL+i] = 0.8f;
    else if(contactLeft.data.boolValue)
      for(int i = 0; i < 4; i++)
      {
        sensorData.data[SensorData::fsrLFL+i] = 1.6f;
        sensorData.data[SensorData::fsrRFL+i] = 0.0f;
      }
    else if(contactRight.data.boolValue)
      for(int i = 0; i < 4; i++)
      {
        sensorData.data[SensorData::fsrLFL+i] = 0.0f;
        sensorData.data[SensorData::fsrRFL+i] = 1.6f;
      }
    else
      for(int i = 0; i < 8; i++)
        sensorData.data[SensorData::fsrLFL+i] = 0.0f; 
  }

  // ultrasonic (model approximation. not absolutly correct in reality)
  static const double scale = 1000; //meter to millimeter
  if(sensorData.usSensorType == SensorData::left)
  {
    const SensorReading& usDistance = ctrl->getSensorReading(spUsCenterLeft);
    sensorData.data[SensorData::us] = float(usDistance.data.doubleValue * scale);
    sensorData.usSensorType = SensorData::leftToRight;
  }
  else if(sensorData.usSensorType == SensorData::leftToRight)
  {
    const SensorReading& usDistance = ctrl->getSensorReading(spUsRight);
    sensorData.data[SensorData::us] = float(usDistance.data.doubleValue * scale);
    sensorData.usSensorType = SensorData::right;
  }
  else if(sensorData.usSensorType == SensorData::rightToLeft)
  {
    const SensorReading& usDistance = ctrl->getSensorReading(spUsLeft);
    sensorData.data[SensorData::us] = float(usDistance.data.doubleValue * scale);
    sensorData.usSensorType = SensorData::left;
  }
  else if(sensorData.usSensorType == SensorData::right)
  {
    const SensorReading& usDistance = ctrl->getSensorReading(spUsCenterRight);
    sensorData.data[SensorData::us] = float(usDistance.data.doubleValue * scale);
    sensorData.usSensorType = SensorData::rightToLeft;
  }

  // angle
  const Matrix3d& world2robot(me->getRotation());
  sensorData.data[SensorData::angleX] = float(atan2(world2robot.col[0].v[2], world2robot.col[2].v[2]));
  sensorData.data[SensorData::angleY] = float(atan2(world2robot.col[1].v[2], world2robot.col[2].v[2]));

  sensorData.timeStamp = SystemCall::getCurrentSystemTime();
}

SimObject* Oracle::getRobot() const
{
  return me;
}

SimObject* Oracle::getBall() const
{
  return ball;
}

void Oracle::getPose2D(const SimObject* obj, Pose2D& pose2D) const
{
  pose2D.rotation = normalize(obj->getRotation().getZAngle() - (naoV3 ? 0 : pi_2));
  const Vector3d& position(obj->getPosition());
  pose2D.translation.x = position.v[0];
  pose2D.translation.y = position.v[1];
}
