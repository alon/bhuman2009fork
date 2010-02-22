/** 
* @file SoundSymbols.cpp
*
* Implementation of class SoundSymbols.
*
* @author Judith Müller
*/

#include "SoundSymbols.h"
#include "Tools/Xabsl/GT/GTXabslTools.h"
#include <stdio.h>

void SoundSymbols::registerSymbols(xabsl::Engine& engine)
{
   // "sound-request"
  char s[256];
  for (int i=0;i<SoundRequest::numOfSounds;i++)
  {
    sprintf(s,"sound.");
    getXabslString(s+strlen(s),SoundRequest::getSoundName((SoundRequest::Sound)i));
    engine.registerEnumElement("sound",s,i);
  }
  engine.registerEnumeratedOutputSymbol("sound","sound",(int*)&soundRequest.sound);
}

