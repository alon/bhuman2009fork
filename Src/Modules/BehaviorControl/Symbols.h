/** 
* @file Symbols.h
*
* Declaration of class Symbols.
*
* @author Max Risler
*/

#ifndef __Symbols_h_
#define __Symbols_h_

#include "Tools/Xabsl/XabslEngine/XabslEngine.h"

/**
* The base class for all classes which define Xabsl symbols for the BehaviorControl module.
*
* @author Max Risler
*/ 
class Symbols
{
public:
  /**
  * Virtual destructor.
  */
  virtual ~Symbols() {}

  /** registers the symbols at an engine */
  virtual void registerSymbols(xabsl::Engine& engine) = 0;
  
  /** updates the symbols */
  virtual void update() {}

  /** initialize the symbols */
  virtual void init() {}
};


#endif // __Symbols_h_

