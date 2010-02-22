
/***************************************************************************
         Copyright (c) Microsoft Corporation, All rights reserved.             
    This code sample is provided "AS IS" without warranty of any kind, 
    it is not recommended for use in a production environment.
***************************************************************************/

#ifndef text_h
#define text_h

/*---------------------------------------------------------
 String function library.

 These definitions allow you to write
 unicode/ansicode independent code.

 Size specific strings:
   UniStr   = unicode string
   UniChar  = unicode character
   CUniStr  = const UniChar*

   AnsiStr  = ascii code string
   AnsiChar = ascii charactre
   CAnsiStr
   
 Portable strings:
   OleStr   = OLECHAR string
   OleChar  = OLECHAR
   COleStr

   Str      = TCHAR string; ie. uses the default windows char size.
   Char     = TCHAR
   CStr

   BSTR     = OleChar Basic Strings
   CBSTR

 Function are all prefixed with the string type:
   uniLen, strLen, ansiLen, oleLen, bstrLen

 Conversions are possible to all other types:
   uniToAnsiDup, strToOleCopy, ansiToStrCat, strToBstrDup

 If you do conversions, your function should start with
 the "USECONV" macro:
   int foo() { USECONV; ...; Char name[100]; oleToStrCopy(name,oleName,99); ... }

 You can do 'inplace' conversion for arguments using the Xxx2Xxx macros:
 a "C" can be added after the "2" for constant arguments:
   LoadTypeLibEx( Str2COle(strName), &lib )

 Most functions take an extra last arguments specifying the maximal
 *length* of the target string. This does NOT include a possible terminating
 '\0' character.

 Constant strings:
   UNI("hi"), OLE("hi"), ANSI("hi"), STR("hi")
   UNI('a'), OLE('a'), ANSI('a'), STR('a')

 Freeing strings:
   strFree, bstrFree
 
---------------------------------------------------------*/

#include <ctype.h>

#ifdef _UNICODE
#ifndef UNICODE
#define UNICODE
#endif
#endif

#define MAXSTR		255
#define MAXPATH		255
#define MAXNAME		99
#define MAXGUID	    99
#define MAXIID      MAXGUID
#define MAXCLSID    MAXGUID


typedef unsigned    Nat;

//define conversions: STR2(UNI/ANSI) OLE2(UNI/ANSI)

#ifdef UNICODE
#define STR2UNI
#define OLE2UNI
#else
#define STR2ANSI
#ifndef OLE2ANSI
#define OLE2UNI
#endif
#endif

#ifdef OLE2UNI
#define BSTR2UNI
#else
#define BSTR2ANSI
#endif


//define the basic characters
typedef wchar_t  UniChar;
typedef char            AnsiChar;

#ifndef OLECHAR
#ifdef OLE2ANSI
typedef AnsiChar    OLECHAR;
#else
typedef UniChar     OLECHAR;
#endif
#endif

//define string prefixes
#define UNI(s)  L##s
#define ANSI(s) (s)

//define the portable Char type
#if defined (UNICODE)
    typedef UniChar Char;
#define STR(s)  UNI(s)
#else
    typedef AnsiChar Char;
#define STR(s)  ANSI(s)
#endif

//define portable OLE/BSTR Char type
#ifdef OLE2ANSI
#define OLE ANSI
#else
#define OLE UNI
#endif

typedef OLECHAR OleChar;
typedef OleChar BstrChar;

//strings
typedef Char*       Str;
typedef UniChar*    UniStr;
typedef AnsiChar*   AnsiStr;
typedef OleChar*    OleStr;
typedef BSTR        Bstr;

typedef const Char*     CStr;
typedef const UniChar*  CUniStr;
typedef const AnsiChar* CAnsiStr;
typedef const OleChar*  COleStr;
typedef COleStr         CBstr;
typedef COleStr         CBSTR;

//helpers
#define ansiFree(s) FREE(s)
#define uniFree(s)  FREE(s)
#define strFree(s)  FREE(s)
#define oleFree(s)  FREE(s)


//define the basic ansi/uni code specific functions
int ansiToUniLen( CAnsiStr s );
int uniToAnsiLen( CUniStr s );

UniStr  ansiToUniCopy( UniStr dest, CAnsiStr src, int maxLen );
AnsiStr uniToAnsiCopy( AnsiStr dest, CUniStr src, int maxLen );

UniStr  ansiToUniDup( CAnsiStr s );
AnsiStr uniToAnsiDup( CUniStr s );

UniStr  ansiToUniCat( UniStr dest, CAnsiStr src, int maxLen );
AnsiStr uniToAnsiCat( AnsiStr dest, CUniStr src, int maxLen );

UniStr  uniCopy( UniStr dest, CUniStr src, int maxLen );
AnsiStr ansiCopy( AnsiStr dest, CAnsiStr src, int maxLen );

UniStr  uniCat( UniStr dest, CUniStr src, int maxLen );
AnsiStr ansiCat( AnsiStr dest, CAnsiStr src, int maxLen );


inline  bool   uniEmpty( CUniStr s )        { return (s == NULL || s[0] == 0);  }
inline  int    uniLen( CUniStr s )          { return (s ? lstrlenW(s) : 0);     }
inline  UniStr uniDup( CUniStr s)           { return (s ? _wcsdup(s) : NULL);    }

inline  int uniToLower( UniStr s )       { size_t inputSize = lstrlenW(s); return (s ? _wcslwr_s(s, inputSize) : NULL);   }
inline  int uniToUpper( UniStr s )       { size_t inputSize = lstrlenW(s); return (s ? _wcsupr_s(s, inputSize) : NULL);   }
inline  int    uniToInt( CUniStr s )        { return (s ? _wtoi(s) : 0 );       }
inline  void   uniClear( UniStr s )         { if (s) s[0] = UNI('\0');          }

inline  int uniCompare( CUniStr s, CUniStr t )
{
    if (s == t) return 0;
    if (!s)     return (-1);
    if (!t)     return 1;
                return wcscmp(s,t);
}

inline  int uniICompare( CUniStr s, CUniStr t )
{
    if (s == t) return 0;
    if (!s)     return (-1);
    if (!t)     return 1;
                return _wcsicmp(s,t);
}

inline  int uniCompareLen( CUniStr s, CUniStr t, Nat n )
{
    if (n == 0) return 0;
    if (s == t) return 0;
    if (!s)     return (-1);
    if (!t)     return 1;
                return wcsncmp(s,t,n);
}

inline  int uniICompareLen( CUniStr s, CUniStr t, Nat n )
{
    if (n == 0) return 0;
    if (s == t) return 0;
    if (!s)     return (-1);
    if (!t)     return 1;
                return _wcsnicmp(s,t,n);
}

inline UniStr uniFindSubStr( CUniStr s, CUniStr sub )  
{
    if (!s)     return NULL;
    if (!sub)   return NULL;
    return (UniStr)wcsstr(s,sub);
}

inline UniStr uniFindChar( CUniStr s, UniChar c )
{
    if (!s)     return NULL;
    return (UniStr)wcschr(s,c);
}

inline UniStr uniFindCharRev( CUniStr s, UniChar c)
{
    if (!s) return NULL;
    return (UniStr)wcsrchr(s,c);
}

#define uniCharToLower  towlower
#define uniCharToUpper  towupper
#define uniIsSpace      iswspace
#define uniIsPrintable  iswprint
#define uniIsControl    iswcntrl
#define uniIsAlpha      iswalpha
#define uniIsAlphaNum   iswalnum
#define uniIsDigit      iswdigit

inline  bool    ansiEmpty( CAnsiStr s )     { return (s == NULL || s[0] == 0);  }
inline  int     ansiLen( CAnsiStr s )       { return (s ? lstrlenA(s) : 0);     }
inline  AnsiStr ansiDup( CAnsiStr s)        { return (s ? _strdup(s) : NULL);    }

inline  int ansiToLower( AnsiStr s )    { size_t inputSize = strlen(s); return (s ? _strlwr_s(s, inputSize) : NULL);   }
inline  int ansiToUpper( AnsiStr s )    { size_t inputSize = strlen(s); return (s ? _strupr_s(s, inputSize) : NULL);   }
inline  int     ansiToInt( CAnsiStr s )     { return (s ? atoi(s) : 0 );        }
inline  void    ansiClear( AnsiStr s )      { if (s) { s[0] = '\0'; }           }

inline  int ansiCompare( CAnsiStr s, CAnsiStr t )
{
    if (s == t) return 0;
    if (!s)     return (-1);
    if (!t)     return 1;
                return strcmp(s,t);
}

inline  int ansiICompare( CAnsiStr s, CAnsiStr t )
{
    if (s == t) return 0;
    if (!s)     return (-1);
    if (!t)     return 1;
                return _stricmp(s,t);
}

inline  int ansiCompareLen( CAnsiStr s, CAnsiStr t, Nat n )
{
    if (n == 0) return 0;
    if (s == t) return 0;
    if (!s)     return (-1);
    if (!t)     return 1;
                return strncmp(s,t,n);
}

inline  int ansiICompareLen( CAnsiStr s, CAnsiStr t, Nat n )
{
    if (n == 0) return 0;
    if (s == t) return 0;
    if (!s)     return (-1);
    if (!t)     return 1;
                return _strnicmp(s,t,n);
}

inline  AnsiStr ansiFindSubStr( CAnsiStr s, CAnsiStr sub )  
{
    if (!s)     return NULL;
    if (!sub)   return NULL;
    return (AnsiStr)strstr(s,sub);
}

inline  AnsiStr ansiFindChar( CAnsiStr s, AnsiChar c )
{
    if (!s)     return NULL;
    return (AnsiStr)strchr(s,c);
}

inline AnsiStr ansiFindCharRev( CAnsiStr s, AnsiChar c )
{
    if (!s)     return NULL;
    return (AnsiStr)strrchr(s,c);
}

#define ansiCharToLower     tolower
#define ansiCharToUpper     toupper

#define ansiIsSpace         isspace
#define ansiIsPrintable     isprint
#define ansiIsControl       iscntrl
#define ansiIsAlpha         isalpha
#define ansiIsAlphaNum      isalnum
#define ansiIsDigit         isdigit

//conversion macros
#define USECONV     int _convlen = 0; _convlen

#define Uni2Ansi(u) (\
    (NULL==u) ? NULL : (\
        _convlen = uniToAnsiLen(u), \
            uniToAnsiCopy( (AnsiStr)_alloca((_convlen+1)*sizeof(AnsiChar)), u, _convlen )))

#define Ansi2Uni(a) (\
    (NULL==a) ? NULL : (\
        _convlen = ansiToUniLen(a), \
           ansiToUniCopy( (UniStr)_alloca((_convlen+1)*sizeof(UniChar)), a, _convlen )))

#define AnsiCopy(a) (\
    (NULL==a) ? NULL : (\
        _convlen = ansiLen(a), \
            ansiCopy( (AnsiStr)_alloca((_convlen+1)*sizeof(AnsiChar)), a, _convlen )))

#define UniCopy(u) (\
    (NULL==u) ? NULL : (\
       _convlen = uniLen(u), \
            uniCopy( (UniStr)_alloca((_convlen+1)*sizeof(UniChar)), u, _convlen )))

#define Uni2CAnsi(u)    ((CAnsiStr)Uni2Ansi(u))
#define Ansi2CUni(a)    ((CUniStr)Ansi2Uni(a))


//Str conversion
#ifdef UNICODE

#define Uni2Str UniCopy
#define Str2Uni UniCopy
inline  CStr    Uni2CStr( CUniStr u )   { return u; };
inline  CUniStr Str2CUni( CStr s )      { return s; };

#define Ansi2Str  Ansi2Uni
#define Str2Ansi  Uni2Ansi
#define Ansi2CStr Ansi2CUni
#define Str2CAnsi Uni2CAnsi

#define uniToStrLen     uniLen
#define uniToStrCopy    uniCopy
#define uniToStrDup     uniDup
#define uniToStrCat     uniCat

#define strToUniLen     uniLen
#define strToUniCopy    uniCopy
#define strToUniDup     uniDup
#define strToUniCat     uniCat

#define ansiToStrLen    ansiToUniLen
#define ansiToStrCopy   ansiToUniCopy
#define ansiToStrDup    ansiToUniDup
#define ansiToStrCat    ansiToUniCat

#define strToAnsiLen    uniToAnsiLen
#define strToAnsiCopy   uniToAnsiCopy
#define strToAnsiDup    uniToAnsiDup
#define strToAnsiCat    uniToAnsiCat

#else

#define Ansi2Str    AnsiCopy
#define Str2Ansi    AnsiCopy
inline  CStr        Ansi2CStr( CAnsiStr a ){ return a; };
inline  CAnsiStr    Str2CAnsi( CStr s  )    { return s; };

#define Uni2Str  Uni2Ansi
#define Str2Uni  Ansi2Uni
#define Uni2CStr Uni2CAnsi
#define Str2CUni Ansi2CUni

#define uniToStrLen     uniToAnsiLen
#define uniToStrCopy    uniToAnsiCopy
#define uniToStrDup     uniToAnsiDup
#define uniToStrCat     uniToAnsiCat

#define strToUniLen     ansiToUniLen
#define strToUniCopy    ansiToUniCopy
#define strToUniDup     ansiToUniDup
#define strToUniCat     ansiToUniCat

#define ansiToStrLen    ansiLen
#define ansiToStrCopy   ansiCopy
#define ansiToStrDup    ansiDup
#define ansiToStrCat    ansiCat

#define strToAnsiLen    ansiLen
#define strToAnsiCopy   ansiCopy
#define strToAnsiDup    ansiDup
#define strToAnsiCat    ansiCat


#endif


//Ole conversion
#ifdef OLE2ANSI

#define Ansi2Ole    AnsiCopy
#define Ole2Ansi    AnsiCopy
inline  COleStr     Ansi2COle( CAnsiStr a ){ return a; };
inline  CAnsiStr    Ole2CAnsi( COleStr s ) { return s; };

#define Uni2Ole  Uni2Ansi
#define Ole2Uni  Ansi2Uni
#define Uni2COle Uni2CAnsi
#define Ole2CUni Ansi2CUni

#define uniToOleLen     uniToAnsiLen
#define uniToOleCopy    uniToAnsiCopy
#define uniToOleDup     uniToAnsiDup
#define uniToOleCat     uniToAnsiCat

#define oleToUniLen     ansiToUniLen
#define oleToUniCopy    ansiToUniCopy
#define oleToUniDup     ansiToUniDup
#define oleToUniCat     ansiToUniCat

#define ansiToOleLen    ansiLen
#define ansiToOleCopy   ansiCopy
#define ansiToOleDup    ansiDup
#define ansiToOleCat    ansiCat

#define oleToAnsiLen    ansiLen
#define oleToAnsiCopy   ansiCopy
#define oleToAnsiDup    ansiDup
#define oleToAnsiCat    ansiCat

#else

#define Uni2Ole UniCopy
#define Ole2Uni UniCopy
inline  COleStr Uni2COle( CUniStr u )   { return u; };
inline  CUniStr Ole2CUni( COleStr o )   { return o; };

#define Ansi2Ole  Ansi2Uni
#define Ole2Ansi  Uni2Ansi
#define Ansi2COle Ansi2CUni
#define Ole2CAnsi Uni2CAnsi

#define uniToOleLen     uniLen
#define uniToOleCopy    uniCopy
#define uniToOleDup     uniDup

#define oleToUniLen     uniLen
#define oleToUniCopy    uniCopy
#define oleToUniDup     uniDup
#define oleToUniCat     uniCat

#define ansiToOleLen    ansiToUniLen
#define ansiToOleCopy   ansiToUniCopy
#define ansiToOleDup    ansiToUniDup
#define ansiToOleCat    ansiToUniCat

#define oleToAnsiLen    uniToAnsiLen
#define oleToAnsiCopy   uniToAnsiCopy
#define oleToAnsiDup    uniToAnsiDup
#define oleToAnsiCat    uniToAnsiCat

#endif

//Ole - Str conversion

#if defined(UNICODE)
//ole and str are the same
#define Ole2Str UniCopy
#define Str2Ole UniCopy
inline  COleStr Str2COle( CStr s )      { return s; };
inline  CStr    Ole2CStr( COleStr o )   { return o; };

#define oleToStrLen     uniLen
#define oleToStrCopy    uniCopy
#define oleToStrDup     uniDup
#define oleToStrCat     uniCat

#define strToOleLen     uniLen
#define strToOleCopy    uniCopy
#define strToOleDup     uniDup
#define strToOleCat     uniCat

#elif defined(OLE2ANSI)
//ole and str are the same
#define Str2Ole AnsiCopy
#define Ole2Str AnsiCopy
inline  COleStr Str2COle( CStr s )      { return s; };
inline  CStr    Ole2CStr( COleStr o )   { return o; };

#define oleToStrLen     ansiLen
#define oleToStrCopy    ansiCopy
#define oleToStrDup     ansiDup
#define oleToStrCat     ansiCat

#define strToOleLen     ansiLen
#define strToOleCopy    ansiCopy
#define strToOleDup     ansiDup
#define strToOleCat     ansiCat


#else
//ole is uni, str is ansi
#define Ole2Str  Uni2Ansi
#define Str2Ole  Ansi2Uni
#define Ole2CStr Uni2CAnsi
#define Str2COle Ansi2CUni

#define oleToStrLen     uniToAnsiLen
#define oleToStrCopy    uniToAnsiCopy
#define oleToStrDup     uniToAnsiDup
#define oleToStrCat     uniToAnsiCat

#define strToOleLen     ansiToUniLen
#define strToOleCopy    ansiToUniCopy
#define strToOleDup     ansiToUniDup
#define strToOleCat     ansiToUniCat

#endif


//utility functions

//Str
#ifdef UNICODE

#define strEmpty            uniEmpty
#define strLen              uniLen
#define strCopy             uniCopy
#define strCat              uniCat
#define strDup              uniDup
#define strClear            uniClear
#define strCompare          uniCompare
#define strICompare         uniICompare
#define strCompareLen       uniCompareLen
#define strICompareLen      uniICompareLe
#define strToLower          uniToLower
#define strToUpper          uniToUpper
#define strCharToLower      uniCharToLower
#define strCharToUpper      uniCharToUpper
#define strFindSubStr       uniFindSubStr
#define strFindChar         uniFindChar
#define strFindCharRev      uniFindCharRev
#define strToInt            uniToInt
#define strIsSpace          uniIsSpace
#define strIsPrintable      uniIsPrintable  
#define strIsControl        uniIsControl    
#define strIsAlpha          uniIsAlpha      
#define strIsAlphaNum       uniIsAlphaNum   
#define strIsDigit          uniIsDigit      
#else

#define strEmpty            ansiEmpty
#define strLen              ansiLen
#define strCopy             ansiCopy
#define strCat              ansiCat
#define strDup              ansiDup
#define strClear            ansiClear
#define strCompare          ansiCompare
#define strICompare         ansiICompare
#define strCompareLen       ansiCompareLen
#define strICompareLen      ansiICompareLen
#define strToLower          ansiToLower
#define strToUpper          ansiToUpper
#define strCharToLower      ansiCharToLower
#define strCharToUpper      ansiCharToUpper
#define strFindSubStr       ansiFindSubStr
#define strFindChar         ansiFindChar
#define strFindCharRev      ansiFindCharRev
#define strToInt            ansiToInt
#define strIsSpace          ansiIsSpace
#define strIsPrintable      ansiIsPrintable  
#define strIsControl        ansiIsControl    
#define strIsAlpha          ansiIsAlpha      
#define strIsAlphaNum       ansiIsAlphaNum   
#define strIsDigit          ansiIsDigit      


#endif


//Ole
#ifdef OLE2ANSI

#define oleEmpty            ansiEmpty
#define oleLen              ansiLen
#define oleCopy             ansiCopy
#define oleCat              ansiCat
#define oleDup              ansiDup
#define oleClear            ansiClear
#define oleCompare          ansiCompare
#define oleICompare         ansiICompare
#define oleCompareLen       ansiCompareLen
#define oleICompareLen      ansiICompareLen
#define oleToLower          ansiToLower
#define oleToUpper          ansiToUpper
#define oleCharToLower      ansiCharToLower
#define oleCharToUpper      ansiCharToUpper
#define oleFindSubStr       ansiFindSubStr
#define oleFindChar         ansiFindChar
#define oleFindCharRev      ansiFindCharRev
#define oleToInt            ansiToInt
#define oleIsSpace          ansiIsSpace
#define oleIsPrintable      ansiIsPrintable  
#define oleIsControl        ansiIsControl    
#define oleIsAlpha          ansiIsAlpha      
#define oleIsAlphaNum       ansiIsAlphaNum   
#define oleIsDigit          ansiIsDigit      

#else

#define oleEmpty            uniEmpty
#define oleLen              uniLen
#define oleCopy             uniCopy
#define oleCat              uniCat
#define oleDup              uniDup
#define oleClear            uniClear
#define oleCompare          uniCompare
#define oleICompare         uniICompare
#define oleCompareLen       uniCompareLen
#define oleICompareLen      uniICompareLen
#define oleToLower          uniToLower
#define oleToUpper          uniToUpper
#define oleCharToLower      uniCharToLower
#define oleCharToUpper      uniCharToUpper
#define oleFindSubStr       uniFindSubStr
#define oleFindChar         uniFindChar
#define oleFindCharRev      uniFindCharRev
#define oleToInt            uniToInt
#define oleIsSpace          uniIsSpace
#define oleIsPrintable      uniIsPrintable  
#define oleIsControl        uniIsControl    
#define oleIsAlpha          uniIsAlpha      
#define oleIsAlphaNum       uniIsAlphaNum   
#define oleIsDigit          uniIsDigit      

#endif



//BSTR
inline  bool bstrEmpty( CBSTR s )      { return (s == NULL || s[0] == 0); }
inline  int  bstrLen( CBSTR s)         { return (s ? SysStringLen((BSTR)s) : 0 ); }
inline  BSTR bstrDup( CBSTR s)         { return (s ? SysAllocString((BSTR)s) : NULL ); }

void bstrFree( BSTR& );
BSTR bstrCopy( BSTR&, COleStr, int maxLen );
BSTR bstrCat( BSTR&, COleStr, int maxLen ); 

//conversion
//use dup for 'to bstr' conversion
#define ansiToBstrDup(s)    bstrDup( Ansi2COle(s) )
#define uniToBstrDup(s)     bstrDup( Uni2COle(s) )
#define strToBstrDup(s)     bstrDup( Str2COle(s) )
#define oleToBstrDup(s)     bstrDup( s )

#define bstrNew(s)          oleToBstrDup( OLE(s) )

#define bstrToAnsiDup   oleToAnsiDup
#define bstrToUniDup    oleToUniDup
#define bstrToStrDup    oleToStrDup
#define bstrToOleDup    oleDup

#define Bstr2Ansi       Ole2Ansi
#define Bstr2CAnsi      Ole2CAnsi
#define Bstr2Uni        Ole2Uni
#define Bstr2CUni       Ole2CUni
#define Bstr2Str        Ole2Str
#define Bstr2CStr       Ole2CStr
#define Bstr2Ole        OleCopy
inline  COleStr Bstr2COle( BSTR s)  { return s; }

//len
#define ansiToBstrLen   ansiToOleLen
#define uniToBstrLen    uniToOleLen
#define strToBstrLen    strToOleLen
#define oleToBstrLen    oleLen

#if defined(UNICODE)
#define bstrToAnsiLen   uniToAnsiLen
#define bstrToUniLen    bstrLen
#define bstrToStrLen    bstrLen
#elif defined(OLE2ANSI)
#define bstrToAnsiLen   bstrLen
#define bstrToUniLen    ansiToUniLen
#define bstrToStrLen    bstrLen
#else
#define bstrToAnsiLen   uniToAnsiLen
#define bstrToUniLen    bstrLen
#define bstrToStrLen    uniToAnsiLen
#endif

#define bstrToOleLen    bstrLen

//copy
#define ansiToBstrCopy(d,s,l)   bstrCopy(d,Ansi2COle(s),l)
#define uniToBstrCopy(d,s,l)    bstrCopy(d,Uni2COle(s),l)
#define strToBstrCopy(d,s,l)    bstrCopy(d,Str2COle(s),l)
#define oleToBstrCopy(d,s,l)    bstrCopy(d,s,l)

#define bstrToAnsiCopy      oleToAnsiCopy
#define bstrToUniCopy       oleToUniCopy
#define bstrToStrCopy       oleToStrCopy
#define bstrToOleCopy       oleCopy

//cat
#define ansiToBstrCat(d,s,l)    bstrCat(d,Ansi2COle(s),l)
#define uniToBstrCat(d,s,l)     bstrCat(d,Uni2COle(s),l)
#define strToBstrCat(d,s,l)     bstrCat(d,Str2COle(s),l)
#define oleToBstrCat(d,s,l)     bstrCat(d,s,l)

#define bstrToAnsiCat           oleToAnsiCat
#define bstrToUniCat            oleToUniCat
#define bstrToStrCat            oleToStrCat
#define bstrToOleCat            oleCat

#define bstrClear           bstrFree
#define bstrCompare         oleCompare
#define bstrICompare        oleICompare
#define bstrCompareLen      oleCompareLen
#define bstrICompareLen     oleICompareLen
#define bstrToLower         oleToLower
#define bstrToUpper         oleToUpper
#define bstrFindSubStr      oleFindSubStr
#define bstrFindChar        oleFindChar
#define bstrFindCharRev     oleFindCharRev
#define bstrToInt           oleToInt

//end include
#endif