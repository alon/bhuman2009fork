/** 
* @file BH2009ObstacleSymbols.cpp
*
* Implementation of class BH2009ObstacleSymbols.
*
* @author <a href="mailto:Tim.Laue@dfki.de">Tim Laue</a>
* @author <a href="mailto:rieskamp@informatik.uni-bremen.de">Andrik Rieskamp</a>
* @author <a href="mailto:aschreck@informatik.uni-bremen.de">André Schreck</a>
*/

#include "BH2009ObstacleSymbols.h"


void BH2009ObstacleSymbols::registerSymbols(xabsl::Engine& engine)
{
  engine.registerDecimalInputSymbol("obstacle.distance_to_left_obstacle", &obstacleModel.distanceToLeftObstacle);
  engine.registerDecimalInputSymbol("obstacle.distance_to_right_obstacle", &obstacleModel.distanceToRightObstacle);
  engine.registerDecimalInputSymbol("obstacle.distance_to_center_left_obstacle", &obstacleModel.distanceToCenterLeftObstacle);
  engine.registerDecimalInputSymbol("obstacle.distance_to_center_right_obstacle", &obstacleModel.distanceToCenterRightObstacle);
  engine.registerDecimalInputSymbol("obstacle.distance_to_closest_obstacle", &distanceToClosestObstacle);
  engine.registerDecimalInputSymbol("obstacle.distance_to_closest_left", &distanceToClosestObstacleLeft);
  engine.registerDecimalInputSymbol("obstacle.distance_to_closest_right", &distanceToClosestObstacleRight);
}

void BH2009ObstacleSymbols::update()
{
  distanceToClosestObstacleLeft  = std::min(obstacleModel.distanceToLeftObstacle, obstacleModel.distanceToCenterLeftObstacle);
  distanceToClosestObstacleRight = std::min(obstacleModel.distanceToRightObstacle, obstacleModel.distanceToCenterRightObstacle);
  distanceToClosestObstacle = std::min(distanceToClosestObstacleLeft,distanceToClosestObstacleRight);
}
