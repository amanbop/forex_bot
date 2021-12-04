#pragma once

#include "util.h"

class ResponseListener : public IO2GResponseListener
{
public:
    /** Constructor. */
    ResponseListener();

    /** Destructor. */
    virtual ~ResponseListener();

    /** Increase reference counter. */
    virtual long addRef();

    /** Decrease reference counter. */
    virtual long release();

    virtual void onRequestCompleted(const char *requestId, IO2GResponse *response = 0);

    virtual void onRequestFailed(const char *requestId, const char *error);

    virtual void onTablesUpdates(IO2GResponse *tablesUpdates);

    HANDLE getEvent() const;

    /** Gets order response. */
    IO2GResponse *getOrdersResponse();

private:
    long mRefCount;
    HANDLE mEvent;
    IO2GResponse *mLastOrdersResponse;
};

