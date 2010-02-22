/** 
* @file Modules/MotionControl/WalkingEngine.h
* This file declares a module that creates the walking motions.
* @author <a href="mailto:allli@informatik.uni-bremen.de">Alexander Härtl</a>
* @author Colin Graf
*/

#ifndef WalkingEngine_H
#define WalkingEngine_H

#include "Tools/Module/Module.h"
#include "Representations/Configuration/RobotDimensions.h"
#include "Representations/Configuration/MassCalibration.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/SensorData.h"
#include "Representations/Sensing/RobotModel.h"
#include "Representations/MotionControl/MotionSelection.h"
#include "Representations/MotionControl/WalkingEngineStandOutput.h"
#include "Representations/MotionControl/WalkingEngineOutput.h"
#include "Representations/MotionControl/HeadJointRequest.h"
#include "Representations/Sensing/InertiaMatrix.h"
#include "Tools/Math/Pose3D.h"
#include "Tools/Optimization/ParticleSwarm.h"
#include "Tools/RingBuffer.h"
#include "Tools/RingBufferWithSum.h"

MODULE(WalkingEngine)
  REQUIRES(RobotDimensions)
  REQUIRES(MassCalibration)
  REQUIRES(FrameInfo)
  REQUIRES(MotionSelection)
  REQUIRES(RobotModel)
  REQUIRES(HeadJointRequest)
  REQUIRES(FilteredSensorData)
  REQUIRES(InertiaMatrix)
  PROVIDES_WITH_MODIFY(WalkingEngineOutput)
  PROVIDES_WITH_MODIFY(WalkingEngineStandOutput)
END_MODULE

class WalkingEngine : public WalkingEngineBase
{
public:
  /**
  * Default constructor.
  */
  WalkingEngine();

  /**
  * Describes the shape of an oscillation.
  */
  class OscillationShape : public Streamable
  {
  public:
    double sine;
    double sqrtSine;
    double linear;

    OscillationShape(const double& sine = 1., const double& sqrtSine = 0., const double& linear = 0.) : sine(sine), sqrtSine(sqrtSine), linear(linear) {}

    double getValue(double phase)
    {
      ASSERT(phase >= 0. && phase < 1.);
      double s = sin(phase * pi2);
      double ss = sqrt(fabs(s)) * sgn(s);
      double l;
      if(phase < 0.5)
      {
        if(phase < 0.25)
          l = phase * 4.;
        else
          l = 1. - (phase - 0.25) * 4.;
      }
      else
      {
        if(phase < 0.75)
          l = (phase - 0.5) * -4.;
        else
          l = -1. + (phase - 0.75) * 4.;
      }
      return (s * sine + ss * sqrtSine + l * linear) / (sine + sqrtSine + linear);
    }

  private:
    /**
    * The method makes the object streamable.
    * @param in The stream from which the object is read.
    * @param out The stream to which the object is written. 
    */
    virtual void serialize(In* in, Out* out)
    {  
      STREAM_REGISTER_BEGIN();
        STREAM(sine);
        STREAM(sqrtSine);
        STREAM(linear);
      STREAM_REGISTER_FINISH();
    }
  };

  /**
  * A collection of parameters for the walking engine.
  */
  class Parameters : public Streamable
  {
  public:
    /** Default constructor. */
    Parameters() {}

    double stepDuration;
    double stepLift;

    Vector3<> footOrigin;
    Vector3<> footOriginRotation;
    Vector3<> footCenter;
    double footRotation;

    int legHardness;
    int leg5Hardness;

    double bodyOriginTilt;
    double bodyRotation;

    Vector2<> armOriginRotation;
    Vector2<> armMoveRotation;

    double coMShift;
    double coMLift;
    OscillationShape coMShiftShape;
    Vector2<> coMSpeedOffset;
    Vector2<> coMAccelerationOffset;
    double coMTransferRatio;

    Vector3<> liftOffset;
    Vector2<> liftPhases; 

    Vector2<> movePhases;

    Pose2D maxSpeed;
    double maxBackwardSpeed;
    Pose2D maxDashSpeed;
    Pose2D maxSpeedChange;
    bool maxSpeedEllipsoidClipping;

    double measuredCoMGain;
    unsigned int measuredCoMDelay;

    Vector2<> balanceStep;
    Vector2<> balanceNextStep;
    Vector3<> balanceCoM;
    double balancePhaseAcceleration;
    double balancePhaseDeceleration;
    Vector2<> balanceBodyRotation;
    Vector2<> balanceFootRotation;
    Vector2<> balanceArm;
    double balanceMaxInstability;
    unsigned int balanceMaxInstabilityStandTime;
    bool balanceAirFoot;
    double balancePhaseOld;

    Pose2D odometryScale;
    bool odometryUseInertiaMatrix;

  private:
    /**
    * The method makes the object streamable.
    * @param in The stream from which the object is read.
    * @param out The stream to which the object is written. 
    */
    virtual void serialize(In* in, Out* out)
    {  
      STREAM_REGISTER_BEGIN();
        STREAM(stepDuration);
        STREAM(stepLift);

        STREAM(footOrigin);
        STREAM(footOriginRotation);
        STREAM(footCenter);      
        STREAM(footRotation); // * oscillation

        STREAM(legHardness);
        STREAM(leg5Hardness);

        STREAM(bodyOriginTilt);
        STREAM(bodyRotation); // * oscillation

        STREAM(armOriginRotation); 
        STREAM(armMoveRotation); // * oscillation

        STREAM(coMShift); // * oscillation
        STREAM(coMLift);  // * oscillation
        STREAM(coMShiftShape);
        STREAM(coMSpeedOffset);
        STREAM(coMAccelerationOffset);
        STREAM(coMTransferRatio);

        STREAM(liftOffset);
        STREAM(liftPhases); 

        STREAM(movePhases);

        STREAM(maxSpeed);
        STREAM(maxBackwardSpeed);
        STREAM(maxDashSpeed);
        STREAM(maxSpeedChange);
        STREAM(maxSpeedEllipsoidClipping);
        
        STREAM(measuredCoMGain);
        STREAM(measuredCoMDelay);

        STREAM(balanceStep);
        STREAM(balanceNextStep);
        STREAM(balanceCoM);
        STREAM(balancePhaseOld);
        STREAM(balancePhaseAcceleration);
        STREAM(balancePhaseDeceleration);
        STREAM(balanceBodyRotation);
        STREAM(balanceFootRotation);
        STREAM(balanceArm);
        STREAM(balanceMaxInstability);
        STREAM(balanceMaxInstabilityStandTime);
        STREAM(balanceAirFoot);

        STREAM(odometryScale);
        STREAM(odometryUseInertiaMatrix);

      STREAM_REGISTER_FINISH();
    }
  };

private:

  class Step
  {
  public:
    Step() : rotation(0.) {}
    Vector3<> size;
    Vector3<> originalSize;
    double rotation;
    Vector3<> liftOffset;
  };
  
  class CoMSet
  {
  public:
    CoMSet(const Vector3<>& left = Vector3<>(), const Vector3<>& right = Vector3<>(), const Vector2<>& bodyRotation = Vector2<>(), unsigned int timeStamp = 0) : left(left), right(right), bodyRotation(bodyRotation), timeStamp(timeStamp) {}
    Vector3<> left;
    Vector3<> right;
    Vector2<> bodyRotation;
    double timeStamp;
  };

  WalkingEngineStandOutput standOutput;
  RobotModel standRobotModel;
  double lastBodyOriginTilt;
  Vector3<> lastFootOrigin;
  Vector3<> lastFootOriginRotation;
  CoMSet standCoMSet;

  Parameters p;
  double phase; /**< The current position in walk phase. [0-1[ */
  bool wasActive; /**< Was this module active in the previous frame? */
  bool wasLeftActive;
  bool wasRightActive;
  Step leftStep;
  Step rightStep;
  Vector3<> bodyShift;
  unsigned int lastIterationTime; /**< The time stamp of last execution. */
  bool isLeavingPossible;
  Pose2D walkTarget;

  RingBuffer<CoMSet, 20> oldCoMSets;

  RingBufferWithSum<double, 100> instability;
  unsigned int enforceStandTime;

  ParticleSwarm optimization;
  unsigned int optimizationTime;
  Parameters bestParameters;

  Pose3D lastInertiaMatrix;

  Pose3D lastLeft;
  Pose3D lastRight;

  void init();

  /**
  * The central update method to generate the walking motion
  * @param walkingEngineOutput The WalkingEngineOutput (mainly the resulting joint angles)
  */
  void update(WalkingEngineOutput& walkingEngineOutput);

  /**
  * The update method to generate the standing motion from walkingParameters
  * @param standOutput The WalkingEngineStandOutput (mainly the resulting joint angles)
  */
  void update(WalkingEngineStandOutput& standOutput) { standOutput = this->standOutput; }

  /**
  * The method sets the joint angles (independently from the legs)
  * @param leftRotation The angle of armLeft0 and armLeft1
  * @param rightRotation The angle of armRight0 and armRight1
  * @param jointRequest The JointRequest in which the arm joints are set
  */
  void setArmJoints(const Vector2<>& leftRotation, const Vector2<>& rightRotation, JointRequest& jointRequest);

  void setLegJoints(const Pose3D& leftTarget, const Pose3D& rightTarget, const double& ratio, JointRequest& jointRequest);

  void setHeadJoints(JointRequest& jointRequest);
  void setHeadJoints(float pan, float tilt, JointRequest& jointRequest);

  void setMaxSpeeds(WalkingEngineOutput& walkingEngineOutput);

  void setJoints(const double& oscillation, const CoMSet& newCoMSet, const Vector2<>& leftArm, const Vector2<>& rightArm, Pose3D& left, Pose3D& right, JointRequest& jointRequest);

  void calculateStepSize(bool left, Pose2D& resultingSpeed);

  void calculateError(const CoMSet& measuredCoMSet, const CoMSet& oldDesiredCoMSet, Vector3<>& coMError, Vector3<>& rotationError);
  void calculateMeasuredCoM(const CoMSet& oldDesiredCoMSet, CoMSet& measuredCoMSet);
  void calculateOldDesiredCoM(CoMSet& oldDesiredCoMSet);
  void calculateDesiredCoM(const double& lift, const Vector3<>& coMOffset, const Vector3<>& bodyRotation, const Pose3D& left, const Pose3D& right, CoMSet& desiredCoMSet, const double& fadeIn, const double& moveFadeIn);
  bool calculateCorrectedPhase(const CoMSet& measuredCoMSet, const CoMSet& oldDesiredCoMSet);
  void calculateCorrectedCoM(const double& oscillation, const Vector3<>& error, const CoMSet& desiredCoMSet, CoMSet& correctedCoMSet);

  void initOptimization();
  void updateOptimization();
};

#endif // WalkingEngine_H
