/**
* @author Aldebaran Robotics 
* Aldebaran Robotics (c) 2007 All Rights Reserved - This file is confidential.\n
*
* Version : $Id$
*/


#ifndef AL_TIMEVAL_HH
#define AL_TIMEVAL_HH

#ifdef _WIN32
  #include <sys/timeb.h>
  #include <sys/types.h>
  #include <winsock.h> // for timeval
//  #include "alh.h"
  int gettimeofday(struct timeval* t,void* timezone);
#else
  #include <sys/time.h>
#endif
#include <ostream>

/*
 * ALTimeval.h and ALTimeval.cpp contain a set of utility functions that
 * manipulate timestamps easily.
 * This is mostly about operator overloading and basic functions.
 * ALTimeval should be cross-platform, and implements gettimeofday on
 * Windows.
 */

// reset time to 0,0
inline void reset( struct timeval & t )
{
  t.tv_sec = 0;
  t.tv_usec = 0;
}

// is time set to (0,0) ?
inline bool isNull( struct timeval & t )
{
  return t.tv_sec == 0 && t.tv_usec == 0;
}


void AddMicroSec( struct timeval & t, unsigned int pnTimeInMicroSec );

struct timeval operator+( const struct timeval & t1, const struct timeval & t2 );
struct timeval operator+( const struct timeval & t1, long t2 ); // WARNING: t2 is in second !!!!!
struct timeval operator-( const struct timeval & t1, const struct timeval & t2 );
struct timeval operator-( const struct timeval & t1, long t2 ); // WARNING: t2 is in second !!!!!
bool operator==( const struct timeval & t1, const struct timeval & t2 );
bool operator< ( const struct timeval & t1, const struct timeval & t2 );
bool operator> ( const struct timeval & t1, const struct timeval & t2 );
bool operator<=( const struct timeval & t1, const struct timeval & t2 );
bool operator>=( const struct timeval & t1, const struct timeval & t2 );

struct timeval now( void );
struct timeval epoch( void );
void printtv( const struct timeval & t );
std::ostream &operator<<( std::ostream& os, const struct timeval & st );

std::string toString( const struct timeval & t );

typedef struct timeval ALTimeval;



/**
 * \brief give duration time between two dates (in micro seconds)
 *
 * Take 2 dates and return the duration between then is us.
 *
 * @param begin starting date
 * @param end ending date
 *
 * @return duration between the two dates
 */

int duration( const struct timeval & begin, const struct timeval & end );

#endif // AL_TIMEVAL_HH
