/**
* @file Platform/linux/Robot.h
* This file implements a class that implements a robot as a list of processes.
* @author <a href="mailto:Bernd.Gersdorf@dfki.de">Bernd Gersdorf</a>
*/
#ifndef __Robot_h__
#define __Robot_h__

// #include "Controller/Controller.h"
#include "Platform/ProcessFramework.h"
#include "Platform/SystemCall.h"

/**
* The class implements a robot as a list of processes.
*/
class Robot : public ProcessList
{
public:
  /**
  * Default constructor.
  */
  Robot();

  /**
  * The function connects a sender and a receiver.
  * @param sender The sender.
  * @param receiver The receiver.
  */
  static void connect(SenderList* sender, ReceiverList* receiver);

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
