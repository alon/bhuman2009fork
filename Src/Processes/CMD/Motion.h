/**
* @file Processes/CMD/Motion.h
* Declaration of a class that represents the process that sends commands to the robot at 50Hz.
*/

#ifndef __Motion_h_
#define __Motion_h_

#include "Tools/Process.h"
#include "Tools/Module/ModulePackage.h"

/**
* @class Motion 
* A class that represents the process that sends commands to the robot at 125Hz.
*/
class Motion : public Process
{
private:
  DEBUGGING;
  RECEIVER(CognitionToMotion);
  SENDER(MotionToCognition);

public:
  /**
  * Default constructor.
  */
  Motion();
  
  /**
  * Default destructor.
  * Sets the global pointers again (only for Simulator).
  */
  ~Motion() {setGlobals();}
  
  /** 
  * The method is called from the framework once in every frame.
  */
  virtual int main();
  
  /** 
  * The method is called directly before the first call of main().
  */
  virtual void init();

  /**
  * The method is called when the process is terminated.
  */
  virtual void terminate();

  /** 
  * The function handles incoming debug messages.
  * @param message the message to handle.
  * @return Has the message been handled?
  */
  virtual bool handleMessage(InMessage& message);
  
private:
  ModuleManager moduleManager; /**< The solution manager handles the execution of modules. */
};

#endif // __Motion_h_
