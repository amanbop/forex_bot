

#include "../include/header.h"
#include "../include/ExampleBase.h"

#include "../include/Session.h"
#include "../include/ResponseListener.h"

CBaseExample::CBaseExample(IO2GSession* session):
mSession(session)
{
    mSession->addRef();
    mResponseListener = new ResponseListener(mSession);
}

CBaseExample::~CBaseExample()
{
    mSession->release();
}

IO2GRequestFactory* CBaseExample::getRequestFactory()
{
    return mSession->getRequestFactory();
}

ResponseListener* CBaseExample::getResponseListener()
{
    return mResponseListener;
}

void CBaseExample::sendRequest(IO2GRequest* request)
{
    if (request == NULL)
        return;

    std::vector<std::string> requestsID;
    getRequestsID(request, requestsID);

    mSession->sendRequest(request);
    request->release();

    mResponseListener->wait(requestsID);
}

void CBaseExample::getRequestsID(IO2GRequest* request, TOrdersID& requestsID)
{
    if (request == NULL)
        return;

    int iCount = request->getChildrenCount();
    if (iCount == 0)
        return requestsID.push_back(request->getRequestID());

    for (int i = 0; i < iCount; i++)
        getRequestsID(request->getChildRequest(i), requestsID);
}


void CBaseExample::readOrder(IO2GResponse* response)
{
    using namespace O2G2;
    O2G2Ptr<IO2GResponseReaderFactory> factory = mSession->getResponseReaderFactory();
    O2G2Ptr<IO2GOrderResponseReader> reader = factory->createOrderResponseReader(response);
    if (!reader)
        return;

    LPCSTR szOrderID = reader->getOrderID();
    mOrdersID.push_back(szOrderID);
}

void CBaseExample::getOrdersIDFromResponses()
{
    ResponseListener::TResponses responses;
    mResponseListener->getResponses(responses);

    size_t count = responses.size();
    for (size_t i = 0; i < count; i++)
    {
        IO2GResponse* response = responses[i];
        readOrder(response);
        response->release();
    }
}

std::string CBaseExample::getAccountID(int index)
{
    using namespace O2G2;
    O2G2Ptr<IO2GResponseReaderFactory> factory = mSession->getResponseReaderFactory();

    O2G2Ptr<IO2GLoginRules> loginRules = mSession->getLoginRules();
    O2G2Ptr<IO2GResponse> response = loginRules->getTableRefreshResponse(Accounts);
    O2G2Ptr<IO2GAccountsTableResponseReader> reader = factory->createAccountsTableReader(response);
    if (!reader)
        return "";

    O2G2Ptr<IO2GAccountRow> account = reader->getRow(index);
    return account->getAccountID();
}

int CBaseExample::getBaseUnitSize(int index)
{
    using namespace O2G2;
    O2G2Ptr<IO2GResponseReaderFactory> factory = mSession->getResponseReaderFactory();

    O2G2Ptr<IO2GLoginRules> loginRules = mSession->getLoginRules();
    O2G2Ptr<IO2GResponse> response = loginRules->getTableRefreshResponse(Accounts);
    O2G2Ptr<IO2GAccountsTableResponseReader> reader = factory->createAccountsTableReader(response);
    if (!reader)
        return 0;

    O2G2Ptr<IO2GAccountRow> account = reader->getRow(index);
    return account->getBaseUnitSize();
}


IO2GOfferRow* CBaseExample::getOffer(const std::string& offerID)
{
    return mResponseListener->getOffer(offerID);
}

