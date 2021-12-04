#include <string>
#include "sessionstatus.h"
#include "responselistener.h"
#include <iostream>
#include "stdafx.h"

int main(int argc, char *argv[])
{
    std::string orderID = "";
    std::string user = "";
    std::string password = "";
    std::string url = "";
    std::string connection = "";
    std::string subsession = "";
    std::string pin = "";

    if (argc < 4)
    {
        printf("usage: SearchInTable.exe orderID user password url connection [subsession] [pin]\n");
        return -1;
    }

    orderID = argv[1];
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

        IO2GLoginRules *loginRules = session->getLoginRules();
        std::string sAccountID = "";
        if (loginRules->isTableLoadedByDefault(Accounts))
        {
            IO2GResponseReaderFactory *responseReaderFactory = session->getResponseReaderFactory();

            IO2GResponse *accountResponse = loginRules->getTableRefreshResponse(Accounts);
            IO2GAccountsTableResponseReader *responseReader = responseReaderFactory->createAccountsTableReader(accountResponse);

            accountResponse->release();
            responseReaderFactory->release();

            IO2GAccountRow *accountRow = responseReader->getRow(0); // Get first account from table

            responseReader->release();
            sAccountID = accountRow->getAccountID();
            accountRow->release();

            IO2GRequestFactory *requestFactory = session->getRequestFactory();
            IO2GRequest *request = requestFactory->createRefreshTableRequestByAccount(Orders, sAccountID.c_str());
            requestFactory->release();

            if (request)
            {
                ResponseListener *responseListener = new ResponseListener(session);
                responseListener->setRequest(request->getRequestID(), orderID.c_str());
                session->subscribeResponse(responseListener);

                session->sendRequest(request);
                uni::WaitForSingleObject(responseListener->getResponseEvent(), INFINITE);
                request->release();

                session->unsubscribeResponse(responseListener);
                responseListener->release();
            }
            else
                std::cout << "Cannot create request" << std::endl;

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

