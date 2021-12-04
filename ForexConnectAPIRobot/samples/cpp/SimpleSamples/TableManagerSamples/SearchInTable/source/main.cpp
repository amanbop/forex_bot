#include <string>
#include "../../../../../../include/Order2Go2.h"
#include "sessionstatus.h"
#include "stdafx.h"

int main(int argc, char *argv[])
{
    std::string sOrderID = "";
    std::string user = "";
    std::string password = "";
    std::string url = "";
    std::string connection = "";
    std::string subsession = "";
    std::string pin = "";
    if (argc < 4)
    {
        printf("usage: SearchInTable.exe orderID user password url connection [subsession] [pin]\n");
        return -1;
    }

    sOrderID = argv[1];
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

        while (tableManager->getStatus() != TablesLoaded &&
               tableManager->getStatus() != TablesLoadFailed)
            uni::Sleep(50);

        if (tableManager->getStatus() == TablesLoaded)
        {
            IO2GOrdersTable *ordersTable = (IO2GOrdersTable *)tableManager->getTable(::Orders);
            // This table is being refreshed right now.

            std::cout << "Search by OrderID \"" << sOrderID << "\":" << std::endl;
            IO2GOrderTableRow *orderRow = NULL;
            if (ordersTable->findRow(sOrderID.c_str(), orderRow))
            {
                std::cout << "AccountID: " << orderRow->getAccountID() << \
                std::cout << " Amount: " << orderRow->getAmount() << std::endl;
                orderRow->release();
            }
            else
                std::cout << "Cannot find row" << std::endl;
            ordersTable->release();
        }

        IO2GTradesTable *tradesTable = (IO2GTradesTable*)tableManager->getTable(Trades);
        // This table is being refreshed right now.

        while (tradesTable->getStatus() != Refreshed && tradesTable->getStatus() != Failed)
            uni::Sleep(50);

        if (tradesTable->getStatus() == Refreshed)
        {
            std::cout << std::endl;
            IO2GTradeTableRow *tradeRow = NULL;
            IO2GTableIterator tableIterator = {0, 0, NULL};
            const std::string sBuySell = O2G2::Buy;

            std::cout << "Search by BuySell direction \"" << sBuySell << "\":" << std::endl;
            while (tradesTable->getNextRow(tableIterator, tradeRow))
            {
                if (sBuySell == tradeRow->getBuySell())
                {
                    std::cout << "TradeID: " << tradeRow->getTradeID() << \
                        " AccountID: " << tradeRow->getAccountID() << \
                        " Amount: " << tradeRow->getAmount() << std::endl;
                }
                tradeRow->release();
            }
        }
        tradesTable->release();

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

