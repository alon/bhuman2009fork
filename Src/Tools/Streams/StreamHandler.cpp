
#include "Tools/Streams/StreamHandler.h"
#include <string>
#ifndef _WIN32
#include <cxxabi.h>
#endif
#include "Platform/GTAssert.h"

StreamHandler::StreamHandler()
: registering(false),
  registeringBase(false)
{
  basicTypeSpecification[typeid(double).name()];
  basicTypeSpecification[typeid(bool).name()];
  basicTypeSpecification[typeid(float).name()];
  basicTypeSpecification[typeid(short).name()];
  basicTypeSpecification[typeid(unsigned short).name()];
  basicTypeSpecification[typeid(int).name()];
  basicTypeSpecification[typeid(unsigned int).name()];
  basicTypeSpecification[typeid(long).name()];
  basicTypeSpecification[typeid(unsigned long).name()];
  basicTypeSpecification[typeid(char).name()];
  basicTypeSpecification[typeid(unsigned char).name()];
  basicTypeSpecification[typeid(std::string).name()];
}

void StreamHandler::clear()
{
  basicTypeSpecification.clear();
  specification.clear();
  enumSpecification.clear();
  stringTable.clear();
}

void StreamHandler::startRegistration(const char* name, bool registerWithExternalOperator)
{ 
  if(registeringBase)
  {
    ++registeringEntryStack.top().second.baseClass;
    registeringBase = false;
  }
  else
  {
    Specification::iterator registeringEntry = specification.find(name);
    if(registeringEntry == specification.end())
    {
      specification[name];
      RegisteringAttributes attr;
      attr.registering = true;
      attr.baseClass = 0;
      attr.externalOperator = registerWithExternalOperator;
      registeringEntryStack.push(RegisteringEntry(specification.find(name), attr));
      registering = true;
    }
    else
    {
      RegisteringAttributes attr;
      attr.registering = false;
      attr.baseClass = 0;
      attr.externalOperator = registerWithExternalOperator;
      registeringEntryStack.push(RegisteringEntry(registeringEntry, attr));
      registering = false;
    }
  }
}

void StreamHandler::finishRegistration()
{ 
  if(registeringEntryStack.size() > 0)
  {
    if(!registeringEntryStack.top().second.baseClass)
    {
      registeringEntryStack.pop();
    }
    else
    {
      --registeringEntryStack.top().second.baseClass;
    }
  }
  if(registeringEntryStack.size() <= 0)
  {
    registering = false;
  }
  else
  {
    registering = registeringEntryStack.top().second.registering;
  }
}

std::string StreamHandler::demangle(const char* name)
{
#ifdef _WIN32
  return name;
#else
  char realName[1000]; // This should be big enough, so realloc is never called.
  int status;
  size_t length = sizeof(realName);
  abi::__cxa_demangle(name, realName, &length, &status);
  ASSERT(!status);
  return realName;
#endif
}

Out& operator<<(Out& out, const StreamHandler& streamHandler)
{
  // basic types
  out << streamHandler.basicTypeSpecification.size();;
  for(StreamHandler::BasicTypeSpecification::const_iterator basicTypeIter = streamHandler.basicTypeSpecification.begin();
      basicTypeIter != streamHandler.basicTypeSpecification.end(); ++basicTypeIter)
      out << StreamHandler::demangle(basicTypeIter->first);

  // specification
  out << streamHandler.specification.size();;
  for(StreamHandler::Specification::const_iterator specificationIter = streamHandler.specification.begin();
      specificationIter != streamHandler.specification.end(); ++specificationIter)
  {
    out << StreamHandler::demangle(specificationIter->first) << specificationIter->second.size();
    for(std::vector<StreamHandler::TypeNamePair>::const_iterator typeNamePairIter = specificationIter->second.begin();
        typeNamePairIter != specificationIter->second.end(); ++typeNamePairIter)
      out << typeNamePairIter->first.c_str() << StreamHandler::demangle(typeNamePairIter->second);
  }

  // enum specification
  out << streamHandler.enumSpecification.size();;
  for(StreamHandler::EnumSpecification::const_iterator enumSpecificationIter = streamHandler.enumSpecification.begin();
      enumSpecificationIter != streamHandler.enumSpecification.end(); ++enumSpecificationIter)
  {
    out << StreamHandler::demangle(enumSpecificationIter->first) << enumSpecificationIter->second.size();
    for(std::vector<const char*>::const_iterator enumElementsIter = enumSpecificationIter->second.begin();
        enumElementsIter != enumSpecificationIter->second.end(); ++enumElementsIter)
      out << *enumElementsIter;
  }
  
  return out;
}

In& operator>>(In& in, StreamHandler& streamHandler)
{
  // note: tables are not cleared, so all data read is appended!
  // However, clear() has to be called once before the first use of this operator

  std::string first,
              second;

  // basic types
  int size;
  in >> size;
  while(size-- > 0)
  {
    in >> first;
    streamHandler.basicTypeSpecification[streamHandler.getString(first)];
  }
  
  // specification
  in >> size;
  while(size-- > 0)
  {
    int size2;
    in >> first >> size2;
    const char* f = streamHandler.getString(first);
    std::vector<StreamHandler::TypeNamePair> v;
    v.reserve(size2);
      
    while(size2-- > 0)
    {
      in >> first >> second;
      v.push_back(StreamHandler::TypeNamePair(first, streamHandler.getString(second)));
    }
    
    streamHandler.specification[f] = v;
  }

  // enum specification
  in >> size;
  while(size-- > 0)
  {
    int size2;
    in >> first >> size2;
    std::vector<const char*> v;
    v.reserve(size2);
      
    while(size2-- > 0)
    {
      in >> second;
      v.push_back(streamHandler.getString(second));
    }

    streamHandler.enumSpecification[streamHandler.getString(first)] = v;
  }

  return in;
}

const char* StreamHandler::getString(const std::string& string)
{
  StringTable::iterator i;
  i = stringTable.find(string);
  if(i == stringTable.end())
  {
    stringTable[string];
    i = stringTable.find(string);
  }
  return i->first.c_str();
}
