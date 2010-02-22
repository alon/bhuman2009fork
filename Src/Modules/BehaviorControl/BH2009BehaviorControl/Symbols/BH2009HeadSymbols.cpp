/** 
* @file BH2009HeadSymbols.cpp
*
* Implementation of class BH2009HeadSymbols.
*
* @author Judith Müller
*/

#include "BH2009HeadSymbols.h"
#include "Tools/Xabsl/GT/GTXabslTools.h"
#include "Tools/Math/Common.h"
#include "Representations/Configuration/RobotDimensions.h"
#include "Tools/Debugging/DebugDrawings.h"
#include "Tools/Math/Geometry.h"

PROCESS_WIDE_STORAGE BH2009HeadSymbols* BH2009HeadSymbols::theInstance = 0;

void BH2009HeadSymbols::registerSymbols(xabsl::Engine& engine)
{
  // "head.head_tilt"
  engine.registerDecimalOutputSymbol("head.head_tilt",
    &setTilt,
    &getTilt);

  // "head.head_pan"
  engine.registerDecimalOutputSymbol("head.head_pan",
    &setPan,
    &getPan);

  // "head.executed_head_pan"
  engine.registerDecimalInputSymbol("head.executed_head_pan",
    &getExecutedPan);

  // "head.executed_head_tilt"
  engine.registerDecimalInputSymbol("head.executed_tilt",
    &getExecutedTilt);

  // "head.speed"
  engine.registerDecimalOutputSymbol("head.speed",
    &setSpeed,
    &getSpeed);

  // "head.position_reached"
  engine.registerBooleanInputSymbol("head.position_reached",
    &getPositionReached);

 // "head.position_unreachable"
  engine.registerBooleanInputSymbol("head.position_unreachable",
    &getPositionUnreachable);

  engine.registerDecimalInputSymbol("head.look_at_point.head_pan",
    &lookAtPointHeadPan);
  engine.registerDecimalInputSymbolParametersChanged("head.look_at_point.head_pan",
    &lookAtPointParametersChanged);
  engine.registerDecimalInputSymbolDecimalParameter("head.look_at_point.head_pan","head.look_at_point.head_pan.x",
    &lookAtPointX);
  engine.registerDecimalInputSymbolDecimalParameter("head.look_at_point.head_pan","head.look_at_point.head_pan.y",
    &lookAtPointY);
  engine.registerDecimalInputSymbolDecimalParameter("head.look_at_point.head_pan","head.look_at_point.head_pan.z",
    &lookAtPointZ);
  engine.registerDecimalInputSymbolDecimalParameter("head.look_at_point.head_pan","head.look_at_point.head_pan.offset_in_image.x",
    &offsetInImageX);
  engine.registerDecimalInputSymbolDecimalParameter("head.look_at_point.head_pan","head.look_at_point.head_pan.offset_in_image.y",
    &offsetInImageY);
  engine.registerDecimalInputSymbol("head.look_at_point.head_tilt",
    &lookAtPointHeadTilt);
  engine.registerDecimalInputSymbolParametersChanged("head.look_at_point.head_tilt",
    &lookAtPointParametersChanged);
  engine.registerDecimalInputSymbolDecimalParameter("head.look_at_point.head_tilt","head.look_at_point.head_tilt.x",
    &lookAtPointX);
  engine.registerDecimalInputSymbolDecimalParameter("head.look_at_point.head_tilt","head.look_at_point.head_tilt.y",
    &lookAtPointY);
  engine.registerDecimalInputSymbolDecimalParameter("head.look_at_point.head_tilt","head.look_at_point.head_tilt.z",
    &lookAtPointZ);
  engine.registerDecimalInputSymbolDecimalParameter("head.look_at_point.head_tilt","head.look_at_point.head_tilt.offset_in_image.x",
    &offsetInImageX);
  engine.registerDecimalInputSymbolDecimalParameter("head.look_at_point.head_tilt","head.look_at_point.head_tilt.offset_in_image.y",
    &offsetInImageY);
  engine.registerDecimalInputSymbol("head.max_head_tilt_up_for_target_head_pan",
    &maxTiltUp);
  engine.registerDecimalInputSymbolParametersChanged("head.max_head_tilt_up_for_target_head_pan",
    &maxTiltUpParametersChanged);
  engine.registerDecimalInputSymbolDecimalParameter("head.max_head_tilt_up_for_target_head_pan","head.max_head_tilt_up_for_target_head_pan.intendedTilt",
    &intendedHeadTiltUp);
}

void BH2009HeadSymbols::setTilt(double rot)
{
  theInstance->headMotionRequest.tilt = fromDegrees(rot);
}

double BH2009HeadSymbols::getTilt()
{
  return toDegrees(theInstance->headMotionRequest.tilt);
}

double BH2009HeadSymbols::getExecutedTilt()
{
  return toDegrees(theInstance->jointData.angles[JointData::headTilt]);
}

void BH2009HeadSymbols::setPan(double rot)
{
  theInstance->headMotionRequest.pan = fromDegrees(rot);
}

double BH2009HeadSymbols::getPan()
{
  return toDegrees(theInstance->headMotionRequest.pan);
}

double BH2009HeadSymbols::getExecutedPan()
{
  return toDegrees(theInstance->jointData.angles[JointData::headPan]);
}

void BH2009HeadSymbols::setSpeed(double rot)
{
  theInstance->headMotionRequest.speed = fromDegrees(rot);
}

double BH2009HeadSymbols::getSpeed()
{
  return toDegrees(theInstance->headMotionRequest.speed);
}

bool BH2009HeadSymbols::getPositionReached()
{
  double tolerance = fromDegrees(2);
  return (
    (fabs(theInstance->headMotionRequest.pan - theInstance->jointData.angles[JointData::headPan]) < tolerance || 
    (fabs(theInstance->jointCalibration.joints[JointData::headPan].maxAngle - theInstance->jointData.angles[JointData::headPan]) < tolerance && panAngleTooLarge()) ||
    (fabs(theInstance->jointCalibration.joints[JointData::headPan].minAngle - theInstance->jointData.angles[JointData::headPan]) < tolerance && panAngleTooSmall())) &&
    (fabs(theInstance->headMotionRequest.tilt - theInstance->jointData.angles[JointData::headTilt]) < tolerance ||
    (fabs(theInstance->jointCalibration.joints[JointData::headTilt].maxAngle - theInstance->jointData.angles[JointData::headTilt]) < tolerance && tiltAngleTooLarge()) ||
    (fabs(theInstance->jointCalibration.joints[JointData::headTilt].minAngle - theInstance->jointData.angles[JointData::headTilt]) < tolerance && tiltAngleTooSmall())));
}

bool BH2009HeadSymbols::getPositionUnreachable()
{
  return (
    panAngleTooLarge() || panAngleTooSmall() || tiltAngleTooLarge() || tiltAngleTooSmall() );
}

bool BH2009HeadSymbols::panAngleTooSmall()
{
  return (
    theInstance->headMotionRequest.pan < theInstance->jointCalibration.joints[JointData::headPan].minAngle);
}

bool BH2009HeadSymbols::panAngleTooLarge()
{
  return (
    theInstance->headMotionRequest.pan > theInstance->jointCalibration.joints[JointData::headPan].maxAngle);
}

bool BH2009HeadSymbols::tiltAngleTooSmall()
{
  return (
    theInstance->headMotionRequest.tilt < theInstance->jointCalibration.joints[JointData::headTilt].minAngle);
}

bool BH2009HeadSymbols::tiltAngleTooLarge()
{
  return (
    theInstance->headMotionRequest.tilt > theInstance->jointCalibration.joints[JointData::headTilt].maxAngle);
}

void BH2009HeadSymbols::lookAtPointParametersChanged()
{
  theInstance->calculateLookAtPoint();
}

void BH2009HeadSymbols::calculateLookAtPoint()
{
  Vector2<double> pos(lookAtPointX, lookAtPointY);
  
  if (pos.abs() < 50) // clip ball distances close to and inside of the robot
    pos.normalize(50.0);

  Vector3<double> target(pos.x, pos.y, lookAtPointZ);
  double headPan, headTilt;

  Vector2<double> angles;

  //Calculate the head-pan:
  Vector2<double> panJointToBall(target.x, target.y);
  angles.y = atan2(panJointToBall.y, panJointToBall.x);
  headPan = angles.y;

  //Copied from old Repository
  double beta;
  double distCTF;
  double h;
  h = cameraMatrix.translation.z;
  Vector2<double> camXY(cameraMatrix.translation.x, cameraMatrix.translation.y);
  Vector2<double> fieldXY(target.x, target.y);
  distCTF = (fieldXY - camXY).abs();
  beta = atan2(h, distCTF);
  headTilt = -beta + robotDimensions.headTiltToCameraTilt;

  const Range<double> jointRangeHeadPan(fromDegrees(-90), fromDegrees(90));
  const Range<double> jointRangeHeadTilt(fromDegrees(-75), fromDegrees(90));

  double newLookAtPointHeadPan = toDegrees(jointRangeHeadPan.limit(headPan));
  double newLookAtPointHeadTilt = toDegrees(jointRangeHeadTilt.limit(headTilt));

  // avoid head shaking on "look_at_ball"
  if(fabs(lookAtPointHeadPan-newLookAtPointHeadPan) > 5.0)
    lookAtPointHeadPan = newLookAtPointHeadPan;
  if(fabs(lookAtPointHeadTilt-newLookAtPointHeadTilt) > 3.0)
    lookAtPointHeadTilt = newLookAtPointHeadTilt;
}

void BH2009HeadSymbols::maxTiltUpParametersChanged()
{
  theInstance->calculateMaxTiltUp();
}

void BH2009HeadSymbols::calculateMaxTiltUp()
{
  double currentHeadPan = getPan();
  if(currentHeadPan <= 15)
  {
    maxTiltUp = intendedHeadTiltUp > 44 ? 44 : intendedHeadTiltUp;
  }
  else if(currentHeadPan <= 53)
  {
    maxTiltUp = intendedHeadTiltUp > 27 ? 27 : intendedHeadTiltUp;
  }
  else 
  {
    maxTiltUp = intendedHeadTiltUp > 20 ? 20 : intendedHeadTiltUp;
  }
}

void BH2009HeadSymbols::update()
{

  DECLARE_DEBUG_DRAWING("module:BehaviorControl:HeadSymbols:LookAtPoint","drawingOnImage"); // draws the target position for look at point

  COMPLEX_DRAWING("module:BehaviorControl:HeadSymbols:LookAtPoint",
    Vector2<int> pointInImage;
  Geometry::calculatePointInImage(Vector3<double>(lookAtPointX, lookAtPointY, lookAtPointZ), cameraMatrix, cameraInfo, pointInImage);
  CIRCLE("module:BehaviorControl:HeadSymbols:LookAtPoint", pointInImage.x, pointInImage.y, 10, 
    2, Drawings::ps_solid, ColorClasses::blue, Drawings::bs_null, ColorClasses::none);
  LINE("module:BehaviorControl:HeadSymbols:LookAtPoint", 
    offsetInImageX - 10 + cameraInfo.resolutionWidth / 2, 
    offsetInImageY - 10 + cameraInfo.resolutionHeight / 2, 
    offsetInImageX + 10 + cameraInfo.resolutionWidth / 2, 
    offsetInImageY + 10 + cameraInfo.resolutionHeight / 2,
    2, Drawings::ps_solid, ColorClasses::black);
  LINE("module:BehaviorControl:HeadSymbols:LookAtPoint", 
    offsetInImageX + 10 + cameraInfo.resolutionWidth / 2, 
    offsetInImageY - 10 + cameraInfo.resolutionHeight / 2, 
    offsetInImageX - 10 + cameraInfo.resolutionWidth / 2, 
    offsetInImageY + 10 + cameraInfo.resolutionHeight / 2,
    2, Drawings::ps_solid, ColorClasses::black);
  );
}
