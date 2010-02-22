/** 
* @file MathSymbols.h
*
* Declaration of class MathSymbols.
*
* @author Max Risler
*/

#ifndef __MathSymbols_h_
#define __MathSymbols_h_

#include "../Symbols.h"
#include "Platform/SystemCall.h"

/**
* The Xabsl symbols that are defined in "math_symbols.xabsl"
*
* @author Max Risler
*/ 
class MathSymbols : public Symbols
{
public:
  /*
  * Constructor.
  */
  MathSymbols()
  {
    theInstance = this;
  }

  PROCESS_WIDE_STORAGE_STATIC MathSymbols* theInstance; /**< Points to the only instance of this class in this process or is 0 if there is none. */

  /** registers the symbols at an engine */
  void registerSymbols(xabsl::Engine& engine);
    
  /** The parameter "normalize.angle" of the function "normalize" */
  double normalizeAngle;

  /** calculates the decimal input function "sgn" */
  static double getSgn();

  /** calculates the decimal input function "min" */
  static double getMin();

  /** calculates the decimal input function "max" */
  static double getMax();

  /** calculates the decimal input function "abs" */
  static double getAbs();

  /** calculates the decimal input function "random" */
  static double getRandom();

  /** calculates the decimal input function "sin" */
  static double getSin();

  /** calculates the decimal input function "cos" */
  static double getCos();

  /** calculates the decimal input function "normalize" */
  static double getNormalize();

  /** calculates the decimal input function "clip" */
  static double getClip();

  /** calculates the decimal input function "exclude" */
  static double getExclude();

  /** calculates the decimal input function "distance" */
  static double getDistance();

  /** calculates the decimal input function "distance" */
  static double getDistanceAB();

  /** calculates the decimal input function "atan2" */
  static double getAtan2();

  /** The parameter "sgn.value" of the function "sgn" */
  double sgnValue;

  /** The parameter "abs.value" of the function "abs" */
  double absValue;

  /** The parameter "min0.value" of the function "min" */
  double min0Value;

  /** The parameter "min1.value" of the function "min" */
  double min1Value;

  /** The parameter "max.value0" of the function "max" */
  double max0Value;

  /** The parameter "max.value1" of the function "max" */
  double max1Value;

  /** The parameter "*.alpha" of the function "sin" and "cos" */
  double alphaValue;

  double clipValue;
  double clipMin;
  double clipMax;

  double excludeValue;
  double excludeMin;
  double excludeMax;

  double distanceX;
  double distanceY;
  double distanceX2;
  double distanceY2;

  double atan2Y;
  double atan2X;
};

#endif // __MathSymbols_h_

