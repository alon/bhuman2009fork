/**
* @file Tools/ColorIndex.cpp
*
* Implementation of the conversion functions for the ColorIndex::Color enum
*
* @author <a href="oberlies@sim.tu-darmstadt.de">Tobias Oberlies</a>
*/


#include "ColorIndex.h"

ColorIndex::Color ColorIndex::toColorIndex(ColorClasses::Color cclass)
{
  switch( cclass)
  {
  case ColorClasses::orange: return orange;
  case ColorClasses::yellow: return yellow;
  case ColorClasses::blue: return blueMedium;
  case ColorClasses::green: return greenMedium;
  case ColorClasses::white: return white;
  case ColorClasses::black: return black;
  case ColorClasses::red: return red;
  case ColorClasses::robotBlue: return cyan;
  default: return black;
  }
}

void ColorIndex::toRGB(ColorIndex::Color colour, unsigned char& r, unsigned char& g, unsigned char& b)
{
#define RGB(_rhex,_ghex,_bhex) r=0x##_rhex; g=0x##_ghex; b=0x##_bhex; return

  switch( colour)
  {
  case yellowLight:   RGB(FF,FF,7F);
  case yellow:        RGB(FF,FF,00);
  case yellowMedium:  RGB(DF,DF,00);
  case yellowDark:    RGB(9F,9F,00);
  case orangeLight:   RGB(FF,CF,7F);
  case orange:        RGB(FF,9F,00);
  case orangeMedium:  RGB(DF,8B,00);
  case orangeDark:    RGB(99,78,00);
  case redLight:      RGB(FF,7F,7F);
  case red:           RGB(FF,00,00);
  case redMedium:     RGB(DF,00,00);
  case redDark:       RGB(9F,00,00);
  case pinkLight:     RGB(FF,7F,BF);
  case pink:          RGB(FF,00,7F);
  case pinkMedium:    RGB(DF,00,6F);
  case pinkDark:      RGB(9F,00,4F);
  case magentaLight:  RGB(FF,9F,FF);
  case magenta:       RGB(FF,00,FF);
  case magentaMedium: RGB(DF,00,DF);
  case magentaDark:   RGB(9F,00,9F);
  case violetLight:   RGB(7F,7F,FF);
  case violet:        RGB(97,2F,FF);
  case violetMedium:  RGB(7F,00,FF);
  case violetDark:    RGB(4F,00,9F);
  case blueLight:     RGB(7F,7F,FF);
  case blue:          RGB(2F,2F,FF);
  case blueMedium:    RGB(00,00,FF);
  case blueDark:      RGB(00,00,BF);
  case azureLight:    RGB(8F,C7,FF);
  case azure:         RGB(3F,9F,FF);
  case azureMedium:   RGB(00,77,EF);
  case azureDark:     RGB(00,4F,9F);
  case cyanLight:     RGB(8F,FF,FF);
  case cyan:          RGB(00,FF,FF);
  case cyanMedium:    RGB(00,BF,BF);
  case cyanDark:      RGB(00,7F,7F);
  case tealLight:     RGB(8F,FF,C7);
  case teal:          RGB(00,FF,7F);
  case tealMedium:    RGB(00,CF,67);
  case tealDark:      RGB(00,9F,4F);
  case greenLight:    RGB(7F,FF,7F);
  case green:         RGB(00,FF,00);
  case greenMedium:   RGB(00,CF,00);
  case greenDark:     RGB(00,9F,00);
  case springLight:   RGB(CF,FF,7F);
  case spring:        RGB(9F,FF,00);
  case springMedium:  RGB(8B,DF,00);
  case springDark:    RGB(63,9F,00);

  case white:         RGB(FF,FF,FF);
  case grayLight:     RGB(CF,CF,CF);
  case gray:          RGB(9F,9F,9F);
  case grayMedium:    RGB(7F,7F,7F);
  case grayDark:      RGB(5F,5F,5F);
  default:            RGB(00,00,00);
  }

#undef RGB
}
