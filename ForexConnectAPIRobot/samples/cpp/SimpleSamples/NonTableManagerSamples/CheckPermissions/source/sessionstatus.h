#pragma once
#include "stdafx.h"
#include "../../../../threadlib/include/winEmul.h"
#ifdef WIN32
 #define uni
#else
 #define uni win_emul
 #if defined(__APPLE__) && defined(__MACH__)
    #include <libkern/OSAtomic.h>
    #ifdef __LP64__
         #define InterlockedIncrement(A) (long)OSAtomicIncrement64((volatile int64_t *)A)
         #define InterlockedDecrement(A) (long)OSAtomicDecrement64((volatile int64_t *)A)
    #else
         #define InterlockedIncrement(A) (long)OSAtomicIncrement32((volatile int32_t *)A)
         #define InterlockedDecrement(A) (long)OSAtomicDecrement32((volatile int32_t *)A)
    #endif
  #else  // linux and similar
         #define InterlockedIncrement(A) (long)__sync_add_and_fetch(A, 1L)
         #define InterlockedDecrement(A) (long)__sync_sub_and_fetch(A, 1L)
  #endif
#endif

/** Status listener for the sessions. */

class SessionStatusListener : public IO2GSessionStatus
{
 private:
    long mRefCount;
    /** Sub session identifier. */
    std::string mSubSessionID;
    /** Pin code. */
    std::string mPin;
    /** The last session status code received. */
    IO2GSessionStatus::O2GSessionStatus mStatusCode;
    /** Flag indicating what error happened. */
    bool mHasError;
    /** Error. */
    std::string mError;
    /** Flag indicating whether sessions must be printed. */
    bool mPrintSubsessions;
    /** Session object. */
    IO2GSession *mSession;
    /** Event handle. */
    HANDLE mEvent;
 protected:
    /** Destructor. */
    ~SessionStatusListener();

 public:
    /** Constructor.

        @param session          Session to listen
        @param printSubsessions Print sub sessions
        @param subSessionID     Identifier of the subsession or NULL in case
                                no subsession selector is expected
        @param pin              Pin code or NULL in case no pin code request is expected
      */
    SessionStatusListener(IO2GSession *session, bool printSubsessions, const char *subSessionID = 0, const char *pin = 0);

    /** Increase reference counter. */
    virtual long addRef();

    /** Decrease reference counter. */
    virtual long release();

    /** Callback called when login has been failed. */
    virtual void  onLoginFailed (const char *error);

    /** Callback called when session status has been changed. */
    virtual void  onSessionStatusChanged(IO2GSessionStatus::O2GSessionStatus status);

    /** Get the session status. */
    IO2GSessionStatus::O2GSessionStatus getStatusCode() const;

    /** Check whether error happened. */
    bool wasError() const;

    /** Reset error information (use before login). */
    void resetError();

    /** Get error. */
    const char *getError() const;

    /** Get event handle to monitor the status. */
    HANDLE getEvent() const;
};

