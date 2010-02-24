/**
* @author Aldebaran Robotics 
* Aldebaran Robotics (c) 2007 All Rights Reserved - This file is confidential.\n
*
* Version : $Id$
*/


#ifndef AL_PROXY_H
#define AL_PROXY_H

#include <stdlib.h>

#include "albroker.h" // for inlined "pCall" using macro "PCALL"
#include "almodule.h" // for inlining "run" using method "execute"
#include "alvalue.h"
#include "altask.h"
#include "stdarg.h"
#include "sstream"
#include "althreadpool.h"
#ifdef WIN32
#include "process.h"
#else

#endif
#include "alptr.h"
#include "altaskmonitor.h"  // todo try to avoid it
#ifdef AL_PERF_CALCULATION_PROXY
#include "rttime.h"
#endif

// Deprecated macro
//#define AL_PROXY_DEBUG(...) printf(__VA_ARGS__)
#define AL_PROXY_DEBUG(...)

using namespace std;

namespace AL
{


#define CHECK_RESULT()                                                                                                \
  if (_get_type<R>::value != (int) (*lDesc)[1][0])                                                                    \
  {                                                                                                                   \
  throw ALERROR("ALProxy", "call", std::string( "wrong return type for function '" ) + pName + "' type is '" + typeToStr((int) _get_type<R>::value) + "' instead of '" + typeToStr( (int) (*lDesc)[1][0]) + "'" ); \
}

#define CHECK_PARAM(x)                                                                                                \
  if (_get_type< P ## x >::value != (int) (*lDesc)[2][x - 1][0])                                                      \
  {                                                                                                                   \
  throw ALERROR("ALProxy", "call/pCall", std::string("When trying to call module '") + getModule()->getName() + "' function '" + pName + "' parameter #" + (char) (x + '0') + " is of type " + typeToStr( _get_type< P ## x >::value ) + " instead of type " +  typeToStr((int) (*lDesc)[2][x - 1][0])); \
}

  // this pcall takes a list of parameters
  class _pcall : public ALTask
  {
  public:
    ALWeakPtr<ALBroker> parentBroker;

    _pcall(ALPtr<ALModule> pModule,std::string pMethodName, ALValue pParams, ALValue pID      )
      : fModule(pModule),
      fMethodName(pMethodName),
      fParams(pParams),
      notif(false),
      fID(pID)
    { 
      fAutoDelete = true;
      fBroker = pModule->getParentBroker();
      AL_ASSERT(pModule != NULL);
      parentBroker = pModule->getParentBroker();
      pModule->getParentBroker()->getTaskMonitor()->add(pID); 
      DEBUG_TASK_MSG(pModule->getParentBroker()->getTaskMonitor()->show())
    };
    _pcall(ALPtr<ALModule> pModule,std::string pMethodName, ALValue pParams, ALPtr<ALModule> pMod , ALValue pID)
      : fModule(pModule),
      fMethodName(pMethodName),
      fParams(pParams),
      notif(true),
      fMod(pMod),
      fID(pID)
    { 
      fAutoDelete = true; 
      AL_ASSERT(pModule != NULL);
      pModule->getParentBroker()->getTaskMonitor()->add(pID); // motion compatibility
      DEBUG_TASK_MSG(pModule->getParentBroker()->getTaskMonitor()->show())
    };
    ~_pcall() 
    { 
      ALPtr<ALBroker> parent = parentBroker.lock();
    };

#ifdef DEBUG
    virtual std::string getMethodName(){return fMethodName;}; 
#endif

    void run()
    {
      AL::ALMonitoredSection section(fModule->getParentBroker(), fID);
      try
      { 
        fModule->execute(fMethodName, fParams, fResult);
      }
      catch(...){}
      if (notif)
      {
        try 
        {
          AL::ALPtr<AL::ALProxy> prox = fBroker->getProxy((fMod->getModuleInfo())->name);
          const ALValue *lDesc = fMod->getFunctionDesc("notify");
          if (!lDesc)
            throw ALERROR("ALProxy", "call", "Function does not exist");
        }
        catch(...)
        {std::cout << "pcall_CATCH"<<  std::endl; } 
      }
    };

  private:
    ALPtr<ALBroker>		fBroker;  // will not kill broker if running task
    ALPtr<ALModule>		fModule;
    std::string		fMethodName;
    ALValue				fParams;
    ALValue				fResult;
    bool          notif;
    ALPtr<ALModule>      fMod; // one is caller other is called
    ALValue       fID;
  }; // class _pcall

  // altask for pcalln with std::string pModuleName, see EOF
  class _pcalln : public ALTask
  {
  public:

    _pcalln(ALPtr<ALModule> pModule,std::string pMethodName, ALValue pParams, std::string pNotifModName, ALValue pID);
    ~_pcalln();

#ifdef DEBUG
    virtual std::string getMethodName(){return fMethodName;}; 
#endif

    void run();

  private:

    ALPtr<ALModule>		fModule;
    std::string		fMethodName;
    ALValue				fParams;
    ALValue				fResult;
    std::string      fNotifModName;
    ALValue       fID;
  }; // class _pcalln


  // Tasks for the Thread pool
  // This pcall class will call the correct execute method
  class _pcallScript : public ALTask
  {
  public:
    _pcallScript( ALPtr<ALModule> pModule, const std::string& pMethod, const ALValue& pParams)
      : fModule(pModule),
        fMethod(pMethod),
        fParams(pParams)
    { 
      AL_ASSERT(fModule);
      fAutoDelete = true; 
    }
    virtual ~_pcallScript(){}

    void run ()
    { 
      try
      {
        ALValue result;
        fModule->execute(fMethod, fParams, result);
      }
      catch(ALError& e)
      {
        std::cout << "_pcallScript error: " << e.toString() << std::endl;
      }
    }
#ifdef DEBUG
    virtual std::string getMethodName(){return "local pCall in script (Python, Urbi...)";}; 
#endif

  private:
    const ALPtr<ALModule> fModule;
    const std::string     fMethod;
    const ALValue         fParams;
  };



  class _pcall0 : public ALTask
  {
  public:
    ALPtr<ALModule> parentModule;
    _pcall0( ALFunctor0<ALModule, ALVoid> * pFunctor ,ALValue& pID, ALPtr<ALModule> notifyModule )
      : fFunctor(pFunctor),notif(false),fID(pID)
    { 
      parentModule = notifyModule; // module can't be destroyed if pcall running on it
      fAutoDelete = true; 
      parentModule->getParentBroker()->getTaskMonitor()->add(fID);  // move add from pcall to here
    }
    _pcall0( ALFunctor0<ALModule, ALVoid> * pFunctor,ALPtr<ALModule> notifyModule,ALValue& pID )
      : fFunctor(pFunctor),notif(true),fMod(notifyModule),fID(pID)
    { 
      parentModule = notifyModule;
      fAutoDelete = true; 
      parentModule->getParentBroker()->getTaskMonitor()->add(fID);  // move add from pcall to here
    }
    ~_pcall0(){};

    void run ()
    { 
      // monitor function call
      AL::ALMonitoredSection section(parentModule->getParentBroker(), fID);

      (*fFunctor)(); 
      if (notif)
      {
        try 
        {
          std::cout << "thread notif" << std::endl;
          ALPtr<ALProxy> prox = parentModule->getParentBroker()->getProxy((fMod->getModuleInfo())->name);
          std::cout << "thread notif proxy" << std::endl;
          std::cout << fID.toString( VerbosityMini) <<  std::endl;
          const ALValue *lDesc = fMod->getFunctionDesc("notify");
          if (!lDesc)
            throw ALERROR("ALProxy", "call", "Function does not exist");
          //(*(ALFunctor_2<ALModule, AL::ALValue, AL::ALValue,ALVoid> *)(fMod->getFunction("notify")))(fID,ALValue());

        }
        catch(...)
        {std::cout << "pcall_CATCH"<<  std::endl; } 
      }
    }
#ifdef DEBUG
    virtual std::string getMethodName(){return "local pCall";}; 
#endif

  private:
    ALFunctor0<ALModule, ALVoid> * fFunctor;
    bool notif;
    ALPtr<ALModule> fMod;
    ALValue fID;
  };



  template <typename P1>
  class _pcall1 : public ALTask
  {
  public:
    _pcall1(ALFunctor_1<ALModule, P1, ALVoid> * pFunctor,
      P1 p1, ALPtr<ALModule> pMod, ALValue& pID )
      : fFunctor(pFunctor), f1(p1),notif(false),fMod(pMod), fID(pID)
    { 
      fAutoDelete = true; 
      pMod->getParentBroker()->getTaskMonitor()->add(fID);  // move add from pcall to here
    }
    _pcall1( ALFunctor_1<ALModule, P1, ALVoid> * pFunctor, P1 p1,ALPtr<ALModule> pMod,ALValue& pID, bool notify )
      : fFunctor(pFunctor), f1(p1),notif(true),fMod(pMod),fID(pID)
    { 
      fAutoDelete = true; 
      pMod->getParentBroker()->getTaskMonitor()->add(fID);  // move add from pcall to here
    }
    ~_pcall1() { }

    void run ()
    { 
      // monitor function
      AL::ALMonitoredSection section(fMod->getParentBroker(), fID);
      (*fFunctor)(f1); 
      if (notif)
      {
        try 
        {
          const ALValue *lDesc = fMod->getFunctionDesc("notify");
          if (!lDesc)
            throw ALERROR("ALProxy", "call", "Function does not exist");
          //(*(ALFunctor_2<ALModule, AL::ALValue, AL::ALValue,ALVoid> *)(fMod->getFunction("notify")))(fID,ALValue());
        }
        catch(...)
        {std::cout << "pcall_CATCH"<<  std::endl; } 
      }
      //fMod->getParentBroker()->getTaskMonitor()->remove((int)fID[0]); 
    }
#ifdef DEBUG
    virtual std::string getMethodName(){return "local pCall";}; 
#endif

  private:
    ALFunctor_1<ALModule, P1, ALVoid> * fFunctor;
    P1 f1;
    bool notif;
    ALPtr<ALModule> fMod;
    ALValue fID;
  };



  template <typename P1, typename P2>
  class _pcall2 : public ALTask
  {
  public:
    _pcall2( ALFunctor_2<ALModule, P1, P2, ALVoid> * pFunctor,
      P1 p1, P2 p2,ALPtr<ALModule> pMod, ALValue& pID  )
      : fFunctor(pFunctor), f1(p1), f2(p2),notif(false),fMod(pMod), fID(pID)
    { 
      fAutoDelete = true; 
      pMod->getParentBroker()->getTaskMonitor()->add(fID);  // move add from pcall to here
    }
    _pcall2( ALFunctor_1<ALModule, P1, ALVoid> * pFunctor, P1 p1,P2 p2,ALPtr<ALModule> pMod,ALValue& pID, bool notify )
      : fFunctor(pFunctor), f1(p1), f2(p2),notif(true),fMod(pMod),fID(pID)
    { 
      fAutoDelete = true; 
      pMod->getParentBroker()->getTaskMonitor()->add(fID);  // move add from pcall to here
    }
    ~_pcall2() { }

    void run () { 
      // monitor function
      AL::ALMonitoredSection section(fMod->getParentBroker(), fID);

      (*fFunctor)(f1, f2); 
      if (notif)
      {
        try 
        {
          const ALValue *lDesc = fMod->getFunctionDesc("notify");
          if (!lDesc)
            throw ALERROR("ALProxy", "call", "Function does not exist");
          //(*(ALFunctor_2<ALModule, AL::ALValue, AL::ALValue,ALVoid> *)(fMod->getFunction("notify")))(fID,ALValue());
        }
        catch(...)
        {std::cout << "pcall_CATCH"<<  std::endl; } 
      }
    }
#ifdef DEBUG
    virtual std::string getMethodName(){return "local pCall";}; 
#endif
  private:
    ALFunctor_2<ALModule, P1, P2, ALVoid> * fFunctor;
    P1 f1;
    P2 f2;
    bool notif;
    ALPtr<ALModule> fMod;
    ALValue fID;
  };



  template <typename P1, typename P2, typename P3>
  class _pcall3 : public ALTask
  {
  public:
    _pcall3( ALFunctor_3<ALModule, P1, P2, P3, ALVoid> * pFunctor,
      P1 p1, P2 p2, P3 p3,ALPtr<ALModule> pMod, ALValue& pID  )
      : fFunctor(pFunctor), f1(p1), f2(p2), f3(p3),notif(false),fMod(pMod), fID(pID)
    { 
      fAutoDelete = true; 
      pMod->getParentBroker()->getTaskMonitor()->add(fID);  // move add from pcall to here
    }
    _pcall3( ALFunctor_1<ALModule, P1, ALVoid> * pFunctor, P1 p1, P2 p2, P3 p3,ALModule *pMod,ALValue& pID, bool notify )
      : fFunctor(pFunctor), f1(p1), f2(p2), f3(p3),notif(true),fMod(pMod),fID(pID)
    { 
      fAutoDelete = true; 
      pMod->getParentBroker()->getTaskMonitor()->add(fID);  // move add from pcall to here
    }
    ~_pcall3() { }

    void run () { 

      // monitor function call
      AL::ALMonitoredSection section(fMod->getParentBroker(), fID);

      (*fFunctor)(f1, f2, f3);
      if (notif)
      {
        try 
        {
          const ALValue *lDesc = fMod->getFunctionDesc("notify");
          if (!lDesc)
            throw ALERROR("ALProxy", "call", "Function does not exist");
          // (*(ALFunctor_2<ALModule, AL::ALValue, AL::ALValue,ALVoid> *)(fMod->getFunction("notify")))(fID,ALValue());
        }
        catch(...)
        {std::cout << "pcall_CATCH"<<  std::endl; } 
      }
    }
#ifdef DEBUG
    virtual std::string getMethodName(){return "local pCall";}; 
#endif
  private:
    ALFunctor_3<ALModule, P1, P2, P3, ALVoid> * fFunctor;
    P1 f1;
    P2 f2;
    P3 f3;
    bool notif;
    ALPtr<ALModule> fMod;
    ALValue fID;
  };





  template <typename P1, typename P2, typename P3, typename P4>
  class _pcall4 : public ALTask
  {
  public:
    _pcall4( ALFunctor_4<ALModule, P1, P2, P3, P4, ALVoid> * pFunctor,
      P1 p1, P2 p2, P3 p3, P4 p4,ALPtr<ALModule> pMod, ALValue& pID   )
      : fFunctor(pFunctor), f1(p1), f2(p2), f3(p3), f4(p4),notif(false),fMod(pMod), fID(pID)
    { 
      fAutoDelete = true; 
      pMod->getParentBroker()->getTaskMonitor()->add(fID);  // move add from pcall to here
    }
    _pcall4( ALFunctor_1<ALModule, P1, ALVoid> * pFunctor, P1 p1, P2 p2, P3 p3,P4 p4,ALPtr<ALModule> pMod,ALValue& pID )
      : fFunctor(pFunctor), f1(p1), f2(p2), f3(p3), f4(p4),notif(true),fMod(pMod),fID(pID)
    { 
      fAutoDelete = true; 
      pMod->getParentBroker()->getTaskMonitor()->add(fID);  // move add from pcall to here
    }
    ~_pcall4() { }

    void run () { 
      // monitor function
      AL::ALMonitoredSection section(fMod->getParentBroker(), fID);

      (*fFunctor)(f1, f2, f3, f4); 
      if (notif)
      {
        try 
        {
          const ALValue *lDesc = fMod->getFunctionDesc("notify");
          if (!lDesc)
            throw ALERROR("ALProxy", "call", "Function does not exist");
          //(*(ALFunctor_2<ALModule, AL::ALValue, AL::ALValue,ALVoid> *)(fMod->getFunction("notify")))(fID,ALValue());
        }
        catch(...)
        {std::cout << "pcall_CATCH"<<  std::endl; } 
      }
    }
#ifdef DEBUG
    virtual std::string getMethodName(){return "local pCall";}; 
#endif
  private:
    ALFunctor_4<ALModule, P1, P2, P3, P4, ALVoid> * fFunctor;
    P1 f1;
    P2 f2;
    P3 f3;
    P4 f4;
    bool notif;
    ALPtr<ALModule> fMod;
    ALValue fID;
  };


  template <typename P1, typename P2, typename P3, typename P4, typename P5>
  class _pcall5 : public ALTask
  {
  public:
    _pcall5( ALFunctor_5<ALModule, P1, P2, P3, P4, P5, ALVoid> * pFunctor,
      P1 p1, P2 p2, P3 p3, P4 p4, P5 p5,ALPtr<ALModule> pMod, ALValue& pID    )
      : fFunctor(pFunctor), f1(p1), f2(p2), f3(p3), f4(p4), f5(p5),notif(false),fMod(pMod), fID(pID)
    { 
      fAutoDelete = true; 
      pMod->getParentBroker()->getTaskMonitor()->add(fID);  // move add from pcall to here
    }
    _pcall5( ALFunctor_1<ALModule, P1, ALVoid> * pFunctor, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5,ALPtr<ALModule> pMod,ALValue& pID )
      : fFunctor(pFunctor), f1(p1), f2(p2), f3(p3), f4(p4), f5(p5),notif(true),fMod(pMod),fID(pID)
    { 
      fAutoDelete = true; 
      pMod->getParentBroker()->getTaskMonitor()->add(fID);  // move add from pcall to here
    }
    ~_pcall5() { }

    void run () 
    { 
      // monitor function
      AL::ALMonitoredSection section(fMod->getParentBroker(), fID);

      (*fFunctor)(f1, f2, f3, f4, f5); 
      if (notif)
      {
        try 
        {
          const ALValue *lDesc = fMod->getFunctionDesc("notify");
          if (!lDesc)
            throw ALERROR("ALProxy", "call", "Function does not exist");
          //(*(ALFunctor_2<ALModule, AL::ALValue, AL::ALValue,ALVoid> *)(fMod->getFunction("notify")))(fID,ALValue());
        }
        catch(...)
        {std::cout << "pcall_CATCH"<<  std::endl; } 
      }
    }
#ifdef DEBUG
    virtual std::string getMethodName(){return "local pCall";}; 
#endif
  private:
    ALFunctor_5<ALModule, P1, P2, P3, P4, P5, ALVoid> * fFunctor;
    P1 f1;
    P2 f2;
    P3 f3;
    P4 f4;
    P5 f5;
    bool notif;
    ALPtr<ALModule> fMod;
    ALValue fID;
  };


  template <typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
  class _pcall6 : public ALTask
  {
  public:
    _pcall6( ALFunctor_6<ALModule, P1, P2, P3, P4, P5, P6, ALVoid> * pFunctor,
      P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6,ALPtr<ALModule> pMod, ALValue& pID  )
      : fFunctor(pFunctor), f1(p1), f2(p2), f3(p3), f4(p4), f5(p5), f6(p6),notif(false),fMod(pMod), fID(pID)
    { 
      fAutoDelete = true; 
      pMod->getParentBroker()->getTaskMonitor()->add(fID);  // move add from pcall to here
    }
    _pcall6( ALFunctor_1<ALModule, P1, ALVoid> * pFunctor, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5,P6 p6,ALPtr<ALModule> pMod,ALValue& pID )
      : fFunctor(pFunctor), f1(p1), f2(p2), f3(p3), f4(p4), f5(p5), f6(p6),notif(true),fMod(pMod),fID(pID)
    { 
      fAutoDelete = true; 
      pMod->getParentBroker()->getTaskMonitor()->add(fID);  // move add from pcall to here
    }
    ~_pcall6() { }

    void run () {
      (*fFunctor)(f1, f2, f3, f4, f5, f6); 
      if (notif)
      {
        // monitor function
        AL::ALMonitoredSection section(fMod->getParentBroker(), fID);

        try 
        {
          const ALValue *lDesc = fMod->getFunctionDesc("notify");
          if (!lDesc)
            throw ALERROR("ALProxy", "call", "Function does not exist");
          // (*(ALFunctor_2<ALModule, AL::ALValue, AL::ALValue,ALVoid> *)(fMod->getFunction("notify")))(fID,ALValue());
        }
        catch(...)
        {std::cout << "pcall_CATCH"<<  std::endl; } 
      }
    }
#ifdef DEBUG
    virtual std::string getMethodName(){return "local pCall";}; 
#endif
  private:
    ALFunctor_6<ALModule, P1, P2, P3, P4, P5, P6, ALVoid> * fFunctor;
    P1 f1;
    P2 f2;
    P3 f3;
    P4 f4;
    P5 f5;
    P6 f6;
    bool notif;
    ALPtr<ALModule> fMod;
    ALValue fID;
  };


  /**
  * ALProxy allow to remotly call module's method.
  */

  class ALProxy : public AL::AnyPtr<ALProxy>
  {
  public:

    /**
     * return true if current language is a script language.
     * In that case, we call the execute method, that has to be 
     * redefined in the module class defined in this language.
     * Look in script wrapper project for a sample in python.
     */
    bool isScript(void)
    {
      return (getModule()->getModuleType() != AL::ALModule::CPP);
    }


    template <typename R , typename P1, typename P2  >
    R callRef(const std::string& pName, const P1 &p1,  const P2 &p2)
    {
      //const ALValue *lDesc = getModule()->getFunctionDesc(pName);
      return (*(ALFunctor_2<ALModule, P1, P2, R> *)(getModule()->getFunction(pName)))(p1, p2);
    }

    void CALL_METHODS(const std::string &szMethodName,const ALValue &param, ALValue &result);
    void PCALL_METHODSNoID(const std::string &szMethodName, const ALValue &param, ALValue &result);
    void PCALL_METHODS(const std::string &szMethodName, const ALValue &param, ALValue &result);
    //void PCALLN_METHODS(ALPtr<ALModule> mod , const std::string &szMethodName, const ALValue &param, ALValue &result);
    //void ALProxy::GENERIC_PCALLN_METHODS(const std::string & pModuleName ,const std::string &  szMethodName,const ALValue & param,ALValue &result);


    /* ----------------------------------------------------------------------------------------------------------*/
    /* -----------------------    call template with return value        ----------------------------------------*/
    /* ----------------------------------------------------------------------------------------------------------*/

    template <typename R>
    R callLocal(const std::string& pName)
    {
      
      
        if (isScript())
        { 
          throw ALERROR("ALProxy", "call", "call<R> not yet implemented in script language. Use pCall or callVoid instead");
        }
#ifdef DEBUG
        const ALValue *lDesc = getModule()->getFunctionDesc(pName);
        if (!lDesc)
        {
          throw ALERROR("ALProxy", "call", "Function does not exist");
        }
        CHECK_RESULT()
#endif 
          return (*(ALFunctor0<ALModule, R> *)(getModule()->getFunction(pName)))(); 
    }


    template <typename R, typename P1>
    R callLocal(const std::string& pName,  const P1 &p1)
    {
      if(fIsLocal)
      {
        if (isScript())
        { 
          throw ALERROR("ALProxy", "call", "call<R> not yet implemented on script language. Use pCall or callVoid instead");
        }
#ifdef DEBUG
        const ALValue *lDesc = getModule()->getFunctionDesc(pName);
        if (!lDesc)
          throw ALERROR("ALProxy", "call", "Function does not exist");

        CHECK_RESULT()
        CHECK_PARAM(1)
#endif
          return (*(ALFunctor_1<ALModule, P1, R> *)(getModule()->getFunction(pName)))(p1);
      }
    }


    
    template <typename R, typename P1, typename P2>
    R callLocal(const std::string& pName, const P1 &p1, const P2 &p2)
    {
        if (isScript())
        { 
          throw ALERROR("ALProxy", "call", "call<R> not yet implemented on script language. Use pCall or callVoid instead");
        }

#ifdef DEBUG
        const ALValue *lDesc = getModule()->getFunctionDesc(pName);
        if (!lDesc)
          throw ALERROR("ALProxy", "call", "Function does not exist");

        CHECK_RESULT()
          CHECK_PARAM(1)
          CHECK_PARAM(2) 
#endif
          return (*(ALFunctor_2<ALModule, P1, P2, R> *)(getModule()->getFunction(pName)))(p1, p2);
    
    
    }

    


    template <typename R, typename P1, typename P2, typename P3>
    R callLocal(const std::string& pName, const P1 &p1,  const P2 &p2, const P3 &p3)
    {

      if (isScript())
      { 
        ALValue param;
        ALValue result;
        param.arraySetSize(3);
        param[0] = p1;
        param[1] = p2;
        param[2] = p3;
        getModule()->execute(pName, param, result);
        return result;
      }

#ifdef DEBUG
      const ALValue *lDesc = getModule()->getFunctionDesc(pName);
      if (!lDesc)
        throw ALERROR("ALProxy", "call", "Function does not exist");

      CHECK_RESULT()
        CHECK_PARAM(1)
        CHECK_PARAM(2)
        CHECK_PARAM(3)
#endif
        return (*(ALFunctor_3<ALModule, P1, P2, P3, R> *)(getModule()->getFunction(pName)))(p1, p2, p3);


    }



    // call with result, no parameters
    template <typename R>
    R call(const std::string& pName)
    {
      if(fIsLocal)
      {
        if (isScript())
        { 
          ALValue param;
          ALValue result;
          getModule()->execute(pName, param, result);
          return result;
        }
#ifdef DEBUG
        const ALValue *lDesc = getModule()->getFunctionDesc(pName);
        if (!lDesc)
        {
          throw ALERROR("ALProxy", "call", "Function does not exist");
        }
        CHECK_RESULT()
#endif 
          return (*(ALFunctor0<ALModule, R> *)(getModule()->getFunction(pName)))();
      }
      else
      {
        ALValue p, r;
        CALL_METHODS(pName, p, r);
        if( !r.isValid() )
        {
          throw ALERROR("ALProxy", "call", "Waiting for the result of a void function");
        }
        return r.Convert<R>();
      }
    }

    template <typename R, typename P1>
    R call(const std::string& pName,  const P1 &p1)
    {
      if(fIsLocal)
      {
        if (isScript())
        { 
          ALValue param;
          ALValue result;
          param.arraySetSize(1);
          param[0] = p1;
          getModule()->execute(pName, param, result);
          return result;
        }
#ifdef DEBUG
        const ALValue *lDesc = getModule()->getFunctionDesc(pName);
        if (!lDesc)
          throw ALERROR("ALProxy", "call", "Function does not exist");

        CHECK_RESULT()
          CHECK_PARAM(1)
#endif
          return (*(ALFunctor_1<ALModule, P1, R> *)(getModule()->getFunction(pName)))(p1);
      }
      else
      {
        ALValue p, r;
        p.arraySetSize(1);
        p[0] = p1;
        CALL_METHODS(pName, p, r);
        if( !r.isValid() )
          throw ALERROR("ALProxy", "call", "Waiting for the result of a void function");
        return r.Convert<R>();

      }
    }


    template <typename R, typename P1, typename P2>
    R call(const std::string& pName, const P1 &p1, const P2 &p2)
    {
      if(fIsLocal)
      {
        if (isScript())
        { 
          ALValue param;
          ALValue result;
          param.arraySetSize(2);
          param[0] = p1;
          param[1] = p2;
          getModule()->execute(pName, param, result);
          return result;
        }

#ifdef DEBUG
        const ALValue *lDesc = getModule()->getFunctionDesc(pName);
        if (!lDesc)
          throw ALERROR("ALProxy", "call", "Function does not exist");

        CHECK_RESULT()
          CHECK_PARAM(1)
          CHECK_PARAM(2)
#endif
          return (*(ALFunctor_2<ALModule, P1, P2, R> *)(getModule()->getFunction(pName)))(p1, p2);
      }
      else
      {
        ALValue p, r;
        p.arraySetSize(2);
        p[0] = p1;
        p[1] = p2;
        CALL_METHODS(pName, p, r);
        if( !r.isValid() )
          throw ALERROR("ALProxy", "call", "Waiting for the result of a void function");

        return r.Convert<R>();

      }
    }


    template <typename R, typename P1, typename P2, typename P3>
    R call(const std::string& pName, const P1 &p1,  const P2 &p2, const P3 &p3)
    {
      if(fIsLocal)
      {
        if (isScript())
        { 
          ALValue param;
          ALValue result;
          param.arraySetSize(3);
          param[0] = p1;
          param[1] = p2;
          param[2] = p3;
          getModule()->execute(pName, param, result);
          return result;
        }

#ifdef DEBUG
        const ALValue *lDesc = getModule()->getFunctionDesc(pName);
        if (!lDesc)
          throw ALERROR("ALProxy", "call", "Function does not exist");

        CHECK_RESULT()
          CHECK_PARAM(1)
          CHECK_PARAM(2)
          CHECK_PARAM(3)
#endif
          return (*(ALFunctor_3<ALModule, P1, P2, P3, R> *)(getModule()->getFunction(pName)))(p1, p2, p3);
      }
      else
      {
        ALValue p, r;
        p.arraySetSize(3);
        p[0] = p1;
        p[1] = p2;
        p[2] = p3;
        CALL_METHODS(pName, p, r);
        if( !r.isValid() )
          throw ALERROR("ALProxy", "call", "Waiting for the result of a void function");


        return r.Convert<R>();

      }
    }




    template <typename R, typename P1, typename P2, typename P3, typename P4>
    R call(const std::string& pName, const P1 &p1, const P2 &p2, const  P3 &p3,  const P4 &p4)
    {
      if(fIsLocal)
      {
        if (isScript())
        { 
          ALValue param;
          ALValue result;
          param.arraySetSize(4);
          param[0] = p1;
          param[1] = p2;
          param[2] = p3;
          param[3] = p4;
          getModule()->execute(pName, param, result);
          return result;
        }
#ifdef DEBUG
        const ALValue *lDesc = getModule()->getFunctionDesc(pName);
        if (!lDesc)
          throw ALERROR("ALProxy", "call", "Function does not exist");

        CHECK_RESULT()
          CHECK_PARAM(1)
          CHECK_PARAM(2)
          CHECK_PARAM(3)
          CHECK_PARAM(4)
#endif

          return (*(ALFunctor_4<ALModule, P1, P2, P3, P4, R> *)(getModule()->getFunction(pName)))(p1, p2, p3, p4);

      }
      else
      {
        ALValue p, r;
        p.arraySetSize(4);
        p[0] = p1;
        p[1] = p2;
        p[2] = p3;
        p[3] = p4;
        CALL_METHODS(pName, p, r);

        if( !r.isValid() )
          throw ALERROR("ALProxy", "call", "Waiting for the result of a void function");


        return r.Convert<R>();

      }
    }


    template <typename R, typename P1, typename P2, typename P3, typename P4, typename P5>
    R call(const std::string& pName, const  P1 &p1, const  P2 &p2, const  P3 &p3, const P4 &p4, const  P5 &p5)
    {

      if(fIsLocal)
      {
        if (isScript())
        { 
          ALValue param;
          ALValue result;
          param.arraySetSize(5);
          param[0] = p1;
          param[1] = p2;
          param[2] = p3;
          param[3] = p4;
          param[4] = p5;
          getModule()->execute(pName, param, result);
          return result;
        }
#ifdef DEBUG
        const ALValue *lDesc = getModule()->getFunctionDesc(pName);
        if (!lDesc)
          throw ALERROR("ALProxy", "call", "Function does not exist");

        CHECK_RESULT()
          CHECK_PARAM(1)
          CHECK_PARAM(2)
          CHECK_PARAM(3)
          CHECK_PARAM(4)
          CHECK_PARAM(5)
#endif
          return (*(ALFunctor_5<ALModule, P1, P2, P3, P4, P5, R> *)(getModule()->getFunction(pName)))(p1, p2, p3, p4, p5);
      }
      else
      {
        ALValue p, r;
        p.arraySetSize(5);
        p[0] = p1;
        p[1] = p2;
        p[2] = p3;
        p[3] = p4;
        p[4] = p5;
        CALL_METHODS(pName, p, r);
        if( !r.isValid() )
          throw ALERROR("ALProxy", "call", "Waiting for the result of a void function");

        return (R)r;
      }
    }



    template <typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
    R call(const std::string& pName,  const P1 &p1, const P2 &p2, const  P3 &p3, const  P4 &p4,  const P5 &p5, const P6 &p6)
    {
      if(fIsLocal)
      {
        if (isScript())
        { 
          ALValue param;
          ALValue result;
          param.arraySetSize(6);
          param[0] = p1;
          param[1] = p2;
          param[2] = p3;
          param[3] = p4;
          param[4] = p5;
          param[5] = p6;
          getModule()->execute(pName, param, result);
          return result;
        }
#ifdef DEBUG
        const ALValue *lDesc = getModule()->getFunctionDesc(pName);
        if (!lDesc)
          throw ALERROR("ALProxy", "call", "Function does not exist");

        CHECK_RESULT()
          CHECK_PARAM(1)
          CHECK_PARAM(2)
          CHECK_PARAM(3)
          CHECK_PARAM(4)
          CHECK_PARAM(5)
          CHECK_PARAM(6)
#endif
          return (*(ALFunctor_6<ALModule, P1, P2, P3, P4, P5, P6, R> *)(getModule()->getFunction(pName)))(p1, p2, p3, p4, p5, p6);
      }
      else
      {
        ALValue p, r;
        p.arraySetSize(6);
        p[0] = p1;
        p[1] = p2;
        p[2] = p3;
        p[3] = p4;
        p[4] = p5;
        p[5] = p6;

        CALL_METHODS(pName, p, r);
        if( !r.isValid() )
          throw ALERROR("ALProxy", "call", "Waiting for the result of a void function");

        return (R)r;
      }
    }

    /* ----------------------------------------------------------------------------------------------------------*/
    /* -----------------------    call template without return value        ----------------------------------------*/
    /* ----------------------------------------------------------------------------------------------------------*/


    // call without result
    void callVoid(const std::string& pName)
    {
      if (fIsLocal)
      {
        if (isScript())
        { 
          ALValue param;
          ALValue result;
          getModule()->execute(pName, param, result);
          return;
        }
#ifdef DEBUG
        const ALValue *lDesc = getModule()->getFunctionDesc(pName);
        if (!lDesc)
          throw ALERROR("ALProxy", "call", "Function does not exist");
#endif
        (*(ALFunctor0<ALModule, ALVoid> *)(getModule()->getFunction(pName)))();

      }
      else
      {
        ALValue p, r;

        CALL_METHODS( pName, p, r );

        return;
      }
    }


    // call return void with 1 const ref parameters
    template <typename P1>
    void callVoid(const std::string& pName, const P1 &p1)
    {
      if (fIsLocal)
      {
        if (isScript())
        { 
          ALValue param;
          ALValue result;
          param.arraySetSize(1);
          param[0] = p1;
          getModule()->execute(pName, param, result);
        }
#ifdef DEBUG
        const ALValue *lDesc = getModule()->getFunctionDesc(pName);
        if (!lDesc)
        {
          throw ALERROR("ALProxy", "call", "Function does not exist");
        }
        CHECK_PARAM(1)
#endif
          (*(ALFunctor_1<ALModule, P1, void> *)(getModule()->getFunction(pName)))(p1);

      }
      else
      {
        ALValue p, r;
        p.arraySetSize(1);
        p[0] = p1;
        CALL_METHODS( pName, p, r );
        return;
      }
    }


    // call return void with 2 const ref parameters
    template <typename P1, typename P2>
    void callVoid(const std::string& pName, const P1 &p1,  const P2 &p2)
    {
      if (fIsLocal)
      {
        if (isScript())
        { 
          ALValue param;
          ALValue result;
          param.arraySetSize(2);
          param[0] = p1;
          param[1] = p2;
          getModule()->execute(pName, param, result);
        }
#ifdef DEBUG
        const ALValue *lDesc = getModule()->getFunctionDesc(pName);
        if (!lDesc)
        {
          throw ALERROR("ALProxy", "call", "Function does not exist");
        }
        CHECK_PARAM(1)
        CHECK_PARAM(2)
#endif
          (*(ALFunctor_2<ALModule, P1, P2, void> *)(getModule()->getFunction(pName)))(p1, p2);

      }
      else
      {
        ALValue p, r;
        p.arraySetSize(2);
        p[0] = p1;
        p[1] = p2;
        CALL_METHODS( pName, p, r );
        return;
      }
    }



    // call return void with 3 const ref parameters
    template <typename P1, typename P2, typename P3>
    void callVoid(const std::string& pName, const P1 &p1,  const P2 &p2, const P3 &p3)
    {
      if (fIsLocal)
      {
        if (isScript())
        { 
          ALValue param;
          ALValue result;
          param.arraySetSize(3);
          param[0] = p1;
          param[1] = p2;
          param[2] = p3;
          getModule()->execute(pName, param, result);
        }
#ifdef DEBUG
        const ALValue *lDesc = getModule()->getFunctionDesc(pName);
        if (!lDesc)
        {
          throw ALERROR("ALProxy", "call", "Function does not exist");
        }
        CHECK_PARAM(1)
          CHECK_PARAM(2)
          CHECK_PARAM(3)
#endif
          (*(ALFunctor_3<ALModule, P1, P2, P3, void> *)(getModule()->getFunction(pName)))(p1, p2, p3);

      }
      else
      {
        ALValue p, r;
        p.arraySetSize(3);
        p[0] = p1;
        p[1] = p2;
        p[2] = p3;
        CALL_METHODS( pName, p, r );
        return;
      }
    }



    template <typename P1, typename P2, typename P3, typename P4>
    void callVoid(const std::string& pName,  const P1 &p1, const P2 &p2, const P3 &p3, const P4 &p4)
    {

      if (fIsLocal)
      {
        if (isScript())
        { 
          ALValue param;
          ALValue result;
          param.arraySetSize(4);
          param[0] = p1;
          param[1] = p2;
          param[2] = p3;
          param[3] = p4;
          getModule()->execute(pName, param, result);
          return;
        }

#ifdef DEBUG
        const ALValue *lDesc = getModule()->getFunctionDesc(pName);
        if (!lDesc)
          throw ALERROR("ALProxy", "call", "Function does not exist");

        CHECK_PARAM(1)
          CHECK_PARAM(2)
          CHECK_PARAM(3)
          CHECK_PARAM(4)
#endif

          (*(ALFunctor_4<ALModule, P1, P2, P3, P4, ALVoid> *)(getModule()->getFunction(pName)))(p1, p2, p3, p4);
      }
      else
      {
        ALValue p, r;

        p.arraySetSize(4);
        p[0] = p1;
        p[1] = p2;
        p[2] = p3;
        p[3] = p4;

        CALL_METHODS( pName, p, r );

        return;
      }
    }


    template <typename P1, typename P2, typename P3, typename P4, typename P5>
    void callVoid(const std::string& pName,  const P1 &p1, const  P2 &p2,  const P3 &p3, const P4 &p4, const P5 &p5)
    {

      if (fIsLocal)
      {
        if (isScript())
        { 
          ALValue param;
          ALValue result;
          param.arraySetSize(5);
          param[0] = p1;
          param[1] = p2;
          param[2] = p3;
          param[3] = p4;
          param[4] = p5;
          getModule()->execute(pName, param, result);
          return;
        }
#ifdef DEBUG
        const ALValue *lDesc = getModule()->getFunctionDesc(pName);
        if (!lDesc)
          throw ALERROR("ALProxy", "call", "Function does not exist");

        CHECK_PARAM(1)
        CHECK_PARAM(2)
        CHECK_PARAM(3)
        CHECK_PARAM(4)
        CHECK_PARAM(5)
#endif

          (*(ALFunctor_5<ALModule, P1, P2, P3, P4, P5, ALVoid> *)(getModule()->getFunction(pName)))(p1, p2, p3, p4, p5);
      }
      else
      {
        ALValue p, r;

        p.arraySetSize(5);
        p[0] = p1;
        p[1] = p2;
        p[2] = p3;
        p[3] = p4;
        p[4] = p5;

        CALL_METHODS( pName, p, r );

        return;
      }
    }


    template <typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
    void callVoid(const std::string& pName,  const P1 &p1, const  P2 &p2, const  P3 &p3, const  P4 &p4,  const P5 &p5, const  P6 &p6)
    { 
      if (fIsLocal)
      {
        if (isScript())
        { 
          ALValue param;
          ALValue result;
          param.arraySetSize(6);
          param[0] = p1;
          param[1] = p2;
          param[2] = p3;
          param[3] = p4;
          param[4] = p5;
          param[5] = p6;
          getModule()->execute(pName, param, result);
          return;
        }
#ifdef DEBUG
        const ALValue *lDesc = getModule()->getFunctionDesc(pName);
        if (!lDesc)
          throw ALERROR("ALProxy", "call", "Function does not exist");

        CHECK_PARAM(1)
        CHECK_PARAM(2)
        CHECK_PARAM(3)
        CHECK_PARAM(4)
        CHECK_PARAM(5)
        CHECK_PARAM(6)
#endif
          (*(ALFunctor_6<ALModule, P1, P2, P3, P4, P5, P6, ALVoid> *)(getModule()->getFunction(pName)))(p1, p2, p3, p4, p5, p6);
      }
      else
      {
        ALValue p, r;

        p.arraySetSize(6);
        p[0] = p1;
        p[1] = p2;
        p[2] = p3;
        p[3] = p4;
        p[4] = p5;
        p[5] = p6;

        CALL_METHODS( pName, p, r );

        return;
      }
    }


    /* ----------------------------------------------------------------------------------------------------------*/
    /* -----------------------    pcall template (without return value)        ----------------------------------------*/
    /* ----------------------------------------------------------------------------------------------------------*/


    //pCall
    int pCall(const std::string& pName)
    {
      if (fIsLocal)
      {
        if (isScript())
        {
          ALValue param;
          ALValue r = getModule()->getParentBroker()->TaskID(getModule()->getModuleInfo()->name, pName); 
          getModule()->getParentBroker()->getThreadPool()->enqueue(ALPtr<_pcallScript>(new _pcallScript(getModule(), pName, param)));
          return r[0];
        }

        const ALValue *lDesc = getModule()->getFunctionDesc(pName);
        if (!lDesc)
          throw ALERROR("ALProxy", "call", "Function does not exist");

        ALValue r = getParentBroker()->TaskID(getModule()->getModuleInfo()->name,pName); 
        getModule()->getParentBroker()->getThreadPool()->enqueue(ALPtr<_pcall0>(new _pcall0((ALFunctor0<ALModule, ALVoid> *)(getModule()->getFunction(pName)),r,getModule())));
        return r[0];
      }
      else
      {
        ALValue p, r;
        PCALL_METHODS( pName, p, r );
        return r[0];
      }
    }


    template <typename P1>
    int pCall(const std::string& pName, const P1 &p1)
    {
      AL_PROXY_DEBUG( "void call(std::string pName, P1 p1)\n" );

      if (fIsLocal)
      {
        if (isScript())
        { 
          ALValue param;
          param.arraySetSize(1);
          param[0] = p1;

          ALValue r = getModule()->getParentBroker()->TaskID(getModule()->getModuleInfo()->name, pName); 
          getModule()->getParentBroker()->getThreadPool()->enqueue(ALPtr<_pcallScript>(new _pcallScript(getModule(), pName, param)));
          return r[0];
        }
#ifdef DEBUG
        const ALValue *lDesc = getModule()->getFunctionDesc(pName);
        if (!lDesc)
          throw ALERROR("ALProxy", "call", "Function does not exist");

        CHECK_PARAM(1)
#endif

          ALValue r = getParentBroker()->TaskID(getModule()->getModuleInfo()->name,pName); 
        DEBUG_TASK_MSG(getParentBroker()->getTaskMonitor()->show())
          getModule()->getParentBroker()->getThreadPool()->enqueue(ALPtr<_pcall1<P1> >(new _pcall1<P1>((ALFunctor_1<ALModule, P1, ALVoid> *)(getModule()->getFunction(pName)), p1,getModule(),r )));
        return r[0];
      }
      else
      {
        ALValue p, r;

        p.arraySetSize(1);
        p[0] = p1;

        PCALL_METHODS( pName, p, r );

        return r[0];
      }
    }


    template <typename P1, typename P2>
    int pCall(const std::string& pName, const P1 &p1, const  P2 &p2)
    {
      if (fIsLocal)
      {
        if (isScript())
        { 
          ALValue param;
          param.arraySetSize(2);
          param[0] = p1;
          param[1] = p2;

          ALValue r = getModule()->getParentBroker()->TaskID(getModule()->getModuleInfo()->name, pName); 
          getModule()->getParentBroker()->getThreadPool()->enqueue(ALPtr<_pcallScript>(new _pcallScript(getModule(), pName, param)));
          return r[0];
        }

#ifdef DEBUG
        const ALValue *lDesc = getModule()->getFunctionDesc(pName);
        if (!lDesc)
          throw ALERROR("ALProxy", "call", "Function does not exist");

        CHECK_PARAM(1)
          CHECK_PARAM(2)
#endif

          ALValue r = getModule()->getParentBroker()->TaskID(getModule()->getModuleInfo()->name,pName); 

        getModule()->getParentBroker()->getThreadPool()->enqueue(ALPtr<_pcall2<P1, P2> >(new _pcall2<P1, P2>((ALFunctor_2<ALModule, P1, P2, ALVoid> *)(getModule()->getFunction(pName)), p1, p2,getModule(),r )));
        return r[0];
      }
      else
      {
        ALValue p, r;
        p.arraySetSize(2);
        p[0] = p1;
        p[1] = p2;
        PCALL_METHODS( pName, p, r );

        return r[0];
      }
    }

template <typename P1, typename P2, typename P3>
    int pCallNoID(const std::string& pName, const P1 &p1, const P2 &p2, const  P3 &p3)
    {
      if (fIsLocal)
      {
        if (isScript())
        {
          ALValue param;
          param.arraySetSize(3);
          param[0] = p1;
          param[1] = p2;
          param[2] = p3;

          ALValue r = getModule()->getParentBroker()->TaskID(getModule()->getModuleInfo()->name, pName); 
          getModule()->getParentBroker()->getThreadPool()->enqueue(ALPtr<_pcallScript>(new _pcallScript(getModule(), pName, param)));
          return r[0];
        }
#ifdef DEBUG
        const ALValue *lDesc = getModule()->getFunctionDesc(pName);
        if (!lDesc)
          throw ALERROR("ALProxy", "call", "Function does not exist");

        CHECK_PARAM(1)
          CHECK_PARAM(2)
          CHECK_PARAM(3)
#endif

          ALValue r  = getModule()->getParentBroker()->TaskID(getModule()->getModuleInfo()->name,pName); 
        getModule()->getParentBroker()->getThreadPool()->enqueue(ALPtr<_pcall3<P1, P2, P3> >(new _pcall3<P1, P2, P3>((ALFunctor_3<ALModule, P1, P2, P3, ALVoid> *)(getModule()->getFunction(pName)), p1, p2, p3,getModule(),r)));
        return r[0];
      }
      else
      {
        ALValue p, r;
        p.arraySetSize(3);
        p[0] = p1;
        p[1] = p2;
        p[2] = p3;
        PCALL_METHODSNoID( pName, p, r );
        return r[0];
      }
    }


    template <typename P1, typename P2, typename P3>
    int pCall(const std::string& pName, const P1 &p1, const P2 &p2, const  P3 &p3)
    {
      if (fIsLocal)
      {
        if (isScript())
        {
          ALValue param;
          param.arraySetSize(3);
          param[0] = p1;
          param[1] = p2;
          param[2] = p3;

          ALValue r = getModule()->getParentBroker()->TaskID(getModule()->getModuleInfo()->name, pName); 
          getModule()->getParentBroker()->getThreadPool()->enqueue(ALPtr<_pcallScript>(new _pcallScript(getModule(), pName, param)));
          return r[0];
        }
#ifdef DEBUG
        const ALValue *lDesc = getModule()->getFunctionDesc(pName);
        if (!lDesc)
          throw ALERROR("ALProxy", "call", "Function does not exist");

        CHECK_PARAM(1)
          CHECK_PARAM(2)
          CHECK_PARAM(3)
#endif

          ALValue r  = getModule()->getParentBroker()->TaskID(getModule()->getModuleInfo()->name,pName); 
        getModule()->getParentBroker()->getThreadPool()->enqueue(ALPtr<_pcall3<P1, P2, P3> >(new _pcall3<P1, P2, P3>((ALFunctor_3<ALModule, P1, P2, P3, ALVoid> *)(getModule()->getFunction(pName)), p1, p2, p3,getModule(),r)));
        return r[0];
      }
      else
      {
        ALValue p, r;
        p.arraySetSize(3);
        p[0] = p1;
        p[1] = p2;
        p[2] = p3;
        PCALL_METHODS( pName, p, r );
        return r[0];
      }
    }



    template <typename P1, typename P2, typename P3, typename P4>
    ALValue pCall(const std::string& pName, const P1 &p1, const  P2 &p2, const  P3 &p3, const  P4 &p4)
    {

      if (fIsLocal)
      {
        if (isScript())
        { 
          ALValue param;
          param.arraySetSize(4);
          param[0] = p1;
          param[1] = p2;
          param[2] = p3;
          param[3] = p4;

          ALValue r = getModule()->getParentBroker()->TaskID(getModule()->getModuleInfo()->name, pName); 
          getModule()->getParentBroker()->getThreadPool()->enqueue(ALPtr<_pcallScript>(new _pcallScript(getModule(), pName, param)));
          return r[0];
        }
#ifdef DEBUG
        const ALValue *lDesc = getModule()->getFunctionDesc(pName);
        if (!lDesc)
          throw ALERROR("ALProxy", "call", "Function does not exist");

        CHECK_PARAM(1)
          CHECK_PARAM(2)
          CHECK_PARAM(3)
          CHECK_PARAM(4)
#endif

          ALValue r  = getParentBroker()->TaskID(getModule()->getModuleInfo()->name,pName); 

        getModule()->getParentBroker()->getThreadPool()->enqueue(ALPtr<_pcall4<P1, P2, P3, P4> >(new _pcall4<P1, P2, P3, P4>((ALFunctor_4<ALModule, P1, P2, P3, P4, ALVoid> *)(getModule()->getFunction(pName)), p1, p2, p3, p4,getModule(), r )));
        return r[0];
      }
      else
      {
        ALValue p, r;

        p.arraySetSize(4);
        p[0] = p1;
        p[1] = p2;
        p[2] = p3;
        p[3] = p4;
        PCALL_METHODS( pName, p, r );
        return r[0];
      }
    }



    template <typename P1, typename P2, typename P3, typename P4, typename P5>
    int pCall(const std::string& pName, const P1 &p1, const  P2 &p2, const  P3 &p3,  const P4 &p4, const P5 &p5)
    {

      if (fIsLocal)
      {
        if (isScript())
        { 
          ALValue param;
          param.arraySetSize(5);
          param[0] = p1;
          param[1] = p2;
          param[2] = p3;
          param[3] = p4;
          param[4] = p5;

          ALValue r = getModule()->getParentBroker()->TaskID(getModule()->getModuleInfo()->name, pName); 
          getModule()->getParentBroker()->getThreadPool()->enqueue(ALPtr<_pcallScript>(new _pcallScript(getModule(), pName, param)));
          return r[0];
        }
#ifdef DEBUG
        const ALValue *lDesc = getModule()->getFunctionDesc(pName);
        if (!lDesc)
          throw ALERROR("ALProxy", "call", "Function does not exist");

        CHECK_PARAM(1)
          CHECK_PARAM(2)
          CHECK_PARAM(3)
          CHECK_PARAM(4)
          CHECK_PARAM(5)
#endif

          ALValue r = getParentBroker()->TaskID(getModule()->getModuleInfo()->name,pName); 

        DEBUG_TASK_MSG(getParentBroker()->getTaskMonitor()->show()) 
          getModule()->getParentBroker()->getThreadPool()->enqueue(ALPtr<_pcall5<P1, P2, P3, P4, P5> >(new _pcall5<P1, P2, P3, P4, P5>((ALFunctor_5<ALModule, P1, P2, P3, P4, P5, ALVoid> *)(getModule()->getFunction(pName)), p1, p2, p3, p4, p5,getModule(), r )));
        return r[0];
      }
      else
      {
        ALValue p, r;

        p.arraySetSize(5);
        p[0] = p1;
        p[1] = p2;
        p[2] = p3;
        p[3] = p4;
        p[4] = p5;

        PCALL_METHODS( pName, p, r );

        return r[0];
      }
    }


    template <typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
    int pCall(const std::string& pName,  const P1 &p1, const  P2 &p2, const P3 &p3, const  P4 &p4, const P5 &p5, const P6 &p6)
    {
      if (fIsLocal)
      {
        if (isScript())
        { 
          ALValue param;
          param.arraySetSize(6);
          param[0] = p1;
          param[1] = p2;
          param[2] = p3;
          param[3] = p4;
          param[4] = p5;
          param[5] = p6;

          ALValue r = getModule()->getParentBroker()->TaskID(getModule()->getModuleInfo()->name, pName); 
          getModule()->getParentBroker()->getThreadPool()->enqueue(ALPtr<_pcallScript>(new _pcallScript(getModule(), pName, param)));
          return r[0];
        }
#ifdef DEBUG
        const ALValue *lDesc = getModule()->getFunctionDesc(pName);
        if (!lDesc)
          throw ALERROR("ALProxy", "call", "Function does not exist");

        CHECK_PARAM(1)
          CHECK_PARAM(2)
          CHECK_PARAM(3)
          CHECK_PARAM(4)
          CHECK_PARAM(5)
          CHECK_PARAM(6)
#endif

          ALValue r = getParentBroker()->TaskID(getModule()->getModuleInfo()->name,pName);  


        getModule()->getParentBroker()->getThreadPool()->enqueue(ALPtr<_pcall6<P1, P2, P3, P4, P5, P6> >(new _pcall6<P1, P2, P3, P4, P5, P6>((ALFunctor_6<ALModule, P1, P2, P3, P4, P5, P6, ALVoid> *)(getModule()->getFunction(pName)), p1, p2, p3, p4, p5, p6, getModule(), r)));
        return r[0];
      }
      else
      {
        ALValue p, r;

        p.arraySetSize(6);
        p[0] = p1;
        p[1] = p2;
        p[2] = p3;
        p[3] = p4;
        p[4] = p5;
        p[5] = p6;

        PCALL_METHODS( pName, p, r );

        return r[0];
      }



    }


  public :

    ALWeakPtr<ALBroker> parentBroker;

    inline ALPtr<ALBroker> getParentBroker(void) const
    {
      ALPtr<ALBroker> returnPtr = parentBroker.lock();
      if (returnPtr == NULL)
      {
        printf("no broker available");
        throw ALERROR( "ALBroker", "no broker available","" );
      }
      return returnPtr;
    }

    /*
    * Basic constructor
    */
    ALProxy(ALPtr<ALBroker> pBroker , const std::string& pModuleName );

    /**
    *This constructeur allow to use proxy without instancing a broker
    */
    ALProxy(const std::string& pModuleName, const std::string& pIp, int pPort, ALPtr<ALBroker> pBroker = ALPtr<ALBroker>() );

    /*
    * Basic destructor
    */
    virtual ~ALProxy(void);

    int getInfo( ALModuleInfo &pModuleInfo );

    bool isLocal( ){ return fIsLocal; };

    bool isValid(){return fIsValid;};

    /**
    * A generic call, without type optimisation, using ALValue for each call... (local or not)
    *
    * Used from a C++ code not knowing the signature of the called method
    *
    * @params strMethodName the name of the method to call
    * @params listParams the list of parameters (possibly no params)
    *
    * @return an alvalue, possibly invalid if function returns nothing
    */      
    ALValue genericCall( const std::string& strMethodName, const ALValue& listParams );
    void genericCall2( const std::string& strMethodName, const ALValue& listParams );
    typedef int (*TGenericCall_CallbackOnFinished)( const char* pszCallerName, int nID, const ALValue & result );
    /**
    * A generic post-call, without type optimisation, using ALValue for each call... (local or not)
    *
    * Used from a C++ code not knowing the signature of the called method
    *
    * @params strMethodName the name of the method to call
    * @params listParams the list of parameters (possibly no params)
    * @params pCallbackToCallOnFinished a callback to call when task is finished (NULL if no callback)
    * @params pszCallerName the name of the caller (proxy name or module name, dependings on langage, script, caller...)
    *
    * @return nID: ID of the new created task (0 if error)
    */      

    int genericPCall( const std::string& strMethodName, const ALValue& listParams, TGenericCall_CallbackOnFinished pCallbackToCallOnFinished = NULL, const char* pszCallerName = NULL );

    /**
    * A generic post-call, without type optimisation, using ALValue for each call... (local or not)
    *
    * Used from a C++ code not knowing the signature of the called method
    *
    * @params pName the name of the method to call
    * @params param the list of parameters ( no param : empty alvalue ) 
    * @return ID: 
    */ 
    int genericPCall2( const std::string& pName, const ALValue& pParam);

    /**
    * A generic post-call, without type optimisation, using ALValue for each call... (local or not)
    *
    * Used from a C++ code not knowing the signature of the called method
    *
    * @params pMod module to notify when finished
    * @params pName the name of the method to call
    * @params param the list of parameters ( no param : empty alvalue ) 
    * @return ID: 
    */ 
    ALValue genericPCallN(ALPtr<ALModule> pMod, const std::string& pName, const ALValue& pParam);

  private:
    int xGetFreeTaskUID( void );

    enum typeOfProxy
    {
      IN_DISTRIBUTED_TREE = 0,
      OUT_DISTRIBUTED_TREE  = 1,
    };

    typeOfProxy typeProxy;
  public:



    /**
    * version
    * @return the revision number number
    */
    std::string version();



  public:

    inline ALPtr<ALModule> getModule() const
    { 
      ALPtr<ALModule> returnPtr = fModule.lock();
      if (returnPtr == NULL)
      {
        throw ALERROR("ALProxy", "getModule", std::string( "module is deleted, cannot call function of " ) + fModuleInfo->name ); 
      }
      return returnPtr; 
    };

    inline std::string getParentBrokerName()
    {
      return fParentBrokerName;
    }

  protected :

    std::string fEndpoint;
    struct soap *fBrokerSoap;
    ALPtr<ALValue> fConnectionInfo;
    ALPtr<ALMutex> fBrokerSoap_mutex;
    ALPtr<ALModuleInfo> fModuleInfo;
    ALWeakPtr<ALModule> fModule;
    bool fIsLocal;
    bool fIsValid; 
    ALWeakPtr<ALBroker> fParentBroker;
    std::string fParentBrokerName;
  }; // ALProxy



} // namespace AL


#endif


