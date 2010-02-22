/**
* @file Platform/linux/File.cpp
* 
* Implementation of class File for Linux.
*/

#include <stdarg.h>
#include <string.h>
#include <stdlib.h> // for getenv
#include <sys/stat.h>
#include <iostream>
#include <string>

#include "GTAssert.h"
#include "File.h"

File::File(const std::string& rawName, const std::string& mode)
{
#ifdef TARGET_SIM
  std::string name(rawName);
  for(int i = name.length(); i >= 0; i--)
    if(name[i] == '\\')
      name[i] = '/';
#else
  const std::string& name(rawName);
#endif

  std::string fileName;
  if(name[0] != '/' && name[0] != '\\' && name[0] != '.' && (name[0] == 0 || name[1] != ':'))
    fileName = std::string(getGTDir()) + "/Config/" + name;
  else
    fileName = name;
  stream = fopen(fileName.c_str(), mode.c_str());

  // if file doesn't exist; fall back to the default location
  if(stream == 0 && !strncmp(name.c_str(), "Locations/", 10) &&
    strncmp(name.c_str() + 10, "Default/", 8))
  {
    const char* subPath = strchr(name.c_str() + 10, '/');
    if(subPath)
    {
      ++subPath;
      fileName = std::string(getGTDir()) + "/Config/Locations/Default/" + subPath;
      stream = fopen(fileName.c_str(), mode.c_str());
    }
  }
}

File::~File()
{
  if(stream != 0)
    fclose(stream);
}

void File::read(void* p,unsigned size)
{
  VERIFY(!eof());
  VERIFY(fread(p,1,size,stream) > 0);
}

void File::write(const void *p,unsigned size)
{
  //if opening failed, stream will be 0 and fwrite would crash
  ASSERT(stream!=0);
  VERIFY(fwrite(p,1,size,stream) == size);
}

void File::printf(const char* format, ...)
{
  va_list args;
  va_start(args,format);
  vfprintf(stream,format,args);
  va_end(args);
}

bool File::eof()
{
  //never use feof(stream), because it informs you only after reading
  //to far and is never reset, e.g. if the stream grows afterwards
  //our implementation can handle both correctly:
  if(! stream)
    return false;
  else
  {
    int c = fgetc(stream);
    if(c==EOF)
      return true;
    else
    {
      VERIFY(ungetc(c,stream)!=EOF);
      return false;
    }
  }
}

char* File::getGTDir()
{
  static char dir[FILENAME_MAX] = {0};
  if(!dir[0])
  {
    char* result = getenv("GT_DIR");
    if (result)
    {
      strcpy(dir, result); 
    }
    else
    {
#ifdef SIMROBOTQT
      VERIFY(getcwd(dir, sizeof(dir) - 7) != 0);
      char* end = dir + strlen(dir) - 1;
      struct stat buff;;
      for(;;)
      {
        if(*end == '/' || end == dir - 1)
        {
          strcpy(end + 1, "Config");
          if(stat(dir, &buff) == 0)
            if(S_ISDIR(buff.st_mode))
            {
              end[end > dir ? 0 : 1] = '\0';
              return dir;
            }
        }
        if(end < dir)
          break;
        end--;
      }
#endif
      strcpy(dir, ".");///"media/userdata");
    }
  }
  return dir;
}

void File::flush()
{
  fflush (stream);
}
