/**
* \file TeamMateData.h
* Declaration of a class representing information about the teammates.
* \author Colin Graf
*/ 

#ifndef __TeamMateData_h_
#define __TeamMateData_h_

#include "Representations/Modeling/RobotPose.h"
#include "Representations/Modeling/BallModel.h"
#include "Representations/BehaviorControl/BehaviorData.h"
#include <sstream>

/**
* \class TeamMateData
* A class representing information about the teammates.
*/ 
class TeamMateData : public Streamable  
{
private:
  virtual void serialize(In* in, Out* out)
  {  
    STREAM_REGISTER_BEGIN();
    STREAM(numOfConnectedPlayers);
    STREAM(sendThisFrame);
    STREAM(wasConnected);
    STREAM_ARRAY(timeStamps);
    STREAM_ARRAY(ballModels);
    STREAM_ARRAY(robotPoses);
    STREAM_ARRAY(behaviorData);
    STREAM_REGISTER_FINISH();
  }

public:
  enum Player
  {
    firstPlayer=1,
    player1=1,
    player2,
    player3,
    player4,
    numOfPlayers
  };
  unsigned numOfConnectedPlayers; /**< The number of robots of which messages were received recently. _Not_ including this robot itself. */
  unsigned firstTeamMate; /** player number of first team mate */
  unsigned secondTeamMate; /** player number of second team mate */
  bool sendThisFrame; /**< The team communication will be sent in this frame. */
  bool wasConnected; /**< Whether we have been connected to a team mate. */
  unsigned int timeStamps[numOfPlayers]; /**< The times when messages from different robots arrived. */
  BallModel ballModels[numOfPlayers]; /**< The last received ball model of each team mate. */
  RobotPose robotPoses[numOfPlayers]; /**< The last received robot pose of each team mate. */
  BehaviorData behaviorData[numOfPlayers]; /**< The last received behavior data of each team mate. */

  /**
  * Default constructor.
  */
  TeamMateData() : 
    numOfConnectedPlayers(0), 
    sendThisFrame(false),
    wasConnected(false)
  {
    for(int i = 0; i < numOfPlayers; ++i)
      timeStamps[i] = 0;
  }

  /** drawing function for representation*/
  void draw()
  {
    DECLARE_DEBUG_DRAWING("representation:TeamMateData", "drawingOnField");
    for(int i=1; i<numOfPlayers; ++i)
    {
      if(timeStamps[i])
      {
        Vector2<double>& rPos = robotPoses[i].translation;
        Vector2<double> dirPos = robotPoses[i] * Vector2<double>(200,0);
        std::stringstream numStream;
        numStream << i;
        CIRCLE("representation:TeamMateData", rPos.x, rPos.y, 200, 20, Drawings::ps_solid, 
          ColorRGBA(255,0,0), Drawings::bs_null, ColorClasses::white);
        LINE("representation:TeamMateData", rPos.x, rPos.y, dirPos.x, dirPos.y, 20, 
          Drawings::ps_solid, ColorRGBA(255,0,0));
        DRAWTEXT("representation:TeamMateData", rPos.x, rPos.y, 150, ColorRGBA(255,0,0), numStream.str().c_str());
        DRAWTEXT("representation:TeamMateData", rPos.x + 200, rPos.y - 200, 150, 
          ColorRGBA(255,0,0), BehaviorData::getRoleName(behaviorData[i].role));
        Vector2<double> bPos = ballModels[i].estimate.getPositionInFieldCoordinates(robotPoses[i]);
        CIRCLE("representation:TeamMateData", bPos.x, bPos.y, 50, 20, Drawings::ps_solid, 
          ColorRGBA(255,0,0), Drawings::bs_solid, ColorRGBA(255,0,0));
        LINE("representation:TeamMateData", rPos.x, rPos.y, bPos.x, bPos.y, 20, Drawings::ps_dash, ColorRGBA(255,0,0));
      }
    }
  }
};

#endif //__TeamMateData_h_
