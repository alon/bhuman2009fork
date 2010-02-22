/*
 * @file Tools/Streams/Streamable.h
 * 
 * Base class for all types streamed through StreamHandler macros.
 *
 * @author Michael Spranger
 * @author Tobias Oberlies
 */
#ifndef __Streamable_h_
#define __Streamable_h_

#include "Tools/Streams/StreamHandler.h"

/**
 * Base class for all classes using the STREAM or STREAM_EXT macros (see Tools/Debugging/
 * StreamHandler.h) for automatic streaming of class specifications to RobotControl. 
 * Only those instances of those classes can be parsed by RobotControl when they are 
 * transmitted through MODIFY and SEND macros (see Tools/Debugging/DebugDataTable.h).
 */
class ImplicitlyStreamable
{
};


/**
 * Base class for all classes using the STREAM macros for streaming instances.
 */
class Streamable : public ImplicitlyStreamable
{
protected:
  virtual void serialize(In*,Out*)=0;
public:
  void streamOut(Out&)const;
  void streamIn(In&);
};


In& operator>>( In& in, Streamable& streamable);

Out& operator<<( Out& out, const Streamable& streamable);

#endif // Streamable
