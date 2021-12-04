#include <stdio.h>
#include <string>
#include <vector>
#include <map>
#include <time.h>
#include <math.h>
#include "../../../../../../include/Order2Go2.h"
#include "offer.h"
#include "responselistener.h"
#include "sessionstatus.h"

ResponseListener::ResponseListener(IO2GSession *session)
{
    mSession = session;
    mSession->addRef();
    mOffers = new OfferCollection();
    mRefCount = 1;
}

ResponseListener::~ResponseListener()
{
    delete mOffers;
    mSession->release();
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

/** Request execution completed data handler. */
void ResponseListener::onRequestCompleted(const char *requestId, IO2GResponse  *response)
{
    if (response != 0)
    {
        if (response->getType() == GetOffers)
        {
            //process updates
            IO2GResponseReaderFactory *factory = mSession->getResponseReaderFactory();
            if (factory == 0)
                return;

            IO2GOffersTableResponseReader *offersReader = factory->createOffersTableReader(response);
            if (offersReader != 0)
            {
                mOffers->clear();
                int size = offersReader->size();

                for (int i = 0; i < size; i++)
                {
                    IO2GOfferRow *offer_row = offersReader->getRow(i);
                    const char *id = offer_row->getOfferID();

                    Offer *offer = new Offer(offer_row->getOfferID(), offer_row->getInstrument(),
                                             offer_row->getDigits(), offer_row->getPointSize(),
                                             offer_row->getTime(), offer_row->getBid(), offer_row->getAsk());
                    mOffers->addOffer(offer);
                    printOffer(offer);
                    offer_row->release();
                }
                offersReader->release();
            }
            factory->release();
        }
    }
}

/** Request execution failed data handler. */
void ResponseListener::onRequestFailed(const char *requestId , const char *error)
{
    printf("Request %s failed:\n%s\n", requestId, error);
}

/** Request update data received data handler. */
void ResponseListener::onTablesUpdates(IO2GResponse *data)
{
    //process updates
    IO2GResponseReaderFactory *factory = mSession->getResponseReaderFactory();
    if (factory == 0)
        return;

    IO2GOffersTableResponseReader *offersReader = factory->createOffersTableReader(data);
    if (offersReader != 0)
    {
        int size = offersReader->size();

        for (int i = 0; i < size; i++)
        {
            IO2GOfferRow *offer_row = offersReader->getRow(i);
            const char *id = offer_row->getOfferID();

            Offer *offer = mOffers->findOfferForUpdate(id);

            if (offer != 0)
            {
                if (offer_row->isTimeValid())
                    offer->setTime(offer_row->getTime());
                if (offer_row->isBidValid())
                    offer->setBid(offer_row->getBid());
                if (offer_row->isAskValid())
                    offer->setAsk(offer_row->getAsk());
                printOffer(offer);
            }
            offer_row->release();
        }
        offersReader->release();
    }

    factory->release();
}

/** Print offer to the console. */
void ResponseListener::printOffer(Offer *offer)
{
    char sbid[32], sask[32], stime[64];
    char format[32];
    sprintf(format, "%%.%if", offer->getPrecision());
    sprintf(sbid, format, offer->getBid());
    sprintf(sask, format, offer->getAsk());
    formatDate(offer->getTime(), stime);
    printf("%s %s bid:=%s ask:=%s\n", offer->getInstrument(), stime, sbid, sask);
}

/** Format date to print */
void ResponseListener::formatDate(DATE d, char *buf)
{
    double d_int, d_frac;
    d_frac = modf(d, &d_int);
    time_t t = time_t(d_int - 25569.0) * 86400 + time_t(floor(d_frac * 86400));
    struct tm *t1 = gmtime(&t);
    sprintf(buf, "%04i/%02i/%02i %02i:%02i:%02i", t1->tm_year + 1900, t1->tm_mon + 1, t1->tm_mday, t1->tm_hour, t1->tm_min, t1->tm_sec);
}

