#include <string>
#include "../../../../../../include/Order2Go2.h"
#include "sessionstatus.h"
#include "stdafx.h"

/** Print table using getNextRow() method */
void PrintTable1(IO2GTradesTable *);

/** Print table using IO2GEachRowListener */
void PrintTable2(IO2GTradesTable *);

int main(int argc, char *argv[])
{
    std::string user = "";
    std::string password = "";
    std::string url = "";
    std::string connection = "";
    std::string subsession = "";
    std::string pin = "";
    if (argc < 3)
    {
        printf("usage: PrintTable.exe user password url connection [subsession] [pin]\n");
        return -1;
    }

    user = argv[1];
    password = argv[2];
    if (argc >= 4)
        url = argv[3];
    else
        url = "http://www.fxcorporate.com/Hosts.jsp";
    if (argc >= 5)
        connection = argv[4];
    else
        connection = "Demo";
    if (argc >= 6)
        subsession = argv[5];
    if (argc >= 7)
        pin = argv[6];

    IO2GSession *session = CO2GTransport::createSession();

    // Enable TableManager usage. Must be before login.
    session->useTableManager(::Yes, NULL);

    SessionStatusListener *listener = new SessionStatusListener(session, true, subsession.c_str(), pin.c_str());
    session->subscribeSessionStatus(listener);
    listener->resetError();

    session->login(user.c_str(), password.c_str(), url.c_str(), connection.c_str());

    bool connected = false;

    while (true)
    {
        uni::WaitForSingleObject(listener->getEvent(), 250);
        if (listener->getStatusCode() == IO2GSessionStatus::Connected)
        {
            connected = true;
            break;
        }
        if (listener->wasError())
        {
            printf("Connection failed: %s", listener->getError());
            break;
        }
    }

    if (connected)
    {
        IO2GTableManager *tableManager = session->getTableManager();
        while (tableManager->getStatus() != TablesLoaded &&
               tableManager->getStatus() != TablesLoadFailed)
            uni::Sleep(50);

        if (tableManager->getStatus() == TablesLoaded)
        {
            IO2GTradesTable *tradesTable = (IO2GTradesTable *)tableManager->getTable(Trades);

            PrintTable1(tradesTable);
            PrintTable2(tradesTable);

            tradesTable->release();
        }

        tableManager->release();
        session->logout();
        while (true)
        {
            uni::WaitForSingleObject(listener->getEvent(), 250);
            if (listener->getStatusCode() == IO2GSessionStatus::Disconnected)
                break;
        }
    }

    session->unsubscribeSessionStatus(listener);
    listener->release();
    session->release();
}

/** Print table using getNextRow() method */
void PrintTable1(IO2GTradesTable *tradesTable)
{
    if (tradesTable)
    {
        std::cout << "TradesTable: " << std::endl;
        IO2GTradeTableRow *tradeRow = NULL;
        IO2GTableIterator tableIterator = {0, 0, NULL};
        while (tradesTable->getNextRow(tableIterator, tradeRow))
        {
            std::cout << "ID: " << tradeRow->getTradeID() << \
                " Direction: " << tradeRow->getBuySell() << \
                " Amount: " << tradeRow->getAmount() << std::endl;
            tradeRow->release();
        }
    }
}

/** Implement IO2GEachRowListener interface */
class EachRowListener : public IO2GEachRowListener
{
private:
    volatile long mRefCount;
public:
    /** Ctor */
    EachRowListener()
    {
        mRefCount = 1;
    }

    /** Dtor */
    ~EachRowListener() { }

    /** IAddRef interace */
    long addRef()
    {
        return InterlockedIncrement(&mRefCount);
    }
    long release()
    {
        long rc = InterlockedDecrement(&mRefCount);
        if (rc == 0)
            delete this;
        return rc;
    }

    /** IO2GEachRowListener interface */
    void onEachRow(const char *rowID, IO2GRow *rowData)
    {
        IO2GTradeTableRow *row = dynamic_cast<IO2GTradeTableRow *>(rowData);
        if (row)
            std::cout << "ID: " << row->getTradeID() << \
                         " Direction: " << row->getBuySell() << \
                         " Amount: " << row->getAmount() << std::endl;
    }
};

/** Print table using IO2GEachRowListener */
void PrintTable2(IO2GTradesTable *tradesTable)
{
    if (tradesTable)
    {
        EachRowListener *eachRowListener = new EachRowListener();
        std::cout << "TradesTable: " << std::endl;
        tradesTable->forEachRow(eachRowListener);
        eachRowListener->release();
    }
}
