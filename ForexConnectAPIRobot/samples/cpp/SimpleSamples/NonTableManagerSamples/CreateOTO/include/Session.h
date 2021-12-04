
#pragma once


class CSession : public AddRef<IO2GSessionStatus>
{
    static CSession* smInstance;

    IO2GSession* mSession;
    O2GSessionStatus mStatus;

    std::string mSubID;
    std::string mPin;

    CEventWithError mStatusEvent;
    O2GSessionStatus mWaitStatus;


    CSession();
    virtual ~CSession();
public:

    static CSession* getInstance();

    virtual void onSessionStatusChanged(O2GSessionStatus status);
    virtual void onLoginFailed(const char* error);

    void login(const char* user, const char* password, const char* url,
               const char* connection, const char* subsession,const char* pin);

    void logout()
    {
        mSession->logout();
    }

    bool wait(O2GSessionStatus status);

    operator IO2GSession*()
    {
        return mSession;
    }

    LPCSTR getError()
    {
        return mStatusEvent.getError().c_str();
    }
};

