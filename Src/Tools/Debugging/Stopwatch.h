/** 
* @file Stopwatch.h
* The file declares the stopwatch macros.
* @author <a href="mailto:juengel@informatik.hu-berlin.de">Matthias Jüngel</a>
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
*/

#ifndef __Stopwatch_h_
#define __Stopwatch_h_

#include "Platform/SystemCall.h"

#ifdef RELEASE
#include "Tools/Team.h"
#include "Tools/Debugging/ReleaseOptions.h"

#define STOP_TIME_ON_REQUEST(eventID, expression) \
  if(true) \
  { \
    const char* _p = eventID; \
    if(Global::getReleaseOptions().stopwatch == *_p) \
    { \
      static unsigned _counter = 0; \
      unsigned _startTimeN = SystemCall::getRealSystemTime();\
      { expression }\
      TEAM_OUTPUT(idStopwatch, bin, eventID << _startTimeN << SystemCall::getRealSystemTime() << _counter++); \
    } \
    else \
    { \
      expression \
    } \
  } \
  else \
    ((void) 0)

#else // RELEASE
#include "Tools/Debugging/Debugging.h"

/*
 * Allows for the measurement of time 
 * @param eventID The id of the stop watch
 * @param expression The expression of which the execution time is measured
 */
#define STOP_TIME_ON_REQUEST(eventID, expression) \
  if(true) \
  { \
    DEBUG_RESPONSE("stopwatch:" eventID, \
      static unsigned _counter = 0; \
      unsigned _startTimeN = SystemCall::getRealSystemTime();\
      { expression } \
      OUTPUT(idStopwatch, bin, eventID << _startTimeN << SystemCall::getRealSystemTime() << _counter++); \
    ); \
    DEBUG_RESPONSE_NOT("stopwatch:" eventID, expression ); \
  } \
  else \
    ((void) 0)
#endif // RELEASE

#endif //Stopwatch_h
