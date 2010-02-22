/**
* @file Main.cpp
* Implementation of a NaoQi module that provides basic ipc NaoQi DCM access via semaphore and shared memory.
* @author Colin Graf
*/

#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <semaphore.h>
#include <signal.h>
#include <sys/resource.h>

#include <albroker.h>
#include <alproxy.h>
#include <almemoryproxy.h>
#include <almemoryfastaccess.h>

#include "bhuman.h"

#define ALLOWED_FRAMEDROPS 3

int fd = -1;
LbhData* data = (LbhData*)MAP_FAILED;
sem_t* sem = SEM_FAILED;

AL::ALProxy* proxy = 0;
AL::ALValue* request = 0;
AL::ALValue* usRequest = 0;
ALMemoryFastAccess* memory = 0;
std::vector<float> sensorValues;
int timeOffset;
int frameDrops = ALLOWED_FRAMEDROPS + 1;
int startInterpolation = -1;
const unsigned int interpolationTime = 2000;
unsigned int startUnpressed = 0; /**< Last time when start button was not pressed. */
bool shuttingDown = false; /**< Is Nao shutting down? */
bool sitting = true; /**< Whether the sitdown motion was completed. */
unsigned int rightEarLEDsChanged = 0; /**< Last time when the right ear leds were changed by the B-Human code. */
float rightEarLEDs[10]; /**< The previous state of the right ear LEDs. */

inline unsigned int getSystemTime()
{
  static unsigned int base = 0;
  struct timeval tv;
  gettimeofday(&tv, 0);
  unsigned int time = (unsigned int) (tv.tv_sec * 1000 + tv.tv_usec / 1000);
  if(!base)
    base = time - 10000; // avoid time == 0, because it is often used as a marker
  return time - base;
}

void setEyeLeds()
{
  for(int i = 42; i < 90; i++)
    (*request)[0][5][i][0] = 0.f;

  const unsigned int now = getSystemTime();
  if(data->state != ok)
  {
    // set the "libbhuman is active and bhuman crashed" leds
    float blink = now / 500 & 1;
    for(int i = 42; i <= 49; i++)
      (*request)[0][5][i][0] = data->actuators[data->actualActuators][i] = blink;
    for(int i = 66; i <= 73; i++)
      (*request)[0][5][i][0] = data->actuators[data->actualActuators][i] = 1 - blink;
  }
  else
  {
    // set the "libbhuman is active and bhuman is not running" leds
    (*request)[0][5][62][0] = data->actuators[data->actualActuators][62] = 1.f;
    (*request)[0][5][86][0] = data->actuators[data->actualActuators][62] = 1.f;
  }
}

void setBatteryLeds()
{
  if(getSystemTime() - rightEarLEDsChanged > 5000)
    for(int i = 0; i < sensorValues[69]*10 && i < 10; i++)
      (*request)[0][5][100+i][0] = 1.f;
}

bool sitDown(int now)
{
  //maybe we should set the hardness to something like 50, to
  //avoid making-servos-kap0tt, when the last position was for example
  //the goalie action
  const double* to = sitDownAngles;
  double ratio = (now - startInterpolation)/(double)interpolationTime;
  if(ratio < 1)
  {
    for(int i = 0; i < 42; i+=2)
      (*request)[0][5][i][0] = data->actuators[data->actualActuators][i] * (1-ratio) + to[i/2] * ratio;
    return false;
  }
  else 
  {
    for(int i = 0; i < lbhNumOfActuators; ++i)
      (*request)[0][5][i][0] = data->actuators[data->actualActuators][i] = 0.;
    return true;
  }

}

void onPreProcess()
{
  // set all actuator values according to the values in the shared memory block
  if(data != MAP_FAILED && proxy && request)
    try
    {
      const unsigned int now = getSystemTime();
      data->readingActuators = data->actualActuators;

      (*request)[0][4][0] = (int)now + timeOffset + 0; // 0 delay!
      if(frameDrops <= ALLOWED_FRAMEDROPS && sitting) // only set actuators, when ./bhuman is running; allowing only ALLOWED_FRAMEDROPS missed frames
      {
        for(int i = 0; i < lbhNumOfActuators; ++i)
          (*request)[0][5][i][0] = shuttingDown ? 0.0f : data->actuators[data->readingActuators][i];
        for(int i = 100; i < 110; ++i)
        {
          if(data->actuators[data->readingActuators][i] != rightEarLEDs[i - 100])
            rightEarLEDsChanged = now;
          rightEarLEDs[i - 100] = data->actuators[data->readingActuators][i];
        }
        startInterpolation = -1;
      }
      else // do something smart if bhuman is not responding
      {
        if(startInterpolation == -1)
        {
          startInterpolation = now;
          sitting = false;
          if(sem != SEM_FAILED)
            while(sem_trywait(sem) == 0);
        }

        if(sitDown(now))
          sitting = true;
        setEyeLeds();
      }
      setBatteryLeds();
      proxy->genericCall("setAlias", *request);

      static float lastUsActuator = -1.f;
      if(frameDrops <= ALLOWED_FRAMEDROPS && lastUsActuator != data->usActuator[data->readingActuators] && data->usActuator[data->readingActuators] >= 0.f)
      {
        lastUsActuator = data->usActuator[data->readingActuators];
        (*usRequest)[0][4][0] = (int) now + timeOffset + 0;
        (*usRequest)[0][5][0][0] = lastUsActuator;
        proxy->genericCall("setAlias", *usRequest);
      }
    }
    catch(AL::ALError& e)
    {
    }
}

void onPostProcess()
{
  // get new sensor values and copy them to the shared memory block
  if(data != MAP_FAILED && memory)
    try
    {
      memory->GetValues(sensorValues);
      int writingSensors = 0;
      if(writingSensors == data->actualSensors)
        ++writingSensors;
      if(writingSensors == data->readingSensors)
        if(++writingSensors == data->actualSensors)
          ++writingSensors;
      assert(writingSensors != data->actualSensors);
      assert(writingSensors != data->readingSensors);
      for(int i = 0; i < lbhNumOfSensors; ++i)
        data->sensors[writingSensors][i] = sensorValues[i];
      data->actualSensors = writingSensors;

      // detect shutdown request via chest-button
      const unsigned int now = getSystemTime();
      if(!sensorValues[85]) // chestButton
        startUnpressed = now;
      else if(startUnpressed && !shuttingDown && now - startUnpressed > 3000)
      {
        if(sensorValues[81] || sensorValues[82]) // right foot bumper
          system("harakiri --deep && halt") != 0;
        else if(sensorValues[83] || sensorValues[84]) // left foot bumper
          system("shutdown -n -r now") != 0;
        else
          system("halt") != 0;
        shuttingDown = true;
      }
    }
    catch(AL::ALError& e)
    {
    }
  
  // raise the semaphore
  if(sem != SEM_FAILED && sitting)
  {
    int sval;
    if(sem_getvalue(sem, &sval) == 0)
    {
      if(sval < 1)
      {
        sem_post(sem);
        frameDrops = 0;
      }
      else
      {
        if(frameDrops == 0)
          fprintf(stderr, "libbhuman: dropped frame.\n");
        frameDrops++;        
      }
    }
  }
}

void close()
{
  fprintf(stderr, "libbhuman: Stop.\n");

  // unmap the shared memory
  if(data != MAP_FAILED)
  {
    munmap(data, sizeof(LbhData));
    data = (LbhData*)MAP_FAILED;
  }
  
  // close shared memory
  if(fd != -1)
  {
    close(fd);
    fd = -1;
  }
    
  // close semaphore
  if(sem != SEM_FAILED)
  {
    sem_close(sem);
    sem = SEM_FAILED;
  }
  
  //
  if(proxy != 0)
  {
    delete proxy;
    proxy = 0;
  }
  if(request != 0)
  {
    delete request;
    request = 0;
  }
  if(usRequest != 0)
  {
    delete usRequest;
    usRequest = 0;
  }
  if(memory != 0)
  {
    delete memory;
    memory = 0;
  }
  
  fprintf(stderr, "libbhuman: Stopped.\n");
}

int create(AL::ALPtr<AL::ALBroker> pBroker)
{
  fprintf(stderr, "libbhuman: Start.\n");

  // created shared memory
  if((fd = shm_open(LBH_MEM_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR)) == -1)
  {
    perror("libbhuman: shm_open");
    close();
    return 0;
  }

  if(ftruncate(fd, sizeof(LbhData)) == -1)
  {
    perror("libbhuman: ftruncate");
    close();
    return 0;
  }
  
  // map the shared memory
  if((data = (LbhData*)mmap(NULL, sizeof(LbhData), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED)
  {
    perror("libbhuman: mmap");
    close();
    return 0;
  }
  memset(data, 0, sizeof(LbhData));
  
  // open semaphore
  if((sem = sem_open(LBH_SEM_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, 0)) == SEM_FAILED)
  {
    perror("libbhuman: sem_open");
    close();
    return 0;
  }
  
  try
  {
    // get the robot name
    {
      AL::ALMemoryProxy* proxy = new AL::ALMemoryProxy(pBroker);
      std::string robotName = (std::string)proxy->getData("Device/DeviceList/ChestBoard/BodyNickName",0);
      int len = std::min<>(robotName.length(), sizeof(data->robotName) - 1);
      memcpy(data->robotName, robotName.c_str(), len);
      data->robotName[len] = '\0';
      delete proxy;
    }
    
    // create "allActuators" and "usRequest" alias
    proxy = new AL::ALProxy(pBroker, "DCM");
    {
      AL::ALValue params;
      params.arraySetSize(1);
      params[0].arraySetSize(2);
      params[0][0] = "allActuators";
      params[0][1].arraySetSize(lbhNumOfActuators);
      for(int i = 0; i < lbhNumOfActuators; ++i)
        params[0][1][i] = std::string(lbhActuatorNames[i]);
      proxy->genericCall("createAlias", params);

      params[0][0] = "usRequest";
      params[0][1].arraySetSize(1);
      params[0][1][0] = std::string(lbhUSActuatorNames[0]);
      proxy->genericCall("createAlias", params);
    }
    
    // prepare "allActuators" request
    request = new AL::ALValue;
    request->arraySetSize(1);
    (*request)[0].arraySetSize(6);
    (*request)[0][0] = "allActuators";
    (*request)[0][1] = "ClearAll"; //"Merge";
    (*request)[0][2] = "time-separate";
    (*request)[0][3] = 0;
    (*request)[0][4].arraySetSize(1);
    (*request)[0][5].arraySetSize(lbhNumOfActuators);
    for(int i = 0; i < lbhNumOfActuators; ++i)
      (*request)[0][5][i].arraySetSize(1);

    usRequest = new AL::ALValue;
    usRequest->arraySetSize(1);
    (*usRequest)[0].arraySetSize(6);
    (*usRequest)[0][0] = "usRequest";
    (*usRequest)[0][1] = "Merge"; // doesn't work with "ClearAll"
    (*usRequest)[0][2] = "time-separate";
    (*usRequest)[0][3] = 0;
    (*usRequest)[0][4].arraySetSize(1);
    (*usRequest)[0][5].arraySetSize(1);
    (*usRequest)[0][5][0].arraySetSize(1);
    
    // prepare fastmemoryaccess
    {
      TStringArray sensors;
      sensors.reserve(lbhNumOfSensors);
      for(int i = 0; i < lbhNumOfSensors; ++i)
        sensors.push_back(lbhSensorNames[i]);
      memory = new ALMemoryFastAccess;
      memory->ConnectToVariables(pBroker, sensors);
    }
    sensorValues.reserve(lbhNumOfSensors);
    sensorValues.resize(lbhNumOfSensors);
    
    // determine time offset between libbhuman and naoqi time
    {
      ALValue delta = 0;
      timeOffset = (int)proxy->genericCall("getTime", delta) - getSystemTime();
    }
    
    // register "onPreProcess" and "onPostProcess" callbacks
    proxy->getModule()->atPreProcess(&onPreProcess);
    proxy->getModule()->atPostProcess(&onPostProcess);
  }
  catch(AL::ALError& e)
  {
    fprintf(stderr, "libbhuman: %s\n", e.toString().c_str());
    close();
    return 0;
  }

  return 0;
}


class BHuman : public AL::ALModule
{
public:

  BHuman(AL::ALPtr<AL::ALBroker> pBroker, const std::string& pName): ALModule(pBroker, pName)
  {
    setModuleDescription( "A module that provides basic ipc NaoQi DCM access using shared memory." );

    if(create(pBroker) != 0)
      throw ALERROR("BHuman", "constructor", "");
  }

  virtual ~BHuman()
  {
    close();
  }

  void dataChanged(const std::string& pDataName, const ALValue& pValue, const std::string& pMessage) {}
  bool innerTest() { return true; }
};


extern "C" int _createModule(AL::ALPtr<AL::ALBroker> pBroker)
{
  AL::ALModule::createModule<BHuman>(pBroker, "BHuman");
  return 0;
}

extern "C" int _closeModule() // NaoQi doesn't invoke this function
                              // thats why we register the "BHuman" module for using the constructor and destructor
{
  return 0;
}
