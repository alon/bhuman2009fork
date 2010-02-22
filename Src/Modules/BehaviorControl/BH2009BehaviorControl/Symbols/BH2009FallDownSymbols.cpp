/** 
* @file BH2009FallDownSymbols.cpp
*
* Implementation of class FallDownSymbols.
*
* @author Judith Müller
*/

#include "BH2009FallDownSymbols.h"
#include "Tools/Xabsl/GT/GTXabslTools.h"
#include <stdio.h>

void BH2009FallDownSymbols::registerSymbols(xabsl::Engine& engine)
{
  // The FallDownState
  char s[256];
  engine.registerEnumeratedInputSymbol("fall_down_state","fall_down_state", (int*)&state.state);
  for (int i=0;i<FallDownState::numOfStates;i++)
  {
    sprintf(s, "fall_down_state.");
    getXabslString(s + strlen(s),FallDownState::getStateName((FallDownState::State)i));
    engine.registerEnumElement("fall_down_state",s,i);
  }
}
