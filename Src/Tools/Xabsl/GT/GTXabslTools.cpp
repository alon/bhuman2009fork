/**
* @file GTXabslTools.cpp
* 
* Xabsl related helper functions for GT
*
* @author Uwe Düffert
* @author Max Risler
*/

#include <cstring>
#include <string>
#include "GTXabslTools.h"

void getXabslString(char* dstXabsl, const char* src)
{
  unsigned int i=0;
  unsigned int j=0;
  char lastChar='A';
  int charsSinceLastMinus=0;
  while (i<strlen(src))
  {
    if (((lastChar<'A')||(lastChar>'Z'))&&(src[i]>='A')&&(src[i]<='Z'))
    {
      lastChar=src[i]|0x20; 
      if (charsSinceLastMinus > 1)
      {
        dstXabsl[j++]='_';
        charsSinceLastMinus=0;
      }
      dstXabsl[j++]=src[i++]|0x20;
      dstXabsl[j]=0;
      charsSinceLastMinus++;
    }
    else
    {
      lastChar=src[i];
      dstXabsl[j++]=src[i++];
      dstXabsl[j]=0;
      charsSinceLastMinus++;
    }
  }
}
