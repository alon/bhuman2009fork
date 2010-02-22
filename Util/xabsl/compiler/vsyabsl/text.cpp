
/***************************************************************************
         Copyright (c) Microsoft Corporation, All rights reserved.             
    This code sample is provided "AS IS" without warranty of any kind, 
    it is not recommended for use in a production environment.
***************************************************************************/

/*---------------------------------------------------------
 String function library.
---------------------------------------------------------*/
#include "common.h"
#include <windows.h>
#include "text.h"

UniStr  uniCopy( UniStr dest, CUniStr src, int maxLen )
{
    if (!dest) return NULL;
    if (!src)  return dest;
    if (maxLen < 0) maxLen = uniLen(src);
    if (maxLen <= 0) return dest;

    wcsncpy_s( dest, maxLen,src, maxLen );
    dest[maxLen] = UNI('\0');
    return dest;
}

AnsiStr  ansiCopy( AnsiStr dest, CAnsiStr src, int maxLen )
{
    if (!dest) return NULL;
    if (!src)  return dest;
    if (maxLen < 0) maxLen = ansiLen(src);
    if (maxLen <= 0) return dest;

    strncpy_s( dest,maxLen, src, maxLen );
    dest[maxLen] = '\0';
    return dest;
}

UniStr  uniCat( UniStr dest, CUniStr src, int maxLen )
{
    if (!dest) return NULL;
    if (!src)  return dest;
    
    int len = uniLen(dest);
    if (maxLen < 0)  maxLen = len + uniLen(src);
    if (maxLen <= 0 || maxLen <= len) return dest;

    wcsncat_s( dest, maxLen - len,src, maxLen - len );
    dest[maxLen] = UNI('\0');
    return dest;
}

AnsiStr  ansiCat( AnsiStr dest, CAnsiStr src, int maxLen )
{
    if (!dest) return NULL;
    if (!src)  return dest;
    
    int len = ansiLen(dest);
    if (maxLen < 0)  maxLen = len + ansiLen(src);
    if (maxLen <= 0 || maxLen <= len) return dest;

    strncat_s( dest, maxLen - len, src, maxLen - len );
    dest[maxLen] = '\0';
    return dest;
}


int ansiToUniLen( CAnsiStr s )
{
    if (s == NULL)
        return 0;
    size_t convertedSize;
    size_t inputSize = strlen(s);
    return (int)mbstowcs_s( &convertedSize, NULL, 0, s, inputSize);
}
 
int uniToAnsiLen( CUniStr s )
{
    if (s == NULL)
        return 0;
    size_t convertedSize;
    size_t inputSize = lstrlenW(s);
    return (int)wcstombs_s(&convertedSize, NULL, 0, s, inputSize);
}

UniStr ansiToUniCopy( UniStr dest, CAnsiStr src, int maxLen )
{
    if (!dest) return NULL;
    dest[0] = UNI('\0');
    if (!src || maxLen <= 0)  return dest;
    
    size_t convertedSize;
    size_t inputSize = strlen(src);
    mbstowcs_s( &convertedSize, dest, maxLen+1, src, inputSize );
    dest[maxLen] = UNI('\0');
    return dest;
}

AnsiStr uniToAnsiCopy( AnsiStr dest, CUniStr src, int maxLen )
{
    if (!dest) return NULL;
    dest[0] = '\0';
    if (!src || maxLen <= 0)  return dest; 

    size_t convertedSize;
    size_t inputSize = lstrlenW(src);
    wcstombs_s(&convertedSize, dest, maxLen+1, src, inputSize );
    dest[maxLen] = '\0';
    return dest;
}

UniStr  ansiToUniCat( UniStr dest, CAnsiStr src, int maxLen )
{
    if (!dest) return NULL;
    if (!src)  return dest;

    int len = uniLen(dest);
    ansiToUniCopy( dest+len, src, maxLen - len );
    return dest;
}

AnsiStr uniToAnsiCat( AnsiStr dest, CUniStr src, int maxLen )
{
    if (!dest) return NULL;
    if (!src)  return NULL;

    int len = ansiLen(dest);
    uniToAnsiCopy( dest+len, src, maxLen - len );
    return dest;
}


UniStr ansiToUniDup( CAnsiStr s )
{
    if (!s) return NULL;

    int    len  = ansiToUniLen(s);
    UniStr dest = (UniStr) malloc( (len+1)*sizeof(UniChar));

    return ansiToUniCopy( dest, s, len );
}

AnsiStr uniToAnsiDup( CUniStr s )
{
    if (!s) return NULL;

    int     len  = uniToAnsiLen(s);
    AnsiStr dest = (AnsiStr) malloc( (len+1)*(sizeof(AnsiChar)) );

    return uniToAnsiCopy( dest, s, len );
}



/*void bstrFree( BSTR& bstr )
{
    if (bstr) SysFreeString(bstr);
    bstr = NULL;
}`*/

BSTR bstrCopy( BSTR& bstr, COleStr src, int maxLen )
{
    if (maxLen<0)  maxLen = oleLen(src); 
    if (maxLen<=0) return NULL;
    
    SysReAllocStringLen( &bstr, src, maxLen );
    return bstr;
}

BSTR bstrCat( BSTR& bstr, COleStr ostr, int maxLen )
{
    if (!ostr) return bstr;
    if (maxLen < 0) maxLen = oleLen(ostr);
    if (maxLen <= 0) return bstr;

    int bstrlen = bstrLen(bstr);

    BSTR temp = SysAllocStringLen( NULL, 
                    bstrlen + maxLen + 1 );
    if (!temp) return bstr;

    oleCopy( temp, bstr, bstrlen );
    oleCat( temp, ostr, maxLen );
    bstrFree(bstr);
    bstr = temp;

    return bstr;
}

