#include <string>
#include "../../../../../../include/Order2Go2.h"
#include "sessionstatus.h"
#include "responselistener.h"
#include "stdafx.h"

inline void mySleep(unsigned int msec)
{
    #ifdef WIN32
    Sleep((DWORD)msec);
    #else
    usleep(1000 * msec);
    #endif
}

IO2GRequest *CreateOrder(IO2GSession *session, std::string accID, std::string offerID, const char *SellBuy, double dRate, const char *orderType);

int main(int argc, char *argv[])
{
    std::string instrument = "";
    std::string user = "";
    std::string password = "";
    std::string url = "";
    std::string connection = "";
    std::string subsession = "";
    std::string pin = "";
    if (argc < 4)
    {
        printf("usage: NetStopLimit.exe instrument user password url connection [subsession] [pin]\n");
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

        IO2GLoginRules *loginRules = session->getLoginRules();

        int iAmount = 0;
        std::string sAccountID;
        if(loginRules->isTableLoadedByDefault(::Accounts))
        {
            IO2GResponse *response = loginRules->getTableRefreshResponse(::Accounts);
            if(response)
            {
                IO2GResponseReaderFactory *readerFactory = session->getResponseReaderFactory();
                IO2GAccountsTableResponseReader *responseReader = \
                    readerFactory->createAccountsTableReader(response);
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
        double dAsk = 0;
        double dBid = 0;
        double dPointSize = 0;

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
                            std::cout << "Instrument " << instrument << " found. ID: " << sOfferID << std::endl;
                            dAsk = row->getAsk();
                            dBid = row->getBid();
                            dPointSize = row->getPointSize();
                            bOffer = true;
                        }
                    }
                    row->release();
                    if (bOffer)
                        break;
                }
                reader->release();
                factory->release();
            }
        }

        if ( bOffer && bAccount )
        {
            ResponseListener *responseListener = new ResponseListener(session);
            session->subscribeResponse(responseListener);

            IO2GRequest *request = 0;

            request = CreateOrder(session, sAccountID, sOfferID, "B", dAsk - 30 * dPointSize, O2G2::Orders::LimitEntry);
            responseListener->setRequest(request->getRequestID());
            session->sendRequest(request);

            uni::WaitForSingleObject(responseListener->getResponseEvent(), INFINITE);
            request->release();

            request = CreateOrder(session, sAccountID, sOfferID, "B", dAsk + 30 * dPointSize, O2G2::Orders::StopEntry);
            responseListener->setRequest(request->getRequestID());
            session->sendRequest(request);

            uni::WaitForSingleObject(responseListener->getResponseEvent(), INFINITE);
            request->release();

            responseListener->release();
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

IO2GRequest *CreateOrder(IO2GSession *session, std::string accID, std::string offerID, const char *SellBuy, double dRate, const char* orderType)
{
    IO2GRequestFactory *requestFactory = session->getRequestFactory();
    IO2GValueMap *valuemap = requestFactory->createValueMap();

    valuemap->setString(Command, O2G2::Commands::CreateOrder);
    valuemap->setString(OrderType, orderType);
    valuemap->setString(AccountID, accID.c_str());
    valuemap->setString(OfferID, offerID.c_str());
    valuemap->setString(BuySell, SellBuy);
    valuemap->setDouble(Rate, dRate );
    valuemap->setString(NetQuantity, "Y");

    IO2GRequest *request = requestFactory->createOrderRequest(valuemap);
    valuemap->release();
    requestFactory->release();
    return request;
}

