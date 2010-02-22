
/***************************************************************************
         Copyright (c) Microsoft Corporation, All rights reserved.             
    This code sample is provided "AS IS" without warranty of any kind, 
    it is not recommended for use in a production environment.
***************************************************************************/

#include "common.h"
#include <stdio.h>
#include <stdarg.h>

/*---------------------------------------------------------
  Reference counting
-----------------------------------------------------------*/
ULONG IncRefCount( inout ULONG* refCount )
{
  return (ULONG)InterlockedIncrement( (LONG*)refCount );
}

ULONG DecRefCount( inout ULONG* refCount )
{
  return (ULONG)InterlockedDecrement( (LONG*)refCount );
}



/*---------------------------------------------------------
  Debugging
-----------------------------------------------------------*/
static const wchar_t*   g_traceModuleName = NULL;
static CRITICAL_SECTION g_traceGuard;
static DWORD            g_traceThreadID;

void Trace( const wchar_t* format, ... )
{
  va_list args;
  va_start( args, format );

  EnterCriticalSection( &g_traceGuard );

  if (g_traceModuleName)
  {
    OutputDebugString(g_traceModuleName);
    OutputDebugString( L": " );
  }

  const size_t bufferSize = 1024;
  wchar_t buffer[bufferSize];
  
  DWORD threadID = GetCurrentThreadId();
  if (threadID != g_traceThreadID)
  {
    if (0 == swprintf_s( buffer, bufferSize, L"tid 0x%x: ", threadID ))
    {
      buffer[bufferSize-1] = 0;
      OutputDebugString(buffer);
    }
  }

  if (0 == vswprintf_s( buffer, bufferSize, format, args ))
  {
    buffer[bufferSize-1] = 0;
    OutputDebugString(buffer);
  }

  OutputDebugString( L"\n" );

  LeaveCriticalSection( &g_traceGuard );

  va_end( args );
  return;
}


/*---------------------------------------------------------
  Track object life times
-----------------------------------------------------------*/
struct ObjectInfo
{
  ObjectInfo*    next;
  const wchar_t* name;
  IUnknown*      obj;
};

static ObjectInfo*      objectList = NULL;

void TraceDumpObjects()
{
  EnterCriticalSection( &g_traceGuard );
  
  TRACE(L"live objects:");
  for (ObjectInfo* objectInfo = objectList; objectInfo; objectInfo = objectInfo->next )
  {
    ULONG refCount = objectInfo->obj->AddRef();
    if (refCount > 0)
    {
      objectInfo->obj->Release();
      refCount--;
    }
    TRACE3( L"  0x%x, refcount %u, %s", objectInfo->obj, refCount, objectInfo->name); 
  }

  LeaveCriticalSection( &g_traceGuard );
}

void TraceInit( in const wchar_t* moduleName )
{
  InitializeCriticalSection( &g_traceGuard );
  g_traceThreadID     = GetCurrentThreadId();
  
  g_traceModuleName = moduleName;  
  objectList        = NULL;
  
}

void TraceDone()
{
  EnterCriticalSection( &g_traceGuard );

  if (objectList)
  {
    TRACE( L"found live objects:" );
    TraceDumpObjects();
  }

  ObjectInfo* objectInfo;
  ObjectInfo* objectNext;

  objectInfo = objectList;
  while (objectInfo)
  {
    objectNext = objectInfo->next;
    FREE(objectInfo);
    objectInfo = objectNext;
  }

  objectList = NULL;

  LeaveCriticalSection( &g_traceGuard );
  DeleteCriticalSection( &g_traceGuard );
}



void TraceCreate( const wchar_t* name, IUnknown* obj )
{
  if (!obj) return;

  ObjectInfo* objectInfo = ALLOC(ObjectInfo);
  if (!objectInfo)        
  {
    TRACE1(L"TraceCreate: creation of %s failed", name );
    return;
  }

  TRACE2( L"create %s at 0x%x", name, obj );

  EnterCriticalSection( &g_traceGuard );

  objectInfo->name = name;
  objectInfo->obj  = obj;
  objectInfo->next = objectList;
  objectList       = objectInfo;

  LeaveCriticalSection( &g_traceGuard );
}

void TraceDestroy( IUnknown* obj )
{
  EnterCriticalSection( &g_traceGuard );

  ObjectInfo* objectInfo;
  ObjectInfo* objectPrev = NULL;

  for (objectInfo = objectList; objectInfo; objectInfo = objectInfo->next)
  {
    if (objectInfo->obj == obj) break;
    objectPrev = objectInfo;
  }

  if (!objectInfo)
  {
    TRACE1( L"TraceDestroy: destroying unknown object at 0x%x", obj );
  }
  else
  {
    TRACE2( L"destroy %s at 0x%8x", objectInfo->name, objectInfo->obj );
    if (objectPrev)
      objectPrev->next = objectInfo->next;
    else
      objectList = objectInfo->next;

    FREE(objectInfo);
  }

  LeaveCriticalSection( &g_traceGuard );
}
