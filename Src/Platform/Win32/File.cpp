/**
* @file Platform/Win32/File.cpp
* 
* Implementation of class File for Win32.
*/

#include <windows.h>
#include <stdarg.h>
#include <string.h>
#include <direct.h>

#include "GTAssert.h"
#include "File.h"

File::File(const std::string& name, const std::string& mode)
{
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
  if(stream != 0) fclose(stream);
}

void File::read(void* p,unsigned size) 
{
  VERIFY(!eof());fread(p,1,size,stream);
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

const char* File::getGTDir()
{
  static char dir[MAX_PATH] = {0};
  if(!dir[0])
  {
#ifdef TARGET_SIM

    // determine module file from command line
    const char* commandLine = GetCommandLine();
    int len;
    if(*commandLine == '"')
    {
      commandLine++;
      const char* end = strchr(commandLine, '"');
      if(end)
        len = end - commandLine;
      else
        len = strlen(commandLine);
    }
    else
      len = strlen(commandLine);
    if(len >= sizeof(dir) - 8)
      len = sizeof(dir) - 8;
    memcpy(dir, commandLine, len);
    dir[len] = '\0';

    // if there is no given directory, use the current working dir
    if(!strchr(dir, '\\'))
    {
      len = int(GetCurrentDirectory(sizeof(dir) - 9, dir));
      if(len && dir[len - 1] != '\\')
      {
        dir[len++] = '\\';
        dir[len] = '\0';
      }
    }

    //drive letter in lower case:
    if(len && dir[1] == ':')
      *dir |= tolower(*dir);

    // try to find the config directory
    char* end = dir + len - 1;
    for(;;)
    {
      if(*end == '/' || *end == '\\' || *end == ':' || end == dir - 1)
      {
        if(*end == ':')
          *(end++) = '\\';
        strcpy(end + 1, "Config");
        DWORD attr = GetFileAttributes(dir);
        if(attr != INVALID_FILE_ATTRIBUTES && attr & FILE_ATTRIBUTE_DIRECTORY)
        {
          end[end > dir ? 0 : 1] = '\0';
          for(; end >= dir; end--)
            if(*end == '\\')
              *end = '/';
          return dir;
        }
      }
      if(end < dir)
        break;
      end--;
    }
    ASSERT(false);
#else
    strcpy(dir, ".");
#endif
  }
  return dir;
}
