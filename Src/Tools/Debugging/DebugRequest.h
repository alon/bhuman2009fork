/** 
* @file DebugRequest.h
* Declaration of class DebugRequest
*
* @author Matthias Jüngel
*/

#ifndef __DebugRequest_h__
#define __DebugRequest_h__

#include "Tools/Streams/InOut.h"
#include "Platform/SystemCall.h"

class DebugRequest
{
public:
  /** Constructor, resets the table */
  DebugRequest();
  DebugRequest(const std::string& description, bool enable = true, bool once = false);

  /** the == operator */
  std::string description;
  bool enable;
  bool once;
};

/**
* Streaming operator that reads a DebugRequest from a stream.
* @param stream The stream from which is read.
* @param debugRequest The DebugRequest object.
* @return The stream.
*/ 
In& operator>>(In& stream,DebugRequest& debugRequest);

/**
* Streaming operator that writes a DebugRequest to a stream.
* @param stream The stream to write on.
* @param debugRequest The DebugRequest object.
* @return The stream.
*/ 
Out& operator<<(Out& stream, const DebugRequest& debugRequest);

class RobotConsole;

/** 
* @class DebugRequestTable
*
* A singleton class that maintains the table of currently active debug requests. 
*/
class DebugRequestTable
{
private:
  /**
   * Default constructor.
   * No other instance of this class is allowed except the one accessible via getDebugRequestTable
   * therefore the constructor is private. */
  DebugRequestTable();

  /* Copy constructor.
   * Copying instances of this class is not allowed
   * therefore the copy constructor is private. */
  DebugRequestTable(const DebugRequestTable&) {}

  /*
  * only a process is allowed to create the instance.
  */
  friend class Process;
  friend class RobotConsole;

  enum { maxNumberOfDebugRequests = 1000 };

public:
  /**
  * Returns a reference to the process wide instance.
  */
  static DebugRequestTable& getInstance();

  /**
  * The Debug Key Table
  */
  DebugRequest debugRequests[maxNumberOfDebugRequests];
  int currentNumberOfDebugRequests;
  
  /** */
  void addRequest(const DebugRequest& debugRequest);

  /** */
  void removeRequest(const char* description);

  /** */
  bool isActive (const char* name) const
  {
    if(name == lastName)
      return lastIndex < currentNumberOfDebugRequests && debugRequests[lastIndex].enable;
    else if(name == prevLastName)
    {
      const char* t1 = prevLastName;
      prevLastName = lastName;
      lastName = t1;
      int t2 = prevLastIndex;
      prevLastIndex = lastIndex;
      lastIndex = t2;
      return lastIndex < currentNumberOfDebugRequests && debugRequests[lastIndex].enable;
    }
    else
    {
      prevLastName = lastName;
      lastName = name;
      prevLastIndex = lastIndex;
      for(lastIndex = 0; lastIndex < currentNumberOfDebugRequests; ++lastIndex)
        if(debugRequests[lastIndex].description == name)
          return debugRequests[lastIndex].enable;
      return false;
    }
  }

  /** */
  bool once (const char* name) const
  {
    if(name == lastName)
      return lastIndex < currentNumberOfDebugRequests && debugRequests[lastIndex].once;
    else if(name == prevLastName)
    {
      const char* t1 = prevLastName;
      prevLastName = lastName;
      lastName = t1;
      int t2 = prevLastIndex;
      prevLastIndex = lastIndex;
      lastIndex = t2;
      return lastIndex < currentNumberOfDebugRequests && debugRequests[lastIndex].once;
    }
    else
    {
      prevLastName = lastName;
      lastName = name;
      prevLastIndex = lastIndex;
      for(lastIndex = 0; lastIndex < currentNumberOfDebugRequests; ++lastIndex)
        if(debugRequests[lastIndex].description == name)
          return debugRequests[lastIndex].once;
      return false;
    }
  }

  /** */
  void disable (const char* name);

  /** */
  bool notYetPolled(const char* name);
  
  /** */
  void removeAllRequests() 
  {
    currentNumberOfDebugRequests = 0;
  }

  /** */
  bool poll;

  int pollCounter;

  const char* alreadyPolledDebugRequests[maxNumberOfDebugRequests];
  int alreadyPolledDebugRequestCounter;
  mutable const char* lastName,
                    * prevLastName;
  mutable int lastIndex,
              prevLastIndex;
};

#endif //__DebugRequest_h__
