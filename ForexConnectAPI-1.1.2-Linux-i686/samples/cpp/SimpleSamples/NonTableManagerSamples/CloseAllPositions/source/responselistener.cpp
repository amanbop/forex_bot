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
    pliTradeRows = new std::list<IO2GTradeRow *>;
    pvecRequestID = 0;
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
    mSuccess = false;
    uni::ResetEvent(mResponseEvent);
    uni::ResetEvent(mUpdateEvent);
}

void ResponseListener::setRequestVector(std::vector<std::string> *lpvecRequestID)
{
    pvecRequestID = lpvecRequestID;
    iRequestsProcessed = 0;
    mSuccess = false;
    uni::ResetEvent(mResponseEvent);
    uni::ResetEvent(mUpdateEvent);
}

/** Request execution completed data handler. */
void ResponseListener::onRequestCompleted(const char *requestId, IO2GResponse  *response)
{
    if (response != 0)
    {
        if (response->getType() == CreateOrderResponse)
        {
            for(unsigned i = 0; i < pvecRequestID->size(); ++i)
            {
                if ((*pvecRequestID)[i] == requestId)
                {
                    ++iRequestsProcessed;
                    std::cout << "Request complete. ID: " << requestId << std::endl;
                    break;
                }
            }
            if (iRequestsProcessed == pvecRequestID->size())
            {
                mSuccess = true;
                uni::SetEvent(mResponseEvent);
            }
        }
        if (response->getType() == GetTrades && mRequestID == requestId)
        {
            IO2GResponseReaderFactory *readerFactory = mSession->getResponseReaderFactory();
            IO2GTradesTableResponseReader *responseReader = readerFactory->createTradesTableReader(response);
            for (int i = 0; i < responseReader->size(); ++i)
                pliTradeRows->push_back(responseReader->getRow(i));
            mSuccess = true;
            mRequestID = "";
            uni::SetEvent(mResponseEvent);
        }
    }
}

/** Request execution failed data handler. */
void ResponseListener::onRequestFailed(const char *requestId , const char *error)
{
    std::cout << "Request failed. ID: " << requestId << " : " << error << std::endl;
    if (requestId)
    {
        bool bRequestFromVector = false;
        for (unsigned i = 0; i < pvecRequestID->size(); ++i)
        {
            if ((*pvecRequestID)[i] == requestId)
            {
                ++iRequestsProcessed;
                bRequestFromVector = true;
                break;
            }
        }
        if (bRequestFromVector)
        {
            if (iRequestsProcessed == pvecRequestID->size())
            {
                mSuccess = false;
                uni::SetEvent(mResponseEvent);
            }
        }
        else
        {
            if (mRequestID == requestId)
            {
                mSuccess = false;
                mRequestID = "";
                uni::SetEvent(mResponseEvent);
            }
        }
    }
}

/** Request update data received data handler. */
void ResponseListener::onTablesUpdates(IO2GResponse *data)
{
}

