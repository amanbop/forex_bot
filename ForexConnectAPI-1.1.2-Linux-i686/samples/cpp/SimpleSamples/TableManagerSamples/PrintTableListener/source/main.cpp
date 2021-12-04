#include <string>
#include "../../../../../../include/Order2Go2.h"
#include "sessionstatus.h"
#include "stdafx.h"

int main(int argc, char *argv[])
{
    std::string user = "";
    std::string password = "";
    std::string url = "";
    std::string connection = "";
    std::string subsession = "";
    std::string pin = "";
    if (argc < 3)
    {
        printf("usage: PrintTableListener.exe user password url connection [subsession] [pin]\n");
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

    TablesManagerStatusListener *tablesStatusListener = new TablesManagerStatusListener();
    // Enable TableManager usage. Must be before login.
    session->useTableManager(::Yes, tablesStatusListener);

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

        uni::WaitForSingleObject(tablesStatusListener->getTablesLoadedEvent(), INFINITE);

        if (tablesStatusListener->isTablesLoaded())
        {
            IO2GTableManager *tableManager = session->getTableManager();
            IO2GTradesTable *tradesTable = (IO2GTradesTable *)tableManager->getTable(Trades);
            // This table is being refreshed right now.

            std::cout << "TradesTable: " << std::endl;
            IO2GTradeTableRow *tradeRow = NULL;
            IO2GTableIterator tableIterator = {0, 0, NULL};
            while (tradesTable->getNextRow(tableIterator, tradeRow))
            {
                std::cout << "ID: " << tradeRow->getTradeID() << \
                    " Direction: " << tradeRow->getBuySell() << \
                    " Amount: " << tradeRow->getAmount() << std::endl;
                tradeRow->release();
            }
            tableManager->release();
        }

        session->logout();
        while (true)
        {
            uni::WaitForSingleObject(listener->getEvent(), 250);
            if (listener->getStatusCode() == IO2GSessionStatus::Disconnected)
                break;
        }
    }

    tablesStatusListener->release();

    session->unsubscribeSessionStatus(listener);
    listener->release();
    session->release();
}
