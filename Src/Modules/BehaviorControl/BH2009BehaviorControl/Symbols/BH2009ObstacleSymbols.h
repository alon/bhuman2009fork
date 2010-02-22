/** 
* @file BH2009ObstacleSymbols.h
*
* Declaration of class BH2009ObstacleSymbols.
*
* @author <a href="mailto:Tim.Laue@dfki.de">Tim Laue</a>
* @author <a href="mailto:rieskamp@informatik.uni-bremen.de">Andrik Rieskamp</a>
* @author <a href="mailto:aschreck@informatik.uni-bremen.de">André Schreck</a>
*/

#ifndef __BH2009ObstacleSymbols_h_
#define __BH2009ObstacleSymbols_h_

#include "../../Symbols.h"
#include "Representations/Modeling/ObstacleModel.h"


/**
* @class BH2009ObstacleSymbols
* Symbols for collision avoidence with ultrasound sensors:
*/
class BH2009ObstacleSymbols : public Symbols
{
public:
  /** Constructor */
  BH2009ObstacleSymbols(const ObstacleModel& obstacleModel):
    obstacleModel(obstacleModel)
  {}

  /** Registers the symbols at an engine */
  void registerSymbols(xabsl::Engine& engine);

  /** Updates some of the symbols */
  void update();

private:
  double distanceToClosestObstacle;      /**< As the name says... */
  double distanceToClosestObstacleLeft;  /**< As the name says... */
  double distanceToClosestObstacleRight; /**< As the name says... */
  const ObstacleModel& obstacleModel; /**< Reference to ObstacleModel which contains obstacle information */
};

#endif // __BH2009ObstacleSymbols_h_
