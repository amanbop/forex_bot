#include <string>
#include "../../../../../../include/Order2Go2.h"
#include "sessionstatus.h"
#include "tablelistener.h"
#include "stdafx.h"

void CreateOrder(IO2GSession *session, std::string accID, std::string offerID, const char *SellBuy, double dRate, const char orderType[3]);
void EditOrder(IO2GSession *session, std::string accID, std::string orderID, double dRate);
void DeleteOrder(IO2GSession *session, std::string accID, std::string orderID);

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
        printf("usage: NetStopLimit.exe instrument user password url connection subsession? pin?");
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
        double dAsk = 0, dBid = 0, dPointSize = 0;

        // Check the instrument
        IO2GOfferTableRow *offerRow = NULL;
        IO2GTableIterator tableIterator = {0, 0, NULL};
        while (offersTable->getNextRow(tableIterator, offerRow))
        {
            if (instrument == offerRow->getInstrument() )
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
                break;
            }
            accountRow->release();
        }
        accountsTable->release();

        if (bValidAccount && bInstrumentFound)
        {
            while (tableManager->getStatus() != TablesLoaded &&
                tableManager->getStatus() != TablesLoadFailed)
                uni::Sleep(50);

            if (tableManager->getStatus() == TablesLoaded)
            {
                IO2GOrdersTable *ordersTable = (IO2GOrdersTable *)tableManager->getTable(::Orders);

                TableListener *tableListener = new TableListener();
                ordersTable->subscribeUpdate(Insert, tableListener);
                ordersTable->subscribeUpdate(Delete, tableListener);
                ordersTable->subscribeUpdate(Update, tableListener);

                // Create Net Stop Limit
                CreateOrder(session, sAccountID, sOfferID, O2G2::Buy, ( dAsk +  (35 * dPointSize) ), O2G2::Orders::StopEntry );
                uni::WaitForSingleObject(tableListener->getEvent(), INFINITE);

                // Create LimitEntry
                CreateOrder(session, sAccountID, sOfferID, O2G2::Buy, (dAsk - ( 15 * dPointSize ) ), O2G2::Orders::LimitEntry );
                uni::WaitForSingleObject(tableListener->getEvent(), INFINITE);

                ordersTable->unsubscribeUpdate(Insert, tableListener);

                std::string id = "";

                // decrease stop entry rate by 10 pointSize
                id = tableListener->getOrderID(O2G2::Orders::StopEntry);
                if ( id == "" )
                {
                    printf("order ID not found in table\n");
                    return -1;
                }
                EditOrder(session, sAccountID, id, ( dAsk + ( 25 * dPointSize ) ) );
                uni::WaitForSingleObject(tableListener->getEvent(), INFINITE);

                // decrease limit entry rate by 10 point size
                id = tableListener->getOrderID(O2G2::Orders::LimitEntry);
                if ( id == "" )
                {
                    printf("order ID not found in table\n");
                    return -1;
                }
                EditOrder(session, sAccountID, id, ( dAsk - ( 25 * dPointSize ) ) );
                uni::WaitForSingleObject(tableListener->getEvent(), INFINITE);

                ordersTable->unsubscribeUpdate(::Update, tableListener);

                // delete stop entry
                id = tableListener->getOrderID(O2G2::Orders::StopEntry);
                if ( id == "" )
                {
                    printf("order ID not found in table\n");
                    return -1;
                }
                DeleteOrder(session, sAccountID, id);
                uni::WaitForSingleObject(tableListener->getEvent(), INFINITE);

                // delete limit entry
                id = tableListener->getOrderID(O2G2::Orders::LimitEntry);
                if ( id == "" )
                {
                    printf("order ID not found in table\n");
                    return -1;
                }
                DeleteOrder(session, sAccountID, id);
                uni::WaitForSingleObject(tableListener->getEvent(), INFINITE);

                ordersTable->unsubscribeUpdate(Delete, tableListener);

                tableListener->release();
                ordersTable->release();
            }
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

void CreateOrder(IO2GSession *session, std::string accID, std::string offerID, const char *SellBuy, double dRate, const char orderType[3])
{
    IO2GRequestFactory *requestFactory = session->getRequestFactory();
    IO2GValueMap *valuemap = requestFactory->createValueMap();

    valuemap->setString(Command, O2G2::Commands::CreateOrder);
    valuemap->setString(OrderType, orderType);
    valuemap->setString(AccountID, accID.c_str());
    valuemap->setString(OfferID, offerID.c_str());
    valuemap->setString(BuySell, SellBuy);
    valuemap->setDouble(Rate, dRate );
    valuemap->setString(NetQuantity, "Y");

    IO2GRequest *request = requestFactory->createOrderRequest(valuemap);
    valuemap->release();
    session->sendRequest(request);
    request->release();
    requestFactory->release();
}

void EditOrder(IO2GSession *session, std::string accID, std::string orderID, double dRate)
{
    IO2GRequestFactory *requestFactory = session->getRequestFactory();
    IO2GValueMap *valuemap = requestFactory->createValueMap();

    valuemap->setString(Command, O2G2::Commands::EditOrder);
    valuemap->setString(OrderID, orderID.c_str());
    valuemap->setString(AccountID, accID.c_str());
    valuemap->setDouble(Rate, dRate);

    IO2GRequest *request = requestFactory->createOrderRequest(valuemap);
    valuemap->release();
    session->sendRequest(request);
    request->release();
    requestFactory->release();
}

void DeleteOrder(IO2GSession *session, std::string accID, std::string orderID)
{
    IO2GRequestFactory *requestFactory = session->getRequestFactory();
    IO2GValueMap *valuemap = requestFactory->createValueMap();

    valuemap->setString(Command, O2G2::Commands::DeleteOrder);
    valuemap->setString(OrderID, orderID.c_str());
    valuemap->setString(AccountID, accID.c_str());

    IO2GRequest *request = requestFactory->createOrderRequest(valuemap);
    valuemap->release();
    session->sendRequest(request);
    request->release();
    requestFactory->release();
}

