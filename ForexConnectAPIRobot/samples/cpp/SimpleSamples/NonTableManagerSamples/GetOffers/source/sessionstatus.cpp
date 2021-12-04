#include <string>
#include "../../../../../../include/Order2Go2.h"
#include "sessionstatus.h"

/** Constructor. */
SessionStatusListener::SessionStatusListener(IO2GSession *session, bool printSubsessions, const char *subSessionID, const char *pin)
{
    if (subSessionID != 0)
        mSubSessionID = subSessionID;
    else
        mSubSessionID = "";
    if (pin != 0)
        mPin = pin;
    else
        mPin = "";
    mSession = session;
    mSession->addRef();
    mHasError = false;
    mError = "";
    mPrintSubsessions = printSubsessions;
    mRefCount = 1;
    mStatusCode = IO2GSessionStatus::Disconnected;
    mEvent = uni::CreateEvent(0, FALSE, FALSE, 0);
}

/** Destructor. */
SessionStatusListener::~SessionStatusListener()
{
    mSession->release();
    mSubSessionID.clear();
    mPin.clear();
    mError.clear();
    uni::CloseHandle(mEvent);
}

/** Increase reference counter. */
long SessionStatusListener::addRef()
{
    return InterlockedIncrement(&mRefCount);
}

/** Decrease reference counter. */
long SessionStatusListener::release()
{
    long rc = InterlockedDecrement(&mRefCount);
    if (rc == 0)
        delete this;
    return rc;
}

/** Callback called when login has been failed. */
void SessionStatusListener::onLoginFailed(const char *error)
{
    mHasError = true;
    mError = error;
    uni::SetEvent(mEvent);
}

/** Callback called when session status has been changed. */
void SessionStatusListener::onSessionStatusChanged(IO2GSessionStatus::O2GSessionStatus status)
{
    switch (status)
    {
    case    IO2GSessionStatus::Disconnected:
            printf("status::disconnected\n");
            break;
    case    IO2GSessionStatus::Connecting:
            printf("status::connecting\n");
            break;
    case    IO2GSessionStatus::TradingSessionRequested:
            printf("status::trading session requested\n");
            break;
    case    IO2GSessionStatus::Connected:
            printf("status::connected\n");
            break;
    case    IO2GSessionStatus::Reconnecting:
            printf("status::reconnecting\n");
            break;
    case    IO2GSessionStatus::Disconnecting:
            printf("status::disconnecting\n");
            break;
    case    IO2GSessionStatus::SessionLost:
            printf("status::session lost\n");
            break;
    }

    if (status == IO2GSessionStatus::TradingSessionRequested)
    {
        IO2GSessionDescriptorCollection *descriptors = mSession->getTradingSessionDescriptors();
        bool found = false;
        if (descriptors != 0)
        {
            if (mPrintSubsessions)
                printf("descriptors available:\n");
            int i;
            for (i = 0; i < descriptors->size(); i++)
            {
                IO2GSessionDescriptor *descriptor = descriptors->get(i);

                if (mPrintSubsessions)
                    printf("  id:='%s' name='%s' description='%s' %s\n", descriptor->getID(), descriptor->getName(), descriptor->getDescription(), descriptor->requiresPin() ? "requires pin" : "");

                if (strcmp(mSubSessionID.c_str(), descriptor->getID()) == 0)
                    found = true;

                descriptor->release();
            }
            descriptors->release();
        }
        if (!found)
        {
            onLoginFailed("The specified sub session identifier is not found");
        }
        else
        {
            mSession->setTradingSession(mSubSessionID.c_str(), mPin.c_str());
        }
    }
    mStatusCode = status;
    uni::SetEvent(mEvent);
}

/** Get the session status. */
IO2GSessionStatus::O2GSessionStatus SessionStatusListener::getStatusCode() const
{
    return mStatusCode;
}

/** Check whether error happened. */
bool SessionStatusListener::wasError() const
{
    return mHasError;
}

/** Reset error information (use before login). */
void SessionStatusListener::resetError()
{
    mHasError = false;
    mError = "";
}

/** Get error. */
const char *SessionStatusListener::getError() const
{
    return mError.c_str();
}

HANDLE SessionStatusListener::getEvent() const
{
    return mEvent;
}

