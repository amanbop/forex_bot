#include "responselistener.h"
#include "stdafx.h"

ResponseListener::ResponseListener()
{
    mRefCount = 1;
    mEvent = uni::CreateEvent(0, FALSE, FALSE, 0);
}

ResponseListener::~ResponseListener()
{
    uni::CloseHandle(mEvent);
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
void ResponseListener::setRequest(const char *requestId)
{
    mRequestID = requestId;
}

/** Request execution completed data handler. */
void ResponseListener::onRequestCompleted(const char *requestId, IO2GResponse  *response)
{
    if (requestId && mRequestID == requestId)
    {
        std::cout << "Request complete ID " << requestId << std::endl;
        uni::SetEvent(mEvent);
    }
}

/** Request execution failed data handler. */
void ResponseListener::onRequestFailed(const char *requestId , const char *error)
{
    if (requestId && mRequestID == requestId)
    {
        std::cout << "Request failed ID " << requestId << " :" << error << std::endl;
        uni::SetEvent(mEvent);
    }
}

/** Request update data received data handler. */
void ResponseListener::onTablesUpdates(IO2GResponse *data)
{
}

