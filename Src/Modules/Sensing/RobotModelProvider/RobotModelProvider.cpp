/**
* @file RobotModelProvider.cpp
*
* This file implements a module that provides information about the current state of the robot's limbs.
*
* @author <a href="mailto:allli@informatik.uni-bremen.de">Alexander Härtl</a>
*/

#include "RobotModelProvider.h"

void RobotModelProvider::update(RobotModel &robotModel)
{
  robotModel.setJointData(theFilteredJointData, theRobotDimensions, theMassCalibration);
}


MAKE_MODULE(RobotModelProvider, Sensing)
