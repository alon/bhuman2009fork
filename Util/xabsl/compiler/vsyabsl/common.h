
/***************************************************************************
         Copyright (c) Microsoft Corporation, All rights reserved.             
    This code sample is provided "AS IS" without warranty of any kind, 
    it is not recommended for use in a production environment.
***************************************************************************/

#ifndef common_h
#define common_h

/*---------------------------------------------------------
  Standard includes
-----------------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <malloc.h>
#include <windows.h>

#include "text.h"
#include "babelservice.h"

/*---------------------------------------------------------
  Debugging
-----------------------------------------------------------*/
void  Trace( const wchar_t* format, ... );

void  TraceInit( const wchar_t* moduleName );
void  TraceDone();
void  TraceCreate( const wchar_t* name, IUnknown* obj );
void  TraceDestroy( IUnknown* obj );
void  TraceDumpObjects();

#ifdef NDEBUG
# define ASSERT(x)
# define TRACE(s)       
# define TRACE1(s,x)     
# define TRACE2(s,x,y)   
# define TRACE3(s,x,y,z) 

# define TRACE_INIT(name)
# define TRACE_DONE
# define TRACE_CREATE(name,obj)  
# define TRACE_DESTROY(obj)      
# define TRACE_LIVE_OBJECTS
#else
# define ASSERT(x)       assert(x)
# define TRACE(s)        Trace(s)
# define TRACE1(s,x)     Trace(s,x)
# define TRACE2(s,x,y)   Trace(s,x,y)
# define TRACE3(s,x,y,z) Trace(s,x,y,z)

# define TRACE_INIT(name)        TraceInit(name)
# define TRACE_DONE              TraceDone()
# define TRACE_CREATE(name,obj)  TraceCreate(name,obj)
# define TRACE_DESTROY(obj)      TraceDestroy(obj)
# define TRACE_LIVE_OBJECTS      TraceDumpObjects()
#endif


/*---------------------------------------------------------
  IDL
-----------------------------------------------------------*/
#define in
#define out
#define inout


/*---------------------------------------------------------
  Argument checking
-----------------------------------------------------------*/
#define OUTARG(p)       ASSERT(p); if (!(p)) return E_INVALIDARG; else *(p) = 0;
#define OUTVAR(v)       ASSERT(v); if (!(v)) return E_INVALIDARG; else VariantInit(v);

#define INOUTARG(p)     ASSERT(p); if (!(p)) return E_INVALIDARG;
#define REFARG(p)       ASSERT(p); if (!(p)) return E_INVALIDARG;


/*---------------------------------------------------------
  Memory management
-----------------------------------------------------------*/
#ifdef DELETE
#undef DELETE
#endif

template <typename T>
T* BabelAlloc(size_t count)
{
    if (count >= 0xFFFFFFFF / sizeof(T))
        return NULL;

    return (T*)malloc(sizeof(T)*count);
}

#define ALLOC(tp)       (tp*)malloc(sizeof(tp))
#define NALLOC(tp,n)    BabelAlloc<tp>(n)

#define FREE(p)         if (p) { free((void*)(p)); (p) = NULL; }
#define DELETE(p)       if (p) { delete (p); (p) = NULL; }
#define RELEASE(p)      if (p) { (p)->Release(); (p) = NULL; }
#define REMOVE(p)       if (p) { while ((p)->Release() != 0) ; (p) = NULL; }
#define ADDREF(p)       ASSERT(p); if (p) { (p)->AddRef(); }

/*---------------------------------------------------------
  Strings size and utilities
-----------------------------------------------------------*/
#define MAXSTR		255
#define MAXPATH		255
#define MAXNAME		99
inline void bstrFree(BSTR& bstr) { SysFreeString(bstr); bstr = NULL; }

/*---------------------------------------------------------
  Reference counting
-----------------------------------------------------------*/
ULONG IncRefCount( inout ULONG* refCount );
ULONG DecRefCount( inout ULONG* refCount );

extern ULONG  g_dllRefCount;

/*---------------------------------------------------------
  COM
-----------------------------------------------------------*/
#define WINERROR	HRESULT_FROM_WIN32


#endif