/** 
* @file BH2009FieldSymbols.h
*
* Declaration of class BH2009FieldSymbols.
*/

#ifndef __BH2009FieldSymbols_h_
#define __BH2009FieldSymbols_h_

#include "../../Symbols.h"
#include "Representations/Configuration/FieldDimensions.h"


/**
* The Xabsl symbols that are defined in "field_symbols.xabsl"
*/ 
class BH2009FieldSymbols : public Symbols
{
public:
  /* Constructor */
  BH2009FieldSymbols(FieldDimensions const& fieldDimensions) : fieldDimensions(fieldDimensions)
  {}

  /** registers the symbols at the engine */
  void registerSymbols(xabsl::Engine& engine);

  /** init function, called after construction, before first use of any symbol */
  void init()
  {
    ballRadius = (double)fieldDimensions.ballRadius;
    xPosOwnGoal = (double)fieldDimensions.xPosOwnGoal;
    xPosOwnGroundline = (double)fieldDimensions.xPosOwnGroundline;
    xPosOpponentGroundline = (double)fieldDimensions.xPosOpponentGroundline;
    yPosLeftSideline = (double)fieldDimensions.yPosLeftSideline;
    yPosRightSideline = (double)fieldDimensions.yPosRightSideline; 
    yPosLeftFieldBorder = (double)fieldDimensions.yPosLeftFieldBorder;
    yPosRightFieldBorder = (double)fieldDimensions.yPosRightFieldBorder; 
    centerCircleRadius = (double)fieldDimensions.centerCircleRadius;
    xPosOwnPenaltyArea = (double)fieldDimensions.xPosOwnPenaltyArea;
    xPosOpponentPenaltyArea = (double)fieldDimensions.xPosOpponentPenaltyArea;
    yPosLeftPenaltyArea = (double)fieldDimensions.yPosLeftPenaltyArea;
    yPosRightPenaltyArea = (double)fieldDimensions.yPosRightPenaltyArea;
  };

private:
  FieldDimensions const& fieldDimensions;
  double ballRadius;
  double xPosOwnGoal;
  double xPosOwnGroundline;
  double xPosOpponentGroundline;
  double yPosLeftSideline;
  double yPosRightSideline; 
  double yPosLeftFieldBorder;
  double yPosRightFieldBorder; 
  double centerCircleRadius;
  double xPosOwnPenaltyArea;
  double xPosOpponentPenaltyArea;
  double yPosLeftPenaltyArea;
  double yPosRightPenaltyArea;
};


#endif // __BH2009FieldSymbols_h_
