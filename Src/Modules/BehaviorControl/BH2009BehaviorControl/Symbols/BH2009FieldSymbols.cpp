/** 
* @file BH2009FieldSymbols.cpp
*
* Implementation of class BH2009FieldSymbols.
*/

#include "BH2009FieldSymbols.h"


void BH2009FieldSymbols::registerSymbols(xabsl::Engine& engine)
{
  engine.registerDecimalInputSymbol("field.ball_radius", &ballRadius);
  engine.registerDecimalInputSymbol("field.own_goal.x", &xPosOwnGoal);
  engine.registerDecimalInputSymbol("field.own_ground_line.x", &xPosOwnGroundline);
  engine.registerDecimalInputSymbol("field.opponent_ground_line.x", &xPosOpponentGroundline);
  engine.registerDecimalInputSymbol("field.left_sideline.y", &yPosLeftSideline);
  engine.registerDecimalInputSymbol("field.right_sideline.y", &yPosRightSideline); 
  engine.registerDecimalInputSymbol("field.left_field_border.y", &yPosLeftFieldBorder);
  engine.registerDecimalInputSymbol("field.right_field_border.y", &yPosRightFieldBorder); 
  engine.registerDecimalInputSymbol("field.center_circle_radius", &centerCircleRadius);
  engine.registerDecimalInputSymbol("field.own_penalty_area.x", &xPosOwnPenaltyArea);
  engine.registerDecimalInputSymbol("field.opponent_penalty_area.x", &xPosOpponentPenaltyArea);
  engine.registerDecimalInputSymbol("field.left_penalty_area.y", &yPosLeftPenaltyArea);
  engine.registerDecimalInputSymbol("field.right_penalty_area.y", &yPosRightPenaltyArea);
}
