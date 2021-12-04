#include "../../../../../../include/Order2Go2.h"
#include "ResponseListener.h"
#include "stdio.h"

/** Constructor. */
ResponseListener::ResponseListener()
{
    mEvent = uni::CreateEvent(0, FALSE, FALSE, 0);
    mRefCount = 1;
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

void ResponseListener::onRequestCompleted(const char *requestId, IO2GResponse *response)
{
    printf("Request completed.\nrequestID=%s", requestId);
    uni::SetEvent(mEvent);
}

void ResponseListener::onRequestFailed(const char *requestId, const char *error)
{
    printf("Request failed.\nrequestID=%s; error=%s", requestId, error);
    uni::SetEvent(mEvent);
}

void ResponseListener::onTablesUpdates(IO2GResponse *tablesUpdates)
{
}

HANDLE ResponseListener::getEvent() const
{
    return mEvent;
}

