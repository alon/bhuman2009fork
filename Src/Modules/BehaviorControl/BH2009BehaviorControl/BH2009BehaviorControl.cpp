/**
* @file BH2009BehaviorControl.cpp
* 
* Implementation of class BH2009BehaviorControl.
*
* @author Max Risler
* @author Judith M�ller
*/

// Shared memory includes
#include <cstring>
#include <cstdio>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <iostream>

#include "BH2009BehaviorControl.h"
#include "Symbols/BH2009BallSymbols.h"
#include "Symbols/BH2009FieldSymbols.h"
#include "Symbols/BH2009HeadSymbols.h"
#include "Symbols/BH2009LEDSymbols.h"
#include "../CommonSymbols/MathSymbols.h"
#include "../CommonSymbols/MotionSymbols.h"
#include "Symbols/BH2009GoalSymbols.h"
#include "../CommonSymbols/SoundSymbols.h"
#include "../CommonSymbols/KeySymbols.h"
#include "Symbols/BH2009GameSymbols.h"
#include "Symbols/BH2009LocatorSymbols.h"
#include "Symbols/BH2009RoleSymbols.h"
#include "Symbols/BH2009TeamSymbols.h"
#include "Symbols/BH2009SoccerSymbols.h"
#include "Symbols/BH2009FallDownSymbols.h"
#include "Symbols/BH2009ObstacleSymbols.h"
#include "Tools/Debugging/ReleaseOptions.h"

#include "cognition_shared_mem.h"

PROCESS_WIDE_STORAGE BH2009BehaviorControl* BH2009BehaviorControl::theInstance = 0;

BH2009BehaviorControl::BH2009BehaviorControl()
: GTXabslEngineExecutor("bh09")
{
  theInstance = this;

  // Common Symbols
  symbols.push_back(new MathSymbols());
  symbols.push_back(new MotionSymbols(behaviorControlOutput.motionRequest, theMotionInfo, theWalkingEngineOutput, theRobotInfo, theRobotName, theRobotPose));
  symbols.push_back(new SoundSymbols(behaviorControlOutput.soundRequest));
  symbols.push_back(new KeySymbols(theKeyStates, theFrameInfo));
  symbols.push_back(new BH2009BallSymbols(theRobotInfo, theBallModel, theFrameInfo, theRobotPose, theTeamMateData, theFieldDimensions));
  symbols.push_back(new BH2009FallDownSymbols(theFallDownState));
  symbols.push_back(new BH2009FieldSymbols(theFieldDimensions));
  symbols.push_back(new BH2009GameSymbols(behaviorControlOutput.behaviorData, behaviorControlOutput.robotInfo, behaviorControlOutput.ownTeamInfo, behaviorControlOutput.gameInfo, theFrameInfo, theBallModel, theRobotPose));
  symbols.push_back(new BH2009GoalSymbols(theGoalPercept, theFrameInfo));
  symbols.push_back(new BH2009HeadSymbols(behaviorControlOutput.headMotionRequest, theJointCalibration, theFilteredJointData, theCameraInfo, theCameraMatrix, theRobotPose, theRobotDimensions));
  symbols.push_back(new BH2009LEDSymbols(behaviorControlOutput.ledRequest, theFilteredSensorData, theBallModel, theFrameInfo, theTeamMateData));
  symbols.push_back(new BH2009LocatorSymbols(theRobotPose, behaviorControlOutput.ownTeamInfo, theFrameInfo, theBallModel, theFieldDimensions, theGroundContactState));
  symbols.push_back(new BH2009ObstacleSymbols(theObstacleModel));
  symbols.push_back(new BH2009RoleSymbols(behaviorControlOutput.behaviorData, theRobotInfo, theBallModel));
  symbols.push_back(new BH2009SoccerSymbols(behaviorControlOutput.behaviorData, theGoalPercept, theRobotPose, theFrameInfo, theFieldDimensions, theBallModel));
  symbols.push_back(new BH2009TeamSymbols(theRobotInfo, theRobotPose, theBallModel, theTeamMateData, theFieldDimensions, theFrameInfo, behaviorControlOutput.behaviorData));

  if (0) {
	  GTXabslEngineExecutor::init();
	  ASSERT(pEngine);
	  if (!errorHandler.errorsOccurred)
	    currentAgent = pEngine->getSelectedAgentName();
  }

  initCognitionSharedMemory();
}

void BH2009BehaviorControl::init()
{
  // init symbols
  for(std::list<Symbols*>::iterator i = symbols.begin(); i != symbols.end(); ++i)
  (*i)->init();
}

BH2009BehaviorControl::~BH2009BehaviorControl()
{
  for(std::list<Symbols*>::iterator i = symbols.begin(); i != symbols.end(); ++i)
    delete *i;
}

void BH2009BehaviorControl::registerSymbolsAndBasicBehaviors()
{
  for(std::list<Symbols*>::iterator i = symbols.begin(); i != symbols.end(); ++i)
    (*i)->registerSymbols(*pEngine);
}

void BH2009BehaviorControl::executeIfEngineCouldNotBeCreated()
{
  behaviorControlOutput.motionRequest.motion = MotionRequest::specialAction;
  behaviorControlOutput.motionRequest.specialActionRequest.specialAction = SpecialActionRequest::demoJesus;
}

void BH2009BehaviorControl::printGeneratedMainActionToString(char* buf) const
{
  behaviorControlOutput.motionRequest.printOut(buf);
}

bool BH2009BehaviorControl::handleMessage(InMessage& message)
{
  if (theInstance)
    return theInstance->handleXabslMessage(message);
  else
    return false;
}

void BH2009BehaviorControl::update(BehaviorControlOutput& behaviorControlOutput)
{
  this->behaviorControlOutput.ownTeamInfo = theOwnTeamInfo;
  this->behaviorControlOutput.robotInfo = theRobotInfo;
  this->behaviorControlOutput.gameInfo = theGameInfo;

  // update symbols
  for(std::list<Symbols*>::iterator i = symbols.begin(); i != symbols.end(); ++i)
    (*i)->update();

  // set agent
  if (currentAgent != theBehaviorConfiguration.agent)
  {
    currentAgent = theBehaviorConfiguration.agent;
    if (!setSelectedAgent(currentAgent.c_str()))
    {
      OUTPUT(idText, text, "BH2009BehaviorControl: Unable to enable selected agent \"" << currentAgent << "\".");
    }
  }

  if (0) {
	  // execute the engine
	  executeEngine();
  }

  readWriteCognitionSharedMemory();
  
  behaviorControlOutput = this->behaviorControlOutput;
  TEAM_OUTPUT(idTeamMateBehaviorData, bin, behaviorControlOutput.behaviorData);
}

void BH2009BehaviorControl::initCognitionSharedMemory()
{
	cognition_mem_fd = shm_open(COGNITION_MEM_NAME, O_RDWR, S_IRUSR | S_IWUSR);
	if(cognition_mem_fd == -1) {
	    std::cout << "Cognition Shared Memory Connection: shared memory open failed for " << COGNITION_MEM_NAME << "\n";
	    cognition_data = NULL;
	    return;
	}
	cognition_data = (CognitionSharedMem*)mmap(NULL, sizeof(CognitionSharedMem),
			PROT_READ | PROT_WRITE, MAP_SHARED, cognition_mem_fd, 0);
	if (cognition_data == MAP_FAILED) {
		std::cout << "Cognition Shared Memory Connection: mmap failed\n";
		cognition_data = NULL;
	}
	std::cout << "Cognition Shared Memory Connection: memory mapped successfully\n";
}

void BH2009BehaviorControl::readWriteCognitionSharedMemory()
{
	if (cognition_data == NULL) return;
	readCognitionSharedMemory();
	writeBHumanSharedMemory();
}

void BH2009BehaviorControl::writeBHumanSharedMemory()
{
	Locked(cognition_data->bhuman);
	cognition_data->bhuman.ball_time_when_last_seen = theBallModel.timeWhenLastSeen;
	cognition_data->bhuman.ball_velocity_estimate.x = theBallModel.estimate.velocity.x;
	cognition_data->bhuman.ball_velocity_estimate.y = theBallModel.estimate.velocity.y;
	cognition_data->bhuman.ball_position_estimate.x = theBallModel.estimate.position.x;
	cognition_data->bhuman.ball_position_estimate.y = theBallModel.estimate.position.y;
	cognition_data->bhuman.writes++;
}

void BH2009BehaviorControl::readCognitionSharedMemory()
{
	static int jesus_moves = 0;
	{
		Locked(cognition_data->cognition);
		if (cognition_data->cognition.writes <= cognition_data->cognition.reads) return;
		cognition_data->cognition.reads++;
		if (cognition_data->cognition.startTurn) {
			jesus_moves++;
			behaviorControlOutput.motionRequest.motion = MotionRequest::specialAction;
			behaviorControlOutput.motionRequest.specialActionRequest.specialAction = SpecialActionRequest::demoJesus;
		}
	}
	if (cognition_data->cognition.startTurn) { // do cout out of the lock - might take time?
		std::cout << "Jesus move on " << jesus_moves << "\n";
	}
}

void BH2009BehaviorControl::update(MotionRequest& motionRequest)
{
  motionRequest = theBehaviorControlOutput.motionRequest;
  if(Global::getReleaseOptions().motionRequest)
  {
    TEAM_OUTPUT(idMotionRequest, bin, motionRequest);
  }
}

void BH2009BehaviorControl::update(LEDRequest& ledRequest)
{
  ledRequest = theBehaviorControlOutput.ledRequest;
}

MAKE_MODULE(BH2009BehaviorControl, Behavior Control)
