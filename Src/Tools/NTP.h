/**
 * @file Tools/NTP.h
 *
 * Representations anf functions for time synchronisation inside
 * the team. Implementation of parts of the Network Time Protocol.
 *
 * @author <a href="mailto:timlaue@informatik.uni-bremen.de">Tim Laue</a>
 */ 

#ifndef __NTP_h_
#define __NTP_h_

#include "Tools/RingBuffer.h"
#include "Tools/Streams/Streamable.h"
#include "Tools/MessageQueue/MessageQueue.h"

/**
* @class SynchronizationMeasurement
*
* A small datatype describing a measurement of
* the time offset between two robots
*/
class SynchronizationMeasurement : public Streamable  
{
public:
  /** Constructor */
  SynchronizationMeasurement() : offset(0), roundTrip(0) {}

  /** Offset of the time of another robot relative to the own time*/
  int offset;
  /** The time, the two NTP messages have been in the WLAN*/
  int roundTrip;

  virtual void serialize(In* in, Out* out)
  {  
    STREAM_REGISTER_BEGIN();
    STREAM(offset);
    STREAM(roundTrip);
    STREAM_REGISTER_FINISH();
  }
};


/**
* @class NTPRequest
*
* A package for starting a synchronization measurement
*/
class NTPRequest : public Streamable
{
public:
  /** The robot, which sent the request*/
  char sender;
  /** The timestamp of the generation of this message.
   *  Should be set as late as possible before transmitting to WLAN*/
  unsigned origination;
  /** The timestamp of the receipt of a request by another robot.
   *  Is not set (and transmitted) by the requesting robot. The receiving 
   *  robot sets this timestamp as early as possible after receiving from WLAN*/
  unsigned receipt;

  /** Constructor*/
  NTPRequest()
  {
    sender = -1;
    origination = 0;
    receipt = 0;
  }

  virtual void serialize(In *in, Out *out)
  {
    STREAM_REGISTER_BEGIN();
      STREAM(sender);
      //Other elements will not be streamed but set after receipt
    STREAM_REGISTER_FINISH();
  }
};

/**
* @class NTPResponse
*
* A package for replying to an NTPRequest
*/
class NTPResponse : public Streamable
{
public:
  /** The robot, to which this message should be sent*/
  char receiver;
  /** The robot, which sent the response*/
  char sender;
  /** The timestamp of the generation of the request*/
  unsigned requestOrigination;
  /** The timestamp of the receipt of the request*/
  unsigned requestReceipt;
  /** The timestamp of the generation of this message.
   *  Should be set as late as possible before transmitting to WLAN*/
  unsigned responseOrigination;
  /** The timestamp of the receipt of the response by the robot which sent the request.
   *  Is not set (and transmitted) by the responding robot. The receiving 
   *  robot sets this timestamp as early as possible after receiving from WLAN*/
  unsigned responseReceipt;

  /** Empty standard constructor*/
  NTPResponse() 
  {
    sender = -1;
    receiver = -1;
    requestOrigination = 0;
    requestReceipt = 0;
    responseOrigination = 0;
    responseReceipt = 0;
  }

  /** Constructor
  * @param request The request which will be responded
  * @param responseSender The robot that sent the response.
  */
  NTPResponse(const NTPRequest& request, char responseSender)
  {
    init(request, responseSender);
  }

  /** Initializes the two request timestamps
  * @param request The request which will be responded
  * @param responseSender The robot that sent the response.
  */
  void init(const NTPRequest& request, char responseSender)
  {
    requestOrigination = request.origination;
    requestReceipt = request.receipt;
    receiver = request.sender;
    sender = responseSender;
  }

  /** Computes time offset and round trip time from an NTPResponse 
  * @return Time offset and tround trip
  */
  SynchronizationMeasurement computeSynchronizationMeasurement() const
  {
    SynchronizationMeasurement t;
    t.roundTrip = int(responseReceipt - requestOrigination) -
                  int(responseOrigination - requestReceipt);
    t.offset = int(requestReceipt - requestOrigination +
                responseOrigination - responseReceipt) / 2;
    return t;
  }

  virtual void serialize(In *in, Out *out)
  {
    STREAM_REGISTER_BEGIN();
      STREAM(receiver);
      STREAM(sender);
      STREAM(requestOrigination);
      STREAM(requestReceipt);
      //Other elements will not be streamed but set after receipt
    STREAM_REGISTER_FINISH();
  }
}; 

/**
* @class SynchronizationMeasurementsBuffer
*
* A class for buffering the last synchronization measurements.
* The currently best time offset is the element with the smallest
* round trip time.
*/
class SynchronizationMeasurementsBuffer
{
private:
  /** Constant for internal buffer size*/
  enum {MAX_NUMBER_OF_MEASUREMENTS = 8};
  /** A buffer for the last measurements*/
  RingBuffer<SynchronizationMeasurement,MAX_NUMBER_OF_MEASUREMENTS> buffer;

public:
  /** Adds a new measurement to the ring buffer
  * @param s The measurement
  */
  void add(const SynchronizationMeasurement& s) 
  {
    buffer.add(s);
  }

  /** Indicates whether an appropriate number of measurements is available
  * @return true, if the internal buffer is full
  */
  bool isFull()
  {
    return buffer.getNumberOfEntries() == MAX_NUMBER_OF_MEASUREMENTS;
  }

  /** Returns the offset with the smallest round trip time:
  * OtherTime - Offset = OtherTime in myTime
  * @return An offset. Returns 0, if no measurements are in the buffer
  */
  long getBestOffset() const 
  {
    if(buffer.getNumberOfEntries() == 0)
      return 0;
    int bestOffset(buffer[0].offset);
    int shortestRoundTrip(buffer[0].roundTrip);
    for(int i=1; i<buffer.getNumberOfEntries(); i++)
    {
      if(buffer[i].roundTrip < shortestRoundTrip)
      {
        shortestRoundTrip = buffer[i].roundTrip;
        bestOffset = buffer[i].offset;
      }
    }
    return bestOffset;
  }

  /** Returns the synchronization measurement with the shortest round trip time:
  * @return A Synchronization Measurement. Returns default synchronization measurement, if no measurements are in the buffer
  */
  SynchronizationMeasurement getBestSyncMeasurement() const
  {
    if(buffer.getNumberOfEntries() == 0)
      return SynchronizationMeasurement();

    SynchronizationMeasurement bestSyncMeas(buffer[0]);
    for(int i=0; i<buffer.getNumberOfEntries(); i++)
    {
      if (buffer[i].roundTrip < bestSyncMeas.roundTrip)
        bestSyncMeas = buffer[i];
    }
    return bestSyncMeas;
  }
};

/**
* @class NTP
* Implementantion of the Network Time Protocol.
*/
class NTP : public MessageHandler
{
public:
  enum {MAX_NUM_OF_NTP_PACKAGES = 12, NTP_REQUEST_INTERVAL = 2000};  /**< Some constants for NTP snychronization*/
  unsigned sendTimeStamps[MAX_NUM_OF_NTP_PACKAGES]; /**< The times when messages from different robots have been sent. Players: 1-11, ground truth: 0. */
  unsigned receiveTimeStamps[MAX_NUM_OF_NTP_PACKAGES]; /**< The times when messages from different robots arrived. Players: 1-11, ground truth: 0. */
private:
  unsigned lastNTPRequestSent; /**< The point of time when the last NTP request has been sent to the team*/
  int robotNumber; /**< The number of this robot. */
  int numberOfReceivedNTPRequests; /**< The number of responses received in the current frame*/
  int numberOfReceivedNTPResponses; /**< The number of requests received in the current frame*/
  NTPRequest receivedNTPRequests[MAX_NUM_OF_NTP_PACKAGES]; /**< The requests received in the current frame*/
  NTPResponse receivedNTPResponses[MAX_NUM_OF_NTP_PACKAGES]; /**< The responses receved in the current frame*/
  SynchronizationMeasurementsBuffer timeSyncBuffers[MAX_NUM_OF_NTP_PACKAGES]; /**< A buffer which contains synchronization data for all other robots*/

public:
  /**
  * Default constructor.
  */
  NTP();

  /**
  * Function for setting the time stamp when a package was sent.
  * @param robotNumber The robot that sent the package.
  * @param timeStamp The time when the package was sent.
  */
  void setSendTimeStamp(int robotNumber, unsigned timeStamp);

  /**
  * Function for setting the time stamp when a package was received.
  * @param robotNumber The robot that received the package.
  * @param timeStamp The time when the package was received.
  */
  void setReceiveTimeStamp(int robotNumber, unsigned timeStamp);

  /** 
  * Function for handling all NTP stuff. Called by update().
  * @param robotNumber The number of this robot.
  * @param now The current time.
  * @param out In case packages have to be sent, they will be streamed to this queue.
  * @return Send synchronization package in this frame?
  */
  bool doSynchronization(int robotNumber, unsigned now, OutMessage& out);

  /** Converts a time stamp from a team mate to local time
  * @param teamMateTime The time on the other robot
  * @param teamMateNumber The number of that robot
  * @return The time in local time
  */
  unsigned getTeamMateTimeInOwnTime(unsigned teamMateTime, int teamMateNumber);

  /** 
  * The method is called for every incoming team message.
  * @param message An interface to read the message from the queue.
  * @return true Was the message handled?
  */
  bool handleMessage(InMessage& message);
};

#endif //__NTP_h_
