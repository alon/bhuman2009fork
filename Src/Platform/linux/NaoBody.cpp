/**
* @file Platform/linux/NaoBody.cpp
* Declaration of a class for accessing the body of the nao via NaoQi/libbhuman.
* @author Colin Graf
* @author jeff
*/

#include <sys/mman.h>
#include <fcntl.h>
#include <semaphore.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <stdio.h>

#include "libbhuman/bhuman.h"
#include "NaoBody.h"
#include "GTAssert.h"

int NaoBody::fd = -1;
void* NaoBody::sem = SEM_FAILED;
LbhData* NaoBody::lbhData = (LbhData*)MAP_FAILED;
int NaoBody::allocations = 0;

unsigned int NaoBody::cognitionThreadId = 0;
unsigned int NaoBody::motionThreadId = 0;

NaoBody::NaoBody() : opened(false), writingActuators(-1)
{
  if(lbhData != MAP_FAILED)
  {
    opened = true;
    ++allocations;
  }
}

NaoBody::~NaoBody()
{
  if(opened)
    if(--allocations == 0)
    {
      if(lbhData != MAP_FAILED)
        munmap(lbhData, sizeof(LbhData));
      if(fd != -1)
        close(fd);
      if(sem != SEM_FAILED)
        sem_close((sem_t*)sem);

      fd = -1;
      sem = SEM_FAILED;
      lbhData = (LbhData*)MAP_FAILED;
    }
}

bool NaoBody::init()
{
  if(opened)
    return true; // already initialized

  if(lbhData != MAP_FAILED)
  {
    opened = true;
    ++allocations;
    return true;
  }

  fd = shm_open(LBH_MEM_NAME, O_RDWR, S_IRUSR | S_IWUSR);
  if(fd == -1)
    return false;

  sem = sem_open(LBH_SEM_NAME, O_RDWR, S_IRUSR | S_IWUSR, 0);
  if(sem == SEM_FAILED)
  {
    close(fd);
    fd = -1;
    return false;
  }

  VERIFY((lbhData = (LbhData*)mmap(NULL, sizeof(LbhData), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) != MAP_FAILED);
  lbhData->state = ok;
#ifdef RELEASE
  lbhData->release = true;
#else
  lbhData->release = false;
#endif

  opened = true;
  ++allocations;
  return true;
}

void NaoBody::setCrashed(int termSignal)
{
  lbhData->state = BHState(termSignal);
}

void NaoBody::wait()
{
  ASSERT(opened);
  sem_wait((sem_t*)sem);
}

const char* NaoBody::getName() const
{
  ASSERT(opened);
  return lbhData->robotName;
}

float* NaoBody::getSensors()
{
  ASSERT(opened);
  lbhData->readingSensors = lbhData->actualSensors;
  return lbhData->sensors[lbhData->readingSensors];
}

void NaoBody::openActuators(float*& actuators, float*& usActuator)
{
  ASSERT(opened);
  ASSERT(writingActuators == -1);
  writingActuators = 0;
  if(writingActuators == lbhData->actualActuators)
    ++writingActuators;
  if(writingActuators == lbhData->readingActuators)
    if(++writingActuators == lbhData->actualActuators)
      ++writingActuators;
  ASSERT(writingActuators != lbhData->actualActuators);
  ASSERT(writingActuators != lbhData->readingActuators);
  actuators = lbhData->actuators[writingActuators];
  usActuator = &lbhData->usActuator[writingActuators];
}

void NaoBody::closeActuators()
{
  ASSERT(writingActuators >= 0);
  lbhData->actualActuators = writingActuators;
  writingActuators = -1;
}

bool NaoBody::initTrace(const char* proc)
{
  if(!strcmp(proc, "Cognition"))
    cognitionThreadId = pthread_self();
  else if(!strcmp(proc, "Motion"))
    motionThreadId = pthread_self();
  else
    return false;
  return true;
}

void NaoBody::traceMsg(const char* file, const int line, const char* msg)
{
  unsigned int threadId = pthread_self();
  if(threadId == cognitionThreadId) 
  { 
    if(lbhData->cognitionTraceCounter == LBH_TRACE_SIZE)
      lbhData->cognitionTraceCounter = 0;
    snprintf(lbhData->cognitionTrace + lbhData->cognitionTraceCounter * LBH_TRACE_MSG_LENGTH,
          LBH_TRACE_MSG_LENGTH,
          "%s:%d  %s", file, line, msg);
    lbhData->cognitionTraceCounter++;
  }
  if(threadId == motionThreadId)
  {
    if(lbhData->motionTraceCounter == LBH_TRACE_SIZE)
      lbhData->motionTraceCounter = 0;
    snprintf(lbhData->motionTrace + lbhData->motionTraceCounter * LBH_TRACE_MSG_LENGTH,
             LBH_TRACE_MSG_LENGTH,
             "%s:%d  %s", file, line, msg);
    lbhData->motionTraceCounter++;
  }
}

void NaoBody::dumpTrace() const
{
  for(FILE* fp = stderr;;)
  {
    fprintf(fp, "---- COGNITION TRACE ----\n");
    for(int i = 0; i < LBH_TRACE_SIZE; i++)
      fprintf(fp, "%d: %s\n", i, lbhData->cognitionTrace+((lbhData->cognitionTraceCounter+i)%LBH_TRACE_SIZE)*LBH_TRACE_MSG_LENGTH);
    fprintf(fp, "---- MOTION TRACE ----\n");
    for(int i = 0; i < LBH_TRACE_SIZE; i++)
      fprintf(fp, "%d: %s\n", i, lbhData->motionTrace+((lbhData->motionTraceCounter+i)%LBH_TRACE_SIZE)*LBH_TRACE_MSG_LENGTH);
    fprintf(fp, "---- %s ----\n", getBHStateName(int(lbhData->state)));
    if(fp == stderr)
    {
      fp = fopen("/var/log/bhuman", "w");
      if(fp == 0)
        return;
    }
    else
    {
      fclose(fp);
      return;
    }
  }
}

const char* NaoBody::getBHStateName(int bhState)
{
  switch(bhState)
  {
  case ok:
    return "ok";
  case abnormalTermination:
    return "abnormalTermination";
  case sigINT:
    return "sigINT";
  case sigQUIT:
    return "sigQUIT";
  case sigILL:
    return "sigILL";
  case sigABRT:
    return "sigABRT";
  case sigFPE:
    return "sigFPE";
  case sigKILL:
    return "sigKILL";
  case sigSEGV:
    return "sigSEGV";
  case sigPIPE:
    return "sigPIPE";
  case sigALRM:
    return "sigALRM";
  case sigTERM:
    return "sigTERM";
  default:
    return "unknown";
  }
}
