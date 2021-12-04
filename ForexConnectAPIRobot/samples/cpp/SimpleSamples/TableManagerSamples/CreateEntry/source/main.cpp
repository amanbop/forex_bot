#include <string>
#include "../../../../../../include/Order2Go2.h"
#include "sessionstatus.h"
#include "stdafx.h"
#include "tablelistener.h"

int main(int argc, char *argv[])
{
    std::string instrument = "";
    std::string user = "";
    std::string password = "";
    std::string url = "";
    std::string connection = "";
    std::string subsession = "";
    std::string pin = "";
    if (argc < 4)
    {
        printf("usage: CreateEntry.exe instrument user password url connection [subsession] [pin]\n");
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
    session->useTableManager(::Yes, NULL);

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
        IO2GTableManager *tableManager = session->getTableManager();
        TableListener *tableListener = new TableListener();
        IO2GTableIterator tableIterator = {0, 0, NULL};

        IO2GOffersTable *offersTable = (IO2GOffersTable *)tableManager->getTable(::Offers);

        bool bInstrumentFound = false;
        std::string sOfferID;
        double ask = 0, bid = 0, pointSize = 0;

        // Check the instrument
        IO2GOfferTableRow *offerRow = NULL;
        while (offersTable->getNextRow(tableIterator, offerRow))
        {
            if (instrument == offerRow->getInstrument())
            {
                bInstrumentFound = true;
                sOfferID = offerRow->getOfferID();
                ask = offerRow->getAsk();
                bid = offerRow->getBid();
                pointSize = offerRow->getPointSize();
            }
            offerRow->release();
            if (bInstrumentFound)
                break;
        }
        offersTable->release();

        IO2GAccountsTable *accountsTable = (IO2GAccountsTable *)tableManager->getTable(Accounts);
        IO2GAccountTableRow *accountRow = NULL;

        bool bValidAccount = false;
        std::string sAccountID;
        int minAmount = 0, maxAmount = 0;

        tableIterator.i = 0;
        tableIterator.j = 0;
        tableIterator.item = NULL;
        // Find first account with available margin
        while (accountsTable->getNextRow(tableIterator, accountRow))
        {
            if( accountRow->getMarginCallFlag()[0] == 'N' ) // No limitations
            {
                bValidAccount = true;
                sAccountID = accountRow->getAccountID();
                minAmount = accountRow->getBaseUnitSize();
                maxAmount = accountRow->getAmountLimit();
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
            valuemap->setString(Command, O2G2::Commands::CreateOrder);
            valuemap->setString(OrderType, O2G2::Orders::Entry);

            valuemap->setString(AccountID, sAccountID.c_str());
            valuemap->setString(OfferID, sOfferID.c_str());
            valuemap->setString(BuySell, O2G2::Buy);
            valuemap->setDouble(Rate, ask - (10 * pointSize ) );
            valuemap->setInt(Amount, minAmount);

            IO2GRequest *request = requestFactory->createOrderRequest(valuemap);
            valuemap->release();

            IO2GOrdersTable *ordersTable = (IO2GOrdersTable *)tableManager->getTable(Orders);
            tableListener->setRequest(request->getRequestID());
            ordersTable->subscribeUpdate(Insert, tableListener);

            session->sendRequest(request);

            uni::WaitForSingleObject(tableListener->getEvent(), INFINITE);
            ordersTable->unsubscribeUpdate(Insert, tableListener);
            ordersTable->release();
            tableListener->release();
            request->release();
            requestFactory->release();
        }

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

