

#include "../include/header.h"
#include "../include/Session.h"
#include "../include/Example.h"

int main(int argc, char *argv[])
{
    const char *user = 0;
    const char *password = 0;
    const char *url = 0;
    const char *connection = 0;
    const char *subsession = 0;
    const char *pin = 0;
    bool autoStep = false;

    if (argc < 5)
    {
        printf("usage: CreateOTO.exe <user> <password> <url> <connection> [subsession] [pin]\n");
        return -1;
        return -1;
    }

    user = argv[1];
    password = argv[2];
    if (argc >= 4)
        url = argv[3];
    else
        url = "http://fxcorporate.com/Hosts.jsp";
    if (argc >= 5)
        connection = argv[4];
    else
        connection = "Demo";
    if (argc >= 6)
        subsession = argv[5];
    if (argc >= 7)
        pin = argv[6];
    if (argc >= 8)
        autoStep = true;

    printf("Login...\n");
    CSession* session = CSession::getInstance();
    session->login(user, password, url, connection, subsession, pin);

    if (session->wait(CSession::Connected) == false)
    {
        printf("%s", session->getError());
        getchar();
    }
    else
    {
        CExample example(*session);
        example.run(autoStep);

        printf("Logout...\n");
        session->logout();
        session->wait(CSession::Disconnected);
    }

    session->release();
};

