/** 
* @file CoordinateSystemProvider.h
* This file declares a module that provides coordinate systems.
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
*/

#ifndef __CoordinateSystemProvider_h_
#define __CoordinateSystemProvider_h_

#include "Tools/Module/Module.h"
#include "Representations/Configuration/RobotDimensions.h"
#include "Representations/Infrastructure/Image.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/JointData.h"
#include "Representations/Perception/CameraMatrix.h"
#include "Representations/Perception/ImageCoordinateSystem.h"
#include "Tools/Debugging/DebugImages.h"

MODULE(CoordinateSystemProvider)
  REQUIRES(Image) // for debugging only
  REQUIRES(FrameInfo)
  REQUIRES(FilteredJointData)
  REQUIRES(CameraInfo)
  REQUIRES(RobotDimensions)
  REQUIRES(CameraMatrix)
  PROVIDES_WITH_MODIFY_AND_OUTPUT(ImageCoordinateSystem);
END_MODULE

class CoordinateSystemProvider : public CoordinateSystemProviderBase
{
private:
  void init();

  void update(ImageCoordinateSystem& imageCoordinateSystem);

  /**
  * The method calculates the scaling factors for the distored image.
  * @param a The constant part of the equation for motion distortion will be returned here.
  * @param b The linear part of the equation for motion distortion will be returned here.
  */
  void calcScaleFactors(double& a, double& b) const;

  CameraMatrix prevCameraMatrix;
  unsigned prevTime;
  DECLARE_DEBUG_IMAGE(corrected);
  DECLARE_DEBUG_IMAGE(horizonAligned);

public:
  /**
  * Default constructor.
  */
  CoordinateSystemProvider() : prevTime(0) {}
};

#endif // __CoordinateSystemProvider_h_
