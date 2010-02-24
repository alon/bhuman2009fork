/**
* @author Aldebaran Robotics 
* Aldebaran Robotics (c) 2007 All Rights Reserved - This file is confidential.\n
*
* Version : $Id$
*/


#ifndef AL_FUNCTOR_CREATE_H_
# define AL_FUNCTOR_CREATE_H_

# include <iostream>
# include "alfunctor.h"
# include "alfunctorwithreturn.h"
# include "alfunctorwithoutreturn.h"
#include "alptr.h"

// Create Functors
// Functor manager parameter of bound functions, limitation to 8 parameters
// 0 Parameter

template <typename C, typename R>
AL::ALPtr<ALFunctorBase>
createFunctor (C *obj, R (C::*f) ())
{
  return AL::ALPtr<ALFunctorBase> (new ALFunctor0<C, R>(obj, f));
}





// 1 Parameter

template <typename C, typename P1, typename R>
AL::ALPtr<ALFunctorBase>
createFunctor (C *obj, R (C::*f) (const P1 &))
{
  return AL::ALPtr<ALFunctorBase>(new ALFunctor_1<C, P1, R>(obj, f));
}


// 2 Parameters

template <typename C, typename P1, typename P2, typename R>
AL::ALPtr<ALFunctorBase>
createFunctor (C *obj, R (C::*f) (const P1 &, const P2 &))
{
  return AL::ALPtr<ALFunctorBase>(new ALFunctor_2<C, P1, P2, R>(obj, f));
}


// 3 Parameters

template <typename C, typename P1, typename P2, typename P3, typename R>
AL::ALPtr<ALFunctorBase>
createFunctor (C *obj, R (C::*f) (const P1 &, const P2 & ,const  P3 &))
{
  return AL::ALPtr<ALFunctorBase>(new ALFunctor_3<C, P1, P2, P3, R>(obj, f));
}

// 4 Parameters

template <typename C, typename P1, typename P2, typename P3, typename P4, typename R>
AL::ALPtr<ALFunctorBase>
createFunctor (C *obj, R (C::*f) (const P1 &, const P2 &, const P3 &, const P4 &))
{
  return AL::ALPtr<ALFunctorBase>(new ALFunctor_4<C, P1, P2, P3, P4, R>(obj, f));
}





// Errors

// 5 Parameters

template <typename C, typename P1, typename P2, typename P3, typename P4, typename P5, typename R>
AL::ALPtr<ALFunctorBase>
createFunctor (C *obj, R (C::*f) (const P1 &, const P2 &, const P3 &, const P4 &, const P5 &))
{
  return AL::ALPtr<ALFunctorBase> (new ALFunctor_5<C, P1, P2, P3, P4, P5, R>(obj, f));
}



// 6 Parameters

template <typename C, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename R>
AL::ALPtr<ALFunctorBase>
createFunctor (C *obj, R (C::*f) ( const P1 &, const P2 &, const P3  &, const P4 &, const P5 &, const P6 &))
{
  return AL::ALPtr<ALFunctorBase>(new ALFunctor_6<C, P1, P2, P3, P4, P5, P6, R>(obj, f));
}

// 7 Parameters

template <typename C, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6,
typename P7, typename R>
AL::ALPtr<ALFunctorBase>
createFunctor (C *obj, R (C::*f) (const P1 &, const  P2 &, const P3 &, const P4 &, const P5 &, const P6 &, const P7&))
{
  std::cerr << "7 parameter function binding is not allowed! Please store your arguments in an ALValue" << std::endl;
  return NULL;
  //  return new ALFunctor7<C, P1, P2, P3, P4, P5, P6, P7, R>(obj, f);
}

// 8 Parameters

template <typename C, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6,
typename P7, typename P8,typename R>
AL::ALPtr<ALFunctorBase>
createFunctor (C *obj, R (C::*f) (P1, P2, P3, P4, P5, P6, P7, P8))
{
  std::cerr << "8 parameter function binding is not allowed! Please store your arguments in an ALValue" << std::endl;
  return NULL;
  //  return new ALFunctor8<C, P1, P2, P3, P4, P5, P6, P8, R>(obj, f);
}


#endif
