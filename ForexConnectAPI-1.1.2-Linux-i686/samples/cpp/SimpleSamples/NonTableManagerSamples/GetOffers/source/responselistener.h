#include "../../../../../../include/Order2Go2.h"


/** Response listener class. */
class OfferCollection;
class Offer;

class ResponseListener : public IO2GResponseListener
{
 private:
    long mRefCount;
    /** Session object. */
    IO2GSession *mSession;
    /** Collection of the offers. */
    OfferCollection *mOffers;
 protected:
    /** Destructor. */
    virtual ~ResponseListener();

    /** Print offer to the console. */
    void printOffer(Offer *offer);

    /** Format date to print */
    void formatDate(DATE d, char *buf);

 public:
    ResponseListener(IO2GSession *session);

    /** Increase reference counter. */
    virtual long addRef();

    /** Decrease reference counter. */
    virtual long release();

    /** Request execution completed data handler. */
    virtual void onRequestCompleted(const char * requestId, IO2GResponse  *response = 0);

    /** Request execution failed data handler. */
    virtual void onRequestFailed(const char *requestId , const char *error);

    /** Request update data received data handler. */
    virtual void onTablesUpdates(IO2GResponse *data);
};

