/**
* @file Tools/Debugging/Trace.h
*
* A class representing a trace.
* 
* @author Thomas Röfer
*/ 
#ifndef __Trace_h_
#define __Trace_h_

#include "Tools/Streams/InOut.h"
#include "Tools/RingBuffer.h"

class Trace;

/**
 * Streaming operator that reads a trace from a stream.
 * @param stream The stream from which is read.
 * @param trace The trace.
 * @return The stream.
 */ 
In& operator>>(In& stream, Trace& trace);
 
/**
 * Streaming operator that writes a robot pose to a stream.
 * @param stream The stream to write on.
 * @param trace The Trace.
 * @return The stream.
 */ 
Out& operator<<(Out& stream, const Trace& trace);

class Trace
{
private:
  /**
  * The class represents single trace points.
  */
  class Line
  {
  public:
    char file[256]; /**< The name of the source file a passed trace point is located in. */
    int line; /**< The line a passed trace point is located in. */

    /**
    * Constructor.
    * @param f The name of the source file a passed trace point is located in.
    * @param l The line a passed trace point is located in.
    */
    Line(const char* f = "", int l = 0);
  };

  RingBuffer<Line, 10> buffer; /**< A buffer for the last 10 passed trace points. */

public:
  /**
  * The function adds a new entry to the list of passed trace points.
  * @param file The name of the source file a passed trace point is located in.
  * @param line The line a passed trace point is located in.
  */
  void setCurrentLine(const char* file, int line);

  friend In& operator>>(In& stream, Trace& trace);
  friend Out& operator<<(Out& stream, const Trace& trace);
};

/**
* The function returns a reference to the shared trace object.
* @return The reference to the shared trace object.
*/
Trace& getTrace();

/**
* The function sets the reference to the shared trace object.
* @param t The reference to the shared trace object.
*/
void setTrace(Trace& t);

#endif
