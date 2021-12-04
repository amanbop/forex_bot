#include "sessionstatus.h"
#include "responselistener.h"
#include "stdafx.h"

std::string GetValidAccount(IO2GAccountsTable *accountsTable);

int main(int argc, char *argv[])
{
    std::string instrument = "";
    std::string user = "";
    std::string direction = "";
    std::string password = "";
    std::string url = "";
    std::string connection = "";
    std::string subsession = "";
    std::string pin = "";
    if (argc < 6)
    {
        printf("usage: CloseAllPositions.exe instrument direction user password url connection [subsession] [pin]\n");
        printf("       direction is \"B\" for \"Buy\" or \"S\" for \"Sell\".\n");
        return -1;
    }

    instrument = argv[1];
    direction = argv[2];
    user = argv[3];
    password = argv[4];
    if (argc >= 6)
        url = argv[5];
    else
        url = "http://www.fxcorporate.com/Hosts.jsp";
    if (argc >= 7)
        connection = argv[6];
    else
        connection = "Demo";
    if (argc >= 8)
        subsession = argv[7];
    if (argc >= 9)
        pin = argv[8];

    if (direction != O2G2::Buy &&
        direction != O2G2::Sell)
    {
        printf("Ivalid direction. Must be \"%s\" for \"Buy\" or \"%s\" for \"Sell\".\n", O2G2::Buy, O2G2::Sell);
        return -1;
    }

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
        ResponseListener *responseListener = new ResponseListener();
        session->subscribeResponse(responseListener);
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

        IO2GAccountsTable *accountsTable = (IO2GAccountsTable *)tableManager->getTable(Accounts);
        std::string sAccountID = GetValidAccount(accountsTable);
        accountsTable->release();
        
        while (tableManager->getStatus() != TablesLoaded &&
               tableManager->getStatus() != TablesLoadFailed)
            uni::Sleep(50);

        if (tableManager->getStatus() == TablesLoaded)
        {
            IO2GRequestFactory *requestFactory = session->getRequestFactory();
            IO2GValueMap *valuemap = requestFactory->createValueMap();

            valuemap->setString(Command, O2G2::Commands::CreateOrder);
            valuemap->setString(OrderType, O2G2::Orders::TrueMarketClose);
            valuemap->setString(AccountID, sAccountID.c_str());
            valuemap->setString(OfferID, sOfferID.c_str());
            valuemap->setString(NetQuantity, "Y");
            valuemap->setString(BuySell, direction.c_str());

            IO2GRequest *request = requestFactory->createOrderRequest(valuemap);
            valuemap->release();
            requestFactory->release();

            responseListener->setRequest(request->getRequestID());

            session->sendRequest(request);
            uni::WaitForSingleObject(responseListener->getEvent(), INFINITE);
            request->release();
        }

        session->unsubscribeResponse(responseListener);
        responseListener->release();
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

std::string GetValidAccount(IO2GAccountsTable *accountsTable)
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
            bFound = true;
        }
        accountRow->release();
        if (bFound)
            break;
    }
    return sAccountID;
}
