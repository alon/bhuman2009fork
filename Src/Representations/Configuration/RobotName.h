/**
* @file RobotName.h
* Declaration of a class representing the name of a robot.
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
*/

#ifndef __RobotName_H__
#define __RobotName_H__

#include "Tools/Streams/Streamable.h"

class RobotName : public Streamable
{
private:
  virtual void serialize(In* in, Out* out)
  { 
    STREAM_REGISTER_BEGIN();
    STREAM(name);
    STREAM_REGISTER_FINISH();
  }

public:
  std::string name; /**< The name of the robot. */
};

#endif
