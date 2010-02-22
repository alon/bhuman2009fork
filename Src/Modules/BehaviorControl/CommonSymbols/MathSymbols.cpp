/** 
* @file MathSymbols.cpp
*
* Implementation of class MathSymbols
*
* @author Max Risler
*/

#include "MathSymbols.h"
#include "Tools/Math/Common.h"

PROCESS_WIDE_STORAGE MathSymbols* MathSymbols::theInstance = 0;

void MathSymbols::registerSymbols(xabsl::Engine& engine)
{
  // "min"
  engine.registerDecimalInputSymbol("min", &getMin);
  engine.registerDecimalInputSymbolDecimalParameter("min", "min.value0", &min0Value);
  engine.registerDecimalInputSymbolDecimalParameter("min", "min.value1", &min1Value);

  // "max"
  engine.registerDecimalInputSymbol("max", &getMax);
  engine.registerDecimalInputSymbolDecimalParameter("max", "max.value0", &max0Value);
  engine.registerDecimalInputSymbolDecimalParameter("max", "max.value1", &max1Value);

  // "abs"
  engine.registerDecimalInputSymbol("abs", &getAbs);
  engine.registerDecimalInputSymbolDecimalParameter("abs", "abs.value", &absValue);

  // "sgn"
  engine.registerDecimalInputSymbol("sgn", &getSgn);
  engine.registerDecimalInputSymbolDecimalParameter("sgn", "sgn.value", &sgnValue);

  // "sin"
  engine.registerDecimalInputSymbol("sin", &getSin);
  engine.registerDecimalInputSymbolDecimalParameter("sin", "sin.angle", &alphaValue);

  // "cos"
  engine.registerDecimalInputSymbol("cos", &getCos);
  engine.registerDecimalInputSymbolDecimalParameter("cos", "cos.angle", &alphaValue);

  // "random"
  engine.registerDecimalInputSymbol("random", &getRandom);

  // "normalize"
  engine.registerDecimalInputSymbol("normalize", &getNormalize);
  engine.registerDecimalInputSymbolDecimalParameter("normalize", "normalize.angle", &normalizeAngle);

  // "clip"
  engine.registerDecimalInputSymbol("clip", &getClip);
  engine.registerDecimalInputSymbolDecimalParameter("clip", "clip.value", &clipValue);
  engine.registerDecimalInputSymbolDecimalParameter("clip", "clip.min", &clipMin);
  engine.registerDecimalInputSymbolDecimalParameter("clip", "clip.max", &clipMax);

  // "exclude"
  engine.registerDecimalInputSymbol("exclude", &getExclude);
  engine.registerDecimalInputSymbolDecimalParameter("exclude", "exclude.value", &excludeValue);
  engine.registerDecimalInputSymbolDecimalParameter("exclude", "exclude.min", &excludeMin);
  engine.registerDecimalInputSymbolDecimalParameter("exclude", "exclude.max", &excludeMax);

  // "vector.abs" (deprecated)
  engine.registerDecimalInputSymbol("vector.abs", &getDistance);
  engine.registerDecimalInputSymbolDecimalParameter("vector.abs", "vector.abs.x", &distanceX);
  engine.registerDecimalInputSymbolDecimalParameter("vector.abs", "vector.abs.y", &distanceY);

  // "distance"
  engine.registerDecimalInputSymbol("distance", &getDistance);
  engine.registerDecimalInputSymbolDecimalParameter("distance", "distance.x", &distanceX);
  engine.registerDecimalInputSymbolDecimalParameter("distance", "distance.y", &distanceY);

  // "distance"
  engine.registerDecimalInputSymbol("distance_a_to_b", &getDistance);
  engine.registerDecimalInputSymbolDecimalParameter("distance_a_to_b", "distance_a_to_b.x1", &distanceX);
  engine.registerDecimalInputSymbolDecimalParameter("distance_a_to_b", "distance_a_to_b.y1", &distanceY);
  engine.registerDecimalInputSymbolDecimalParameter("distance_a_to_b", "distance_a_to_b.x2", &distanceX2);
  engine.registerDecimalInputSymbolDecimalParameter("distance_a_to_b", "distance_a_to_b.y2", &distanceY2);

  // "atan2" (your friend)
  engine.registerDecimalInputSymbol("atan2", getAtan2);
  engine.registerDecimalInputSymbolDecimalParameter("atan2", "atan2.y", &atan2Y);
  engine.registerDecimalInputSymbolDecimalParameter("atan2", "atan2.x", &atan2X);
}

double MathSymbols::getSgn()
{
  return theInstance->sgnValue > 0 ? 1 : -1;
}

double MathSymbols::getAbs()
{
  return fabs(theInstance->absValue);
}

double MathSymbols::getMin()
{
  if (theInstance->min0Value < theInstance->min1Value)
    return theInstance->min0Value;
  return theInstance->min1Value;
}

double MathSymbols::getMax()
{
  if (theInstance->max0Value > theInstance->max1Value)
    return theInstance->max0Value;
  return theInstance->max1Value;
}

double MathSymbols::getSin()
{
  return sin(fromDegrees(theInstance->alphaValue));
}

double MathSymbols::getCos()
{
  return cos(fromDegrees(theInstance->alphaValue));
}

double MathSymbols::getRandom()
{
  return ((double) rand() / (RAND_MAX+1.0));
}

double MathSymbols::getNormalize()
{
  return toDegrees(normalize(fromDegrees(theInstance->normalizeAngle)));
}

double MathSymbols::getClip()
{
  if (theInstance->clipValue < theInstance->clipMin) return theInstance->clipMin;
  if (theInstance->clipValue > theInstance->clipMax) return theInstance->clipMax;
  return theInstance->clipValue;
}

double MathSymbols::getExclude()
{
  if(theInstance->excludeValue > theInstance->excludeMin && theInstance->excludeValue < theInstance->excludeMax)
    return (theInstance->excludeMax - theInstance->excludeValue) <= (theInstance->excludeValue - theInstance->excludeMin) ? theInstance->excludeMax : theInstance->excludeMin;
  return theInstance->excludeValue;
}

double MathSymbols::getDistance()
{
  return sqrt(sqr(theInstance->distanceX)+sqr(theInstance->distanceY));
}

double MathSymbols::getAtan2()
{
  return toDegrees(atan2(theInstance->atan2Y,theInstance->atan2X));
}

double MathSymbols::getDistanceAB()
{
   return sqrt(sqr(theInstance->distanceX - theInstance->distanceX2) + sqr(theInstance->distanceY - theInstance->distanceY2));
}

