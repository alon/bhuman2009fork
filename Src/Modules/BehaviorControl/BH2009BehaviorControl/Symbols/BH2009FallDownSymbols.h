/** 
* @file BH2009FallDownSymbols.h
*
* Declaration of class BH2009FallDownSymbols.
*
* @author Judith Müller
*/

#ifndef __BH2009FallDownSymbols_h_
#define __BH2009FallDownSymbols_h_

#include "../../Symbols.h"
#include "Representations/Modeling/FallDownState.h"

/**
* The Xabsl symbols that are defined in "fall_down_symbols.xabsl"
*/ 
class BH2009FallDownSymbols : public Symbols
{
public:
  /*
  * Constructor.
  * @param state A reference to the FallDownState.
  */
  BH2009FallDownSymbols(const FallDownState& state) : 
      state(state) {}

  /** A reference to the FallDownState */
  const FallDownState& state;

  /** Registers the symbols at an engine */
  void registerSymbols(xabsl::Engine& engine);
};


#endif // __FallDownSymbols_h_
