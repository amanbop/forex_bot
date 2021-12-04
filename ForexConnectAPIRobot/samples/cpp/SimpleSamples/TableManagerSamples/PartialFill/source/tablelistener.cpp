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
    if (row->getTableType() == Orders)
    {
        IO2GOrderRow *orderRow = (IO2GOrderRow*)row;
        if (mRequestID == orderRow->getRequestID())
        {
            std::cout << "New order is added: OrderID = " << orderRow->getOrderID() << \
                ", Order status = " << orderRow->getStatus() << \
                ", OriginQTY = " << orderRow->getOriginAmount() << \
                ", FilledQTY = " << orderRow->getFilledAmount() << std::endl;
        }
    }
    if (row->getTableType() == Trades)
    {
        IO2GTradeRow *tradeRow = (IO2GTradeRow*)row;
        std::cout << "New open position is added: TradeID = " << tradeRow->getTradeID() << \
            ", TradeIDOrigin = " << tradeRow->getTradeIDOrigin() << std::endl;
    }
}

void TableListener::onChanged(const char *rowID, IO2GRow *row)
{
    if (row->getTableType() == Orders)
    {
        IO2GOrderRow *orderRow = (IO2GOrderRow*)row;
        if (mRequestID == orderRow->getRequestID())
        {
            std::cout << "An order is changed: OrderID = " << orderRow->getOrderID() << \
                ", Order status = " << orderRow->getStatus() << \
                ", OriginQTY = " << orderRow->getOriginAmount() << \
                ", FilledQTY = " << orderRow->getFilledAmount() << std::endl;
        }
    }
}

void TableListener::onDeleted(const char *rowID, IO2GRow *row)
{
    if (row->getTableType() == Orders)
    {
        IO2GOrderRow *orderRow = (IO2GOrderRow*)row;
        if (mRequestID == orderRow->getRequestID())
        {
            std::cout << "An order is going to be removed: OrderID = " << orderRow->getOrderID() << \
                ", Order status = " << orderRow->getStatus() << \
                ", OriginQTY = " << orderRow->getOriginAmount() << \
                ", FilledQTY = " << orderRow->getFilledAmount() << std::endl;
            uni::SetEvent(mEvent);
        }
    }
    if (row->getTableType() == Trades)
    {
        IO2GTradeRow *tradeRow = (IO2GTradeRow*)row;
        std::cout << "New open position is added: TradeID = " << tradeRow->getTradeID() << std::endl;
    }
}

