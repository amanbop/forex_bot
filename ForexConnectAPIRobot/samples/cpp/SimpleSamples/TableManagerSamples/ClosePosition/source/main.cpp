#include <string>
#include "../../../../../../include/Order2Go2.h"
#include "sessionstatus.h"
#include "tablelistener.h"
#include "stdafx.h"

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
        printf("usage: ClosePosition.exe instrument user password url connection [subsession] [pin]\n");
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
        // this table is already in Refreshed state.

        bool bInstrumentFound = false;
        std::string sOfferID;

        // Check the instrument
        IO2GOfferTableRow *offerRow = NULL;
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

        while (tableManager->getStatus() != TablesLoaded &&
               tableManager->getStatus() != TablesLoadFailed)
            uni::Sleep(50);

        if (tableManager->getStatus() == TablesLoaded)
        {
            IO2GTradesTable *tradesTable = (IO2GTradesTable *)tableManager->getTable(::Trades);
            bool bPositionFound = false;
            std::string sTradeID;
            std::string sAccountID;
            std::string sBuySell;
            int iAmount = 0;

            IO2GTradeTableRow *tradeRow = NULL;
            tableIterator.i = 0;
            tableIterator.j = 0;
            tableIterator.item = NULL;
            while (tradesTable->getNextRow(tableIterator, tradeRow))
            {
                if( sOfferID == tradeRow->getOfferID() )
                {
                    bPositionFound = true;
                    sBuySell = tradeRow->getBuySell();
                    iAmount = tradeRow->getAmount();
                    sTradeID = tradeRow->getTradeID();
                    sAccountID = tradeRow->getAccountID();
                }
                tradeRow->release();
                if (bPositionFound)
                    break;
            }

            if (bPositionFound)
            {
                // got all needed information. can close a position.
                IO2GRequestFactory *requestFactory = session->getRequestFactory();

                IO2GValueMap *valuemap = requestFactory->createValueMap();
                valuemap->setString(Command, O2G2::Commands::CreateOrder);
                valuemap->setString(AccountID, sAccountID.c_str());
                valuemap->setString(OfferID, sOfferID.c_str());
                valuemap->setString(BuySell, (sBuySell == O2G2::Buy ? O2G2::Sell : O2G2::Buy ) );
                valuemap->setInt(Amount, iAmount);

                IO2GLoginRules *loginRules = session->getLoginRules();
                IO2GPermissionChecker *permissionChecker = loginRules->getPermissionChecker();
                loginRules->release();
                if (permissionChecker->canCreateMarketCloseOrder(instrument.c_str()) != PermissionEnabled )
                { // FIFO
                    valuemap->setString(OrderType, O2G2::Orders::TrueMarketOpen);
                }
                else
                { // Non-FIFO
                    valuemap->setString(OrderType, O2G2::Orders::TrueMarketClose);
                    valuemap->setString(TradeID, sTradeID.c_str());
                }
                permissionChecker->release();

                IO2GRequest *request = requestFactory->createOrderRequest(valuemap);
                valuemap->release();
                if (request)
                {
                    IO2GOrdersTable *ordersTable = (IO2GOrdersTable *)tableManager->getTable(Orders);
                    // This table is being refreshed right now.
                    ordersTable->subscribeUpdate(Insert, tableListener);

                    tableListener->setRequest(request->getRequestID());
                    session->sendRequest(request);
                    request->release();

                    uni::WaitForSingleObject(tableListener->getEvent(), INFINITE);
                    ordersTable->unsubscribeUpdate(Insert, tableListener);

                    ordersTable->release();
                }
                else
                    std::cout << "Cannot create request" << std::endl;
                requestFactory->release();
            }
            else
                std::cout << "No valid position in trades table found" << std::endl;
            tradesTable->release();
        }

        tableListener->release();
        tableManager->release();

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

