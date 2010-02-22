
/***************************************************************************
         Copyright (c) Microsoft Corporation, All rights reserved.             
    This code sample is provided "AS IS" without warranty of any kind, 
    it is not recommended for use in a production environment.
***************************************************************************/

#include "common.h"

#include <strsafe.h>

#include "dllmain.h"
#include "languagedef.h"
#include "winnt.h"

#define SECURE_ACCESS ~(WRITE_DAC | WRITE_OWNER | GENERIC_ALL | ACCESS_SYSTEM_SECURITY)

/*---------------------------------------------------------
  globals
-----------------------------------------------------------*/
ITypeInfo* g_typeInfoBabelService     = NULL;

static HINSTANCE  g_instance          = NULL;
       ULONG      g_dllRefCount       = 0;

//babel package clsid's
static const CLSID babelPackageCLSID7 = 
  { 0x221f0eb7, 0x30f7, 0x45ff, { 0xae, 0x73, 0x59, 0x68, 0xb1, 0x67, 0xcf, 0x9 } };

//registry paths
static const wchar_t vsCurrentVersion[]    = L"8.0";
static const wchar_t regPathVS_NoVersion[] = L"Software\\Microsoft\\VisualStudio\\";

static const wchar_t regPathFileExts[]     = L"Languages\\File Extensions";
static const wchar_t regPathLServices[]    = L"Languages\\Language Services";
static const wchar_t regPathServices[]     = L"Services";

static const size_t clsidLen  = 127;
static const size_t clsidSize = clsidLen + 1;

/*---------------------------------------------------------
  Prototypes
-----------------------------------------------------------*/
static HRESULT RegisterServer(in bool registerIt, in wchar_t* pszRegRoot);
static HRESULT GetRegistrationRoot(wchar_t* pszRegistrationRoot, ULONG cchBuffer);
static HRESULT LoadTypeInfo();

//registry helpers
static HRESULT RegCreateValue( in HKEY key, in const wchar_t* name, in const wchar_t* value );
static HRESULT RegCreateDwordValue( in HKEY key, in const wchar_t* name, in DWORD value );
static HRESULT RegDeleteRecursive( in HKEY keyRoot, in const wchar_t* path );


/*---------------------------------------------------------
  Registry (Inline functions to return a HRESULT)
-----------------------------------------------------------*/
inline HRESULT hrRegOpenKeyEx(HKEY hKey, LPCTSTR lpSubKey, DWORD dwOptions, REGSAM samDesired, PHKEY phkResult)
{
    LONG lRetCode = RegOpenKeyEx(hKey, lpSubKey, dwOptions, samDesired, phkResult);
    return (ERROR_SUCCESS == lRetCode) ? S_OK : HRESULT_FROM_WIN32(lRetCode);
}
inline HRESULT hrRegCreateKeyEx(HKEY hKey, LPCWSTR lpSubKey, DWORD Reserved, LPWSTR lpClass, DWORD dwOptions, REGSAM samDesired, LPSECURITY_ATTRIBUTES lpSecurityAttributes, PHKEY phkResult, LPDWORD lpdwDisposition)
{
    LONG lRetCode = RegCreateKeyEx(hKey, lpSubKey, Reserved, lpClass, dwOptions, samDesired, lpSecurityAttributes, phkResult, lpdwDisposition);
    return (ERROR_SUCCESS == lRetCode) ? S_OK : HRESULT_FROM_WIN32(lRetCode);
}

/*---------------------------------------------------------
  Class Factory
-----------------------------------------------------------*/
class BabelServiceFactory : IClassFactory
{
private:
  ULONG   m_refCount;

public:
  BabelServiceFactory();
  ~BabelServiceFactory();

  //IUnknown
  STDMETHODIMP QueryInterface( REFIID iid, LPVOID* obj );
  STDMETHODIMP_(ULONG) AddRef();
  STDMETHODIMP_(ULONG) Release();

  //IClassFactory
  STDMETHODIMP CreateInstance( IUnknown* outer, REFIID iid, LPVOID* obj );
  STDMETHODIMP LockServer( BOOL lockIt );
};



/*---------------------------------------------------------
  Dll entry points
-----------------------------------------------------------*/
BOOL APIENTRY DllMain( HINSTANCE instance, DWORD reason, LPVOID reserved )
{
  switch (reason)
  {
  case DLL_PROCESS_ATTACH:
    TRACE_INIT( g_languageName );
    TRACE( L"load dll" );
    g_instance    = instance;
    g_dllRefCount = 0;
    LoadTypeInfo();
    break;
  case DLL_PROCESS_DETACH:
    TRACE(L"unload dll");
    RELEASE(g_typeInfoBabelService);
    TRACE_DONE;
    break;
  case DLL_THREAD_ATTACH:
    TRACE(
        L"dll attach thread");
    break;
  case DLL_THREAD_DETACH:
    TRACE(L"dll detach thread");
    break;
  }

  return TRUE;
}

STDAPI DllCanUnloadNow()
{
    return (g_dllRefCount == 0) ? S_OK : S_FALSE;
}


STDAPI DllGetClassObject( REFCLSID clsid, REFIID iid, LPVOID* obj )
{
  OUTARG(obj);

  if (clsid == g_languageCLSID)
  {
    BabelServiceFactory* factory  = new BabelServiceFactory();
    if (!factory) return E_OUTOFMEMORY;

    HRESULT hr = factory->QueryInterface(iid,obj);
    RELEASE(factory);
    return hr;
  }
  else 
    return CLASS_E_CLASSNOTAVAILABLE;
}


STDAPI DllRegisterServer()
{
  return RegisterServer(true, NULL);
}


STDAPI DllUnregisterServer()
{
  return RegisterServer(false, NULL);
}

STDAPI VSDllRegisterServer(wchar_t* strRegRoot)
{
  return RegisterServer(true, strRegRoot);
}


STDAPI VSDllUnregisterServer(wchar_t* strRegRoot)
{
  return RegisterServer(false, strRegRoot);
}


/*---------------------------------------------------------
  Class Factory
-----------------------------------------------------------*/
BabelServiceFactory::BabelServiceFactory()
{
  TRACE_CREATE(L"BabelServiceFactory", this );
  m_refCount = 1;
}

BabelServiceFactory::~BabelServiceFactory()
{
  TRACE_DESTROY(this);
}

//IUnknown
STDMETHODIMP BabelServiceFactory::QueryInterface( REFIID iid, LPVOID* obj )
{
  OUTARG(obj);

  if (iid == IID_IUnknown || iid == IID_IClassFactory)
  {
    *obj = static_cast<IClassFactory*>(this);
  }
  else
    return E_NOINTERFACE;

  AddRef();
  return S_OK;
}

STDMETHODIMP_(ULONG) BabelServiceFactory::AddRef()
{
  return IncRefCount(&m_refCount);
}

STDMETHODIMP_(ULONG) BabelServiceFactory::Release()
{
  if (DecRefCount(&m_refCount) == 0)
  {
    delete this;
    return 0;
  }
  else
    return m_refCount;
}

//IClassFactory
STDMETHODIMP BabelServiceFactory::CreateInstance( IUnknown* outer, REFIID iid, LPVOID* obj )
{
  TRACE(L"BabelServiceFactory::CreateInstance");
  OUTARG(obj);
  HRESULT hr;

  if (outer) return CLASS_E_NOAGGREGATION;

  IBabelService* babelService;
  hr = CreateBabelService( &babelService );
  if (FAILED(hr)) return hr;

  hr = babelService->QueryInterface( iid, obj );
  RELEASE(babelService);

  return hr;
}

STDMETHODIMP BabelServiceFactory::LockServer( BOOL lockIt )
{
  if (lockIt)
  {
    IncRefCount( &g_dllRefCount );
  }
  else
  {
    DecRefCount( &g_dllRefCount );
  }

  return S_OK;
}

/*---------------------------------------------------------
  Load the BabelService type library
-----------------------------------------------------------*/
static HRESULT LoadTypeInfo()
{
  HRESULT hr;

  //load it only once
  if (g_typeInfoBabelService) return S_OK;

  //load the library
  ITypeLib* lib;
  hr = LoadRegTypeLib( LIBID_BabelServiceLib, 1, 0, LANG_NEUTRAL, &lib );
  if (FAILED(hr)) return hr;

  //get the typeinfo 
  hr = lib->GetTypeInfoOfGuid( IID_IBabelService, &g_typeInfoBabelService );
  RELEASE(lib);
  if (FAILED(hr)) return hr; 
  
  return S_OK;
}

/*---------------------------------------------------------
  Registration
-----------------------------------------------------------*/
static HRESULT RegisterService( in const wchar_t* regPathVS, 
                                in const CLSID&   packageCLSID, 
                                in bool           registerIt )
{
  HRESULT hr;

  //get clsid's as string
  wchar_t oleLanguageCLSID[clsidSize];
  if (StringFromGUID2(g_languageCLSID, oleLanguageCLSID, clsidLen) == 0)
  { return E_FAIL; }

  wchar_t olePackageCLSID[clsidLen+1];
  if (StringFromGUID2(packageCLSID, olePackageCLSID, clsidLen) == 0)
  { return E_FAIL; }

  //open root key
  HKEY    keyVS   = 0;
  HKEY    keySub  = 0;
  HKEY    key     = 0;

  hr = hrRegOpenKeyEx(HKEY_LOCAL_MACHINE, regPathVS, 0, KEY_READ, &keyVS);
  if (FAILED(hr)) return hr;

  //file extensions
  hr = hrRegOpenKeyEx(keyVS, regPathFileExts, 0, KEY_WRITE, &keySub);
  if (FAILED(hr)) goto fail;

  int idx;
  for (idx = 0; g_languageFileExtensions[idx] != NULL; idx++)
  {
    const wchar_t* fileExt;

    fileExt = g_languageFileExtensions[idx];

    if (!registerIt)
    {
      RegDeleteRecursive( keySub, fileExt );
    }
    else
    {
	  hr = hrRegCreateKeyEx(keySub, fileExt, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &key, NULL); 
      if (FAILED(hr)) goto fail;

      hr = RegCreateValue( key, NULL, oleLanguageCLSID );
      if (FAILED(hr)) goto fail;

      hr = RegCreateValue( key, L"Name", g_languageName );
      if (FAILED(hr)) goto fail;

      RegCloseKey(key); key = 0;
    }
  }
  RegCloseKey(keySub); keySub = 0;


  //Create an extensions string
  wchar_t fileExtensions[MAXPATH+1];
  fileExtensions[0] = 0;
  for (idx = 0; g_languageFileExtensions[idx] != NULL; idx++)
  {
    hr = StringCchCat( fileExtensions, MAXPATH, g_languageFileExtensions[idx] );
    if (FAILED(hr)) goto fail;
    hr = StringCchCat( fileExtensions, MAXPATH, L";" );
    if (FAILED(hr)) goto fail;
  }

  //language service
  hr = hrRegOpenKeyEx( keyVS, regPathLServices, 0, KEY_WRITE, &keySub );
  if (FAILED(hr)) goto fail;

  if (!registerIt)
  {
    RegDeleteRecursive( keySub, g_languageName );
  }
  else
  {
    hr = hrRegCreateKeyEx( keySub, g_languageName, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &key, NULL );
    if (FAILED(hr)) goto fail;

    hr = RegCreateValue( key, NULL, oleLanguageCLSID );
    if (FAILED(hr)) goto fail;

    hr = RegCreateValue( key, L"Package", olePackageCLSID );
    if (FAILED(hr)) goto fail;

    hr = RegCreateValue( key, L"Extensions", fileExtensions );
    if (FAILED(hr)) goto fail;

    hr = RegCreateDwordValue( key, L"LangResId", 0 );
    if (FAILED(hr)) goto fail;

    hr = RegCreateDwordValue( key, L"RequestStockColors", 1 );
    if (FAILED(hr)) goto fail;

    hr = RegCreateDwordValue( key, L"ShowCompletion", 0 );
    if (FAILED(hr)) goto fail;


    //babel specific entries
    hr = RegCreateDwordValue( key, L"ThreadModel", 1 ); //colorizer/parser can be called from two different threads
    if (FAILED(hr)) goto fail;

    hr = RegCreateDwordValue( key, L"MatchBraces", 1 );
    if (FAILED(hr)) goto fail;

    hr = RegCreateDwordValue( key, L"QuickInfo", 1 );
    if (FAILED(hr)) goto fail;

    const LanguageProperty* property;
    for (property = g_languageProperties; property->name; property++)
    {
      hr = RegCreateDwordValue( key, property->name, property->value );
      if (FAILED(hr)) goto fail;
    }
    
    RegCloseKey( key ); key = 0;
  }
  RegCloseKey( keySub ); keySub = 0;

  //services
  hr = hrRegOpenKeyEx(keyVS, regPathServices, 0, KEY_WRITE, &keySub);
  if (FAILED(hr)) goto fail;

  if (!registerIt)
  {
    RegDeleteRecursive( keySub, oleLanguageCLSID );
  }
  else
  {
    hr = hrRegCreateKeyEx(keySub, oleLanguageCLSID, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &key, NULL);
    if (FAILED(hr)) goto fail;

    hr = RegCreateValue( key, NULL, olePackageCLSID );
    if (FAILED(hr)) goto fail;

    hr = RegCreateValue( key, L"Name", g_languageName );
    if (FAILED(hr)) goto fail;

    RegCloseKey( key ); key = 0;
  }
  RegCloseKey( keySub ); keySub = 0;

  //release resources
fail:
  if (key)    RegCloseKey(key);
  if (keySub) RegCloseKey(keySub);
  if (keyVS)  RegCloseKey(keyVS);
  if (FAILED(hr)) return hr;

  return S_OK;
}

static HRESULT RegisterServer(in bool registerIt, in wchar_t* pszRegRoot)
{
  HRESULT hr = S_OK;

  HKEY    keyCLSID  = 0;
  HKEY    keyClass  = 0;
  HKEY    keyInProc = 0;

  wchar_t szRegistrationRoot[_MAX_PATH + 1];
  szRegistrationRoot[_MAX_PATH] = 0;
  szRegistrationRoot[0] = 0;

  //get clsid as string
  wchar_t oleCLSID[clsidSize];
  if (StringFromGUID2(g_languageCLSID, oleCLSID, clsidLen) == 0)
  { hr = E_FAIL; goto fail; }

  //get dll path
  wchar_t dllPath[MAX_PATH+1];
  DWORD dwLen = ::GetModuleFileName(g_instance, dllPath, MAX_PATH);
  if (dwLen == 0)
  {
    hr = HRESULT_FROM_WIN32(::GetLastError());
    goto fail;
  }
  if (dwLen == MAX_PATH)
  {
    hr = HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
    goto fail;
  }
 
  //open CLSID key
  if (NULL == pszRegRoot)
    hr = GetRegistrationRoot(szRegistrationRoot, _MAX_PATH);
  else
    hr = StringCchCopy(szRegistrationRoot, _MAX_PATH, pszRegRoot);
  if (FAILED(hr)) goto fail;

  const size_t clsidRootSize = _MAX_PATH + 11;
  wchar_t szClsidRoot[clsidRootSize];
  hr = StringCchCopy( szClsidRoot, clsidRootSize, szRegistrationRoot );
  if (FAILED(hr)) goto fail;
  hr = StringCchCat ( szClsidRoot, clsidRootSize, L"\\CLSID" );
  if (FAILED(hr)) goto fail;

  hr = hrRegOpenKeyEx(HKEY_LOCAL_MACHINE, szClsidRoot, 0, KEY_WRITE, &keyCLSID);
  if (FAILED(hr)) goto fail;

  if (!registerIt)
  {
    //delete it
    hr = RegDeleteRecursive( keyCLSID, oleCLSID );
    if (FAILED(hr)) goto fail;
  }
  else
  {
    //create it
    hr = hrRegCreateKeyEx(keyCLSID, oleCLSID, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &keyClass, NULL);
    if (FAILED(hr)) goto fail;

    RegCreateValue( keyClass, NULL, g_languageName);
    
    RegCreateValue( keyClass, L"InprocServer32", dllPath );
    RegCreateValue( keyClass, L"ThreadingModel", L"Free" );
  }

  
fail:
  if (keyInProc) RegCloseKey(keyInProc);
  if (keyClass)  RegCloseKey(keyClass);
  if (keyCLSID)  RegCloseKey(keyCLSID);

  //register language service with VS.
  if (szRegistrationRoot[0] != 0
	  && (SUCCEEDED(hr) || (registerIt == false)))
  {
     hr = RegisterService(szRegistrationRoot, babelPackageCLSID7, registerIt );
  }

  if (FAILED(hr)) return hr;

  return S_OK;
}


/*---------------------------------------------------------
  Registry helpers
-----------------------------------------------------------*/
static HRESULT RegCreateValue( in HKEY key, in const wchar_t* name, in const wchar_t* value )
{
  DWORD dwDataSize = NULL==value ? 0 : sizeof(wchar_t)*(lstrlenW(value)+1);
  LONG lRetCode = RegSetValueEx( key, name, 0, REG_SZ, 
                        reinterpret_cast<const BYTE*>(value), 
                        dwDataSize );
  return HRESULT_FROM_WIN32( lRetCode );
}

static HRESULT RegCreateDwordValue( in HKEY key, in const wchar_t* name, in DWORD value )
{
  LONG lRetCode = RegSetValueEx( key, name, 0, REG_DWORD, 
                        reinterpret_cast<const BYTE*>(&value), sizeof(value) );
  return HRESULT_FROM_WIN32( lRetCode );
}

static HRESULT RegDeleteRecursive( in HKEY keyRoot, in const wchar_t* path )
{
  HRESULT hr;
  HKEY    key       = 0;
  ULONG   subKeys   = 0;
  ULONG   maxKeyLen = 0;

  ULONG    currentKey= 0;
  wchar_t* keyName   = NULL;
  
  hr = hrRegOpenKeyEx( keyRoot, path, 0, (KEY_READ & SECURE_ACCESS), &key );
  if (FAILED(hr)) goto fail;

  LONG lRetCode = RegQueryInfoKey( key, NULL, NULL, NULL,
                                &subKeys, &maxKeyLen, 
                                NULL, NULL, NULL, NULL, NULL, NULL );
  if ( ERROR_SUCCESS != lRetCode )
  {
      hr = HRESULT_FROM_WIN32(lRetCode);
      goto fail;
  }
  if (subKeys == 0) goto fail;

  keyName = (wchar_t*) malloc(sizeof(wchar_t) * (maxKeyLen+1) );
  if (keyName == NULL) 
  {
	  hr = E_OUTOFMEMORY;
	  goto fail;
  }

  for (currentKey = 0; currentKey < subKeys; currentKey++)
  {
    ULONG keyLen;

    keyLen = maxKeyLen+1;
    lRetCode = RegEnumKeyEx( key, currentKey, keyName, &keyLen, NULL, NULL, NULL, NULL );
    if ( ERROR_SUCCESS == lRetCode )
    {
      RegDeleteRecursive(key,keyName);
    }
    else 
    {
        hr = HRESULT_FROM_WIN32(lRetCode);
        ASSERT(0);
    }
  }

fail:
  lRetCode = RegDeleteKey( keyRoot, path );
  if ( SUCCEEDED(hr) && (ERROR_SUCCESS != lRetCode) )
    hr = HRESULT_FROM_WIN32(lRetCode);
  if (key) RegCloseKey(key);
  if (keyName) free(keyName);
  return hr;
}


static HRESULT GetRegistrationRoot(wchar_t* pszRegistrationRoot, ULONG cchBuffer /* = _MAX_PATH*/)
{
  if ( FAILED(StringCchCopy(pszRegistrationRoot, cchBuffer, regPathVS_NoVersion)) )
      return E_UNEXPECTED;

  // this is Environment SDK specific
  // we check for  EnvSdk_RegKey environment variable to
  // determine where to register
  DWORD cchDefRegRoot = lstrlen(regPathVS_NoVersion);
  DWORD cchFreeBuffer = cchBuffer - cchDefRegRoot - 1;
  DWORD cchEnvVarRead = GetEnvironmentVariable(
    /* LPCTSTR */ TEXT("EnvSdk_RegKey"),               // environment variable name
    /* LPTSTR  */ &pszRegistrationRoot[cchDefRegRoot],// buffer for variable value
    /* DWORD   */ cchFreeBuffer);// size of buffer
  if (cchEnvVarRead >= cchFreeBuffer)
    return E_UNEXPECTED;
  // If the environment variable does not exist then we must use 
  if (0 == cchEnvVarRead)
  {
    HRESULT hr = StringCchCopy( pszRegistrationRoot, cchBuffer, regPathVS_NoVersion);
    if (FAILED(hr)) return hr;
    hr = StringCchCat ( pszRegistrationRoot, cchBuffer, vsCurrentVersion );
    if (FAILED(hr)) return hr;
  }

  return S_OK;
}