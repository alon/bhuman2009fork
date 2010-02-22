#ifndef __PointExplorer_h_
#define __PointExplorer_h_

#include "Representations/Configuration/ColorTable64.h"
#include "Representations/Infrastructure/Image.h"
#include "Tools/Debugging/DebugDrawings.h"

/**
 * @class PointExplorer
 *
 * The PointExplorer is a class which runs through the image. It has several
 * function which run on scanlines throught the image. They start from a given
 * point and run until the color changes or until they find a certain color.
 * Additionally there's a function (explorePoint) which not simply runs along
 * a scanline but "explores" a scanline(segment). It runs throught the image
 * and additionally to the normal run functions it also takes the space between
 * the actual scanline and the last into account.
 */
class PointExplorer
{
public:
  /**
   * Initialize the PointExplorer for a frame. Pass the ColorTable, the Image and some parameters.
   * @param image theImage Representation of the frame
   * @param colorTable theColorTable Representation
   * @param exploreStepSize the distance in pixels between the explore scanlines
   * @param gridStepSize the distance in pixels between (normal) scanlines
   * @param skipOffset the amount of pixels allowed to skip in a run
   * @param minSegLength a array giving holding the minimum segment size for each color
   */
  void initFrame(const Image *image, const ColorTable64* colorTable, int exploreStepSize, int gridStepSize, int skipOffset, int* minSegLength);

  /**
   * Run down from (x,y) unless there is a run of skipOffset pixels with color != col.
   * @param x x-coordiante to start from
   * @param y y-coordinate to start from
   * @param col the color of the run
   * @param yEnd the maximal y-coordinate, a run will end when it reaches yEnd
   * @param draw the PenStyle used for the DebugDrawings
   * @return the first pixel (y-coordinate) after the run (col(x,returny) != col || returny == yEnd)
   */
  int runDown(int x, int y, ColorClasses::Color col, int yEnd, Drawings::PenStyle draw=Drawings::ps_null);

  /**
   * Run down from (x,y) unless there is a run of skipOffset pixels with the color col.
   * @param x x-coordiante to start from
   * @param y y-coordinate to start from
   * @param col the color to find
   * @param yEnd the maximal y-coordinate, a run will end when it reaches yEnd
   * @param draw the PenStyle used for the DebugDrawings
   * @return the first pixel (y-coordinate) followed by skipOffset pixels with the color col or yEnd
   */
  int findDown(int x, int y, ColorClasses::Color col, int yEnd, Drawings::PenStyle draw=Drawings::ps_null);

  /**
   * Run down from (x,y) unless there is a run of skipOffset pixels with the color col1 or col2.
   * @param x x-coordiante to start from
   * @param y y-coordinate to start from
   * @param col1 the color to find
   * @param col2 the color to find
   * @param yEnd the maximal y-coordinate, a run will end when it reaches yEnd
   * @param foundCol returns the color of the first skipOffset pixels with the color col1 or col2 => foundColor in {col1, col2}
   * @param draw the PenStyle used for the DebugDrawings
   * @return the first pixel (y-coordinate) followed by skipOffset pixels with the color col or yEnd
   */
  int findDown2(int x, int y, ColorClasses::Color col1, ColorClasses::Color col2, int yEnd, ColorClasses::Color& foundCol, Drawings::PenStyle draw=Drawings::ps_null);

  /**
   * Run up from (x,y) unless there is a run of skipOffset pixels with color != col.
   * @param x x-coordiante to start from
   * @param y y-coordinate to start from
   * @param col the color of the run
   * @param yEnd the minimal y-coordinate, a run will end when it reaches yEnd
   * @param draw the PenStyle used for the DebugDrawings
   * @return the first pixel (y-coordinate) before the run (col(x,returny) != col || returny == yEnd)
   */
  int runUp(int x, int y, ColorClasses::Color col, int yEnd, Drawings::PenStyle draw=Drawings::ps_null);

  /**
   * Run to the right from (x,y) unless there is a run of skipOffset pixels with color != col.
   * @param x x-coordiante to start from
   * @param y y-coordinate to start from
   * @param col the color of the run
   * @param xMax the maximal x-coordinate, a run will end when it reaches xMax
   * @param draw the PenStyle used for the DebugDrawings
   * @return the first pixel (x-coordinate) before the run (col(returnx,y) != col || returnx == xMax)
   */
  int runRight(int x, int y, ColorClasses::Color col, int xMax, Drawings::PenStyle draw=Drawings::ps_null);

  /**
   * Explore a pixel: if col == green and force_detailed == false this is a normal runDown call
   *                  and additionally the size and explored_min_y (==y) and explored_max_y (==runEnd)
   *                  are returned.
   *                  Otherwise it will do a runDown and afterwards look from x to xMin(last scanline)
   *                  whether there are runs connected to the "runDown-run" which start before or end
   *                  after the "runDown-run". These y-coordinates are stored in explored_min_y and
   *                  explored_max_y.
   *                  The size returned is the length of the run multiplied by gridStepSize.
   * @param x x-coordiante to start from
   * @param y y-coordinate to start from
   * @param col the color of the run
   * @param xMin the x-coordinate of the last scanline (exploration is done from x to xMin)
   * @param yEnd the maximal y-coordinate, a run will end when it reaches yEnd
   * @param yMin the minimal y-coordinate (exploration will stop there)
   * @param runEnd returns the y-coordinate of the end of the run
   * @param explored_min_y returns the minimal y-coordinate of all the runs (including the explored ones)
   * @param explored_max_y returns the maximal y-coordinate of all the runs (including the explored ones)
   * @param force_detailed explore green pixels/runs ?
   * @return the size of the run (length * gridStepSize)
   */
  int explorePoint(int x, int y, ColorClasses::Color col, int xMin, int yEnd, int yMin, int& runEnd, int& explored_min_y, int& explored_max_y, bool force_detailed=false);

  /**
   * Returns the ColorClass of the pixel (x,y) in the image.
   * @param x x-coordinate
   * @param y y-coordinate
   * @return the color of the pixel (x,y)
   */
  ColorClasses::Color getColor(int x, int y);

  const Image* theImage; /**< a pointer to the image Representation */
  const ColorTable64* theColorTable64; /**< a pointer to the colortable Representation */

  /**
   * @class Parameters
   * Internal parameters for the PointExplorer.
   */
  class Parameters
  {
    public:
      int exploreStepSize; /**< distance between explore scanlines */
      int gridStepSize; /**< distance between two scanlines */
      int skipOffset; /**< the amount of pixels allowed to skip in a run */
      int* minSegSize; /**< the minimum length of a run for each color */
  };
  Parameters parameters;

};

#endif
