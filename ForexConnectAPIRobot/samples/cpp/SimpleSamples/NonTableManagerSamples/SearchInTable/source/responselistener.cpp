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
void ResponseListener::setRequest(const char *requestId, const char *orderID)
{
    mRequestID = requestId;
    mOrderID = orderID;
    uni::ResetEvent(mResponseEvent);
    uni::ResetEvent(mUpdateEvent);
}

/** Request execution completed data handler. */
void ResponseListener::onRequestCompleted(const char *requestId, IO2GResponse  *response)
{
    if (response != 0)
    {
        if (response->getType() == GetOrders && mRequestID == requestId)
        {
            IO2GResponseReaderFactory *responseReaderFactory = mSession->getResponseReaderFactory();
            bool bFound = false;

            IO2GOrdersTableResponseReader *responseReader = responseReaderFactory->createOrdersTableReader(response);
            responseReaderFactory->release();

            for (int i = 0; i < responseReader->size(); ++i)
            {
                IO2GOrderRow *orderRow = responseReader->getRow(i);
                if (mOrderID == orderRow->getOrderID())
                {
                    std::cout << "Order ID " << mOrderID << " found!" << std::endl;
                    std::cout << "Status: " << orderRow->getStatus() << std::endl;
                    std::cout << "Amount: " << orderRow->getAmount() << std::endl;
                    bFound = true;
                }
                orderRow->release();
                if (bFound)
                    break;
            }

            responseReader->release();
            if (!bFound)
                std::cout << "Order ID " << mOrderID << " not found!" << std::endl;

            uni::SetEvent(mResponseEvent);
        }
    }
}

/** Request execution failed data handler. */
void ResponseListener::onRequestFailed(const char *requestId , const char *error)
{

}

/** Request update data received data handler. */
void ResponseListener::onTablesUpdates(IO2GResponse *data)
{
}

