

#ifndef NDEBUG

#include <stdarg.h>
#include <stdio.h>
#ifdef _WIN32
#include <windows.h>
#endif

#include "Assert.h"

void Assert::trace(const char* format, ...)
{
#ifdef _WIN32
  char data[320];
  va_list ap;
  va_start (ap, format);
  int length;
#ifdef _MSC_VER
  length = vsprintf_s(data, sizeof(data) - 1, format, ap);
#else
  length = ::vsnprintf(data, sizeof(data) - 2, format, ap);
  if(length < 0)
    length = sizeof(data) - 2;
#endif
  va_end (ap);
  data[length++] = '\n';
  data[length] = '\0';
  OutputDebugStringA(data);
#else
  va_list ap;
  va_start (ap, format);
  vfprintf(stderr, format, ap);
  va_end (ap);
  fputc('\n', stderr);
  fflush(stderr);  
#endif
}

#endif
