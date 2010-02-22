/** 
* @file CognitionModules.h
* This file includes all modules that are available in the process Cognition.
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
*/

#ifndef __CognitionModules_h_
#define __CognitionModules_h_

#include "Infrastructure/CameraProvider.h"
#include "Infrastructure/CognitionLogDataProvider.h"
#include "Infrastructure/CognitionConfigurationDataProvider.h"
#include "Infrastructure/WhiteCorrection.h"
#include "Infrastructure/TeamDataProvider.h"
#include "Infrastructure/GameDataProvider.h"
#include "Infrastructure/RobotHealthProvider.h"
#include "Infrastructure/SoundControl.h"
#include "Perception/CameraMatrixProvider.h"
#include "Perception/CoordinateSystemProvider.h"
#include "Perception/Regionizer.h"
#include "Perception/RegionAnalyzer.h"
#include "Perception/BallPerceptor.h"
#include "Perception/LinePerceptor.h"
#include "Perception/GoalPerceptor.h"
#include "Perception/BodyContourProvider/BodyContourProvider.h"
#include "Modeling/GroundTruthProvider/SSLVision.h"
#include "Modeling/FallDownStateDetector/FallDownStateDetector.h"
#include "Modeling/ObstacleModelProvider/ObstacleModelProvider.h"
#include "Modeling/ParticleFilterBallLocator/ParticleFilterBallLocator.h"
#include "Modeling/ParticleFilterSelfLocator/SelfLocator.h"
#include "BehaviorControl/BH2009BehaviorControl/BH2009BehaviorControl.h"

#endif // __CognitionModules_h_
