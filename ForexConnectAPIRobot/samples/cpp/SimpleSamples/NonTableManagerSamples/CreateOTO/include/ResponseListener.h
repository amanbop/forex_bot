
#pragma once

#include "Table.h"

class ResponseListener : public AddRef<IO2GResponseListener>
{
public:
    typedef std::vector<std::string> TRequets;
    typedef std::vector<IO2GResponse*> TResponses;

private:
    IO2GSession* mSession;

    threading::Mutex mMtxRequests;
    threading::Mutex mMtxResponses;

    CEventWithError mReqIDEvent;

    TRequets maRequests;
    TResponses maResponses;

    COffersTable mOffersTable;

    virtual void onRequestCompleted(const char*  requestId, IO2GResponse* response = 0);
    virtual void onRequestFailed(const char* requestId , const char* error);
    virtual void onTablesUpdates(IO2GResponse* tablesUpdates);

    virtual ~ResponseListener();

void initializeOfferTable();
public:

    ResponseListener(IO2GSession* session);

    bool wait(TRequets& requestsID);
    void getResponses(TResponses& responses);

    void clear();

    IO2GOfferRow* getOffer(const std::string& sOfferID)
    {
        return mOffersTable.getRow(sOfferID);
    }
};

