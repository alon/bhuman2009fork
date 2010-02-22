/*
* @file Tools/Debugging/Modify.h
* Macros make data visible/editable through RobotControl.
*
* @author Michael Spranger
* @author Tobias Oberlies
*/


#include "DebugDataTable.h"

/**
 * == Definition of the idDebugDataResponse message format ==
 *
 * The overall syntax is
 *   [identifier string] [flags] [data]
 * where flags is a 32 bit integer specifying the traits of the message (see also 
 * constants defined below):
 *  - bit 0 set means the debug data can't be modified
 *  - bit 1 set means the message contains a compound/list of objects
 * 
 * If the message contains just a single object (bit 1 must be unset), [data] consists of
 *   [class name] [object]
 * If the message contains several objects (bit 1 must be set), [data] consists of
 *   ( [part identifyer string] [class name string] [object] )+
 * 
 * The [part identifyer string] may be arbitrarily chosen, for example to distinguish two 
 * objects of the same type. If the name is left empty (char* array of lenght 0), it 
 * should default to the class name with an initial lowercase letter.
 */

#define DEBUG_DATA_MODIFYABLE 0
#define DEBUG_DATA_READ_ONLY 1
#define DEBUG_DATA_SINGLE_OBJECT 0
#define DEBUG_DATA_COMPOUND 2


#ifdef RELEASE
#define MODIFY( name, object) ((void) 0)
#define MODIFY_ENUM( name, object, numberOfEnumElements, getNameFunctionPtr) ((void) 0)
#define SEND( name, object) ((void) 0)
#define SEND_COMPOUND( id, parts) ((void) 0)
#define SEND_PART( object) ((void) 0)
#define SEND_NAMED_PART( name, object) ((void) 0)

#else

/**
 * Allows for the modification of an object implementing a complete streaming operator 
 * with macros from Tools/Streams/StreamHandler.h (or scalar).
 * @param id An identifier string of the object. Separate hirarchy levels by a single 
 *     colon ':', i.e. "cognition:ball percept". The value must be a string constant. 
 * @param object The object to be modified.
 */
#define MODIFY( id, object) \
  if(true) \
  { \
    DebugDataTable::testForStreamable(object); \
    Global::getDebugDataTable().updateObject( id, object); \
    DEBUG_RESPONSE( "debug data:"id, \
      OUTPUT( idDebugDataResponse, bin, id << (long)(DEBUG_DATA_MODIFYABLE+DEBUG_DATA_SINGLE_OBJECT) << StreamHandler::demangle(typeid(object).name()) << object); \
    ); \
  } \
  else \
    ((void) 0)

/**
 * Allows for the modification of an enumerated value.
 * @param id An identifier string of the variable.
 * @param object The variable to be modified.
 * @param numberOfEnumElements The number of elements of the enumeration type.
 * @param getNameFunctionPtr A function that returns a string for each enumeration value.
 */
#define MODIFY_ENUM( id, object, numberOfEnumElements, getNameFunctionPtr) \
  if(true) \
  { \
    DebugDataTable::testForStreamable(object); \
    Global::getStreamHandler().registerEnum( object, numberOfEnumElements, getNameFunctionPtr); \
    Global::getDebugDataTable().updateEnum( id, object); \
    DEBUG_RESPONSE( "debug data:"id, \
      OUTPUT( idDebugDataResponse, bin, id << (long)(DEBUG_DATA_MODIFYABLE+DEBUG_DATA_SINGLE_OBJECT) << StreamHandler::demangle(typeid(object).name()) << (int)object); \
    ); \
  } \
  else \
    ((void) 0)

#endif
