/**
* @author Aldebaran Robotics 
* Aldebaran Robotics (c) 2007 All Rights Reserved - This file is confidential.\n
*
* Version : $Id$
*/

#ifndef ALSINGLETON_H
#define ALSINGLETON_H

/////////////////////////////////////////////////////////////////////////////
//
// Singleton 
//
/////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include "alptr.h"

//#pragma comment(linker, "/SECTION:.section,rws") 

namespace AL
{
  template <typename T>
  class ALSingleton
  {
  protected:

    ALSingleton () {/* _singleton=NULL;*/ };
    ~ALSingleton () { };

  public:
    static ALPtr<T> getInstance ()
    {
      if (NULL == _singleton)
      {
        
        _singleton = ALPtr<T>(new T);
       
      }
      else
      {
      }
      return _singleton;
      //return (static_cast<T*> (_singleton));
    };

    static ALPtr<T> setInstance (ALPtr<T>  pSingleton)
    {
      if (pSingleton != _singleton)
      {
        kill();
        _singleton = pSingleton;
      }
      return _singleton;
//      return (static_cast<T*> (_singleton));
    };

    static void kill ()
    {
      if (NULL != _singleton)
      {
         _singleton = ALPtr<T>();
        //_singleton = NULL;
      }
    };

  private:
  //  #pragma data_seg (".section")
    static ALPtr<T> _singleton ;
    
   // #pragma data_seg()
  };


  template <typename T>   
  ALPtr<T> ALSingleton<T>::_singleton= ALPtr<T>();
   
}

#endif
