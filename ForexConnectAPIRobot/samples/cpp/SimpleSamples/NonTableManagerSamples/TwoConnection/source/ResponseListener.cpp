#include "stdafx.h"
#include "SimpleLog.h"
#include "ResponseQueue.h"
#include "ResponseListener.h"

ResponseListener::ResponseListener(SimpleLog *log,
                                   ResponseQueue *queue)
{
    mResponse = uni::CreateEvent(NULL, FALSE, FALSE, NULL);
    mQueue = queue;
    mLog = log;
    dwRef = 0;
}

ResponseListener::~ResponseListener()
{
    uni::CloseHandle(mResponse);
}
/** */
void ResponseListener::onRequestCompleted(const char * requestId, IO2GResponse  *response)
{
    response->addRef();
    mQueue->push(response);
    uni::SetEvent(mResponse);
}

void ResponseListener::onRequestFailed(const char *requestID , const char *error)
{
    char buffer[1024];
    sprintf_s(buffer, sizeof(buffer), "Request failed requestID=%s error=%s", requestID, error);
    mLog->logString(buffer);
}

void ResponseListener::onTablesUpdates(IO2GResponse *data)
{
    mLog->addStringToLog("Data recieved");
    mLog->dumpLog();
    data->addRef();
    mQueue->push(data);
    uni::SetEvent(mResponse);
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

