/** 
* @file KeySymbols.h
*
* Declaration of class KeySymbols.
*
* @author Judith Müller
*/

#ifndef __KeySymbols_h_
#define __KeySymbols_h_

#include "../Symbols.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/KeyStates.h"

/**
* The Xabsl symbols that are defined in "switch_symbols.xabsl"
*
* @author Max Risler
* @author Judith Müller
*/ 
class KeySymbols : public Symbols
{
public:
  /*
  * Constructor.
  * @param frameInfo A reference to the FrameInfo
  * @param switchState A reference to the SwitchState.
  */
  KeySymbols(KeyStates const& keyStates, FrameInfo const& frameInfo) : 
    keyStates(keyStates),
    frameInfo(frameInfo)
  {
    theInstance = this;
    for (int i=0; i < KeyStates::numberOfKeys; i++)
    {
      pressed_and_released[i] = false;
      last_pressed_time[i] = 0;
      last_not_pressed_time[i] = 0;
    }
  }

  PROCESS_WIDE_STORAGE_STATIC KeySymbols* theInstance; /**< Points to the only instance of this class in this process or is 0 if there is none. */

  /** A reference to the keyState */
  KeyStates const& keyStates;

  FrameInfo const& frameInfo;
  
  bool pressed_and_released[KeyStates::numberOfKeys];
  unsigned last_pressed_time[KeyStates::numberOfKeys];
  unsigned last_not_pressed_time[KeyStates::numberOfKeys];

  /** registers the symbols at an engine */
  void registerSymbols(xabsl::Engine& engine);
  void update();
};


#endif // __KeySymbols_h_

