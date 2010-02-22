/** 
* @file ReleaseOptions.h
* This file declares a class representing the debugging options in release code.
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
*/

#ifndef __ReleaseOptions_h_
#define __ReleaseOptions_h_

#include "Tools/Streams/Streamable.h"

/**
* @class ReleaseOptions
* A class representing the debugging options in release code.
*/
class ReleaseOptions : public Streamable
{
private:
  virtual void serialize(In* in, Out* out)
  {  
    STREAM_REGISTER_BEGIN();
    STREAM(stopwatch);
    STREAM(sensorData);
    STREAM(robotHealth);
    STREAM(motionRequest);
    STREAM(linePercept);
    STREAM_REGISTER_FINISH();
  }

public:
  /**
  * Default constructor.
  */
  ReleaseOptions() : 
    stopwatch(0),
    sensorData(false),
    robotHealth(true), // Send health data per default
    motionRequest(true), // same here
    linePercept(false) {}

  char stopwatch;     /**< The first character of the name of the stopwatch to send. */
  bool sensorData,    /**< Activate sending sensorData. */
       robotHealth,   /**< Activate sending robot health data. */
       motionRequest, /**< Activate sending motion requests. */
       linePercept; /**< Activate sending line percepts. */
};

#endif // __ReleaseOptions_h_
