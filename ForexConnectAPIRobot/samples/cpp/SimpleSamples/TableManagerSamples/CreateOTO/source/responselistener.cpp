#include <math.h>
#include <time.h>
#include <string>
#include <vector>
#include <map>
#include "responselistener.h"
#include "stdafx.h"
#include <sstream>
#include <iomanip>

ResponseListener::ResponseListener(IO2GSession *session)
{
    mSession = session;
    mSession->addRef();
    mRefCount = 1;
    mRequestFailedEvent = uni::CreateEvent(0, FALSE, FALSE, 0);
}

ResponseListener::~ResponseListener()
{
    mSession->release();
    uni::CloseHandle(mRequestFailedEvent);
}

/** Increase reference counter. */
long ResponseListener::addRef()
{
    return InterlockedIncrement(&mRefCount);
}

/** Decrease reference counter. */
long ResponseListener::release()
{
    long rc = InterlockedDecrement(&mRefCount);
    if (rc == 0)
        delete this;
    return rc;
}

/** Set request. */
void ResponseListener::appendRequest(const char *requestId)
{
    ::threading::Mutex::Lock l(mMutex);
    mliRequestID.push_back(requestId);
}

/** Request execution completed data handler. */
void ResponseListener::onRequestCompleted(const char *requestId, IO2GResponse  *response)
{
}

/** Request execution failed data handler. */
void ResponseListener::onRequestFailed(const char *requestId , const char *error)
{
    if (requestId)
    {
        ::threading::Mutex::Lock l(mMutex);
        for (std::list<std::string>::iterator it = mliRequestID.begin(); it != mliRequestID.end(); ++it)
            if (*it == requestId)
            {
                mliRequestID.erase(it);
                std::cout << "Request failed ID " << requestId << " :" << std::endl;
                std::cout << error << std::endl;
                uni::SetEvent(mRequestFailedEvent);
                break;
            }
    }
}

/** Request update data received data handler. */
void ResponseListener::onTablesUpdates(IO2GResponse *data)
{
}

