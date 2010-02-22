/** 
* @file  Platform/SimRobotQt/SystemCall.h
* @brief static class for system calls from the non NDA classes
* @attention the implementation is system specific! Only definitions here
*/

#ifndef	_SYSTEMCALL_H_
#define	_SYSTEMCALL_H_

/**
 * All process-local global variable declarations have to be preceeded 
 * by this macro. Only variables of simple types can be defined, i.e. 
 * no class instantiations are allowed.
 */
#ifdef _WIN32
#define PROCESS_WIDE_STORAGE __declspec(thread)
#define PROCESS_WIDE_STORAGE_STATIC PROCESS_WIDE_STORAGE static
#else
#define PROCESS_WIDE_STORAGE __thread
#define PROCESS_WIDE_STORAGE_STATIC static PROCESS_WIDE_STORAGE
#endif

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
  inline static int getTimeSince(unsigned aTime) 
  {
    return (int) (getCurrentSystemTime() - aTime);
  }

  /** returns the real time since aTime*/
  inline static int getRealTimeSince(unsigned aTime) 
  {
    return (int) (getRealSystemTime() - aTime);
  }

  /** returns the name of the local machine*/
  static const char* getHostName();

  /** Sleeps for some milliseconds.
  * \param ms The amout of milliseconds.
  */
  static void sleep(unsigned int ms);

  /** returns the current execution mode */
  static Mode getMode();

  /** Returns the load and the physical memory usage in percent */
  static void getLoad(float& mem, float load[3]);
};

#endif // _SYSTEMCALL_H_
