/**
* @file ParticleFilterBallLocator.h
* Contains a BallLocator implementation using a particle filter
*
* @author <a href="mailto:timlaue@informatik.uni-bremen.de">Tim Laue</a>
*/

#ifndef Particle_Filter_Ball_Locator_H_
#define Particle_Filter_Ball_Locator_H_

#include "Tools/Math/GaussianDistribution2D.h"
#include "Tools/Math/GaussianTable.h"
#include "Tools/Module/Module.h"
#include "Tools/SampleSet.h"
#include "Representations/Infrastructure/CameraInfo.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Configuration/FieldDimensions.h"
#include "Representations/Perception/BallPercept.h"
#include "Representations/Perception/CameraMatrix.h"
#include "Representations/Modeling/BallModel.h"
#include "Representations/MotionControl/OdometryData.h"
#include "Representations/Modeling/BallLocatorSampleSet.h"
#include "Representations/MotionControl/WalkingEngineOutput.h"
#include <deque>


MODULE(ParticleFilterBallLocator)
  REQUIRES(BallPercept)
  REQUIRES(CameraMatrix)
  REQUIRES(OdometryData)
  REQUIRES(FieldDimensions)
  REQUIRES(RobotPose)
  REQUIRES(CameraInfo)
  REQUIRES(FrameInfo)
  REQUIRES(WalkingEngineOutput)
  PROVIDES_WITH_MODIFY_AND_OUTPUT_AND_DRAW(BallModel)
  PROVIDES(BallLocatorSampleSet)
END_MODULE


/**
* @class ParticleFilterBallLocatorParameters
* A collection for all parameters
*/
class ParticleFilterBallLocatorParameters: public Streamable
{
private:
  /**
  * The method makes the object streamable.
  * @param in The stream from which the object is read 
  * @param out The stream to which the object is written 
  */
  virtual void serialize(In* in, Out* out)
  {  
    STREAM_REGISTER_BEGIN();
      STREAM(angleStandardDeviation);
      STREAM(closeBallDistanceStandardDeviation);
      STREAM(farBallDistanceStandardDeviation);
      STREAM(closeThreshold);
      STREAM(farThreshold);
      STREAM(alphaSlow);
      STREAM(alphaFast);
      STREAM(scaleFactorVelocityVariance);
      STREAM(scaleFactorOdometryVariance);
      STREAM(templateSpeedDecreaseDistance);
      STREAM(templateSpeedDecreaseFactor);
      STREAM(ballMotionProbability);
      STREAM(speedPercentageAfterOneSecond);
      STREAM(maxVelocity);
      STREAM(minOdoTranslation);
      STREAM(minOdoRotation);
      STREAM(robotBoxSizeX);
      STREAM(robotBoxSizeY);
      STREAM(ballCollisionCheckSquareDistance);
    STREAM_REGISTER_FINISH();
  }

public:
  /** Standard constructor*/
  ParticleFilterBallLocatorParameters(): angleStandardDeviation(0.5*1000),
                                         closeBallDistanceStandardDeviation(100.0),
                                         farBallDistanceStandardDeviation(400.0),
                                         closeThreshold(200.0), farThreshold(2500.0),
                                         alphaSlow(0.2), alphaFast(0.3), scaleFactorVelocityVariance(0.4),
                                         scaleFactorOdometryVariance(3.0), templateSpeedDecreaseDistance(420.0),
                                         templateSpeedDecreaseFactor(5.0), ballMotionProbability(0.2),
                                         speedPercentageAfterOneSecond(70),
                                         maxVelocity(2000), minOdoTranslation(10), minOdoRotation(fromDegrees(5)),
                                         robotBoxSizeX(80), robotBoxSizeY(100)
  {
  }

  double angleStandardDeviation;
  double closeBallDistanceStandardDeviation;
  double farBallDistanceStandardDeviation;
  double closeThreshold;
  double farThreshold;
  double alphaSlow;
  double alphaFast;
  double scaleFactorVelocityVariance;
  double scaleFactorOdometryVariance;
  double templateSpeedDecreaseDistance;
  double templateSpeedDecreaseFactor;
  double ballMotionProbability;
  int speedPercentageAfterOneSecond;
  int maxVelocity;
  double minOdoTranslation;
  double minOdoRotation;
  int robotBoxSizeX;
  int robotBoxSizeY;
  int ballCollisionCheckSquareDistance; //computed at runtime
};


/**
* @class ParticleFilterBallLocator
* A BallLocator using a particle filter
*/
class ParticleFilterBallLocator : public ParticleFilterBallLocatorBase
{
public:
  /** Constructor */
  ParticleFilterBallLocator();

  /** Provides our own ball representation
  * @param ballModel The ball representation which is updated by this module
  */
  void update(BallModel& ballModel);

  /** Provides the SampleSet representation
  * @param sampleSet The SampleSet representation which is updated by this module
  */
  void update(BallLocatorSampleSet& sampleSet);

  /** Initialization of things. Called after all modules have been constructed
   *  and before the first call to update.
   */
  void init();

private:
  /** prepares execution, initializes some values*/
  void preExecution();

  /** motion update step of particle filter localization*/
  void motionUpdate();

  /** Check, if ball sample collides with robot
  * @param s The sample
  */
  void clipSampleWithFeet(BallSample& s);

  /** motion update step of particle filter localization*/
  void sensorUpdate();

  /** resampling step of particle filter localization*/
  void resampling();

  /** Computes a new sample
  * @return The sample
  */
  BallSample generateNewSample();

  /** Sets the member values of the BallModel
  * @param ballModel A reference to the BallModel*/
  void modelGeneration(BallModel& ballModel);

   /** Sets some values after everything else of this module was executed
  * @param ballModel A reference to the BallModel*/
  void postExecution(BallModel& ballModel);

  /** Draws the particles*/
  void drawSamples();

  /** Draws the particles*/
  void drawSamplesToImage();

  /** Collects data and finally prints covariance values*/
  void uncertaintyCalibration();

  typedef SampleSet<BallSample> BallSampleSet;  /** Definition of type for sample set*/
  BallSampleSet* ballSamples;                                 /** Container for all samples*/
  double averageWeighting;                                   /** The current average weighting of a sample*/
  double slowWeighting;
  double fastWeighting;
  int numberOfGeneratedSamples;
  unsigned lastExecution;                                    /** time stamp*/
  unsigned lastSeenBallTime;                                 /** time stamp*/
  unsigned lastCheatingTime;                                 /** time stamp*/
  Pose2D lastOdometry;                                       /** save last odometry state*/
  Pose2D appliedOdometryOffset;                              /** odometry offset used for the current frame*/
  Pose2D accumulatedOdometry;                                /** accumulated odometry */
  GaussianDistribution2D ballPosUncertainty;                 /** distribution describing the uncertainty of ball positions in images*/
  enum {NUM_OF_CALIBRATION_MEASUREMENTS = 500};
  double ballXMeasurements[NUM_OF_CALIBRATION_MEASUREMENTS];
  double ballYMeasurements[NUM_OF_CALIBRATION_MEASUREMENTS];
  bool calibrateUncertainty;
  int currentCalibrationMeasurement;
  ParticleFilterBallLocatorParameters parameters;            /** some parameters */
  GaussianTable gaussian;                                    /** precomputed gaussian values for distances*/  
  GaussianTable gaussianAngles;                              /** precomputed gaussian values for angles*/  
  int newSamplesNumberForDrawing;

  class BufferedPercept
  {
  public:
    BufferedPercept() {}
    BufferedPercept(const BallPercept& bp, unsigned timeStamp, const OdometryData& odometryData):
      pos(bp.relativePositionOnField), timeStamp(timeStamp), odometryData(odometryData)
    {}

    Vector2<double> getOdometryCorrectedPosition(const OdometryData& odoCurrent)
    {
      Pose2D offset = odoCurrent - odometryData;
      return offset*pos;
    }

    Vector2<double> pos;
    unsigned timeStamp;
    OdometryData odometryData;
  };

  std::deque<BufferedPercept> ballPerceptBuffer;             /** a buffer for the last ball percepts*/
};

#endif
