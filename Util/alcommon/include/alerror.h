/**
* @author Aldebaran Robotics 
* Aldebaran Robotics (c) 2007 All Rights Reserved - This file is confidential.\n
*
* Version : $Id$
*/

#ifndef AL_ERROR_H
#define AL_ERROR_H

#include "altypes.h" // for Verbosity
#include "altools.h" // for DecToString


#ifndef FINAL_RELEASE
# define ALERROR(module,methode,description) AL::ALError(module,methode,description,__FILE__,__LINE__)
#else
# define ALERROR(module,methode,description) AL::ALError(module,methode,description,"",__LINE__)
#endif

#define ALERROR_PRINT_CATCHED(err) printf( "WARNING: Catching this error: '%s'\n", err.toString().c_str() )

// Help out windows:
#if defined( _DEBUG ) && !defined( DEBUG )
#define DEBUG
#endif

// moving AL_ASSERT from altools because of use of ALERROR
#ifdef DEBUG
# define AL_ASSERT(b) { if( !(b) ) { throw ALERROR( "ALAssert", "Assert failed", std::string( __FILE__ ) + ":" + DecToString( __LINE__ ) ); } }
#else
# define AL_ASSERT(b) 
#endif

#define AL_ERROR_EXIT_OK                  0
#define AL_ERROR_EXIT_CODE_NETWORK_BUSY -15

namespace AL
{

  /**
  * class ALError is used to send exception.\n
  * 
  */
  class ALError
  {
  public:

    /**
    * ALError constructor.
    * @param pModuleName, module's name that create error
    * @param pMethod, method's name where error occured
    * @param pDescription, error's descripton
    * @param pszFilename, source code file name 
    * @param pnNumLine, source code line number
    */
    ALError (
      std::string pModuleName,
      std::string pMethod,
      std::string pDescription,
      const char* pszFilename = NULL,
      const int pnNumLine = 0
      );



    /**
    * ALError default constructor.
    */
    ALError();

    /**
    * Return a string describing this object, for debug/log purpose.
    * @param pnVerbosity
    * @return a string describing this object, for debug/log purpose.
    */
    std::string toString( Verbosity pnVerbosity = VerbosityMedium ) const;

    /**
    * module's name where error occured
    */
    std::string fModuleName;

    /**
    * method's name where error occured
    */
    std::string fMethod;

    /**
    * error's description
    */
    std::string fDescription;

    /**
    * file's name where the error occured
    */
    std::string fszFilename;

    /**
    * code source line number
    */
    int					fnNumLine;
  }; // class ALError


} // namespace AL

#define AL_CATCH_ERR(commandToExecuteAfterPrintingError)        \
  catch( ALError & err )                                        \
{                                                             \
  std::string strErrorMessage = err.toString() + " from " + __FILE__ + ":" + DecToString( __LINE__ );          \
  printf( "WARNING: Exception caught: %s\n", strErrorMessage.c_str() );                                        \
  commandToExecuteAfterPrintingError;                         \
}                                                             \



#endif
