/** 
* @file WalkingEngine.cpp
* This file implements a module that creates the walking motions.
* @author <a href="mailto:allli@informatik.uni-bremen.de">Alexander Härtl</a>
* @author Colin Graf
*/

#include "WalkingEngine.h"
#include "Tools/Debugging/DebugDrawings.h"
#include "Tools/InverseKinematic.h"

MAKE_MODULE(WalkingEngine, Motion Control)

WalkingEngine::WalkingEngine() : wasActive(false), isLeavingPossible(true), enforceStandTime(0)
{
  p.stepDuration = 900;
  p.stepLift = 0;

  p.footOrigin = Vector3<>(-20., 50., -185);
  p.footOriginRotation = Vector3<>(0., 0., 0.);
  p.footCenter = Vector3<>(0., 0., 0.); // Vector3<>(20., 4., -48.8);
  p.footRotation = 0;

  p.legHardness = 75;
  p.leg5Hardness = 75;

  p.bodyOriginTilt = 0.;
  p.bodyRotation = -0.08;

  p.armOriginRotation = Vector2<>(0.4, 0.);
  p.armMoveRotation = Vector2<>(0., 0.5);

  p.coMShift = 23;
  p.coMLift = 0;
  p.coMShiftShape = OscillationShape(20., 1., 0.);
  p.coMSpeedOffset = Vector2<>(0., 0.);
  p.coMAccelerationOffset = Vector2<>();
  p.coMTransferRatio = 0.5;

  p.liftOffset = Vector3<>(-5., 0., 20.);
  p.liftPhases = Vector2<>(0.1, 0.9); 

  p.movePhases = Vector2<>(0.2, 0.6);

  p.maxSpeed = Pose2D(0.5, 120, 40);
  p.maxBackwardSpeed = -80;
  p.maxDashSpeed = Pose2D(0.8, 0., 0.);
  p.maxSpeedChange = Pose2D(p.maxSpeed.rotation / 2., p.maxSpeed.translation.x / 2., p.maxSpeed.translation.y / 2.);
  p.maxSpeedEllipsoidClipping = true;

  p.measuredCoMGain = 2.;
  p.measuredCoMDelay = 80;

  p.balanceStep = Vector2<>(0.05, 0.);
  p.balanceNextStep = Vector2<>();
  p.balanceCoM = Vector3<>(0.2, 0.2, -0.);
  p.balancePhaseOld = 0.0002;
  p.balancePhaseAcceleration = 0.01;
  p.balancePhaseDeceleration = 0.02;
  p.balanceBodyRotation = Vector2<>();
  p.balanceFootRotation = Vector2<>(0.2, -0.2);
  p.balanceArm = Vector2<>(0, 0.);
  p.balanceMaxInstability = 120.;
  p.balanceMaxInstabilityStandTime = 3000;
  p.balanceAirFoot = true;

  p.odometryScale = Pose2D(1., 1., 1.);
  p.odometryUseInertiaMatrix = true;  
}

void WalkingEngine::init()
{
  InConfigFile stream(Global::getSettings().expandRobotFilename("walking.cfg"));
  if(stream.exists())
    stream >> p;
  else
  {
    InConfigFile stream("walking.cfg");
    if(stream.exists())
      stream >> p;
  }

#ifdef TARGET_SIM
  p.measuredCoMDelay = 180;
#endif

  initOptimization();
}

void WalkingEngine::initOptimization()
{
  double parameters[][3] = {
    // start value,   min,  max
    { p.bodyRotation, p.bodyRotation - 0.01, p.bodyRotation + 0.01 },
    { p.footRotation, p.footRotation - 0.01, p.footRotation + 0.01 },
    { p.coMShift, p.coMShift - 2., p.coMShift + 2.},
  };
  optimization.init(Global::getSettings().expandRobotFilename("expWalking9.log"), parameters);
}

void WalkingEngine::updateOptimization()
{
  double* values;
  optimization.getNextValues(values);
  p.bodyRotation = values[0];
  p.footRotation = values[1];
  p.coMShift = values[2];
}

void WalkingEngine::calculateMeasuredCoM(const CoMSet& oldDesiredCoMSet, CoMSet& measuredCoMSet)
{
  Vector3<> rotationErrorAxis(
    -(theFilteredSensorData.data[SensorData::angleX] + oldDesiredCoMSet.bodyRotation.x / p.measuredCoMGain),
    -(theFilteredSensorData.data[SensorData::angleY] + oldDesiredCoMSet.bodyRotation.y), 0.);
  RotationMatrix rotationError(rotationErrorAxis, rotationErrorAxis.abs());
  
  measuredCoMSet.left = rotationError * oldDesiredCoMSet.left;
  measuredCoMSet.right = rotationError * oldDesiredCoMSet.right;

  measuredCoMSet.bodyRotation.x = -theFilteredSensorData.data[SensorData::angleX] * p.measuredCoMGain;
  measuredCoMSet.bodyRotation.y = -theFilteredSensorData.data[SensorData::angleY];
  measuredCoMSet.timeStamp = oldDesiredCoMSet.timeStamp;
}

void WalkingEngine::calculateError(const CoMSet& measuredCoMSet, const CoMSet& oldDesiredCoMSet, Vector3<>& coMError, Vector3<>& rotationError)
{
  coMError.x = ((measuredCoMSet.left.x - oldDesiredCoMSet.left.x) + (measuredCoMSet.right.x - oldDesiredCoMSet.right.x)) / 2.;
  coMError.y = ((measuredCoMSet.left.y - oldDesiredCoMSet.left.y) + (measuredCoMSet.right.y - oldDesiredCoMSet.right.y)) / 2.;
  coMError.z = ((measuredCoMSet.left.z - oldDesiredCoMSet.left.z) + (measuredCoMSet.right.z - oldDesiredCoMSet.right.z)) / 2.;

  rotationError.x = theFilteredSensorData.data[SensorData::angleX] + oldDesiredCoMSet.bodyRotation.x / p.measuredCoMGain;
  rotationError.y = theFilteredSensorData.data[SensorData::angleY] + oldDesiredCoMSet.bodyRotation.y;
  rotationError.z = 0.;

  instability.add(coMError.x * coMError.x + coMError.y * coMError.y + coMError.z * coMError.z);
}

void WalkingEngine::calculateOldDesiredCoM(CoMSet& oldDesiredCoMSet)
{
  unsigned int motionCycleTime = (unsigned int)(theRobotDimensions.motionCycleTime * 1000.); // in ms  
  const double& oldCoMTimeStamp = theFrameInfo.time - p.measuredCoMDelay;
  
  for(int newerCoMIndex = 1, numberOfEntries = oldCoMSets.getNumberOfEntries(); newerCoMIndex < numberOfEntries; ++newerCoMIndex)
  {
    const double& currentTimeStamp = oldCoMSets.getEntry(newerCoMIndex).timeStamp;
    const double& nextTimeStamp = (newerCoMIndex + 1) < numberOfEntries ? oldCoMSets.getEntry(newerCoMIndex + 1).timeStamp : (currentTimeStamp - motionCycleTime);
    if(oldCoMTimeStamp == currentTimeStamp)
    {
      oldDesiredCoMSet = oldCoMSets.getEntry(newerCoMIndex);
      return;
    }
    else if(oldCoMTimeStamp < currentTimeStamp && oldCoMTimeStamp > nextTimeStamp)
    { // interpolate between two old CoMSets
      const CoMSet& newerCoMSet(oldCoMSets.getEntry(newerCoMIndex));
      const CoMSet& olderCoMSet((newerCoMIndex + 1) < numberOfEntries ? oldCoMSets.getEntry(newerCoMIndex + 1) : standCoMSet);
      const double& olderCoMSetTimeStamp = (newerCoMIndex + 1) < numberOfEntries ? olderCoMSet.timeStamp : (newerCoMSet.timeStamp - motionCycleTime);
      
      double newerRatio = 1. - (newerCoMSet.timeStamp - oldCoMTimeStamp) / (newerCoMSet.timeStamp - olderCoMSetTimeStamp);
      double olderRatio = 1. - newerRatio;
      
      oldDesiredCoMSet.left = olderCoMSet.left * olderRatio + newerCoMSet.left * newerRatio;
      oldDesiredCoMSet.right = olderCoMSet.right * olderRatio + newerCoMSet.right * newerRatio;
      oldDesiredCoMSet.bodyRotation = olderCoMSet.bodyRotation * olderRatio + newerCoMSet.bodyRotation * newerRatio;
      oldDesiredCoMSet.timeStamp = oldCoMTimeStamp;
      return;
    }
  }

  oldDesiredCoMSet = standCoMSet;
  oldDesiredCoMSet.timeStamp = oldCoMTimeStamp;
}

bool WalkingEngine::calculateCorrectedPhase(const CoMSet& measuredCoMSet, const CoMSet& oldDesiredCoMSet)
{
  PLOT("module:WalkingEngine:oldBodyRotation", oldDesiredCoMSet.bodyRotation.x);
  PLOT("module:WalkingEngine:measuredBodyRotation", measuredCoMSet.bodyRotation.x);

  double oldPhase = phase - double(p.measuredCoMDelay) / p.stepDuration;
  if(oldPhase < 0.)
    oldPhase += 1.;
  PLOT("module:WalkingEngine:oldPhase", oldPhase);

  double phaseOffset = 0.;

  if(p.balancePhaseOld != 0.)
  {
    double error = ((measuredCoMSet.left.y - oldDesiredCoMSet.left.y) + (measuredCoMSet.right.y - oldDesiredCoMSet.right.y)) / 2.;

    phaseOffset = error * -p.balancePhaseOld * (cos(oldPhase * pi2 * 2.) + 1.) / 2. * ((oldPhase >= 0.25 && oldPhase < 0.75) ? 1. : -1.);

    phaseOffset = Range<double>(-(theRobotDimensions.motionCycleTime * 1000.) / p.stepDuration * 0.5, (theRobotDimensions.motionCycleTime * 1000.) / p.stepDuration * 0.5).limit(phaseOffset);
  }

  else if(p.bodyRotation != 0. && (p.balancePhaseAcceleration != 0. || p.balancePhaseDeceleration != 0.))
  {
    double measuredPhase = asin(Range<double>(-1., 1.).limit(measuredCoMSet.bodyRotation.x / p.bodyRotation));
    if(oldPhase >= 0.25 && oldPhase < 0.5)
      measuredPhase = pi - measuredPhase;
    else if(oldPhase >= 0.5 && oldPhase < 0.75)
      measuredPhase = pi - measuredPhase;
    else if(oldPhase >= 0.75 && oldPhase < 1.)
      measuredPhase = pi2 + measuredPhase;
    measuredPhase /= pi2;
    PLOT("module:WalkingEngine:measuredPhase", measuredPhase);

    double phaseError = measuredPhase - oldPhase;

    PLOT("module:WalkingEngine:phaseError", phaseError);

    double phaseOffset = phaseError * (phaseError > 0 ? p.balancePhaseAcceleration : p.balancePhaseDeceleration);

    phaseOffset = Range<double>(-(theRobotDimensions.motionCycleTime * 1000.) / p.stepDuration * 0.25, (theRobotDimensions.motionCycleTime * 1000.) / p.stepDuration * 0.25).limit(phaseOffset);  
  }

  PLOT("module:WalkingEngine:phaseOffset", phaseOffset);

  if(phaseOffset == 0.)
    return false;

  phase += phaseOffset;
  if(phase >= 1.)
    phase -= 1.;
  if(phase < 0.)
    phase += 1.;

  const double& timeOffset = phaseOffset * p.stepDuration;
  for(int i = 0, count = oldCoMSets.getNumberOfEntries(); i < count; ++i)
    oldCoMSets.getEntry(i).timeStamp -= timeOffset;

  return true;
}

void WalkingEngine::calculateDesiredCoM(const double& lift, const Vector3<>& coMOffset, const Vector3<>& bodyRotation, const Pose3D& left, const Pose3D& right, CoMSet& desiredCoMSet, const double& fadeIn, const double& moveFadeIn)
{
  const bool leftGround = phase >= 0.5;

  Vector3<>& coM2Foot(leftGround ? desiredCoMSet.left : desiredCoMSet.right);
  Vector3<>& coM2OtherFoot(!leftGround ? desiredCoMSet.left : desiredCoMSet.right);

  const double coMShift = p.coMShift * p.coMShiftShape.getValue(phase);

  double usedFadeIn = !leftGround ? phase * 2. : (phase - 0.5) * 2.;

  const Vector3<> coMMovement = !leftGround ? 
    (leftStep.size * (usedFadeIn * p.coMTransferRatio) - rightStep.size * ((1. - usedFadeIn) * (1. - p.coMTransferRatio))) : 
    (rightStep.size * (usedFadeIn * p.coMTransferRatio) - leftStep.size * ((1. - usedFadeIn) * (1. - p.coMTransferRatio))); // com movement in step direction
  const double rotationMovement = !leftGround ? 
    (leftStep.rotation * (usedFadeIn * p.coMTransferRatio) - rightStep.rotation * ((1. - usedFadeIn) * (1. - p.coMTransferRatio))) : 
    (rightStep.rotation * (usedFadeIn * p.coMTransferRatio) - leftStep.rotation * ((1. - usedFadeIn) * (1. - p.coMTransferRatio)));

  coM2Foot = Pose3D(standRobotModel.centerOfMass * -1.).rotate(RotationMatrix(bodyRotation, bodyRotation.abs()))
    .translate(Vector3<>(0., coMShift, -p.coMLift * lift) - coMMovement - coMOffset)
    .conc(standRobotModel.limbs[leftGround ? RobotModel::footLeft : RobotModel::footRight])
    .rotateZ(-rotationMovement).translate(p.footCenter.x, leftGround ? p.footCenter.y : -p.footCenter.y, p.footCenter.z).translation;

  const Vector3<>& hip2left(Pose3D(left).translate(p.footCenter.x, p.footCenter.y, p.footCenter.z).translation);
  const Vector3<>& hip2right(Pose3D(right).translate(p.footCenter.x, -p.footCenter.y, p.footCenter.z).translation);
  coM2OtherFoot = leftGround ? (coM2Foot + (hip2left * -1. + hip2right)) : (coM2Foot + (hip2right * -1. + hip2left));
  
  desiredCoMSet.timeStamp = theFrameInfo.time;
  desiredCoMSet.bodyRotation = Vector2<>(bodyRotation.x, bodyRotation.y);

  oldCoMSets.add(desiredCoMSet);
}

void WalkingEngine::calculateCorrectedCoM(const double& oscillation, const Vector3<>& error, const CoMSet& desiredCoMSet, CoMSet& correctedCoMSet)
{
  correctedCoMSet = desiredCoMSet;

  Vector3<> correction(
    error.x * -p.balanceCoM.x,
    error.y * -p.balanceCoM.y,
    error.z * -p.balanceCoM.z);

  static const Range<double> correctionLimit(-10., 10);
  correction.x = correctionLimit.limit(correction.x);
  correction.y = correctionLimit.limit(correction.y);
  correction.z = correctionLimit.limit(correction.z);

  correctedCoMSet.left += correction;
  correctedCoMSet.right += correction;
}

void WalkingEngine::setJoints(const double& oscillation, const CoMSet& newCoMSet, 
  const Vector2<>& leftArm, const Vector2<>& rightArm, Pose3D& left, Pose3D& right, JointRequest& jointRequest)
{
  // calculate foot positions using old bodyShift
  left.translation -= bodyShift;
  right.translation -= bodyShift;

  setLegJoints(left, right, 0.5, jointRequest);
  setArmJoints(leftArm, rightArm, jointRequest);
  setHeadJoints(jointRequest);

  const RobotModel tempRobotModel(jointRequest, theRobotDimensions, theMassCalibration);
  const Vector3<>& leftTempCoM(Pose3D(tempRobotModel.centerOfMass * -1.).conc(tempRobotModel.limbs[RobotModel::footLeft]).translate(p.footCenter.x, p.footCenter.y, p.footCenter.z).translation);
  const Vector3<>& rightTempCoM(Pose3D(tempRobotModel.centerOfMass * -1.).conc(tempRobotModel.limbs[RobotModel::footRight]).translate(p.footCenter.x, -p.footCenter.y, p.footCenter.z).translation);
  const double& ratio = 1. - (oscillation + 1.) / 2.;
  const Vector3<>& tempCoM(leftTempCoM * ratio + rightTempCoM * (1. - ratio));
  const Vector3<>& newCoM(newCoMSet.left * ratio + newCoMSet.right * (1. - ratio));
  const Vector3<>& bodyShiftOffset(newCoM - tempCoM); // this is not correct but it works well

  // update body shift
  bodyShift -= bodyShiftOffset; 

  PLOT("module:WalkingEngine:bodyShiftX", bodyShift.x);
  PLOT("module:WalkingEngine:bodyShiftY", bodyShift.y);
  PLOT("module:WalkingEngine:bodyShiftZ", bodyShift.z);

  // apply new body shift
  left.translation -= bodyShiftOffset;
  right.translation -= bodyShiftOffset;

  // generate joint angles
  setLegJoints(left, right, 0.5, jointRequest);
}

void WalkingEngine::update(WalkingEngineOutput& walkingEngineOutput)
{
  MODIFY("module:WalkingEngine:parameters", p);
  MODIFY("module:WalkingEngine:bestParameters", bestParameters);
  bool optimize = false, continuousOptimize = false;
  DEBUG_RESPONSE("module:WalkingEngine:optimize", optimize = true; );
  DEBUG_RESPONSE("module:WalkingEngine:continuousOptimize", continuousOptimize = true; );

  DECLARE_PLOT("module:WalkingEngine:phase");
  DECLARE_PLOT("module:WalkingEngine:oscillation");
  DECLARE_PLOT("module:WalkingEngine:leftFadeIn");
  DECLARE_PLOT("module:WalkingEngine:rightFadeIn");
  DECLARE_PLOT("module:WalkingEngine:leftMoveFadeIn");
  DECLARE_PLOT("module:WalkingEngine:rightMoveFadeIn");
  DECLARE_PLOT("module:WalkingEngine:leftLift");
  DECLARE_PLOT("module:WalkingEngine:rightLift");

  DECLARE_PLOT("module:WalkingEngine:leftX");
  DECLARE_PLOT("module:WalkingEngine:leftY");
  DECLARE_PLOT("module:WalkingEngine:leftZ");

  DECLARE_PLOT("module:WalkingEngine:rightX");
  DECLARE_PLOT("module:WalkingEngine:rightY");
  DECLARE_PLOT("module:WalkingEngine:rightZ");

  DECLARE_PLOT("module:WalkingEngine:bodyShiftX");
  DECLARE_PLOT("module:WalkingEngine:bodyShiftY");
  DECLARE_PLOT("module:WalkingEngine:bodyShiftZ");

  DECLARE_PLOT("module:WalkingEngine:oldDesiredCoMLX");
  DECLARE_PLOT("module:WalkingEngine:oldDesiredCoMLY");
  DECLARE_PLOT("module:WalkingEngine:oldDesiredCoMLZ");

  DECLARE_PLOT("module:WalkingEngine:oldDesiredCoMRX");
  DECLARE_PLOT("module:WalkingEngine:oldDesiredCoMRY");
  DECLARE_PLOT("module:WalkingEngine:oldDesiredCoMRZ");
  
  DECLARE_PLOT("module:WalkingEngine:desiredCoMLX");
  DECLARE_PLOT("module:WalkingEngine:desiredCoMLY");
  DECLARE_PLOT("module:WalkingEngine:desiredCoMLZ");
  
  DECLARE_PLOT("module:WalkingEngine:desiredCoMRX");
  DECLARE_PLOT("module:WalkingEngine:desiredCoMRY");
  DECLARE_PLOT("module:WalkingEngine:desiredCoMRZ");

  DECLARE_PLOT("module:WalkingEngine:measuredCoMLX");
  DECLARE_PLOT("module:WalkingEngine:measuredCoMLY");
  DECLARE_PLOT("module:WalkingEngine:measuredCoMLZ");

  DECLARE_PLOT("module:WalkingEngine:measuredCoMRX");
  DECLARE_PLOT("module:WalkingEngine:measuredCoMRY");
  DECLARE_PLOT("module:WalkingEngine:measuredCoMRZ");

  DECLARE_PLOT("module:WalkingEngine:correctedCoMLX");
  DECLARE_PLOT("module:WalkingEngine:correctedCoMLY");
  DECLARE_PLOT("module:WalkingEngine:correctedCoMLZ");

  DECLARE_PLOT("module:WalkingEngine:correctedCoMRX");
  DECLARE_PLOT("module:WalkingEngine:correctedCoMRY");
  DECLARE_PLOT("module:WalkingEngine:correctedCoMRZ");

  DECLARE_PLOT("module:WalkingEngine:errorX");
  DECLARE_PLOT("module:WalkingEngine:errorY");

  DECLARE_PLOT("module:WalkingEngine:instability");

  DECLARE_PLOT("module:WalkingEngine:oldPhase");
  DECLARE_PLOT("module:WalkingEngine:measuredPhase");
  DECLARE_PLOT("module:WalkingEngine:phaseError");
  DECLARE_PLOT("module:WalkingEngine:phaseOffset");

  DECLARE_PLOT("module:WalkingEngine:bodyRotation");
  DECLARE_PLOT("module:WalkingEngine:oldBodyRotation");
  DECLARE_PLOT("module:WalkingEngine:measuredBodyRotation");

  // update stand output
  if(theMotionSelection.ratios[MotionRequest::stand] > 0. || theMotionSelection.ratios[MotionRequest::walk] > 0.)
  {
    if(lastBodyOriginTilt != p.bodyOriginTilt || // detect origin changes
      lastFootOrigin != p.footOrigin ||
      lastFootOriginRotation != p.footOriginRotation)
    {
      // generate stand joint request
      Pose3D leftLeg(p.footOrigin.x, p.footOrigin.y, p.footOrigin.z);
      Pose3D rightLeg(p.footOrigin.x, -p.footOrigin.y, p.footOrigin.z);
      leftLeg.rotation = RotationMatrix(Vector3<>(p.footOriginRotation.x, p.footOriginRotation.y, p.footOriginRotation.z));
      rightLeg.rotation = RotationMatrix(Vector3<>(-p.footOriginRotation.x, p.footOriginRotation.y, -p.footOriginRotation.z));

      Pose3D left(RotationMatrix::fromRotationY(p.bodyOriginTilt));
      Pose3D right(RotationMatrix::fromRotationY(p.bodyOriginTilt));
      left.conc(leftLeg);
      right.conc(rightLeg);

      setLegJoints(left, right, 0.5, standOutput);
      setArmJoints(p.armOriginRotation, p.armOriginRotation, standOutput);
      setHeadJoints(0.f, 0.523599f, standOutput);

      // calculate the stand robot model      
      standRobotModel.setJointData(standOutput, theRobotDimensions, theMassCalibration);

      standCoMSet.left = Pose3D(standRobotModel.centerOfMass * -1.).conc(standRobotModel.limbs[RobotModel::footLeft]).translate(p.footCenter.x, p.footCenter.y, p.footCenter.z).translation;
      standCoMSet.right = Pose3D(standRobotModel.centerOfMass * -1.).conc(standRobotModel.limbs[RobotModel::footRight]).translate(p.footCenter.x, -p.footCenter.y, p.footCenter.z).translation;

      lastBodyOriginTilt = p.bodyOriginTilt;
      lastFootOrigin = p.footOrigin;
      lastFootOriginRotation = p.footOriginRotation;
    }

    setHeadJoints(standOutput);
  }

  // update walking engine output
  if(theMotionSelection.ratios[MotionRequest::walk] > 0.)
  {
    // update walking phase
    if(!wasActive)
    {
      phase = theMotionSelection.walkRequest.speed.translation.y > 0. ? 0.5 : 0.;
      wasLeftActive = false;
      wasRightActive = false;
      leftStep = Step();
      rightStep = Step();
      walkingEngineOutput.speed = Pose2D();
      oldCoMSets.init();
      instability.init();

      if(optimize || continuousOptimize)
      {
        if(optimization.isRated())
          updateOptimization();
        optimizationTime = theFrameInfo.time + 5000;
#ifdef TARGET_SIM
        OUTPUT(idConsole, text, "mv 0 0 300 0 0 0");
#endif
      }
    }
    else
    {
      phase += double(theFrameInfo.time - lastIterationTime) / p.stepDuration;
      if(phase >= 1.)
        phase -= 1.;
    }

    // calculate measured and corresponding desired com
    CoMSet oldDesiredCoMSet, measuredCoMSet;
    calculateOldDesiredCoM(oldDesiredCoMSet);
    calculateMeasuredCoM(oldDesiredCoMSet, measuredCoMSet);

    // phase correction
    if(calculateCorrectedPhase(measuredCoMSet, oldDesiredCoMSet))
    {
      calculateOldDesiredCoM(oldDesiredCoMSet);
      calculateMeasuredCoM(oldDesiredCoMSet, measuredCoMSet);
    }

    Vector3<> coMError, rotationError;
    calculateError(measuredCoMSet, oldDesiredCoMSet, coMError, rotationError);

    //
    bool leftActive = phase < 0.5; // left air
    bool rightActive = phase >= 0.5; // right air

    // calculate shapes based on sine and stepPhases
    const double phase2 = phase * 2;
    double oscillation = sin(phase * pi2);

    double leftFadeIn = 0., rightFadeIn = 0.;
    if(phase < 0.5)
      leftFadeIn = (1. - cos(phase2 * pi)) / 2.;
    if(phase >= 0.5)
      rightFadeIn = (1. - cos((phase2 - 1.) * pi)) / 2.;

    double leftLift = 0., rightLift = 0.;
    if(phase2 > p.liftPhases.x && phase2 < p.liftPhases.x + p.liftPhases.y)
      leftLift = (1. - cos((phase2 - p.liftPhases.x) / p.liftPhases.y * pi2)) / 2.;
    if(phase2 - 1. > p.liftPhases.x && phase2 - 1. < p.liftPhases.x + p.liftPhases.y)
      rightLift = (1. - cos((phase2 - 1. - p.liftPhases.x) / p.liftPhases.y * pi2)) / 2.;

    double leftMoveFadeIn = 0., rightMoveFadeIn = 0.;
    if(phase2 > p.movePhases.x && phase2 < 1.)
      leftMoveFadeIn = (phase2 < p.movePhases.x + p.movePhases.y) ? ((1. - cos((phase2 - p.movePhases.x) / p.movePhases.y * pi)) / 2.) : 1.;
    if(phase2 - 1. > p.movePhases.x && phase2 - 1. < 1.)
      rightMoveFadeIn = (phase2 - 1. < p.movePhases.x + p.movePhases.y) ? ((1. - cos((phase2 - 1. - p.movePhases.x) / p.movePhases.y * pi)) / 2.) : 1;

    PLOT("module:WalkingEngine:phase", phase);
    PLOT("module:WalkingEngine:oscillation", oscillation);
    PLOT("module:WalkingEngine:leftFadeIn", leftFadeIn);
    PLOT("module:WalkingEngine:rightFadeIn", -rightFadeIn);
    PLOT("module:WalkingEngine:leftMoveFadeIn", leftMoveFadeIn);
    PLOT("module:WalkingEngine:rightMoveFadeIn", -rightMoveFadeIn);
    PLOT("module:WalkingEngine:leftLift", leftLift);
    PLOT("module:WalkingEngine:rightLift", -rightLift);

    // store walking target
    if(theMotionSelection.walkRequest.speed != Pose2D())
      walkTarget = Pose2D();
    else if(theMotionSelection.walkRequest.target != Pose2D())
      walkTarget = theMotionSelection.walkRequest.target;

    // calculate step sizes at the beginning of each half step phase
    if(leftActive && !wasLeftActive)
      calculateStepSize(true, walkingEngineOutput.speed);
    if(rightActive && !wasRightActive)
      calculateStepSize(false, walkingEngineOutput.speed); 
    walkingEngineOutput.target = walkTarget;

    // add step-size correction
    if(!theMotionSelection.walkRequest.pedantic)
    {
      static const Range<double> maxBalanceStep(-1., 1.);
      if(leftActive && (leftLift > 0. || leftMoveFadeIn == 0.))
      {
        leftStep.size.x += maxBalanceStep.limit(coMError.x * p.balanceStep.x);
        leftStep.size.y += maxBalanceStep.limit(coMError.y * p.balanceStep.y);
      }
      if(rightActive && (rightLift > 0. || rightMoveFadeIn == 0.))
      {
        rightStep.size.x += maxBalanceStep.limit(coMError.x * p.balanceStep.x);
        rightStep.size.y += maxBalanceStep.limit(coMError.y * p.balanceStep.y);
      }
    }
    
    // start with stepOrigin
    Pose3D leftLeg(p.footOrigin.x, p.footOrigin.y, p.footOrigin.z);
    Pose3D rightLeg(p.footOrigin.x, -p.footOrigin.y, p.footOrigin.z);
    Vector2<> leftArm(p.armOriginRotation);
    Vector2<> rightArm(p.armOriginRotation);

    // add air leg lift
    Vector3<> leftLiftOffset(leftStep.liftOffset.x, leftStep.liftOffset.y, leftStep.liftOffset.z);
    Vector3<> rightLiftOffset(rightStep.liftOffset.x, -rightStep.liftOffset.y, rightStep.liftOffset.z);
    leftLiftOffset *= leftLift;
    rightLiftOffset *= rightLift;
    leftLeg.translation += leftLiftOffset;
    rightLeg.translation += rightLiftOffset;

    // add / remove step offsets
    Vector3<> coMOffset;
    double leftRotationZ = 0., rightRotationZ = 0.;
    if(leftActive) // left is air foot
    {
      coMOffset.x += leftStep.size.x * leftFadeIn * p.coMSpeedOffset.x;
      coMOffset.x += rightStep.size.x * (1. - leftFadeIn) * p.coMSpeedOffset.x;
      coMOffset.y += leftStep.size.y * leftFadeIn * p.coMSpeedOffset.y;
      coMOffset.y += rightStep.size.y * (1. - leftFadeIn) * p.coMSpeedOffset.y;

      // basic foot shifting

      leftLeg.translation -= rightStep.size * (1. - leftMoveFadeIn) * leftFadeIn; // decrement step size from the other foot
      leftRotationZ -= rightStep.rotation * (1. - leftMoveFadeIn);

      leftLeg.translation += leftStep.size * leftMoveFadeIn; // add step size
      leftRotationZ += leftStep.rotation * leftMoveFadeIn;

      rightLeg.translation += rightStep.size * (1. - leftFadeIn);

      rightArm += p.armMoveRotation * (leftStep.size.x / (100. / (1000. / p.stepDuration)) * leftMoveFadeIn);
      leftArm += p.armMoveRotation * (rightStep.size.x / (100. / (1000. / p.stepDuration)) * (1. - leftFadeIn));
    }
    else // right is air foot
    {
      coMOffset.x += rightStep.size.x * rightFadeIn * p.coMSpeedOffset.x;
      coMOffset.x += leftStep.size.x * (1. - rightFadeIn) * p.coMSpeedOffset.x;
      coMOffset.y += rightStep.size.y * rightFadeIn * p.coMSpeedOffset.y;
      coMOffset.y += leftStep.size.y * (1. - rightFadeIn) * p.coMSpeedOffset.y;

      // basic foot shifting

      rightLeg.translation -= leftStep.size * (1. - rightMoveFadeIn) * rightFadeIn; // decrement step size from the other foot
      rightRotationZ -= leftStep.rotation * (1. - rightMoveFadeIn);

      rightLeg.translation += rightStep.size * rightMoveFadeIn; // add step size
      rightRotationZ += rightStep.rotation * rightMoveFadeIn;

      leftLeg.translation += leftStep.size * (1. - rightFadeIn);

      leftArm += p.armMoveRotation * (rightStep.size.x / (100. / (1000. / p.stepDuration)) * rightMoveFadeIn);
      rightArm += p.armMoveRotation * (leftStep.size.x / (100. / (1000. / p.stepDuration)) * (1. - rightFadeIn));
    }

    // set foot rotations
    static const Range<double> maxBalanceRotation(-0.1, 0.1);

    Vector3<> airFootRotation(
      -(theFilteredSensorData.data[SensorData::angleX] + p.bodyRotation * oscillation),
      -(theFilteredSensorData.data[SensorData::angleY] + p.bodyOriginTilt), 0.);
    double leftAirFactor = leftActive ? fabs(oscillation) : 0.;
    double rightAirFactor = rightActive ? fabs(oscillation) : 0.;
    if(p.balanceAirFoot)
    {
      leftAirFactor = 0;
      rightAirFactor = 0;
    }

    leftLeg.rotation = RotationMatrix(Vector3<>(
      p.footOriginRotation.x + p.footRotation * oscillation + maxBalanceRotation.limit(rotationError.x * p.balanceFootRotation.x), 
      p.footOriginRotation.y + maxBalanceRotation.limit(rotationError.y * p.balanceFootRotation.y), 
      p.footOriginRotation.z + leftRotationZ) * (1. - leftAirFactor) + airFootRotation * leftAirFactor);
    rightLeg.rotation = RotationMatrix(Vector3<>(
      -p.footOriginRotation.x + p.footRotation * oscillation + maxBalanceRotation.limit(rotationError.x * p.balanceFootRotation.x), 
      p.footOriginRotation.y + maxBalanceRotation.limit(rotationError.y * p.balanceFootRotation.y), 
      -p.footOriginRotation.z + rightRotationZ) * (1. - rightAirFactor) + airFootRotation * rightAirFactor);

    // use body rotation
    Pose3D left(RotationMatrix(Vector3<>(
      p.bodyRotation * oscillation + maxBalanceRotation.limit(rotationError.x * p.balanceBodyRotation.x), 
      p.bodyOriginTilt + maxBalanceRotation.limit(rotationError.y * p.balanceBodyRotation.y), 
      0.)));
    Pose3D right(RotationMatrix(Vector3<>(
      p.bodyRotation * oscillation + maxBalanceRotation.limit(rotationError.x * p.balanceBodyRotation.x), 
      p.bodyOriginTilt + maxBalanceRotation.limit(rotationError.y * p.balanceBodyRotation.y), 
      0.)));
    left.conc(leftLeg);
    right.conc(rightLeg);

    // add arm balancing
    static const Range<double> maxBalanceArmRotation(-1, 1);
    leftArm.x += maxBalanceArmRotation.limit(rotationError.x * p.balanceArm.x);
    leftArm.y += maxBalanceArmRotation.limit(rotationError.y * p.balanceArm.y);
    rightArm.x += maxBalanceArmRotation.limit(rotationError.x * p.balanceArm.x);
    rightArm.y += maxBalanceArmRotation.limit(rotationError.y * p.balanceArm.y);

    // calculate desired CoM position
    CoMSet desiredCoMSet, correctedCoMSet;
    calculateDesiredCoM(std::max<>(leftLift, rightLift), coMOffset, Vector3<>(p.bodyRotation * oscillation, p.bodyOriginTilt, 0.), left, right, desiredCoMSet, leftActive ? leftFadeIn : rightFadeIn, leftActive ? leftMoveFadeIn : rightMoveFadeIn);
    calculateCorrectedCoM(oscillation, coMError, desiredCoMSet, correctedCoMSet);

    PLOT("module:WalkingEngine:bodyRotation", p.bodyRotation * oscillation);

    PLOT("module:WalkingEngine:oldDesiredCoMLX", oldDesiredCoMSet.left.x);
    PLOT("module:WalkingEngine:oldDesiredCoMLY", oldDesiredCoMSet.left.y);
    PLOT("module:WalkingEngine:oldDesiredCoMLZ", oldDesiredCoMSet.left.z);

    PLOT("module:WalkingEngine:oldDesiredCoMRX", oldDesiredCoMSet.right.x);
    PLOT("module:WalkingEngine:oldDesiredCoMRY", oldDesiredCoMSet.right.y);
    PLOT("module:WalkingEngine:oldDesiredCoMRZ", oldDesiredCoMSet.right.z);
    
    PLOT("module:WalkingEngine:measuredCoMLX", measuredCoMSet.left.x);
    PLOT("module:WalkingEngine:measuredCoMLY", measuredCoMSet.left.y);
    PLOT("module:WalkingEngine:measuredCoMLZ", measuredCoMSet.left.z);

    PLOT("module:WalkingEngine:measuredCoMRX", measuredCoMSet.right.x);
    PLOT("module:WalkingEngine:measuredCoMRY", measuredCoMSet.right.y);
    PLOT("module:WalkingEngine:measuredCoMRZ", measuredCoMSet.right.z);

    PLOT("module:WalkingEngine:desiredCoMLX", desiredCoMSet.left.x);
    PLOT("module:WalkingEngine:desiredCoMLY", desiredCoMSet.left.y);
    PLOT("module:WalkingEngine:desiredCoMLZ", desiredCoMSet.left.z);
    
    PLOT("module:WalkingEngine:desiredCoMRX", desiredCoMSet.right.x);
    PLOT("module:WalkingEngine:desiredCoMRY", desiredCoMSet.right.y);
    PLOT("module:WalkingEngine:desiredCoMRZ", desiredCoMSet.right.z);
    
    PLOT("module:WalkingEngine:correctedCoMLX", correctedCoMSet.left.x);
    PLOT("module:WalkingEngine:correctedCoMLY", correctedCoMSet.left.y);
    PLOT("module:WalkingEngine:correctedCoMLZ", correctedCoMSet.left.z);

    PLOT("module:WalkingEngine:correctedCoMRX", correctedCoMSet.right.x);
    PLOT("module:WalkingEngine:correctedCoMRY", correctedCoMSet.right.y);
    PLOT("module:WalkingEngine:correctedCoMRZ", correctedCoMSet.right.z);
    
    // create joint angles from desired CoM position, foot targets, and arm angles
    setJoints(oscillation, correctedCoMSet, leftArm, rightArm, left, right, walkingEngineOutput);

    PLOT("module:WalkingEngine:leftX", left.translation.x - p.footOrigin.x);
    PLOT("module:WalkingEngine:leftY", left.translation.y - p.footOrigin.y);
    PLOT("module:WalkingEngine:leftZ", left.translation.z - p.footOrigin.z);

    PLOT("module:WalkingEngine:rightX", right.translation.x - p.footOrigin.x);
    PLOT("module:WalkingEngine:rightY", right.translation.y + p.footOrigin.y);
    PLOT("module:WalkingEngine:rightZ", right.translation.z - p.footOrigin.z);

    // calculate odometry offset
    if(p.odometryUseInertiaMatrix)
    { // "measured"
      if(lastInertiaMatrix.translation.z != 0.)
      {
        Pose3D odometryOffset3D(lastInertiaMatrix);
        odometryOffset3D.conc(theInertiaMatrix.inertiaOffset);
        odometryOffset3D.conc(theInertiaMatrix.invert());            
        walkingEngineOutput.odometryOffset.translation.x = odometryOffset3D.translation.x * p.odometryScale.translation.x;
        walkingEngineOutput.odometryOffset.translation.y = odometryOffset3D.translation.y * p.odometryScale.translation.y;
        walkingEngineOutput.odometryOffset.rotation = odometryOffset3D.rotation.getZAngle() * p.odometryScale.rotation;
      }
    }
    else
    { // theoretical odometry offset
      if(!wasActive)
      {
        lastLeft = left;
        lastRight = right;
      }
      const Pose3D& activeTarget(leftActive ? left : right);
      const Pose3D& activateLastTarget(leftActive ? lastLeft : lastRight);
      const Pose3D& inactiveTarget(leftActive ? right : left);
      const Pose3D& inactivateLastTarget(leftActive ? lastRight : lastLeft);
      walkingEngineOutput.odometryOffset.translation.x = (activeTarget.translation.x - activateLastTarget.translation.x - (inactiveTarget.translation.x - inactivateLastTarget.translation.x)) * .5 * p.odometryScale.translation.x;
      walkingEngineOutput.odometryOffset.translation.y = (activeTarget.translation.y - activateLastTarget.translation.y - (inactiveTarget.translation.y - inactivateLastTarget.translation.y)) * .5 * p.odometryScale.translation.y;
      walkingEngineOutput.odometryOffset.rotation = (activeTarget.rotation.getZAngle() - activateLastTarget.rotation.getZAngle() - (inactiveTarget.rotation.getZAngle() - inactivateLastTarget.rotation.getZAngle())) * .5 * p.odometryScale.rotation;
      lastLeft = left;
      lastRight = right;
    }

    // remove odometry offset from walking target
    if(walkTarget != Pose2D())
      walkTarget -= walkingEngineOutput.odometryOffset;

    //
    walkingEngineOutput.instability = instability.getAverage();
    PLOT("module:WalkingEngine:instability", walkingEngineOutput.instability );
    if(walkingEngineOutput.instability >= p.balanceMaxInstability) // capitulation
      enforceStandTime = theFrameInfo.time + p.balanceMaxInstabilityStandTime;

    // maybe finish an optimization run
    if(optimize || continuousOptimize)
      if(theFrameInfo.time >= optimizationTime || walkingEngineOutput.instability >= p.balanceMaxInstability)
      {
        if(!optimization.isRated())
        {
          optimization.setFitness(walkingEngineOutput.instability);
          if(optimization.getBestFitness() == walkingEngineOutput.instability)
            bestParameters = p;
        }
        if(!continuousOptimize)
          enforceStandTime = theFrameInfo.time + p.balanceMaxInstabilityStandTime;
        if(theFrameInfo.time >= enforceStandTime && continuousOptimize)
        {
          updateOptimization();
          optimizationTime = theFrameInfo.time + 5000;
        }
      }

    // store last activation state
    wasActive = true;
    wasLeftActive = leftActive;
    wasRightActive = rightActive;
  }
  else
    wasActive = false;    

  // set further informations
  setMaxSpeeds(walkingEngineOutput);
  walkingEngineOutput.positionInWalkCycle = phase;
  walkingEngineOutput.isLeavingPossible = isLeavingPossible;
  walkingEngineOutput.enforceStand = theFrameInfo.time < enforceStandTime;

  //
  lastIterationTime = theFrameInfo.time;
  lastInertiaMatrix = theInertiaMatrix;

  MODIFY("module:WalkingEngine:walkTarget", walkTarget);
}

void WalkingEngine::calculateStepSize(bool left, Pose2D& resultingSpeed)
{
  Step& step(left ? leftStep : rightStep);
  Step& otherStep(left ? rightStep : leftStep);

  // get last speed
  Pose2D lastSpeed(step.rotation + otherStep.rotation, step.originalSize.x + otherStep.originalSize.x, step.originalSize.y + otherStep.originalSize.y);
  lastSpeed.translation *= 1000. / p.stepDuration;
  lastSpeed.rotation *= 1000. / p.stepDuration;

  // choose target speed
  Pose2D targetSpeed(theMotionSelection.walkRequest.speed);
  bool usingWalkTarget = false;
  Pose2D walkTarget(this->walkTarget);
  if(walkTarget != Pose2D())
  {
    usingWalkTarget = true;

    // remove pending odometry offset from walk target
    walkTarget -= Pose2D(otherStep.rotation * 0.5 * p.odometryScale.rotation, 
      otherStep.size.x * 0.5 * p.odometryScale.translation.x, otherStep.size.y * 0.5 * p.odometryScale.translation.y);

    targetSpeed = walkTarget;
    targetSpeed.translation *= 1000. / p.stepDuration;
    targetSpeed.rotation *= 1000. / p.stepDuration;
    targetSpeed.translation.x *= 2.;
  }

  // max speed clipping
  Pose2D maxSpeed(p.maxSpeed);
  if(targetSpeed.translation.x < 0)
    maxSpeed.translation.x = -p.maxBackwardSpeed;
  if(targetSpeed.translation.y == 0 && targetSpeed.rotation == 0 && targetSpeed.translation.x > 0 && p.maxDashSpeed.translation.x > maxSpeed.translation.x)
    maxSpeed.translation.x = p.maxDashSpeed.translation.x;
  if(targetSpeed.translation.x == 0 && targetSpeed.rotation == 0 && p.maxDashSpeed.translation.y > maxSpeed.translation.y)
    maxSpeed.translation.y = p.maxDashSpeed.translation.y;
  if(targetSpeed.translation.x == 0 && targetSpeed.translation.y == 0 && p.maxDashSpeed.rotation > maxSpeed.rotation)
    maxSpeed.rotation = p.maxDashSpeed.rotation;
  if(!p.maxSpeedEllipsoidClipping)
  {
    targetSpeed.translation.x = Range<double>(-maxSpeed.translation.x, maxSpeed.translation.x).limit(targetSpeed.translation.x);
    targetSpeed.translation.y = Range<double>(-maxSpeed.translation.y, maxSpeed.translation.y).limit(targetSpeed.translation.y);
    targetSpeed.rotation = Range<double>(-maxSpeed.rotation, maxSpeed.rotation).limit(targetSpeed.rotation);
  }
  else // clip target speeds to a "three-dimensional ellipse"
  {
    double ratioY = maxSpeed.translation.x / maxSpeed.translation.y;
    double ratioRot = maxSpeed.translation.x / maxSpeed.rotation;
    Vector3<> tempSpeed(targetSpeed.translation.x, targetSpeed.translation.y * ratioY, targetSpeed.rotation * ratioRot);
    double speedVectorLength = tempSpeed.abs();
    if(speedVectorLength > maxSpeed.translation.x)
    {
      // normalize "manually", because abs() was already calculated
      tempSpeed /= speedVectorLength;
      tempSpeed *= maxSpeed.translation.x;
      targetSpeed.translation.x = tempSpeed.x;
      targetSpeed.translation.y = tempSpeed.y / ratioY;
      targetSpeed.rotation = tempSpeed.z / ratioRot;
    }
  }

  // use the the target speed for this foot
  step.size.x = targetSpeed.translation.x;
  step.size.y = targetSpeed.translation.y;  
  step.size.z = p.stepLift;
  step.rotation = targetSpeed.rotation;
  step.liftOffset = p.liftOffset;

  // start without movement and foot lift
  if(!wasActive)
  {
    step.size.x = step.size.y = step.size.z = step.rotation = 0.;
    step.liftOffset = Vector3<>();
  }

  // avoid immediate step size sign changes
  if(otherStep.originalSize.x != 0. && sgn(step.size.x) != sgn(otherStep.originalSize.x))
    step.size.x = 0.;
  if(otherStep.originalSize.y != 0. && sgn(step.size.y) != sgn(otherStep.originalSize.y))
    step.size.y = 0.;
  if(otherStep.rotation != 0. && sgn(step.rotation) != sgn(otherStep.rotation))
    step.rotation = 0.;

  // just move the outer foot, while walking sideways or while rotating
  if((step.size.y < 0. && left) || (step.size.y > 0. && !left))
    step.size.y = 0.;
  if((step.rotation < 0. && left) || (step.rotation > 0. && !left))
    step.rotation = 0.;

  // clip step sizes according to max speed + 10% tolerance (because of the balanceStepCorrection)
  if(step.size.x > 0. && step.size.x > maxSpeed.translation.x * 1.1)
    step.size.x = maxSpeed.translation.x * 1.1;
  else if(step.size.x < 0. && step.size.x < maxSpeed.translation.x * -1.1)
    step.size.x = maxSpeed.translation.x * -1.1;
  if(step.size.y > maxSpeed.translation.y * 1.1)
    step.size.y = maxSpeed.translation.y * 1.1;
  else if(step.size.y < maxSpeed.translation.y * -1.1)
    step.size.y = maxSpeed.translation.y * -1.1;
  if(step.rotation > maxSpeed.rotation * 1.1)
    step.rotation = maxSpeed.rotation * 1.1;
  else if(step.rotation < maxSpeed.rotation * -1.1)
    step.rotation = maxSpeed.rotation * -1.1;

  // clip step size to max speed change (while accelerating)
  if(step.size.x > 0 && step.size.x > lastSpeed.translation.x && step.size.x - lastSpeed.translation.x > p.maxSpeedChange.translation.x * (p.stepDuration / 1000.))
    step.size.x = lastSpeed.translation.x + p.maxSpeedChange.translation.x * (p.stepDuration / 1000.);
  if(step.size.x < 0 && step.size.x < lastSpeed.translation.x && lastSpeed.translation.x - step.size.x > p.maxSpeedChange.translation.x * (p.stepDuration / 1000.))
    step.size.x = lastSpeed.translation.x - p.maxSpeedChange.translation.x * (p.stepDuration / 1000.);
  if(step.size.y > 0 && step.size.y > lastSpeed.translation.y && step.size.y - lastSpeed.translation.y > p.maxSpeedChange.translation.y * (p.stepDuration / 1000.))
    step.size.y = lastSpeed.translation.y + p.maxSpeedChange.translation.y * (p.stepDuration / 1000.);
  if(step.size.y < 0 && step.size.y < lastSpeed.translation.y && lastSpeed.translation.y - step.size.y > p.maxSpeedChange.translation.y * (p.stepDuration / 1000.))
    step.size.y = lastSpeed.translation.y - p.maxSpeedChange.translation.y * (p.stepDuration / 1000.);
  if(step.rotation > 0 && step.rotation > lastSpeed.rotation && step.rotation - lastSpeed.rotation > p.maxSpeedChange.rotation * (p.stepDuration / 1000.))
    step.rotation = lastSpeed.rotation + p.maxSpeedChange.rotation * (p.stepDuration / 1000.);
  if(step.rotation < 0 && step.rotation < lastSpeed.rotation && lastSpeed.rotation - step.rotation > p.maxSpeedChange.rotation * (p.stepDuration / 1000.))
    step.rotation = lastSpeed.rotation - p.maxSpeedChange.rotation * (p.stepDuration / 1000.);

  // scale step size to step duration
  step.size /= 1000. / p.stepDuration;
  step.rotation /= 1000. / p.stepDuration;
  step.size.x /= 2.; // yes, since we perform two halfsteps in x-direction
  step.originalSize = step.size;

  // add next step correction
  step.size.x += (otherStep.size - otherStep.originalSize).x * p.balanceNextStep.x;
  step.size.y += (otherStep.size - otherStep.originalSize).y * -p.balanceNextStep.y; // mirrored

  // clip step size to walk target
  if(usingWalkTarget)
  {
    if((step.size.x > 0. && walkTarget.translation.x > 0. && step.size.x > walkTarget.translation.x) ||
       (step.size.x < 0. && walkTarget.translation.x < 0. && step.size.x < walkTarget.translation.x) )
      step.size.x = walkTarget.translation.x;
    if((step.size.y > 0. && walkTarget.translation.y > 0. && step.size.y > walkTarget.translation.y) ||
       (step.size.y < 0. && walkTarget.translation.y < 0. && step.size.y < walkTarget.translation.y) )
      step.size.y = walkTarget.translation.y;
    if((step.rotation > 0. && walkTarget.rotation > 0. && step.rotation > walkTarget.rotation) ||
       (step.rotation < 0. && walkTarget.rotation < 0. && step.rotation < walkTarget.rotation) )
      step.rotation = walkTarget.rotation;
  }

  //
  isLeavingPossible = fabs(leftStep.size.x) < 10. && fabs(leftStep.size.y) < 10. && fabs(leftStep.rotation) < 0.05 && 
    fabs(rightStep.size.x) < 10. && fabs(rightStep.size.y) < 10. && fabs(rightStep.rotation) < 0.05;

  // get resulting speed from step size
  resultingSpeed = Pose2D(step.rotation + otherStep.rotation, step.originalSize.x + otherStep.originalSize.x, step.originalSize.y + otherStep.originalSize.y);
  resultingSpeed.translation *= 1000. / p.stepDuration;
  resultingSpeed.rotation *= 1000. / p.stepDuration;
}

void WalkingEngine::setArmJoints(const Vector2<>& leftRotation, const Vector2<>& rightRotation, JointRequest& jointRequest)
{
  for(int side = 0; side < 2; side++)
  {
    const Vector2<>& rotation(side == 0 ? leftRotation : rightRotation);
    JointData::Joint firstJoint = side == 0 ? JointData::armLeft0 : JointData::armRight0;
    jointRequest.angles[firstJoint + 0] = rotation.y;
    jointRequest.angles[firstJoint + 1] = -pi_2 + rotation.x;
    jointRequest.angles[firstJoint + 2] = 0;
    jointRequest.angles[firstJoint + 3] = -pi_2 - jointRequest.angles[firstJoint + 1];
  }
}

void WalkingEngine::setLegJoints(const Pose3D& leftTarget, const Pose3D& rightTarget, const double& ratio, JointRequest& jointRequest)
{
  InverseKinematic::calcLegJoints(leftTarget, rightTarget, jointRequest, theRobotDimensions, ratio);

  jointRequest.jointHardness.hardness[JointData::legLeft0] = p.legHardness;
  jointRequest.jointHardness.hardness[JointData::legLeft1] = p.legHardness;
  jointRequest.jointHardness.hardness[JointData::legLeft2] = p.legHardness;
  jointRequest.jointHardness.hardness[JointData::legLeft3] = p.legHardness;
  jointRequest.jointHardness.hardness[JointData::legLeft4] = p.legHardness;
  jointRequest.jointHardness.hardness[JointData::legLeft5] = p.leg5Hardness;
  jointRequest.jointHardness.hardness[JointData::legRight0] = p.legHardness;
  jointRequest.jointHardness.hardness[JointData::legRight1] = p.legHardness;
  jointRequest.jointHardness.hardness[JointData::legRight2] = p.legHardness;
  jointRequest.jointHardness.hardness[JointData::legRight3] = p.legHardness;
  jointRequest.jointHardness.hardness[JointData::legRight4] = p.legHardness;
  jointRequest.jointHardness.hardness[JointData::legRight5] = p.leg5Hardness;
}

void WalkingEngine::setHeadJoints(JointRequest& jointRequest)
{
  jointRequest.angles[JointData::headPan] = theHeadJointRequest.pan;
  jointRequest.angles[JointData::headTilt] = theHeadJointRequest.tilt;
}

void WalkingEngine::setHeadJoints(float pan, float tilt, JointRequest& jointRequest)
{
  jointRequest.angles[JointData::headPan] = pan;
  jointRequest.angles[JointData::headTilt] = tilt;
}

void WalkingEngine::setMaxSpeeds(WalkingEngineOutput& walkingEngineOutput)
{
  walkingEngineOutput.maxSpeed = p.maxSpeed;
  walkingEngineOutput.maxBackwardSpeed = p.maxBackwardSpeed;
}

