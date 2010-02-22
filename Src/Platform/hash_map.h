/** 
* @file  Platform/hash_map.h
* 
* Inclusion and definitions to use hash_map platform independent 
*
* @author Ingo Sieverdingbeck
*/

#ifndef HASH_MAP_H
#define HASH_MAP_H

#include <string>

#if defined(_WIN32)
#  include <hash_map>
#  define HASH_MAP_NAMESPACE stdext
#elif defined(LINUX)
#  include <ext/hash_map>
#  define STRING_HASH_MISSING
#  define HASH_MAP_NAMESPACE __gnu_cxx
#  define stdext __gnu_cxx
#else
#  error "Unknown platform"
#endif

#define USING_HASH_MAP using HASH_MAP_NAMESPACE::hash_map;


#ifdef STRING_HASH_MISSING
namespace HASH_MAP_NAMESPACE {
  template <> struct hash<std::string>
  {
    size_t operator() (const std::string& str) const
    {
      return hash<const char*>()(str.c_str());
    }
  };
}
#endif //STRING_HASH_MISSING


#endif // hash_map
