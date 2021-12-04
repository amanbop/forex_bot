#include "stdafx.h"
#include "SimpleLog.h"
#include "SessionStatusListener.h"

/** Constructor.*/
CSessionStatusListener::CSessionStatusListener(SimpleLog *log, IO2GSession *session, CLoginDataProvider *loginDataProvider)
{
    mLoginDataProvider = loginDataProvider;
    mhSyncEvent = uni::CreateEvent(NULL, FALSE, FALSE, NULL);
    mhStopEvent = uni::CreateEvent(NULL, FALSE, FALSE, NULL);
    mhConnectedEvent = uni::CreateEvent(NULL, FALSE, FALSE, NULL);
    mSession = session;
    mSession->addRef();
    mLog = log;
    dwRef = 0;
}

CSessionStatusListener::~CSessionStatusListener()
{
    mSession->release();
    uni::CloseHandle(mhSyncEvent);
    uni::CloseHandle(mhStopEvent);
    uni::CloseHandle(mhConnectedEvent);
}

/** On session status change*/
void CSessionStatusListener::onSessionStatusChanged(O2GSessionStatus status)
{
    HANDLE hSyncEvent = mhSyncEvent;

    switch(status)
    {
      case Connecting:
          logString("Status: Connecting");
          break;

      case Connected:
          // "How to login" : Use status listener onSessionStatusChanged function to capture Connected event.
          logString("Status: Connected");
          hSyncEvent = mhConnectedEvent;
          break;

      case Disconnecting:
          logString("Status: Disconnecting");
          break;

      case Disconnected:
          // "How to login" : Use status listener onSessionStatusChanged function to capture Disconnected event.
          logString("Status: Disconnected");
          hSyncEvent = mhStopEvent;
          break;

      case Reconnecting:
          logString("Status: Reconnecting");
          break;

      case SessionLost:
          logString("Status: Session Lost");
          hSyncEvent = mhStopEvent;
          break;

      // "How to login" : If you have more than one trading sessions or pin is required to login,
      //                  you have to catch the event TradingSessionRequested in onSessionStatusChanged function of your status listener.
      case TradingSessionRequested:
          logString("Status: TradingSessionRequested");
          if (mLoginDataProvider)
          {
            std::string sessionID = mLoginDataProvider->getSessionID().c_str();
            if (sessionID.empty())
            {
                // "How to login" : In that case get IO2GSessionDescriptorCollection.
                IO2GSessionDescriptorCollection *descriptors = mSession->getTradingSessionDescriptors();

                // "How to login" : Then process elements of this collection
                if (descriptors->size() > 0)
                    sessionID = descriptors->get(0)->getID();

                descriptors->release();
            }

            // "How to login" : Finally set trading session using session Id and pin
            mSession->setTradingSession(sessionID.c_str(),
                                        mLoginDataProvider->getPin().c_str());
          }
          break;
    }

    uni::SetEvent(hSyncEvent);
}

void CSessionStatusListener::onLoginFailed(const char *error)
{
    logString(error);
    uni::SetEvent(mhStopEvent);
}


long CSessionStatusListener::addRef()
{
    return InterlockedIncrement(&dwRef);
}

long CSessionStatusListener::release()
{
    LONG dwRes = InterlockedDecrement(&dwRef);
    if (dwRes == 0)
        delete this;
    return dwRes;
}

CLoginDataProvider::CLoginDataProvider(LPCSTR sessionID, LPCSTR pin)
{
    mSessionID = sessionID;
    mPin = pin;
}

std::string CLoginDataProvider::getPin()
{
    return mPin;
}

std::string CLoginDataProvider::getSessionID()
{
    return mSessionID;
}

/** Log string to file.*/
void CSessionStatusListener::logString(LPCSTR logmessage)
{
    mLog->addStringToLog(logmessage);
}

