#include "stdafx.h"

/** Response listener class. */
class ResponseListener : public IO2GResponseListener
{
 private:
    long mRefCount;
    /** Session object. */
    IO2GSession *mSession;
    /** Response Event handle. */
    HANDLE mEvent;
    /** Request ID to wait. */
    std::string mRequestID;
 protected:
    /** Destructor. */
    virtual ~ResponseListener();

 public:
    /** Constructor */
    ResponseListener();
    /** Increase reference counter. */
    virtual long addRef();

    /** Decrease reference counter. */
    virtual long release();

    /** Set request. */
    void setRequest(const char *requestId);

    /** Request execution completed data handler. */
    virtual void onRequestCompleted(const char * requestId, IO2GResponse  *response = 0);

    /** Request execution failed data handler. */
    virtual void onRequestFailed(const char *requestId , const char *error);

    /** Request update data received data handler. */
    virtual void onTablesUpdates(IO2GResponse *data);

    HANDLE getEvent()
    {
        return mEvent;
    }
};

