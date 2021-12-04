#include <string>
#include <map>
#include <cmath>
#include <ctime>
#include "stdafx.h"
#include "../../../../../../include/Order2Go2.h"
#include "sessionstatus.h"
#include "responselistener.h"

// it's UTC everywhere
// in:  time_t t - day time
// out: DATE * beg - 0:00:00 of the previous trading day for t
void GetPreviousTradingDayLowBorder(const time_t t, DATE *beg)
{
    time_t buf_t = t - ( 24 * 60 * 60 );
    struct tm *ptm = uni_gmtime(&buf_t);
    // such solution affords to avoid checking day/month/year underflow
    if(ptm->tm_wday == 0) // yesterday was Sunday
    {
        time_t buf = buf_t - (2 * 24 * 60 * 60);
        ptm = uni_gmtime(&buf); // get Friday date
    }
    else if(ptm->tm_wday == 6) // yesterday was Saturday
    {
        time_t buf = buf_t - (1 * 24 * 60 * 60);
        ptm = uni_gmtime(&buf); // get Friday date
    }
    ptm->tm_hour = 0;
    ptm->tm_min = 0;
    ptm->tm_sec = 0; // now tm is 0:00:00 of previous trading day.
    *(beg) = (DATE)( floor( (0.0 + mktime(ptm) ) / 86400.0) + 25570.0); // Magic...
}


int main(int argc, char *argv[])
{
    std::string instrument = "";
    std::string tf = "";
    std::string user = "";
    std::string password = "";
    std::string url = "";
    std::string connection = "";
    std::string subsession = "";
    std::string pin = "";
    if (argc < 4)
    {
        printf("usage: GetHistPrices.exe instrument timeframe user password url connection [subsession] [pin]\n");
        printf("       timeframe: t1, m1, m5, m15, m30, H1, H2, H4, H6, H8, H12, D1, W1, M1\n");
        return -1;
    }

    instrument = argv[1];
    tf = argv[2];
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

    IO2GSession *session = CO2GTransport::createSession();
    SessionStatusListener *status_listener = new SessionStatusListener(session, true, subsession.c_str(), pin.c_str());
    ResponseListener *response_listener = new ResponseListener(session);
    session->subscribeSessionStatus(status_listener);
    session->subscribeResponse(response_listener);
    status_listener->resetError();
    printf("connecting...\n");
    session->login(user.c_str(), password.c_str(), url.c_str(), connection.c_str());

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

        IO2GRequestFactory *factory = session->getRequestFactory();

        //find time frame by identifier
        IO2GTimeframeCollection *timeframe_collection = factory->getTimeFrameCollection();
        IO2GTimeframe *timeframe = 0;
        bool found = false;
        int size;
        size = timeframe_collection->size();
        for (int i = 0; i < size; i++)
        {
            IO2GTimeframe *timeframe1 = timeframe_collection->get(i);
            if ( tf == timeframe1->getID())
            {
                timeframe = timeframe1;
                found = true;
                break;
            }
            else
                timeframe1->release();
        }
        timeframe_collection->release();

        if (found)
        {
            DATE timeFrom, timeTo;
            GetPreviousTradingDayLowBorder(time(NULL),&timeFrom);
            timeTo = timeFrom + 1;
            response_listener->setTimeBorders(timeFrom, timeTo);
            while (!response_listener->isFinished())
            {
                //get 300 last candles of the chosen instrument in chosen time frame
                //note: to choose data range use factory->fillMarketDataSnapshotRequestTime(...)  method
                IO2GRequest *request = factory->createMarketDataSnapshotRequestInstrument(instrument.c_str(), timeframe, 300);
                response_listener->getTimeBorders(&timeFrom, &timeTo);
                factory->fillMarketDataSnapshotRequestTime(request, timeFrom, timeTo);
                response_listener->setRequest(request->getRequestID());
                session->sendRequest(request);
                uni::WaitForSingleObject(response_listener->getEvent(), 10000); //wait for 10 sec
                request->release();
            }
            timeframe->release();
            factory->release();
        }
        else
            printf("timeframe %s if not found\n", tf.c_str());

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

