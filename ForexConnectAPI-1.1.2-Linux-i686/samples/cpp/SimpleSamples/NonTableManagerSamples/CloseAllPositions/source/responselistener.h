#include "../../../../../../include/Order2Go2.h"
#include "stdafx.h"

/** Response listener class. */
class OfferCollection;
class Offer;

class ResponseListener : public IO2GResponseListener
{
 private:
    long mRefCount;
    /** Session object. */
    IO2GSession *mSession;
    /** Request we are waiting for. */
    std::string mRequestID;
    std::vector<std::string> *pvecRequestID;
    unsigned int iRequestsProcessed;
    /** Response Event handle. */
    HANDLE mResponseEvent;
    /** Update Event handle. */
    HANDLE mUpdateEvent;
    /** State of last request. */
    bool mSuccess;
    /** List of trade rows to be exported */
    std::list<IO2GTradeRow *> *pliTradeRows;
 protected:
    /** Destructor. */
    virtual ~ResponseListener();

    /** Print offer to the console. */
    void printOffer(Offer *offer);

    /** Format date to print */
    void formatDate(DATE d, std::string &buf);

 public:
    ResponseListener(IO2GSession *session);

    /** Increase reference counter. */
    virtual long addRef();

    /** Decrease reference counter. */
    virtual long release();

    /** Set request. */
    void setRequest(const char *requestId);
    void setRequestVector(std::vector<std::string> *lpvecRequestID);

    /** Request execution completed data handler. */
    virtual void onRequestCompleted(const char * requestId, IO2GResponse  *response = 0);

    /** Request execution failed data handler. */
    virtual void onRequestFailed(const char *requestId , const char *error);

    /** Request update data received data handler. */
    virtual void onTablesUpdates(IO2GResponse *data);

    bool getRequestSuccess()
    {
        return mSuccess;
    }

    HANDLE getResponseEvent()
    {
        return mResponseEvent;
    };

    HANDLE getUpdateEvent()
    {
        return mUpdateEvent;
    }
    std::list<IO2GTradeRow *> *getTradeRowsList()
    {
        return pliTradeRows;
    }
};

