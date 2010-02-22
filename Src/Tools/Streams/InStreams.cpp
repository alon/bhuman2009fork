/**
* @file InStreams.cpp
*
* Implementation of in stream classes.
*
* @author Thomas Röfer
* @author Martin Lötzsch
*/

#include "InStreams.h"
#include <string.h>
#include "Platform/GTAssert.h"

void StreamReader::skipData(int size, PhysicalInStream& stream)
{
  // default implementation
  char* dummy = new char[size];
  readData(dummy, size, stream);
  delete [] dummy;
}

void PhysicalInStream::skipInStream(int size)
{
  // default implementation
  char* dummy = new char[size];
  readFromStream(dummy, size);
  delete [] dummy;
}

void InText::readString(std::string& value, PhysicalInStream& stream)
{
  value = "";
  skipWhitespace(stream);
  bool containsSpaces = theChar == '"';
  if(containsSpaces && !isEof(stream))
    nextChar(stream);
  while(!isEof(stream) && (containsSpaces || !isWhitespace()) && (!containsSpaces || theChar != '"'))
  {
    if(theChar == '\\')
    {
      nextChar(stream);
      if(theChar == 'n')
        theChar = '\n';
      else if(theChar == 'r')
        theChar = '\r';
      else if(theChar == 't')
        theChar = '\t';
    }
    value += theChar;
    if(!isEof(stream))
      nextChar(stream);
  }
  if(containsSpaces && !isEof(stream))
    nextChar(stream);
  skipWhitespace(stream);
}

void InText::readData(void *p,int size, PhysicalInStream& stream)
{
  for(int i = 0; i < size; ++i)
    readChar(*((char*&) p)++,stream);
}

bool InText::isWhitespace()
{
  return theChar == ' ' || theChar == '\n' || theChar == '\r' || theChar == '\t';
}

void InText::skipWhitespace(PhysicalInStream& stream)
{
  while(!isEof(stream) && isWhitespace())
    nextChar(stream);
}

void InConfig::create(const std::string& sectionName, PhysicalInStream& stream)
{
  if(stream.exists() && sectionName != "") 
  {
    std::string fileEntry;
    std::string section = std::string("[") + sectionName + "]";
    
    while(!isEof(stream)) 
    {
      readString(fileEntry,stream);
      if(fileEntry == section)
      {
        if(theChar == '[') // handle empty section
          while (!isEof(stream)) 
            InText::nextChar(stream);
        break;
      }
    }
    readSection = true;
  }
}

bool InConfig::isWhitespace()
{
  return (theChar == '/' && (theNextChar == '*' || theNextChar == '/')) ||
         theChar == '#' || InText::isWhitespace();
}

void InConfig::skipWhitespace(PhysicalInStream& stream)
{
  while(!isEof(stream) && isWhitespace())
  {
    while(!isEof(stream) && InText::isWhitespace())
      nextChar(stream);
    if(!isEof(stream))
    {
      if(theChar == '/' && theNextChar == '/')
      {
        skipLine(stream);
      }
      else if(theChar == '/' && theNextChar == '*')
      {
        skipComment(stream);
      }
      else if(theChar == '#')
      {
        skipLine(stream);
      }
    }
  }
}

void InConfig::nextChar(PhysicalInStream& stream) 
{
  InText::nextChar(stream);
  if (readSection && theChar == '[')
    while (!isEof(stream)) 
      InText::nextChar(stream);
}

void InConfig::skipLine(PhysicalInStream& stream)
{
  while(!isEof(stream) && theChar != '\n')
    nextChar(stream);
  if(!isEof(stream))
    nextChar(stream);
}

void InConfig::skipComment(PhysicalInStream& stream)
{
  // skip /*
  nextChar(stream);
  nextChar(stream);
  while(!isEof(stream) && (theChar != '*' || theNextChar != '/'))
    nextChar(stream);

  // skip */
  if(!isEof(stream))
    nextChar(stream);
  if(!isEof(stream))
    nextChar(stream);
}

void InMemory::readFromStream(void* p, int size)
{
  if (memory!=0) 
  { 
    memcpy(p,memory,size); 
    memory += size; 
  }
}
