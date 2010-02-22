/**
* @file Tools/Debugging/Debugging.h
*
* Macros and functions for debugging
* 
* @author Martin Lötzsch
*/ 
#ifndef __Debugging_h_
#define __Debugging_h_

#include "Tools/MessageQueue/OutMessage.h"
#include "Tools/Debugging/DebugRequest.h"
#include "Tools/Global.h"

#ifdef RELEASE

#define EXECUTE_ONLY_IN_DEBUG(...) ((void) 0)
#define OUTPUT(type,format,expression) ((void) 0)
#define DEBUG_RESPONSE(id, ...) ((void) 0)
#define DEBUG_RESPONSE_OR_RELEASE(id, ...) { __VA_ARGS__ }
#define DEBUG_RESPONSE_NOT(id, ...) { __VA_ARGS__ }
#define NOT_POLLABLE_DEBUG_RESPONSE(id, ...) ((void) 0)

#else // RELEASE

/**
* A macro which executes an expression only if in debug mode.
*/
#define EXECUTE_ONLY_IN_DEBUG(...) \
  { __VA_ARGS__ }

/**
* A macro for sending debug messages.
*
* @param type The type of the message from the MessageID enum in MessageIDs.h
* @param format The message format of the message (bin or text)
* @param expression A streamable expression
*
* Examples:
* <pre>
* OUTPUT(idImage, bin, *pMyImage);
* OUTPUT(idText, text, "MyObject:myFunction() invoked");
* OUTPUT(idText, text, "i: " << i << ", j:" << j);
* </pre>
*/

#ifdef TARGET_ROBOT
#include <iostream>
#define OUTPUT(type, format, expression) \
  if(true) \
  { \
    Global::getDebugOut().format << expression; \
    Global::getDebugOut().finishMessage(type); \
    copyToCout__##format (expression) \
  } \
  else \
    ((void) 0)
#define copyToCout__text(expression) std::cout << expression << std::endl;
#define copyToCout__bin(expression)
#else
#define OUTPUT(type,format,expression) \
  if(true) \
  { \
    Global::getDebugOut().format << expression; \
    Global::getDebugOut().finishMessage(type); \
  } \
  else \
    ((void) 0)
#endif

/**
* A debugging switch, allowing the enabling or disabling of expressions.
* @param id The id of the debugging switch
* @param ... The expression to be executed if id is enabled
*/
#define DEBUG_RESPONSE(id, ...) \
  if(true) \
  {\
    if(Global::getDebugRequestTable().poll && Global::getDebugRequestTable().notYetPolled(id)) \
    { \
      OUTPUT(idDebugResponse, text, id << \
             int(Global::getDebugRequestTable().isActive(id) && !Global::getDebugRequestTable().once(id))); \
    } \
    if(Global::getDebugRequestTable().isActive(id)) \
    { \
      if(Global::getDebugRequestTable().once(id)) \
        Global::getDebugRequestTable().disable(id); \
      __VA_ARGS__ \
    } \
  } \
  else \
    ((void) 0)

/**
* A debugging switch, allowing the enabling or disabling of expressions.
* @param id The id of the debugging switch
* @param ... The expression to be executed if id is enabled
*/
#define DEBUG_RESPONSE_OR_RELEASE(id, ...) \
  if(true) \
  {\
    if(Global::getDebugRequestTable().poll && Global::getDebugRequestTable().notYetPolled(id)) \
    { \
      OUTPUT(idDebugResponse, text, id << \
             int(Global::getDebugRequestTable().isActive(id) && !Global::getDebugRequestTable().once(id))); \
    } \
    if(Global::getDebugRequestTable().isActive(id)) \
    { \
      if(Global::getDebugRequestTable().once(id)) \
        Global::getDebugRequestTable().disable(id); \
      __VA_ARGS__ \
    } \
  } \
  else \
    ((void) 0)

/**
* A debugging switch, allowing the enabling or disabling of expressions.
* @param id The id of the debugging switch
* @param ... The expression to be executed if id is disabled or software is compiled in release mode
*/
#define DEBUG_RESPONSE_NOT(id, ...) \
  if(true) \
  {\
    if(Global::getDebugRequestTable().poll && Global::getDebugRequestTable().notYetPolled(id)) \
    { \
      OUTPUT(idDebugResponse, text, id << \
             int(Global::getDebugRequestTable().isActive(id) && !Global::getDebugRequestTable().once(id))); \
    } \
    if(! (Global::getDebugRequestTable().isActive(id))) \
    { \
      if(Global::getDebugRequestTable().once(id)) \
        Global::getDebugRequestTable().disable(id); \
      __VA_ARGS__ \
    } \
  } \
  else \
    ((void) 0)

/** A debugging switch not to be polled */
#define NOT_POLLABLE_DEBUG_RESPONSE(id, ...) \
  if(true) \
  {\
    if(Global::getDebugRequestTable().isActive(id)) \
    { \
      if(Global::getDebugRequestTable().once(id)) \
        Global::getDebugRequestTable().disable(id); \
      __VA_ARGS__ \
    } \
  } \
  else \
    ((void) 0)

#endif // RELEASE

#endif //__Debugging_h_
