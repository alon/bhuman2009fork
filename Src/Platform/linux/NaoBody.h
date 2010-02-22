/**
* @file Platform/linux/NaoBody.cpp
* Declaration of a class for accessing the body of the nao via NaoQi/libbhuman.
* @author Colin Graf
*/

#ifndef NaoBody_H
#define NaoBody_H

struct LbhData;

/**
* @class NaoBody
* Encapsulates communication with libbhuman.
*/
class NaoBody
{
public:

  /** Default constructor. */
  NaoBody();

  /** Destructor. */
  ~NaoBody();

  /** Establishes the connection to libbhuman.
  * @return Whether the connection was established correctly. */
  bool init();

  /** Waits for a new set of sensor data. */
  void wait();

  /** Activates the eye-blinking mode to indicate a crash.
  * @param termSignal The termination signal that was raised by the crash. */
  void setCrashed(int termSignal);

  /** Accesses the name of the robot's body. 
  * @return The name. */
  const char* getName() const;

  /** Accesses the sensor value buffer. 
  * @return An array of sensor values. Ordered corresponding to \c lbhSensorNames of \c bhuman.h. */
  float* getSensors();

  /** Accesses the actuator value buffer for writing. 
  * @param actuators A reference to a variable to store a pointer to the actuator value buffer in. Ordered corresponding to \c lbhActuatorNames of \c bhuman.h.
  * @param usActuator A reference to a variable to store a pointer to the us actuator value buffer in. */
  void openActuators(float*& actuators, float*& usActuator);
  
  /** Commits the actuator value buffer. */
  void closeActuators();

  /** Initializes low-level flow tracing for the calling process.
  * @param proc The name of the calling process. (should be "Cognition" or "Motion".)
  * @return Whether the initialization was successful.
  */
  static bool initTrace(const char* proc);

  /** Adds a tracing entry.
  * @param file The name of the current file. (Use the __FILE__ macro.)
  * @param line The current file line. (Use the __LINE__ macro.)
  * @param msg A additional message. */
  static void traceMsg(const char* file, const int line, const char* msg);

  /** Prints the trace-entries to stderr. */
  void dumpTrace() const;

private:
  static int fd; /**< The file descriptor for the shared memory block. */
  static void* sem; /**< The semaphore used for synchronizing to the NaoQi DCM. */ 
  static LbhData* lbhData; /**< The pointer to the mapped shared memory block. */
  static int allocations; /**< The amount of instances of this class. */

  static unsigned int cognitionThreadId; /**< The thread id of the coginition process. */
  static unsigned int motionThreadId; /**< The thread id of the motion process. */

  bool opened; /**< Whether the shared memory block and the semaphore was opened. */
  int writingActuators; /**< The index of the opened exclusive actuator writing buffer. */

  /** Converts a BHState into a string.
  * @param bhState The BHState to convert.
  * @return The name of the BHState. */
  static const char* getBHStateName(int bhState);
};

#if defined(TARGET_ROBOT) && !defined(RELEASE)

#define BH_TRACE_INIT(proc) \
  NaoBody::initTrace(proc)

#define BH_TRACE_MSG(msg) \
  NaoBody::traceMsg(__FILE__, __LINE__, msg)

#define BH_TRACE \
  BH_TRACE_MSG("")

#else

#define BH_TRACE ((void)0)
#define BH_TRACE_INIT(x) ((void)0)
#define BH_TRACE_MSG(x) ((void)0)

#endif

#endif
