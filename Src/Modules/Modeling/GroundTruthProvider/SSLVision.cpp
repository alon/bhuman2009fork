/**
* @file SSLVision.cpp
*/

#ifndef WIN32

#include <iostream>
#include <string>
#include <sstream>
#include <errno.h>
#include "SSLVision.h"

#include <messages_robocup_ssl_geometry.pb.h>
#include <messages_robocup_ssl_wrapper.pb.h>
#include <messages_robocup_ssl_detection.pb.h>

#include "Platform/Win32Linux/UdpSocket.h"
#include "Tools/Team.h"

#include "Representations/Sensing/InertiaMatrix.h"
#include "Tools/Debugging/DebugDrawings3D.h"



void SSLVision::init()
{
  sock = new UdpSocket();
  sock->setBlocking(false);
  if(!sock->bind("224.5.23.2", 10002))
  {
    perror("Bind failed.");
    ASSERT(false);
  }

  if(!sock->joinMulticast("224.5.23.2"))
  {
    std::cout << "could not set multicast address" << std::endl;
  }

  lastOdometry = theOdometryData;
  robotPoseValidity = 0;
}


void SSLVision::update(RobotPose& robotPose)
{
  update((GroundTruthRobotPose&) robotPose);

  TEAM_OUTPUT(idTeamMateRobotPose, bin, robotPose);
}


void SSLVision::update(GroundTruthRobotPose& groundTruthRobotPose)
{
  checkNetwork();

  Pose3D groundToHeadPan = theInertiaMatrix;
  groundToHeadPan.translate(-8., -6., -(85. + 27.)); //pos of inertiaboard
  groundToHeadPan.translate(0., 0., theRobotDimensions.zLegJoint1ToHeadPan);

  Pose3D headPanToMarker;
  headPanToMarker.rotateZ(theFilteredJointData.angles[JointData::headPan]);
  headPanToMarker.rotateY(-theFilteredJointData.angles[JointData::headTilt]);
  headPanToMarker.translate(0.,0.,110.);

  Pose3D markerPose(RotationMatrix(0,0,networkRobotPose.rotation),
                    Vector3<double>(networkRobotPose.translation.x, networkRobotPose.translation.y, 550.));

  Pose3D offsetInRobotCoordinates;
  offsetInRobotCoordinates.conc(groundToHeadPan);
  offsetInRobotCoordinates.conc(headPanToMarker);

  Pose3D offsetInWorldCoordinates;
  const double drot = networkRobotPose.rotation - offsetInRobotCoordinates.rotation.getZAngle();
  offsetInWorldCoordinates.rotateZ(drot);
  offsetInWorldCoordinates.translate(offsetInRobotCoordinates.translation);

  const double dx = markerPose.translation.x - offsetInWorldCoordinates.translation.x;
  const double dy = markerPose.translation.y - offsetInWorldCoordinates.translation.y;

  groundTruthRobotPose.translation.x = dx;
  groundTruthRobotPose.translation.y = dy;
  groundTruthRobotPose.rotation = drot;
  groundTruthRobotPose.validity = robotPoseValidity;
}


void SSLVision::update(BallPercept& ballPercept)
{
  checkNetwork();
  ballPercept = networkBallPercept;
}

void SSLVision::update(BallModel& ballModel)
{
  checkNetwork();
  ballModel.timeWhenLastSeen = theFrameInfo.time;
  ballModel.estimate.position = networkBallPercept.relativePositionOnField;
  ballModel.lastPerception.position = networkBallPercept.relativePositionOnField;
}

void SSLVision::update(GroundTruthBallModel& groundTruthBallModel)
{
  checkNetwork();
  groundTruthBallModel.timeWhenLastSeen = theFrameInfo.time;
  groundTruthBallModel.estimate.position = networkBallPercept.relativePositionOnField;
  groundTruthBallModel.lastPerception.position = networkBallPercept.relativePositionOnField;
}

bool SSLVision::checkNetwork()
{
  const int readBufSize = 1024;
  char readBuf[readBufSize];
  bool updated = false;
  int lastLen = 0;

  for(;;)
  {
    const int len = sock->read(readBuf, readBufSize);
    if(len == -1)
    {
      if(errno == EAGAIN)
        break; // no data with O_NONBLOCK flag
      std::cerr << "read failed" << std::endl;
      return false;
    }
    lastLen = len;
  }


  SSL_WrapperPacket packet;
  packet.ParseFromArray(readBuf, lastLen);
  if(!packet.has_detection())
    return false;

  SSL_DetectionFrame sslFrame;
  sslFrame = packet.detection();

  updated |= processBalls(&sslFrame);
  updated |= processRobots(&sslFrame);

  return updated;
}


bool SSLVision::processBalls(const SSL_DetectionFrame* frame)
{
  ASSERT(frame);
  int balls = frame->balls_size();
  networkBallPercept.ballWasSeen = false;
  for(int i = 0; i < balls; ++i)
  {
    const SSL_DetectionBall& ball = frame->balls(i);

    if(networkBallPercept.ballWasSeen &&
       ball.confidence() < networkBallPercept.validity)
      continue;

    Vector2<double> pos(ball.x(), ball.y());
    DEBUG_RESPONSE("module:SSLVision:rotateField", { pos.rotate(pi); } );
    networkBallPercept.relativePositionOnField = pos - currentRobotPose.translation;
    networkBallPercept.relativePositionOnField.rotate(-currentRobotPose.rotation);
    networkBallPercept.ballWasSeen = true;
    networkBallPercept.validity = ball.confidence();
  }

  if(!networkBallPercept.ballWasSeen)
    networkBallPercept.validity = 0;
  return networkBallPercept.ballWasSeen;
}

bool SSLVision::processRobots(const SSL_DetectionFrame* frame)
{
  ASSERT(frame);
  bool found(false);
  //the TEAM_RED is the yellow team (Aibo color)
  bool teamYellow = theOwnTeamInfo.teamColor == TEAM_RED ? 1 : 0;
  int robotid(theRobotInfo.number);
  MODIFY("module:SSLVision:teamYellow", teamYellow);
  MODIFY("module:SSLVision:robotid", robotid);

  const int num = teamYellow ? frame->robots_yellow_size() : frame->robots_blue_size();

  for(int i = 0; i < num; ++i)
  {
    const SSL_DetectionRobot& robot = teamYellow ? frame->robots_yellow(i) : frame->robots_blue(i);
    if(robotid == (int)robot.robot_id())
    {
      networkRobotPose.translation.x = robot.x();
      networkRobotPose.translation.y = robot.y();
      if(robot.has_orientation())
        networkRobotPose.rotation = robot.orientation();

      DEBUG_RESPONSE("module:SSLVision:rotateField",
                     {
                       networkRobotPose.translation.rotate(pi);
                       networkRobotPose.rotation = normalize(networkRobotPose.rotation+pi);
                     } );

      robotPoseValidity = 1.;
      found = true;
    }
  }

  if(found)
    networkRobotPose.validity = 1.0;
  else
    networkRobotPose.validity *= 0.9;

  return found;
}

MAKE_MODULE(SSLVision, Infrastructure)

#endif
