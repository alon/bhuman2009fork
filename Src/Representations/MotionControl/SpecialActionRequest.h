/**
* @file Representations/MotionControl/SpecialActionRequest.h
* This file declares a class to represent special action requests.
* @author <A href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</A>
*/

#ifndef __SpecialActionRequest_H__
#define __SpecialActionRequest_H__

#include "Tools/Streams/Streamable.h"
#include "Tools/Settings.h"
#include "Representations/Infrastructure/RobotInfo.h"
#include "Representations/Configuration/RobotName.h"

/**
* @class SpecialActionRequest
* The class represents special action requests.
*/
class SpecialActionRequest : public Streamable
{
private:
  virtual void serialize(In* in, Out* out)
  {  
    STREAM_REGISTER_BEGIN();
    STREAM_ENUM(specialAction, numOfSpecialActions, getSpecialActionName);
    STREAM(mirror);
    STREAM_REGISTER_FINISH();
  }

public:
  /** ids for all special actions */
  enum SpecialActionID
  {

    playDead,

    standUpBackNao,
    standUpFrontNao,

    kickLeftNao,
    kickNaoSimulator,


    demoJesus, // all joints 0°
    demoUpright, // upright as defined in the rules
    demoHardness,
    
    numOfSpecialActions
  };

  SpecialActionID specialAction; /**< The special action selected. */
  bool mirror; /**< Mirror left and right. */

  /**
  * Default constructor.
  */
  SpecialActionRequest() : specialAction(playDead), mirror(false) {}

  void specialize(RobotInfo::RobotModel model, RobotName robotName)
  {
#ifdef TARGET_SIM
    if(specialAction == kickLeftNao)
      specialAction = kickNaoSimulator;

#endif
  }

  /** 
  * The function returns names for special action ids.
  * @param id The special action id the name of which is returned.
  * @return The corresponding name.
  */
  static const char* getSpecialActionName(SpecialActionID id)
  {
    switch (id)
    {
    case playDead: return "playDead";

    case standUpBackNao: return "standUpBackNao";
    case standUpFrontNao: return "standUpFrontNao";

    case kickLeftNao: return "kickLeftNao";
    case kickNaoSimulator: return "kickNaoSimulator";

    case demoJesus: return "demoJesus";
    case demoUpright: return "demoUpright";
    case demoHardness: return "demoHardness";

    default: return "unknown";
    }
  }

  /**
  * The function searches the id for a special action name.
  * @param name The name of the special action.
  * @return The corresponding id if found, or numOfSpecialActions if not found.
  */
  static SpecialActionID getSpecialActionFromName(const char* name)
  {
    for(int i = 0; i < numOfSpecialActions; ++i)
      if(!strcmp(name, getSpecialActionName(SpecialActionID(i))))
        return SpecialActionID(i);
    return numOfSpecialActions;
  }
};

#endif // __SpecialActionRequest_H__
