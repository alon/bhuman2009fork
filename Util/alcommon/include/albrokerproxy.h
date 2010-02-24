/**
* @author Aldebaran Robotics 
* Aldebaran Robotics (c) 2007 All Rights Reserved - This file is confidential.\n
*
*
*
* Version : $Id$
*/


#ifndef AL_BROKERPROXY_H 
#define AL_BROKERPROXY_H

#include "altypes.h"
#include "alerror.h"
#include <stdlib.h>
#include "alptr.h"
#include "alvalue.h"
#include "almutex.h"
#include "albrokertools.h" // for ALModuleInfo


// deprecated define for debug
//#define AL_BROKER_PROXY_DEBUG(...) printf(__VA_ARGS__)
#define AL_BROKER_PROXY_DEBUG(...)


namespace AL
{
  /**
  * ALBrokerProxy allow to remotly call broker's method.
  */
  class ALBroker;

  class ALBrokerProxy
  {
  public :

    /*
    * Basic constructor
    */
    ALBrokerProxy(ALPtr<ALBroker> parentBroker, const std::string& pBrokerIP, int pBrokerPort);

    void setConnection(ALPtr<ALValue> pConnection)
    {
      fConnection = pConnection;
    }

    /*
    * Basic destructor
    */
    virtual ~ALBrokerProxy(void);

    /**
    * getBrokerName, return name of this broker
    * @param reference on broker name
    * @return 0 on success
    */
    //int getBrokerName( std::string& pBrokerName);


    /**
    * registerBroker, register a new broker to child to this broker
    * @param pBrokerToRegister structure containing broker information
    * @return 0 on success
    */
    int registerBroker( ALPtr<ALModuleInfo> pBrokerToRegister);

    /**
    * unregisterBroker, unregister Broker child
    * @param pBrokerToUnregister strcuture containing broker information
    * @return 0 on success
    */
    int unregisterBroker( ALPtr<ALModuleInfo> pBrokerToUnregister);

    /**
    * getBrokerInfo, return information about this broker
    * @param pModuleInfo, out parameter containing information about this broker
    * @return 0 on success
    */
    int getBrokerInfo( ALPtr<ALModuleInfo> pBrokerInfo );

    /**
    * getInfo, return structure containing module's information
    * @param pModuleName, module's name
    * @param pModuleInfo, output parameter containing module's information
    * @return 0 on success
    */
    int getInfo( const std::string& pModuleName, ALPtr<ALModuleInfo> pModuleInfo);

    /**
    * pPostMethodCall. generic call to method. usefull in c++ remote procedure call
    * @param pMethodeName name of the method to be called
    * @param pParams, [input] list of input parameters.
    * @return 0 on success
    */
    int postMethodCall( const std::string& pModuleName, const std::string& pMethodName, const AL::ALValue& pParams);

    /**
    * methodCall. generic call to method. usefull in c++ remote procedure call
    * @param pMethodeName name of the method to be called
    * @param pParams, [input] list of input parameters.
    * @param pResult, [output] list of output result.
    * @return 0 on success
    */
    int methodCall( const std::string& pModuleName, const std::string& pMethodName, const AL::ALValue& pParams, AL::ALValue &pResult);

    /**
    * exploreToGetModuleByName, used to search a module along the networks
    * @param pModuleName, name of the wanted module
    * @param pSearchUp, allow this broker to look for module in its parent brokers
    * @param pSearchDown, allow this broker to look for module in its childs brokers
    * @param pDontLookIntoBrokerName, specify a broker name for which the search must not be call. Usefull to avoid dead lock during search
    * @param pModInfo, out parameter containing module information if found.
    * @return 0 on success
    */
    int exploreToGetModuleByName(const std::string& pModuleName, bool pSearchUp, bool pSearchDown, const std::string& pDontLookIntoBrokerName, ALPtr<ALModuleInfo> pModInfo);

    /**
    * getModuleByName takes a module name as an input and returns the list of information
    * a program will need to contact the module.
    * getModuleByName should not be used directly : it should be used through helper functions
    * in order to hide the complexity of the returned information.
    * @param moduleName name of the module
    * @param modInfo returned information about the module\p
    * the struct al__moduleInfo is define with these member:\p
    * - [string] name\p
    * - [int]    architecture (see ALTypes.h)\p
    * - [string] IP address\p
    * - [int]    port\p
    * - [int]    Process ID\p
    * - [int]    module pointer\p
    * @return 0 on success
    */
    int getModuleByName(const std::string& pModuleName, ALPtr<ALModuleInfo> pModInfo);

    /**
    * unregisterModule is typically called when a module quits, as it unregisters the module
    * from naoqi.
    * @param moduleName name of the module
    * @return 0 on success
    */
    int unregisterModuleReference(const std::string& pModuleName);

    /**
    * getModuleList returns the registered module list.
    * @params pModuleList list of module information
    * @return 0 on success
    */
    int getModuleList( ALPtr< TALModuleInfoVector > pBrokerList );

    /**
    * getBrokerList returns the registered Broker list.
    * @params pBrokerList list of Broker information
    * @return 0 on success
    */
    int getBrokerList( ALPtr<TALModuleInfoVector> pBrokerList );

    /**
    * getGlobalModuleList returns the registered module list in every broker (global).
    * @params pModuleList list of module information
    * @return 0 on success
    */
    int getGlobalModuleList( ALPtr < TALModuleInfoVector > pModuleList );

    /**
    * dataChanged, allows to call the dataChanged method on a specified module
    * @param pModuleName, module concern by the dataChanged call
    * @param pDataName, Name of the ALMemory Data
    * @param pValue, value of the ALMemory Data
    * @param pMessage, message associate to this callback
    * @return 0 on success
    */
    int dataChanged(const std::string& pModuleName, const std::string& pDataName, const AL::ALValue& pValue, const std::string& pMessage);

    /**
    * getMethodList, returns a list of method name of a specified module
    * @param pModuleName, name of the module
    * @param pMethodListName, output parameter containing the returned method name list.
    * @return 0 on success
    */
    int getMethodList(const std::string& pModuleName, ALPtr< std::vector<std::string> >pMethodListName);

    /**
    * getMethodHelp, returns a method's description string
    * @param pModuleName, module's name
    * @param pMethodName, method's name. 
    * @param pMethodHelp, output parameter containing a structured method's description
    * @return 0 on success
    */
    int getMethodHelp( const std::string& pModuleName, const std::string& pMethodeName, AL::ALValue &pMethodHelp );

    /**
    * init, initialize broker with its own ip and port, and with a possible parent broker.\n
    * @param pBrokerName name of this new broker
    * @param pIP bind ip of the server
    * @param pPort listenning port of the server
    * @param pParentBrokerIP ip of the parent broker
    * @param pParentPort port of the parent broker
    * @return 0 on success
    */
    int init(const std::string& pBrokerName, const std::string& pIP, int pPort, const std::string& pParentBrokerIP, int pParentBrokerPort, bool pKeepAlive = false);

    /**
    * sendBackIP, send back client's ip.
    * @param pSoap, soap structure containing client'sip
    * @param pIP, out parameter containing IP as string
    * @return 0 on success
    */
    int sendBackIP( std::string& pIP );

    /**
    * version, return version of the module
    * @param pModuleName, module's name
    * @param pVersion, revision of the module as string
    * @return 0 on success
    */
    int version(std::string& pVersion);
    /**
    * stop task pId
    */
    int stop(int pID);
   /**
    * stop all tasks pMethodName
    */
    int stop(std::string pMethodName);
   /**
    * return the broker running task list ordered for telepathe
    */
    int getDebugTaskList(ALValue& taskList);

    /**
    * kill broker
    */
    int exit( );

    public: 
     inline ALPtr<ALBroker> getParentBroker(void)
     {
       ALPtr<ALBroker> returnBroker = fParentBroker.lock();
       if (returnBroker == NULL)
       {
          throw ALERROR("ThreadedRemoteExecution", "getParentBroker", "module does not exists");
       }
       return returnBroker;
     }

  protected :
      // connection structure to other broker
      ALPtr<ALValue> fConnection;


    /**
    * structure containing soap server information
    */
    ALPtr<ALMutex> fBrokerSoap_mutex;

       /**
    * broker pointer
    */ 
    ALWeakPtr<ALBroker> fParentBroker;

  };

}

#endif


