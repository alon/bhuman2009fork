/** 
* @file ControllerQt/TeamRobot.h
* Declaration of a class representing a process that communicates with a remote robot via team communication.
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
*/

#ifndef __TeamRobot_h_
#define __TeamRobot_h_

#include "RobotConsole.h"
#include "Representations/Configuration/FieldDimensions.h"
#include "Tools/Debugging/Modify.h"

/**
* @class TeamRobot
* A class representing a process that communicates with a remote robot via team communication.
*/
class TeamRobot : public RobotConsole, public Thread<TeamRobot>
{
private:
  MessageQueue teamIn, /**< The queue for incoming team messages for this robot. */
               teamOut; /**< The queue for outgoing team messages from this robot (not used). */

  char name[80]; /**< The name of the robot. */
  int number; /**< The player number of the robot this process communicates with. */
  FieldDimensions fieldDimensions; /**< The field dimensions for drawing a field. */

  /** The main loop of the process. */
  void run()
  {
    while(isRunning())
      SystemCall::sleep(processMain());
  }

  /**
  * The method is required because RobotConsole is derived from Process.
  * However, since Process is not started, this method will never be called.
  */
  virtual int main();

public:
  /**
  * Constructor.
  * @param name The name of the robot.
  * @param number The player number of the robot this process communicates with.
  */
  TeamRobot(const char* name, int number);

  /** The function starts the process. */
  void start() {Thread<TeamRobot>::start(this, &TeamRobot::run);}

  /**
  * The function returns the name of the robot.
  * @return The name.
  */
  const char* getName() const {return name;}

  /**
  * The function returns the name of the robot.
  * @return The name.
  */
  int getNumber() const {return number;}
};

#endif // __TeamRobot_h_
