/** 
* @file BH2009GoalSymbols.h
*
* Declaration of class BH2009GoalSymbols.
*
* @author Tim Laue
*/

#ifndef __BH2009GoalSymbols_h_
#define __BH2009GoalSymbols_h_

#include "../../Symbols.h"
#include "Representations/Perception/GoalPercept.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Tools/Math/Common.h"

/**
* The Xabsl symbols that are defined in "goal_symbols.xabsl"
*/ 
class BH2009GoalSymbols : public Symbols
{
public:
  /*
  * Constructor.
  * @param goalPosition A reference to the GoalPosition.
  */
  BH2009GoalSymbols(const GoalPercept& goalPercept, const FrameInfo& frameInfo) : 
    frameInfo(frameInfo),
    goalPercept(goalPercept) 
  {
    theInstance = this;
  }

  PROCESS_WIDE_STORAGE_STATIC BH2009GoalSymbols* theInstance; /**< Points to the only instance of this class in this process or is 0 if there is none. */

  /** A reference to the FrameInfo */
  const FrameInfo& frameInfo;
   
  /** A reference to the GoalModel */
  const GoalPercept& goalPercept;

  /** registers the symbols at an engine */
  void registerSymbols(xabsl::Engine& engine);

  static bool goalInFrontOfMe();
  static bool goalInFrontOfMeSafe();
  static double getCenterOfFreePart();
  static double getAngleToLeftEnd();
  static double getAngleToRightEnd();
  static double getFreePartSize();

  void update();


private:
  double timeSinceOppGoalWasSeen;
  double timeSinceOwnGoalWasSeen;
  double timeSinceAnyGoalWasSeen;
  bool goalWasSeen;
};

#endif // __GoalSymbols_h_

