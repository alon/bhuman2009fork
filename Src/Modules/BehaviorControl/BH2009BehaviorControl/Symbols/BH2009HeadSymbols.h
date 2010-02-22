/** 
* @file BH2009HeadSymbols.h
*
* Declaration of class BH2009HeadSymbols.
*
* @author Max Risler
*/

#ifndef __BH2009HeadSymbols_h_
#define __BH2009HeadSymbols_h_

#include "../../Symbols.h"
#include "Representations/Infrastructure/CameraInfo.h"
#include "Representations/Perception/CameraMatrix.h"
#include "Representations/MotionControl/HeadMotionRequest.h"
#include "Representations/MotionControl/MotionInfo.h"
#include "Representations/Infrastructure/JointData.h"
#include "Representations/Modeling/RobotPose.h"
#include "Representations/Configuration/JointCalibration.h"
#include "Representations/Configuration/RobotDimensions.h"
#include "Tools/Math/Vector3.h"

/**
* The Xabsl symbols that are defined in "head_symbols.xabsl"
*
* @author Max Risler
*/ 
class BH2009HeadSymbols : public Symbols
{
public:
  /*
  * Constructor.
  * @param headMotionRequest A reference to the HeadMotionRequest.
  * @param jointCalibration A reference to the JointCalibration.
  * @param sensorData A reference to the SensorData.
  * @param cameraInfo A reference to the CameraInfo.
  * @param cameraMatrix A reference to the CameraMatrix.
  */
  BH2009HeadSymbols(HeadMotionRequest& HeadMotionRequest, JointCalibration const& jointCalibration, FilteredJointData const& jointData, CameraInfo const& cameraInfo, CameraMatrix const& cameraMatrix, RobotPose const& robotPose, RobotDimensions const& robotDimensions) : 
    headMotionRequest(HeadMotionRequest),
    jointCalibration(jointCalibration),
    jointData(jointData),
    cameraInfo(cameraInfo),
    cameraMatrix(cameraMatrix),
    robotPose(robotPose),
    robotDimensions(robotDimensions),
    lookAtPointX(0),
    lookAtPointY(0),
    offsetInImageX(0),
    offsetInImageY(0),
    lookAtPointHeadPan(0),
    lookAtPointHeadTilt(0)
  {
    theInstance = this;
  }

  PROCESS_WIDE_STORAGE_STATIC BH2009HeadSymbols* theInstance; /**< Points to the only instance of this class in this process or is 0 if there is none. */

  /** A reference to the HeadMotionRequest */
  HeadMotionRequest& headMotionRequest;

  /** A reference to the JointCalibration */
  JointCalibration const& jointCalibration;

  /** A reference to the FilteredJointData */
  FilteredJointData const& jointData;

  /** A reference to the CameraInfo */
  CameraInfo const& cameraInfo;

  /** A reference to the CameraMatrix */
  CameraMatrix const& cameraMatrix;

  RobotPose const& robotPose;

  /** A reference to the RobotDimensions */
  RobotDimensions const& robotDimensions;

  /** registers the symbols at an engine */
  void registerSymbols(xabsl::Engine& engine);

  static void setTilt(double rot);
  static double getTilt();

  static double getExecutedTilt();

  static void setPan(double rot);
  static double getPan();

  static double getExecutedPan();

  static void setSpeed(double rot);
  static double getSpeed();

  static bool getPositionReached();
  static bool getPositionUnreachable();

  double lookAtPointX;
  double lookAtPointY;
  double lookAtPointZ;

  double offsetInImageX;
  double offsetInImageY;
  
  double lookAtPointHeadPan;
  double lookAtPointHeadTilt;

  double maxTiltUp;
  double intendedHeadTiltUp;

  static void lookAtPointParametersChanged();
  static void maxTiltUpParametersChanged();

  virtual void update();

private:
  /**
  * Calculate angles to look at a given point.
  * Calculation taken from GT2006 HeadControl. */
  void calculateLookAtPoint();
  void calculateMaxTiltUp(); // Calculate the maximum tilt for a given headpan
  /**
  * Methods to check if a requested joint angle is reachable or if it outruns the head limits*/
  inline static bool panAngleTooLarge();
  inline static bool panAngleTooSmall();
  inline static bool tiltAngleTooLarge();
  inline static bool tiltAngleTooSmall();
};


#endif // __HeadSymbols_h_

