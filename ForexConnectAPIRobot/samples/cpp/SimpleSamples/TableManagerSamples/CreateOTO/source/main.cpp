#include <string>
#include "../../../../../../include/Order2Go2.h"
#include "sessionstatus.h"
#include "stdafx.h"
#include "tablelistener.h"
#include "responselistener.h"

int main(int argc, char *argv[])
{
    std::string instrument = "";
    std::string user = "";
    std::string password = "";
    std::string url = "";
    std::string connection = "";
    std::string subsession = "";
    std::string pin = "";
    if (argc < 5)
    {
        printf("usage: CreateOTO.exe instrument user password url connection [subsession] [pin]\n");
        return -1;
    }

    instrument = argv[1];
    user = argv[2];
    password = argv[3];
    if (argc >= 5)
        url = argv[4];
    else
        url = "http://www.fxcorporate.com/Hosts.jsp";
    if (argc >= 6)
        connection = argv[5];
    else
        connection = "Demo";
    if (argc >= 7)
        subsession = argv[6];
    if (argc >= 8)
        pin = argv[7];

    IO2GSession *session = CO2GTransport::createSession();

    // Enable TableManager usage. Must be before login.
    session->useTableManager(Yes, NULL);

    SessionStatusListener *listener = new SessionStatusListener(session, true, subsession.c_str(), pin.c_str());
    session->subscribeSessionStatus(listener);
    listener->resetError();

    session->login(user.c_str(), password.c_str(), url.c_str(), connection.c_str());

    bool connected = false;

    while (true)
    {
        uni::WaitForSingleObject(listener->getEvent(), 250);
        if (listener->getStatusCode() == IO2GSessionStatus::Connected)
        {
            connected = true;
            break;
        }
        if (listener->wasError())
        {
            printf("Connection failed: %s", listener->getError());
            break;
        }
    }

    if (connected)
    {
        const int iOrdersInOTO = 3;

        IO2GTableManager *tableManager = session->getTableManager();

        IO2GOffersTable *offersTable = (IO2GOffersTable *)tableManager->getTable(::Offers);

        bool bInstrumentFound = false;
        std::string sOfferID;
        double dAsk = 0, dBid = 0, dPointSize = 0;

        // Check the instrument
        IO2GOfferTableRow *offerRow = NULL;
        IO2GTableIterator tableIterator = {0, 0, NULL};
        while (offersTable->getNextRow(tableIterator, offerRow))
        {
            if (instrument == offerRow->getInstrument())
            {
                bInstrumentFound = true;
                sOfferID = offerRow->getOfferID();
                dAsk = offerRow->getAsk();
                dBid = offerRow->getBid();
                dPointSize = offerRow->getPointSize();
            }
            offerRow->release();
            if (bInstrumentFound)
                break;
        }
        offersTable->release();

        IO2GAccountsTable *accountsTable = (IO2GAccountsTable *)tableManager->getTable(Accounts);

        bool bValidAccount = false;
        std::string sAccountID;
        int iAmount = 0;

        // Find first account with available margin
        IO2GAccountTableRow *accountRow = NULL;
        tableIterator.i = tableIterator.j = 0;
        tableIterator.item = NULL;
        while (accountsTable->getNextRow(tableIterator, accountRow))
        {
            if (accountRow->getMarginCallFlag()[0] == 'N') // No limitations
            {
                bValidAccount = true;
                sAccountID = accountRow->getAccountID();
                iAmount = accountRow->getBaseUnitSize();
            }
            accountRow->release();
            if (bValidAccount)
                break;
        }
        accountsTable->release();

        if (bValidAccount && bInstrumentFound)
        {
            IO2GRequestFactory *requestFactory = session->getRequestFactory();

            IO2GValueMap *valuemap = requestFactory->createValueMap();
            valuemap->setString(Command, O2G2::Commands::CreateOTO);

            // valuemap #0 is main order. Others are secondary.
            for (int i = 0; i < iOrdersInOTO; ++i)
            {
                IO2GValueMap *valuemapChild = requestFactory->createValueMap();

                valuemapChild->setString(Command, O2G2::Commands::CreateOrder);
                valuemapChild->setString(OrderType, O2G2::Orders::Entry);
                valuemapChild->setString(OfferID, sOfferID.c_str());
                valuemapChild->setString(AccountID, sAccountID.c_str());
                valuemapChild->setString(BuySell, O2G2::Buy);
                if (i == 0) // rate for main order must be a bit higher in this situation.
                    valuemapChild->setDouble(Rate, dAsk - (50 * dPointSize));
                else // when OTO is executed, bid price will be (dBid - (50 * dPointSize)). So
                    valuemapChild->setDouble(Rate, dAsk - ((50 + 30) * dPointSize));
                valuemapChild->setInt(Amount, iAmount);

                valuemap->appendChild(valuemapChild);
            }

            IO2GRequest *request = requestFactory->createOrderRequest(valuemap);
            valuemap->release();

            if (request)
            {
                ResponseListener *responseListener = new ResponseListener(session);
                session->subscribeResponse(responseListener);

                TableListener *tableListener = new TableListener();
                IO2GOrdersTable *ordersTable = (IO2GOrdersTable *)tableManager->getTable(Orders);
                ordersTable->subscribeUpdate(Insert, tableListener);

                int iRequests = request->getChildrenCount();

                for (int i = 0; i < iRequests; ++i)
                {
                    IO2GRequest *requestChild = request->getChildRequest(i);
                    responseListener->appendRequest(requestChild->getRequestID());
                    tableListener->appendRequest(requestChild->getRequestID());
                    requestChild->release();
                }

                session->sendRequest(request);
                request->release();

                const HANDLE aHandles[2] = {responseListener->getRequestFailedEvent(), tableListener->getEvent()};
                uni::WaitForMultipleObjects(2, aHandles, FALSE, INFINITE);

                ordersTable->unsubscribeUpdate(Insert, tableListener);
                tableListener->release();
                ordersTable->release();

                session->unsubscribeResponse(responseListener);
                responseListener->release();
            }
            else
                std::cout << "Cannot create request" << std::endl;

            requestFactory->release();
        }
        else
            std::cout << "No valid account or invalid instrument" << std::endl;

        session->logout();
        while (true)
        {
            uni::WaitForSingleObject(listener->getEvent(), 250);
            if (listener->getStatusCode() == IO2GSessionStatus::Disconnected)
                break;
        }
    }

    session->unsubscribeSessionStatus(listener);
    listener->release();
    session->release();
}

