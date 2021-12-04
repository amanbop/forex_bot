
#pragma once

#include "ExampleBase.h"

class IO2GSession;
class ResponseListener;

class CBaseExample
{
    IO2GSession* mSession;
    O2G2Ptr<ResponseListener> mResponseListener;

public:
    typedef std::vector<std::string> TOrdersID;

protected:
    TOrdersID mOrdersID;

    void sendRequest(IO2GRequest* request);
    void getRequestsID(IO2GRequest* request, TOrdersID& requestsID);

    void readOrder(IO2GResponse* response);
    void getOrdersIDFromResponses();
    std::string getAccountID(int index);
    int getBaseUnitSize(int index);

    IO2GOfferRow* getOffer(const std::string& offerID);
    IO2GRequestFactory* getRequestFactory();
    ResponseListener* getResponseListener();
public:

    CBaseExample(IO2GSession* session);
    virtual ~CBaseExample();

    virtual void run(){}
};

