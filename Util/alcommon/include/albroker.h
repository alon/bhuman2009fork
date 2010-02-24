/**
* @author Aldebaran Robotics 
* Aldebaran Robotics (c) 2007 All Rights Reserved - This file is confidential.\n
*
* Version : $Id$
*/
 
#ifndef AL_BROKER_H
#define AL_BROKER_H

#define ENQUEUE getParentBroker()->getCallQueueManager()->enqueue

#include <list>
#include "altypes.h"
#include "altask.h"
#include "albrokertools.h" // for ALModuleInfo
#include <map>
#include "alptr.h"
#include "alvalue.h"
#include "altimeval.h"

// deprecated flag for debug information
//#define AL_BROKER_DEBUG(...) printf(__VA_ARGS__)
#define AL_BROKER_DEBUG(...)

// deprecated flag for debug information
//#define AL_BROKER_THREAD_DEBUG(...) printf(__VA_ARGS__)
#define AL_BROKER_THREAD_DEBUG(...)

#define BROKERMASK_KEEPALIVE 1  // no ::exit() if connection lost
#define BROKERMASK_LIGHT 2      // no thread pool
#define BROKERMASK_NOSERVER 4   // no server

namespace AL
{
  class ALProxy;
  class ALModule;
  class ALBroker;
  class ALBrokerProxy;
  class ALThreadPool;
  class ALTaskMonitor;
  class ALCallQueueManager;
  class ALSharedLibrary;
  class ALMemoryProxy;
  class ALLedsProxy;
  class ALLoggerProxy;
  class ALMotionProxy;
  class ALPreferencesProxy;
  class DCMProxy;
  class ALBrokerManager;
  class ALMutex;
  class ALMutexRW;

  /** 
  * Broker task is the thread server thread.
  */
  class BrokerTask : public AL::ALTask, public AL::AnyPtr<BrokerTask>
  {
  protected:
    BrokerTask(AL::ALPtr<ALBroker> pBroker): fBroker(pBroker){}

  public:


    static BrokerTask::Ptr createBrokerTask(AL::ALPtr<ALBroker> pBroker);

    virtual ~BrokerTask() {}
    void run ();

  private:
    AL::ALPtr<ALBroker> fBroker;
  };




  class ALConnectionInformation
  {
  public:
    ALConnectionInformation()
    {
      fTryNumber = 0;
      maxPing = 0;
      beatToggle = 0;
      lastTimeBetweenBeat  = 0;
      fPingStarted = false;

    }

    int getTryNumber(void)
    {
      return fTryNumber;
    }

    void resetTryNumber(void)
    {
      fTryNumber = 0;
    }

    void increaseTryNumber(void)
    {
      fTryNumber++;
    }


    void resetPing()
    {
      fPingStarted  = false;
    }

    void startPing(void)
    {
      fPingStarted = true;
    }

    bool getStarted(void)
    {
      return fPingStarted;
    }

    void start(void)
    {
        gettimeofday(&fLastPingTime, 0);
    }
    void stop(void)
    {
        gettimeofday(&fCurrentPingTime, 0);
    }

    int diff(void) // diff in s
    {
      return (fCurrentPingTime.tv_sec - fLastPingTime.tv_sec);
    }
    
    unsigned int lastTimeBetweenBeat;
    int beatToggle;
    int maxPing;
    
  private:
    struct timeval fLastPingTime;
    struct timeval fCurrentPingTime;
    int fTryNumber;
    bool fPingStarted;

  };


  typedef std::map< std::string, ALConnectionInformation  >  THeartBeatMap;

  class ALBrokerInfo: public ALModuleInfo
  {
  public:

    ALBrokerInfo(ALPtr <ALBroker> pBroker, ALPtr < ALModuleInfo> moduleInfo);

    inline ALPtr<ALBrokerProxy> getBrokerProxy(){return fBrokerProxy;};

    ALPtr< ALConnectionInformation > getConnectionInformation(void){return fConnectionInformation;};

    void setConnectionInformation(ALPtr< ALConnectionInformation > pConnectionInformation){fConnectionInformation =  pConnectionInformation;};

    ~ALBrokerInfo(void)
    {
    }
  protected:

      // ALBrokerProxy contain client functions
      ALPtr< ALBrokerProxy > fBrokerProxy;

      // ping time....
      ALPtr< ALConnectionInformation > fConnectionInformation;
    

  };



  /**
  * ALBroker is a class that implements basic ORB (Object Request Broker) functionnalities.
  * It allows modules located on different hosts to synchronise and communicate with each
  * others seamlessly, using proxies that encapsulate all the communication info.
  * ALBroker comes with smart function calls : functions are called directly when two modules
  * are located in the same process, and SOAP is used when they're remotly linked.
  */



  class ALBroker : public AnyPtr<ALBroker>
  {
  public :

    /**
    * Get library manager (manage dynamic library loading)
    * @return ALPtr<ALSharedLibrary>
    */
    inline ALPtr<ALSharedLibrary> getLibraryManager(void)
    {
      AL_ASSERT(fLibraryManager != NULL);
      return fLibraryManager;
    }

    /**
    * set broker IP
    * @param strToFill receive computer IP address
    */
    bool setCurrentIP( std::string & strToFill );

    /**
    * Basic constructor
    * @param pPath: AL_DIR (main naoqi folder)
    */
    ALBroker(std::string pPath = "");

    short getChildNumber();

    void checkHeartBeat(ALPtr<ALModule> pModuleThatCheck);

    ALPtr <ALConnectionInformation> getConnectionInformation(const std::string &pBrokerName);

    /**
    * Basic destructor
    */
    virtual ~ALBroker(void);

    /**
    * TaskID. set a unique id to each call method
    * @param pModuleName name of the module to be invoqued
    * @param pMethodeName name of the method to be called
    * @return an ALValue  [0]  -> (int)uID
    [1]  -> (string)methodName
    [2]  -> (string)moduleName
    */
    ALValue TaskID(const std::string& pModuleName, const std::string& pMethodName);

    /**
    * moduleID
    * return unique module ID
    */
    int moduleID();

    /**
    * set brokerManager pointer for multiple dll on windows
    */
    void setBrokerManagerInstance(ALPtr<ALBrokerManager> pBrokerManager);


    //////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////// List of Bind Method /////////////////////////////////
    // Bound methods are reachable by all the modules linked to the main broker.
    // each of these methods are declared in the ../interface/I_xxx.cpp file
    //

    /**
    * methodCall. Generic call to method. Useful in c++ remote procedure call
    * @param pMethodeName name of the method to be call
    * @param pParams, [input] list of input parameters.
    * @param pResult, [output] list of output result.
    * @return 0 on success
    */
    bool methodCall(const std::string& pModuleName, const std::string& pMethodeName, const AL::ALValue& pParams, AL::ALValue &pResult);

    /**
    * methodCall. generic call to method enqueued in a threadpool. soap return without waiting for the end of the procedure
    * @param pMethodeName name of the method to be call
    * @param pParams, [input] list of input parameters.
    * @param pResult, [output] list of output result.
    * @return 0 on success
    */
    int methodPCall(struct soap *soap, const std::string& pModuleName, const std::string& pMethodeName, const AL::ALValue& pParams, AL::ALValue &pResult);

    /**
    * methodPCallN. generic call to method enqueued in a threadpool. soap return without waiting for the end of the procedure
    * @param pNIP ip of the module which want to be notified
    * @param pNPort port of the module which want to be notified
    * @param pNModuleName name of the module which want to be notified
    * @param pMethodeName name of the method to be call
    * @param pParams, [input] list of input parameters.
    * @param pResult, [output] list of output result.
    * @return 0 on success
    */
    int methodPCallN(struct soap *soap,const std::string& pNIP, const int& pNPort,const std::string& pNModuleName, const std::string& pNMethodName, const std::string& pModuleName, const std::string& pMethodName, const ALValue& pParams, ALValue &pResult);

  
    /**
    * updateIPFromOS update broker ip address 
    */
    void updateIPFromOS(void);

    void loadAndLaunchNetwork(const std::string &pPath, bool pHeartbeat = true);
    

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
    int getModuleByName( const std::string& pModuleName, ALPtr<ALModuleInfo> pModInfo );

    /** getLocalModuleByName same as getModuleByName, but don't look on the network to find it
    */
    int getLocalModuleByName(const std::string& pModuleName, ALPtr<ALModuleInfo> pModInfo);

    /**
    * registerModule gives naoqi all the information a module will need to get contacted.
    * It should be called on module declaration.
    * @param modInfo information about the module\p
    * the struct al__moduleInfo is defined with these members:\p
    * - [string] name\p
    * - [int]    architecture (see ALTypes.h)\p
    * - [string] IP address\p
    * - [int]    port\p
    * - [int]    Process ID\p
    * - [int]    module pointer\p
    * @return 0 on success
    */
    int registerModule(ALPtr<ALModule> pModule);

    /**
    * getModuleByName other way to get module by name
    */
    ALPtr<ALModule> getModuleByName(const std::string& pModuleName);

    /**
    * unregisterModule is called when a module, belonging to this broker, quits.
    * References to module (proxies, subscriptions to ALMemory, ...) will be deleted
    * @param pModuleName name of the module exiting
    * @return 0 on success
    */
    int unregisterModule(const std::string& pModuleName);

    /**
    * unregisterModuleReference is called to inform that a module in the global system has quit.
    * Some brokers can then remove some "critic" or "dangerous" reference to this module
    * @param pModuleName name of the module no longer existing
    * @return 0 on success
    */
    int unregisterModuleReference(const std::string& pModuleName);

    /**
    * removeAllProxies 
    * remove all proxies
    */
    void removeAllProxies();

      /**
    * removeProxy is called to remove a proxy from the proxy list maintained by the broker
    * It is automatically called when a module is unregistered.
    * @param pModuleName name of the module no longer existing
    * @return 0 if failed, 1 otherwise
    */
    int removeProxy(const std::string& pModuleName);

   /**
    * removeProxy is called to remove a proxy from the proxy list maintained by the broker
    * It is automatically called when a module is unregistered.
    * @param pModuleName name of the module no longer existing
    * @return 0 if failed, 1 otherwise
    */
    int removeProxyFromBroker(const std::string& pBrokerName, bool removeProxyFromBroker = true);

    /**
    * deleteParentBrokerProxy
    * remove parent reference
    */
    void removeParentBrokerProxy(void);
    


    /**
    * init, initialize specialized proxies
    */
    void initProxy(void);


    /**
    * isModulePresent is called to know if a module is present.
    * @param pModuleName name of the module to search
    * @return true if the module is local or if the module is present in a broker connected.
    */
    bool isModulePresent( const std::string& pModuleName );

    /**
    * getModuleList return the registered module list.
    * @params pModuleList list of module information
    * @return 0 on success
    */
    int getModuleList( ALPtr <TALModuleInfoVector> pModuleList );

    /**
    * getBrokerList return the registered Broker list.
    * @params pBrokerList list of Broker information
    * @return 0 on success
    */
    int getBrokerList( TALModuleInfoVector &pBrokerList );

    /**
    * getModuleList return the registered module list in every broker (global).
    * @params pModuleList list of module information
    * @return 0 on success
    */
    int getGlobalModuleList( ALPtr< TALModuleInfoVector > pModuleList );


    /**
    * init, initialize broker with its own ip and port, and with a possible parent broker.\n
    * @param pBrokerName name of this new broker
    * @param pIP bind ip of the server ("" by default: find the local ip)
    * @param pPort listening port of the server (0 by default: find a free port)
    * @param pParentBrokerIP ip of the parent broker
    * @param pParentPort port of the parent broker
    * @param pKeepAlive if true, broker and child modules wouldn't be killed if naoqi is killed
    * @return 0 on success
    */
    int init(const std::string& pBrokerName, const std::string& pIP, int pPort, const std::string& pParentBrokerIP, int pParentBrokerPort, int brokerMask = 0);

    /**
    * init, initialize broker with port, and with a possible parent broker. To discover IP, the parent broker will send back IP.\n
    * @param pBrokerName name of this new broker
    * @param pPort listening port of the server (0 by default: find a free port)
    * @param pParentBrokerIP ip of the parent broker
    * @param pParentPort port of the parent broker
    * @return 0 on success
    */
    int init(const std::string& pBrokerName, int pPort, const std::string& pParentBrokerIP, int pParentBrokerPort, int pKeepAlive = 0);

    /**
    * sendBackIP, send back client's ip.
    * @param pSoap, soap structure containing client's ip
    * @param pIP, out parameter containing IP as string
    * @return 0 on success
    */
    int sendBackIP(/*struct soap* pSoap*/ unsigned long ip, std::string& pIP);

    /**
    * change module info with sendBackIP 
    */
    int translateModuleInfo(struct soap* pSoap , ALModuleInfo &moduleInfo);

    /**
    * registerBroker, register a new broker to child to this broker
    * @param pBrokerToRegister structure containing broker information
    * @return 0 on success
    */
    int registerBroker( ALPtr<ALModuleInfo> pBrokerToRegister);

    /**
    * unregisterBroker, unregister Broker child
    * @param pBrokerToUnregister structure containing broker information
    * @return 0 on success
    */
    int unregisterBroker( const ALModuleInfo &pBrokerToUnregister);

    /**
    * getBrokerInfo, return information about this broker
    * @param pModuleInfo, out parameter containing information about this broker
    * @return 0 on success
    */
    int getBrokerInfo( ALModuleInfo &pModuleInfo);

    /**
    * exploreToGetModuleByName, used to search a module along the networks
    * @param pModuleName, name of the wanted module
    * @param pSearchUp, allow this broker to look for module in its parent brokers
    * @param pSearchDown, allow this broker to look for module in its children brokers
    * @param pDontLookIntoBrokerName, specify a broker name for which the search must not be called. Usefull to avoid dead lock during search
    * @param pModInfo, out parameter containing module information if found.
    * @return 0 on success
    */
    int exploreToGetModuleByName(const std::string& pModuleName, bool pSearchUp, bool pSearchDown, const std::string& pDontLookIntoBrokerName, ALPtr<ALModuleInfo> pModInfo);


    //dmerej 2009-04-06 FIXME add comments!
    void runServer();

    void configureClient(const std::string &pIP, int pPort);

    void registerClient(const std::string &pIP, int pPort, bool pHeartBeat = true);

    ////////////////// ALModule bound Methods ////////////////////

    /**
    * dataChanged, allow to call the dataChanged method on a specified module
    * @param pModuleName, module concern by the dataChanged call
    * @param pDataName, Name of the ALMemory Data
    * @param pValue, value of the ALMemory Data
    * @param pMessage, message associate to this callback
    * @return 0 on success
    */
    int dataChanged(const std::string& pModuleName, const std::string& pDataName, const AL::ALValue& pValue, const std::string& pMessage);

    /**
    * getMethodList, return a list of method name of a specified module
    * @param pModuleName, name of the module
    * @param pMethodListName, output parameter containing the returned method name list.
    * @return 0 on success
    */
    int getMethodList(const std::string& pModuleName, std::vector<std::string> &pMethodListName) const;

    /**
    * getMethodHelp, return a method's description string
    * @param pModuleName, module's name
    * @param pMethodName, method's name.
    * @param pMethodHelp, output parameter containing a structured method's description
    * @return 0 on success
    */
    int getMethodHelp( const std::string& pModuleName, const std::string& pMethodName, AL::ALValue &pMethodHelp ) const;

    /**
    * version, return version of the module
    * @param pModuleName, module's name
    * @param pVersion, revision of the module as string
    * @return 0 on success
    */
    int version( std::string& pVersion ) const;

    /**
    * getInfo, return structure containing module's information
    * @param pModuleName, module's name
    * @param pModuleInfo, output parameter containing module's information
    * @return 0 on success
    */
    int getInfo( const std::string& pModuleName, ALModuleInfo &pModuleInfo) const;
    ////////////////// End of bound method List ////////////////////
    ///////////////////////////////////////////////////////////////


    /**
    * return a smart pointer on the module info structure
    */
    inline ALPtr<ALModuleInfo> getModuleInfo(void)
    {
      return fBrokerInfo;
    }

    /**
    * shutdown, close all modules and send exit command to child broker
    */
    int shutdown();

    /**
    * return return is this broker have a parent broker
    */
    bool HasParent() {return fHasParent;}

    /**
    * getBrokerSoap,
    * @return the soap structure associate to the parent broker
    */
    //struct soap* getBrokerSoap(){ return &fBrokerSoap;}; // tnetwork
    ALPtr<ALProxy> getBrokerSoap(){ return fBrokerSoapNetwork; };

    /**
    * fName getter
    */
    std::string getName() const {return fName;};

    /**
    * fBrokerIP getter
    */
    const std::string& getIP() const; 

    void setIP(const std::string &pIP);

    /**
    * fBorkerPort getter
    */
    int getPort() const {return fBrokerPort;};

    void setPort(int pPort)  {fBrokerPort = pPort;};

    /**
    * get a proxy
    * @param pProxyName proxy's name
    */
    ALPtr<ALProxy> getProxy(const std::string& pProxyName);

    /**
    * httpGet call for http get request on server
    * @param pPath The path requested by the sender
    */
    const std::string httpGet( const std::string & pPath );

    /**
    * getParentBrokerProxy return proxy to the parent broker, if any, 
    * return NULL otherwise
    */
    ALPtr<ALBrokerProxy> getParentBrokerProxy() const { return fParentBrokerProxy; };

    /**
    * return a smart pointer on the thread pool
    */
    ALPtr<ALThreadPool> getThreadPool();

    /**
    * return a smart pointer on the queue manager
    */
    ALPtr<ALCallQueueManager> getCallQueueManager();

    /**
    * return a smart pointer on the TaskMonitor
    */
    ALPtr <ALTaskMonitor> getTaskMonitor();

    /**
    * state of the server
    */
    /*static*/ bool fStopServer; // smart pointer

  private :
    /**
    * Pointer on library manager
    */
    ALPtr<ALSharedLibrary> fLibraryManager; // manager shared library for broker

    /**
    * Full httpDisplay or not (debug purpose)
    */

    bool fHttpFullDisplay;


    /**
    * AL_DIR path
    */
    std::string fStrALPath;

    /**
    * unique task ID (pcall management)
    */
    int uID;

    /**
    * unique module ID
    */
    int fmoduleID;


  //dmerej 2009-04-06 
  //TODO: use for loops, avoid using recursions...
    /**
    * xIterateChildBroker, used to iterate child broker (fBrokers) and create an ALBrokerProxy for each. 
    * If a ChildBroker has disappeared, it will be removed from the list.
    * @param : itCurrent: an iterator on the list, modified to point the next one. Initialised with fBrokers.begin.
    * @return : return an ALBrokerProxy created to discuss with the son. Callers must delete ALBrokerProxy* after having used it. return NULL when all child have been iterated.
    *
    */
    ALPtr<ALBrokerProxy> xIterateChildBroker( std::map<std::string, ALPtr <ALBrokerInfo> >::iterator & itCurrent );

    /**
    * xInitSocket will initialize a soap socket and eventually throw an exception if something is wrong. 
    */
    void xInitSocket();



  protected :

    /* taskID mutex */
    ALPtr<ALMutex> fIDMutex;

    /**
    * xGetModuleByName, return the module pointer if exist.
    */
    ALPtr<ALModule> xGetModuleByName(const std::string& pName) const;

    /**
    * Name of the broker
    */
    std::string fName;

    /**
    * IP of the broker
    */
    std::string fBrokerIP;

    /**
    * port of the broker
    */
    int fBrokerPort;

    /**
    * list of broker connected to this broker
    */
    std::map<std::string, ALPtr<ALBrokerInfo> > fBrokers;

    /**
    * broker management mutex
    */
    ALPtr <ALMutexRW> fLockBrokers;

    /**
    * soap structure associate to the parent broker
    */
    ALPtr<ALProxy> fBrokerSoapNetwork;

    /**
    * is this broker have a parent broker ?
    */
    bool fHasParent;

    /**
    * Broker Proxy to the parent broker
    */
    ALPtr<ALBrokerProxy> fParentBrokerProxy; 

    /**
    * structure containing parent broker information
    */
    ALPtr<ALModuleInfo> fParentBrokerInfo; 


    /**
    * structure containing broker information
    */
    ALPtr<ALModuleInfo> fBrokerInfo; 

    /**
    * map of module's structure information
    */
    std::map<std::string, ALPtr<ALModule> > fModules;
    
    /**
    * module management mutex
    */
    ALPtr<ALMutexRW> fLockModules;

    /**
    * map of proxy. Useful to share proxies along modules
    * Used by getProxy()
    */
    std::map<std::string, ALPtr<ALProxy> > fProxies;
    
    /**
    * Proxies management mutex
    */
    ALPtr<ALMutexRW> fLockProxies;

    /**
    * pool of thread to process execution requests
    */
    ALPtr<ALThreadPool> fThreadPool; 

    ALPtr<ALCallQueueManager> fTaskManager;
    ALPtr<ALTaskMonitor> fTaskMonitor;

    /**
    * Mutex which is unlocked once the runBroker thread is about to quit. 
    * This way the broker is not destroyed before the runBroker thread is finished.
    */
    pthread_mutex_t fMutexEndSig;



  private:

    //  TEST CODE FOR EVAL PARSING
    AL::ALValue xParseEvalQueryString(const std::string& pQuery) const;
    std::string xUrlDecode(const std::string& pString) const;

    /**
    * return the broker description as xml.
    */
    std::string inline xGetBrokerPageAsXml();

    /**
    * return the module description as xml
    */
    std::string inline xGetModuleAsXml(const std::string& pModuleName);

    /**
    * xGetMemoryContentAsXml, temporary function to output memory content in a webpage
    */
    std::string xGetMemoryContentAsXml();

    /**
    * return the module description as an full xml formatted string
    */
    std::string inline xGetModulePageAsXml(const std::string& pModuleName, const std::string& prevModuleName, const std::string& nextModuleName, const std::list<std::string> path);

    /**
    * return content of a file
    */
    std::string inline xGetFile( std::string pFileName);

    /**
    * Creates a WSDL description of the methods available
    */
    std::string xGenerateWSDL();

    /**
    * Evaluate a method from a given module via httpGet
    */
    std::string xEvaluateHttp( const std::string& pPath );

    /**
    * pointer to modules
    */
    ALPtr<ALMemoryProxy> fMemoryProxy;
    ALPtr<ALLedsProxy> fLedsProxy;
    ALPtr<ALLoggerProxy> fLoggerProxy;
    ALPtr<ALMotionProxy> fMotionProxy;
    ALPtr<DCMProxy> fDCMProxy;
    ALPtr<ALPreferencesProxy> fPreferencesProxy;


    /**
    * protect application when exiting
    */
    ALPtr<ALMutex> fShutdownMutex;
  public: 


    /**
    * true if debug application. Used only on windows to load library with _d post fix
    */
    bool isDebug(void);

    /**
    * true if path is al_dir
    */
    static bool isValidPath(const std::string &strPath);

    /**
    * get AL path (AL_DIR)
    */
    std::string getALPath(void);

    /**
    * get log path (AL_DIR/log)
    */
    std::string getLogPath(void);

    /**
    * getModuleBinPath(AL_DIR/modules/bin)
    */
    std::string getModuleBinPath(void);

    /**
    * getModuleLibPath (AL_DIR/modules/lib)
    */
    std::string getModuleLibPath(void);

    /**
    * getBinPath (AL_DIR/bin)
    */

    std::string getBinPath(void);

    /**
    * getPreferencesPath (AL_DIR/preferences)
    */
    std::string getPreferencesPath(void);

    /**
    *  isExiting
    * @return true if application is exiting
    */
    inline bool isExiting(void){ return fStopServer; }

    /**
    *  brokerMutex mutex that protect broker
    * 
    */
    pthread_mutex_t brokerMutex;


    /**
    *  template to get any module pointer
    * @param pSspecializedProxy: returned pointer
    * @param pStrName module name
    * @return pSspecializedProxy
    */
    template<class T> 
    ALPtr<T> getSpecialisedProxy(ALPtr<T> &pSspecializedProxy, std::string pStrName);

    /**
    * get pointer to memory. See specialized proxies in AL_DIR/modules/proxies
    * @return MemoryProxy
    */
    ALPtr<ALMemoryProxy> getMemoryProxy(void);

    /**
    * get pointer to leds. See specialized proxies in AL_DIR/modules/proxies
    * @return LedsProxy
    */
    ALPtr<ALLedsProxy> getLedsProxy(void);

    /**
    * get pointer to logger. See specialized proxies in AL_DIR/modules/proxies
    * @return LedsProxy
    */
    ALPtr<ALLoggerProxy> getLoggerProxy(void);

    /**
    * get pointer to motion. See specialized proxies in AL_DIR/modules/proxies
    * @return MotionProxy
    */
    ALPtr<ALMotionProxy> getMotionProxy(void);

    /**
    * get pointer to preferences. See specialized proxies in AL_DIR/modules/proxies
    * @return MotionProxy
    */
    ALPtr<ALPreferencesProxy> getPreferencesProxy(void);


    /**
    * get pointer to DCM. See specialized proxies in AL_DIR/modules/proxies
    * @return MotionProxy
    */
    ALPtr<DCMProxy> getDcmProxy(void);

    /**
    * createBroker helps to broker creation: 
    * @param pName broker name
    * @param pIP broker IP
    * @param pPort broker port
    * @param pParentIP parent broker IP
    * @param pParentPort parent broker port
    * @param pKeepAlive true => broker will not be destroyed if parent broker is destroyed
    * @param pPath AL_DIR path
    * @return pointer on broker
    */
    static ALPtr<ALBroker> createBroker(const std::string &pName, const std::string &pIP, int pPort, const std::string &pParentIP, int pParentPort, int pKeepAlive = 0, std::string pPath = "", bool pLoadLib = true);

    /**
    * access to broker manager
    */
    ALWeakPtr<ALBrokerManager> fBrokerManager;
 
    
  };

  // Class used to send tasks to the thread pool
  class process_request;



} // namespace AL

#endif
