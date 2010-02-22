
/***************************************************************************
         Copyright (c) Microsoft Corporation, All rights reserved.             
    This code sample is provided "AS IS" without warranty of any kind, 
    it is not recommended for use in a production environment.
***************************************************************************/

#ifndef languagedef_h
#define languagedef_h


/*---------------------------------------------------------
  Create a new babel service
---------------------------------------------------------*/
HRESULT CreateBabelService( out IBabelService** babelService );

/*---------------------------------------------------------
  The user-defined class needs to be called "Service"
---------------------------------------------------------*/
class Service;
extern Service*       g_service;

/*---------------------------------------------------------
  Globals
---------------------------------------------------------*/
extern const wchar_t* g_languageName;
extern const wchar_t* g_languageFileExtensions[];
extern const CLSID    g_languageCLSID;

struct LanguageProperty
{
  const wchar_t* name;
  DWORD          value;
};

extern const LanguageProperty g_languageProperties[];


#endif