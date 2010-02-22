/**
* @file RobotPose.cpp
*
* contains the implementation of the streaming operators 
* for the class RobotPose
*/

#include "RobotPose.h"
#include "Tools/Math/Geometry.h"


void RobotPose::draw()
{
  DECLARE_DEBUG_DRAWING("representation:RobotPose", "drawingOnField"); 
  Vector2<double> bodyPoints[4] = {Vector2<double>(55,90),
                                   Vector2<double>(-55,90),
                                   Vector2<double>(-55,-90),
                                   Vector2<double>(55,-90)};
  for(int i=0; i<4; i++)
  {
    bodyPoints[i] = Geometry::rotate(bodyPoints[i], rotation);
    bodyPoints[i] += translation;
  }
  Vector2<double> dirVec(200,0);
  dirVec.rotate(rotation);
  dirVec += translation;
  LINE("representation:RobotPose", translation.x, translation.y, dirVec.x, dirVec.y, 
    20, Drawings::ps_solid, ColorClasses::white);
  POLYGON("representation:RobotPose", 4, bodyPoints, 20, Drawings::ps_solid, 
    ownTeamColorForDrawing, Drawings::bs_solid, ColorClasses::white);
  CIRCLE("representation:RobotPose", translation.x, translation.y, 42, 1, 
    Drawings::ps_solid, ownTeamColorForDrawing, Drawings::bs_solid, ownTeamColorForDrawing);
  
  DECLARE_DEBUG_DRAWING("origin:RobotPose", "drawingOnField"); // Set the origin to the robot's current position
  DECLARE_DEBUG_DRAWING("origin:RobotPoseWithoutRotation", "drawingOnField");
  ORIGIN("origin:RobotPose", translation.x, translation.y, rotation);
  ORIGIN("origin:RobotPoseWithoutRotation", translation.x, translation.y, 0);
}

void GroundTruthRobotPose::draw()
{
  DECLARE_DEBUG_DRAWING("representation:GroundTruthRobotPose", "drawingOnField"); 
  ColorRGBA ownTeamColorForDrawing(this->ownTeamColorForDrawing);
  ownTeamColorForDrawing.a = 128;
  ColorRGBA transparentWhite(ColorClasses::white);
  transparentWhite.a = 128;
  Vector2<double> bodyPoints[4] = {Vector2<double>(55,90),
                                   Vector2<double>(-55,90),
                                   Vector2<double>(-55,-90),
                                   Vector2<double>(55,-90)};
  for(int i=0; i<4; i++)
  {
    bodyPoints[i] = Geometry::rotate(bodyPoints[i], rotation);
    bodyPoints[i] += translation;
  }
  Vector2<double> dirVec(200,0);
  dirVec.rotate(rotation);
  dirVec += translation;
  LINE("representation:GroundTruthRobotPose", translation.x, translation.y, dirVec.x, dirVec.y, 
    20, Drawings::ps_solid, transparentWhite);
  POLYGON("representation:GroundTruthRobotPose", 4, bodyPoints, 20, Drawings::ps_solid, 
    ownTeamColorForDrawing, Drawings::bs_solid, transparentWhite);
  CIRCLE("representation:GroundTruthRobotPose", translation.x, translation.y, 42, 1, 
    Drawings::ps_solid, ownTeamColorForDrawing, Drawings::bs_solid, ownTeamColorForDrawing);

  DECLARE_DEBUG_DRAWING("origin:GroundTruthRobotPose", "drawingOnField"); // Set the origin to the robot's ground truth position
  DECLARE_DEBUG_DRAWING("origin:GroundTruthRobotPoseWithoutRotation", "drawingOnField");
  ORIGIN("origin:GroundTruthRobotPose", translation.x, translation.y, rotation);
  ORIGIN("origin:GroundTruthRobotPoseWithoutRotation", translation.x, translation.y, 0);
}
