#include "tablelistener.h"

TableListener::TableListener(void)
{
    mEvent = uni::CreateEvent(0, FALSE, FALSE, 0);
    mRef = 1;
}

TableListener::~TableListener(void)
{
    uni::CloseHandle(mEvent);
}

long TableListener::addRef()
{
    return InterlockedIncrement(&mRef);
}

long TableListener::release()
{
    InterlockedDecrement(&mRef);
    if(mRef == 0)
        delete this;
    return mRef;
}

HANDLE TableListener::getEvent()
{
    return mEvent;
}

void TableListener::onStatusChanged(O2GTableStatus status)
{
    switch(status)
    {
    case Refreshed:
        uni::SetEvent(mEvent);
        break;
    case Refreshing:
    case Initial:
    case Failed:
        break;
    }
}

void TableListener::onAdded(const char *rowID, IO2GRow *row)
{
    IO2GOrderRow *orderRow = static_cast<IO2GOrderRow *>(row);
    for (unsigned int i = 0; i < mVect.size(); ++i)
    {
        if( mVect[i] == orderRow->getRequestID() )
        {
            printf("added order. id: %s\n",orderRow->getOrderID());
            mReqsComplete++;
        }
    }
    if( mReqsComplete == mVect.size() )
    {
        uni::SetEvent(mEvent);
        return;
    }
}

void TableListener::onChanged(const char *rowID, IO2GRow *row)
{
    IO2GOrderRow *orderRow = static_cast<IO2GOrderRow *>(row);
    for (unsigned int i = 0; i < mVect.size(); ++i)
    {
        if( mVect[i] == orderRow->getRequestID() )
        {
            printf("changed order. id: %s\n",orderRow->getOrderID());
            mReqsComplete++;
        }
    }
    if( mReqsComplete == mVect.size() )
    {
        uni::SetEvent(mEvent);
        return;
    }
}

void TableListener::onDeleted(const char *rowID, IO2GRow *row)
{
}

