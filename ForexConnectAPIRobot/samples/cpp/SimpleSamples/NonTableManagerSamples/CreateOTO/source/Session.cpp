

#include "../include/header.h"
#include "../include/Session.h"

CSession* CSession::smInstance = NULL;

CSession* CSession::getInstance()
{
    if (smInstance == NULL)
        smInstance = new CSession;
    return smInstance;
}

CSession::CSession()
{
    mSession = CO2GTransport::createSession();
    mSession->subscribeSessionStatus(this);
}

CSession::~CSession()
{
    mSession->unsubscribeSessionStatus(this);
    smInstance = NULL;
    mSession->release();
}

void CSession::login(const char* user, const char* password, const char* url,
                     const char* connection, const char* subsession,const char* pin)
{
    mSession->login(user, password, url, connection);

    mSubID = subsession == NULL ? "" : subsession;
    mPin = pin == NULL ? "" : pin;
}

void CSession::onSessionStatusChanged(O2GSessionStatus status)
{
    mStatus = status;
    switch(status)
    {
        case TradingSessionRequested:
            mSession->setTradingSession(mSubID.c_str(), mPin.c_str());
        break;
    }

    if (status == mWaitStatus)
        mStatusEvent.signal();
}

void CSession::onLoginFailed(const char* error)
{
    mStatusEvent.setError(error);
    mStatusEvent.signal();
}

bool CSession::wait(O2GSessionStatus status)
{
    if (mStatus == status)
        return true;

    mWaitStatus = status;
    return mStatusEvent.wait();
}

