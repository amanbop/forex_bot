#include <string>
#include "sessionstatus.h"
#include "responselistener.h"
#include <iostream>
#include "stdafx.h"
#include <cmath>

std::string GetEntryOrderType(double dBid, double dAsk, double dOrderPrice, std::string sSide, double dPointSize, int iCondDistLimit, int iCondDistStop);
int myRound(double);

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
        printf("usage: CreateEntry.exe user password url connection [subsession] [pin]\n");
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

        IO2GLoginRules *loginRules = session->getLoginRules();

        std::string sAccountID;
        int iBaseUnitSize = 0;
        bool bAccount = false;

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
                    if (row->getMarginCallFlag()[0] == 'N')
                    {
                        sAccountID = row->getAccountID();
                        iBaseUnitSize = row->getBaseUnitSize();
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
        bool bOffer = false;

        if(bAccount && loginRules->isTableLoadedByDefault(Offers))
        {
            IO2GResponse *response = loginRules->getTableRefreshResponse(Offers);
            if (response)
            {
                IO2GResponseReaderFactory *factory = session->getResponseReaderFactory();
                IO2GOffersTableResponseReader *reader = factory->createOffersTableReader(response);
                for (int i = 0; i < reader->size(); ++i)
                {
                    IO2GOfferRow *row = reader->getRow(i);
                    if (instrument == row->getInstrument())
                    {
                        if (row->getSubscriptionStatus()[0] != 'T')
                        {
                            std::cout << "Instrument " << instrument << " is not available. status: " \
                                      << row->getSubscriptionStatus() << std::endl;
                        }
                        else
                        {
                            sOfferID = row->getOfferID();
                            dAsk = row->getAsk();
                            dBid = row->getBid();
                            dPointSize = row->getPointSize();
                            std::cout << "Instrument " << instrument << " found. ID: " << sOfferID << std::endl;
                            bOffer = true;
                        }
                    }
                    row->release();
                    if (bOffer)
                        break;
                }
                reader->release();
                factory->release();
                response->release();
            }
        }

        if (bOffer && bAccount)
        {
            IO2GTradingSettingsProvider *tradingSettingsProvider = loginRules->getTradingSettingsProvider();
            int iCondDistEntryLimit = tradingSettingsProvider->getCondDistEntryLimit(instrument.c_str());
            int iCondDistEntryStop = tradingSettingsProvider->getCondDistEntryStop(instrument.c_str());
            tradingSettingsProvider->release();

            double dAmendment = dPointSize * 10;

            //Buy, Stop Entry
            std::string sSide = O2G2::Buy;
            double dOrderPrice = dAsk + dAmendment;

            //Buy, Limit Entry
            //std::string sSide = O2G2::Buy;
            //double dOrderPrice = dAsk - dAmendment;

            //Sell, Stop Entry
            //std::string sSide = O2G2::Sell;
            //double dOrderPrice = dBid - dAmendment;

            //Sell, Limit Entry
            //std::string sSide = O2G2::Sell;
            //double dOrderPrice = dBid + dAmendment;

            std::string sEntryOrderType = GetEntryOrderType(dBid, dAsk, dOrderPrice, sSide, dPointSize, iCondDistEntryLimit, iCondDistEntryStop);

            IO2GRequestFactory *requestFactory = session->getRequestFactory();
            IO2GValueMap *valueMap = requestFactory->createValueMap();
            valueMap->setString(Command, O2G2::Commands::CreateOrder);
            valueMap->setString(OrderType, sEntryOrderType.c_str());
            valueMap->setDouble(Rate, dOrderPrice);
            valueMap->setString(OfferID, sOfferID.c_str());
            valueMap->setString(BuySell, sSide.c_str());
            valueMap->setString(AccountID, sAccountID.c_str());
            valueMap->setInt(Amount, iBaseUnitSize);

            IO2GRequest *request = requestFactory->createOrderRequest(valueMap);
            valueMap->release();

            ResponseListener *responseListener = new ResponseListener(session);
            responseListener->setRequest(request->getRequestID());
            session->subscribeResponse(responseListener);

            session->sendRequest(request);

            uni::WaitForSingleObject(responseListener->getResponseEvent(), INFINITE);
            request->release();
            session->unsubscribeResponse(responseListener);
            responseListener->release();
            requestFactory->release();
        }
        else
        {
            std::cout << "No valid account or instrument not found" << std::endl;
        }

        loginRules->release();

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

int myRound(double dValue)
{
    int iResult = static_cast<int>(dValue);
    double dFraction = dValue - iResult;

    if (dFraction >= 0.5)
        return iResult++;

    if (dFraction <= -0.5)
        return iResult--;

    return iResult;
}

std::string GetEntryOrderType(double dBid, double dAsk, double dOrderPrice, std::string sSide, double dPointSize, int iCondDistLimit, int iCondDistStop)
{
    double dAbsoluteDifference = 0.0;
    if (sSide == O2G2::Buy)
        dAbsoluteDifference = dOrderPrice - dAsk;
    else
        dAbsoluteDifference = dBid - dOrderPrice;

    int iDifferenceInPips = myRound(dAbsoluteDifference / dPointSize);

    if (iDifferenceInPips >= 0)
    {
        if (iDifferenceInPips <= iCondDistStop)
            return "";
        return O2G2::Orders::StopEntry;
    }
    else
    {
        if (-iDifferenceInPips <= iCondDistLimit)
            return "";
        return O2G2::Orders::LimitEntry;
    }
}

