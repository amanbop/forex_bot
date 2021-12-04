// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//
#pragma once

/*
#if (_MSC_VER > 1310)
// Used template for transalate not security function(strcpy, sprintf .. etc)
// to security (strcpy_s).
#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES  1
#endif
*/

#ifdef WIN32

 #ifndef _WIN32_WINNT        // Allow use of features specific to Windows XP or later.
 #define _WIN32_WINNT 0x0501 // Change this to the appropriate value to target other versions of Windows.
 #endif

#include <windows.h>
#include <tchar.h>

#else  // crossplatform defines

#ifndef FALSE
#define FALSE               0
#endif
#ifndef TRUE
#define TRUE                1
#endif
#define WINAPI
typedef unsigned long       DWORD, *LPDWORD;
typedef int                 BOOL, *LPBOOL;
typedef unsigned char       BYTE, *LPBYTE;
typedef unsigned short      WORD, *LPWORD;
typedef int                 *LPINT;
typedef long                LONG, *LPLONG, HRESULT;
typedef void                *LPVOID, *HINTERNET, *HANDLE, *HMODULE;
typedef const void          *LPCVOID;
typedef unsigned int        UINT, *PUINT;
typedef char                *LPSTR, *LPTSTR, _TCHAR;
typedef const char          *LPCSTR, *LPCTSTR;
typedef wchar_t             WCHAR, *PWCHAR, *LPWCH, *PWCH, *LPWSTR, *PWSTR;
typedef const WCHAR         *LPCWCH, *PCWCH, *LPCWSTR, *PCWSTR;

#define _stricmp(A,B) strcasecmp(A,B)
#define sprintf_s(A, B, args...) sprintf(A, ## args)
#ifndef iPhone
#define _tmain      main
#endif

typedef double DATE;
typedef struct _SYSTEMTIME {
    unsigned short wYear;
    unsigned short wMonth;
    unsigned short wDayOfWeek;
    unsigned short wDay;
    unsigned short wHour;
    unsigned short wMinute;
    unsigned short wSecond;
    unsigned short wMilliseconds;
} SYSTEMTIME, *PSYSTEMTIME, *LPSYSTEMTIME;

#define GetTimeFormat(A,B,C,D,E,F) sprintf(E, "%02i:%02i:%02i", st.wHour, st.wMinute, st.wSecond);
#define GetDateFormat(A,B,C,D,E,F) sprintf(E, "%02i/%02i/%04i", st.wMonth, st.wDay, st.wYear);

#define _WINDEF_

#endif

#include <stdio.h>
#include <string>
#include <list>
#include <map>
#include <iostream>
#include <fstream>
#include <queue>
#include <cstring>

#include "../../../../../../include/Order2Go2.h"
#include "../../../../threadlib/include/threadlib.h"

// TODO: reference additional headers your program requires here

