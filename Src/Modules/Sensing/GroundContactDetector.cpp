/**
* @file GroundContactDetector.cpp
* Implementation of module GroundContactDetector.
* @author Colin Graf
*/

#include "GroundContactDetector.h"
#include "Tools/Debugging/DebugDrawings.h"

#include <cfloat>

MAKE_MODULE(GroundContactDetector, Sensing)

GroundContactDetector::GroundContactDetector() : 
  leftRange(FLT_MAX, -FLT_MAX), rightRange(FLT_MAX, -FLT_MAX), framesWithContact(0), framesWithoutContact(0), contactStartTime(0), contact(false), wasContacted(false)
{
  p.minContactPercent = 0.25f;
  p.minFootContactUnity = 2;
  p.minCycleContactUnity = 10;
  p.safeContactTime = 3300;

  InConfigFile stream("groundContact.cfg");
  if(stream.exists())
    stream >> p;
}

void GroundContactDetector::getMinAndMax(const float* data, float& min, float& max)
{
  max = min = *(data++);
  if(*data < min) min = *data;
  if(*data > max) max = *data;
  ++data;
  if(*data < min) min = *data;
  if(*data > max) max = *data;
  ++data;
  if(*data < min) min = *data;
  if(*data > max) max = *data;
}

void GroundContactDetector::update(GroundContactState& groundContactState)
{
  MODIFY("module:GroundContactDetector:parameters", p);
  
  bool lastContact = groundContactState.contact;
  if(theSensorData.data[SensorData::fsrLFL] != SensorData::off && theSensorData.data[SensorData::fsrRFL] != SensorData::off)
  {
    float min, max;
    getMinAndMax(&theSensorData.data[SensorData::fsrLFL], min, max);
    if(min > leftRange.max) leftRange.max = min;
    if(max < leftRange.min) leftRange.min = max;
    getMinAndMax(&theSensorData.data[SensorData::fsrRFL], min, max);
    if(min > rightRange.max) rightRange.max = min;
    if(max < rightRange.min) rightRange.min = max;

    unsigned int leftContact = 0;
    float size = leftRange.getSize();
    if(size > 0.f)
      for(int i = SensorData::fsrLFL; i <= SensorData::fsrLBR; ++i)
        if((theSensorData.data[i] - leftRange.min) / size > p.minContactPercent)
          leftContact++;
    unsigned int rightContact = 0;
    size = rightRange.getSize();
    if(size > 0.f)
      for(int i = SensorData::fsrRFL; i <= SensorData::fsrRBR; ++i)
        if((theSensorData.data[i] - rightRange.min) / size > p.minContactPercent)
          rightContact++;

    //MODIFY("module:GroundContactDetector:leftMin", leftRange.min);
    //MODIFY("module:GroundContactDetector:leftMax", leftRange.max);
    //MODIFY("module:GroundContactDetector:rightMin", rightRange.min);
    //MODIFY("module:GroundContactDetector:rightMax", rightRange.max);

    //MODIFY("module:GroundContactDetector:leftContact", leftContact);
    //MODIFY("module:GroundContactDetector:rightContact", rightContact);

    if(leftContact >= p.minFootContactUnity || rightContact >= p.minFootContactUnity) // contact 
    {
      framesWithContact++;
      framesWithoutContact = 0;
    }
    else
    {
      framesWithContact = 0;
      framesWithoutContact++;
    }

    if(contact)
      contact = framesWithoutContact <= p.minCycleContactUnity;
    else
      contact = framesWithContact > p.minCycleContactUnity;

    if(contact)
      wasContacted = true;

    groundContactState.contact = contact || !wasContacted || theMotionRequest.motion == MotionRequest::specialAction || theMotionInfo.executedMotionRequest.motion == MotionRequest::specialAction;
  }
  else
    groundContactState.contact = true;

  if((groundContactState.contact && !lastContact) || (groundContactState.contact && contactStartTime == 0))
    contactStartTime = theFrameInfo.time;
  groundContactState.contactSafe = groundContactState.contact && theFrameInfo.getTimeSince(contactStartTime) >= p.safeContactTime;

  PLOT("module:GroundContactDetector:groundContact", groundContactState.contact ? 0.75 : 0.25);
}
