/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2007 All Rights Reserved - This file is confidential.\n
*
* Version : $Id$
*/


// dcoz. 11/03/09. TODO: LOTS OF CLEANING:
//
// - Fix encoding format of the comments !
//
// - move functions to a proper namespace ! (eg ALTools:: )
//  (functions such as replace() are likely to create nasty conflicts)
//
// - Replace AL_TRACE, AL_DEBUG_LINE with standard logging functions that
//   everybody can use.


/**
* Module where to put all various common tools to aldebaran naoqi, naoqicore, modules...
* It's only some small functions not doing any network message / remote connection...
*/

#ifndef AL_TOOLS_H
#define AL_TOOLS_H

#include "altypes.h"
#include "configcore.h"

/**
* Return a string describing a value.
* @param pnType Type to convert to string
*/

//namespace AL
// {
std::string HexaToString( int pnValue );
std::string DecToString( int pnValue );
std::string DoubleToString( double prValue );
std::string FloatToString( float prValue );
std::string BoolToString( bool pbValue );


// }


#ifndef FINAL_RELEASE
# define AL_DEBUG_LINE() printf( "debugline: %s:%d\n", __FILE__, __LINE__ )
# define AL_TRACE(...) printf(__VA_ARGS__)
#else
# define AL_DEBUG_LINE()  /* nothing */
# define AL_TRACE(...)    /* nothing */
#endif



#include <string.h>


#if 0 // def WIN32

# include <malloc.h>
# include <tchar.h>

template <typename CHAR_TYPE>
const CHAR_TYPE *strstr_ignorecase
(
 const CHAR_TYPE   *  szStringToBeSearched,
 const CHAR_TYPE   *  szSubstringToSearchFor
 )
{
  const CHAR_TYPE   *  pPos = NULL;
  CHAR_TYPE   *  szCopy1 = NULL;
  CHAR_TYPE   *  szCopy2 = NULL;

  // verify parameters
  if ( szStringToBeSearched == NULL ||
    szSubstringToSearchFor == NULL )
  {
    return szStringToBeSearched;
  }

  // empty substring - return input (consistent with strstr)
  if ( _tcslen(szSubstringToSearchFor) == 0 ) {
    return szStringToBeSearched;
  }

  szCopy1 = _tcslwr(_tcsdup(szStringToBeSearched));
  szCopy2 = _tcslwr(_tcsdup(szSubstringToSearchFor));

  if ( szCopy1 == NULL || szCopy2 == NULL  ) {
    // another option is to raise an exception here
    free((void*)szCopy1);
    free((void*)szCopy2);
    return NULL;
  }

  pPos = strstr(szCopy1, szCopy2);

  if ( pPos != NULL ) {
    // map to the original string
    pPos = szStringToBeSearched + (pPos - szCopy1);
  }

  free((void*)szCopy1);
  free((void*)szCopy2);

  return pPos;
}; // strstr_ignorecase(...)

#else // WIN32

const char* strstr_ignorecase(const char* szStringToBeSearched,
                              const char* szSubstringToSearchFor);

#endif // WIN32

// _Elem=char,_Traits=std::char_traits<char>

// Return mutex
#include "pthread.h"
std::string RWLockToString( const pthread_rwlock_t aMutex );

// return a copy of strSrc with all strToFind replaced
std::string replace( const std::string& strSrc, const std::string& strToFind, const std::string& strToReplace, bool bCaseSensitive = true );

// Replace XML string with escape characters
void StringToXml( const std::string& strSrc, std::string& strDst );

// replace escape characters in html string
void XmlToString( const std::string& strSrc, std::string& strDst );

// Escape characters so Python or other people reads it peacefully
void escapeCharactersInString( std::string& strDst );

// return the timestamp of current time
std::string TimeStamp( void );

// return a string describing the structure ALModuleInfo (mainly for debug purpose)
//std::string ToString( const ALModuleInfo& pInfo );



// TODO(dcoz): Rename this function. On linux, it doesn't need to use a shell.
// will launch a shell command (on every platform)
// return true when command succeed
bool LaunchShellCommand( const char* pszCommandAndParameters[], bool bWaitUntilEndOfCommand = false );

// transform lib name depending on platform => dll / dylib / so
const std::string LibNameToFilename( const std::string& strLibName );

// return true if a file with this name exists
bool IsFileExist( const char* pszFileName );

// cree un fichier non vide, return false si erreur, si il existe deja, cela faira une erreur aussi...
bool CreateDummyFile( const char* pszFileName );

// Return current RAM used
int ALTools_GetCurrentRamUsed( void );

// Macro to check ram used between two calls. Should be call before and after one block, with one name to help debug
// to avoid checkiong ram, call with false.
// samples:
//    CHECK_RAM( function_toto, false );
//    Toto();
//    CHECK_RAM( function_toto );
#define CHECK_RAM(checkname,bCheck)   ALTools_CheckRam( __FILE__, __LINE__, checkname, bCheck )

void ALTools_CheckRam( const char* pszFile, const int pnNumLine, const char* szCheckName = NULL, bool bCheck = true );

// take information about version number and output it to an official well formated string
std::string ALTools_GetVersionString( const char* pszMajor, const char* pszMinor, const char* pszPatch, const char* pszRevision );

#define ALTOOLS_VERSION(modulenameskull) ALTools_GetVersionString( DecToString(CPACK_PACKAGE_VERSION_MAJOR).c_str(), DecToString(CPACK_PACKAGE_VERSION_MINOR).c_str(), DecToString(CPACK_PACKAGE_VERSION_PATCH).c_str() , modulenameskull ## _REVISION )

#define ALTOOLS_VERSION_REV_INT(modulenameskull) ALTools_GetVersionString( modulenameskull ## _VERSION_MAJOR, modulenameskull ## _VERSION_MINOR, DecToString( modulenameskull ## _REVISION ).c_str() )


inline unsigned char klip( int val )
{
  if( val > 255 )
  {
    return 255;
  }
  if( val < 0 )
  {
    return 0;
  }
  return (unsigned char)val;
}

int ALTools_factorial(int n);
int ALTools_nCp(int n, int p);


// /data/toto.xml => /opt/naoqi/data/toto.xml
std::string ConcatLocalPathToALDirPath( const std::string & strPath );

// return the list of file in a directory (or an empty list on error)
// path example: /media/userdata/rock/
// WARNING: ne fonctionne pas en win32 (juste pas cod�)
TStringArray getListFile( const char* szPath );

// return the volatile path on the current machine (eg on geode: /var/volatile/ )
std::string getVolatilePath( void );

/**
* used to create a node in a generated xml string
*/
std::string addXmlTag(const std::string& tag, const std::string& attr, const std::string& text);

// add to current env the urbi environnement variable
void setUrbiEnvironnement( const char * pszAlPath );

/**
* All methods of tools should be
*/
namespace ALTools
{
  /**
   * Remove from a string all non alphabetic caracters : '/', '!', ',', ']'...
   * Leave numbers, alphabet, and also '_'.
   * Used by Choregraphe to handle box and I/O names.
   */
  std::string removeNonAlphabeticCaracters(const std::string pStr);

  /**
   * Get the python binary full path to execute, given the alpath as parameter
   * Needed by ALLauncher to run a remote python script
   * Returns "python" on Windows & Geode (ignore parameter)
   * Look in the SDK otherwise
   */
  std::string getPythonExecutablePath(const std::string& alPath);

  // convert an hexa string to int.
  // 0xFF => 255 ...
  int hexaAsciiToInt( const char* str );

  //-----------------------------------------------------------------------------
  //  Name  :  isEmptyChar
  /**
  * @param ch the char to test
  * @return true if it's a space, tab, ...
  *
  * Object : test if a char is a space, tab, eol, ...
  *
  */
  // Created: Alma 09-02-02
  //-----------------------------------------------------------------------------
  bool isEmptyChar( const char ch );


  //-----------------------------------------------------------------------------
  //  Name  :  firstNonEmptyChar
  /**
  * @param szInput a text to analyse, eg: " 1", "toto", or "  [36,3,"toto",3.5,[1,2,3]]"
  * @return the index [0..len-1] of the first interesting character (not ' ', nor '\t', ...)
  *
  * Object : Return the index of the first real char of a buffer
  *
  */
  // Created: Alma 09-02-02
  //-----------------------------------------------------------------------------
  int firstNonEmptyChar( const char* szInput);


  /**
  * isPrivateString
  * return true if string contain private string "ALFrameManager__"
  */
  bool isPrivateString(const std::string &pStr);
}

#endif // AL_TOOLS_H
