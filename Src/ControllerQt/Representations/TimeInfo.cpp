/**
* @file ControllerQt/Representations/TimeInfo.cpp
*
* Implementation of class TimeInfo
*
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
*/

#include "TimeInfo.h"
#include "Tools/MessageQueue/InMessage.h"
#include "Platform/SystemCall.h"
#include <cfloat>

TimeInfo::TimeInfo()
{
  reset();
}

void TimeInfo::reset()
{
  infos.clear();
  timeStamp = 0;
}

void TimeInfo::reset(Info& info)
{
  info.entries.init();
  info.lastReceived = 0;
}

bool TimeInfo::handleMessage(InMessage& message)
{
  if(message.getMessageID() == idStopwatch)
  {
    unsigned time1, time2, counter1;
    std::string id;

    message.bin >> id >> time1 >> time2 >> counter1;
    
    Info& info = infos[id];
    if(SystemCall::getTimeSince(info.lastReceived) > 2000)
      reset(info);
    info.entries.add(Entry(time1, time2, counter1));
    timeStamp = info.lastReceived = SystemCall::getCurrentSystemTime();
    return true;
  }
  else
    return false;
}

bool TimeInfo::getStatistics(const Info& info, double& minTime, double& maxTime, double& avgTime, double& minDelta, double& maxDelta, double& avgFreq) const
{
  if(SystemCall::getTimeSince(info.lastReceived) < 1000 && info.entries.getNumberOfEntries() > 1)
  {
    const Entry& first = info.entries[0];
    //const Entry& second = info.entries[1];
    avgTime = minTime = maxTime = first.endTime - first.startTime;
    minDelta = DBL_MAX;
    maxDelta = 0;
    double avgDelta = 0;
    for(int i = 1; i < info.entries.getNumberOfEntries(); ++i)
    {
      unsigned duration = info.entries[i].endTime - info.entries[i].startTime;
      if(duration < minTime)
        minTime = duration;
      if(duration > maxTime)
        maxTime = duration;
      avgTime += duration;
      double delta = (double)(info.entries[i - 1].startTime - info.entries[i].startTime) / (double)(info.entries[i - 1].counter - info.entries[i].counter);
      if(delta < minDelta)
        minDelta = delta;
      if(delta > maxDelta)
        maxDelta = delta;
      avgDelta += delta;
    }
    avgTime /= info.entries.getNumberOfEntries();
    avgDelta /= (info.entries.getNumberOfEntries() - 1);
    avgFreq = 1000 / avgDelta;
    return true;
  }
  else
    return false;
}
