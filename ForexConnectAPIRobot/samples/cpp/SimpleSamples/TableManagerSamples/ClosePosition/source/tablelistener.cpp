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
}

void TableListener::onAdded(const char *rowID, IO2GRow *row)
{
    IO2GOrderRow *orderRow = static_cast<IO2GOrderRow *>(row);
    if( mRequestID == orderRow->getRequestID() )
    {
        printf("added order. id: %s\n",orderRow->getOrderID());
        uni::SetEvent(mEvent);
    }
}

void TableListener::onChanged(const char *rowID, IO2GRow *row)
{
}

void TableListener::onDeleted(const char *rowID, IO2GRow *row)
{
}
