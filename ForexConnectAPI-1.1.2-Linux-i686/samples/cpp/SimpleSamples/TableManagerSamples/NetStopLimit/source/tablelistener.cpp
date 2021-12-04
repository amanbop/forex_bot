#include "tablelistener.h"

TableListener::TableListener(void)
{
    mEvent = uni::CreateEvent(0, FALSE, FALSE, 0);
    mRef = 1;
    ordersState.limit = 0;
    ordersState.stop = 0;
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
    case ::Refreshed:
        uni::SetEvent(mEvent);
        break;
    case ::Refreshing:
    case ::Initial:
    case ::Failed:
        break;
    }
}

void TableListener::onAdded(const char *rowID, IO2GRow *row)
{
    IO2GOrderRow *orderRow = static_cast<IO2GOrderRow *>(row);
    if ( strncmp(orderRow->getType(), O2G2::Orders::LimitEntry, 3) == 0 )
    {
        printf("added LimitEntry order. id: %s\n", orderRow->getOrderID() );
        mLimitOrderID = orderRow->getOrderID();
        ordersState.limit = 1;
        uni::SetEvent(mEvent);
        return;
    }
    if ( strncmp(orderRow->getType(), O2G2::Orders::StopEntry, 3) == 0 )
    {
        printf("added StopEntry order. id: %s\n", orderRow->getOrderID() );
        mStopOrderID = orderRow->getOrderID();
        ordersState.stop = 1;
        uni::SetEvent(mEvent);
        return;
    }
}

void TableListener::onChanged(const char *rowID, IO2GRow *row)
{
    IO2GOrderRow *orderRow = static_cast<IO2GOrderRow *>(row);
    if ( strncmp(orderRow->getType(), O2G2::Orders::LimitEntry, 3) == 0 )
    {
        printf("changed LimitEntry order. id: %s\n", orderRow->getOrderID() );
        mLimitOrderID = orderRow->getOrderID();
        ordersState.limit = 1;
        uni::SetEvent(mEvent);
        return;
    }
    if ( strncmp(orderRow->getType(), O2G2::Orders::StopEntry, 3) == 0 )
    {
        printf("changed StopEntry order. id: %s\n", orderRow->getOrderID() );
        mStopOrderID = orderRow->getOrderID();
        ordersState.stop = 1;
        uni::SetEvent(mEvent);
        return;
    }
}

void TableListener::onDeleted(const char *rowID, IO2GRow *row)
{
    IO2GOrderRow *orderRow = static_cast<IO2GOrderRow *>(row);
    if ( strncmp(orderRow->getType(), O2G2::Orders::LimitEntry, 3) == 0 )
    {
        printf("delete LimitEntry order. id: %s\n", orderRow->getOrderID() );
        mLimitOrderID = orderRow->getOrderID();
        ordersState.limit = 1;
        uni::SetEvent(mEvent);
        return;
    }
    if ( strncmp(orderRow->getType(), O2G2::Orders::StopEntry, 3) == 0 )
    {
        printf("delete StopEntry order. id: %s\n", orderRow->getOrderID() );
        mStopOrderID = orderRow->getOrderID();
        ordersState.stop = 1;
        uni::SetEvent(mEvent);
        return;
    }
}

std::string TableListener::getOrderID( const char *orderType ) const
{
    std::string res = "";
    if( ordersState.limit && ( strncmp(orderType, O2G2::Orders::LimitEntry, 3) == 0 ) )
    {
        res = mLimitOrderID;
    }
    if( ordersState.stop && ( strncmp(orderType, O2G2::Orders::StopEntry, 3) == 0 ) )
    {
        res = mStopOrderID;
    }
    return res;
}

