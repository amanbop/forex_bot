#include <string>
#include <map>
#include "../../../../../../include/Order2Go2.h"
#include "sessionstatus.h"
#include "responselistener.h"

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
        printf("usage: GetOffers.exe user password url connection [subsession] [pin]\n");
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

    IO2GSession *session = CO2GTransport::createSession();
    SessionStatusListener *status_listener = new SessionStatusListener(session, true, subsession, pin);
    ResponseListener *response_listener = new ResponseListener(session);
    session->subscribeSessionStatus(status_listener);
    session->subscribeResponse(response_listener);
    status_listener->resetError();
    printf("connecting...\n");
    session->login(user, password, url, connection);

    bool connected = false;

    while (true)
    {
        uni::WaitForSingleObject(status_listener->getEvent(), 250);
        if (status_listener->getStatusCode() == IO2GSessionStatus::Connected)
        {
            connected = true;
            break;
        }
        if (status_listener->wasError())
        {
            printf("Connection failed: %s", status_listener->getError());
            break;
        }
    }

    if (connected)
    {
        printf("connected\n");

        //subscribe for offer updates if offers aren't subscribed automatically
        IO2GLoginRules *rules = session->getLoginRules();
        if (!rules->isTableLoadedByDefault(Offers))
        {
            //subscribe
            IO2GRequestFactory *factory = session->getRequestFactory();
            IO2GRequest *request = factory->createRefreshTableRequest(Offers);
            session->sendRequest(request);
            request->release();
            factory->release();
        }
        else
        {
            //process response in case the offers are already subscribed
            IO2GResponse *response = rules->getTableRefreshResponse(Offers);
            response_listener->onRequestCompleted("", response);
            response->release();
        }

        //do nothing 10 seconds, lets all work
        Sleep(10000);

        session->logout();
        printf("disconnecting...\n");
        while (true)
        {
            uni::WaitForSingleObject(status_listener->getEvent(), 250);
            if (status_listener->getStatusCode() == IO2GSessionStatus::Disconnected)
                break;
        }
        printf("disconnected\n");
    }

    session->unsubscribeResponse(response_listener);
    response_listener->release();
    session->unsubscribeSessionStatus(status_listener);
    status_listener->release();
    session->release();
}

