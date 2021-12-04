#include <string>
#include "../../../../../../include/Order2Go2.h"
#include "sessionstatus.h"

int main(int argc, char *argv[])
{
    const char *user = 0;
    const char *password = 0;
    const char *url = 0;
    const char *connection = 0;
    const char *subsession = 0;
    const char *pin = 0;
    if (argc < 3)
    {
        printf("usage: Login.exe user password url connection [subsession] [pin]\n");
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
    SessionStatusListener *listener = new SessionStatusListener(session, true, subsession, pin);
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

        IO2GLoginRules *loginRules = session->getLoginRules();
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
                    IO2GAccountRow *accountRow = responseReader->getRow(i);
                    printf("%02d: %s\n",i+1,accountRow->getAccountName());
                    accountRow->release();
                }
                responseReader->release();
                readerFactory->release();
                response->release();
            }
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

