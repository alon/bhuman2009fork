/**
 * @file OdometryData.cpp
 *
 * Implementation of the OdometryData class
 *
 * @author Max Risler
 */ 

#include "OdometryData.h"

In& operator>>(In& stream,OdometryData& odometryData)
{
  STREAM_REGISTER_BEGIN_EXT(odometryData);
    STREAM_BASE_EXT( stream, (Pose2D&) odometryData);
  STREAM_REGISTER_FINISH();
  return stream;
}
 
Out& operator<<(Out& stream, const OdometryData& odometryData)
{
  STREAM_REGISTER_BEGIN_EXT(odometryData);
    STREAM_BASE_EXT( stream, (Pose2D&) odometryData);
  STREAM_REGISTER_FINISH();
  return stream;
}
