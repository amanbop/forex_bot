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
        printf("usage: CloseAllPositions.exe instrument user password url connection [subsession] [pin]\n");
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

    printf("Connecting...\n");
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
            printf("Connection failed: %s\n", listener->getError());
            break;
        }
    }

    if (connected)
    {
        printf("Connected\n");

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
                            std::cout << "Instrument " << instrument << " is not available. Status: " \
                                      << row->getSubscriptionStatus() << std::endl;
                        }
                        else
                        {
                            sOfferID = row->getOfferID();
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
            IO2GRequestFactory *requestFactory = session->getRequestFactory();
            ResponseListener *responseListener = new ResponseListener(session);
            session->subscribeResponse(responseListener);

            IO2GRequest *request = requestFactory->createRefreshTableRequestByAccount(Trades, sAccountID.c_str());
            responseListener->setRequest(request->getRequestID());
            session->sendRequest(request);
            uni::WaitForSingleObject(responseListener->getResponseEvent(), INFINITE);
            request->release();

            std::list<IO2GTradeRow *> *pliTradeRows = responseListener->getTradeRowsList();

            IO2GValueMap *valuemap = requestFactory->createValueMap();
            valuemap->setString(Command, O2G2::Commands::CreateOrder);

            IO2GPermissionChecker *permissionChecker = loginRules->getPermissionChecker();
            std::string sBuySell = "";
            int iAmount = 0;

            for (std::list<IO2GTradeRow *>::iterator i = pliTradeRows->begin(); i != pliTradeRows->end(); ++i)
            {
                IO2GTradeRow *row = *i;
                IO2GValueMap *valuemapChild = requestFactory->createValueMap();

                sBuySell = row->getBuySell();
                iAmount = row->getAmount();

                valuemapChild->setString(Command, O2G2::Commands::CreateOrder);
                valuemapChild->setString(OfferID, sOfferID.c_str());
                valuemapChild->setString(AccountID, row->getAccountID());
                valuemapChild->setString(BuySell, (sBuySell == O2G2::Buy ? O2G2::Sell : O2G2::Buy ) );
                valuemapChild->setInt(Amount, iAmount);

                if ( permissionChecker->canCreateMarketCloseOrder(instrument.c_str()) != PermissionEnabled )
                { // FIFO
                    valuemapChild->setString(OrderType, O2G2::Orders::TrueMarketOpen);
                    valuemapChild->setString(TradeID, row->getTradeID());
                }
                else
                { // Non-FIFO
                    valuemapChild->setString(OrderType, O2G2::Orders::TrueMarketClose);
                }

                valuemap->appendChild(valuemapChild);
                valuemapChild->release();
            }
            permissionChecker->release();

            request = requestFactory->createOrderRequest(valuemap);
            valuemap->release();
            if (request)
            {
                std::vector<std::string> vecRequestID(request->getChildrenCount());

                for (int i = 0; i < request->getChildrenCount(); ++i)
                {
                    IO2GRequest *requestChild = request->getChildRequest(i);
                    vecRequestID[i] = requestChild->getRequestID();
                    requestChild->release();
                }

                responseListener->setRequestVector(&vecRequestID);
                session->sendRequest(request);
                uni::WaitForSingleObject(responseListener->getResponseEvent(), INFINITE);
                request->release();
            }

            session->unsubscribeResponse(responseListener);
            responseListener->release();
            requestFactory->release();
        }
        else
        {
            std::cout << "No valid account or instrument found" << std::endl;
        }

        loginRules->release();

        session->logout();
        printf("Disconnecting...\n");
        while (true)
        {
            uni::WaitForSingleObject(listener->getEvent(), 250);
            if (listener->getStatusCode() == IO2GSessionStatus::Disconnected)
                break;
        }
        printf("Disconnected\n");
    }

    session->unsubscribeSessionStatus(listener);
    listener->release();
    session->release();
}

