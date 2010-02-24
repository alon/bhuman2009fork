/**
* @author Aldebaran Robotics 
* Aldebaran Robotics (c) 2007 All Rights Reserved - This file is confidential.\n
*
* Version : $Id$
*/


#ifndef AL_FUNCTOR_H
#define AL_FUNCTOR_H

// Activate or desactivate log
//#define AL_FUNCTOR_DEBUG(...) printf(__VA_ARGS__)
#define AL_FUNCTOR_DEBUG(...)

// Common base for all Functors!

#include "alerror.h"
#include "alvalue.h"
#include <string>
#include <pthread.h>
#include "alptr.h"
//#include "alsignal.hpp"
//#include <boost/signal.hpp>

#ifdef WIN32
typedef void ALVoid;
#else
// class ALVoid { };
typedef void ALVoid;
#endif



//typedef AL::Signal<void ()> voidSignal;
//typedef AL::Signal<void ()> voidSignal;
//AL::Signal<void ()> sig;
//AL::Signal2 < void , int , int  > sig2; 
 
/*
template < void , typename T0 > 
class bFunctor2
{
  boost::signal2 < void , int , int > sig2;
}*/

/**
* Generic functor class
*/

class ALFunctorBase
{
public:
  /**
  * generic call to a bound module's method
  * @param pParams, input parameter
  * @param pResult, output result
  */
  virtual void call (const AL::ALValue& pParams, AL::ALValue& pResult) = 0;

  virtual ~ALFunctorBase(){};

};

#define AL_FUNCTOR_ASSUME_NBR_PARAMS(params,nbrparamsrequired) \
  if( params.getSize() != nbrparamsrequired ) \
{ \
  throw ALERROR( "ALFunctor", "call", std::string( "Wrong number of parameters for this call requiring: " #nbrparamsrequired " and received: " ) + DecToString( params.getSize() ) + " (total params is: " + params.toString( AL::VerbosityMini ) ); \
} \

#include "alfunctorwithreturn.h"
#include "alfunctorwithoutreturn.h"
#include "alfunctorcreate.h"

#endif
