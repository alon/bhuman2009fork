/**
* @file SensorModel.h
*
* Declares an abstract base class for dífferent sensor models.
*
* @author <a href="mailto:Tim.Laue@dfki.de">Tim Laue</a>
*/

#ifndef _SensorModel_h_
#define _SensorModel_h_

class SelfLocatorParameter;

/**
* @class SensorModel
* Abstract base class
*/
class SensorModel
{
protected:
  /** Parameters of the self locator. Might become changed towards local parameters.*/
  const SelfLocatorParameter& theSelfLocatorParameter;
  /** Reference to frame information */
  const FrameInfo& theFrameInfo;
  /** Reference to field information */
  const FieldDimensions& theFieldDimensions;
  /** Reference to camera matrix */
  const CameraMatrix& theCameraMatrix;
  /** Module for checking perceptions before their use. */
  const PerceptValidityChecker& thePerceptValidityChecker;

  /** Computes a weighting for the angle difference between model and observation
  * @param measuredAngle The measured value
  * @param modelPosition The position of the sensed object in the global frame of reference
  * @param robotPose The pose of the robot (or the sample)
  * @param standardDeviation The standard deviation to become applied
  * @param bestPossibleWeighting Used for scaling to [0..1]
  * @return A weighting [0..1]
  */
  double computeAngleWeighting(double measuredAngle, const Vector2<double>& modelPosition,
    const Pose2D& robotPose, double standardDeviation, double bestPossibleWeighting)
  {
    const double modelAngle = Geometry::angleTo(robotPose, modelPosition);
    return gaussianProbability(fabs(modelAngle-measuredAngle), standardDeviation) / bestPossibleWeighting;
  }

  /** Computes a weighting for the distance difference (described as angle) between model and observation
  * @param measuredDistanceAsAngle The angle between the vertical axis through the robot and the ray to the object
  * @param modelPosition The position of the sensed object in the global frame of reference
  * @param robotPose The pose of the robot (or the sample)
  * @param cameraZ The height of the camera
  * @param standardDeviation The standard deviation to become applied
  * @param bestPossibleWeighting Used for scaling to [0..1]
  * @return A weighting [0..1]
  */
  double computeDistanceWeighting(double measuredDistanceAsAngle, const Vector2<double>& modelPosition,
    const Pose2D& robotPose, double cameraZ, double standardDeviation, double bestPossibleWeighting)
  {
    const double modelDistance = (robotPose.translation - modelPosition).abs();
    const double modelDistanceAsAngle = (pi_2 - atan2(cameraZ,modelDistance));
    return gaussianProbability(fabs(modelDistanceAsAngle-measuredDistanceAsAngle), 
      standardDeviation) / bestPossibleWeighting;
  }

public:
  /** Possible observations. */
  class Observation
  {
  public:
    enum Type
    {
      POINT,
      CORNER,
      GOAL_POST,
      CENTER_CIRCLE
    } type; /**< The type of the observation. */
    int index; /**< A hint how to find the observation in the corresponding data structure. */

    /** Default constructor. */
    Observation() {}

    /** 
    * Constructor. 
    * @param type The type of the observation.
    * @param index A hint how to find the observation in the corresponding data structure.
    */
    Observation(Type type, int index) : type(type), index(index) {}
  };

  const Observation::Type type; /**< The observation type processed by this sensor model. */

  /** Different result of computeWeightings function*/
  enum SensorModelResult
  {
    NO_SENSOR_UPDATE = 0,         // No weighting for any sample
    PARTIAL_SENSOR_UPDATE,        // Weightings for some samples
    FULL_SENSOR_UPDATE            // Weightings for all samples
  };

  /** Constructor. */
  SensorModel(const SelfLocatorParameter& selfLocatorParameter,
    const FrameInfo& frameInfo, const FieldDimensions& fieldDimensions,
    const CameraMatrix& cameraMatrix, const PerceptValidityChecker& perceptValidityChecker,
    Observation::Type type) :
    theSelfLocatorParameter(selfLocatorParameter), theFrameInfo(frameInfo),
      theFieldDimensions(fieldDimensions), theCameraMatrix(cameraMatrix),
      thePerceptValidityChecker(perceptValidityChecker), type(type)
    {}

  /** Destructor */
  virtual ~SensorModel() {}

  /** Function for computing weightings for a sample set.
  * @param samples The samples (not changed by this function
  * @param selectedIndices The indices of the selected observations.
  * @param weightings List of weightings. -1 means: no update
  * @return An overall result of the computation
  */
  virtual SensorModelResult computeWeightings(const SampleSet<SelfLocatorSample>& samples,
    const std::vector<int>& selectedIndices, std::vector<double>& weightings) = 0;
};


#endif
