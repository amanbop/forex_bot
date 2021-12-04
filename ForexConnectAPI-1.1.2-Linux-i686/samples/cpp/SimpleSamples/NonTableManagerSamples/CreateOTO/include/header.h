
#pragma once

#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>
#include <algorithm>
#include <map>


#ifdef WIN32

 #ifndef _WIN32_WINNT        // Allow use of features specific to Windows XP or later.
 #define _WIN32_WINNT 0x0501 // Change this to the appropriate value to target other versions of Windows.
 #endif

#include <windows.h>

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

#endif // crossplatform defines

#include "../../../../../../include/Order2Go2.h"
#include "../../../../threadlib/include/threadlib.h"

#ifndef NULL
#define NULL 0
#endif

inline void mySleep(unsigned int msec)
{
    #ifdef WIN32
    Sleep((DWORD)msec);
    #else
    usleep(1000 * msec);
    #endif
}


template <typename TBase>
class AddRef : public TBase
{
    long mRef;

protected:
    typedef AddRef<TBase> _TBase;

    AddRef()
    {
        mRef = 0;
    }
    virtual ~AddRef()
    {
    }

public:
    virtual long addRef()
    {
        return InterlockedIncrement(&mRef);
    }

    virtual long release()
    {
        long ref = InterlockedDecrement(&mRef);
        if (ref == 0)
            delete this;
        return ref;
    }
};


class CEvent
{
    HANDLE mHandle;

public:

    CEvent()
    {
        mHandle = uni::CreateEvent(NULL, FALSE, FALSE, NULL);
    }

    void reset()
    {
        uni::ResetEvent(mHandle);
    }

    virtual bool wait() const
    {
        uni::WaitForSingleObject(mHandle, (UINT)-1);
        return true;
    }
    void signal()
    {
        uni::SetEvent(mHandle);
    }
};

class CEventWithError : public CEvent
{
    std::string mError;
public:

    virtual bool wait() const
    {
        CEvent::wait();
        return mError.empty();
    }

    void setError(const char* error)
    {
        mError = error == NULL ? "" : error;
    }
    const std::string& getError() const
    {
        return mError;
    }
};

