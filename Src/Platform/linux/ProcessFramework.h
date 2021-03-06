/**
 * @file Platform/linux/ProcessFramework.h
 *
 * This file declares classes corresponding to the process framework.
 *
 * @author <A href="mailto:Thomas.Roefer@dfki.de">Thomas R�fer</A>
 */
#ifndef __ProcessFramework_h__
#define __ProcessFramework_h__

#include "SystemCall.h"
#include "Thread.h"
#include <list>

const int NAME_LENGTH_MAX = 100; /**< The maximum number of robots in this address context. */

class Robot;
class SenderList;
class ReceiverList;

/**
* This class is the platform dependent base class for all processes.
*/
class PlatformProcess
{
private:
  DECLARE_SYNC;
  SenderList* firstSender; /**< The begin of the list of all senders of this process. */
  ReceiverList* firstReceiver; /**< The begin of the list of all receivers of this process. */
  int blockMask, /**< A mask with bits set for all blocking senders and receivers. */
      eventMask; /**< A mask with bits set for all senders and receivers that received an event */
  int lastTime; /**< The last time when Process::Main() was finished. */
  int id; /**< A counter that is used to provide different ids to senders and receivers. */
  unsigned sleepUntil; /**< The process should sleep until this point in time is reached. */
  int priority; /**< The priority of the process. */

protected:
  /**
  * The function will be call from the process framework in each frame.
  * It shall provide the main functionality of the process.
  * @return If 0 is returned, the function main will not be called on a timed basis
  *         again. A positive number specifies the minimum number of microseconds to
  *         wait before the next frame starts. A negative number specifies a cycle 
  *         time, i.e. the time between two activations of the processMain() function.
  */
  virtual int processMain() = 0;

public:
  /**
  * Constructor.
  */
  PlatformProcess() :
    firstSender(0),
    firstReceiver(0),
    blockMask(0),
    eventMask(0),
    lastTime(0),
    id(0),
    sleepUntil(0),
    priority(0)
  {
  }

  /**
  * The function returns a new id each time it is called.
  * @return The next unused id.
  */
  int getNextId() {return id++;}

  /**
  * The function returns the begin of list of all senders.
  * Note that the function returns a reference that can be changed.
  * @return A reference to the address of the first element. 
  */
  SenderList*& getFirstSender() {return firstSender;}

  /**
  * The function returns the begin of list of all receivers.
  * Note that the function returns a reference that can be changed.
  * @return A reference to the address of the first element. 
  */
  ReceiverList*& getFirstReceiver() {return firstReceiver;}

  /**
  * The functions sets or resets a bit in the blocking mask.
  * After a bit is set in the blocking mask for a certain 
  * sender or receiver, a new frame will not be started before
  * this sender or receiver received an event.
  * @param id The id of the sender or receiver.
  * @param block Should it block or not?
  */
  void setBlockingId(int id,bool block = true);

  /**
  * The function is called when an event was received. 
  * If this was the last event the process was waiting for, the next
  * frame is started, i.e. NextFrame() is called.
  * @param id The id of the sender or receiver that received an event.
  */
  void setEventId(int id);

  /**
  * Returns the event mask.
  * @return The event mask.
  */
  int getEventMask() {return eventMask;}

  /**
  * Resets the event mask.
  */
  void resetEventMask() {eventMask = 0;}

  /**
  * The function is called once for each frame.
  */
  
  /**
  * The function checks whether a timeout condition is satisfied if one was defined.
  * If this is the case, the corresponding eventId is set.
  */
  void checkTime()
  {
    if(sleepUntil && sleepUntil <= SystemCall::getCurrentSystemTime())
    {
      sleepUntil = 0;
      setEventId(31);
    }
  }

    /**
    * The function returns whether the process is waiting for any event.
    * If it is not waiting for an event, it can terminate.
    * @return Waiting for any event (sender, receiver, or time)?
    */
    bool isWaiting() const
    {
      return blockMask != 0;
    }

    /**
    * The function sets the priority of the process.
    * @attention The priority can only be set in the constructor of the process.
    *            Any further changes will be ignored.
    * @param priority The new priority. Reasonable values are -2 .. 2 and 15.
    */
    void setPriority(int priority) {this->priority = priority;}

    /**
    * The function determines the priority of the process.
    * @return The priority of the process.
    */
    int getPriority() const {return priority;}

    /**
    * The method is called when the process is terminated.
    */
    virtual void terminate() {}
    
    // TODO BG PORT
    virtual void nextFrame();
};

#include "Receiver.h"
#include "Sender.h"


/**
* The class is a helper that allows to instantiate a class as an Win32 process.
* ProcessBase contains the parts that need not to be implemented as a template.
* It will only be used by the macro MAKE_PROCESS and should never be used directly.
*/
class ProcessBase : public Thread<ProcessBase>
{
protected:
  /**
  * The main function of this Windows thread.
  */
  virtual void main() = 0;
  
public:
  /**
  * Virtual destructor.
  */
  virtual ~ProcessBase() {}

  /**
  * The function starts the process by starting the Windows thread.
  */
  void start() {Thread<ProcessBase>::start(this, &ProcessBase::main);}

  /**
  * The functions searches for a sender with a given name.
  * @param name The name of the sender.
  * @return If the sender was found, a pointer to it is returned.
  *         Otherwise, the function returns 0.
  */ 
  virtual SenderList* lookupSender(const char* name) = 0;

  /**
  * The functions searches for a receiver with a given name.
  * @param name The name of the receiver.
  * @return If the receiver was found, a pointer to it is returned.
  *         Otherwise, the function returns 0.
  */ 
  virtual ReceiverList* lookupReceiver(const char* name) = 0;

  /**
  * The function returns the name of the process.
  * @return The name of the process that normally is its class name.
  */
  virtual const char* getName() const = 0;

  /**
  * The function returns a pointer to the process if it has the given name.
  * @param processName The name of the process that is searched for.
  * @return If the process has the required name, a pointer to it is
  *         returned. Otherwise, the function returns 0.
  */
  virtual PlatformProcess* getProcess(const char* processName) = 0;
};    

/**
* The class is a helper that allows to instantiate a class as an Win32 process.
* ProcessCreator contains the parts that need to be implemented as a template.
* It will only be used by the macro MAKE_PROCESS and should never be used directly.
*/
template<class T> class ProcessFrame : public ProcessBase
{
private:
  T process; /**< The process. */
  const char* name; /**< The name of the process. */

protected:
  /**
  * The main function of this Windows thread.
  */
  virtual void main()
  {
    // Call process.nextFrame if no blocking receivers are waiting
    process.setBlockingId(31);
    process.setEventId(31);
    setPriority(process.getPriority());
    usleep(1000); // always leave processing time to other threads
    while(this->isRunning() && process.isWaiting())
    {
      if(process.getFirstSender())
        process.getFirstSender()->checkAllForRequests();
      if(process.getFirstReceiver())
        process.getFirstReceiver()->checkAllForPackages();
      process.checkTime();
      usleep(1000); // always leave processing time to other threads
    }
    process.terminate();
  }

public:
  /**
  * Constructor.
  * @param name The name of the process.
  */
  ProcessFrame(const char* name) : name(name) {}

  /**
  * The functions searches for a sender with a given name.
  * @param senderName The name of the sender.
  * @return If the sender was found, a pointer to it is returned.
  *         Otherwise, the function returns 0.
  */ 
  virtual SenderList* lookupSender(const char* senderName) 
  {return process.getFirstSender() ? process.getFirstSender()->lookup(name,senderName) : 0;}

  /**
  * The functions searches for a receiver with a given name.
  * @param receiverName The name of the receiver.
  * @return If the receiver was found, a pointer to it is returned.
  *         Otherwise, the function returns 0.
  */ 
  virtual ReceiverList* lookupReceiver(const char* receiverName)
  {return process.getFirstReceiver() ? process.getFirstReceiver()->lookup(name,receiverName) : 0;}

  /**
  * The function returns the name of the process.
  * @return the name of the process.
  */
  virtual const char* getName() const {return name;}

  /**
  * The function returns a pointer to the process if it has the given name.
  * @param processName The name of the process that is searched for.
  * @return If the process has the required name, a pointer to it is
  *         returned. Otherwise, the function returns 0.
  */
  virtual PlatformProcess* getProcess(const char* processName)
  {
    if(!strcmp(name,processName))
      return &process;
    else
      return 0;
  }
};

class ProcessList;

/**
* The class is a base class for process creators.
*/
class ProcessCreatorBase
{
private:
  static std::list<ProcessCreatorBase*> list; /**< The list of all process creators. */

protected:
  /**
  * The function creates a process.
  * @return A pointer to the new process.
  */
  virtual ProcessBase* create() = 0;

public:
  /**
  * Constructor.
  */
  ProcessCreatorBase() { 
    list.push_back(this); 
  }

  friend class ProcessList;
};

/**
* The template class instatiates creators for processes of a certain type.
*/
template <class T> class ProcessCreator : public ProcessCreatorBase
{
private:
  const char* name; /**< The name of the process that will be created. */

protected:
  /**
  * The function creates a process.
  * @return A pointer to the new process.
  */
  ProcessBase* create() {
    return new T(name);
  }

public:
  /**
  * Constructor.
  * @param name The name of the process that will be created.
  */
  ProcessCreator(const char* name) {this->name = name;}
};

/**
* The class implements a list of processes.
*/
class ProcessList : public std::list<ProcessBase*>
{
public:
  /**
  * Constructor.
  * Creates a process for each process constructor and inserts them
  * into the list.
  */
  ProcessList()
  {
    for(std::list<ProcessCreatorBase*>::const_iterator i = ProcessCreatorBase::list.begin(); i != ProcessCreatorBase::list.end(); ++i) {
      push_back((*i)->create());
    }
  }

  /**
  * Destructor.
  */
  ~ProcessList()
  {
    for(iterator i = begin(); i != end(); ++i) 
      delete *i;
  }

  /**
  * The function announces to all processes in the list that they should stop.
  */
  void announceStop()
  {
    for(iterator i = begin(); i != end(); ++i) 
      (*i)->announceStop();
  }

  /**
  * The function starts all processes in the list.
  */
  void start() 
  {
    for(iterator i = begin(); i != end(); ++i) 
      (*i)->start();
  }
};

/**
* The macro MAKE_PROCESS instatiates a process creator.
* As a convention, it should be used in the last line of the 
* source file. For each process, MAKE_PROCESS must exactly be used
* once.
* @param className The type of the class that will later be instantiated
*                 as a process.
*/
#define MAKE_PROCESS(className) \
  ProcessCreator<ProcessFrame<className> > _create##className(#className)

#endif
