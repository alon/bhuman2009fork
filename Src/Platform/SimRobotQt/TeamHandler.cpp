/** 
* @file Platform/SimRobotQt/TeamHandler.cpp
* The file implements a class for team communication between simulated robots.
* @author <A href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</A>
*/

#include "TeamHandler.h"
#include "Tools/Streams/OutStreams.h"
#include "Tools/Streams/InStreams.h"
#include "Platform/GTAssert.h"

TeamHandler::TeamHandler(MessageQueue& in, MessageQueue& out, PlatformProcess* process) :
in(in),
out(out),
process(process)
{
  process->setBroadcastReceiver(this, &BroadcastReceiver::onReceive);
}

void TeamHandler::send()
{
  if(!out.isEmpty())
  {
    OutBinarySize size;
    size << out;
    char* package = new char[size.getSize()];
    ASSERT(package);
    OutBinaryMemory memory(package);
    memory << out;
    out.clear();
    process->sendBroadcastPackage(package, size.getSize());
    delete [] package;
  }
}

void TeamHandler::onReceive(const char* data)
{
  in.out.bin << SystemCall::getCurrentSystemTime();
  in.out.finishMessage(idReceiveTimeStamp);
  InBinaryMemory memory(data);
  memory >> in;
}
