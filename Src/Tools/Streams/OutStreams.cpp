/**
* @file OutStreams.cpp
*
* Implementation of out stream classes.
*
* @author Thomas Röfer
* @author Martin Lötzsch
*/

#include "OutStreams.h"

void OutText::writeString(const char* value, PhysicalOutStream& stream)
{
  stream.writeToStream(" ", 1);
  bool containsSpaces = !*value || *value == '"' || strcspn(value, " \n\r\t") < strlen(value);
  if(containsSpaces)
    stream.writeToStream("\"", 1);
  for(; *value; ++value)
    if(*value == '"' && containsSpaces)
      stream.writeToStream("\\\"", 2);
    else if(*value == '\n')
      stream.writeToStream("\\n", 2);
    else if(*value == '\r')
      stream.writeToStream("\\r", 2);
    else if(*value == '\t')
      stream.writeToStream("\\t", 2);
    else if(*value == '\\')
      stream.writeToStream("\\\\", 2);
    else
      stream.writeToStream(value, 1);
  if(containsSpaces)
    stream.writeToStream("\"", 1);
}

void OutText::writeData(const void *p,int size, PhysicalOutStream& stream)
{
  for(int i = 0; i < size; ++i)
    writeChar(*((const char*&) p)++, stream);
}

void OutTextRaw::writeString(const char* value, PhysicalOutStream& stream)
{
  stream.writeToStream(value,(int)strlen(value));
}

void OutTextRaw::writeData(const void *p,int size, PhysicalOutStream& stream)
{
  for(int i = 0; i < size; ++i)
    writeChar(*((const char*&) p)++, stream);
}
