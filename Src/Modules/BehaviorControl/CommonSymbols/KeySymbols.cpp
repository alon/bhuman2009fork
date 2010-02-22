/** 
* @file KeySymbols.cpp
*
* Implementation of class KeySymbols.
*
* @author Judith Müller
*/

#include "KeySymbols.h"

PROCESS_WIDE_STORAGE KeySymbols* KeySymbols::theInstance = 0;

void KeySymbols::registerSymbols(xabsl::Engine& engine)
{
  engine.registerBooleanInputSymbol("key.right_foot_right_button_pressed_and_released", &pressed_and_released[KeyStates::rightFootRight]);
  engine.registerBooleanInputSymbol("key.left_foot_right_button_pressed_and_released", &pressed_and_released[KeyStates::leftFootRight]);
  engine.registerBooleanInputSymbol("key.left_foot_left_button_pressed_and_released", &pressed_and_released[KeyStates::leftFootLeft]);
  engine.registerBooleanInputSymbol("key.right_foot_left_button_pressed_and_released", &pressed_and_released[KeyStates::rightFootLeft]);
  engine.registerBooleanInputSymbol("key.chest_button_pressed_and_released", &pressed_and_released[KeyStates::chest]);
}

void KeySymbols::update()
{
  for (int i=0; i < KeyStates::numberOfKeys; i++)
  {
      if (!keyStates.pressed[i]) last_not_pressed_time[i] = frameInfo.time;
      else if (frameInfo.getTimeSince(last_not_pressed_time[i]) > 200) last_pressed_time[i] = frameInfo.time;
      if (frameInfo.getTimeSince(last_pressed_time[i]) > 0 && frameInfo.getTimeSince(last_pressed_time[i]) < 400) pressed_and_released[i] = true;
      else pressed_and_released[i] = false;
  }
}
