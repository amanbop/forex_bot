#include <string>
#include "../../../../../../include/Order2Go2.h"
#include "sessionstatus.h"
#include "ResponseListener.h"

bool isAccountExists(IO2GSession *session, const char *accountIDToCheck)
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
            for (int i = 0; i < responseReader->size(); ++i)
            {
                O2G2Ptr<IO2GAccountRow> accountRow = responseReader->getRow(i);
                const char* accountID = accountRow->getAccountID();
                if (strcmp(accountID, accountIDToCheck) == 0)
                    return true;
            }
        }
    }
    return false;
}

bool isOrderExists(IO2GSession *session, ResponseListener* responseListener, const char *accountID, const char *orderIDToCheck)
{
    O2G2Ptr<IO2GRequestFactory> factory = session->getRequestFactory();
    if (!factory)
        return false;
    session->sendRequest(factory->createRefreshTableRequestByAccount(Orders, accountID));

    O2G2Ptr<IO2GLoginRules> loginRules = session->getLoginRules();
    O2G2Ptr<IO2GResponse> response = responseListener->getOrdersResponse();
    if (response)
    {
        O2G2Ptr<IO2GResponseReaderFactory> readerFactory = session->getResponseReaderFactory();
        O2G2Ptr<IO2GOrdersTableResponseReader> responseReader = \
            readerFactory->createOrdersTableReader(response);
        for (int i = 0; i < responseReader->size(); ++i)
        {
            O2G2Ptr<IO2GOrderRow> orderRow = responseReader->getRow(i);
            const char* orderID = orderRow->getOrderID();
            if (strcmp(orderID, orderIDToCheck) == 0)
                return true;
        }
    }
    return false;
}

int main(int argc, char *argv[])
{
    const char *user = 0;
    const char *password = 0;
    const char *url = 0;
    const char *connection = 0;
    const char *subsession = 0;
    const char *pin = 0;
    const char *accountID = 0;
    const char *order = 0;
    const char *contingencyID = 0;
    if (argc < 5)
    {
        printf("usage: RemoveFromGroup.exe order account user password [url] [connection] [subsession] [pin]\n");
        return -1;
    }

    order = argv[1];
    accountID = argv[2];
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

        if (isAccountExists(session, accountID)
            && isOrderExists(session, responseListener, accountID, order))
        {
            O2G2Ptr<IO2GRequestFactory> requestFactory = session->getRequestFactory();

            // Main RemoveFromContingencyGroup ValueMap
            O2G2Ptr<IO2GValueMap> mainValueMap = requestFactory->createValueMap();
            mainValueMap->setString(Command, "RemoveFromContingencyGroup");

            // ValueMap for an order to be removed
            O2G2Ptr<IO2GValueMap> valueMap = requestFactory->createValueMap();
            valueMap->setString(OrderID, order);
            valueMap->setString(AccountID, accountID);
            valueMap->setString(Command, "RemoveFromContingencyGroup");
            mainValueMap->appendChild(valueMap);

            // Send request using Main ValueMap
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

