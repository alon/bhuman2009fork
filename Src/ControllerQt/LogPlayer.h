/**
* @file LogPlayer.h
*
* Definition of class LogPlayer
*
* @author Martin L�tzsch
*/

#ifndef LogPlayer_h_
#define LogPlayer_h_

#include "Tools/MessageQueue/MessageQueue.h"

/**
* @class LogPlayer
*
* A message queue that can record and play logfiles.
* The messages are played in the same time sequence as they were recorded.
*
* @author Martin L�tzsch
*/
class LogPlayer : public MessageQueue
{
public:
  /** 
  * Constructor 
  * @param targetQueue The queue into that messages from played logfiles shall be stored.
  */
  LogPlayer(MessageQueue& targetQueue);

  /** Deletes all messages from the queue */
  void init();

  /** 
  * Opens a log file and reads all messages into the queue.
  * @param fileName the name of the file to open
  * @return if the reading was successful
  */
  bool open(const char* fileName);

  /** 
  * Playes the queue. 
  * Note that you have to call replay() regularely if you want to use that function
  */
  void play();

  /** Pauses playing the queue. */
  void pause();

  /** Stops playing the queue, resets the position in the queue to the first message */
  void stop();

  /** Stops recording, resets the position to the first message */
  void recordStop();

  /** Starts recording.
   * Note that you have to notify the queue on new messages with handleMessage().
   */
  void recordStart();

  /** Plays the next message in the queue */
  void stepForward();

  /** Plays the previous message in the queue */
  void stepBackward();

  /** repeats the current message in the queue */
  void stepRepeat();

  /** jumps to given message-number in the queue */
  void gotoFrame(int frame);

  /** Set loop mode. If disabled the logfile is played only once. */
  void setLoop(bool);

  /**
  * Writes all messages in the log player queue to a log file.
  * @param fileName the name of the file to write
  * @return if the writing was successful
  */
  bool save(const char* fileName);

  /**
  * Writes all images in the log player queue to a bunch of files (*.bmp or *.jpg).
  * @param raw Savecolor unconverted
  * @param fileName the name of one file to write, all files will be enumerated by appending a 3 digit number to the filename.
  * @return if the writing of all files was successful
  */
  bool saveImages(const bool raw, const char* fileName);

  /**
  * Adds the message to the queue depending on isRecording. 
  * That function should be called for every message in the queue that the 
  * log player shall work on.
  */
  void handleMessage(InMessage& message);

  /** 
  * If playing a log file, that function checks if it is time to release the next
  * message dependend on the time stamp. Call that function whenever there is some
  * processing time left.
  * @return Was log data replayed?
  */
  bool replay();

  /**
  * The functions filters the message queue.
  * @param messageIDs An null-terminated array of message ids that should be kept.
  */
  void keep(MessageID* messageIDs);

  /**
  * The functions filters the message queue.
  * @param messageIDs An null-terminated array of message ids that should be removed.
  */
  void remove(MessageID* messageIDs);

  /**
  * The function creates a histogram on the message ids contained in the log file.
  * @param frequency An array that is filled with the frequency of message ids.
  */
  void statistics(int frequency[numOfDataMessageIDs]);

  /** different states of the logplayer */
  enum LogPlayerState
  {
    initial, recording, paused, playing
  };

  LogPlayerState state; /**< The state of the log player. */
  int currentFrameNumber; /**< The number of the current frame. */
  int numberOfFrames; /**< The overall number of frames available. */

private:
  MessageQueue& targetQueue; /**< The queue into that messages from played logfiles shall be stored. */
  int currentMessageNumber; /**< The current message number in the message queue. */
  int numberOfMessagesWithinCompleteFrames; /**< The number of messages within complete frames. Messages behind that number will be skipped. */
  bool loop;

  /**
  * The method counts the number of frames.
  */
  void countFrames();
};

#endif //LogPlayer_h_
