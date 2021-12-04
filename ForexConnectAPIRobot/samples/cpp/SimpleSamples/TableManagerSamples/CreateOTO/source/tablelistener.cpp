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
        IO2GOrderRow *orderRow = static_cast<IO2GOrderRow *>(row);
        ::threading::Mutex::Lock l(mMutex);
        for (std::list<std::string>::iterator it = mliRequestID.begin(); it != mliRequestID.end(); ++it)
        {
            if (*it == orderRow->getRequestID())
            {
                mliRequestID.erase(it);
                std::cout << "Added order ID " << orderRow->getOrderID() << std::endl;
                break;
            }
        }
        if (mliRequestID.empty())
            uni::SetEvent(mEvent);
    }
}

void TableListener::onChanged(const char *rowID, IO2GRow *row)
{
}

void TableListener::onDeleted(const char *rowID, IO2GRow *row)
{
}

