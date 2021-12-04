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
    mSuccess = false;
    mRefCount = 1;
    mResponseEvent = uni::CreateEvent(0, FALSE, FALSE, 0);
    mUpdateEvent = uni::CreateEvent(0, FALSE, FALSE, 0);
}

ResponseListener::~ResponseListener()
{
    mSession->release();
    uni::CloseHandle(mResponseEvent);
    uni::CloseHandle(mUpdateEvent);
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
    uni::ResetEvent(mResponseEvent);
    uni::ResetEvent(mUpdateEvent);
    mSuccess = false;
}

/** Request execution completed data handler. */
void ResponseListener::onRequestCompleted(const char *requestId, IO2GResponse  *response)
{
    if (response != 0 && response->getType() == CreateOrderResponse && mRequestID == requestId)
    {
        std::cout << "Request complete. ID: " << requestId << std::endl;
        mSuccess = true;
        uni::SetEvent(mResponseEvent);
    }
}

/** Request execution failed data handler. */
void ResponseListener::onRequestFailed(const char *requestId , const char *error)
{
    if (requestId != 0 && mRequestID.length() > 0 && mRequestID == requestId)
    {
        std::cout << "Request failed. ID: " << requestId << " : " << error << std::endl;
        mSuccess = false;
        uni::SetEvent(mResponseEvent);
    }
}

/** Request update data received data handler. */
void ResponseListener::onTablesUpdates(IO2GResponse *data)
{
    if (data)
    {
        IO2GResponseReaderFactory *factory = mSession->getResponseReaderFactory();
        IO2GTablesUpdatesReader *reader = factory->createTablesUpdatesReader(data);
        if (reader)
        {
            for (int i = 0; i < reader->size(); ++i)
            {
                if (reader->getUpdateType(i) == Insert)
                {
                    if (mOrderID.length() != 0)
                    {
                        IO2GOrderRow *orderRow = reader->getOrderRow(i);
                        if (orderRow)
                        {
                            if (mOrderID == orderRow->getOrderID())
                            {
                                std::cout << "Orders table updated: Added order. ID: " << mOrderID << std::endl;
                                uni::SetEvent(mUpdateEvent);
                                break;
                            }
                            orderRow->release();
                        }
                    }
                }
            }
            reader->release();
        }
        factory->release();
    }
}

