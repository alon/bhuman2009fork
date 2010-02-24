/**
* @author Aldebaran Robotics 
* Aldebaran Robotics (c) 2007 All Rights Reserved - This file is confidential.\n
*
* Version : $Id$
*/


#ifndef ENQUEUE_REQUEST_H
#define ENQUEUE_REQUEST_H
#include "altask.h"
/// class enqueue_request - 
namespace AL
{

class enqueue_request : public AL::ALTask {
  // Attributes
private:
   int fIndex;
  // Operations
public:
  enqueue_request (int pIndex);
  ~enqueue_request ();
  void run ();
};

}

#endif

