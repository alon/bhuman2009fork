/**
* @author Aldebaran Robotics 
* Aldebaran Robotics (c) 2007 All Rights Reserved - This file is confidential.\n
*
* Version : $Id$
*/


#ifndef AL_FUNCTOR_WITH_RETURN_H_
# define AL_FUNCTOR_WITH_RETURN_H_
# include "alfunctor.h"

// Functor with return values

// 0 parameter Functor

template <typename C, typename R = ALVoid>
class ALFunctor0 : public ALFunctorBase
{
public:

  ALFunctor0(C *pObject, R (C::*pFunction) ()) : fObject(pObject), fFunction (pFunction) { };

  R operator() () { return (R)(fObject->*fFunction)(); };

  void call (const AL::ALValue& /* pParams */, AL::ALValue& pResult)
  {
    pResult = (R)(fObject->*fFunction)();
  };

private:
  C *fObject;
  typedef R (C::*functionType) ();
  functionType fFunction;
};


// 1 parameter Functor
template <typename C, typename P1, typename R = ALVoid>
class ALFunctor_1 : public ALFunctorBase
{
public:

  ALFunctor_1(C *pObject, R (C::*pFunction) (const P1 &)) : fObject(pObject), fFunction (pFunction) { };

  R operator() (const P1 &p1) { return (R)(fObject->*fFunction)(p1); };

  void call (const AL::ALValue& pParams, AL::ALValue& pResult)
  {
    // Parameters checkin'
#ifdef DEBUG
    AL_FUNCTOR_ASSUME_NBR_PARAMS( pParams, 1 );
    AL_FUNCTOR_DEBUG( "in call(), params=%s\n", pParams.toString( VerbosityFull ).c_str() );
#endif
    ALValue p1 = pParams[0];
    pResult = (R)(fObject->*fFunction)(p1);
  };

private:
  C *fObject;
  typedef R (C::*functionType) (const P1 &);
  functionType fFunction;
};


// 2 parameters Functor




// reference functor
template <typename C, typename P1, typename P2, typename R = ALVoid>
class ALFunctor_2 : public ALFunctorBase
{
public:

  ALFunctor_2(C *pObject, R (C::*pFunction) (const P1 &, const P2 &)) : fObject(pObject), fFunction (pFunction) { };

  R operator() (const P1 &p1, const P2 &p2) { return (R)(fObject->*fFunction)(p1, p2); };

  void call (const AL::ALValue& pParams, AL::ALValue& pResult)
  {
    // Parameters checkin'
#ifdef DEBUG
    AL_FUNCTOR_ASSUME_NBR_PARAMS( pParams, 2 );
#endif

    ALValue p1 = pParams[0];
    ALValue p2 = pParams[1];
    pResult = (R)(fObject->*fFunction)(p1, p2);
    
  };

private:
  C *fObject;
  typedef R (C::*functionType) (const P1 &, const P2 &);
  functionType fFunction;
};




template <typename C, typename P1, typename P2, typename P3, typename R = ALVoid>
class ALFunctor_3 : public ALFunctorBase
{
public:

  ALFunctor_3(C *pObject, R (C::*pFunction) (const P1 &, const P2 &, const P3 &)) : fObject(pObject), fFunction (pFunction) { };

  R operator() (const P1 &p1, const P2 &p2, const P3 &p3) { return (R)(fObject->*fFunction)(p1, p2, p3); };

  void call (const AL::ALValue& pParams, AL::ALValue& pResult)
  {
    // Parameters checkin'
#ifdef DEBUG
    AL_FUNCTOR_ASSUME_NBR_PARAMS( pParams, 3 );
#endif
    ALValue p1 = pParams[0];
    ALValue p2 = pParams[1];
    ALValue p3 = pParams[2];
    pResult = (R)(fObject->*fFunction)(p1, p2,p3);
  
  };

private:
  C *fObject;
  typedef R (C::*functionType) (const P1 &, const P2 &, const P3 &);
  functionType fFunction;
};



// 4 parameters Functor

template <typename C, typename P1, typename P2, typename P3, typename P4, typename R = ALVoid>
class ALFunctor_4 : public ALFunctorBase
{
public:

  ALFunctor_4(C *pObject, R (C::*pFunction) (const P1 &, const P2 &, const P3 &, const P4 &)) : fObject(pObject), fFunction (pFunction) { };

  R operator() (const P1 &p1, const P2 &p2,const  P3 &p3, const P4 &p4) { return (R)(fObject->*fFunction)(p1, p2, p3, p4); };

  void call (const AL::ALValue& pParams, AL::ALValue& pResult)
  {
    // Parameters checkin'
#ifdef DEBUG
    AL_FUNCTOR_ASSUME_NBR_PARAMS( pParams, 4 );
#endif
    ALValue p1 = pParams[0];
    ALValue p2 = pParams[1];
    ALValue p3 = pParams[2];
    ALValue p4 = pParams[3];
    pResult = (R)(fObject->*fFunction)(p1, p2,p3,p4);
   
  };

private:
  C *fObject;
  typedef R (C::*functionType) (const P1 &, const P2 &, const P3 &, const P4 &);
  functionType fFunction;
};

// 5 parameters Functor

template <typename C, typename P1, typename P2, typename P3, typename P4, typename P5, typename R = ALVoid>
class ALFunctor_5 : public ALFunctorBase
{
public:

  ALFunctor_5(C *pObject, R (C::*pFunction) (const P1 &, const  P2 &, const P3 &, const P4 &, const P5 &)) : fObject(pObject), fFunction (pFunction) { };

  R operator() (const P1 &p1, const P2 &p2, const P3 &p3, const P4 &p4, const P5 &p5) { return (R)(fObject->*fFunction)(p1, p2, p3, p4, p5); };

  void call (const AL::ALValue& pParams, AL::ALValue& pResult)
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
    pResult = (R)(fObject->*fFunction)(p1, p2,p3,p4,p5);
    
  };

private:
  C *fObject;
  typedef R (C::*functionType) (const P1 &, const P2 &, const P3 &, const P4 &, const P5 &);
  functionType fFunction;
};

// 6 parameters Functor

template <typename C, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename R = ALVoid>
class ALFunctor_6 : public ALFunctorBase
{
public:

  ALFunctor_6(C *pObject, R (C::*pFunction) (const P1 &, const  P2 &, const P3 &, const  P4 &, const  P5 &, const P6 &)) : fObject(pObject), fFunction (pFunction) { };

  R operator() (const P1  &p1, const  P2 &p2, const P3 &p3, const P4 &p4, const P5 &p5, const P6 &p6) { return (R)(fObject->*fFunction)(p1, p2, p3, p4, p5, p6); };

  void call (const AL::ALValue& pParams, AL::ALValue& pResult)
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
    pResult = (R)(fObject->*fFunction)(p1, p2,p3,p4,p5,p6);
    
  };

private:
  C *fObject;
  typedef R (C::*functionType) (const P1 &, const P2 &, const P3 &, const P4 &, const P5 &, const P6 &);
  functionType fFunction;
};

#endif
