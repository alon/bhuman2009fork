/** 
* \file BH2009RoleSymbols.h
* Collection of symbols for dynamic role choosing.
* \author Colin Graf
*/

#ifndef __BH2009RoleSymbols_h_
#define __BH2009RoleSymbols_h_

#include "../../Symbols.h"
#include "Representations/Infrastructure/RobotInfo.h"
#include "Representations/Infrastructure/TeamInfo.h"
#include "Representations/Infrastructure/TeamMateData.h"
#include "Representations/Modeling/BallModel.h"
#include "BH2009BallSymbols.h"
#include "BH2009LocatorSymbols.h"
#include "BH2009GoalSymbols.h"

/**
* \class BH2009RoleSymbols
* The Xabsl symbols that are defined in "role_symbols.xabsl"
* \author Colin Graf
*/ 
class BH2009RoleSymbols : public Symbols
{
public:
  /*
  * Constructor.
  * \param robotInfo A reference to the RobotInfo.
  */
  BH2009RoleSymbols(BehaviorData& behaviorData, const RobotInfo& robotInfo, const BallModel& ballModel) : 
      behaviorData(behaviorData),
      robotInfo(robotInfo),
      ballModel(ballModel),
      nativeRole(BehaviorData::undefined),
      isPenaltyStriker(false),
      isPenaltyKeeper(false)
  {
    theInstance = this;
  }

  PROCESS_WIDE_STORAGE_STATIC BH2009RoleSymbols* theInstance;

  /** registers the symbols at an engine */
  void registerSymbols(xabsl::Engine& engine);

  /** updates the symbols */
  void update();

  /** initialize the symbols */
  void init();

private:
  BehaviorData& behaviorData;
  const RobotInfo& robotInfo;
  const BallModel& ballModel;

  BehaviorData::Role nativeRole; /**< The role defined by player number. */

  // Flags for striker
  bool isPenaltyStriker; /**< A flag to determine whether this striker is in the penalty shootout or not */

  // Flags for Keeper
  bool isPenaltyKeeper; /**< A flag to determine whether this keeper is in the penalty shootout or not */

  int getRobotModel();
};


#endif // __BH2009RoleSymbols_h_

