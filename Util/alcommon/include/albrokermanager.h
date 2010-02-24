/**
* @author Aldebaran Robotics 
* Aldebaran Robotics (c) 2007 All Rights Reserved - This file is confidential.\n
*
* Version : $Id$
*/


#ifndef AL_BROKERMANAGER_H 
#define AL_BROKERMANAGER_H

#include "alsingleton.h"
#include "alptr.h"
#include "albroker.h"
#include "alerror.h"
#include <vector>
#include <string>
#include "almutex.h"
#include "alcriticalsectionread.h"
#include "alcriticalsectionwrite.h"

#define MAXBROKER 10



namespace AL
{

  class ALBroker;

  // should be only singleton with various main pointer
  class ALBrokerManager : public ALSingleton<ALBrokerManager>
  {
    friend class ALSingleton<ALBrokerManager>;
  protected:
    ALBrokerManager();
  public:
    virtual ~ALBrokerManager();

    /**
    * addBroker 
    * add a broker in the map of broker
    */
    void addBroker(ALPtr<ALBroker> pBroker);

    /**
    * remove a broker from the map of broker
    * carrefull also shutdown the broker
    */
    void removeBroker(ALPtr<ALBroker> pBroker);

    /**
    * remove a broker from the map of broker
    * no shutdown of the broker
    */
    void removeFromList(ALPtr<ALBroker> pBroker);


    inline ALPtr<ALBroker> getRandomBroker(void)
    {
      ALCriticalSectionRead section(mutex);
      if (fBrokerList.size()<=0)
        throw ALERROR("ALBrokerManager", "getAnyBroker", "There is no current broker defined in BrokerManager.");

      return (fBrokerList[0]);
    }


    /**
    * get a broker from index
    * no shutdown of the broker
    */
    inline ALPtr<ALBroker> getBroker(int i)
    {
      ALCriticalSectionRead section(mutex);
      AL_ASSERT(i<((int)(fBrokerList.size())));
      return (fBrokerList[i]);

    };

    /**
    * get a broker from endpoint
     @param pModuleName name of the module no longer existing
    * 
    */
    ALPtr<ALBroker> getBrokerByIPPort(const std::string &strEndPoint);


    /**
    * get a broker by ip and port
    * @param ip of the broker
    * @param port of the broker
    * 
    */
    ALPtr<ALBroker> getBrokerByIPPort(const std::string &strIP, int pPort);


    /**
    * remove and shutdown all brokers (program should not work any more after that)
    * 
    */
    void killAllBroker(void);

    ALPtr<ALBroker>   getReservedBroker(void);

  private:
    std::vector<ALPtr<ALBroker> >fBrokerList;
    ALPtr<ALBroker> fBrokerReserved; // unique broker use for method that don't use broker in parameter
    ALPtr<ALMutexRW> mutex;

  };

}
#endif
