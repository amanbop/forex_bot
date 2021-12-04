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
    if (argc < 5)
    {
        printf("usage: PartialFill.exe instrument user password url connection [subsession] [pin]\n");
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

        IO2GOffersTable *offersTable = (IO2GOffersTable *)tableManager->getTable(::Offers);

        bool bInstrumentFound = false;
        std::string sOfferID;

        // Check the instrument
        IO2GOfferTableRow *offerRow = NULL;
        IO2GTableIterator tableIterator = {0, 0, NULL};
        while (offersTable->getNextRow(tableIterator, offerRow))
        {
            if (instrument == offerRow->getInstrument())
            {
                bInstrumentFound = true;
                sOfferID = offerRow->getOfferID();
            }
            offerRow->release();
            if (bInstrumentFound)
                break;
        }
        offersTable->release();

        IO2GAccountsTable *accountsTable = (IO2GAccountsTable *)tableManager->getTable(Accounts);

        bool bValidAccount = false;
        std::string sAccountID;
        int iAmount = 70000;

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
            }
            accountRow->release();
            if (bValidAccount)
                break;
        }
        accountsTable->release();

        if (bValidAccount && bInstrumentFound)
        {
            // got all needed information. can make a position.
            IO2GRequestFactory *requestFactory = session->getRequestFactory();

            IO2GValueMap *valuemap = requestFactory->createValueMap();
            valuemap->setString(Command, O2G2::Commands::CreateOrder);
            valuemap->setString(OrderType, O2G2::Orders::TrueMarketOpen);
            valuemap->setString(AccountID, sAccountID.c_str());                // The identifier of the account the order should be placed for.
            valuemap->setString(OfferID, sOfferID.c_str());                    // The identifier of the instrument the order should be placed for.
            valuemap->setString(BuySell, O2G2::Buy);                           // The order direction: "S" for "Sell", "B" for "Buy".
            valuemap->setInt(Amount, iAmount);                                 // The minimum quantity of the instrument to be bought or sold.

            IO2GRequest *request = requestFactory->createOrderRequest(valuemap);
            valuemap->release();

            if (request)
            {
                IO2GTradesTable *tradesTable = (IO2GTradesTable*)tableManager->getTable(Trades);
                TableListener *tableListener = new TableListener();
                IO2GOrdersTable *ordersTable = (IO2GOrdersTable *)tableManager->getTable(Orders);
                tableListener->setRequest(request->getRequestID());
                ordersTable->subscribeUpdate(Insert, tableListener);
                ordersTable->subscribeUpdate(Update, tableListener);
                ordersTable->subscribeUpdate(Delete, tableListener);
                tradesTable->subscribeUpdate(Insert, tableListener);
                tradesTable->subscribeUpdate(Delete, tableListener);

                session->sendRequest(request);
                request->release();

                uni::WaitForSingleObject(tableListener->getEvent(), INFINITE);

                ordersTable->unsubscribeUpdate(Insert, tableListener);
                ordersTable->unsubscribeUpdate(Update, tableListener);
                ordersTable->unsubscribeUpdate(Delete, tableListener);
                tradesTable->unsubscribeUpdate(Insert, tableListener);
                tradesTable->unsubscribeUpdate(Delete, tableListener);

                tableListener->release();
                ordersTable->release();
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

