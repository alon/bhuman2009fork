/** 
* @file ControllerQt/RemoteRobot.h
* Declaration of a class representing a process that communicates with a remote robot via TCP.
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas R�fer</a>
*/

#ifndef __RemoteRobot_h_
#define __RemoteRobot_h_

#include "RobotConsole.h"
#include "Tools/Debugging/TcpConnection.h"

/**
* @class RemoteRobot
* A class representing a process that communicates with a remote robot via TCP.
*/
class RemoteRobot : public RobotConsole, public TcpConnection, private Thread<RemoteRobot>
{
private:
  DEBUGGING;

  char name[80]; /**< The name of the robot. */
  char ip[80]; /**< The ip of the robot. */
  int bytesTransfered; /**< The number of bytes transfered so far. */
  double transferSpeed; /**< The transfer speed in kb/s. */
  unsigned timeStamp; /**< The time when the transfer speed was measured. */

  /**
  * The main loop of the process.
  */
  void run();

  /**
  * The function connects to another process.
  */
  void connect();

  /**
  * The function is called from the framework once in every frame.
  */
  virtual int main();

public:
  /**
  * Constructor.
  * @param name The name of the robot.
  * @param ip The ip address of the robot.
  */
  RemoteRobot(const char* name, const char* ip);

  /**
  * The function starts the process.
  */
  void start() {Thread<RemoteRobot>::start(this, &RemoteRobot::run);}

  /**
  * The function is called to announce the termination of the process.
  */
  void announceStop();

  /**
  * The function must be called to exchange data with SimRobot.
  * It sends the motor commands to SimRobot and acquires new sensor data.
  */
  void update();

  /**
  * The function returns the name of the robot.
  * @return The name.
  */
  const char* getName() const {return name;}
};

#endif // __RemoteRobot_h_
