// forexConnectRobot.cpp : Main file for the application.
//

#include "stdafx.h"
#include "signal.h"
#include "SimpleLog.h"
#include "ResponseListener.h"
#include "SessionStatusListener.h"
#include "ResponseQueue.h"
#include "EventListener.h"

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

main {}
