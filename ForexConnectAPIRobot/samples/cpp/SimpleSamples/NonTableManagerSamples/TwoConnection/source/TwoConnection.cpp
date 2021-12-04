// Login02G2.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "SimpleLog.h"
#include "ResponseListener.h"
#include "SessionStatusListener.h"
#include "ResponseQueue.h"
#include "EventListener.h"
#include "signal.h"

const char HOSTS_STR[] = "/Hosts.jsp";

HANDLE g_StopWaitingEvent = NULL;

void SignalHandler(int signalType)
{
    uni::SetEvent(g_StopWaitingEvent);
}

struct LoginData
{
    std::string m_sUserName;
    std::string m_sPassword;
    std::string m_sHost;
    std::string m_sConnection;
    std::string m_sDatabaseID;
    std::string m_sPin;
};

void login(LoginData *pLoginData, LoginData *pLoginData2)
{
    std::cout << "Connection 1 to: " << pLoginData->m_sUserName << " "
              << "*" << " "
              << pLoginData->m_sHost << " "
              << pLoginData->m_sConnection << " "
              << pLoginData->m_sDatabaseID << " "
              << pLoginData->m_sPin << std::endl;

    std::cout << "Connection 2 to: " << pLoginData2->m_sUserName << " "
              << "*" << " "
              << pLoginData2->m_sHost << " "
              << pLoginData2->m_sConnection << " "
              << pLoginData2->m_sDatabaseID << " "
              << pLoginData2->m_sPin << std::endl;

    std::string sFile = pLoginData->m_sUserName + ".log";


    // "How to login" : The events are coming asynchronously.
    //                  They should come in another thread.
    //                  When the event will come, the signal will be sent to the main thread.
    EventListener *eventListener = new EventListener(sFile, g_StopWaitingEvent
                                            /*&responseQueue,
                                            &log,
                                            pSession,
                                            sessionStatusListener->getStatusEvent(),
                                            responseListener->getReponseEvent(),
                                            sessionStatusListener->getConnectedEvent(),
                                            sessionStatusListener->getStopEvent()*/);

    size_t nHostLen = pLoginData->m_sHost.length();

    if (nHostLen && pLoginData->m_sHost[nHostLen-1] == '/')
        pLoginData->m_sHost.erase(nHostLen-1);

    if (pLoginData->m_sHost.find(HOSTS_STR) == std::string::npos)
        pLoginData->m_sHost.append(HOSTS_STR);

    eventListener->login(pLoginData->m_sUserName.c_str(),
                    pLoginData->m_sPassword.c_str(),
                    pLoginData->m_sHost.c_str(),
                    pLoginData->m_sConnection.c_str());

    eventListener->start();

    std::string sFile2 = pLoginData2->m_sUserName + ".log";

    EventListener *eventListener2 = new EventListener(sFile2, g_StopWaitingEvent);

    size_t nHostLen2 = pLoginData2->m_sHost.length();

    if (nHostLen2 && pLoginData2->m_sHost[nHostLen2-1] == '/')
        pLoginData2->m_sHost.erase(nHostLen2-1);

    if (pLoginData2->m_sHost.find(HOSTS_STR) == std::string::npos)
        pLoginData2->m_sHost.append(HOSTS_STR);


    eventListener2->login(pLoginData2->m_sUserName.c_str(),
                    pLoginData2->m_sPassword.c_str(),
                    pLoginData2->m_sHost.c_str(),
                    pLoginData2->m_sConnection.c_str());
    // Disable price updates
    eventListener2->disablePriceUpdates();
    eventListener2->start();

    DWORD dwRes = uni::WaitForSingleObject(g_StopWaitingEvent, INFINITE);
    //uni::SetEvent(g_StopWaitingEvent);

    delete eventListener;
    delete eventListener2;

    delete pLoginData;
    delete pLoginData2;
    uni::CloseHandle(g_StopWaitingEvent);
}

int _tmain(int argc, _TCHAR* argv[])
{
    if ((argc == 2 && _stricmp(*(argv + 1), "help") == 0) || argc < 9)
    {
        std::cout  << "Usage: TwoConnection.exe username1 password1 host1 connection1 username2 password2 host2 connection2 [dbID1] [dbID2] [PIN1] [PIN2]" << std::endl;
        return -1;
    }

    g_StopWaitingEvent = uni::CreateEvent(NULL, TRUE, FALSE, NULL);
    ::signal(SIGINT, SignalHandler);

    LoginData *pLoginData1 = new LoginData();
    LoginData *pLoginData2 = new LoginData();

    pLoginData1->m_sUserName = *(argv + 1);
    pLoginData1->m_sPassword = *(argv + 2);
    pLoginData1->m_sHost = *(argv + 3);
    pLoginData1->m_sConnection = *(argv + 4);

    pLoginData2->m_sUserName = *(argv + 5);
    pLoginData2->m_sPassword = *(argv + 6);
    pLoginData2->m_sHost = *(argv + 7);
    pLoginData2->m_sConnection = *(argv + 8);

    if (argc > 12)
    {
        pLoginData1->m_sDatabaseID = *(argv + 9);
        pLoginData1->m_sPin = *(argv + 11);
        pLoginData2->m_sDatabaseID = *(argv + 10);
        pLoginData2->m_sPin = *(argv + 12);
    }

    login(pLoginData1, pLoginData2);

    return 0;
}

