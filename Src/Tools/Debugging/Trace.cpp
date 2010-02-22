/**
* @file Tools/Debugging/Trace.cpp
*
* A class representing a trace.
* 
* @author Thomas Röfer
*/ 

#include <cstring>
#include "Trace.h"
#include "Platform/GTAssert.h"

static Trace* trace = 0;

Trace::Line::Line(const char* f, int l)
: line(l)
{
  strcpy(file, f);
}

void Trace::setCurrentLine(const char* file, int line) 
{
  buffer.add(Line(file, line));
}

In& operator>>(In& stream, Trace& trace) 
{
  trace.buffer.init();
  int size;
  std::string buf;
  stream >> size;
  for(int i = 0; i < size; ++i)
  {
    Trace::Line line;
    stream >> buf >> line.line;
    ASSERT(buf.size() < sizeof(line.file));
    strcpy(line.file, buf.c_str());
    trace.buffer.add(line);
  }
  return stream;
}
 
Out& operator<<(Out& stream, const Trace& trace) 
{
  stream << trace.buffer.getNumberOfEntries() << endl;
  for(int i = trace.buffer.getNumberOfEntries() - 1; i >= 0; --i)
    stream << trace.buffer[i].file << trace.buffer[i].line << endl;
  return stream;
}

Trace& getTrace()
{
  return *trace;
}

void setTrace(Trace& t)
{
  trace = &t;
}
