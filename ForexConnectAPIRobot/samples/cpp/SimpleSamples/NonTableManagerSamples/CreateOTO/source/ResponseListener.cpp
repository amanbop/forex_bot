

#include "../include/header.h"
#include "../include/ResponseListener.h"

#include "../include/Session.h"

ResponseListener::ResponseListener(IO2GSession* session):
mSession(session)
{
    mSession->addRef();
    mSession->subscribeResponse(this);

    initializeOfferTable();
}

ResponseListener::~ResponseListener()
{
    mSession->unsubscribeResponse(this);
    mSession->release();
}

void ResponseListener::onRequestCompleted(const char* requestId, IO2GResponse* response/* = 0*/)
{
    if (requestId == NULL || *requestId == '\x0')
        return;

    threading::Mutex::Lock lock(mMtxRequests);

    TRequets::iterator iter = std::find(maRequests.begin(), maRequests.end(), requestId);
    if (iter != maRequests.end())
    {
        response->addRef();
        maRequests.erase(iter);
        threading::Mutex::Lock lock2(mMtxResponses);
        maResponses.push_back(response);
    }
    if (maRequests.empty())
        mReqIDEvent.signal();
}

void ResponseListener::onRequestFailed(const char* requestId, const char* error)
{
    printf("%s: %s\n", requestId, error);
    if (requestId == NULL || *requestId == '\x0')
        return;

    threading::Mutex::Lock lock(mMtxRequests);

    TRequets::iterator iter = std::find(maRequests.begin(), maRequests.end(), requestId);
    if (iter != maRequests.end())
        maRequests.erase(iter);
    if (maRequests.empty())
        mReqIDEvent.signal();
}

void ResponseListener::onTablesUpdates(IO2GResponse* tablesUpdates)
{
    if (tablesUpdates->getType() == TablesUpdates)
    {
        O2G2Ptr<IO2GResponseReaderFactory> factory = mSession->getResponseReaderFactory();
        if (!factory)
            return;
        O2G2Ptr<IO2GTablesUpdatesReader> updateReader = factory->createTablesUpdatesReader(tablesUpdates);
        if (!updateReader)
            return;
        int iSize = updateReader->size();
        for (int i = 0; i < iSize; i++)
        {
            O2GTable tableType = updateReader->getUpdateTable(i);
            switch (tableType)
            {
            case Orders:
                {
                    IO2GOrderRow* pRow = updateReader->getOrderRow(i);
                    O2GTableUpdateType updateType = updateReader->getUpdateType(i);
                    LPCSTR szAction = updateType == Insert ? "Add" :
                                      updateType == Delete ? "Delete" :
                                      updateType == Update ? "Change" : "UnknownAction";
                    printf("\t\t\t %s: OrderID: %s. PrimaryID: %s\n", szAction, pRow->getOrderID(), pRow->getPrimaryID());
                    pRow->release();
                }
                break;
            case Offers:
                {
                    IO2GOfferRow* pRow = updateReader->getOfferRow(i);
                    mOffersTable.update(pRow);
                    pRow->release();
                }
                break;
            }
        }
    }
    if (tablesUpdates->getType() == GetOffers)
    {
        O2G2Ptr<IO2GResponseReaderFactory> factory = mSession->getResponseReaderFactory();
        if (!factory)
            return;
        O2G2Ptr<IO2GOffersTableResponseReader> offersReader = factory->createOffersTableReader(tablesUpdates);
        if (!offersReader)
            return;
        int iSize = offersReader->size();
        for (int i = 0; i < iSize; i++)
        {
            IO2GOfferRow* pRow = offersReader->getRow(i);
            mOffersTable.update(pRow);
            pRow->release();
        }
    }
}

bool ResponseListener::wait(TRequets& requestsID)
{
    if (requestsID.empty())
        return false;

    mMtxRequests.lock();
    maRequests = requestsID;
    mMtxRequests.unlock();

    return mReqIDEvent.wait();
}

void ResponseListener::getResponses(TResponses& responses)
{
    threading::Mutex::Lock lock2(mMtxResponses);
    responses = maResponses;
    maResponses.clear();
}

void ResponseListener::clear()
{
    threading::Mutex::Lock lock(mMtxRequests);
    threading::Mutex::Lock lock2(mMtxResponses);

    maRequests.clear();
    size_t count = maResponses.size();
    for (size_t i = 0; i < count; i++)
        maResponses[i]->release();
    maResponses.clear();
}

void ResponseListener::initializeOfferTable()
{
    using namespace O2G2;
    O2G2Ptr<IO2GResponseReaderFactory> factory = mSession->getResponseReaderFactory();

    O2G2Ptr<IO2GLoginRules> loginRules = mSession->getLoginRules();
    O2G2Ptr<IO2GResponse> response = loginRules->getTableRefreshResponse(Offers);
    onTablesUpdates(response);
}

