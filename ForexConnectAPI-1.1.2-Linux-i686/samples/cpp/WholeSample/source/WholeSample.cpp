// WholeSample.cpp : Defines the entry point for the console application.
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

#ifdef WIN32

BOOL CtrlHandler( DWORD fdwCtrlType )
{
  switch( fdwCtrlType )
  {
    // Handle the CTRL-C signal.
    case CTRL_C_EVENT:
    case CTRL_CLOSE_EVENT:
    case CTRL_BREAK_EVENT:
    case CTRL_LOGOFF_EVENT:
    case CTRL_SHUTDOWN_EVENT:
        SetEvent(g_StopWaitingEvent);
      return TRUE;

    default:
      return FALSE;
  }
}

#else

void SignalHandler(int signalType)
{
    uni::SetEvent(g_StopWaitingEvent);
}

#endif

struct LoginData
{
    std::string m_sUserName;
    std::string m_sPassword;
    std::string m_sHost;
    std::string m_sConnection;
    std::string m_sDatabaseID;
    std::string m_sPin;
};

void login(LoginData *pLoginData)
{
    std::cout << "Connect to: " << pLoginData->m_sUserName << " "
              << "*" << " "
              << pLoginData->m_sHost << " "
              << pLoginData->m_sConnection << " "
              << pLoginData->m_sDatabaseID << " "
              << pLoginData->m_sPin << std::endl;

    std::string sFile = pLoginData->m_sUserName + ".log";

    // "How to login" : Create session.
    IO2GSession *pSession = CO2GTransport::createSession();

    SimpleLog log(sFile.c_str());

    CLoginDataProvider oLoginData(pLoginData->m_sDatabaseID.c_str(),
                                  pLoginData->m_sPin.c_str());

    // "How to login" : Create an instance of session status listener.
    CSessionStatusListener *sessionStatusListener = new CSessionStatusListener(&log, pSession, &oLoginData);
    sessionStatusListener->addRef();

    // "How to login" : Subscribe status listener object to session status.
    pSession->subscribeSessionStatus(sessionStatusListener);

    ResponseQueue responseQueue;

    // "How to login" : Create an instance of session response listener.
    ResponseListener *responseListener = new ResponseListener(&log, &responseQueue);
    responseListener->addRef();

    // "How to login" : Subscribe response listener object to session response.
    pSession->subscribeResponse(responseListener);

    size_t nHostLen = pLoginData->m_sHost.length();

    if (nHostLen && pLoginData->m_sHost[nHostLen-1] == '/')
        pLoginData->m_sHost.erase(nHostLen-1);

    if (pLoginData->m_sHost.find(HOSTS_STR) == std::string::npos)
        pLoginData->m_sHost.append(HOSTS_STR);

    // "How to login" : Login using user ID, password, URL, and connection.
    pSession->login(pLoginData->m_sUserName.c_str(),
                    pLoginData->m_sPassword.c_str(),
                    pLoginData->m_sHost.c_str(),
                    pLoginData->m_sConnection.c_str());

    // "How to login" : The events are coming asynchronously.
    //                  They should come in another thread.
    //                  When the event will come, the signal will be sent to the main thread.
    EventListener *eventListener = new EventListener(&responseQueue,
                                            &log,
                                            pSession,
                                            sessionStatusListener->getStatusEvent(),
                                            responseListener->getReponseEvent(),
                                            sessionStatusListener->getConnectedEvent(),
                                            sessionStatusListener->getStopEvent());

    g_StopWaitingEvent = sessionStatusListener->getStopEvent();

    eventListener->waitNextEvent();

    // "How to login" : Unsubscribe session from status listener.
    pSession->unsubscribeSessionStatus(sessionStatusListener);
    sessionStatusListener->release();

    // "How to login" : Unsubscribe session from response listener.
    pSession->unsubscribeResponse(responseListener);
    responseListener->release();

    delete eventListener;

    if (pSession)
        pSession->release();

    delete pLoginData;
}

int _tmain(int argc, _TCHAR* argv[])
{
    if (argc < 5)
    {
        std::cout  << "Usage: WholeSample username password host connection [databaseID] [PIN]" << std::endl;
        return -1;
    }

#ifdef WIN32
    SetConsoleCtrlHandler((PHANDLER_ROUTINE) CtrlHandler, TRUE);
#else
    ::signal(SIGINT, SignalHandler);
#endif

    LoginData *pLoginData = new LoginData();


    pLoginData->m_sUserName = *(argv + 1);
    pLoginData->m_sPassword = *(argv + 2);
    pLoginData->m_sHost = *(argv + 3);
    pLoginData->m_sConnection = *(argv + 4);

    if (argc >= 6)
        pLoginData->m_sDatabaseID = *(argv + 5);

    if (argc >= 7)
        pLoginData->m_sPin = *(argv + 6);


    login(pLoginData);

    return 0;
}

