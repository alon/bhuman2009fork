/** 
* @file MotionModules.h
* This file includes all modules that are available in the process Motion.
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
*/

#ifndef __MotionModules_h_
#define __MotionModules_h_

#include "Infrastructure/NaoProvider.h"
#include "Infrastructure/MotionLogDataProvider.h"
#include "Infrastructure/MotionRobotHealthProvider.h"
#include "Infrastructure/MotionConfigurationDataProvider.h"
#include "Sensing/JointFilter.h"
#include "Sensing/SensorFilter/SensorFilter.h"
#include "Sensing/InertiaMatrixProvider.h"
#include "Sensing/GroundContactDetector.h"
#include "Sensing/RobotModelProvider/RobotModelProvider.h"
#include "MotionControl/MotionSelector.h"
#include "MotionControl/SpecialActions.h"
#include "MotionControl/WalkingEngine.h"
#include "MotionControl/InverseKinematicEngine.h"
#include "MotionControl/MotionCombinator.h"
#include "MotionControl/HeadMotionEngine.h"

#endif // __MotionModules_h_
