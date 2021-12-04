#include <string>
#include "../../../../../../include/Order2Go2.h"
#include "sessionstatus.h"
#include "stdafx.h"
#include "tablelistener.h"

std::vector<std::string> CreateOCO(IO2GSession *session, std::string accID, std::string offerID, double dRate, const char SellBuy[2], int amount, int quan);
void RemoveFromContingencyGroup(IO2GSession *session, const std::vector<std::string> &IDs);

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
        printf("usage: CreateOCO.exe instrument user password url connection [subsession] [pin]");
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
            if ( instrument == offerRow->getInstrument() )
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
            // create here
            std::vector<std::string> vec = CreateOCO(session, sAccountID, sOfferID, dAsk - (5 * dPointSize), O2G2::Buy, iAmount, 3);

            if (!vec.empty())
                RemoveFromContingencyGroup(session, vec);
            else
                printf("No orders created\n");
        }
        else
            std::cout << "No valid account or invalid instrument" << std::endl;

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

std::vector<std::string> CreateOCO(IO2GSession *session, std::string accID, std::string offerID, double dRate, const char *SellBuy, int amount, int quan)
{
    std::vector<std::string> vectID(quan, std::string());

    IO2GRequestFactory *requestFactory = session->getRequestFactory();

    // create parent value map
    IO2GValueMap *mainValueMap = requestFactory->createValueMap();
    mainValueMap->setString(Command, O2G2::Commands::CreateOCO);
    // append children: orders
    for (int i = 0; i < quan; ++i)
    {
        IO2GValueMap *valuemap = requestFactory->createValueMap();
        valuemap->setString(Command, O2G2::Commands::CreateOrder);
        valuemap->setString(OrderType, O2G2::Orders::LimitEntry);
        valuemap->setString(AccountID, accID.c_str());          // The identifier of the account the order should be placed for.
        valuemap->setString(OfferID, offerID.c_str());              // The identifier of the instrument the order should be placed for.
        valuemap->setString(BuySell, SellBuy);              // The order direction ("B" for buy, "S" for sell)
        valuemap->setDouble(Rate, dRate );  // The rate at which the order must be filled (below current rate for Buy, above current rate for Sell)
        valuemap->setInt(Amount, amount );                 // The quantity of the instrument to be bought or sold.

        mainValueMap->appendChild(valuemap);
        valuemap->release();
    }

    if( quan > 0 )
    {
        IO2GRequest *request = requestFactory->createOrderRequest(mainValueMap);
        mainValueMap->release();

        for (int i = 0; i < request->getChildrenCount(); ++i)
        {
            IO2GRequest *rq = request->getChildRequest(i);
            vectID[i] = rq->getRequestID();
            rq->release();
        }

        TableListener *tableListener = new TableListener();
        tableListener->setRequest(vectID);

        IO2GTableManager *tableManager = session->getTableManager();
        IO2GOrdersTable *ordersTable = (IO2GOrdersTable *)tableManager->getTable(Orders);
        ordersTable->subscribeUpdate(Insert, tableListener);

        printf("add %d orders to OCO\n", quan);
        session->sendRequest(request);
        request->release();

        uni::WaitForSingleObject(tableListener->getEvent(), INFINITE);
        printf("done\n");

        ordersTable->unsubscribeUpdate(Insert, tableListener);
        ordersTable->release();
        tableManager->release();
        tableListener->release();

        requestFactory->release();
    }
    return vectID;
}

void RemoveFromContingencyGroup(IO2GSession *session, const std::vector<std::string> &vecIDs)
{
    IO2GRequestFactory *requestFactory = session->getRequestFactory();
    IO2GTableManager *tableManager = session->getTableManager();
    IO2GOrdersTable *ordersTable = (IO2GOrdersTable *) tableManager->getTable(::Orders);
    while (ordersTable->getStatus() != Refreshed && ordersTable->getStatus() != Failed)
        uni::Sleep(50);

    int size = ordersTable->size();
    bool bOrderFound = false;

    IO2GValueMap *valuemap = requestFactory->createValueMap();
    valuemap->setString(Command, O2G2::Commands::RemoveFromContingencyGroup);

    for(int i = 0; i < size; ++i)
    {
        IO2GOrderRow *row = ordersTable->getRow(i);
        if( row->getContingencyType() == 1 ) // is in OCO group
        {
            for (unsigned int j = 0; j < vecIDs.size(); ++j)
            {
                if( vecIDs[j] == row->getRequestID() )
                {
                    IO2GValueMap *valuemapChild = requestFactory->createValueMap();
                    valuemapChild->setString(Command, O2G2::Commands::RemoveFromContingencyGroup);
                    valuemapChild->setString(AccountID, row->getAccountID());
                    valuemapChild->setString(OrderID, row->getOrderID());
                    valuemap->appendChild(valuemapChild);
                    valuemapChild->release();
                    bOrderFound = true;
                    break;
                }
            }
        }
        row->release();
    }

    if(bOrderFound)
    {
        IO2GRequest *request = requestFactory->createOrderRequest(valuemap);
        valuemap->release();

        TableListener *tableListener = new TableListener();
        tableListener->setRequest(vecIDs);
        ordersTable->subscribeUpdate(Update, tableListener);

        printf("excluding orders form OCO\n");
        session->sendRequest(request);
        uni::WaitForSingleObject(tableListener->getEvent(), INFINITE );
        printf("done\n");

        ordersTable->unsubscribeUpdate(Update, tableListener);
        tableListener->release();
        request->release();
    }
    else
    {
        printf("order not found\n");
        valuemap->release();
    }
    ordersTable->release();
    tableManager->release();
    requestFactory->release();
}

