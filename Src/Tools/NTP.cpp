/**
 * @file Tools/NTP.cpp
 *
 * Representations and functions for time synchronisation inside
 * the team. Implementation of parts of the Network Time Protocol.
 *
 * @author <a href="mailto:timlaue@informatik.uni-bremen.de">Tim Laue</a>
 */ 

#include "NTP.h"

NTP::NTP() :
lastNTPRequestSent(0),
robotNumber(0),
numberOfReceivedNTPRequests(0),
numberOfReceivedNTPResponses(0)
{
  memset(sendTimeStamps, 0, sizeof(sendTimeStamps));
  memset(receiveTimeStamps, 0, sizeof(receiveTimeStamps));
}

void NTP::setSendTimeStamp(int robotNumber, unsigned timeStamp)
{
  if(robotNumber >= 0 && robotNumber < MAX_NUM_OF_NTP_PACKAGES)
    sendTimeStamps[robotNumber] = timeStamp;
}

void NTP::setReceiveTimeStamp(int robotNumber, unsigned timeStamp)
{
  if(robotNumber >= 0 && robotNumber < MAX_NUM_OF_NTP_PACKAGES)
    receiveTimeStamps[robotNumber] = timeStamp;
}

bool NTP::doSynchronization(int robotNumber, unsigned now, OutMessage& out)
{
  this->robotNumber = robotNumber;
  bool sendNTPData(false);

  // Process incoming NTP responses:
  for(int i = 0; i < numberOfReceivedNTPResponses; ++i)
  {
    NTPResponse& response = receivedNTPResponses[i];
    response.responseOrigination = sendTimeStamps[int(response.sender)];
    response.responseReceipt = receiveTimeStamps[int(response.sender)];
    SynchronizationMeasurement syncMeasurement = response.computeSynchronizationMeasurement();
    timeSyncBuffers[int(response.sender)].add(syncMeasurement);
  }
  numberOfReceivedNTPResponses = 0;

  // Process incoming NTP requests:
  for(int i = 0; i < numberOfReceivedNTPRequests; ++i)
  {
    NTPRequest& request = receivedNTPRequests[i];
    request.origination = sendTimeStamps[int(request.sender)];
    request.receipt = receiveTimeStamps[int(request.sender)];

    //Send a response to this request:
    out.bin << NTPResponse(request, robotNumber);
    out.finishMessage(idNTPResponse);
    sendNTPData = true;
  }
  numberOfReceivedNTPRequests = 0;

  // Send NTP requests to teammates?
  if(now - lastNTPRequestSent >= NTP_REQUEST_INTERVAL)
  {
    NTPRequest request;
    request.sender = robotNumber;
    out.bin << request;
    out.finishMessage(idNTPRequest);
    sendNTPData = true;
    lastNTPRequestSent = now;
  }

  return sendNTPData;
}

unsigned NTP::getTeamMateTimeInOwnTime(unsigned teamMateTime, int teamMateNumber)
{
  if(teamMateNumber >= 0 && teamMateNumber < MAX_NUM_OF_NTP_PACKAGES)
  {
    int offset(timeSyncBuffers[teamMateNumber].getBestOffset());
    int localTime = int(teamMateTime) - offset;
    return (unsigned) localTime;
  }
  return 0;
}

bool NTP::handleMessage(InMessage& message)
{
  switch(message.getMessageID())
  {
  case idNTPRequest:
    if(numberOfReceivedNTPRequests < MAX_NUM_OF_NTP_PACKAGES)
    {
      message.bin >> receivedNTPRequests[numberOfReceivedNTPRequests];
      //Check, if the robot has not sent himself a request:
      if(receivedNTPRequests[numberOfReceivedNTPRequests].sender != robotNumber)
        ++numberOfReceivedNTPRequests;
    }
    return true;

  case idNTPResponse:
    if(numberOfReceivedNTPResponses < MAX_NUM_OF_NTP_PACKAGES)
    {
      message.bin >> receivedNTPResponses[numberOfReceivedNTPResponses];
      //Check, if this response belongs to an own request:
      if(receivedNTPResponses[numberOfReceivedNTPResponses].receiver == robotNumber)
        ++numberOfReceivedNTPResponses;
    }
    return true;

  default:
    return false; 
  }
}
