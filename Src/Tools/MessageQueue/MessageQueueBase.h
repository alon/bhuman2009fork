/**
* @file MessageQueueBase.h
* Declaration of the class that performs the memory management for the class MessageQueue.
* @author Martin Lötzsch
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
*/

#ifndef __MessageQueueBase_h_
#define __MessageQueueBase_h_

#include "MessageIDs.h"
#include <stdlib.h>

class MessageQueueBase;

/**
* @class MessageQueueBase 
* The class performs the memory management for the class MessageQueue.
* On Windows, the queue will grow when needed, on the robot, it will remain constant at
* the size defined by setSize() and reject further messages.
*/
class MessageQueueBase
{
public:
  /**
  * Default constructor.
  */
  MessageQueueBase();

  /**
  * Destructor.
  */
  ~MessageQueueBase() {if(buf) free(buf);}

  /**
  * Sets the size of the queue. Ignored on the Win32 platform.
  * @param size The maximum size of the queue in bytes. 
  */
  void setSize(size_t size);

  /** 
  * The method removes all messages from the queue.
  */
  void clear();

  /**
  * The method removes a message from the queue.
  * @param message The number of the message.
  */
  void removeMessage(int message);

  /**
  * The method reserves a number of bytes in the message queue.
  * @param size The number of bytes to reserve.
  * @return The address of the reserved space or 0 if there was no room.
  */
  char* reserve(size_t size);

  /**
  * The method adds a number of bytes to the last message in the queue.
  * @param p The address the data is located at.
  * @param size The number of bytes to be written.
  */
  void write(const void* p, int size);

  /** 
  * The method finishes the last message in the queue. 
  * The number of messages is increased, and a new message can be started.
  * @param id The type id of the message.
  */
  void finishMessage(MessageID id);

  /**
  * The method cancels the current message.
  */
  void cancelMessage() {writePosition = 0;}

  /** 
  * The method returns whether the the currently selected message for reading was read completely.
  * @return Has the end of the message been reached?
  */
  bool eof() const {return readPosition == getMessageSize();}

  /** 
  * The method reads a number of bytes from the currently selected message for reading.
  * @param p The address the data is written to. Note that p must point to a memory area
  *          that is at least "size" bytes large.
  * @param size The number of bytes to be read.
  */
  void read(void* p, int size); 

  /**
  * The method gives direct read access to the selected message for reading.
  * @return The address of the first byte of the message
  */
  const char* getData() const {return buf + selectedMessageForReadingPosition + headerSize;}

  /**
  * The method returns the message id of the currently selected message for reading.
  * @return The message id.
  */
  MessageID getMessageID() const {return MessageID(buf[selectedMessageForReadingPosition]);}

  /** 
  * The method returns the message size of the currently selected message for reading.
  * @return The size in bytes.
  */
  int getMessageSize() const;

  /** 
  * The method resets read position of the currently selected message for reading
  * so that the message can be read again.
  */
  void resetReadPosition() {readPosition = 0;}

  /**
  * The method selects a message for reading.
  * @param message The number of the message that is selected.
  */
  void setSelectedMessageForReading(int message);

  /**
  * The method returns the selected message for reading.
  * @return The number of the message that is selected.
  */
  int getSelectedMessageForReading() const {return lastMessage;}

  /** 
  * The method deletes older messages from the queue if newer messages of same type
  * are already in the queue. However, some message types remain untouched.
  * This method should not be called during message handling.
  */
  void removeRepetitions();

private:
  enum {headerSize = 4}; /**< The size of the header of each message in bytes. */
  char* buf; /**< The buffer on that the queue works. */
  size_t selectedMessageForReadingPosition; /**< The position of the message that is selected for reading. */
  size_t maximumSize; /**< The maximum queue size (in bytes). */
  size_t reservedSize; /**< The queue size reserved (in bytes). */
  size_t usedSize; /** The queue size used (in bytes). It is also the position where the next message starts. */
  size_t writePosition; /**< The current size of the next message. */
  bool writingOfLastMessageFailed; /**< If true, then the writing of the last message failed because there was not enough space. */
  int readPosition; /**< The position up to where a message is already read. */
  int lastMessage; /**< Cache the current message in the message queue. */
  int numberOfMessages; /**< The number of messages stored. */

  friend class MessageQueue;
};

#endif // __MessageQueueBase_h_
