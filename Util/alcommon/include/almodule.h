/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2007 All Rights Reserved - This file is confidential.\n
*
* Version : $Id$
*/


#ifndef AL_MODULE_H
#define AL_MODULE_H

#include <iostream>
#include <map>
#include "alsignal.hpp"
#include "altypes.h"
#include "alptr.h"
#include "alerror.h" // for ALERROR
#include "alvalue.h" // must be defined to instantiate functor with params of ALValue type
#include "alfunctor.h" // for ALFunctorBase
#include "albrokertools.h" // for ALModuleInfo
#include "alcriticalsection.h"
#include "almutex.h"



#define BIND_METHOD( x )  do { completeAndCheck(&x, fDesc); bindMethod(createFunctor(this, &x)); } while (0);


/*
* ALModule is the superclass of user modules.
* It implements basic functions that enables communication with naoqi brokers, so
* that the distributed approach is painless for users who just want to come up with
* a simple module quickly, compile it and have it work, without having to deal with
* interprocess communication.
*/

namespace AL
{
  // synchronizer typedef definition
  typedef std::map<std::string , std::pair< ALPtr<ALCondition> , ALPtr<ALMutex> >  >  TSynchronizerMap;
  typedef TSynchronizerMap::iterator             ITSynchronizerMap;

  class ALBroker;
  class ALProxy;

  class ALModule: public AnyPtr<ALModule>
  {
  friend class baseModule;
  public :

    /**
    * functionName, use to define the bound method name
    * @param pName, method's name
    * @param pClass, class's name
    * @param pFunctionDescription method's Description
    */
    void functionName ( const std::string& pName, const std::string& pClass, const std::string& pFunctionDescription );

    /**
    * help to describe a method's parameter.
    * @param pName parameter's name
    * @param pDesc Parameter's description
    */
    void addParam ( const std::string& pName, const std::string& pDesc);


    /**
    * true if someone asked application or module exit
    */
    bool isClosing;

    /**
    * module type
    */
    enum moduleType
    {
      CPP = 0,
      PYTHON = 1,
      RUBY = 2,
      LUA = 3,
      MATLAB = 4,
      URBI,
    };

    /**
    * initModule register a module to a broker. Automatically called when loading a module from autoload.ini
    */
    void initModule(void);


    /**
    * getProxy friendly method to get access to functionality
    * ex: getProxy("ALMotion")->callVoid("walk",5)
    */
    ALPtr<AL::ALProxy> getProxy(const std::string& pModuleName);

    /**
    * stop another module's method ID. Works if pModuleName has implemented and bound a stop method
    *
    * @param pModuleName module with stop method bound
    * @param pID ID of the method to stop
    */
    void stop(const std::string& pModuleName, const int &pID);

    /**
    * wait end of ID bound method execution
    *
    * @param pID method ID
    * @param pTimeout wait timeout
    * @return true if timeouted
    */
    bool wait(const int &pID, const int &pTimeout);

   /**
    * wait end of ID bound method execution
    *
    * @param pMethodName (get the first pMethodName running in the module)
    * @param pTimeout wait timeout
    * @return true if timeouted
    */
    int waitMethod(const std::string &pMethodName, const int &pTimeout);


    /**
    * true if method ID is running
    *
    * @param pID method ID
    */
    bool isRunning(const int &pID);

    /**
    * return the module broker name
    *
    */
    std::string getBrokerName();

    /**
    * method that will be called at every module creation
    * user can overload it
    *
    */

    virtual void init(void){};

    /*
    * getMethodID
    * return unique ID if method call.
    * @return method ID
    *
    * Useful to check for instance if user asked your method to stop.
    *   First get ID with getMethodID, then call isRequireStop 
    */
    int getMethodID(void);

    /*
    * isPCalled
    *
    * @return true if method is pcalled
    */
    bool isPCalled();

    /*
    * isPCalled
    *
    * @return true if method is pcalled
    */
    inline moduleType getModuleType(void)
    {
      return fModuleType;
    }

    /**
    * Creates an ALModule. An ALModule has a name,  and is registered in a broker,
    * so that its methods can be called by other modules, via a proxy constructed with 
    * module's name
    */
    ALModule( ALPtr<ALBroker> pBroker, const std::string& pName, moduleType type = CPP  );

    /**
    * Destructor
    */
    virtual ~ALModule ();

    /*
    * execute takes a method name, a set of arguments to this method, and an empty set of
    * results. It looks for the method that is bound to this name in its base, executes it
    * with the set of arguments, fills pResult with the results, and returns. pResult is
    * then fully usable.
    */
    virtual void execute(const std::string& pMethod, const AL::ALValue& pParams, AL::ALValue& pResult); // Alma 08-05-13: Adding a virtual for specific implementation in urbi UALModule

    /**
    * getMethodList, return the module method list
    * @return method list
    */
    virtual std::vector<std::string> getMethodList( );

    /**
    * getMethodHelp, return a method's description string
    * @param pMethodName, method's name.
    * @return pMethodHelp, a structured method's description
    */
    virtual AL::ALValue getMethodHelp ( const std::string& pMethodName );

    /**
    * moduleHelp retrieve the module's description.
    * @return a string describing the module
    */
    AL::ALValue moduleHelp();

    /**
    * dataChanged. Called by stm when subcription
    * has been modified.
    * @param pDataName Name of the modified data
    * @param pValue Name of the modified data
    * @param pMessage Name of the modified data
    */
    virtual void dataChanged (const std::string& /*pDataName*/, const AL::ALValue& /*pValue*/, const std::string& /*pMessage*/)
    {
      throw ALERROR("ALModule", "dataChanged", "virtual ALModule::dataChanged was called. Function should be redefined in child");
    }

    /**
    * @return a string containing version
    */
    virtual std::string version (  )
    {
      return "";
    }

    /**
     * Just a ping. Used to test connectivity to a module.
     * Always returns true
     */
    bool ping( void );

    /**
    * notify. called by server when a task is finished ( for pCallN ). In development
    * @param pId Id of this task
    */
    /// virtual void notify( ALValue pId ){};

    /**
    * exit. Exit
    * @param pParams unused
    * @param pResult unused
    */
    virtual void exit( );


    /**
    * innerTest. Run internal test to this module.
    * @param pParams unused
    * @param pResult return true if internal tests are ok
    */
    virtual bool innerTest( void )
    {return false;};

    /**
    * fName getter
    */
    const std::string&  getName () const { return fModuleInfo->name; };

    /**
    * fParentBroker setter
    */
    void setParentBroker (ALPtr<ALBroker> pParentBroker);

    /**
    * ModuleInfo getter
    */
    inline ALPtr<ALModuleInfo> getModuleInfo (){ return fModuleInfo;};

    /**
    * Call by the broker webpage to detail the module
    */
    virtual std::string httpGet () const{ return "";};

    /**
    * Called by a proxy to check function's parameter
    */
    const AL::ALValue * getFunctionDesc(const std::string& pName)
    {
      AL::ALCriticalSection section(fMethodDescMapMutex);
      ALMethodDescMap::iterator i = fMethodDescMap.find(pName);
      if (i == fMethodDescMap.end())
      {
        throw ALERROR("ALModule", "getFunctionDesc", std::string("Function ") + pName + " does not exists");
      }
      else
        return &(i->second);
    };

    /**
    * Called by a proxy to get a functor
    */
    const ALFunctorBase * getFunction(std::string pName)
    {
      AL::ALCriticalSection section(fFunctorMapMutex);
      ALFunctorMap::iterator i = fFunctorMap.find(pName);
      if (i == fFunctorMap.end())
      {
        throw ALERROR("ALModule", "getFunction", "Function does not exists");
      }
      else
      {
        return i->second.get();
      }
    };

    /**
    * Broker Accessor
    */
    inline ALPtr<ALBroker> getParentBroker () const
    {
      ALPtr<ALBroker> returnPtr = fParentBroker.lock();
      if(returnPtr == NULL)
        throw ALERROR("ALModule", "getParentBroker", "Module does not have a Broker anymore.");
      return returnPtr;
    };

    /**
    * local stop. Use ProxyModule.stop(...) for remote/local compatibility
    * @stop a module function pcalled
    */
    void functionStop(int pIDTask) ;

    /**
    * evaluate python string. In developement.
    *
    */
    //std::string eval(const std::string& pToEval);

    /**
    * bind a method on ALModule::methodMissing. Used by python interpreter
    * @param pModuleName python module name
    * @param pMethod method
    *
    */
    void autoBind(const std::string &pModule, const std::string &pMethod, int paramNumber);

    /**
    * methodMissing Should always raise an exception
    *
    * To bind Python methods,  we bind empty C++ functions,
    * so they can appear on the web page.
    *
    * But if we call them directly, something went wrong. (There should be a field
    * is the method description telling us it's a Python method)
    */
    std::string methodMissing0(){return "";};
    std::string methodMissing1(const int &param1){return "";};
    std::string methodMissing2(const int &param1, const int &param2){return "";};
    std::string methodMissing3(const int &param1, const int &param2,const int &param3){return "";};
    std::string methodMissing4(const int &param1, const int &param2,const int &param3,const int &param4){return "";};
    std::string methodMissing5(const int &param1, const int &param2,const int &param3,const int &param4, const int &param5){return "";};
        std::string methodMissing6(const int &param1, const int &param2,const int &param3,const int &param4, const int &param5, const int &param6){return "";};

    

    /**
    * define the module description
    * @param pVal string conatining module's description
    */
    inline void setModuleDescription (const std::string& pVal){ fModuleDescription[0] = pVal; };


  protected :

    /**
    * use to define a bound method.
    * @param pFunctor, pointer to a generic functor
    * @param method's name
    */
    void bindMethod (ALPtr<ALFunctorBase> pFunctor, char * pName);

    /**
    * addModuleExample, add a module example ( written in c++, urbi, ... )
    * @param pLanguage string containing example's language
    * @param pExample string containing example ( use \n for line return )
    */
    void addModuleExample (const std::string& pLanguage , const std::string& pExample);

    /**
    * addMethodExample, add a module example ( written in c++, urbi, ... )
    * @param pLanguage string containing example's language
    * @param pMethod string containing the methode name
    * @param pExample string containing example to add to pMethod
    */
    void addMethodExample (const std::string& pLanguage , const std::string& pExample);


    /**
    * help to describe a method's return.
    * @param pName return's name
    * @param pDesc return's description
    */
    void setReturn ( const std::string& pName, const std::string& pDesc);


    //FIXME: one of the two bindMethod should be not used
    void bindMethod (ALPtr<ALFunctorBase> pFunctor);

    /**
    * Buffer to store description
    */
    AL::ALValue fDesc; // temporary function description. cf ALMethodDescMap for details

    /**
    * name of this module
    */
    std::string fName;


  public:
    template <class T>
    static ALPtr<T> createModule(ALPtr<ALBroker> pBroker)
    {
      ALPtr<T> module = ALPtr<T>(new T(pBroker));
      module->initModule(); // register module in broker
      module->init();     // init if you redefined it
      return module;
    }

    template <class T>
    static ALPtr<T> createModule(ALPtr<ALBroker> pBroker, const std::string& name)
    {
      ALPtr<T> module = ALPtr<T>(new T(pBroker, name));
      module->initModule();
      module->init();     // init if you redefined it
      return module;
    }

    template <class T>
    static ALPtr<T> createPythonModule(ALPtr<ALBroker> pBroker, const std::string& name)
    {
      ALPtr<T> module = createModule<T>(pBroker, name);
      module->fModuleType = AL::ALModule::PYTHON;
      return module;
    }


    template <class T>
    static ALPtr<T> createUrbiModule(ALPtr<ALBroker> pBroker, const std::string& name)
    {
      ALPtr<T> module = createModule<T>(pBroker, name);
      module->fModuleType = AL::ALModule::URBI;
      return module;
    }


    /**
    * isModuleStopped
    * know if program or module termination is asked
    */
    inline bool isModuleStopped(){ return fStopModule; }

    inline void setModuleID(int ID)
    {
      fModuleID = ID;
    }

    inline int getModuleID()
    {
      return fModuleID;
    }
  public:
    typedef AL::functional::signal<void ()>                     ProcessSignal;
    typedef ProcessSignal::slot_function_type                   ProcessSignalSlot;
    typedef AL::functional::signals::connect_position           ProcessSignalPosition;
    typedef AL::functional::signals::connection                 ProcessSignalConnection;

    inline ProcessSignalConnection atPreProcess(ProcessSignalSlot subscriber, ProcessSignalPosition pos = AL::functional::signals::at_back)
    {
      return fPreProcess.connect(subscriber, pos);
    }

    inline ProcessSignalConnection atPostProcess(ProcessSignalSlot subscriber, ProcessSignalPosition pos = AL::functional::signals::at_back)
    {
      return fPostProcess.connect(subscriber, pos);
    }

    inline void removeAllPreProcess(void) { fPreProcess.disconnect_all_slots(); }
    inline void removeAllPostProcess(void) { fPostProcess.disconnect_all_slots(); }

    inline void preProcess(void) { fPreProcess(); }
    inline void postProcess(void) { fPostProcess(); }

    /**
    * addSynchronizer
    * Create pthread condition for module synchronization. Only add condition in constructor
    * @param condition name
    */
    inline void addSynchronizer(const std::string &pSynchronizerName)
    {
      fSynchonizer[pSynchronizerName].first = ALCondition::createALCondition();
      fSynchonizer[pSynchronizerName].second = ALMutex::createALMutex();
    }

    /**
    * getSynchronizer
    * get smart pointer on synchronizer
    * @param condition name
    */
    ALPtr<ALCondition> getSynchronizer(const std::string &pSynchronizerName)
    {
#ifdef DEBUG
      ITSynchronizerMap found =  fSynchonizer.find( "pSynchronizerName" );
      if ( found == fSynchonizer.end() )
      {
        return ALPtr<ALCondition>();
      }
#endif
      return fSynchonizer[pSynchronizerName].first;
    }

    /**
    * waitSynchronizer
    * wait on condition
    * @param condition name
    */
    void waitSynchronizer(const std::string &pSynchronizerName)
    {
      #ifdef DEBUG
      ITSynchronizerMap found =  fSynchonizer.find( pSynchronizerName );
      if ( found == fSynchonizer.end() )
      {
        return;
      }
      #endif
      ALCriticalSection section(fSynchonizer[pSynchronizerName].second);
      fSynchonizer[pSynchronizerName].first->wait(fSynchonizer[pSynchronizerName].second);
    }

    /**
    * signalSynchronizer
    * signal condition
    * @param condition name
    */
    void signalSynchronizer(const std::string &pSynchronizerName)
    {
            #ifdef DEBUG
      ITSynchronizerMap found =  fSynchonizer.find( pSynchronizerName );
      if ( found == fSynchonizer.end() )
      {
        return;
      }
      #endif
      fSynchonizer[pSynchronizerName].first->broadcast();
    }


  protected:
    ProcessSignal fPreProcess;
    ProcessSignal fPostProcess;


  private:

    // synchronizer storage
    std::map<std::string , std::pair< ALPtr<ALCondition> , ALPtr<ALMutex> >  > fSynchonizer;

    // module type
    moduleType fModuleType;

    // true if module should stop
    bool fStopModule;

    int fModuleID;

    ALPtr<ALMutex> fCloseModuleMutex;

    /**
    * This is used in order to be sure we don't call any method of the module during its 
    * initialization
    */
    ALPtr<ALMutexRW> fInitMutex;

    /**
    * type of the functor map
    */
    typedef std::map<std::string, ALPtr<ALFunctorBase> > ALFunctorMap;

    /**
    * Map containing all bound method's functors
    */
    ALFunctorMap fFunctorMap;
    ALPtr<ALMutex> fFunctorMapMutex;

    /**
    * fModuleInfo, structure containing Module information, such as Name, IP, Port...
    */
    ALPtr<ALModuleInfo> fModuleInfo;

    /**
    * fParentBroker, all module are linked with one broker in the same process.
    * The broker is in charge of interprocess communication.
    */
    ALWeakPtr<ALBroker> fParentBroker;

    /**
    * fModuleDescription, description of this module.
    */
    AL::ALValue fModuleDescription;

    /**
    * type of the description map
    */
    typedef std::map<std::string, AL::ALValue> ALMethodDescMap;

    /**
    * Array of method names ( needed to keep declaration's sorting )
    */
    AL::ALValue ALMethodsName;

    /**
    * map of function descriptions
    * Each method is described by an AL::ALValue:
    *		- [0]: name and description of the method
    *		- [1]: array of param descriptions
    *		- [2]: return value description
    *		- [3]: array of code examples
    *      -[3][i][0]: language name
    *      -[3][i][1]: source code example
    *		- [4]: bool is C++
    */
    ALMethodDescMap fMethodDescMap;
    
    ALPtr<ALMutex> fMethodDescMapMutex;



  };

  /**
  * Allowable bound method parameter types
  */
  enum ParamType
  {
    ParamError = 0,
    ParamVoid = 1,

    ParamBool = 2,
    ParamInt = 3,
    ParamFloat = 4,
    ParamString = 5,
    ParamALValue = 6,

    ParamConstBool = 7,
    ParamConstInt = 8,
    ParamConstFloat = 9,
    ParamConstString = 10,
    ParamConstALValue = 11,

    ParamBoolRef = 12,
    ParamIntRef = 13,
    ParamFloatRef = 14,
    ParamStringRef = 15,
    ParamALValueRef = 16,

    ParamConstBoolRef = 17,
    ParamConstIntRef = 18,
    ParamConstFloatRef = 19,
    ParamConstStringRef = 20,
    ParamConstALValueRef = 21,

    // Alma 08-02-19: Adding some new type param
    ParamVectorBool = 22,
    ParamVectorInt = 23,
    ParamVectorFloat = 24,
    ParamVectorString = 25,
    ParamVectorALValue = 26,

  };

  // used by proxy generator, could be move to proxygenerator module ?
  const char * typeToStr (const enum ParamType pType);
  const char * typeToStr (const int pType);

  template <typename T>
  struct _get_type { enum {value = ParamError}; };

  template <>
  struct _get_type<bool> { enum {value = ParamBool}; };

  template <>
  struct _get_type<int> { enum {value = ParamInt}; };

  template <>
  struct _get_type<float> { enum {value = ParamFloat}; };

  template <>
  struct _get_type<std::string> { enum {value = ParamString}; };

  template <>
  struct _get_type<AL::ALValue> { enum {value = ParamALValue}; };

  template <>
  struct _get_type<void> { enum {value = ParamVoid}; };

  // Alma 08-02-19: Adding some new type param
  template <>
  struct _get_type<std::vector<int> > { enum {value = ParamVectorInt}; };

  template <>
  struct _get_type<std::vector<float> > { enum {value = ParamVectorFloat}; };

  template <>
  struct _get_type<std::vector<std::string> > { enum {value = ParamVectorString}; };

  template <typename T>
  //    struct _get_type <const T> { enum {value =  5 +_get_type<T>::value}; };
  struct _get_type <const T> { enum {value = _get_type<T>::value}; };

  template <typename T>
  //    struct _get_type <T&> { enum {value = 10 + _get_type<T>::value}; };
  struct _get_type <T&> { enum {value = _get_type<T>::value}; };

  template <typename C, typename R>
  void completeAndCheck(R (C::* /*pFunction*/) (), AL::ALValue & pDesc)
  {
    pDesc[1][0] = _get_type<R>::value;
    pDesc[2].arraySetSize(0);
  }

  template <typename C, typename P1, typename R>
  void completeAndCheck(R (C::* /*pFunction*/) (P1), AL::ALValue & pDesc)
  {
    pDesc[1][0] = _get_type<R>::value;

    // Filling by default
    AL::ALValue t;
    t.arraySetSize(3);
    std::string name = "arg0";
    t[2] = "arg";
    for (int i = pDesc[2].getSize(); i < 1; ++i)
    {
      name[3] = (char) (i + '1');
      t[1] = name;
      pDesc[2].arrayPush(t);
    }

    pDesc[2].arraySetSize(1);

    pDesc[2][0][0] = _get_type<P1>::value;
  }

  template <typename C, typename P1, typename P2, typename R>
  void completeAndCheck(R (C::* /*pFunction*/) (P1, P2), AL::ALValue & pDesc)
  {
    pDesc[1][0] = _get_type<R>::value;

    // Filling by default
    AL::ALValue t;
    t.arraySetSize(3);
    std::string name = "arg0";
    t[2] = "arg";
    for (int i = pDesc[2].getSize(); i < 2; ++i)
    {
      name[3] = (char) (i + '1');
      t[1] = name;
      pDesc[2].arrayPush(t);
    }

    pDesc[2].arraySetSize(2);

    pDesc[2][0][0] = _get_type<P1>::value;
    pDesc[2][1][0] = _get_type<P2>::value;
  }

  template <typename C, typename P1, typename P2, typename P3, typename R>
  void completeAndCheck(R (C::* /*pFunction*/) (P1, P2, P3), AL::ALValue & pDesc)
  {
    pDesc[1][0] = _get_type<R>::value;

    // Filling by default
    AL::ALValue t;
    t.arraySetSize(3);
    std::string name = "arg0";
    t[2] = "arg";
    for (int i = pDesc[2].getSize(); i < 3; ++i)
    {
      name[3] = i + '1';
      t[1] = name;
      pDesc[2].arrayPush(t);
    }

    pDesc[2].arraySetSize(3);

    pDesc[2][0][0] = _get_type<P1>::value;
    pDesc[2][1][0] = _get_type<P2>::value;
    pDesc[2][2][0] = _get_type<P3>::value;

  }

  template <typename C, typename P1, typename P2, typename P3, typename P4, typename R>
  void completeAndCheck(R (C::* /*pFunction*/) (P1, P2, P3, P4), AL::ALValue & pDesc)
  {
    pDesc[1][0] = _get_type<R>::value;

    // Filling by default
    AL::ALValue t;
    t.arraySetSize(3);
    std::string name = "arg0";
    t[2] = "arg";
    for (int i = pDesc[2].getSize(); i < 4; ++i)
    {
      name[3] = i + '1';
      t[1] = name;
      pDesc[2].arrayPush(t);
    }

    pDesc[2].arraySetSize(4);

    pDesc[2][0][0] = _get_type<P1>::value;
    pDesc[2][1][0] = _get_type<P2>::value;
    pDesc[2][2][0] = _get_type<P3>::value;
    pDesc[2][3][0] = _get_type<P4>::value;
  }

  template <typename C, typename P1, typename P2, typename P3, typename P4, typename P5, typename R>
  void completeAndCheck(R (C::* /*pFunction*/) (P1, P2, P3, P4, P5), AL::ALValue & pDesc)
  {
    pDesc[1][0] = _get_type<R>::value;

    // Filling by default
    AL::ALValue t;
    t.arraySetSize(3);
    std::string name = "arg0";
    t[2] = "arg";
    for (int i = pDesc[2].getSize(); i < 5; ++i)
    {
      name[3] = i + '1';
      t[1] = name;
      pDesc[2].arrayPush(t);
    }

    pDesc[2].arraySetSize(5);

    pDesc[2][0][0] = _get_type<P1>::value;
    pDesc[2][1][0] = _get_type<P2>::value;
    pDesc[2][2][0] = _get_type<P3>::value;
    pDesc[2][3][0] = _get_type<P4>::value;
    pDesc[2][4][0] = _get_type<P5>::value;
  }

  template <typename C, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename R>
  void completeAndCheck(R (C::* /*pFunction*/) (P1, P2, P3, P4, P5, P6), AL::ALValue & pDesc)
  {
    pDesc[1][0] = _get_type<R>::value;

    // Filling by default
    AL::ALValue t;
    t.arraySetSize(3);
    std::string name = "arg0";
    t[2] = "arg";
    for (int i = pDesc[2].getSize(); i < 6; ++i)
    {
      name[3] = i + '1';
      t[1] = name;
      pDesc[2].arrayPush(t);
    }

    pDesc[2].arraySetSize(6);

    pDesc[2][0][0] = _get_type<P1>::value;
    pDesc[2][1][0] = _get_type<P2>::value;
    pDesc[2][2][0] = _get_type<P3>::value;
    pDesc[2][3][0] = _get_type<P4>::value;
    pDesc[2][4][0] = _get_type<P5>::value;
    pDesc[2][5][0] = _get_type<P6>::value;
  }



}
#endif
