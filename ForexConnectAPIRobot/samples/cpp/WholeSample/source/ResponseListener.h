#pragma once
class SimpleLog;
class ResponseQueue;
class ResponseListener : public IO2GResponseListener
{
 public:
    ResponseListener(SimpleLog *log,
                     ResponseQueue *queue);
    ~ResponseListener();
    /** */
    void onRequestCompleted(const char * requestId, IO2GResponse  *response = 0);
    void onRequestFailed(const char *requestId , const char *error);
    void onTablesUpdates(IO2GResponse *data);
    HANDLE getReponseEvent()
    {
        return mResponse;
    };
    // Thread safe implementation.
    long addRef();
    long release();
 private:
    SimpleLog *mLog;
    volatile LONG dwRef;

    HANDLE mResponse;
    HANDLE mRequestFailed;
    ResponseQueue *mQueue;
};


