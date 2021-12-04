#include <string>
#include "sessionstatus.h"
#include "responselistener.h"
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
        printf("usage: CreateEntryAndFindOrder.exe user password url connection [subsession] [pin]\n");
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
    SessionStatusListener *listener = new SessionStatusListener(session, true, subsession.c_str(), pin.c_str());
    session->subscribeSessionStatus(listener);
    listener->resetError();
    printf("connecting...\n");
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
        printf("connected\n");
        bool bAccount = false;
        bool bOffer = false;
        int iAmount = 0;

        IO2GLoginRules *loginRules = session->getLoginRules();

        std::string sAccountID;
        if(loginRules->isTableLoadedByDefault(::Accounts))
        {
            IO2GResponse *response = loginRules->getTableRefreshResponse(::Accounts);
            if(response)
            {
                IO2GResponseReaderFactory *readerFactory = session->getResponseReaderFactory();
                IO2GAccountsTableResponseReader *responseReader = readerFactory->createAccountsTableReader(response);
                for(int i = 0; i < responseReader->size(); ++i)
                {
                    IO2GAccountRow *row = responseReader->getRow(i);
                    if ( row->getMarginCallFlag()[0] == 'N' )
                    {
                        sAccountID = row->getAccountID();
                        iAmount = row->getBaseUnitSize();
                        std::cout << "Using account ID: " << sAccountID << std::endl;
                        bAccount = true;
                    }
                    row->release();
                    if (bAccount)
                        break;
                }
                responseReader->release();
                readerFactory->release();
                response->release();
            }
        }

        std::string sOfferID;
        double dAsk;
        double dPointSize;
        if( bAccount && loginRules->isTableLoadedByDefault(Offers) )
        {
            IO2GResponse *response = loginRules->getTableRefreshResponse(Offers);
            if ( response )
            {
                IO2GResponseReaderFactory *factory = session->getResponseReaderFactory();
                IO2GOffersTableResponseReader *reader = factory->createOffersTableReader(response);
                for ( int i = 0; i < reader->size(); ++i )
                {
                    IO2GOfferRow *row = reader->getRow(i);
                    if ( instrument == row->getInstrument() )
                    {
                        if ( row->getSubscriptionStatus()[0] != 'T' )
                        {
                            std::cout << "Instrument " << instrument << " is not available. status: " \
                                      << row->getSubscriptionStatus() << std::endl;
                        }
                        else
                        {
                            sOfferID = row->getOfferID();
                            dAsk = row->getAsk();
                            dPointSize = row->getPointSize();
                            std::cout << "Instrument " << instrument << " found. ID: " << sOfferID << std::endl;
                            bOffer = true;
                        }
                    }
                    row->release();
                }
                reader->release();
                factory->release();
            }
        }

        if ( bOffer && bAccount )
        {
            std::string sBuySell = O2G2::Buy;
            std::string sLastRequestID;

            IO2GRequestFactory *requestFactory = session->getRequestFactory();
            IO2GValueMap *valuemap = requestFactory->createValueMap();

            valuemap->setString(Command, O2G2::Commands::CreateOrder);
            valuemap->setString(OrderType, O2G2::Orders::LimitEntry);
            valuemap->setString(AccountID, sAccountID.c_str());
            valuemap->setString(OfferID, sOfferID.c_str());
            valuemap->setDouble(Rate, dAsk - ( 10 * dPointSize ) );
            valuemap->setString(BuySell, sBuySell.c_str());
            valuemap->setInt(Amount, iAmount);

            IO2GRequest *request = requestFactory->createOrderRequest(valuemap);
            valuemap->release();

            ResponseListener *responseListener = new ResponseListener(session);
            sLastRequestID = request->getRequestID();
            responseListener->setRequest(sLastRequestID.c_str());
            session->subscribeResponse(responseListener);

            session->sendRequest(request);
            HANDLE aEvents[2];
            aEvents[0] = responseListener->getResponseEvent();
            aEvents[1] = responseListener->getUpdateEvent();
            uni::WaitForMultipleObjects(2, aEvents, TRUE, INFINITE);

            request->release();

            if ( responseListener->getRequestSuccess() )
            {
                IO2GRequest *request = requestFactory->createRefreshTableRequestByAccount(Orders, sAccountID.c_str());

                responseListener->setRequest(request->getRequestID());
                session->sendRequest(request);
                uni::WaitForSingleObject(responseListener->getResponseEvent(), INFINITE);
            }

            session->unsubscribeResponse(responseListener);
            responseListener->release();
            requestFactory->release();
        }
        else
        {
            std::cout << "No valid account or instrument not found" << std::endl;
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
}

