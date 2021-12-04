#include <string>
#include "../../../../../../include/Order2Go2.h"
#include "sessionstatus.h"
#include "stdafx.h"
#include "responselistener.h"
#include "tablelistener.h"

void PrintOrdersTable(IO2GOrdersTable *ordersTable);
std::string GetValidAccount(IO2GAccountsTable *accountsTable, int &iMinAmount);

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
        printf("usage: LockUpdates.exe user password url connection [subsession] [pin]\n");
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
            IO2GOrdersTable *ordersTable = (IO2GOrdersTable *)tableManager->getTable(Orders);
            
            ResponseListener *responseListener = new ResponseListener();
            session->subscribeResponse(responseListener);

            TableListener *tableListener = new TableListener(tableManager);
            ordersTable->subscribeUpdate(Insert, tableListener);

            IO2GAccountsTable *accountsTable = (IO2GAccountsTable *)tableManager->getTable(Accounts);
            int iAmount = 0;
            std::string sAccountID = GetValidAccount(accountsTable, iAmount);
            accountsTable->release();

            if (sAccountID.length() > 0)
            {
                // Create TrueMarketOpen order
                IO2GRequestFactory *requestFactory = session->getRequestFactory();

                IO2GValueMap *valuemap = requestFactory->createValueMap();
                valuemap->setString(Command, O2G2::Commands::CreateOrder);
                valuemap->setString(OrderType, O2G2::Orders::TrueMarketOpen);
                valuemap->setString(AccountID, sAccountID.c_str());                // The identifier of the account the order should be placed for.
                valuemap->setString(OfferID, "1"); // EUR/USD                      // The identifier of the instrument the order should be placed for.
                valuemap->setString(BuySell, O2G2::Buy);                           // The order direction: "S" for "Sell", "B" for "Buy".
                valuemap->setInt(Amount, iAmount);                                 // The minimum quantity of the instrument to be bought or sold.

                IO2GRequest *request = requestFactory->createOrderRequest(valuemap);
                valuemap->release();

                if (request)
                {
                    HANDLE phEvents[2] = {responseListener->getEvent(), tableListener->getEvent()};
                    responseListener->setRequest(request->getRequestID());
                    tableListener->setRequest(request->getRequestID());

                    session->sendRequest(request);

                    DWORD dwRes = uni::WaitForMultipleObjects(2, phEvents, FALSE, 60000);
                    if (dwRes == WAIT_OBJECT_0 + 1L)
                    {
                        // Order was added to orders table.
                        // Tables are locked now (onAdded() has locked them)
                        PrintOrdersTable(ordersTable);
                        tableManager->unlockUpdates();
                    }
                    else if (dwRes == WAIT_TIMEOUT)
                        std::cout << "Die by timeout" << std::endl;

                    request->release();
                }
                else
                    std::cout << "Cannot create request" << std::endl;

                requestFactory->release();
            }
            else
                std::cout << "No valid account" << std::endl;

            ordersTable->unsubscribeUpdate(Insert, tableListener);
            tableListener->release();

            session->unsubscribeResponse(responseListener);
            responseListener->release();

            ordersTable->release();
        }

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

void PrintOrdersTable(IO2GOrdersTable *ordersTable)
{
    std::cout << "Orders table: " << std::endl;
    IO2GOrderTableRow *orderRow = NULL;
    IO2GTableIterator tableIterator = {0, 0, NULL};
    while (ordersTable->getNextRow(tableIterator, orderRow))
    {
        std::cout << "OrderID=" << orderRow->getOrderID() << \
            "; AccountID=" << orderRow->getAccountID() << \
            "; Type=" << orderRow->getType() << \
            "; Status=" << orderRow->getStatus() << \
            "; OfferID=" << orderRow->getOfferID() << \
            "; Amount=" << orderRow->getAmount() << \
            "; BuySell=" << orderRow->getBuySell() << \
            "; Rate=" << orderRow->getRate() << std::endl;
        orderRow->release();
    }
}

std::string GetValidAccount(IO2GAccountsTable *accountsTable, int &iMinAmount)
{
    std::string sAccountID = "";
    bool bFound = false;
    // Find first account with available margin
    IO2GAccountTableRow *accountRow = NULL;
    IO2GTableIterator tableIterator = {0};
    while (accountsTable->getNextRow(tableIterator, accountRow))
    {
        if (accountRow->getMarginCallFlag()[0] == 'N') // No limitations
        {
            sAccountID = accountRow->getAccountID();
            iMinAmount = accountRow->getBaseUnitSize();
            bFound = true;
        }
        accountRow->release();
        if (bFound)
            break;
    }
    return sAccountID;
}
