#include "../../../../../../include/Order2Go2.h"
#include "stdafx.h"

/** Response listener class. */
class ResponseListener : public IO2GResponseListener
{
 private:
    long mRefCount;
    /** Session object. */
    IO2GSession *mSession;
    /** Request we are waiting for. */
    std::list<std::string> mliRequestID;
    /** Response Event handle. */
    HANDLE mRequestFailedEvent;
    /** Update Event handle. */
    HANDLE mUpdateEvent;

    ::threading::Mutex mMutex;
 protected:
    /** Destructor. */
    virtual ~ResponseListener();

 public:
    ResponseListener(IO2GSession *session);

    /** Increase reference counter. */
    virtual long addRef();

    /** Decrease reference counter. */
    virtual long release();

    /** Set request. */
    void appendRequest(const char *requestId);

    /** Request execution completed data handler. */
    virtual void onRequestCompleted(const char * requestId, IO2GResponse  *response = 0);

    /** Request execution failed data handler. */
    virtual void onRequestFailed(const char *requestId , const char *error);

    /** Request update data received data handler. */
    virtual void onTablesUpdates(IO2GResponse *data);

    HANDLE getRequestFailedEvent()
    {
        return mRequestFailedEvent;
    }
};

