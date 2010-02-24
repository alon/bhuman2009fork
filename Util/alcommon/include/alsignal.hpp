/*
** alsignal.hpp
** Login : <ctaf@localhost.localdomain>
** Started on  Tue Jan 27 11:26:24 2009 Cedric GESTES
** $Id$
**
** Author(s):
**  - Cedric GESTES <gestes@aldebaran-robotics.com>
**
** Copyright (C) 2009 Aldebaran Robotics
*/

#ifndef   	ALSIGNAL_HPP_
# define   	ALSIGNAL_HPP_

# include <boost/signal.hpp>
# include <boost/bind.hpp>

namespace AL
{
  // we put it in another namespace, so people writing using namespace AL;
  // will not have any conflict with the signal function on Windows
  namespace functional
  {

    namespace signals
    {
      using namespace ::boost::signals;
    }

    //include boost bind into our namespace
    using ::boost::bind;

    using ::boost::signal;
//     using ::boost::signals::connection;
//     using ::boost::signals::connect_position;
  }
};

#endif	    /* !ALSIGNAL_HPP_ */
