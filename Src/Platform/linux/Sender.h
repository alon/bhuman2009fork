/** 
 * @file Platform/linux/Sender.h
 *
 * This file declares classes related to senders.
 *
 * @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas R�fer</a>
 */
#ifndef __SENDER_H__
#define __SENDER_H__

#ifndef __ProcessFramework_h__
#error Never include this file directly. Include ProcessFramework.h instead.
#endif

#include "Platform/linux/DebugHandler.h"
#include "Platform/GTAssert.h"

const int RECEIVERS_MAX = 20; /**< The maximum number of receivers connected to a single sender */

class SenderList
{
  private:
    SenderList* next;           /**< The successor of the current sender. */
    char name[NAME_LENGTH_MAX]; /**< The name of a sender without the module's name. */
  
  protected:
    PlatformProcess* process;   /**< The process this sender is associated with. */
    int eventId;                /**< The id of the current sender in the range [0..30]. */
    bool blocking;              /**< Stores whether this is a blocking sender. */

    /**
     * The function sends a package to all receivers that requested it.
     */
    virtual void sendPackage() = 0;

    /**
     * The function checks whether all receivers of this sender are ready to receive the next package.
     */    
    virtual void checkForRequest() = 0;

  public:
    /**
     * The constructor.
     * @param process The process this sender is associated with.
     * @param senderName The Aperios connection name of the sender without the process name.
     * @param blocking Decides whether this sender blocks the execution of the next frame
     *                 until all connected receivers have requested a new package.
     */
    SenderList(PlatformProcess* process,const char* senderName,bool blocking);

    /**
     * Returns the begin of the list of all senders.
     * @return The first sender in the list, or 0 if the list ist empty.
     */
    SenderList*& getFirst();

    /**
     * Returns the next sender in the list.
     * @return The next sender in the list, or 0 if this sender is the last one.
     */
    SenderList* getNext() const {return next;}

    /**
     * Returns the Aperios connection name of the sender.
     * @return The Aperios connection name without the process name ("Sender.type.O")
     */
    const char* getName() const {return name;}

    /**
     * The function must be called to finish the current frame.
     */
    void finishFrame();

    /**
     * The function checks for all senders whether all their receivers are ready to receive the next package.
     */    
    void checkAllForRequests();

    /**
     * The function adds a receiver to this sender.
     * @param r The receiver that is attached to this sender.
     */
    virtual void add(ReceiverList* r) = 0;

    /**
     * The function searches for a sender with the given name.
     * @param processName The name of the sender's process.
     * @param senderName The name of the requested sender.
     * @return If the sender is found, a pointer to it is returned.
     *         Otherwise, the funtion returns 0.
     */ 
    SenderList* lookup(const char* processName,const char* senderName);
};


/**
 * The class implements a sender.
 * A sender is an object that sends packages to an Aperios queue.
 */
template<class T> class Sender : public SenderList, public T
{
  // private:
  protected:
    ReceiverList* receiver[RECEIVERS_MAX], /**< A list of all receivers. */
                * alreadyReceived[RECEIVERS_MAX]; /**< A list of all receivers that have already received the current package. */
    int numOfReceivers, /**< The number of entries in the receiver list. */
        numOfAlreadyReceived; /**< The number of entries in the already received list. */

    /**
     * The function checks whether all receivers of this sender are ready to receive the next package.
     */    
    virtual void checkForRequest()
    {
      for(int i = 0; i < numOfReceivers; ++i)
        if(receiver[i]->hasPendingPackage())
          return;
      process->setEventId(eventId);
    }

    /**
     * The function adds a receiver to this sender.
     * @param r The receiver that is attached to this sender.
     */
    virtual void add(ReceiverList* r)
      {receiver[numOfReceivers++] = r;}

  //protected:
    /**
     * The function sends a package to all receivers that requested it.
     */
    virtual void sendPackage()
    {
      if(numOfAlreadyReceived != -1)
      { // send() has been called at least once
        int i;
        for(i = 0; i < numOfReceivers; ++i)
        {
          if(!receiver[i]->hasPendingPackage())
          {
            int j;
            for(j = 0; j < numOfAlreadyReceived; ++j)
              if(receiver[i] == alreadyReceived[j])
                break;
            if(j == numOfAlreadyReceived)
            { // receiver[i] has not received its requested package yet
              const T& data = *static_cast<const T*>(this);
              OutBinarySize size;
              size << data;
              void* r = (void*) new char[size.getSize()];
              ASSERT(r);
              OutBinaryMemory memory(r);
              memory << data;
              receiver[i]->setPackage(r);
              // note that receiver[i] has received the current package
              ASSERT(numOfAlreadyReceived < RECEIVERS_MAX);
              alreadyReceived[numOfAlreadyReceived++] = receiver[i];
            }
          }
        }
      }
    }
    
  public:
    /**
     * The constructor.
     * @param process The process this sender is associated with.
     * @param senderName The Aperios connection name of the sender without the process name.
     * @param blocking Decides whether this sender blocks the execution of the next frame
     *                 until all connected receivers have requested a new package.
     */
    Sender(PlatformProcess* process,const char* senderName,bool blocking)
    : SenderList(process,senderName,blocking)
    {
      numOfReceivers = 0;
      numOfAlreadyReceived = -1;
    }

    /**
     * Returns whether a new package was requested from the sender.
     * This is always true if this is a blocking sender.
     * @return Has a new package been requested?
     */
    bool requestedNew() const 
    {
      for(int i = 0; i < numOfReceivers; ++i)
        if(!receiver[i]->hasPendingPackage())
          return true;
      return false;
    }

    /**
     * Marks the package for sending and transmits it to all receivers that already requested for it.
     * All other receiver may get it later if they request for it before the package is changed.
     */
    void send()
    {
      process->setBlockingId(eventId,blocking);
      numOfAlreadyReceived = 0;
      sendPackage();
    }

};

#endif
