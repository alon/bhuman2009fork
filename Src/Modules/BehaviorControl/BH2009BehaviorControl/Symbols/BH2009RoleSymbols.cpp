/** 
* \file BH2009RoleSymbols.cpp
* Collection of symbols for dynamic role choosing.
* \author Colin Graf
*/

#include "BH2009RoleSymbols.h"
#include "BH2009TeamSymbols.h"
#include "BH2009GoalSymbols.h"
#include "BH2009SoccerSymbols.h"
#include "Tools/Team.h"


PROCESS_WIDE_STORAGE BH2009RoleSymbols* BH2009RoleSymbols::theInstance = 0;

void BH2009RoleSymbols::registerSymbols(xabsl::Engine& engine)
{
  engine.registerDecimalInputSymbol("role.connected_players", &BH2009TeamSymbols::staticGetConnectedPlayers);
  engine.registerBooleanInputSymbol("role.lost_connection", &BH2009TeamSymbols::staticGetLostConnection);
  engine.registerBooleanOutputSymbol("role.is_penalty_striker", &isPenaltyStriker);
  engine.registerBooleanOutputSymbol("role.is_penalty_keeper", &isPenaltyKeeper);

  engine.registerEnumElement("role.role", "role.role.undefined", BehaviorData::undefined); 
  engine.registerEnumElement("role.role", "role.role.keeper", BehaviorData::keeper);
  engine.registerEnumElement("role.role", "role.role.defender", BehaviorData::defender);
  engine.registerEnumElement("role.role", "role.role.supporter", BehaviorData::supporter);
  engine.registerEnumElement("role.role", "role.role.striker", BehaviorData::striker);
 


  engine.registerEnumeratedOutputSymbol("role.role", "role.role", (int*)&behaviorData.role);
  engine.registerEnumeratedInputSymbol("role.native_role", "role.role", (int*)&nativeRole);
}

void BH2009RoleSymbols::init() 
{
  if(robotInfo.number >= BehaviorData::firstRole && robotInfo.number < BehaviorData::numOfRoles)
    behaviorData.role = nativeRole = (BehaviorData::Role)robotInfo.number;
}

void BH2009RoleSymbols::update()
{
  // calculate
  const BallState& estimate = ballModel.estimate;
  Vector2 <double> ballPositionRel = estimate.position;
}
