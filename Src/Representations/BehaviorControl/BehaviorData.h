/**
* \file BehaviorData.h
* The file declares a class that containts data about the current behavior state.
* \author Colin Graf
*/ 

#ifndef __BehaviorData_h_
#define __BehaviorData_h_

#include "Tools/Streams/Streamable.h"

/**
* \class BehaviorData
* A class that containts data about the current behavior state.
*/ 
class BehaviorData : public Streamable  
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
    STREAM_ENUM(role, numOfRoles, BehaviorData::getRoleName);
    STREAM_ENUM(action, numOfActions, BehaviorData::getActionName);
    STREAM(kickoffInProgress);
    STREAM_REGISTER_FINISH();
  }

public:
  /**
  * Default constructor.
  */
  BehaviorData() : 
      role(undefined), 
      action(unknown),
      kickoffInProgress(false)
  {
  }
  
  enum Role
  {
    undefined,
    firstRole=1,
    keeper=1,
    supporter,
    striker,
    defender,
    numOfRoles
  } role; /**< A dynamically chosen role. */

  /** The function returns the name as a string of a role.
  * \param role The role
  * \return The name of the role
  */
  static const char* getRoleName(Role role)
  {
    static const char* name[] =
    {
      "undefined",
      "keeper",
      "supporter",
      "striker",
      "defender"
    };
    return name[role > 0 && (size_t) role < sizeof(name) / sizeof(*name) ? role : 0];
  };
  
  enum Action
  {
    unknown,
    dribble,
    goToBall,
    searchForBall,
    goToTarget,
    prepareKick,
    kick,
    kickSidewards,
    pass,
    block,
    hold,
    standUp,
    patrol,
    passBeforeGoal,
    kickoff,
    waitForPass,
    preparePass,
    numOfActions
  } action; /**< What is the robot doing in general? */


  /** The function returns the name as a string of a behavior action.
  * \param action The behavior action
  * \return The name of the behavior action
  */
  static const char* getActionName(Action action)
  {
    static const char* name[] =
    {
      "unknown",
      "dribble",
      "goToBall",
      "searchForBall",
      "goToTarget",
      "prepareKick",
      "kick",
      "kickSidewards",
      "pass",
      "block",
      "hold",
      "standUp",
      "patrol",
      "passBeforeGoal",
      "kickoff",
      "waitForPass"
    };
    return name[action > 0 && (size_t) action < sizeof(name) / sizeof(*name) ? action : 0];
  };
  
  bool kickoffInProgress;
};


#endif //__BehaviorData_h_
