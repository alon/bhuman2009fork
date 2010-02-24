/**
* @author Aldebaran Robotics 
* Aldebaran Robotics (c) 2007 All Rights Reserved - This file is confidential.\n
*
* Version : $Id$
*/


#ifndef AL_FUNCTOR_WITHOUT_RETURN_H_
# define AL_FUNCTOR_WITHOUT_RETURN_H_
# include "alfunctor.h"
# include "alfunctorwithreturn.h"
#include "boost/bind.hpp"
#include "boost/signal.hpp"
#include "boost/function.hpp"
#include "alptr.h"


// Functor without return values

// 0 parameter Functor


template <typename C>
class ALFunctor0 <C, ALVoid> : public ALFunctorBase
{
public:

  ALFunctor0(C *pObject, void (C::*pFunction) ()) : fObject(pObject), fFunction (pFunction) { };

  void operator() () { (fObject->*fFunction)(); };

  void call (const AL::ALValue& pParams, AL::ALValue&  /*pResult*/)
  {
    (fObject->*fFunction)();
  };

private:
  C *fObject;
  typedef void (C::*functionType) ();
  functionType fFunction;
};





template <typename C, typename P1>
class ALFunctor_1 <C, P1, ALVoid> : public ALFunctorBase
{
public:

  ALFunctor_1(C *pObject, void (C::*pFunction) (const P1 &)) : fObject(pObject), fFunction (pFunction) { }

  void operator() (const P1 &p1) { (fObject->*fFunction)(p1); };

  void call (const AL::ALValue& pParams, AL::ALValue&  /*pResult*/)
  {
    // Parameters checkin'
#ifdef DEBUG
    AL_FUNCTOR_ASSUME_NBR_PARAMS( pParams, 1 );
#endif

    ALValue p1 = pParams[0];
    (fObject->*fFunction)(p1);
    

  };

private:
  C *fObject;
  typedef void (C::*functionType) (const P1 &);
  functionType fFunction;
};

// 2 parameters Functor

template <typename C, typename P1, typename P2>
class ALFunctor_2 <C, P1, P2, ALVoid> : public ALFunctorBase
{
public:

  ALFunctor_2(C *pObject, void (C::*pFunction) (const P1 &, const P2 &)) : fObject(pObject), fFunction (pFunction) { };

  void operator() (const P1 &p1, const P2 &p2) { (fObject->*fFunction)(p1, p2); };

  void call (const AL::ALValue& pParams, AL::ALValue&  )
  {
    // Parameters checkin'
#ifdef DEBUG
    AL_FUNCTOR_ASSUME_NBR_PARAMS( pParams, 2 );
#endif

    ALValue p1 = pParams[0];
    ALValue p2 = pParams[1];
    (fObject->*fFunction)(p1, p2);
    
  };

private:
  C *fObject;
  typedef void (C::*functionType) (const P1 &, const P2 &);
  functionType fFunction;
};

// 3 parameters Functor



template <typename C, typename P1, typename P2, typename P3>
class ALFunctor_3 <C, P1, P2, P3, ALVoid> : public ALFunctorBase
{
public:

  ALFunctor_3(C *pObject, void (C::*pFunction) (const P1 &,const  P2 &, const P3 &)) : fObject(pObject), fFunction (pFunction) { };

  inline void operator() (const P1 &p1, const P2 &p2, const P3 &p3) { (fObject->*fFunction)(p1, p2, p3); };

  void call (const AL::ALValue& pParams, AL::ALValue& /*pResult*/)
  {

    // Parameters checkin'
    AL_FUNCTOR_ASSUME_NBR_PARAMS( pParams, 3 );
    ALValue p1 = pParams[0];
    ALValue p2 = pParams[1];
    ALValue p3 = pParams[2];
    (fObject->*fFunction)(p1,p2,p3);
  
  };

private:
  C *fObject;
  //typedef void (C::*functionType) (P1, P2, P3);
  typedef void (C::*functionType) (const P1 & ,const P2 &, const P3 &);
  functionType fFunction;
};


// 4 parameters Functor

template <typename C, typename P1, typename P2, typename P3, typename P4>
class ALFunctor_4 <C, P1, P2, P3, P4, ALVoid> : public ALFunctorBase
{
public:

  ALFunctor_4(C *pObject, void (C::*pFunction) (const P1 &, const P2&, const P3&, const P4 &)) : fObject(pObject), fFunction (pFunction) { };

  void operator() (const P1 &p1, const P2 &p2, const P3 &p3, const P4 &p4) { (fObject->*fFunction)(p1, p2, p3, p4); };

  void call (const AL::ALValue& pParams, AL::ALValue&  /*pResult*/)
  {
    // Parameters checkin'
#ifdef DEBUG
    AL_FUNCTOR_ASSUME_NBR_PARAMS( pParams, 4 );
#endif
    ALValue p1 = pParams[0];
    ALValue p2 = pParams[1];
    ALValue p3 = pParams[2];
    ALValue p4 = pParams[3];

    (fObject->*fFunction)(p1,p2,p3,p4);
    
  };

private:
  C *fObject;
  typedef void (C::*functionType) (const P1 &, const P2 &, const P3 &, const P4 &);
  functionType fFunction;
};

// 5 parameters Functor

template <typename C, typename P1, typename P2, typename P3, typename P4, typename P5>
class ALFunctor_5 <C, P1, P2, P3, P4, P5, ALVoid> : public ALFunctorBase
{
public:

  ALFunctor_5(C *pObject, void (C::*pFunction) (const P1 &, const P2 &, const P3 &, const P4 &, const P5 &)) : fObject(pObject), fFunction (pFunction) { };

  void operator() (const P1 &p1, const P2 &p2, const P3 &p3, const P4 &p4, const P5 &p5) { (fObject->*fFunction)(p1, p2, p3, p4, p5); };

  void call (const AL::ALValue& pParams, AL::ALValue&  /*pResult*/)
  {
    // Parameters checkin'
#ifdef DEBUG
    AL_FUNCTOR_ASSUME_NBR_PARAMS( pParams, 5 );
#endif

    ALValue p1 = pParams[0];
    ALValue p2 = pParams[1];
    ALValue p3 = pParams[2];
    ALValue p4 = pParams[3];
    ALValue p5 = pParams[4];
    (fObject->*fFunction)(p1,p2,p3,p4,p5);
    
  };

private:
  C *fObject;
  typedef void (C::*functionType) (const P1 &, const P2 &, const P3 &,  const P4 &, const P5 &);
  functionType fFunction;
};

// 6 parameters Functor

template <typename C, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
class ALFunctor_6 <C, P1, P2, P3, P4, P5, P6, ALVoid> : public ALFunctorBase
{
public:

  ALFunctor_6(C *pObject, void (C::*pFunction) (const P1 &, const P2 &, const P3 &, const P4 &, const P5 &, const P6 &)) : fObject(pObject), fFunction (pFunction) { };

  void operator() (const P1 &p1, const P2 &p2, const P3 &p3, const P4 &p4, const P5 &p5, const P6 &p6) { (fObject->*fFunction)(p1, p2, p3, p4, p5, p6); };

  void call (const AL::ALValue& pParams, AL::ALValue&  /*pResult*/)
  {
    // Parameters checkin'
#ifdef DEBUG
    AL_FUNCTOR_ASSUME_NBR_PARAMS( pParams, 6 );
#endif
    ALValue p1 = pParams[0];
    ALValue p2 = pParams[1];
    ALValue p3 = pParams[2];
    ALValue p4 = pParams[3];
    ALValue p5 = pParams[4];
    ALValue p6 = pParams[5];
    (fObject->*fFunction)(p1,p2,p3,p4,p5,p6);
  };

private:
  C *fObject;
  typedef void (C::*functionType) (const P1 &, const P2 &, const P3 &, const P4 &, const P5 &, const P6 &);
  functionType fFunction;
};


#endif
