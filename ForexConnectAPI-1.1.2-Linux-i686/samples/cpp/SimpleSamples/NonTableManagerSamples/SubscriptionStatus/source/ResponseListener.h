#pragma once
#include "stdafx.h"
class SimpleLog;
class ResponseListener : public IO2GResponseListener
{
 public:
    ResponseListener(SimpleLog *log);
    ~ResponseListener();
    /** */
    void setRequest(const char *requestID);
    /** */
    void onRequestCompleted(const char * requestId, IO2GResponse  *response = 0);
    void onRequestFailed(const char *requestId , const char *error);
    void onTablesUpdates(IO2GResponse *data);
    HANDLE getReponseEvent()
    {
        return mResponse;
    };
    HANDLE getFailureEvent()
    {
        return mRequestFailed;
    };
    // Thread safe implementation.
    long addRef();
    long release();
    IO2GResponse *getLastRespnonse();
 private:
    SimpleLog *mLog;
    volatile LONG dwRef;

    std::string mRequestID;
    IO2GResponse *mLastResponse;
    HANDLE mResponse;
    HANDLE mRequestFailed;
};


