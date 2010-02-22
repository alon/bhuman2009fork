/** 
* @file SoundSymbols.h
*
* Declaration of class SoundSymbols.
*
* @author Judith Müller
*/

#ifndef __SoundSymbols_h_
#define __SoundSymbols_h_

#include "../Symbols.h"
#include "Representations/Infrastructure/SoundRequest.h"

class SoundSymbols : public Symbols
{
public:
  /** Constructor.*/
  SoundSymbols(SoundRequest& soundRequest) :
     soundRequest(soundRequest)
  {}

  SoundRequest& soundRequest;

  /** registers the symbols at an engine */
  void registerSymbols(xabsl::Engine& engine);
};


#endif // __SoundSymbols_h_
