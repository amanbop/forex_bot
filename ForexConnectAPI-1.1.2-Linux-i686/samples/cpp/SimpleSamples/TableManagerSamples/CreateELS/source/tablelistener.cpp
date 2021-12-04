#include "tablelistener.h"

TableListener::TableListener(void)
{
    mEvent = uni::CreateEvent(0, FALSE, FALSE, 0);
    mOrderID = "";
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
    if (row->getTableType() == Orders)
    {
        IO2GOrderRow *orderRow = static_cast<IO2GOrderRow *>(row);
        if (mRequestID == orderRow->getRequestID())
        {
            std::string sOrderType = orderRow->getType();
            std::cout << "Added order. ID: " << orderRow->getOrderID() << \
                " (" << sOrderType << ")" << std::endl;
            mOrderID = orderRow->getOrderID();
            uni::SetEvent(mEvent);
        }
    }
}

void TableListener::onChanged(const char *rowID, IO2GRow *row)
{
    if (row->getTableType() == Orders)
    {
        IO2GOrderRow *orderRow = static_cast<IO2GOrderRow *>(row);
        if (mRequestID == orderRow->getRequestID())
        {
            std::cout << "Changed order. ID: " << orderRow->getOrderID() << std::endl;
            uni::SetEvent(mEvent);
        }
    }
}

void TableListener::onDeleted(const char *rowID, IO2GRow *row)
{
    if (row->getTableType() == Orders)
    {
        IO2GOrderRow *orderRow = static_cast<IO2GOrderRow *>(row);
        if (mRequestID == orderRow->getRequestID())
        {
            std::cout << "Delete order. ID: " << orderRow->getOrderID() << std::endl;
            uni::SetEvent(mEvent);
        }
    }
}

