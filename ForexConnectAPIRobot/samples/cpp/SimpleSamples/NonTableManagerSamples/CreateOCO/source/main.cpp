#include <string>
#include <stdlib.h>
#include "../../../../../../include/Order2Go2.h"
#include "sessionstatus.h"
#include "ResponseListener.h"

IO2GAccountRow *getAccountRow(IO2GSession *session)
{
    O2G2Ptr<IO2GLoginRules> loginRules = session->getLoginRules();
    if (loginRules->isTableLoadedByDefault(::Accounts))
    {
        O2G2Ptr<IO2GResponse> response = loginRules->getTableRefreshResponse(::Accounts);
        if (response)
        {
            O2G2Ptr<IO2GResponseReaderFactory> readerFactory = session->getResponseReaderFactory();
            O2G2Ptr<IO2GAccountsTableResponseReader> responseReader = \
                readerFactory->createAccountsTableReader(response);
            IO2GAccountRow *accountRow = NULL;
            for (int i = 0; i < responseReader->size(); ++i)
            {
                accountRow = responseReader->getRow(i);
                std::string accountKind = accountRow->getAccountKind();
                if (accountKind == "32" || accountKind == "36")
                {
                    // the caller should call release
                    return accountRow;
                }
                accountRow->release();
            }
        }
    }
    return NULL;
}

IO2GOfferRow *findOfferRow(IO2GSession *session, const char *instrument)
{
    O2G2Ptr<IO2GLoginRules> loginRules = session->getLoginRules();
    if (loginRules->isTableLoadedByDefault(::Offers))
    {
        O2G2Ptr<IO2GResponse> response = loginRules->getTableRefreshResponse(::Offers);
        if (response)
        {
            O2G2Ptr<IO2GResponseReaderFactory> readerFactory = session->getResponseReaderFactory();
            O2G2Ptr<IO2GOffersTableResponseReader> responseReader = \
                readerFactory->createOffersTableReader(response);
            const std::string sInstrument = instrument;
            for (int i = 0; i < responseReader->size(); ++i)
            {
                IO2GOfferRow *offerRow = responseReader->getRow(i);

                if (sInstrument == offerRow->getInstrument())
                {
                    // the caller should call release
                    return offerRow;
                }
                offerRow->release();
            }
        }
    }
    return NULL;
}

int main(int argc, char *argv[])
{
    const char *user = 0;
    const char *password = 0;
    const char *url = 0;
    const char *connection = 0;
    const char *subsession = 0;
    const char *pin = 0;
    const char *instrument = 0;
    const char *amountStr = 0;
    if (argc < 5)
    {
        printf("usage: CreateOCO.exe instrument amount user password [url] [connection] [subsession] [pin]\n");
        return -1;
    }

    instrument = argv[1];
    amountStr = argv[2];
    int amount = atoi(amountStr);
    if (amount <= 0)
    {
        printf("Incorrect amount");
        return -1;
    }
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

    IO2GSession *session = CO2GTransport::createSession();
    SessionStatusListener *listener = new SessionStatusListener(session, true, subsession, pin);
    session->subscribeSessionStatus(listener);
    ResponseListener* responseListener = new ResponseListener();
    session->subscribeResponse(responseListener);
    listener->resetError();
    printf("connecting...\n");
    session->login(user, password, url, connection);

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
        printf("connected\n");

        O2G2Ptr<IO2GAccountRow> accountRow = getAccountRow(session);
        O2G2Ptr<IO2GOfferRow> offerRow = findOfferRow(session, instrument);
        if (accountRow && offerRow )
        {
            O2G2Ptr<IO2GRequestFactory> requestFactory = session->getRequestFactory();

            const char *accountID = accountRow->getAccountID();
            const char *offerID = offerRow->getOfferID();
            double pointSize = offerRow->getPointSize();
            // For the purpose of this example we will place entry orders 30 pips from the current market price
            double rateUp = offerRow->getAsk() + 30.0 * pointSize;
            double rateDown = offerRow->getBid() - 30.0 * pointSize;

            // Main OCO ValueMap
            O2G2Ptr<IO2GValueMap> mainValueMap = requestFactory->createValueMap();
            mainValueMap->setString(Command, "CreateOCO");

            // ValueMap for upswing
            O2G2Ptr<IO2GValueMap> valueMapUp = requestFactory->createValueMap();
            valueMapUp->setString(Command, "CreateOrder");
            valueMapUp->setString(OrderType, "SE");
            valueMapUp->setString(AccountID, accountID);
            valueMapUp->setString(OfferID, offerID);
            valueMapUp->setString(BuySell, "B");
            valueMapUp->setDouble(Rate, rateUp);
            valueMapUp->setInt(Amount, amount);
            mainValueMap->appendChild(valueMapUp);

            // ValueMap for downswing
            O2G2Ptr<IO2GValueMap> valueMapDown = requestFactory->createValueMap();
            valueMapDown->setString(Command, "CreateOrder");
            valueMapDown->setString(OrderType, "SE");
            valueMapDown->setString(AccountID, accountID);
            valueMapDown->setString(OfferID, offerID);
            valueMapDown->setString(BuySell, "S");
            valueMapDown->setDouble(Rate, rateDown);
            valueMapDown->setInt(Amount, amount);
            mainValueMap->appendChild(valueMapDown);

            // Send request using Main OCO ValueMap
            O2G2Ptr<IO2GRequest> request = requestFactory->createOrderRequest(mainValueMap);
            session->sendRequest(request);

            uni::WaitForSingleObject(responseListener->getEvent(), 10000);
        }

        session->logout();
        printf("disconnecting...\n");
        while (true)
        {
            uni::WaitForSingleObject(listener->getEvent(), 250);
            if (listener->getStatusCode() == IO2GSessionStatus::Disconnected)
                break;
        }
        printf("disconnected\n");
    }

    session->unsubscribeResponse(responseListener);
    responseListener->release();
    session->unsubscribeSessionStatus(listener);
    listener->release();
    session->release();
}

