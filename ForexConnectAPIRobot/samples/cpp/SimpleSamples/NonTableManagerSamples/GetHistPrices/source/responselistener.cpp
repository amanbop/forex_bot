#include <math.h>
#include <time.h>
#include <string>
#include <vector>
#include <map>
#include "../../../../../../include/Order2Go2.h"
#include "responselistener.h"
#include "sessionstatus.h"
#include "stdafx.h"
#include <sstream>
#include <iomanip>

ResponseListener::ResponseListener(IO2GSession *session)
{
    mSession = session;
    mSession->addRef();
    mRefCount = 1;
    mFinished = false;
    mEvent = uni::CreateEvent(0, FALSE, FALSE, 0);
}

ResponseListener::~ResponseListener()
{
    mSession->release();
    uni::CloseHandle(mEvent);
}

/** Increase reference counter. */
long ResponseListener::addRef()
{
    return InterlockedIncrement(&mRefCount);
}

/** Decrease reference counter. */
long ResponseListener::release()
{
    long rc = InterlockedDecrement(&mRefCount);
    if (rc == 0)
        delete this;
    return rc;
}

/** Set request. */
void ResponseListener::setRequest(const char *requestId)
{
    mRequestID = requestId;
    mFinished = false;
}

bool ResponseListener::isFinished() const
{
    return mFinished;
}

void ResponseListener::setTimeBorders(const DATE timeFrom, const DATE timeTo)
{
    mTimeFrom = timeFrom;
    mTimeTo = timeTo;
}

void ResponseListener::getTimeBorders(DATE *timeFrom, DATE *timeTo) const
{
    *timeFrom = mTimeFrom;
    *timeTo = mTimeTo;
}


/** Request execution completed data handler. */
void ResponseListener::onRequestCompleted(const char *requestId, IO2GResponse  *response)
{
    if (response != 0)
    {
        if (response->getType() == MarketDataSnapshot)
        {
            //process updates
            IO2GResponseReaderFactory *factory = mSession->getResponseReaderFactory();
            if (factory == 0)
                return;

            IO2GTimeConverter *timeConverter = mSession->getTimeConverter();
            IO2GMarketDataSnapshotResponseReader *reader = factory->createMarketDataSnapshotReader(response);
            if (reader != 0)
            {
                int size = reader->size();
                bool isBar = reader->isBar();
                for (int i = (size - 1); i >= 0; --i)
                {
                    std::string stime;

                    //DATE dt = timeConverter->convert(reader->getDate(i), IO2GTimeConverter::UTC, IO2GTimeConverter::Local);
                    DATE dt = reader->getDate(i); // print time in UTC     ^- in local time
                    formatDate(dt, stime);

                    if (isBar)
                    {
                        printf("%s %f %f %f %f/%f %f %f %f/%i\n", stime.c_str(),
                               reader->getBidOpen(i), reader->getBidHigh(i), reader->getBidLow(i), reader->getBidClose(i),
                               reader->getAskOpen(i), reader->getAskHigh(i), reader->getAskLow(i), reader->getAskClose(i),
                               reader->getVolume(i));
                    }
                    else
                    {
                        printf("%s %f/%f\n", stime.c_str(), reader->getBid(i), reader->getAsk(i));
                    }

                }
                mTimeTo = reader->getDate(0);
                if(size < 300)
                {
                    mFinished = true;
                }
                reader->release();
            }
            timeConverter->release();
            factory->release();
        }
    }

    if (requestId != 0 && mRequestID.length() > 0 && strcmp(requestId, mRequestID.c_str()) == 0)
    {
        if( (mTimeTo - mTimeFrom) < 0.001 )
            mFinished = true;
        uni::SetEvent(mEvent);
    }
}

/** Request execution failed data handler. */
void ResponseListener::onRequestFailed(const char *requestId , const char *error)
{
    printf("Request %s failed:\n%s\n", requestId, error);
    if (requestId != 0 && mRequestID.length() > 0 && strcmp(requestId, mRequestID.c_str()) == 0)
    {
        mFinished = true;
        uni::SetEvent(mEvent);
    }
}

/** Request update data received data handler. */
void ResponseListener::onTablesUpdates(IO2GResponse *data)
{
}

/** Format date to print */
void ResponseListener::formatDate(DATE d, std::string &buf)
{
    double d_int, d_frac;
    d_frac = modf(d, &d_int);
    time_t t = time_t(d_int - 25569.0) * 86400 + time_t(floor(d_frac * 86400));
    struct tm *t1 = uni_gmtime(&t);

    using namespace std;
    stringstream sstream;
    sstream << setw(4) << t1->tm_year + 1900 << "/" \
            << setw(2) << setfill('0') << t1->tm_mon + 1 << "/" \
            << setw(2) << setfill('0') << t1->tm_mday << " " \
            << setw(2) << setfill('0') << t1->tm_hour << ":" \
            << setw(2) << setfill('0') << t1->tm_min << ":" \
            << setw(2) << setfill('0') << t1->tm_sec;
    buf = sstream.str();
}

