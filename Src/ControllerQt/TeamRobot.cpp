/** 
* @file ControllerQt/TeamRobot.cpp
* Implementation of a class representing a process that communicates with a remote robot via team communication.
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
*/

#include "TeamRobot.h"
#define Drawings ::Drawings // Base class also defines Drawings, but need the global class

int TeamRobot::main() 
{
  SYNC;
  OUTPUT(idProcessBegin, bin, 't');

  DECLARE_DEBUG_DRAWING("representation:RobotPose", "drawingOnField"); // The robot pose
  DECLARE_DEBUG_DRAWING("origin:RobotPose", "drawingOnField"); // Set the origin to the robot's current position
  DECLARE_DEBUG_DRAWING("representation:BallModel", "drawingOnField"); // drawing of the ball model
  DECLARE_DEBUG_DRAWING("representation:GoalPercept:Field", "drawingOnField"); // drawing of the goal percept
  DECLARE_DEBUG_DRAWING("representation:MotionRequest", "drawingOnField"); // drawing of a request walk vector
  DECLARE_DEBUG_DRAWING("representation:LinePercept:Field", "drawingOnField");

  if(SystemCall::getTimeSince(robotPoseReceived) < 1000)
    robotPose.draw();
  if(SystemCall::getTimeSince(ballModelReceived) < 1000)
    ballModel.draw();
  if(SystemCall::getTimeSince(goalPerceptReceived) < 1000)
    goalPercept.draw();
  if(SystemCall::getTimeSince(motionRequestReceived) < 1000)
    motionRequest.draw();
  if(SystemCall::getTimeSince(linePerceptReceived) < 1000)
    linePercept.drawOnField(fieldDimensions, 0);

  int teamColor = 0,
      swapSides = 0;
  MODIFY("teamColor", teamColor);
  MODIFY("swapSides", swapSides);

  if(swapSides ^ teamColor)
  {
    ORIGIN("field polygons", 0, 0, pi2); // hack: swap sides!
  }
  fieldDimensions.drawPolygons(teamColor);
  fieldDimensions.draw();

  DECLARE_DEBUG_DRAWING("robotState", "drawingOnField"); // text decribing the state of the robot
  int lineY = 2550;
  DRAWTEXT("robotState", -3600, lineY, 150, ColorClasses::white, "behavior: role: " << BehaviorData::getRoleName(behaviorData.role) << ", action: " << BehaviorData::getActionName(behaviorData.action));
  lineY -= 180;
  if(ballModel.timeWhenLastSeen)
  {
    DRAWTEXT("robotState", -3600, lineY, 150, ColorClasses::white, "ballLastSeen: " << SystemCall::getRealTimeSince(ballModel.timeWhenLastSeen) << " ms");
  }
  else
  {
    DRAWTEXT("robotState", -3600, lineY, 150, ColorClasses::white, "ballLastSeen: never");
  }
  lineY -= 180;
  DRAWTEXT("robotState", -3600, lineY, 150, ColorClasses::white, "ballPercept: " << ballModel.lastPerception.position.x << ", " << ballModel.lastPerception.position.y);
  lineY -= 180;
  if(goalPercept.timeWhenOppGoalLastSeen)
  {
    DRAWTEXT("robotState", -3600, lineY, 150, ColorClasses::white, "goalLastSeen: " << SystemCall::getRealTimeSince(goalPercept.timeWhenOppGoalLastSeen) << " ms");
  }
  else
  {
    DRAWTEXT("robotState", -3600, lineY, 150, ColorClasses::white, "goalLastSeen: never");
  }
  lineY -= 180;
  DRAWTEXT("robotState", -3600, lineY, 150, ColorClasses::white, "batteryLevel: " << (robotHealth.batteryLevel * 100.f) << " %");
  lineY -= 180;
  DRAWTEXT("robotState", -3600, lineY, 150, ColorClasses::white, "max temperature: " << robotHealth.maxTemperature << " C");
  lineY -= 180;
  DRAWTEXT("robotState", -3600, lineY, 150, ColorClasses::white, "cognition rate: " << roundNumberToInt(robotHealth.cognitionFrameRate) << " fps");
  lineY -= 180;
  DRAWTEXT("robotState", -3600, lineY, 150, ColorClasses::white, "motion rate: " << roundNumberToInt(robotHealth.motionFrameRate) << " fps");
  lineY -= 180;
  DRAWTEXT("robotState", -3600, lineY, 150, ColorClasses::white, "memory usage: " << (robotHealth.memoryLoad  * 100.f) << " %");
  lineY -= 180;
  DRAWTEXT("robotState", -3600, lineY, 150, ColorClasses::white, "load average: " << (robotHealth.load[0] * 100.f) << " % " << (robotHealth.load[1] * 100.f) << " % " << (robotHealth.load[2] * 100.f) << " % ");
  lineY -= 180;
  DRAWTEXT("robotState", -2600, 0, 900, ColorRGBA(255,255,255,50), robotHealth.robotName);

  DECLARE_DEBUG_DRAWING("robotOffline", "drawingOnField"); // A huge X to display the online/offline state of the robot
  if(SystemCall::getTimeSince(robotPoseReceived) > 500)
  {
    LINE("robotOffline", -3600, 2600, 3600, -2600, 50, Drawings::ps_solid, ColorRGBA(0xff, 0, 0));
    LINE("robotOffline", 3600, 2600, -3600, -2600, 50, Drawings::ps_solid, ColorRGBA(0xff, 0, 0));
  }    

  DEBUG_RESPONSE("automated requests:DrawingManager", OUTPUT(idDrawingManager, bin, Global::getDrawingManager()); );
  DEBUG_RESPONSE("automated requests:DrawingManager3D", OUTPUT(idDrawingManager3D, bin, Global::getDrawingManager3D()); );
  DEBUG_RESPONSE("automated requests:StreamSpecification", OUTPUT(idStreamSpecification, bin, Global::getStreamHandler()); );

  OUTPUT(idProcessFinished, bin, 't');
  teamOut.moveAllMessages(teamIn);
  return 50;
}

TeamRobot::TeamRobot(const char* name, int number) :
RobotConsole(teamIn, teamOut), number(number)
{
  strcpy(this->name, name); 
  mode = SystemCall::teamRobot;
  fieldDimensions.load();
}
