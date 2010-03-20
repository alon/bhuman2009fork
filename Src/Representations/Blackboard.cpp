/** 
* @file Blackboard.cpp
* Implementation of a class representing the blackboard.
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
*/

#include "Blackboard.h"
#include <string.h>
#include <stdlib.h>

Blackboard::Blackboard() :
  // Initialize all representations by themselves:
  // Infrastructure
  theJointData(theJointData),
  theJointRequest(theJointRequest),
  theSensorData(theSensorData),
  theKeyStates(theKeyStates),
  theLEDRequest(theLEDRequest),
  theImage(theImage),
  theCameraInfo(theCameraInfo),
  theFrameInfo(theFrameInfo),
  theCognitionFrameInfo(theCognitionFrameInfo),
  theRobotInfo(theRobotInfo),
  theOwnTeamInfo(theOwnTeamInfo),
  theOpponentTeamInfo(theOpponentTeamInfo),
  theGameInfo(theGameInfo),
  theTeamStatus(theTeamStatus),
  theSoundRequest(theSoundRequest),
  theSoundOutput(theSoundOutput),
  theTeamMateData(theTeamMateData),
  theMotionRobotHealth(theMotionRobotHealth),
  theRobotHealth(theRobotHealth),
  theBoardInfo(theBoardInfo),

  // Configuration
  theRobotName(theRobotName),
  theColorTable64(theColorTable64),
  theCameraSettings(theCameraSettings),
  theFieldDimensions(theFieldDimensions),
  theRobotDimensions(theRobotDimensions),
  theJointCalibration(theJointCalibration),
  theSensorCalibration(theSensorCalibration),
  theCameraCalibration(theCameraCalibration),
  theBehaviorConfiguration(theBehaviorConfiguration),
  theMassCalibration(theMassCalibration),

  // Perception
  theCameraMatrix(theCameraMatrix),
  theRobotCameraMatrix(theRobotCameraMatrix),
  theInertiaMatrix(theInertiaMatrix),
  theImageCoordinateSystem(theImageCoordinateSystem),
  theBallSpots(theBallSpots),
  theLineSpots(theLineSpots),
  theBallPercept(theBallPercept),
  theExtendedBallPercepts(theExtendedBallPercepts),
  theLinePercept(theLinePercept),
  theRegionPercept(theRegionPercept),
  theFilteredJointData(theFilteredJointData),
  theFilteredSensorData(theFilteredSensorData),
  theGoalPercept(theGoalPercept),
  theGroundContactState(theGroundContactState),

  // Modeling
  theFallDownState(theFallDownState),
  theBallModel(theBallModel),
  theGroundTruthBallModel(theGroundTruthBallModel),
  theObstacleModel(theObstacleModel),
  theRobotPose(theRobotPose),
  theGroundTruthRobotPose(theGroundTruthRobotPose),
  theRobotPoseHypotheses(theRobotPoseHypotheses),
  theSelfLocatorSampleSet(theSelfLocatorSampleSet),
  theBallLocatorSampleSet(theBallLocatorSampleSet),
  theRobotModel(theRobotModel),
  theBodyContour(theBodyContour),

  // BehaviorControl
  theBehaviorControlOutput(theBehaviorControlOutput),

  // MotionControl
  theOdometryData(theOdometryData),
  theGroundTruthOdometryData(theGroundTruthOdometryData),
  theMotionRequest(theMotionRequest),
  theHeadMotionRequest(theHeadMotionRequest),
  theHeadJointRequest(theHeadJointRequest),
  theMotionSelection(theMotionSelection),
  theSpecialActionsOutput(theSpecialActionsOutput),
  theWalkingEngineOutput(theWalkingEngineOutput),
  theWalkingEngineStandOutput(theWalkingEngineStandOutput),
  theMotionInfo(theMotionInfo),
  theUnstableJointRequest(theUnstableJointRequest),
  theSensorPrediction(theSensorPrediction)
{
}

void Blackboard::operator=(const Blackboard& other)
{
  memcpy(this, &other, sizeof(Blackboard)); 
}

void* Blackboard::operator new(size_t size)
{
  return calloc(1, size);
}

void Blackboard::operator delete(void* p)
{
  return free(p);
}

void Blackboard::distract()
{
}

PROCESS_WIDE_STORAGE Blackboard* Blackboard::theInstance = 0;
