/**
* @file ControllerQt/RoboCupCtrl.h
*
* This file declares the class RoboCupCtrl.
*
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
*/

#ifndef __RoboCupCtrl_H__
#define __RoboCupCtrl_H__

#include <SimRobotCore/Controller/Controller.h>
#include "Platform/SimRobotQt/Robot.h"
#include "Tools/Math/Pose2D.h"


/**
* The class implements the SimRobot controller for RoboCup.
*/
class RoboCupCtrl : public Controller
{
private:
  static RoboCupCtrl* controller; /**< A pointer to the SimRobot controller. */

protected:
  static std::string robotName; /**< The name of the robot currently constructed. */
  std::list<Robot*> robots; /**< The list of all robots. */
  SimObject* ball; /** the ball */
  SimObject* obj; /**< The current robot constructed. */
  bool simTime; /**< Switches between simulation time mode and real time mode. */
  bool dragTime; /**< Drag simulation to avoid running faster then realtime. */
  int time; /**< The simulation time. */
  unsigned lastTime; /**< The last time execute was called. */
  std::string statusText; /**< The text to be printed in the status bar. */
  int joystickID; /**< The next joystick id. */

  /**
  * Has to be called by derived class to start processes.
  */
  void start();

  /**
  * Has to be called by derived class to stop processes.
  */
  void stop();

public:
  /**
  * Constructor.
  */
  RoboCupCtrl();

  /**
  * The function is called in each simulation step.
  */
  void execute();

  /**
  * The function returns a pointer to the SimRobot controller.
  * @return The pointer to the SimRobot controller.
  */
  static RoboCupCtrl* getController() {return controller;}

  /**
  * The function returns the full name of the robot currently constructed.
  * @return The name of the robot.
  */
  static std::string& getRobotFullName() {return robotName;}

  /**
  * The function returns the name of the robot associated to the current thread.
  * @return The name of the robot.
  */
  const char* getRobotName() const;

  /**
  * The function returns the model of the robot associated to the current thread.
  * @return The model of the robot.
  */
  std::string getModelName() const;

  /**
  * The function returns the simulation time.
  * @return The pseudo-time in milliseconds.
  */
  unsigned getTime() const;

  /**
  * The method broadcasts a package to all robots.
  * @param package The package. It will not be freed.
  * @param size The size of the package.
  */
  void broadcastPackage(const char* package, unsigned size);

  /**
  * The method returns the next joystick id.
  * @return The next joystick
  */
  int getNextJoystickID() {return joystickID++;}
};

#endif
