/** 
* @file  Platform/linux/SystemCall.h
* @brief static class for system calls from the non NDA classes
* @attention the implementation is system specific! Only definitions here
*
* @author <A href=mailto:brunn@sim.informatik.tu-darmstadt.de>Ronnie Brunn</A>
* @author <A href=mailto:risler@sim.informatik.tu-darmstadt.de>Max Risler</A>
*/

#ifndef _SYSTEMCALL_H_
#define _SYSTEMCALL_H_

/**
* All process-local global variable declarations have to be preceeded 
* by this macro. Only variables of simple types can be defined, i.e. 
* no class instantiations are allowed.
*/
#define PROCESS_WIDE_STORAGE __thread
#define PROCESS_WIDE_STORAGE_STATIC static PROCESS_WIDE_STORAGE
/**
* static class for system calls
* @attention the implementation is system specific!
*/
class SystemCall
{
public:
  enum Mode
  {
    physicalRobot,
    remoteRobot,
    simulatedRobot,
    logfileReplay,
    teamRobot,
    groundTruth
  };

  /** returns the current system time in milliseconds*/
  static unsigned getCurrentSystemTime ();
  
  /** returns the real system time in milliseconds (never the simulated one)*/
  static unsigned getRealSystemTime ();
  
  /** returns the time since aTime*/
  static int getTimeSince(unsigned long aTime) 
  {
    return (int) (getCurrentSystemTime() - aTime);
  }

  /** returns the real time since aTime*/
  static int getRealTimeSince(unsigned long aTime) 
  {
    return (int) (getRealSystemTime() - aTime);
  }

  /** returns the name of the local machine*/
  static const char* getHostName();

  /** returns the current execution mode */
  static Mode getMode();

  /** waits a certain number of ms. */
  static void sleep(unsigned long ms);

  /** Returns the load and the physical memory usage in percent */
  static void getLoad(float& mem, float load[3]);
};


#endif // _SYSTEMCALL_H_
