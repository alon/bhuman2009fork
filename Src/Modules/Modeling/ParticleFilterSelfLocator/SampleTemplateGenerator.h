/**
* @file SampleTemplateGenerator.h
*
* This file declares a submodule that generates robot positions from percepts.
*
* @author <a href="mailto:Tim.Laue@dfki.de">Tim Laue</a>
*/

#ifndef __SampleTemplateGenerator_h_
#define __SampleTemplateGenerator_h_

#include "Representations/Configuration/FieldDimensions.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/MotionControl/OdometryData.h"
#include "Representations/Perception/GoalPercept.h"
#include "Tools/RingBuffer.h"


/**
* @class SampleTemplate
* Robot position generated from perceptions
*/
class SampleTemplate : public Pose2D
{
public:
  /** Constructor */
  SampleTemplate():Pose2D(),timestamp(0) {}

  /** Constructor */
  SampleTemplate(const Pose2D& pose):Pose2D(pose),timestamp(0) {}

  /** Timestamp of visual input for construction of this template */
  unsigned timestamp;
};


/**
* @class SampleTemplateGenerator
*
* A module for computing poses from percepts
*/
class SampleTemplateGenerator
{
private:
  const GoalPercept& theGoalPercept;
  const FrameInfo& theFrameInfo;
  const FieldDimensions& theFieldDimensions;
  const OdometryData& theOdometryData;
  const double& standardDeviationGoalpostSampleBearingDistance;
  const double& standardDeviationGoalpostSampleSizeDistance;
  const bool& clipTemplateGeneration;
  const Range<double>& clipTemplateGenerationRangeX;
  const Range<double>& clipTemplateGenerationRangeY;

  class FullGoal
  {
  public:
    Vector2<double> seenLeftPosition;
    Vector2<double> realLeftPosition;
    Vector2<double> seenRightPosition;
    Vector2<double> realRightPosition;
    int timestamp;
    Pose2D odometry;
  };

  class KnownGoalpost
  {
  public:
    Vector2<double> seenPosition;
    Vector2<double> realPosition;
    int timestamp;
    Pose2D odometry;
  };

  class UnknownGoalpost
  {
  public:
    Vector2<double> seenPosition;
    Vector2<double> realPositions[2];
    int timestamp;
    Pose2D odometry;
  };

  enum {MAX_PERCEPTS = 10, MAX_TIME_TO_KEEP = 5000};
  RingBuffer<FullGoal,MAX_PERCEPTS> fullGoals;
  RingBuffer<KnownGoalpost,MAX_PERCEPTS> knownGoalposts;
  RingBuffer<UnknownGoalpost,MAX_PERCEPTS> unknownGoalposts;
  Vector2<double> realPostPositions[GoalPercept::NUMBER_OF_GOAL_POSTS];

  template<typename T>
  void removeOldPercepts(RingBuffer<T, MAX_PERCEPTS>& buffer);

  SampleTemplate generateTemplateFromFullGoal(const FullGoal& goal) const;

  SampleTemplate generateTemplateFromPosition(const Vector2<double>& posSeen, 
    const Vector2<double>& posReal, const Pose2D& postOdometry) const;

  SampleTemplate generateRandomTemplate() const;

  /** 
  * The function checks whether a position is insided the configured area to which template generation should be clipped.
  * @param pos A point on the field
  * @return true if the point is inside this area
  */
  bool checkTemplateClipping(const Vector2<double> pos) const;

public:
  SampleTemplateGenerator(const GoalPercept& goalPercept, const FrameInfo& frameInfo,
    const FieldDimensions& fieldDimensions, const OdometryData& odometryData,
    const double& standardDeviationGoalpostSampleBearingDistance,
    const double& standardDeviationGoalpostSampleSizeDistance,
    const bool& clipTemplateGeneration,
    const Range<double>& clipTemplateGenerationRangeX,
    const Range<double>& clipTemplateGenerationRangeY);

  void init();

  void bufferNewPerceptions();

  SampleTemplate getNewTemplate();

  bool templatesAvailable();

  void draw();
};

#endif// __SampleTemplateGenerator_h_
