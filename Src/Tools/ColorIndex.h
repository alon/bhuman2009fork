/**
 * @file Tools/ColorIndex.h
 *
 * A list of named colours meant to be used for debug images. Despite the name 
 * DebugGrayScaleImage support 128 gray levels and all colours listed below. 
 * In order to be suitable for marking single (or small numbers of) pixels, all colours 
 * have maximum saturation and are (mostly) distinct.
 *
 * @author <a href="oberlies@sim.tu-darmstadt.de">Tobias Oberlies</a>
 */

#include "ColorClasses.h"

class ColorIndex
{
public:

	/**
	 * List of colours, ordered by hue. For each hue I chose four different brightnes
	 * levels: light, normal, medium (dark), dark. 
	 * Usually, 'normal' is the most saturated colour. The exceptions are azure, blue, 
	 * and violett since it was not possible to have two levels darker than the saturated
	 * colour that are still distict. So 'azureMedium', 'blueMedium', and 'violetMedium' 
	 * are the most saturated  and 'azure', 'blue', and 'violett' are slightly lighter.
	 */
	enum Color
	{
		black = 0,
		white = 1,

		yellow,   yellowLight, yellowMedium, yellowDark,
		orange,   orangeLight, orangeMedium, orangeDark,
		red,      redLight, redMedium, redDark,
		pink,     pinkLight, pinkMedium, pinkDark,
		magenta,  magentaLight, magentaMedium, magentaDark,
		violet,   violetLight, violetMedium, violetDark,
		blue,     blueLight, blueMedium, blueDark,
		azure,    azureLight, azureMedium, azureDark,
		cyan,     cyanLight, cyanMedium, cyanDark,
		teal,     tealLight, tealMedium, tealDark,
		green,    greenLight, greenMedium, greenDark,
		spring,   springLight, springMedium, springDark,
		
		gray,     grayLight, grayMedium, grayDark,

		numOfColors
	};

	/**
	 * Convert colour class to colour index. 
	 */
  static Color toColorIndex(ColorClasses::Color cclass);

	/** 
	 * Returns the RGB representation of the colour.
	 */
	static void toRGB(Color colour, unsigned char& r, unsigned char& g, unsigned char& b);


private:
	/**
	 * Don't construct any instances of this class.
	 */
	ColorIndex();
};

