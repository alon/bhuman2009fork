/** 
* @file CoordinateSystemProvider.cpp
* This file implements a module that provides a coordinate system in image coordinates
* that is parallel to the ground and compensates for distortions resulting from the
* rolling shutter.
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
*/

#include "CoordinateSystemProvider.h"
#include "Tools/Debugging/DebugDrawings.h"
#include "Tools/Math/Geometry.h"

void CoordinateSystemProvider::init()
{
  ImageCoordinateSystem::initTables(theCameraInfo);
}

void CoordinateSystemProvider::update(ImageCoordinateSystem& imageCoordinateSystem)
{
  Geometry::Line horizon = Geometry::calculateHorizon(theCameraMatrix, theCameraInfo);
  imageCoordinateSystem.origin = horizon.base;
  imageCoordinateSystem.rotation.c[0] = horizon.direction;
  imageCoordinateSystem.rotation.c[1] = Vector2<double>(-horizon.direction.y, horizon.direction.x);
  imageCoordinateSystem.invRotation = imageCoordinateSystem.rotation.transpose();

  RotationMatrix r(theCameraMatrix.rotation.transpose() * prevCameraMatrix.rotation);
  imageCoordinateSystem.offset = Vector2<double>(r.getZAngle(), r.getYAngle());

  calcScaleFactors(imageCoordinateSystem.a, imageCoordinateSystem.b);
  imageCoordinateSystem.offsetInt = Vector2<int>(int(imageCoordinateSystem.offset.x * 1024 + 0.5), 
                                                 int(imageCoordinateSystem.offset.y * 1024 + 0.5));
  imageCoordinateSystem.aInt = int(imageCoordinateSystem.a * 1024 + 0.5);
  imageCoordinateSystem.bInt = int(imageCoordinateSystem.b * 1024 + 0.5);
  imageCoordinateSystem.cameraInfo = theCameraInfo;
  prevCameraMatrix = theCameraMatrix;
  prevTime = theFilteredJointData.timeStamp;

  DECLARE_DEBUG_DRAWING("horizon", "drawingOnImage"); // displays the horizon
  ARROW("horizon",
       imageCoordinateSystem.origin.x,
       imageCoordinateSystem.origin.y,
       imageCoordinateSystem.origin.x + imageCoordinateSystem.rotation.c[0].x * 50,
       imageCoordinateSystem.origin.y + imageCoordinateSystem.rotation.c[0].y * 50,
       1, Drawings::ps_solid, ColorRGBA(255,0,0));
  ARROW("horizon",
       imageCoordinateSystem.origin.x,
       imageCoordinateSystem.origin.y,
       imageCoordinateSystem.origin.x + imageCoordinateSystem.rotation.c[1].x * 50,
       imageCoordinateSystem.origin.y + imageCoordinateSystem.rotation.c[1].y * 50,
       1, Drawings::ps_solid, ColorRGBA(255,0,0));
  GENERATE_DEBUG_IMAGE(corrected,
    INIT_DEBUG_IMAGE_BLACK(corrected);
    int yDest = -imageCoordinateSystem.toCorrectedCenteredNeg(0, 0).y;
    for(int ySrc = 0; ySrc < theImage.cameraInfo.resolutionHeight; ++ySrc)
      for(int yDest2 = -imageCoordinateSystem.toCorrectedCenteredNeg(0, ySrc).y; yDest <= yDest2; ++yDest)
      {
        int xDest = -imageCoordinateSystem.toCorrectedCenteredNeg(0, ySrc).x;
        for(int xSrc = 0; xSrc < theImage.cameraInfo.resolutionWidth; ++xSrc)
        {
          for(int xDest2 = -imageCoordinateSystem.toCorrectedCenteredNeg(xSrc, ySrc).x; xDest <= xDest2; ++xDest)
          {
            DEBUG_IMAGE_SET_PIXEL_YUV(corrected, xDest + int(theCameraInfo.opticalCenter.x + 0.5), 
                                                 yDest + int(theCameraInfo.opticalCenter.y + 0.5),
                                                 theImage.image[ySrc][xSrc].y, 
                                                 theImage.image[ySrc][xSrc].cb,
                                                 theImage.image[ySrc][xSrc].cr);
          }
        }
      }
    SEND_DEBUG_IMAGE(corrected);
  );

  GENERATE_DEBUG_IMAGE(horizonAligned,
    INIT_DEBUG_IMAGE_BLACK(horizonAligned);
    for(int ySrc = 0; ySrc < theImage.cameraInfo.resolutionHeight; ++ySrc)
      for(int xSrc = 0; xSrc < theImage.cameraInfo.resolutionWidth; ++xSrc)
      {
        Vector2<> corrected(imageCoordinateSystem.toCorrected(Vector2<int>(xSrc, ySrc)));
        corrected.x -= theCameraInfo.opticalCenter.x;
        corrected.y -= theCameraInfo.opticalCenter.y;
        const Vector2<>& horizonAligned(imageCoordinateSystem.toHorizonAligned(corrected));

        DEBUG_IMAGE_SET_PIXEL_YUV(horizonAligned, int(horizonAligned.x + theCameraInfo.opticalCenter.x + 0.5), 
                                             int(horizonAligned.y + theCameraInfo.opticalCenter.y + 0.5),
                                             theImage.image[ySrc][xSrc].y, 
                                             theImage.image[ySrc][xSrc].cb,
                                             theImage.image[ySrc][xSrc].cr);
      }
    SEND_DEBUG_IMAGE(horizonAligned);
  );


}

void CoordinateSystemProvider::calcScaleFactors(double& a, double& b) const
{
  double imageRecordingTime = theRobotDimensions.imageRecordingTime;
  DEBUG_RESPONSE("module:CoordinateSystemProvider:no correction", 
    imageRecordingTime = 0;
  );
  if(theFilteredJointData.timeStamp != prevTime)
  {
    double timeDiff = int(theFilteredJointData.timeStamp - prevTime) / 1000.0; // in seconds
    double timeDiff2 = int(theFrameInfo.time - theFilteredJointData.timeStamp) / 1000.0; // in seconds
    a = timeDiff2 / timeDiff;
    b = imageRecordingTime / theImage.cameraInfo.resolutionHeight / timeDiff;
  }
  else
    a = b = 0;
}

MAKE_MODULE(CoordinateSystemProvider, Perception)
