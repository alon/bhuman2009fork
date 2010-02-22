/** 
* @file SSLVision.h
* Received data from the small size vision.
* @author Armin Burchardt <armin@informatik.uni-bremen.de>
*/

#ifndef SSLVision_H
#define SSLVision_H

#ifndef WIN32

#include "Tools/Module/Module.h"
#include "Representations/Infrastructure/RobotInfo.h"
#include "Representations/Configuration/RobotDimensions.h"
#include "Representations/Infrastructure/TeamInfo.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/JointData.h"
#include "Representations/Modeling/RobotPose.h"
#include "Representations/Modeling/BallModel.h"
#include "Representations/MotionControl/OdometryData.h"
#include "Representations/Perception/BallPercept.h"
#include "Representations/Sensing/InertiaMatrix.h"

MODULE(SSLVision)
  REQUIRES(FrameInfo)
  REQUIRES(RobotInfo)
  REQUIRES(RobotDimensions)
  REQUIRES(InertiaMatrix)
  REQUIRES(FilteredJointData)
  REQUIRES(OdometryData)
  REQUIRES(OwnTeamInfo)
  PROVIDES_WITH_MODIFY_AND_DRAW(RobotPose)
  PROVIDES_WITH_MODIFY_AND_OUTPUT_AND_DRAW(GroundTruthRobotPose)
  PROVIDES_WITH_MODIFY_AND_DRAW(BallPercept)
  PROVIDES_WITH_MODIFY_AND_DRAW(BallModel)
  PROVIDES_WITH_MODIFY_AND_DRAW(GroundTruthBallModel)
END_MODULE

class UdpSocket;
class SSL_DetectionFrame;

class SSLVision : public SSLVisionBase
{
private:
  void init();
  void update(RobotPose& robotPose);
  void update(GroundTruthRobotPose& groundTruthRobotPose);

  void update(BallPercept& ballPercept);
  void update(BallModel& ballModel);
  void update(GroundTruthBallModel& groundTruthBallModel);

  bool checkNetwork();
  bool processBalls(const SSL_DetectionFrame* frame);
  bool processRobots(const SSL_DetectionFrame* frame);

  UdpSocket* sock;

  RobotPose networkRobotPose;
  BallPercept networkBallPercept;
  unsigned networkOdometryTime;

  RobotPose currentRobotPose;
  Pose2D lastOdometry;
  double robotPoseValidity;
};

#endif


#endif
