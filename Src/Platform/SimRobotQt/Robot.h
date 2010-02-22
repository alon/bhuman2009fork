/**
* @file Platform/SimRobotQt/Robot.h
*
* This file declares the class Robot.
*
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
*/
#ifndef __Robot_h__
#define __Robot_h__

#include <SimRobotCore/Controller/Controller.h>
#include "Platform/ProcessFramework.h"
#include "Representations/Infrastructure/Image.h"
#include "Representations/Infrastructure/SensorData.h"

class RobotConsole;

/**
* The class implements a robot as a list of processes.
*/
class Robot : public ProcessList
{
private:
  RobotConsole* robotProcess; /**< A pointer to the process that simulates the physical robots. */
  SimObject* obj; /**< A pointer to the associated robot in SimRobot. */
  std::string name; /**< The name of the robot. */
  PlatformProcess* broadcastReceiver; /**< The receiver of broadcast packages. */

public:
  /**
  * Constructor.
  * @param name The name of the robot.
  * @param obj A pointer to the associated robot in SimRobot.
  */
  Robot(const char* name, SimObject* obj);

  /**
  * The function updates all sensors and sends motor commands to SimRobot.
  */
  void update();

  /**
  * The function returns the name of the robot.
  * @return The name of the robot.
  */
  const char* getName() const {return name.c_str();}

  /**
  * The function returns the name of the robot.
  * @return The name of the robot.
  */
  std::string getModel() const;

  /**
  * The function returns the SimRobot object representing a robot.
  * @return The SimRobot object pointer that was specified at construction.
  */
  SimObject* getSimRobotObject() const {return obj;}

  /**
  * The function returns a pointer to the process that simulates the physical robots.
  * @return The pointer to the process.
  */
  RobotConsole* getRobotProcess() const {return robotProcess;}

  /**
  * The function connects a sender and a receiver.
  * @param sender The sender.
  * @param receiver The receiver.
  */
  static void connect(SenderList* sender,ReceiverList* receiver);

  /**
  * The method queues a broadcast package.
  * @param package The package. It will be deleted after it was delivered.
  */
  void setBroadcastPackage(const char* package);

  /**
  * The method returns whether this process listens for broadcast packages.
  * Don't call setBroadcastPackage when the process is not listening.
  */
  bool listensForBroadcastPackages() const {return broadcastReceiver != 0;}

private:
  /**
  * The function looks up a sender.
  * @param senderName The Aperios name of the sender. If the process name is missing
  *                   i.e. senderName starts with a dot, the first process with a 
  *                   sender that matches the rest of the name is used.
  * @return A pointer to the sender or 0 if no sender exists with the specified name.
  */
  SenderList* getSender(const char* senderName);

  /**
  * The function looks up a receiver.
  * @param receiverName The Aperios name of the receiver.
  * @return A pointer to the receiver or 0 if no receiver exists with the specified name.
  */
  ReceiverList* getReceiver(const char* receiverName);
};

#endif
