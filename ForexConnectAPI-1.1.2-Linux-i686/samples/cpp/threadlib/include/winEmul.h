/* Copyright 2011 Forex Capital Markets LLC

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use these files except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#ifndef WINEMUL_H
#define WINEMUL_H

#ifdef WIN32

#ifdef PTHREADS

#ifdef _M_IX86
#define _X86_
#endif
#include <windef.h>

#else
#include <windows.h>
#endif

#ifndef uni
#define uni
#endif

#else

#ifndef PTHREADS
#define PTHREADS
#endif

#ifndef uni
#define uni win_emul
#endif

#define WINBASEAPI
#define CONST const

#define STATUS_WAIT_0   ((DWORD)0x00000000L)
#define STATUS_TIMEOUT  ((DWORD)0x00000102L)

#ifndef _WINDEF_

#define WINAPI
#ifndef FALSE
#define FALSE               0
#endif
typedef unsigned long DWORD;
typedef int BOOL;
typedef wchar_t WCHAR; // Note: sizeof(wchar_t) == 4!
typedef CONST WCHAR* LPCWSTR;
typedef const char* LPCSTR;
typedef void* HANDLE;

#endif

static const DWORD MAXIMUM_WAIT_OBJECTS(32);

#endif

#ifndef _WINBASE_

#define WAIT_TIMEOUT    STATUS_TIMEOUT
#define WAIT_OBJECT_0   ((STATUS_WAIT_0 ) + 0 )
#define CREATE_SUSPENDED 0x00000004

#ifndef INFINITE
#define INFINITE        0xFFFFFFFF    // Infinite timeout
#endif//INFINITE

typedef struct _SECURITY_ATTRIBUTES
{
    BOOL bInheritHandle;
}SECURITY_ATTRIBUTES;
typedef SECURITY_ATTRIBUTES* LPSECURITY_ATTRIBUTES;

#ifndef CreateEvent
#define CreateEvent  CreateEventW
#endif

#ifndef OpenEvent
#define OpenEvent  OpenEventW
#endif

#endif


/*#ifdef __cplusplus
# define CEXTERN extern "C"
#else*/
#define CEXTERN extern
//#endif

#ifdef PTHREADS
namespace win_emul
{

CEXTERN DWORD WINAPI WaitForSingleObject(HANDLE hHandle, DWORD dwMilliseconds);
CEXTERN DWORD WINAPI WaitForMultipleObjects    (
    DWORD nCount, CONST HANDLE *lpHandles, BOOL bWaitAll, DWORD dwMilliseconds);
CEXTERN BOOL WINAPI SetEvent(HANDLE hEvent);
CEXTERN BOOL WINAPI ResetEvent(HANDLE hEvent);
CEXTERN BOOL WINAPI PulseEvent(HANDLE hEvent);  // Used in CRsEvent.cpp

CEXTERN HANDLE WINAPI CreateEventW(
    LPSECURITY_ATTRIBUTES lpEventAttributes, BOOL bManualReset, BOOL bInitialState, LPCWSTR lpName);

CEXTERN HANDLE WINAPI OpenEventW(DWORD dwDesiredAccess, BOOL bInheritHandle, LPCWSTR lpName);
CEXTERN BOOL WINAPI CloseHandle(HANDLE hObject);

CEXTERN DWORD WINAPI GetTickCount();
CEXTERN void WINAPI Sleep(DWORD dwMilliseconds);
}

#endif

namespace threading
{

inline CEXTERN DWORD WINAPI WaitForSingleObject(HANDLE hHandle, DWORD dwMilliseconds)
    {return uni::WaitForSingleObject(hHandle, dwMilliseconds);}
inline CEXTERN DWORD WINAPI WaitForMultipleObjects(DWORD nCount, CONST HANDLE *lpHandles, BOOL bWaitAll, DWORD dwMilliseconds)
    {return uni::WaitForMultipleObjects (nCount,lpHandles, bWaitAll, dwMilliseconds);}
inline CEXTERN BOOL WINAPI SetEvent(HANDLE hEvent)
    {return uni::SetEvent(hEvent);}
inline CEXTERN BOOL WINAPI ResetEvent(HANDLE hEvent)
    {return uni::ResetEvent(hEvent);}
inline CEXTERN BOOL WINAPI PulseEvent(HANDLE hEvent)
    {return uni::PulseEvent(hEvent);}

inline CEXTERN HANDLE WINAPI CreateEventW(LPSECURITY_ATTRIBUTES lpEventAttributes, BOOL bManualReset, BOOL bInitialState, LPCWSTR lpName)
    {return uni::CreateEventW(lpEventAttributes, bManualReset, bInitialState, lpName);}

inline CEXTERN HANDLE WINAPI OpenEventW(DWORD dwDesiredAccess, BOOL bInheritHandle, LPCWSTR lpName)
    {return uni::OpenEventW(dwDesiredAccess, bInheritHandle, lpName);}
inline CEXTERN BOOL WINAPI CloseHandle(HANDLE hObject)
    {return uni::CloseHandle(hObject);}

inline CEXTERN DWORD WINAPI GetTickCount()
    {return uni::GetTickCount();}
inline CEXTERN void WINAPI Sleep(DWORD dwMilliseconds)
    {uni::Sleep(dwMilliseconds);}
}

#endif

