#pragma once

class SimpleLog;
class CLoginDataProvider
{
 public:
    CLoginDataProvider(LPCSTR sessionID, LPCSTR pin);
    std::string getPin();
    std::string getSessionID();
 private:
    std::string mPin;
    std::string mSessionID;
};

class CSessionStatusListener : public IO2GSessionStatus
{
 public:
    /** Constructor.*/
    CSessionStatusListener(SimpleLog *log, IO2GSession *session, CLoginDataProvider *loginDataProvider);
    ~CSessionStatusListener();
    /** On session status change*/
    void onSessionStatusChanged(O2GSessionStatus status);
    /** On login failed event*/
    void onLoginFailed(const char *error);

    // Thread safe implementation.
    long addRef();
    long release();

    HANDLE getStatusEvent()
    {
        return mhSyncEvent;
    }

    HANDLE getStopEvent()
    {
        return mhStopEvent;
    }

    HANDLE getConnectedEvent()
    {
        return mhConnectedEvent;
    }
 private:
    /** Log string to file.*/
    void logString(LPCSTR logmessage);

    volatile LONG dwRef;
    HANDLE mhSyncEvent;
    HANDLE mhStopEvent;
    HANDLE mhConnectedEvent;
    IO2GSession *mSession;
    CLoginDataProvider  *mLoginDataProvider;
    SimpleLog *mLog;
};

