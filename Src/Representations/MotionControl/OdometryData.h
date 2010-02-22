/**
* @file OdometryData.h
* Contains the OdometryData class.
*
* @author Max Risler
*/

#ifndef __OdometryData_h_
#define __OdometryData_h_

#include "Tools/Math/Pose2D.h"
#include "Tools/Streams/InOut.h"

/**
* @class OdometryData
* OdometryData contains an approximation of overall movement the robot has done.
* @attention Only use differences of OdometryData at different times.
* Position in mm
*/
class OdometryData : public Pose2D
{
public:
  bool operator==(const OdometryData& other) const 
  {
    return (const Pose2D&) *this == other;
  }
  
  bool operator!=(const OdometryData& other) const
  {
    return !(*this == other);
  }
};


/**
* Streaming operator that reads a OdometryData from a stream.
* @param stream The stream from which is read.
* @param odometryData The OdometryData object.
* @return The stream.
*/ 
In& operator>>(In& stream,OdometryData& odometryData);

/**
* Streaming operator that writes a OdometryData to a stream.
* @param stream The stream to write on.
* @param odometryData The OdometryData object.
* @return The stream.
*/ 
Out& operator<<(Out& stream, const OdometryData& odometryData);

/**
* @class GroundTruthOdometryData
* Contains an observed overall movement the robot has done.
*/
class GroundTruthOdometryData : public OdometryData {};

#endif //__OdometryData_h_
