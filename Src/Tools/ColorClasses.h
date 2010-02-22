/**
* @file ColorClasses.h
* Declaration of class ColorClasses
*
* @author <A href="mailto:Tim.Laue@dfki.de">Tim Laue</A>
*/

#ifndef __ColorClasses_h__
#define __ColorClasses_h__


/**
* @class ColorClasses
* 
* Static class for color class functions.
*
* @author <A href="mailto:Tim.Laue@dfki.de">Tim Laue</A>
*/
class ColorClasses
{
public:
  enum Color
  {
    none,                   /*<! all other objects */
    orange,                 /*<! ball */
    yellow,                 /*<! yellow goal */
    blue,                   /*<! blue goal */
    white,                  /*<! lines */
    green,                  /*<! field */
    black,                  /*<! most probably: nothing */
    red,                    /*<! color of red robots> */
    robotBlue,              /*<! color of blue robots> */
    numOfColors             /*<! number of colors */
  };

  /**
  * Returns the name of a color class
  * @param color The color class
  * @return The name
  */
  static const char* getColorName(Color color)
  {
    switch(color) 
    {
    case none: return "none";
    case orange: return "orange"; 
    case yellow: return "yellow"; 
    case blue: return "blue";
    case green: return "green"; 
    case white: return "white";
    case black: return "black";
    case red: return "red";
    case robotBlue: return "robotBlue";
    default: return "unknown color!"; 
    };
  }
  
  /**
  * The method returns prototypical color values for a color class for visualization.
  * @param colorClass The color class.
  * @param y The Y channel of the prototypical color.
  * @param cr The Cr channel of the prototypical color.
  * @param cb The Cb channel of the prototypical color.
  */
  static inline void getColorClassColor(ColorClasses::Color colorClass, unsigned char& y, unsigned char& cr, unsigned char& cb)
  {
    switch(colorClass)
    {
    case ColorClasses::white:
      y = 255;
      cr = 127;
      cb = 127;
      break;
    case ColorClasses::green:
      y = 180;
      cr = 0;
      cb = 0;
      break;
    case ColorClasses::orange:
      y = 164;
      cr = 255;
      cb = 0;
      break;
    case ColorClasses::yellow:
      y = 255;
      cr = 170;
      cb = 0;
      break;
    case ColorClasses::blue:
      y = 60;
      cr = 80;
      cb = 255;
      break;
    case ColorClasses::black:
      y = 0;
      cr = 127;
      cb = 127;
      break;
    case ColorClasses::red:
      y = 105;
      cr = 234;
      cb = 127;
      break;
    case ColorClasses::robotBlue:
      y = 161;
      cr = 14;
      cb = 166;
      break;
    default:
      y = 70;
      cr = 127;
      cb = 127;
    }
  }
};

#endif //__ColorClasses_h_
