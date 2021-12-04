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
    /** Flag indicating that the request was finished. */
    bool mFinished;
    DATE mTimeFrom;
    DATE mTimeTo;
    /** Event handle. */
    HANDLE mEvent;
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

    /** Set time borders. */
    void setTimeBorders(const DATE timeFrom, const DATE timeTo);

    /** Get time borders. */
    void getTimeBorders(DATE *timeFrom, DATE *timeTo) const;

    /** Set request. */
    void setRequest(const char *requestId);

    /** Check whether the request has been finished. */
    bool isFinished() const;

    /** Request execution completed data handler. */
    virtual void onRequestCompleted(const char * requestId, IO2GResponse  *response = 0);

    /** Request execution failed data handler. */
    virtual void onRequestFailed(const char *requestId , const char *error);

    /** Request update data received data handler. */
    virtual void onTablesUpdates(IO2GResponse *data);

    HANDLE getEvent()
    {
        return mEvent;
    };
};

