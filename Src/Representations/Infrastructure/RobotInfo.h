/**
* @file RobotInfo.h
* The file declares a class that encapsulates the structure RobotInfo defined in
* the file RoboCupGameControlData.h that is provided with the GameController.
* It also maps the robot's name on the robot's model.
*
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas R�fer</a>
* @author <a href="mailto:aschreck@informatik.uni-bremen.de">Andr&eacute; Schreck</a>
*/ 

#ifndef __RobotInfo_h_
#define __RobotInfo_h_

#include <cstring>
#include "RoboCupGameControlData.h"
#include "Tools/Streams/Streamable.h"

class RobotInfo : public RoboCup::RobotInfo, public Streamable  
{
private:
  /**
  * The method makes the object streamable.
  * @param in The stream from which the object is read (if in != 0).
  * @param out The stream to which the object is written (if out != 0).
  */
  virtual void serialize(In* in, Out* out)
  {  
    STREAM_REGISTER_BEGIN();
    STREAM(number); // robot number: 1..11 
    STREAM(penalty); // PENALTY_NONE, PENALTY_BALL_HOLDING, ...
    STREAM(secsTillUnpenalised); // estimate of time till unpenalised.
    STREAM_ENUM(robotModel, numberOfRobotModels, getRobotModelName);
    STREAM_REGISTER_FINISH();
  }

public:
  /**
  * Default constructor.
  */
  RobotInfo() : number(0)
  {
    memset((RoboCup::RobotInfo*) this, 0, sizeof(RoboCup::RobotInfo));
  }

  enum RobotModel
  {
    nao,
    numberOfRobotModels
  };
  RobotModel robotModel;

  int number; /**< The number of the robot. */

  static RobotModel getRobotModelByRobotName(const char* robotName)
  {
    return nao;
  }


  static const char* getRobotModelName(RobotModel model)
  {
    switch(model)
    {
      case nao: return "nao";
      default: return "unknown";
    }
  }
};

#endif //__RobotInfo_h_
