
#ifdef _WIN32

#ifndef EXIT_FAILURE
#include <stdlib.h>
#endif

inline char* itoa_s(int value, char* buffer, size_t size, int base)
{
  _itoa_s(value, buffer, size, base);
  return buffer;
}

#else

inline char* itoa_s(int value, char* buffer, size_t size, int base)
{
  static char num[] = "0123456789abcdefghijklmnopqrstuvwxyz";

  char* end = buffer;
  char* hardend = buffer + size - 1;
  int sign;
  char* begin, temp;

  if(base < 2 || base > 35)
  {
    *end = '\0'; 
    return buffer;
  }
  if((sign = value) < 0)
  {
    value = -value;
    hardend--;
  }
  do 
  {
    if(end >= hardend)
    {
      *end = '\0'; 
      return buffer;
    }
    *end++ = num[value % base];    
  }
  while(value /= base);
  if(sign<0)
    *end++='-';
  *end='\0';

  begin = buffer;
  end--;
  while(end > begin)
  {
    temp = *end;
    *end-- = *begin;
    *begin++ = temp;
  }

  return buffer;
}

#endif
