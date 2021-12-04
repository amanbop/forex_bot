#include <string>
#include "sessionstatus.h"
#include "tablelistener.h"
#include <iostream>
#include "stdafx.h"

int main(int argc, char *argv[])
{
    std::string user = "";
    std::string password = "";
    std::string url = "";
    std::string connection = "";
    std::string subsession = "";
    std::string pin = "";

    std::string instrument = "EUR/USD";

    if (argc < 3)
    {
        printf("usage: FindRowByColumnValue.exe user password url connection [subsession] [pin]\n");
        return -1;
    }

    user = argv[1];
    password = argv[2];
    if (argc >= 4)
        url = argv[3];
    else
        url = "http://www.fxcorporate.com/Hosts.jsp";
    if (argc >= 5)
        connection = argv[4];
    else
        connection = "Demo";
    if (argc >= 6)
        subsession = argv[5];
    if (argc >= 7)
        pin = argv[6];

    IO2GSession *session = CO2GTransport::createSession();

    // enable TableManager. Must be done before login
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

        std::string sAccountID = "";
        std::string sBuySell = O2G2::Buy;
        std::string sOfferID = "";
        bool bAccountFound = false;
        bool bOfferFound = false;
        int iBaseUnitSize = 0;
        int iLots = 0;
        double dRate = 0.0;
        double dRateLimit = 0.0;
        double dRateStop = 0.0;

        IO2GAccountsTable *accountsTable = (IO2GAccountsTable *)tableManager->getTable(Accounts);
        // Accounts table is already loaded, so we do not need to refresh it.
        IO2GAccountTableRow *accountRow = NULL;
        while (accountsTable->getNextRow(tableIterator, accountRow))
        {
            if (accountRow->getMarginCallFlag()[0] == 'N')
            {
                sAccountID = accountRow->getAccountID();
                IO2GLoginRules *loginRules = session->getLoginRules();
                IO2GTradingSettingsProvider *tradingSettingsProvider = loginRules->getTradingSettingsProvider();
                iBaseUnitSize = tradingSettingsProvider->getBaseUnitSize(instrument.c_str(), accountRow);
                iLots = tradingSettingsProvider->getMinQuantity(instrument.c_str(), accountRow);
                tradingSettingsProvider->release();
                loginRules->release();
                bAccountFound = true;
            }
            accountRow->release();
            if (bAccountFound)
                break;
        }
        accountsTable->release();

        IO2GOffersTable *offersTable = (IO2GOffersTable *)tableManager->getTable(Offers);
        tableIterator.i = 0;
        tableIterator.j = 0;
        tableIterator.item = NULL;
        IO2GOfferTableRow *offerRow = NULL;
        while (offersTable->getNextRow(tableIterator, offerRow))
        {
            if (instrument == offerRow->getInstrument())
            {
                sOfferID = offerRow->getOfferID();
                double dPointSize = offerRow->getPointSize();
                if (sBuySell == O2G2::Buy)
                {
                    double dAsk = offerRow->getAsk();
                    dRate = dAsk - 20 * dPointSize;
                    dRateLimit = dAsk + 20 * dPointSize;
                    dRateStop = dAsk - 40 * dPointSize;
                }
                else
                {
                    double dBid = offerRow->getBid();
                    dRate = dBid + 20 * dPointSize;
                    dRateLimit = dBid - 20 * dPointSize;
                    dRateStop = dBid + 40 * dPointSize;
                }
                bOfferFound = true;
            }
            offerRow->release();
            if (bOfferFound)
                break;
        }
        offersTable->release();

        if (bOfferFound && bAccountFound)
        {
            while (tableManager->getStatus() != TablesLoaded &&
                   tableManager->getStatus() != TablesLoadFailed)
                uni::Sleep(50);

            if (tableManager->getStatus() == TablesLoaded)
            {
                IO2GOrdersTable *ordersTable = (IO2GOrdersTable *)tableManager->getTable(Orders);
                ordersTable->subscribeUpdate(Update, tableListener);
                ordersTable->subscribeUpdate(Insert, tableListener);
                ordersTable->subscribeUpdate(Delete, tableListener);

                IO2GValueMap *valuemap = NULL;
                IO2GRequest *request = NULL;
                IO2GRequestFactory *requestFactory = session->getRequestFactory();

                // ============ Create entry order with attached limit entry order ====================
                std::cout << "Create ELS: Instrument=" << instrument << \
                    "; OrderType=" << O2G2::Orders::Entry << \
                    "; Rate=" << dRate << \
                    "; BuySell=" << sBuySell << \
                    "; Amount=" << iLots << \
                    "; RateLimit=" << dRateLimit << \
                    "; RateStop=" << dRateStop << std::endl;

                valuemap = requestFactory->createValueMap();
                valuemap->setString(Command, O2G2::Commands::CreateOrder);
                valuemap->setString(OrderType, O2G2::Orders::Entry);
                valuemap->setString(AccountID, sAccountID.c_str());
                valuemap->setString(OfferID, sOfferID.c_str());
                valuemap->setString(BuySell, sBuySell.c_str());
                valuemap->setInt(Amount, iLots);
                valuemap->setDouble(Rate, dRate);
                valuemap->setDouble(RateLimit, dRateLimit);
                valuemap->setDouble(RateStop, dRateStop);

                request = requestFactory->createOrderRequest(valuemap);
                valuemap->release();

                std::string sRequestID = "";
                if (request)
                {
                    sRequestID = request->getRequestID();
                    tableListener->setRequest(sRequestID.c_str());

                    session->sendRequest(request);
                    uni::WaitForSingleObject(tableListener->getEvent(), INFINITE);
                    request->release();
                }

                ordersTable->unsubscribeUpdate(Update, tableListener);
                ordersTable->unsubscribeUpdate(Insert, tableListener);
                ordersTable->unsubscribeUpdate(Delete, tableListener);
                tableListener->release();

                // ======== find all rows in ordersTable with sRequestID in "RequestID" column ==========
                IO2GOrderTableRow *orderRow = 0;
                IO2GTableIterator orderTableIterator = {0};
                while (ordersTable->getNextRowByColumnValue("RequestID", sRequestID.c_str(), orderTableIterator, orderRow))
                {
                    std::cout << "OrderID: " << orderRow->getOrderID() \
                        << "; OrderType: " << orderRow->getType() \
                        << "; Status: " << orderRow->getStatus() << std::endl;
                    orderRow->release();
                }

                ordersTable->release();
            }

            tableManager->release();
        }
        else
        {
            std::cout << "No valid account or instrument not found" << std::endl;
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

