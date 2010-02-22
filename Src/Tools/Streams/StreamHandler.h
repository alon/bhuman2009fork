#ifndef __StreamHandler_h_
#define __StreamHandler_h_

#include "Tools/Streams/InOut.h"
#include "Tools/Global.h"

#ifdef RELEASE

/**
* Must be used at the end of any streaming operator or serialize(In*,Out*) function
*/
#define STREAM_REGISTER_FINISH()

/** Macros dedicated for the use within the serialize(In*,Out*) of data types derived from Streamable function */

/**
* Must be used at the beginning of the serialize(In*,Out*) function
*/
#define STREAM_REGISTER_BEGIN()

/**
* Registeres and streams a base class
* @param base A pointer to the base class.
*/
#define STREAM_BASE( base) \
  this-> base ::serialize( in, out);

/**
* Registration and streaming of a member.
* @param member The member to be streamed.
*/
#define STREAM( member) \
   if( in){ \
      *in >> member; \
    }else{ \
      *out << member; \
    }

/**
* Registration and streaming of a member of type enum. The member is streamed as if it were an integer and no information about the enumeration is gathered.
* @param enumeration The enumeration to be registered and streamed.
*/
#define STREAM_ENUMASINT( enumeration) \
  if( in){ \
    streamEnum( *in , enumeration);\
  }else{ \
    streamEnum( *out , enumeration);\
  }

/**
* Registration and streaming of a member of type enum.
* @param enumeration The enumeration to be registered and streamed.
* @param numberOfEnumElements The number of enumerators in the type of enumeration
* @param getNameFunctionPtr A function pointer to a function taking the type of enumeration as argument and returning string representations of the enumerators.
*/
#define STREAM_ENUM( enumeration, numberOfEnumElements, getNameFunctionPtr) \
  if( in){ \
    streamEnum( *in , enumeration);\
  }else{ \
    streamEnum( *out , enumeration);\
  }

/**
* Registration and streaming of a static array member.
* @param array The static array to be streamed
*/
#define STREAM_ARRAY( array) \
  if(in) \
    streamStaticArray( *in, array, sizeof(array)); \
  else \
    streamStaticArray( *out, array, sizeof(array));

#define STREAM_ENUM_ARRAY( s, numberOfEnumElements, getNameFunctionPtr) \
  if( in){ \
    streamStaticEnumArray( *in, s, sizeof(s));\
  }else{ \
    streamStaticEnumArray( *out, s, sizeof(s));\
  }

/**
* Registration and streaming of a dynamic array member.
* @param array The dynamic array to be streamed
* @param count The number of elements of the array to be streamed.
*/
#define STREAM_DYN_ARRAY( array, count) \
  if(in) \
    streamDynamicArray( *in, array, count); \
  else \
    streamDynamicArray( *out, array, count); 

/**
* Registration and streaming of a STL vector.
* @param s The vector to be streamed
*/
#define STREAM_VECTOR( s) \
  if(in) \
    streamVector( *in, s); \
  else \
    streamVector( *out, s); 

/** Macros dedicated for the use within streaming operators */

/**
* Must be used at the beginning of the streaming operator
* @param object Object to be streamed within this streaming operator (should be the second argument to the streaming operator)
*/
#define STREAM_REGISTER_BEGIN_EXT( object)
  
/**
* Registeres and streams a member.
* @param object Object to be streamed.
*/
#define STREAM_EXT( stream, object) \
  streamObject( stream , object);

/**
* Registeres and streams a base class.
* @param object A reference to a base class representation.
*/
#define STREAM_BASE_EXT( stream, object) \
  streamObject( stream, object);

/**
* Registeres and streams a member of type enumeration as integer, loosing all information about the enumeration.
* @para stream Reference to the stream, that should be streamed to.
* @param enumeration Enumeration to be streamed.
*/
#define STREAM_ENUMASINT_EXT( stream, enumeration) \
  streamEnum( stream, enumeration);

/**
* Registeres and streams a member of type enumeration, gathering information about the enumeration.
* @para stream Reference to the stream, that should be streamed to.
* @param enumeration Enumeration to be streamed.
* @param numberOfElements The number of enumerators in the enumeration type.
* @param getNameFunctionPtr Pointer to a function returning string representations of enumerators.
*/
#define STREAM_ENUM_EXT( stream, s, numberOfEnumElements, getNameFunctionPtr) \
  streamEnum( stream , s);

/**
* Registeres and streams a static array member
* @para stream Reference to the stream, that should be streamed to.
* @param array The static array to be streamed.
*/
#define STREAM_ARRAY_EXT( stream, array) \
  streamStaticArray( stream, array, sizeof(array));

/**
* Registeres and streams a dynamic array member
* @para stream Reference to the stream, that should be streamed to.
* @param array The dynamic array to be streamed.
* @param count The number of elements in the dynamic array to be streamed
*/
#define STREAM_DYN_ARRAY_EXT( stream, s, count) \
  streamDynamicArray( stream, s, count);

#else // #ifdef RELEASE

#define STREAM_REGISTER_BEGIN_EXT( s) \
  Global::getStreamHandler().startRegistration( typeid(s).name(), true);

#define STREAM_EXT( stream, s) \
  Global::getStreamHandler().registerWithSpecification( #s, s); \
  streamObject( stream ,s);

#define STREAM_ENUMASINT_EXT( stream, s) \
  { int ____xyzstreamhandlertemp; Global::getStreamHandler().registerWithSpecification( #s, ____xyzstreamhandlertemp); }\
  streamEnum( stream , s);

#define STREAM_ENUM_EXT( stream, s, numberOfEnumElements, getNameFunctionPtr) \
  Global::getStreamHandler().registerEnumWithSpecification( #s, s, numberOfEnumElements, getNameFunctionPtr); \
  streamEnum( stream , s);

#define STREAM_BASE_EXT( stream, s) \
  Global::getStreamHandler().registerBase(); \
  streamObject( stream, s);

#define STREAM_ARRAY_EXT( stream, s) \
  Global::getStreamHandler().registerWithSpecification( #s, s); \
  streamStaticArray( stream, s, sizeof(s));

#define STREAM_DYN_ARRAY_EXT( stream, s, count) \
  Global::getStreamHandler().registerDynamicArrayWithSpecification( #s, s); \
  streamDynamicArray( stream, s, count);

#define STREAM_REGISTER_BEGIN() \
  Global::getStreamHandler().startRegistration( typeid(*this).name(), false);

#define STREAM_BASE( s) \
  Global::getStreamHandler().registerBase(); \
  this-> s ::serialize( in, out);
  
#define STREAM( s) \
  Global::getStreamHandler().registerWithSpecification( #s, s); \
    if( in){ \
      *in >> s; \
    }else{ \
      *out << s; \
    }

#define STREAM_ENUMASINT( s) \
  { int ____xyzstreamhandlertemp; Global::getStreamHandler().registerWithSpecification( #s, ____xyzstreamhandlertemp); }\
  if( in){ \
    streamEnum( *in , s);\
  }else{ \
    streamEnum( *out , s);\
  }

#define STREAM_ENUM( s, numberOfEnumElements, getNameFunctionPtr) \
  Global::getStreamHandler().registerEnumWithSpecification( #s, s, numberOfEnumElements, getNameFunctionPtr); \
  if( in){ \
    streamEnum( *in , s);\
  }else{ \
    streamEnum( *out , s);\
  }

#define STREAM_ARRAY( s) \
  Global::getStreamHandler().registerWithSpecification( #s, s); \
  if(in) \
    streamStaticArray( *in, s, sizeof(s)); \
  else \
    streamStaticArray( *out, s, sizeof(s));

#define STREAM_ENUM_ARRAY( s, numberOfEnumElements, getNameFunctionPtr) \
  Global::getStreamHandler().registerEnumArrayWithSpecification( #s, s, numberOfEnumElements, getNameFunctionPtr); \
  if( in){ \
    streamStaticEnumArray( *in, s, sizeof(s));\
  }else{ \
    streamStaticEnumArray( *out, s, sizeof(s));\
  }

#define STREAM_DYN_ARRAY( s, count) \
  Global::getStreamHandler().registerDynamicArrayWithSpecification( #s, s); \
  if(in) \
    streamDynamicArray( *in, s, count); \
  else \
    streamDynamicArray( *out, s, count); 

#define STREAM_VECTOR( s) \
  Global::getStreamHandler().registerDynamicArrayWithSpecification( #s, s); \
  if(in) \
    streamVector( *in, s); \
  else \
    streamVector( *out, s); 

#define STREAM_REGISTER_FINISH() \
  Global::getStreamHandler().finishRegistration();

#endif // #ifndef RELEASE

#include <vector>
#include <stack>
#include "Platform/hash_map.h"
USING_HASH_MAP

class StreamHandler;
  
In& operator>>(In& in, StreamHandler& streamHandler);
Out& operator<<(Out& out, const StreamHandler& streamHandler);

class ConsoleRoboCupCtrl;
class RobotConsole;

/**
* singleton stream handler class
*/
class StreamHandler
{
private:
  /**
   * Default constructor.
   * No other instance of this class is allowed except the one accessible via getStreamHandler
   * therefore the constructor is private.
   */
  StreamHandler();

  /**
   * Copy constructor.
   * Copying instances of this class is not allowed
   * therefore the copy constructor is private. */
  StreamHandler(const StreamHandler&) {}

  /*
  * only a process is allowed to create the instance.
  */
  friend class Process;

  struct RegisteringAttributes
  {
    short baseClass;
    bool registering;
    bool externalOperator;
  };

  typedef hash_map<const char*, const char*> BasicTypeSpecification;
  BasicTypeSpecification basicTypeSpecification;

  typedef std::pair< std::string, const char*> TypeNamePair;
  typedef hash_map<const char*, std::vector<TypeNamePair> > Specification;
  Specification specification;

  typedef hash_map<const char*, std::vector<const char*> > EnumSpecification;
  EnumSpecification enumSpecification;

  typedef hash_map<std::string, int> StringTable;
  StringTable stringTable;

  typedef std::pair<Specification::iterator, RegisteringAttributes> RegisteringEntry;
  typedef std::stack<RegisteringEntry> RegisteringEntryStack;
  RegisteringEntryStack registeringEntryStack;

  bool registering;
  bool registeringBase;

  const char* getString(const std::string& string);

public:
  void clear();

  void startRegistration(const char* name, bool registerWithExternalOperator);

  void registerBase() {registeringBase = true;}

  void finishRegistration();

  template<class T>
  void registerWithSpecification(const char* name, T& t)
  {
    if(registering && registeringEntryStack.top().second.registering)
    {
      std::string nameString = name;
      if(registeringEntryStack.top().second.externalOperator)
      {
        nameString.erase(0, nameString.find(".") + 1);
        registeringEntryStack.top().first->second.push_back(TypeNamePair(nameString, typeid(t).name()));
      }
      else
        registeringEntryStack.top().first->second.push_back(TypeNamePair(name, typeid(t).name()));
    }
  }

  template<class T>
  void registerDynamicArrayWithSpecification(const char* name, T& t)
  {
    if(registering && registeringEntryStack.top().second.registering)
    {
      std::string nameString = name;
      if(registeringEntryStack.top().second.externalOperator)
      {
        nameString.erase(0, nameString.find(".") + 1);
        registeringEntryStack.top().first->second.push_back(TypeNamePair(nameString, typeid(&t[0]).name()));
      }
      else
        registeringEntryStack.top().first->second.push_back(TypeNamePair(name, typeid(&t[0]).name()));
    }
  }
    
  template<class T, class E>
  void registerEnumWithSpecification(const char* name, const T& t, int numberOfEnumElements, const char* (*fp)(E))
  {
    if(registering && registeringEntryStack.top().second.registering)
    {
      std::string nameString = name;
      if(registeringEntryStack.top().second.externalOperator)
      {
        nameString.erase(0, nameString.find(".") + 1);
        registeringEntryStack.top().first->second.push_back(TypeNamePair(nameString, typeid(t).name()));
      }
      else
        registeringEntryStack.top().first->second.push_back(TypeNamePair(name, typeid(t).name()));

      // register enum if necessary
      if(enumSpecification.find(typeid(t).name()) == enumSpecification.end())
      {
        enumSpecification[typeid(t).name()];
        for(int i = 0; i < numberOfEnumElements; ++i)
          enumSpecification[typeid(t).name()].push_back((*fp)((E) i));
      }
    }
  }
    
  template<class T, class E>
  void registerEnumArrayWithSpecification(const char* name, const T& t, int numberOfEnumElements, const char* (*fp)(E))
  {
    if(registering && registeringEntryStack.top().second.registering)
    {
      std::string nameString = name;
      if(registeringEntryStack.top().second.externalOperator)
      {
        nameString.erase(0, nameString.find(".") + 1);
        registeringEntryStack.top().first->second.push_back(TypeNamePair(nameString, typeid(t).name()));
      }
      else
        registeringEntryStack.top().first->second.push_back(TypeNamePair(name, typeid(t).name()));

      // register enum if necessary
      if(enumSpecification.find(typeid(t[0]).name()) == enumSpecification.end())
      {
        enumSpecification[typeid(t[0]).name()];
        for(int i = 0; i < numberOfEnumElements; ++i)
          enumSpecification[typeid(t[0]).name()].push_back((*fp)((E) i));
      }
    }
  }
    
  template<class T, class E>
  void registerEnum(const T& t, int numberOfEnumElements, const char* (*fp)(E))
  {
    if(enumSpecification.find(typeid(t).name()) == enumSpecification.end())
    {
      enumSpecification[typeid(t).name()];
      for( int i = 0; i < numberOfEnumElements; ++i)
        enumSpecification[typeid(t).name()].push_back((*fp)((E) i));
    }
  }

  static std::string demangle(const char* name);

  friend In& operator>>(In&, StreamHandler&);
  friend Out& operator<<(Out&, const StreamHandler&);
  friend class ConsoleRoboCupCtrl;
  friend class RobotConsole;
};

template<class T>
In& streamComplexStaticArray(In& in, T inArray[], int size)
{
  int numberOfEntries = size / sizeof(T);
  for(int i = 0; i < numberOfEntries; ++i)
    in >> inArray[i];
  return in;
}

template<class T>
Out& streamComplexStaticArray(Out& out, T outArray[], int size)
{
  int numberOfEntries = size / sizeof(T);
  for(int i = 0; i < numberOfEntries; ++i)
    out << outArray[i];
  return out;
}

template<class T>
In& streamBasicStaticArray(In& in, T inArray[], int size)
{
  if(in.isBinary())
  {
    in.read(inArray, size);
    return in;
  }
  else
    return streamComplexStaticArray(in, inArray, size);
}

template<class T>
Out& streamBasicStaticArray(Out& out, T outArray[], int size)
{
  if(out.isBinary())
  {
    out.write(outArray, size);
    return out;
  }
  else
    return streamComplexStaticArray(out, outArray, size);
}

template<class T>
In& streamStaticEnumArray(In& in, T inArray[], int size)
{
  if(in.isBinary())
  {
    in.read(inArray, size);
    return in;
  }
  else
  {
    int numberOfEntries = size / sizeof(T);
    for(int i = 0; i < numberOfEntries; ++i)
    {
      int j;
      in >> j;
      inArray[i] = (T) j;
    }
    return in;
  }
}

template<class T>
Out& streamStaticEnumArray(Out& out, T outArray[], int size)
{
  if(out.isBinary())
  {
    out.write(outArray, size);
    return out;
  }
  else
  {
    int numberOfEntries = size / sizeof(T);
    for(int i = 0; i < numberOfEntries; ++i)
    {
      int j = (int) outArray[i];
      out << j;
    }
    return out;
  }
}

inline In& streamStaticArray(In& in, unsigned char inArray[], int size) {return streamBasicStaticArray(in, inArray, size);}
inline Out& streamStaticArray(Out& out, unsigned char outArray[], int size) {return streamBasicStaticArray(out, outArray, size);}
inline In& streamStaticArray(In& in, char inArray[], int size) {return streamBasicStaticArray(in, inArray, size);}
inline Out& streamStaticArray(Out& out, char outArray[], int size) {return streamBasicStaticArray(out, outArray, size);}
inline In& streamStaticArray(In& in, unsigned short inArray[], int size) {return streamBasicStaticArray(in, inArray, size);}
inline Out& streamStaticArray(Out& out, unsigned short outArray[], int size) {return streamBasicStaticArray(out, outArray, size);}
inline In& streamStaticArray(In& in, short inArray[], int size) {return streamBasicStaticArray(in, inArray, size);}
inline Out& streamStaticArray(Out& out, short outArray[], int size) {return streamBasicStaticArray(out, outArray, size);}
inline In& streamStaticArray(In& in, unsigned int inArray[], int size) {return streamBasicStaticArray(in, inArray, size);}
inline Out& streamStaticArray(Out& out, unsigned int outArray[], int size) {return streamBasicStaticArray(out, outArray, size);}
inline In& streamStaticArray(In& in, int inArray[], int size) {return streamBasicStaticArray(in, inArray, size);}
inline Out& streamStaticArray(Out& out, int outArray[], int size) {return streamBasicStaticArray(out, outArray, size);}
inline In& streamStaticArray(In& in, unsigned long inArray[], int size) {return streamBasicStaticArray(in, inArray, size);}
inline Out& streamStaticArray(Out& out, unsigned long outArray[], int size) {return streamBasicStaticArray(out, outArray, size);}
inline In& streamStaticArray(In& in, long inArray[], int size) {return streamBasicStaticArray(in, inArray, size);}
inline Out& streamStaticArray(Out& out, long outArray[], int size) {return streamBasicStaticArray(out, outArray, size);}
inline In& streamStaticArray(In& in, float inArray[], int size) {return streamBasicStaticArray(in, inArray, size);}
inline Out& streamStaticArray(Out& out, float outArray[], int size) {return streamBasicStaticArray(out, outArray, size);}
inline In& streamStaticArray(In& in, double inArray[], int size) {return streamBasicStaticArray(in, inArray, size);}
inline Out& streamStaticArray(Out& out, double outArray[], int size) {return streamBasicStaticArray(out, outArray, size);}
template<class T> 
In& streamStaticArray(In& in, T inArray[], int size) {return streamComplexStaticArray(in, inArray, size);}
template<class T>
Out& streamStaticArray(Out& out, T outArray[], int size) {return streamComplexStaticArray(out, outArray, size);}

template<class T>
In& streamComplexDynamicArray(In& in, T& inArray, int& numberOfEntries)
{
  in >> numberOfEntries;
  for(int i = 0; i < numberOfEntries; ++i)
    in >> inArray[i];
  return in;
}

template<class T>
Out& streamComplexDynamicArray(Out& out, const T& outArray, int numberOfEntries)
{
  out << numberOfEntries;
  for(int i = 0; i < numberOfEntries; ++i)
    out << outArray[i];
  return out;
}

template<class T>
In& streamBasicDynamicArray(In& in, T* inArray, int& numberOfEntries)
{
  if(in.isBinary())
  {
    in >> numberOfEntries;
    in.read(inArray, numberOfEntries * sizeof(T));
    return in;
  }
  else
    return streamComplexDynamicArray(in, inArray, numberOfEntries);
}

template<class T>
Out& streamBasicDynamicArray(Out& out, T* outArray, int numberOfEntries)
{
  if(out.isBinary())
  {
    out << numberOfEntries;
    out.write(outArray, numberOfEntries * sizeof(T));
    return out;
  }
  else
    return streamComplexDynamicArray(out, outArray, numberOfEntries);
}

inline In& streamDynamicArray(In& in, unsigned char inArray[], int& numberOfEntries) {return streamBasicDynamicArray(in, inArray, numberOfEntries);}
inline Out& streamDynamicArray(Out& out, unsigned char outArray[], int numberOfEntries) {return streamBasicDynamicArray(out, outArray, numberOfEntries);}
inline In& streamDynamicArray(In& in, char inArray[], int& numberOfEntries) {return streamBasicDynamicArray(in, inArray, numberOfEntries);}
inline Out& streamDynamicArray(Out& out, char outArray[], int numberOfEntries) {return streamBasicDynamicArray(out, outArray, numberOfEntries);}
inline In& streamDynamicArray(In& in, unsigned short inArray[], int& numberOfEntries) {return streamBasicDynamicArray(in, inArray, numberOfEntries);}
inline Out& streamDynamicArray(Out& out, unsigned short outArray[], int numberOfEntries) {return streamBasicDynamicArray(out, outArray, numberOfEntries);}
inline In& streamDynamicArray(In& in, short inArray[], int& numberOfEntries) {return streamBasicDynamicArray(in, inArray, numberOfEntries);}
inline Out& streamDynamicArray(Out& out, short outArray[], int numberOfEntries) {return streamBasicDynamicArray(out, outArray, numberOfEntries);}
inline In& streamDynamicArray(In& in, unsigned int inArray[], int& numberOfEntries) {return streamBasicDynamicArray(in, inArray, numberOfEntries);}
inline Out& streamDynamicArray(Out& out, unsigned int outArray[], int numberOfEntries) {return streamBasicDynamicArray(out, outArray, numberOfEntries);}
inline In& streamDynamicArray(In& in, int inArray[], int& numberOfEntries) {return streamBasicDynamicArray(in, inArray, numberOfEntries);}
inline Out& streamDynamicArray(Out& out, int outArray[], int numberOfEntries) {return streamBasicDynamicArray(out, outArray, numberOfEntries);}
inline In& streamDynamicArray(In& in, unsigned long inArray[], int& numberOfEntries) {return streamBasicDynamicArray(in, inArray, numberOfEntries);}
inline Out& streamDynamicArray(Out& out, unsigned long outArray[], int numberOfEntries) {return streamBasicDynamicArray(out, outArray, numberOfEntries);}
inline In& streamDynamicArray(In& in, long inArray[], int& numberOfEntries) {return streamBasicDynamicArray(in, inArray, numberOfEntries);}
inline Out& streamDynamicArray(Out& out, long outArray[], int numberOfEntries) {return streamBasicDynamicArray(out, outArray, numberOfEntries);}
inline In& streamDynamicArray(In& in, float inArray[], int& numberOfEntries) {return streamBasicDynamicArray(in, inArray, numberOfEntries);}
inline Out& streamDynamicArray(Out& out, float outArray[], int numberOfEntries) {return streamBasicDynamicArray(out, outArray, numberOfEntries);}
inline In& streamDynamicArray(In& in, double inArray[], int& numberOfEntries) {return streamBasicDynamicArray(in, inArray, numberOfEntries);}
inline Out& streamDynamicArray(Out& out, double outArray[], int numberOfEntries) {return streamBasicDynamicArray(out, outArray, numberOfEntries);}
template<class T>
In& streamDynamicArray(In& in, T& inArray, int& numberOfEntries) {return streamComplexDynamicArray(in, inArray, numberOfEntries);}
template<class T>
Out& streamDynamicArray(Out& out, const T& outArray, int numberOfEntries) {return streamComplexDynamicArray(out, outArray, numberOfEntries);}

template<class T>
In& streamComplexVector(In& in, std::vector<T>& inVector)
{
  unsigned numberOfEntries;
  in >> numberOfEntries;
  inVector.resize(numberOfEntries);
  for(unsigned i = 0; i < numberOfEntries; ++i)
    in >> inVector[i];
  return in;
}

template<class T>
Out& streamComplexVector(Out& out, std::vector<T>& outVector)
{
  out << outVector.size();
  for(unsigned i = 0; i < outVector.size(); ++i)
    out << outVector[i];
  return out;
}

template<class T> In& streamBasicVector(In& in, std::vector<T>& inVector)
{
  if(in.isBinary())
  {
    unsigned numberOfEntries;
    in >> numberOfEntries;
    inVector.resize(numberOfEntries);
    if(numberOfEntries)
      in.read(&inVector[0], numberOfEntries * sizeof(T));
    return in;
  }
  else
    return streamComplexVector(in, inVector);
}

template<class T> Out& streamBasicVector(Out& out, std::vector<T>& outVector)
{
  if(out.isBinary())
  {
    out << outVector.size();
    if(outVector.size())
      out.write(&outVector[0], outVector.size() * sizeof(T));
    return out;
  }
  else
    return streamComplexVector(out, outVector);
}

inline In& streamVector(In& in, std::vector<unsigned char>& inVector) {return streamBasicVector(in, inVector);}
inline Out& streamVector(Out& out, std::vector<unsigned char>& outVector) {return streamBasicVector(out, outVector);}
inline In& streamVector(In& in, std::vector<char>& inVector) {return streamBasicVector(in, inVector);}
inline Out& streamVector(Out& out, std::vector<char>& outVector) {return streamBasicVector(out, outVector);}
inline In& streamVector(In& in, std::vector<unsigned short>& inVector) {return streamBasicVector(in, inVector);}
inline Out& streamVector(Out& out, std::vector<unsigned short>& outVector) {return streamBasicVector(out, outVector);}
inline In& streamVector(In& in, std::vector<short>& inVector) {return streamBasicVector(in, inVector);}
inline Out& streamVector(Out& out, std::vector<short>& outVector) {return streamBasicVector(out, outVector);}
inline In& streamVector(In& in, std::vector<unsigned int>& inVector) {return streamBasicVector(in, inVector);}
inline Out& streamVector(Out& out, std::vector<unsigned int>& outVector) {return streamBasicVector(out, outVector);}
inline In& streamVector(In& in, std::vector<int>& inVector) {return streamBasicVector(in, inVector);}
inline Out& streamVector(Out& out, std::vector<int>& outVector) {return streamBasicVector(out, outVector);}
inline In& streamVector(In& in, std::vector<unsigned long>& inVector) {return streamBasicVector(in, inVector);}
inline Out& streamVector(Out& out, std::vector<unsigned long>& outVector) {return streamBasicVector(out, outVector);}
inline In& streamVector(In& in, std::vector<long>& inVector) {return streamBasicVector(in, inVector);}
inline Out& streamVector(Out& out, std::vector<long>& outVector) {return streamBasicVector(out, outVector);}
inline In& streamVector(In& in, std::vector<float>& inVector) {return streamBasicVector(in, inVector);}
inline Out& streamVector(Out& out, std::vector<float>& outVector) {return streamBasicVector(out, outVector);}
inline In& streamVector(In& in, std::vector<double>& inVector) {return streamBasicVector(in, inVector);}
inline Out& streamVector(Out& out, std::vector<double>& outVector) {return streamBasicVector(out, outVector);}
template<class T>
In& streamVector(In& in, std::vector<T>& inVector) {return streamComplexVector(in, inVector);}
template<class T>
Out& streamVector(Out& out, std::vector<T>& outVector) {return streamComplexVector(out, outVector);}

template<class T>
void streamObject(In& in, T& t)
{
  in >> t;
}

template<class T>
void streamObject(Out& out, T& t)
{
  out << t;
}

template<class T>
void streamEnum(In& in, T& t)
{
  int i;
  in >> i;
  t = (T) i;
}

template<class T>
void streamEnum(Out& out, T& t)
{
  int i = (int) t;
  out << i;
}

#endif // __StreamHandler_h_
