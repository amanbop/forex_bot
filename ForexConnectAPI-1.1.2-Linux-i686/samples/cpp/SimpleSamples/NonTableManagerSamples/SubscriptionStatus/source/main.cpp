#include "stdafx.h"
#include "sessionstatus.h"
#include "SimpleLog.h"
#include "ResponseListener.h"

void updateMargin(const char* instrument, IO2GSession *session, ResponseListener *responseListener);

int main(int argc, char* argv[])
{
    const char *user = 0;
    const char *password = 0;
    const char *url = 0;
    const char *connection = 0;
    const char *instrument = 0;
    const char *status = 0;

    if (argc < 5 || (argv[2][0] != 'V' && argv[2][0] != 'T' && argv[2][0] != 'D'))
    {
        printf("usage: SubscriptionStatus.exe instrument status user password [url] [connection]\n");
        printf("status: 'D' - Disabled\n");
        printf("        'T' - Available for trade\n");
        printf("        'V' - View only\n");
        return -1;
    }

    instrument = argv[1];
    status = argv[2];
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

    printf("Symbol: %s\n",instrument);

    IO2GSession *session = CO2GTransport::createSession();
    SessionStatusListener *listener = new SessionStatusListener(session, true, NULL, NULL); // subsession, pin
    session->subscribeSessionStatus(listener);
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

        // Check selected instrument. And print list of all instruments and their status at the same time.
        IO2GResponseReaderFactory *responseReader = session->getResponseReaderFactory();
        IO2GLoginRules *loginRules = session->getLoginRules();
        bool bInstrumentFound = false;
        char szOfferID[256];
        if (loginRules && loginRules->isTableLoadedByDefault(::Offers))
        {
            IO2GResponse *offersResponse = loginRules->getTableRefreshResponse(::Offers);
            IO2GOffersTableResponseReader *offersTableResponseReader = responseReader->createOffersTableReader(offersResponse);
            for (int i = 0; i < offersTableResponseReader->size(); ++i)
            {
                IO2GOfferRow *offerRow = offersTableResponseReader->getRow(i);
                if (strncmp(offerRow->getInstrument(), instrument, 256) == 0)
                {
                    bInstrumentFound = true;
                    strcpy(szOfferID,offerRow->getOfferID());
                }
                switch(offerRow->getSubscriptionStatus()[0])
                {
                case 'V':
                    printf("%s : [V]iew only\n",offerRow->getInstrument());
                    break;
                case 'D':
                    printf("%s : [D]isabled\n",offerRow->getInstrument());
                    break;
                case 'T':
                    printf("%s : Available for [T]rade\n",offerRow->getInstrument());
                    break;
                default:
                    printf("%s : %s\n",offerRow->getInstrument(),offerRow->getSubscriptionStatus());
                }
                offerRow->release();
            }
            offersTableResponseReader->release();
            offersResponse->release();
            loginRules->release();
        }
        responseReader->release();
        // Form and send request
        if(bInstrumentFound)
        {
            IO2GRequestFactory *factory = session->getRequestFactory();

            IO2GValueMap *valueMap = factory->createValueMap();
            valueMap->setString(::Command, O2G2::Commands::SetSubscriptionStatus);
            valueMap->setString(::SubscriptionStatus, status);
            valueMap->setString(::OfferID, szOfferID);

            IO2GRequest *request = factory->createOrderRequest(valueMap);
            valueMap->release();
            SimpleLog *simpleLog = new SimpleLog("subscription.log");
            ResponseListener *responseListener = new ResponseListener(simpleLog);

            responseListener->setRequest(request->getRequestID());
            session->subscribeResponse(responseListener);
            session->sendRequest(request);
            request->release();

            HANDLE phEvents[2];
            phEvents[0] = responseListener->getReponseEvent();
            phEvents[1] = responseListener->getFailureEvent();
            uni::WaitForMultipleObjects(2, phEvents, FALSE, 10000); // wait 10 seconds for response
            updateMargin(instrument, session, responseListener);

            factory->release();
            session->unsubscribeResponse(responseListener);
            responseListener->release();
            if(simpleLog)
                delete simpleLog;
        }
        else
        {
            printf("Instrument '%s' not found!\n", instrument);
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
    session->unsubscribeSessionStatus(listener);
    listener->release();
    session->release();
    if(!connected)
        return -1;
    return 0;
}

void updateMargin(const char* instrument, IO2GSession *session, ResponseListener *responseListener)
{
    O2G2Ptr<IO2GLoginRules> loginRules = session->getLoginRules();
    O2G2Ptr<IO2GTradingSettingsProvider> tradingSetting  = loginRules->getTradingSettingsProvider();
    // Gets account
    O2G2Ptr<IO2GResponse> accountsResponse = loginRules->getTableRefreshResponse(::Accounts);
    O2G2Ptr<IO2GResponseReaderFactory> responseFactory = session->getResponseReaderFactory();
    O2G2Ptr<IO2GAccountsTableResponseReader> accounts = responseFactory->createAccountsTableReader(accountsResponse);
    O2G2Ptr<IO2GAccountRow> account = accounts->getRow(0);
    double mmr,emr,lmr;
    tradingSetting->getMargins(instrument, account, mmr, emr, lmr);
    printf("Margin requirements: after subscription %f %f %f\n", mmr, emr, lmr);

    // Create update request
    O2G2Ptr<IO2GRequestFactory> requestFactory = session->getRequestFactory();
    O2G2Ptr<IO2GValueMap> valueMap = requestFactory->createValueMap();
    valueMap->setString(::Command, O2G2::Commands::UpdateMarginRequirements);

    O2G2Ptr<IO2GRequest> updateMarginRequest = requestFactory->createOrderRequest(valueMap);
    responseListener->setRequest(updateMarginRequest->getRequestID());
    session->sendRequest(updateMarginRequest);
    // Wait response
    HANDLE phEvents[2];
    phEvents[0] = responseListener->getReponseEvent();
    phEvents[1] = responseListener->getFailureEvent();
    int index = uni::WaitForMultipleObjects(2, phEvents, FALSE, 10000); // wait 10 seconds for response
    // Successfully
    if (index == 0)
    {
        O2G2Ptr<IO2GResponse> marginRequirementsResponse = responseListener->getLastRespnonse();
        responseFactory->processMarginRequirementsResponse(marginRequirementsResponse);
        tradingSetting->getMargins(instrument, account, mmr, emr, lmr);
        printf("Margin requirements: after margin requirements has been updated %f %f %f\n", mmr, emr, lmr);
    }
    else
        printf("The update margin requirements request is failed\n");


}

