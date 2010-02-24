/**
* @author Aldebaran Robotics 
* Aldebaran Robotics (c) 2007 All Rights Reserved - This file is confidential.\n
*
* Version : $Id$
*/


#ifndef AL_PTR_H
#define AL_PTR_H

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/enable_shared_from_this.hpp> 

namespace AL
{
  // ALPtr definition
  template <class T> class ALPtr: public boost::shared_ptr<T>
  {
  public:
    template<class Y> explicit ALPtr(Y* p): boost::shared_ptr<T>(p) {}

    template <class Y> ALPtr(const boost::shared_ptr<Y>& p):
    boost::shared_ptr<T>(p) {}

    ALPtr() : boost::shared_ptr<T>(){}
  };

  // ALWeakPtr definition
  template <class T> class ALWeakPtr: public boost::weak_ptr<T>
  {
  public:
    template<class Y> ALWeakPtr(ALPtr<Y> const & p):
    boost::weak_ptr<T>(p) {}

    template<class Y> ALWeakPtr(boost::shared_ptr<Y> const & p):
    boost::weak_ptr<T>(p) {}

    template<class Y> ALWeakPtr(boost::weak_ptr<Y> const & p):
    boost::weak_ptr<T>(p) {}

    ALWeakPtr() : boost::weak_ptr<T>(){}

    ALPtr<T> lock() const {return boost::weak_ptr<T>::lock();}
  };

  // You can use getThis to have access to a smart pointer of this
  template <class T> class EnableGetThis : public boost::enable_shared_from_this<T>
  {
  public:
    inline ALPtr<T> getThis() {return boost::enable_shared_from_this<T>::shared_from_this();}
    inline ALPtr<const T> getThis() const {return boost::enable_shared_from_this<T>::shared_from_this();}
  };

  // add the typedef in your class for easier use
  template <class T> class TypeDefPtr
  {
  public:
    typedef ALPtr<T> Ptr;
    typedef ALPtr<const T> ConstPtr;
    typedef ALWeakPtr<T> WeakPtr;
    typedef ALWeakPtr<const T> WeakConstPtr;
  };

  template <class T> class AnyPtr : public EnableGetThis<T>, public TypeDefPtr<T>
  {
  };

  template<class T, class U> ALPtr<T> static_ptr_cast(ALPtr<U> const & r)
  {
    return boost::static_pointer_cast<T>(r);
  }

  template<class T, class U> ALPtr<T> const_ptr_cast(ALPtr<U> const & r)
  {
    return boost::const_pointer_cast<T>(r);
  }

  template<class T, class U> ALPtr<T> dynamic_ptr_cast(ALPtr<U> const & r)
  {
    return boost::dynamic_pointer_cast<T>(r);
  }

}
#endif
