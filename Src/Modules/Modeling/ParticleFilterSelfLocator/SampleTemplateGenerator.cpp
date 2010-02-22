/**
* @file SampleTemplateGenerator.cpp
*
* This file implements a submodule that generates robot positions from percepts.
*
* @author <a href="mailto:Tim.Laue@dfki.de">Tim Laue</a>
*/

#include "SampleTemplateGenerator.h"
#include "Tools/Math/Probabilistics.h"


SampleTemplateGenerator::SampleTemplateGenerator(const GoalPercept& goalPercept, const FrameInfo& frameInfo,
                                                 const FieldDimensions& fieldDimensions, 
                                                 const OdometryData& odometryData,
                                                 const double& standardDeviationGoalpostSampleBearingDistance,
                                                 const double& standardDeviationGoalpostSampleSizeDistance,
                                                 const bool& clipTemplateGeneration,
                                                 const Range<double>& clipTemplateGenerationRangeX,
                                                 const Range<double>& clipTemplateGenerationRangeY):
theGoalPercept(goalPercept), theFrameInfo(frameInfo),
theFieldDimensions(fieldDimensions), theOdometryData(odometryData),
standardDeviationGoalpostSampleBearingDistance(standardDeviationGoalpostSampleBearingDistance),
standardDeviationGoalpostSampleSizeDistance(standardDeviationGoalpostSampleSizeDistance),
clipTemplateGeneration(clipTemplateGeneration), 
clipTemplateGenerationRangeX(clipTemplateGenerationRangeX), clipTemplateGenerationRangeY(clipTemplateGenerationRangeY) 
{
}

void SampleTemplateGenerator::init()
{
  realPostPositions[GoalPercept::LEFT_OPPONENT] = 
    Vector2<double>(theFieldDimensions.xPosOpponentGoalpost, theFieldDimensions.yPosLeftGoal);
  realPostPositions[GoalPercept::RIGHT_OPPONENT] = 
    Vector2<double>(theFieldDimensions.xPosOpponentGoalpost, theFieldDimensions.yPosRightGoal);
  realPostPositions[GoalPercept::LEFT_OWN] = 
    Vector2<double>(theFieldDimensions.xPosOwnGoalpost, theFieldDimensions.yPosRightGoal); //y coordinates are switched
  realPostPositions[GoalPercept::RIGHT_OWN]  = 
    Vector2<double>(theFieldDimensions.xPosOwnGoalpost, theFieldDimensions.yPosLeftGoal);  //y coordinates are switched
}

void SampleTemplateGenerator::bufferNewPerceptions()
{
  // Buffer data generated from GoalPercept:
  // We currently see the complete opponent goal:
  if((theGoalPercept.posts[GoalPercept::LEFT_OPPONENT].timeWhenLastSeen == theFrameInfo.time) &&
     (theGoalPercept.posts[GoalPercept::RIGHT_OPPONENT].timeWhenLastSeen == theFrameInfo.time) &&
     (theGoalPercept.posts[GoalPercept::LEFT_OPPONENT].distanceType != GoalPost::IS_CLOSER) &&
     (theGoalPercept.posts[GoalPercept::RIGHT_OPPONENT].distanceType != GoalPost::IS_CLOSER))
  {
    FullGoal newFullGoal;
    newFullGoal.realLeftPosition  = realPostPositions[GoalPercept::LEFT_OPPONENT];
    newFullGoal.realRightPosition = realPostPositions[GoalPercept::RIGHT_OPPONENT];
    newFullGoal.seenLeftPosition.x = theGoalPercept.posts[GoalPercept::LEFT_OPPONENT].positionOnField.x;
    newFullGoal.seenLeftPosition.y = theGoalPercept.posts[GoalPercept::LEFT_OPPONENT].positionOnField.y;
    newFullGoal.seenRightPosition.x = theGoalPercept.posts[GoalPercept::RIGHT_OPPONENT].positionOnField.x;
    newFullGoal.seenRightPosition.y = theGoalPercept.posts[GoalPercept::RIGHT_OPPONENT].positionOnField.y;
    newFullGoal.timestamp = theFrameInfo.time;
    newFullGoal.odometry = theOdometryData;
    // Before adding, check if templates can be generated from this perception
    SampleTemplate checkTemplate = generateTemplateFromFullGoal(newFullGoal);
    if(checkTemplate.timestamp)
      fullGoals.add(newFullGoal);
  }
  // We currently see the complete own goal:
  else if((theGoalPercept.posts[GoalPercept::LEFT_OWN].timeWhenLastSeen == theFrameInfo.time) &&
          (theGoalPercept.posts[GoalPercept::RIGHT_OWN].timeWhenLastSeen == theFrameInfo.time) &&
          (theGoalPercept.posts[GoalPercept::LEFT_OWN].distanceType != GoalPost::IS_CLOSER) &&
          (theGoalPercept.posts[GoalPercept::RIGHT_OWN].distanceType != GoalPost::IS_CLOSER))
  {
    FullGoal newFullGoal;
    newFullGoal.realLeftPosition  = realPostPositions[GoalPercept::LEFT_OWN];
    newFullGoal.realRightPosition = realPostPositions[GoalPercept::RIGHT_OWN];
    newFullGoal.seenLeftPosition.x = theGoalPercept.posts[GoalPercept::LEFT_OWN].positionOnField.x;
    newFullGoal.seenLeftPosition.y = theGoalPercept.posts[GoalPercept::LEFT_OWN].positionOnField.y;
    newFullGoal.seenRightPosition.x = theGoalPercept.posts[GoalPercept::RIGHT_OWN].positionOnField.x;
    newFullGoal.seenRightPosition.y = theGoalPercept.posts[GoalPercept::RIGHT_OWN].positionOnField.y;
    newFullGoal.timestamp = theFrameInfo.time;
    newFullGoal.odometry = theOdometryData;
    // Before adding, check if templates can be generated from this perception
    SampleTemplate checkTemplate = generateTemplateFromFullGoal(newFullGoal);
    if(checkTemplate.timestamp)
      fullGoals.add(newFullGoal);
  }
  // We might currently see a single goal post with known side (but not a complete goal)
  else
  {
    for(int p=0; p<GoalPercept::NUMBER_OF_GOAL_POSTS; p++)
    {
      const GoalPost& post = theGoalPercept.posts[p];
      if((post.timeWhenLastSeen == theFrameInfo.time) &&
         (post.distanceType != GoalPost::IS_CLOSER))
      {
        KnownGoalpost newPost;
        newPost.realPosition = realPostPositions[p];
        newPost.seenPosition.x = post.positionOnField.x;
        newPost.seenPosition.y = post.positionOnField.y;
        newPost.timestamp = theFrameInfo.time;
        newPost.odometry = theOdometryData;
        knownGoalposts.add(newPost);
      }
    }
  }
  // Maybe we have seen some goalpost of which we do not know the side:
  for(int p=0; p<GoalPercept::NUMBER_OF_UNKNOWN_GOAL_POSTS; p++)
  {
    const GoalPost& post = theGoalPercept.unknownPosts[p];
    if((post.timeWhenLastSeen == theFrameInfo.time) &&
      (post.distanceType != GoalPost::IS_CLOSER))
    {
      UnknownGoalpost newPost;
      newPost.realPositions[0] = realPostPositions[2*p];
      newPost.realPositions[1] = realPostPositions[2*p+1];
      newPost.seenPosition.x = post.positionOnField.x;
      newPost.seenPosition.y = post.positionOnField.y;
      newPost.timestamp = theFrameInfo.time;
      newPost.odometry = theOdometryData;
      unknownGoalposts.add(newPost);
    }
  }
  // If there are still some too old percepts after adding new ones -> delete them:
  removeOldPercepts(fullGoals);
  removeOldPercepts(knownGoalposts);
  removeOldPercepts(unknownGoalposts);
}

template<typename T>
void SampleTemplateGenerator::removeOldPercepts(RingBuffer<T, MAX_PERCEPTS>& buffer)
{
  while(buffer.getNumberOfEntries())
  {
    T& oldestElement = buffer[buffer.getNumberOfEntries()-1];
    if(theFrameInfo.getTimeSince(oldestElement.timestamp) > MAX_TIME_TO_KEEP)
      buffer.removeFirst();
    else
      break;
  }
}

SampleTemplate SampleTemplateGenerator::getNewTemplate()
{
  SampleTemplate newTemplate;
  // Current solution: Prefer to construct templates from full goals only:
  if(fullGoals.getNumberOfEntries())
  {
    FullGoal& goal = fullGoals[rand() % fullGoals.getNumberOfEntries()];
    newTemplate = generateTemplateFromFullGoal(goal);
  }
  else if(knownGoalposts.getNumberOfEntries())
  {
    KnownGoalpost& goalPost = knownGoalposts[rand() % knownGoalposts.getNumberOfEntries()];
    newTemplate = generateTemplateFromPosition(goalPost.seenPosition, 
      goalPost.realPosition, goalPost.odometry);
  }
  else if(unknownGoalposts.getNumberOfEntries())
  {
    UnknownGoalpost& goalPost = unknownGoalposts[rand() % unknownGoalposts.getNumberOfEntries()];
    newTemplate = generateTemplateFromPosition(goalPost.seenPosition, 
      goalPost.realPositions[rand() % 2], goalPost.odometry);
  }
  if(newTemplate.timestamp == 0) // In some cases, no proper sample is generated, return a random sample
  {
    newTemplate = generateRandomTemplate();
  }
  return newTemplate;
}

bool SampleTemplateGenerator::templatesAvailable()
{
  int sumOfTemplates = fullGoals.getNumberOfEntries() + 
    knownGoalposts.getNumberOfEntries() + unknownGoalposts.getNumberOfEntries();
  return sumOfTemplates > 0;
}

SampleTemplate SampleTemplateGenerator::generateTemplateFromFullGoal(const FullGoal& goal) const
{
  SampleTemplate newTemplate;
  Pose2D odometryOffset = theOdometryData - goal.odometry;
  double leftPostDist = goal.seenLeftPosition.abs();
  double leftDistUncertainty = sampleTriangularDistribution(standardDeviationGoalpostSampleBearingDistance);
  if(leftPostDist+leftDistUncertainty > standardDeviationGoalpostSampleBearingDistance)
    leftPostDist += leftDistUncertainty;
  double rightPostDist = goal.seenRightPosition.abs();
  double rightDistUncertainty = sampleTriangularDistribution(standardDeviationGoalpostSampleBearingDistance);
  if(rightPostDist+rightDistUncertainty > standardDeviationGoalpostSampleBearingDistance)
    rightPostDist += rightDistUncertainty;
  Geometry::Circle c1(goal.realLeftPosition, leftPostDist + theFieldDimensions.goalPostRadius);
  Geometry::Circle c2(goal.realRightPosition, rightPostDist + theFieldDimensions.goalPostRadius);
  // If there are intersections, take the first one that is in the field:
  Vector2<double> p1,p2;
  int result = Geometry::getIntersectionOfCircles(c1, c2, p1, p2);
  if(result)
  {
    if(theFieldDimensions.isInsideCarpet(p1) && checkTemplateClipping(p1))
    {
      double origAngle = (goal.realLeftPosition-p1).angle();
      double observedAngle = goal.seenLeftPosition.angle();
      Pose2D templatePose(origAngle-observedAngle, p1);
      templatePose += odometryOffset;
      newTemplate = templatePose;
      newTemplate.timestamp = theFrameInfo.time;
    }
    else if(theFieldDimensions.isInsideCarpet(p2) && checkTemplateClipping(p2))
    {
      double origAngle = (goal.realLeftPosition-p2).angle();
      double observedAngle = goal.seenLeftPosition.angle();
      Pose2D templatePose(origAngle-observedAngle, p2);
      templatePose += odometryOffset;
      newTemplate = templatePose;
      newTemplate.timestamp = theFrameInfo.time;
    }
  }
  return newTemplate;
}

SampleTemplate SampleTemplateGenerator::generateTemplateFromPosition(
  const Vector2<double>& posSeen, const Vector2<double>& posReal,
  const Pose2D& postOdometry) const
{
  SampleTemplate newTemplate;
  double r = posSeen.abs() + theFieldDimensions.goalPostRadius;
  double distUncertainty = sampleTriangularDistribution(standardDeviationGoalpostSampleBearingDistance);
  if(r+distUncertainty > standardDeviationGoalpostSampleBearingDistance)
    r += distUncertainty;
  Vector2<double> realPosition = posReal;
  double minY = std::max(posReal.y - r, static_cast<double>(theFieldDimensions.yPosRightFieldBorder));
  double maxY = std::min(posReal.y + r, static_cast<double>(theFieldDimensions.yPosLeftFieldBorder));
  Vector2<double> p;
  p.y = minY + randomDouble()*(maxY - minY);
  double xOffset(sqrt(sqr(r) - sqr(p.y - posReal.y)));
  p.x = posReal.x;
  p.x += (p.x > 0) ? -xOffset : xOffset;
  if(theFieldDimensions.isInsideCarpet(p) && checkTemplateClipping(p))
  {
    double origAngle = (realPosition-p).angle();
    double observedAngle = posSeen.angle();
    Pose2D templatePose(origAngle-observedAngle, p);
    Pose2D odometryOffset = theOdometryData - postOdometry;
    templatePose += odometryOffset;
    newTemplate = templatePose;
    newTemplate.timestamp = theFrameInfo.time;
  }
  return newTemplate;
}

SampleTemplate SampleTemplateGenerator::generateRandomTemplate() const
{
  SampleTemplate newTemplate;
  if(clipTemplateGeneration)
    newTemplate = Pose2D::random(clipTemplateGenerationRangeX, clipTemplateGenerationRangeY, Range<double>(-pi,pi));
  else
    newTemplate = theFieldDimensions.randomPoseOnField();
  newTemplate.timestamp = theFrameInfo.time;
  return newTemplate;
}

void SampleTemplateGenerator::draw()
{
  for(int i=0; i<fullGoals.getNumberOfEntries(); ++i)
  {
    FullGoal& goal = fullGoals[i];
    Pose2D odometryOffset = goal.odometry - theOdometryData;
    Vector2<double> leftPost = odometryOffset * goal.seenLeftPosition;
    Vector2<double> rightPost = odometryOffset * goal.seenRightPosition;
    LINE("module:SelfLocator:templates", leftPost.x, leftPost.y,
      rightPost.x, rightPost.y, 50, Drawings::ps_solid, ColorRGBA(140,140,255));
    CIRCLE("module:SelfLocator:templates", leftPost.x, leftPost.y, 
      100, 20, Drawings::ps_solid, ColorRGBA(0,0,0), Drawings::bs_solid, ColorRGBA(140,140,255));
    CIRCLE("module:SelfLocator:templates", rightPost.x, rightPost.y, 
      100, 20, Drawings::ps_solid, ColorRGBA(0,0,0), Drawings::bs_solid, ColorRGBA(140,140,255));
  }
  for(int i=0; i<knownGoalposts.getNumberOfEntries(); ++i)
  {
    KnownGoalpost& post = knownGoalposts[i];
    Pose2D odometryOffset = post.odometry - theOdometryData;
    Vector2<double> postPos = odometryOffset * post.seenPosition;
    CIRCLE("module:SelfLocator:templates", postPos.x, postPos.y, 
      100, 20, Drawings::ps_solid, ColorRGBA(140,140,255), Drawings::bs_solid, ColorRGBA(140,140,255));
    CIRCLE("module:SelfLocator:templates", postPos.x, postPos.y, 
      200, 20, Drawings::ps_solid, ColorRGBA(0,0,0), Drawings::bs_null, ColorRGBA(140,140,255));
  }
}

bool SampleTemplateGenerator::checkTemplateClipping(const Vector2<double> pos) const
{
  if(!clipTemplateGeneration)
    return true;
  else
    return (clipTemplateGenerationRangeX.isInside(pos.x) && clipTemplateGenerationRangeY.isInside(pos.y));
}
