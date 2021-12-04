#include "tablelistener.h"

TableListener::TableListener(IO2GTableManager *tableManager)
{
    mTableManager = tableManager;
    mTableManager->addRef();
    mEvent = uni::CreateEvent(0, FALSE, FALSE, 0);
    mRef = 1;
}

TableListener::~TableListener(void)
{
    mTableManager->release();
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

void TableListener::setRequest(const char *requestID)
{
    mRequestID = requestID;
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
    if (row->getTableType() == Orders)
    {
        IO2GOrderRow *orderRow = static_cast<IO2GOrderRow *>(row);
        if (mRequestID == orderRow->getRequestID())
        {
            mTableManager->lockUpdates();
            uni::SetEvent(mEvent);
        }
    }
}

void TableListener::onChanged(const char *rowID, IO2GRow *row)
{
}

void TableListener::onDeleted(const char *rowID, IO2GRow *row)
{
}

