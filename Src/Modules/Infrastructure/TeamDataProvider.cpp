/** 
* @file TeamDataProvider.cpp
* This file implements a module that provides the data received by team communication.
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
*/

#include "TeamDataProvider.h"
#include "Tools/Settings.h"
#include "Tools/Team.h"
#include "Tools/Debugging/ReleaseOptions.h"

PROCESS_WIDE_STORAGE TeamDataProvider* TeamDataProvider::theInstance = 0;

TeamDataProvider::TeamDataProvider() :
timeStamp(0),
robotNumber(-1),
lastSentTimeStamp(0)
{
  theInstance = this;
}

TeamDataProvider::~TeamDataProvider()
{
  theInstance = 0;
}

void TeamDataProvider::update(TeamMateData& teamMateData)
{
  teamMateData = theTeamMateData;
  teamMateData.numOfConnectedPlayers = 0;
  for(int i = TeamMateData::firstPlayer; i < TeamMateData::numOfPlayers; ++i)
    if(teamMateData.timeStamps[i])
    {
      if(SystemCall::getTimeSince(teamMateData.timeStamps[i]) < 500)
      {
        ++teamMateData.numOfConnectedPlayers;
        if(teamMateData.numOfConnectedPlayers == 1) teamMateData.firstTeamMate = i;
        else if(teamMateData.numOfConnectedPlayers == 2) teamMateData.secondTeamMate = i;
      }
      else
        teamMateData.timeStamps[i] = 0;
    }
  if(teamMateData.numOfConnectedPlayers > 0)
    teamMateData.wasConnected = theTeamMateData.wasConnected = true;
  teamMateData.sendThisFrame = (ntp.doSynchronization(theRobotInfo.number, theFrameInfo.time, Global::getTeamOut()) ||
                             theFrameInfo.getTimeSince(lastSentTimeStamp) >= 100) && 
                             theRobotInfo.penalty == PENALTY_NONE && // theGameInfo.state != STATE_INITIAL &&
                             !(theMotionRequest.motion == MotionRequest::specialAction && theMotionRequest.specialActionRequest.specialAction == SpecialActionRequest::playDead) &&
                             !(theMotionInfo.executedMotionRequest.motion == MotionRequest::specialAction && theMotionInfo.executedMotionRequest.specialActionRequest.specialAction == SpecialActionRequest::playDead) &&
                             theGroundContactState.contact;
  if(teamMateData.sendThisFrame)
    lastSentTimeStamp = theFrameInfo.time;
}

void TeamDataProvider::update(GroundTruthRobotPose& groundTruthRobotPose)
{
  groundTruthRobotPose = theGroundTruthRobotPose;
}

void TeamDataProvider::handleMessages(MessageQueue& teamReceiver)
{
  if(theInstance)
  {
    teamReceiver.handleAllMessages(*theInstance);
    TEAM_OUTPUT(idRobot, bin, theInstance->theRobotInfo.number);
  }

  teamReceiver.clear();
}

bool TeamDataProvider::handleMessage(InMessage& message)
{
  switch(message.getMessageID())
  {
  case idReceiveTimeStamp:
    message.bin >> timeStamp;
    return true;

  case idRobot:
    message.bin >> robotNumber;
    ntp.setReceiveTimeStamp(robotNumber, timeStamp);
    if(robotNumber != theRobotInfo.number) 
      if(robotNumber >= TeamMateData::firstPlayer && robotNumber < TeamMateData::numOfPlayers)
        theTeamMateData.timeStamps[robotNumber] = timeStamp;
    return true;

  case idSendTimeStamp:
    {
      unsigned timeStamp;
      message.bin >> timeStamp;
      ntp.setSendTimeStamp(robotNumber, timeStamp);
      return true;
    }

  case idReleaseOptions:
    message.bin >> Global::getReleaseOptions();
    return true;

  case idTeamMateBallModel:
    if(robotNumber != theRobotInfo.number) 
      if(robotNumber >= TeamMateData::firstPlayer && robotNumber < TeamMateData::numOfPlayers)
      {
        BallModel& ballModel = theTeamMateData.ballModels[robotNumber];
        message.bin >> ballModel;

        if(ballModel.timeWhenLastSeen)
          ballModel.timeWhenLastSeen = ntp.getTeamMateTimeInOwnTime(ballModel.timeWhenLastSeen, robotNumber);
      }
    return true;

  case idTeamMateRobotPose:
    if(robotNumber != theRobotInfo.number) 
      if(robotNumber >= TeamMateData::firstPlayer && robotNumber < TeamMateData::numOfPlayers)
        message.bin >> theTeamMateData.robotPoses[robotNumber];
    return true;

  case idTeamMateBehaviorData:
    if(robotNumber != theRobotInfo.number) 
      if(robotNumber >= TeamMateData::firstPlayer && robotNumber < TeamMateData::numOfPlayers)
        message.bin >> theTeamMateData.behaviorData[robotNumber];
    return true;

  case idGroundTruthBallModel:
    {
      Vector2<double> position;
      message.bin >> theGroundTruthBallModel.timeWhenLastSeen >> position;
      theGroundTruthBallModel.timeWhenLastSeen = ntp.getTeamMateTimeInOwnTime(theGroundTruthBallModel.timeWhenLastSeen, 0);
      if(theOwnTeamInfo.teamColor == TEAM_BLUE)
        position *= -1;
      theGroundTruthBallModel.lastPerception.setPositionAndVelocityInFieldCoordinates(position, Vector2<double>(), 
                                                                                      theGroundTruthRobotPose);
      theGroundTruthBallModel.estimate = theGroundTruthBallModel.lastPerception;
    }
    return true;

  case idGroundTruthRobotPose:
    {
      char teamColor,
           id;
      unsigned timeStamp;
      Pose2D robotPose;
      message.bin >> teamColor >> id >> timeStamp >> robotPose;
      if(teamColor == (int)theOwnTeamInfo.teamColor && id == theRobotInfo.number)
      {
        if(theOwnTeamInfo.teamColor == TEAM_BLUE)
          robotPose = Pose2D(pi) + robotPose;
        (Pose2D&) theGroundTruthRobotPose = robotPose;
      }
    }
    return true;

  default:
    return ntp.handleMessage(message); 
  }
}

MAKE_MODULE(TeamDataProvider, Infrastructure)
