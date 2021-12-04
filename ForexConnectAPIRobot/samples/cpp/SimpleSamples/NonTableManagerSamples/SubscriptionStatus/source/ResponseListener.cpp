#include "stdafx.h"
#include "SimpleLog.h"
#include "ResponseListener.h"

ResponseListener::ResponseListener(SimpleLog *log)
{
    mResponse = uni::CreateEvent(NULL, FALSE, FALSE, NULL);
    mRequestFailed = uni::CreateEvent(NULL, FALSE, FALSE, NULL);
    mLog = log;
    dwRef = 1;
    mLastResponse = NULL;
}

ResponseListener::~ResponseListener()
{
    uni::CloseHandle(mResponse);
    uni::CloseHandle(mRequestFailed);
    if (mLastResponse)
        mLastResponse->release();
}
/** */
void ResponseListener::setRequest(const char* requestID)
{
    mRequestID = requestID;
}
/** */
void ResponseListener::onRequestCompleted(const char * requestID, IO2GResponse *response)
{
    char buffer[1024];
    sprintf_s(buffer, sizeof(buffer), "Request completed requestID=%s", requestID);
    mLog->logString(buffer);
    if(mRequestID == requestID)
    {
        printf("Request completed\n");
        if (mLastResponse)
            mLastResponse->release();
        mLastResponse = response;
        mLastResponse ->addRef();
        uni::SetEvent(mResponse);
    }
}
IO2GResponse* ResponseListener::getLastRespnonse()
{
    mLastResponse->addRef();
    return mLastResponse;
}
void ResponseListener::onRequestFailed(const char *requestID, const char *error)
{
    char buffer[1024];
    sprintf_s(buffer, sizeof(buffer), "Request failed requestID=%s error=%s", requestID, error);
    mLog->logString(buffer);
    if (mRequestID == requestID)
    {
        printf("Request failed\n");
        uni::SetEvent(mRequestFailed);
    }
}

void ResponseListener::onTablesUpdates(IO2GResponse *data)
{
    mLog->addStringToLog("Data received");
    mLog->dumpLog();
    printf("Table updated\n");
    // uni::SetEvent(mResponse);
}

long ResponseListener::addRef()
{
    return InterlockedIncrement(&dwRef);
}

long ResponseListener::release()
{
    LONG dwRes = InterlockedDecrement(&dwRef);
    if (dwRes == 0)
        delete this;
    return dwRes;
}

