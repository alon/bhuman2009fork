/** 
* @file Platform/SimRobotQt/LocalRobot.cpp
*
* Implementation of LocalRobot.
*
* @author <A href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</A>
* @author <A href="mailto:kspiess@tzi.de">Kai Spiess</A>
*/

#include "LocalRobot.h"
#include <ControllerQt/ConsoleRoboCupCtrl.h>
#include "Platform/itoa.h"

LocalRobot::LocalRobot()
: RobotConsole(theDebugReceiver,theDebugSender),
theDebugReceiver(this,"Receiver.MessageQueue.O",false),
theDebugSender(this,"Sender.MessageQueue.S",false),
imageTimeStamp(0),
imageLastTimeStampSent(0)
{
  mode = ConsoleRoboCupCtrl::getMode();
  addViews();

  if(mode == SystemCall::logfileReplay)
  {
    logFile = ConsoleRoboCupCtrl::getLogFile();
    logPlayer.open(logFile.c_str());
    logPlayer.play();
  }
  else if(mode == SystemCall::simulatedRobot)
  {
    oracle.init();
  }
}

int LocalRobot::main() 
{
  // Only one thread can access *this now.
  SYNC;

  if(mode == SystemCall::simulatedRobot)
  {
    if(jointLastTimeStampSent != jointData.timeStamp)
    {
      debugOut.out.bin << 'm';
      debugOut.out.finishMessage(idProcessBegin);
      debugOut.out.bin << jointData;
      debugOut.out.finishMessage(idJointData);
      debugOut.out.bin << sensorData;
      debugOut.out.finishMessage(idSensorData);
      debugOut.out.bin << odometryData;
      debugOut.out.finishMessage(idOdometryData);
      debugOut.out.bin << 'm';
      debugOut.out.finishMessage(idProcessFinished);
      jointLastTimeStampSent = jointData.timeStamp;
    }

    if(imageLastTimeStampSent != image.timeStamp)
    {
      debugOut.out.bin << 'c';
      debugOut.out.finishMessage(idProcessBegin);
      debugOut.out.bin << image;
      debugOut.out.finishMessage(idImage);
      debugOut.out.bin << robotPose;
      debugOut.out.finishMessage(idGroundTruthRobotPose);
      debugOut.out.bin << ballModel;
      debugOut.out.finishMessage(idGroundTruthBallModel);
      debugOut.out.bin << 'c';
      debugOut.out.finishMessage(idProcessFinished);
      imageLastTimeStampSent = image.timeStamp;
    }
  }
  else if(mode == SystemCall::physicalRobot && imageLastTimeStampSent != image.timeStamp)
  {
    debugOut.out.bin << 'c';
    debugOut.out.finishMessage(idProcessBegin);
    debugOut.out.bin << image;
    debugOut.out.finishMessage(idImage);
    debugOut.out.bin << 'c';
    debugOut.out.finishMessage(idProcessFinished);
    imageLastTimeStampSent = image.timeStamp;
  }

  theDebugSender.send();
  return -20;
}

void LocalRobot::update()
{
  RobotConsole::update();

  // Only one thread can access *this now.
  SYNC;

  if(mode == SystemCall::logfileReplay)
  {
    if(logAcknowledged)
    {
      if(logPlayer.replay())
        logAcknowledged = false;
    }
  }
  else if(mode == SystemCall::physicalRobot)
  {
    int duration = SystemCall::getTimeSince(imageTimeStamp);
    if(duration >= 33)
    {
      imageTimeStamp = duration >= 67 ? SystemCall::getCurrentSystemTime() : imageTimeStamp + 33;
      image.timeStamp = SystemCall::getCurrentSystemTime();
    }
  }
  else if(mode == SystemCall::simulatedRobot)
  {
    if(moveOp != noMove)
    {
      if(moveOp == moveBoth)
        ctrl->moveObject(oracle.getRobot(), movePos * 0.001, moveRot * (pi / 180));
      else if(moveOp == movePosition)
        ctrl->moveObject(oracle.getRobot(), movePos * 0.001);
      else if(moveOp == moveBallPosition)
        ctrl->moveObject(oracle.getBall(), movePos * 0.001);
      moveOp = noMove;
    }

    const int imageDelay = 33;
    int duration = SystemCall::getTimeSince(imageTimeStamp);
    if(duration >= imageDelay)
    {
      imageTimeStamp = duration >= 2 * imageDelay ? SystemCall::getCurrentSystemTime() : imageTimeStamp + imageDelay;
      if(calculateImage) // If there is a PDA camera
        oracle.getImage(image);
      else
        image.timeStamp = SystemCall::getCurrentSystemTime();
      oracle.getRobotPose(robotPose);
      oracle.getBallModel(robotPose, ballModel);
    }
    else
      oracle.getRobotPose(robotPose);
    oracle.getOdometryData(robotPose, odometryData);
    oracle.getSensorData(sensorData);
    oracle.getAndSetJointData(jointRequest, jointData);    
  }

  std::string statusText;
  if(mode == SystemCall::logfileReplay)
  {
    statusText = std::string("replaying ") + logFile + " ";
    if(logPlayer.currentFrameNumber != -1)
    {
      char buf[10];
      statusText += itoa_s(logPlayer.currentFrameNumber + 1, buf, sizeof(buf), 10);
    }
    else
      statusText += "finished";
  }

  if(mode != SystemCall::logfileReplay && logPlayer.numberOfFrames != 0)
  {
    if(statusText != "")
      statusText += ", ";
    char buf[10];
    statusText += std::string("recorded ");
    statusText += itoa_s(logPlayer.numberOfFrames, buf, sizeof(buf), 10);
  }

  if(pollingFor)
  {
    statusText += statusText != "" ? ", polling for " : "polling for ";
    statusText += pollingFor;
  }

  if(!statusText.empty())
  {
    statusText = robotName.substr(robotName.find_last_of(".") + 1) + ": " + statusText;
    ctrl->printStatusText(statusText);
  }
}

MAKE_PROCESS(LocalRobot);
