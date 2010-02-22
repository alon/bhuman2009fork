/** 
* @file WhiteCorrection.cpp
* This file implements a module that can provide white corrected cameraSettings
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
* @author <a href="mailto:ingsie@informatik.uni-bremen.de">Ingo Sieverdingbeck</a>
*/

#ifndef __WhiteCorrection_h_
#define __WhiteCorrection_h_

#include "Tools/Module/Module.h"
#include "Representations/Infrastructure/Image.h"
#include "Representations/Configuration/CameraSettings.h"
#include "Tools/Optimization/HillClimbing.h"

MODULE(WhiteCorrection)
  USES(Image)
  PROVIDES_WITH_MODIFY(CameraSettings)
END_MODULE

class WhiteCorrection : public WhiteCorrectionBase
{
private:
  std::string robot; //to detect robot name change
  CameraSettings* theCameraSettings;
  bool loadedCameraSettings;
  HillClimbing<int, double, 2> optimizer;
  unsigned int timeStampLastWhiteCorrection; /**< The point of time of the last image white correction test*/
  bool optimizationTerminated;
  bool optimizationStarted;
  int redBlue[2];

  void update(CameraSettings& cameraSettings);

  void readIfRequired();
  void readCameraSettings();
  double averageYCbCrDifferenceOfCurrentImage() const;
  void findWhiteCorrectionParameters();
  void resetWhiteCorrection();
  void printColorValues() const;

  

public:
  /**
  * Default constructor.
  */
  WhiteCorrection();

  /**
  * Destructor.
  */
  ~WhiteCorrection();

};

#endif // __WhiteCorrection_h_
