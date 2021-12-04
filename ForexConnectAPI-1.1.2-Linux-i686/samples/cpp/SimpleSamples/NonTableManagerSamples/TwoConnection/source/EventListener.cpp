#include "stdafx.h"
#include "SimpleLog.h"
#include "ResponseListener.h"
#include "SessionStatusListener.h"
#include "ResponseQueue.h"
#include "EventListener.h"
#include <sstream>
#include <math.h>
#include "CreateOrderSample.h"
#include "PermissionCheckerSample.h"
#include "TradingSettingsProviderSample.h"

std::string dateToString(DATE date)
{
    char time[512];
    char dateBuffer[512];
    SYSTEMTIME st = {0};
    EventListener::VariantTimeToSystemTime(date, &st);
    GetTimeFormat(LOCALE_USER_DEFAULT, 0, &st, NULL, time, 256);
    GetDateFormat(LOCALE_USER_DEFAULT, 0, &st, NULL, dateBuffer, 256);

    return std::string(dateBuffer) + std::string(" ") + std::string(time);
}

std::string boolToString(bool bBool)
{
    return (bBool) ? "true" : "false";
}

/** */
EventListener::EventListener(ResponseQueue *responses,
                             SimpleLog *log,
                             IO2GSession *session,
                             HANDLE status,
                             HANDLE response,
                             HANDLE connected,
                             HANDLE stop)
{
    mResponses = responses;
    mStatusHandle = status;
    mResponseHandle = response;
    mConnectedHandle = connected;
    mStopHandle = stop;
    mLog = log;
    mSession = session;
    mOrderSample = new CreateOrderSample(mLog, mSession);
    mSession->addRef();
    mPermissionSample = new PermissionCheckerSample(mLog, mSession);
    mTradingSettings = new TradingSettingsProviderSample(mLog, mSession);
}

EventListener::EventListener(std::string sFile, HANDLE stopEvent)
{
    mSession = CO2GTransport::createSession();
    mLog = new SimpleLog(sFile.c_str());

    mLoginData = new CLoginDataProvider("", "");

    mSessionStatusListener = new CSessionStatusListener(mLog, mSession, mLoginData);
    mSessionStatusListener->addRef();
    mSession->subscribeSessionStatus(mSessionStatusListener);

    mResponses = new ResponseQueue;

    // "How to login" : Create an instance of session response listener.
    mResponseListener = new ResponseListener(mLog, mResponses);
    mResponseListener->addRef();
    mSession->subscribeResponse(mResponseListener);

    mStatusHandle = mSessionStatusListener->getStatusEvent();
    mResponseHandle = mResponseListener->getReponseEvent();
    mConnectedHandle = mSessionStatusListener->getConnectedEvent();
    mStopHandle = mSessionStatusListener->getStopEvent();
    mStopAllHandle = stopEvent;

    mOrderSample = new CreateOrderSample(mLog, mSession);
    mPermissionSample = new PermissionCheckerSample(mLog, mSession);
    mTradingSettings = new TradingSettingsProviderSample(mLog, mSession);
}

EventListener::~EventListener()
{
    AThread::join();
    delete mTradingSettings;
    delete mPermissionSample;
    delete mLoginData;
    if (mOrderSample)
        delete mOrderSample;
    mSession->unsubscribeSessionStatus(mSessionStatusListener);
    mSessionStatusListener->release();
    mSession->unsubscribeResponse(mResponseListener);
    mResponseListener->release();
    delete mResponses;
    delete mLog;
    mSession->release();
}

void EventListener::login(const char *user, const char *pwd, const char *url, const char *connection)
{
    mSession->login(user, pwd, url, connection);
}

int EventListener::run(void *)
{
    waitNextEvent();
    return 0;
}

/** Event listener*/
void EventListener::waitNextEvent()
{
    HANDLE aHandles[5];
    aHandles[0] = mStatusHandle;
    aHandles[1] = mConnectedHandle;
    aHandles[2] = mResponseHandle;
    aHandles[3] = mStopHandle;
    aHandles[4] = mStopAllHandle;

    bool bContinue = true;
    while (bContinue)
    {
        DWORD dwRes = uni::WaitForMultipleObjects(5, aHandles, FALSE, INFINITE);
        if (dwRes == WAIT_OBJECT_0)
            onStatusChanged();
        else if (dwRes == WAIT_OBJECT_0 + 1)
            onConnected();
        else if (dwRes == WAIT_OBJECT_0 + 2)
            onResponse();
        else if (dwRes == WAIT_OBJECT_0 + 3)
        {

            // "How to login" : Logout when you are done.
            mSession->logout();
            dumpLog();

            bContinue = false;
        }
        else if (dwRes == WAIT_OBJECT_0 + 4)
        {
          mSession->logout();
          dumpLog();
          bContinue = false;
        }
        else
            dumpLog();

    }
}

void EventListener::onResponse()
{

    IO2GResponse *response = mResponses->pop();
    while (response)
    {
        if (response->getType() == MarketDataSnapshot)
            onMarketDataSnaphsotRecieved(response);
        else if (response->getType() == GetOffers || response->getType() == TablesUpdates)
        {
            onOffersRecieved(response);
            if (response->getType() == TablesUpdates)
                onTableUpdateReceive(response);
        }
        else if (response->getType() == GetOrders)
            onOrderReceived(response);
        else if (response->getType() == CreateOrderResponse)
            onOrderResponse(response);
        else if (response->getType() == GetTrades)
            onTradeReceived(response);
        else if (response->getType() == GetClosedTrades)
            onClosedTradeReceived(response);

        response->release();
        response = mResponses->pop();
    }
}

void EventListener::onConnected()
{
    dumpLog();
    // Show log time frames.
    logTimeFrames();
    // Send history data.
    sendMarketDataSnapshot();
    // Show login rules.
    IO2GLoginRules *loginRules = mSession->getLoginRules();
    // Has been received offers table snapshot?
    if (loginRules && loginRules->isTableLoadedByDefault(Offers))
    {
        IO2GResponse *response = loginRules->getTableRefreshResponse(Offers);
        onOffersRecieved(response);
        response->release();
    }
    else
        sendGetOffers();
    // Has been received accounts table snapshot?
    if (loginRules && loginRules->isTableLoadedByDefault(Accounts))
    {
        IO2GResponse *response = loginRules->getTableRefreshResponse(Accounts);
        onAccountsReceived(response);
        response->release();
    }


    if (loginRules)
    {
        // Get system properties from login rules.
        IO2GResponse *systemPropertiesResponse = loginRules->getSystemPropertiesResponse();
        onSystemPropertiesReceived(systemPropertiesResponse);
        systemPropertiesResponse->release();
        loginRules->release();

        // Create sample order.
        mOrderSample->prepareParamsFromLoginRules(loginRules);
        mOrderSample->createOrders();

        // print permissions
        mPermissionSample->printPermissions();
        // print trading settings
        mTradingSettings->printSettings();
    }
}

void EventListener::onStatusChanged()
{
    dumpLog();
}

/** Dump stirng to log.*/
void EventListener::dumpLog()
{
    mLog->dumpLog();
}

/** Logs time stamps*/
void EventListener::logTimeFrames()
{
    IO2GRequestFactory *factory = mSession->getRequestFactory();
    IO2GTimeframeCollection *timeFrames = factory->getTimeFrameCollection();
    char szBuffer[256];
    for (int i = 0; i < timeFrames->size(); i++)
    {
        IO2GTimeframe *timeFrame = timeFrames->get(i);
        sprintf_s(szBuffer, sizeof(szBuffer), "Time frame id=%s unit=%d size=%d\n", timeFrame->getID(), timeFrame->getUnit(), timeFrame->getSize());
        mLog->logString(szBuffer);
        timeFrame->release();
    }
    mLog->dumpLog();
    timeFrames->release();
    factory->release();
}

/** sendMarketDataSnapshotRequest*/
void EventListener::sendMarketDataSnapshot()
{
    IO2GRequestFactory *factory = mSession->getRequestFactory();
    IO2GTimeframeCollection *timeFrames = factory->getTimeFrameCollection();
    IO2GTimeframe *timeFrame;/// = timeFrames->get("m15");
    IO2GRequest *getMarketDataSnapshot;// = factory->createMarketDataSnapshotRequestInstrument("EUR/USD", timeFrame, 300);
    //for (int i = 0 ;i < timeFrames->size(); i++)
    {
        timeFrame = timeFrames->get(0);
        getMarketDataSnapshot = factory->createMarketDataSnapshotRequestInstrument("EUR/USD", timeFrame, 300);
        if (getMarketDataSnapshot == NULL)
            return;
        factory->fillMarketDataSnapshotRequestTime(getMarketDataSnapshot/*, dateFrom, dateTo*/);
        mSession->sendRequest(getMarketDataSnapshot);
        getMarketDataSnapshot->release();
        timeFrame->release();
    }
    timeFrames->release();
    factory->release();
}

void EventListener::onMarketDataSnaphsotRecieved(IO2GResponse *response)
{
    IO2GResponseReaderFactory *factory = mSession->getResponseReaderFactory();
    IO2GMarketDataSnapshotResponseReader *marketSnapshotReader = factory->createMarketDataSnapshotReader(response);
    char szBuffer[1024];
    char szTime[256];
    char szDate[256];
    IO2GTimeConverter *converter = mSession->getTimeConverter();
    for (int i = 0; i < marketSnapshotReader->size(); i++)
    {
        DATE date = marketSnapshotReader->getDate(i);
        date = converter->convert(date, IO2GTimeConverter::UTC, IO2GTimeConverter::Local);
        SYSTEMTIME st = {0};
        VariantTimeToSystemTime(date, &st);
        GetTimeFormat(LOCALE_USER_DEFAULT, 0, &st, NULL, szTime, 256);
        GetDateFormat(LOCALE_USER_DEFAULT, 0, &st, NULL, szDate, 256);
        sprintf_s(szBuffer, sizeof(szBuffer), "Date=%s %s,BidOpen=%f,BidHigh=%f,BidLow=%f,BidClose=%fAskOpen=%f,AskHigh=%f,AskLow=%f,AskClose=%f", szDate, szTime,
                                    marketSnapshotReader->getBidOpen(i),
                                    marketSnapshotReader->getBidHigh(i),
                                    marketSnapshotReader->getBidLow(i),
                                    marketSnapshotReader->getBidClose(i),
                                    marketSnapshotReader->getAskOpen(i),
                                    marketSnapshotReader->getAskHigh(i),
                                    marketSnapshotReader->getAskLow(i),
                                    marketSnapshotReader->getAskClose(i));
        mLog->logString(szBuffer);
    }
    // Last minute volume
    DATE date = marketSnapshotReader->getLastBarTime();
    date = converter->convert(date, IO2GTimeConverter::UTC, IO2GTimeConverter::Local);
    SYSTEMTIME st = {0};
    VariantTimeToSystemTime(date, &st);
    GetTimeFormat(LOCALE_USER_DEFAULT, 0, &st, NULL, szTime, 256);
    sprintf_s(szBuffer, sizeof(szBuffer), "LastMinute=%s Volume=%d", szTime, marketSnapshotReader->getLastBarVolume());
    mLog->logString(szBuffer);
    marketSnapshotReader->release();
    factory->release();
}


void EventListener::onOffersRecieved(IO2GResponse *response)
{
    IO2GResponseReaderFactory *factory = mSession->getResponseReaderFactory();
    if (factory == NULL)
        return;
    IO2GOffersTableResponseReader *offersReader = factory->createOffersTableReader(response);
    if (offersReader == NULL)
    {
        factory->release();
        return;
    }


    if (response->getType() == TablesUpdates)
        onUpdateOffers(offersReader);
    else
        onOffersTableResponse(offersReader);

    offersReader->release();
    factory->release();
}

void EventListener::onUpdateOffers(IO2GOffersTableResponseReader *offersReader)
{
    char szBuffer[1024];
    int count = offersReader->size();

    sprintf_s(szBuffer, sizeof(szBuffer), "Offer updates count %d", count);
    mLog->logString(szBuffer);

    for (int i = 0; i < count; i++)
    {
        IO2GOfferRow *offer = offersReader->getRow(i);
        sprintf_s(szBuffer, sizeof(szBuffer), "%s OfferID=%s Bid=%f Ask=%f Volume=%i", offer->getInstrument(), offer->getOfferID(), offer->getBid(), offer->getAsk(), offer->getVolume());
        mLog->logString(szBuffer);
        offer->release();
    }

}
void EventListener::onOffersTableResponse(IO2GOffersTableResponseReader *offersReader)
{
    useGenericTableInterface(offersReader);
    useOffersTableIntefaces(offersReader);
}

void EventListener::onAccountsReceived(IO2GResponse *response)
{
    IO2GResponseReaderFactory *factory = mSession->getResponseReaderFactory();
    if (factory == NULL)
        return;
    IO2GAccountsTableResponseReader *accountsReader = factory->createAccountsTableReader(response);
    if (accountsReader == NULL)
    {
        factory->release();
        return;
    }

    useGenericTableInterface(accountsReader);
    useAccountsTableIntefaces(accountsReader);
    // Gets orders from server
    getOrders(accountsReader);
    accountsReader->release();
    // Get trades from server
    getTrades();
    // Get closed trades from server
    getClosedTrades();
    factory->release();
}

void EventListener::useAccountsTableIntefaces(IO2GAccountsTableResponseReader *accountsReader)
{
    mLog->logString("Accounts Table");
    for (int i = 0; i < accountsReader->size(); i++)
    {
        IO2GAccountRow *pAccount = accountsReader->getRow(i);
        printToLogAccount(pAccount);
        pAccount->release();
    }
}

void EventListener::useGenericTableInterface(IO2GGenericTableResponseReader *genericReader)
{
    char szBuffer[3072];
    mLog->logString("Generic table");
    IO2GTableColumnCollection *columns = genericReader->columns();
    // Print column
    int count = columns->size();
    for (int i = 0; i < count; i++)
    {
        IO2GTableColumn *column = columns->get(i);
        sprintf_s(szBuffer, sizeof(szBuffer), "%d %s %i", i, column->getID(), column->getType());
        column->release();
        mLog->logString(szBuffer);
    }
    for (int i = 0; i < genericReader->size(); i++)
    {
        std::ostringstream stream;
        int count = columns->size();
        for (int j = 0; j < count; j++)
        {
            const void* value = genericReader->getCell(i, j);
            LPCSTR szIsValid = boolToString(genericReader->isCellValid(i, j)).c_str();
            IO2GTableColumn *column = columns->get(j);
            switch (column->getType())
            {
            case IO2GTableColumn::Integer:
            {
                int iValue = *(const int*)(value);
                stream << column->getID() << "=" << iValue << " Valid=" << szIsValid << " ";
            }
            break;
            case IO2GTableColumn::Double:
            {
                double dblValue = *(const double*)(value);
                stream << column->getID() << "=" << dblValue << " Valid=" << szIsValid << " ";
            }
            break;
            case IO2GTableColumn::Boolean:
            {
                bool bValue = *(const bool*)(value);
                stream << column->getID() << "=" << (boolToString(bValue).c_str()) << " Valid=" << szIsValid << " ";
            }
            break;
            case IO2GTableColumn::Date:
            {
                DATE date = *(DATE*)(value);
                std::string sDate = dateToString(date);
                stream << column->getID() << "=" << sDate.c_str() << " Valid=" << szIsValid << " ";
            }
            break;
            case IO2GTableColumn::String:
            {
                const char* szValue = (const char*)value;
                stream << column->getID() << "=" << szValue << " Valid=" << szIsValid << " ";
            }
            break;
            }
            column->release();
        }

        stream << std::ends;
        mLog->logString(stream.str().c_str());
    }
    columns->release();

}

void EventListener::useOffersTableIntefaces(IO2GOffersTableResponseReader *offersReader)
{
    mLog->logString("Offers Table");
    for (int i = 0; i < offersReader->size(); i++)
    {
        IO2GOfferRow *offer = offersReader->getRow(i);
        printToLogOffer(offer);
        offer->release();
    }
}

void EventListener::sendGetOffers()
{
   IO2GRequestFactory *factory = mSession->getRequestFactory();
   IO2GRequest *refreshOffers = factory->createRefreshTableRequest(Offers);
   mSession->sendRequest(refreshOffers);
   refreshOffers->release();
   factory->release();
}

void EventListener::getOrders(IO2GAccountsTableResponseReader *accountsReader)
{
    int size = accountsReader->size();
    IO2GRequestFactory *factory = mSession->getRequestFactory();
    for (int i = 0; i < size; i++)
    {
        IO2GAccountRow *account = accountsReader->getRow(i);
        LPCSTR accountID = account->getAccountID();
        IO2GRequest *request = factory->createRefreshTableRequestByAccount(Orders, accountID);
        mSession->sendRequest(request);
        account->release();
        request->release();
    }
    factory->release();
}

void EventListener::getTrades()
{
    IO2GRequestFactory *factory = mSession->getRequestFactory();
    IO2GRequest *request = factory->createRefreshTableRequest(Trades);
    mSession->sendRequest(request);
    request->release();
    factory->release();
}

void EventListener::getClosedTrades()
{
    IO2GRequestFactory *factory = mSession->getRequestFactory();
    IO2GRequest *request = factory->createRefreshTableRequest(ClosedTrades);
    mSession->sendRequest(request);
    request->release();
    factory->release();
}

void EventListener::onOrderReceived(IO2GResponse *response)
{
    IO2GResponseReaderFactory *factory = mSession->getResponseReaderFactory();
    if (factory == NULL)
        return;
    IO2GOrdersTableResponseReader *ordersReader = factory->createOrdersTableReader(response);
    if (ordersReader == NULL)
    {
        factory->release();
        return;
    }

    useGenericTableInterface(ordersReader);
    useOrdersTableIntefaces(ordersReader);
    ordersReader->release();
    factory->release();
}

void EventListener::onTradeReceived(IO2GResponse *response)
{
    O2G2Ptr<IO2GResponseReaderFactory> factory = mSession->getResponseReaderFactory();
    if (!factory)
        return;
    O2G2Ptr<IO2GTradesTableResponseReader> tradesReader = factory->createTradesTableReader(response);
    if (!tradesReader)
        return;


    useGenericTableInterface(tradesReader);
    useTradesTableInterfaces(tradesReader);
}

void EventListener::onClosedTradeReceived(IO2GResponse *response)
{
    O2G2Ptr<IO2GResponseReaderFactory> factory = mSession->getResponseReaderFactory();
    if (!factory)
        return;
    O2G2Ptr<IO2GClosedTradesTableResponseReader> closedTradesReader = factory->createClosedTradesTableReader(response);
    if (!closedTradesReader)
        return;

    useGenericTableInterface(closedTradesReader);
    useClosedTradesTableInterfaces(closedTradesReader);
}

void EventListener::useOrdersTableIntefaces(IO2GOrdersTableResponseReader *ordersReader)
{
    for (int i = 0; i < ordersReader->size(); i++)
    {
        IO2GOrderRow *order = ordersReader->getRow(i);
        printToLogOrder(order);
        order->release();
    }
}

void EventListener::useTradesTableInterfaces(IO2GTradesTableResponseReader *tradesReader)
{
    for (int i = 0; i < tradesReader->size(); i++)
    {
        IO2GTradeRow *trade = tradesReader->getRow(i);
        printToLogTrade(trade);
        trade->release();
    }
}

void EventListener::useClosedTradesTableInterfaces(IO2GClosedTradesTableResponseReader *closedTradesReader)
{
    for (int i = 0; i < closedTradesReader->size(); i++)
    {
        IO2GClosedTradeRow *closedTrade = closedTradesReader->getRow(i);
        printToLogClosedTrade(closedTrade);
        closedTrade->release();
    }
}

void EventListener::onTableUpdateReceive(IO2GResponse *response)
{
    IO2GResponseReaderFactory *factory = mSession->getResponseReaderFactory();
    if (factory == NULL)
        return;
    IO2GTablesUpdatesReader *updatesReader = factory->createTablesUpdatesReader(response);
    if (updatesReader == NULL)
    {
        factory->release();
        return;
    }
    char szBuffer[512];
    for (int i = 0; i < updatesReader->size(); i++)
    {
        O2GTable o2gTable = updatesReader->getUpdateTable(i);
        if (o2gTable == TableUnknown)
            continue;
        O2GTableUpdateType o2gUpdateType = updatesReader->getUpdateType(i);
        LPCSTR szUpdateType = "Unkown";
        switch(o2gUpdateType)
        {
        case Insert:
            szUpdateType = "Insert";
        break;
        case Delete:
            szUpdateType = "Delete";
        break;
        case Update:
            szUpdateType = "Update";
        break;
        default:
        break;
        }

        switch (o2gTable)
        {
        case Offers:
        {
            sprintf_s(szBuffer, sizeof(szBuffer), "Action=%s Table=%s", szUpdateType, "Offer");
            mLog->logString(szBuffer);

            IO2GOfferRow *offer = updatesReader->getOfferRow(i);
            printToLogOffer(offer);
            offer->release();
        break;
        }
        case Accounts:
        {
            sprintf_s(szBuffer, sizeof(szBuffer), "Action=%s Table=%s", szUpdateType, "Account");
            mLog->logString(szBuffer);
            IO2GAccountRow *account = updatesReader->getAccountRow(i);
            printToLogAccount(account);
            account->release();
        }
        break;
        case Orders:
        {
            sprintf_s(szBuffer, sizeof(szBuffer), "Action=%s Table=%s", szUpdateType, "Order");
            mLog->logString(szBuffer);
            IO2GOrderRow *order = updatesReader->getOrderRow(i);
            printToLogOrder(order);
            order->release();
        }
        break;
        case Trades:
        {
            sprintf_s(szBuffer, sizeof(szBuffer), "Action=%s Table=%s", szUpdateType, "Trade");
            mLog->logString(szBuffer);
            IO2GTradeRow *trade = updatesReader->getTradeRow(i);
            printToLogTrade(trade);
            if (o2gUpdateType == Insert)
                mOrderSample->CreateOrderForTrade(trade);
            trade->release();
        }
        break;
        case ClosedTrades:
        {
            sprintf_s(szBuffer, sizeof(szBuffer), "Action=%s Table=%s", szUpdateType, "Closed Trade");
            mLog->logString(szBuffer);
            IO2GClosedTradeRow *closedTrade = updatesReader->getClosedTradeRow(i);
            printToLogClosedTrade(closedTrade);
            closedTrade->release();
        }
        break;
        case Messages:
        {
            sprintf_s(szBuffer, sizeof(szBuffer), "Action=%s Table=%s", szUpdateType, "Message");
            mLog->logString(szBuffer);
            IO2GMessageRow *message = updatesReader->getMessageRow(i);
            printToLogMessage(message);
            message->release();
        }
        break;
        }
    }
    factory->release();
    updatesReader->release();
}

void EventListener::printToLogOffer(IO2GOfferRow *offer)
{
    char szBuffer[1024];
    std::string time = dateToString(offer->getTime());
    sprintf_s(szBuffer, sizeof(szBuffer), "%s OfferID=%s Bid=%f Ask=%f Low=%f High=%f Volume=%i QuoteID=%s Time=%s BidTradable=%s AskTradable=%s ContractCurreny=%s Digits=%i PointSize=%f SubscriptionStatus=%s TradingStatus=%s InstrumentType=%i ContractMultiplier=%f ValueDate=%s",
                                            offer->getInstrument(),
                                            offer->getOfferID(),
                                            offer->getBid(),
                                            offer->getAsk(),
                                            offer->getLow(),
                                            offer->getHigh(),
                                            offer->getVolume(),
                                            offer->getQuoteID(),
                                            time.c_str(),
                                            offer->getBidTradable(),
                                            offer->getAskTradable(),
                                            offer->getContractCurrency(),
                                            offer->getDigits(),
                                            offer->getPointSize(),
                                            offer->getSubscriptionStatus(),
                                            offer->getTradingStatus(),
                                            offer->getInstrumentType(),
                                            offer->getContractMultiplier(),
                                            offer->getValueDate());

    mLog->logString(szBuffer);
}

void EventListener::printToLogAccount(IO2GAccountRow *account)
{
    char szBuffer[1024];
    std::string lastMarginCallDate = dateToString(account->getLastMarginCallDate());
    sprintf_s(szBuffer, sizeof(szBuffer), "AccountID=%s AccountName=%s AccountKind=%s AccountLimit=%d Balance=%f NonTradableEquity=%f M2MEquity=%f UsedMargin=%f UsedMargin3=%f MarginCallFlag=%s LastMarginCallDate=%s MainteanceType=%s AmountLimit=%d MaintenanceFlag=%s BaseUnitSize=%d LevarageProfileID=%s ManagerAccountID=%s",
                account->getAccountID(),
                account->getAccountName(),
                account->getAccountKind(),
                account->getAmountLimit(),
                account->getBalance(),
                account->getNonTradeEquity(),
                account->getM2MEquity(),
                account->getUsedMargin(),
                account->getUsedMargin3(),
                account->getMarginCallFlag(),
                lastMarginCallDate.c_str(),
                account->getMaintenanceType(),
                account->getAmountLimit(),
                boolToString(account->getMaintenanceFlag()).c_str(),
                account->getBaseUnitSize(),
                account->getLevarageProfileID(),
                account->getManagerAccountID());
    mLog->logString(szBuffer);
}

void EventListener::printToLogOrder(IO2GOrderRow *order)
{
    char szBuffer[1024];
    std::string expireDate = dateToString(order->getExpireDate());
    std::string statusTime = dateToString(order->getStatusTime());

    sprintf_s(szBuffer, sizeof(szBuffer), "OrderID=%s OfferID=%s TradeID=%s AccountID=%s AccountName=%s AccountKind=%s Amount=%d BuySell=%s Rate=%f RateMin=%f RateMax=%f ExecutionRate=%f TimeInForce=%s LifeTime=%f Status=%s StatusTime=%s Type=%s NetQuantity=%s TrailStep=%d TrailRate=%f PegType=%s PegOffset=%f ContingentOrderID=%s WorkingIndicator=%s ContingencyType=%d PrimaryID=%s OriginAmount=%d FilledAmount=%d RequestID=%s RequestTXT=%s ExpireDate=%s ValueDate=%s Parties=%s",
                order->getOrderID(),
                order->getOfferID(),
                order->getTradeID(),
                order->getAccountID(),
                order->getAccountName(),
                order->getAccountKind(),
                order->getAmount(),
                order->getBuySell(),
                order->getRate(),
                order->getRateMin(),
                order->getRateMax(),
                order->getExecutionRate(),
                order->getTimeInForce(),
                order->getLifetime(),
                order->getStatus(),
                statusTime.c_str(),
                order->getType(),
                boolToString(order->getNetQuantity()).c_str(),
                order->getTrailStep(),
                order->getTrailRate(),
                order->getPegType(),
                order->getPegOffset(),
                order->getContingentOrderID(),
                boolToString(order->getWorkingIndicator()).c_str(),
                order->getContingencyType(),
                order->getPrimaryID(),
                order->getOriginAmount(),
                order->getFilledAmount(),
                order->getRequestID(),
                order->getRequestTXT(),
                order->getParties(),
                expireDate.c_str(),
                order->getValueDate());
    mLog->logString(szBuffer);
}

// Implementation from gstool::date
bool EventListener::VariantTimeToSystemTime(double dt, SYSTEMTIME *st)
{
    split(dt, st->wYear, st->wMonth, st->wDay, st->wHour, st->wMinute, st->wSecond, st->wMilliseconds);
    return true;
}

// Implementation from gstool::date
void EventListener::split(double dtSrc, unsigned short& wYear, unsigned short& wMonth, unsigned short& wDay,
                unsigned short& wHour, unsigned short& wMinute, unsigned short& wSecond,
                unsigned short& wMSecond)
{
    const long minDate = -657434L;                      //!< minimal date value
    const long maxDate = 2958465L;                      //!< maximal date value
    const long monthDays[13] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};

    wMonth = wYear = wDay = wHour = wMinute = wSecond = wMSecond = 0;
    dtSrc += 1e-10;
    //The legal range does not actually span year 0 to 9999.
    if (dtSrc > maxDate || dtSrc < minDate) // about year 100 to about 9999
                return ;

    long nDays;             // Number of days since Dec. 30, 1899
    long nDaysAbsolute;     // Number of days since 1/1/0
//    long nSecsInDay;        // Time in seconds since midnight
    long nMinutesInDay;     // Minutes in day

    long n400Years;         // Number of 400 year increments since 1/1/0
    long n400Century;       // Century within 400 year block (0,1,2 or 3)
    long n4Years;           // Number of 4 year increments since 1/1/0
    long n4Day;             // Day within 4 year block
                                                    //  (0 is 1/1/yr1, 1460 is 12/31/yr4)
    long n4Yr;              // Year within 4 year block (0,1,2 or 3)
    bool bLeap4 = true;     // TRUE if 4 year block includes leap year

    double dblDate = dtSrc; // tempory serial date

    // If a valid date, then this conversion should not overflow
    nDays = (long)dblDate;

    // Round to the second
//    dblDate += ((dtSrc > 0.0) ? halfSecond : -halfSecond);

    nDaysAbsolute = (long)dblDate + 693959L; // Add days from 1/1/0 to 12/30/1899

    dblDate = fabs(dblDate);
    long nMSecsInDay = (long)((dblDate - floor(dblDate)) * 86400000) + 1;

    //Calculate the day of week (sun=1, mon=2...)
    //  -1 because 1/1/0 is Sat.  +1 because we want 1-based

    // Leap years every 4 yrs except centuries not multiples of 400.
    n400Years = (long)(nDaysAbsolute / 146097L);

    // Set nDaysAbsolute to day within 400-year block
    nDaysAbsolute %= 146097L;

    // -1 because first century has extra day
    n400Century = (long)((nDaysAbsolute - 1) / 36524L);

    // Non-leap century
    if (n400Century != 0)
    {
            // Set nDaysAbsolute to day within century
            nDaysAbsolute = (nDaysAbsolute - 1) % 36524L;

            // +1 because 1st 4 year increment has 1460 days
            n4Years = (long)((nDaysAbsolute + 1) / 1461L);

            if (n4Years != 0)
                    n4Day = (long)((nDaysAbsolute + 1) % 1461L);
            else
            {
                    bLeap4 = false;
                    n4Day = (long)nDaysAbsolute;
            }
    }
    else
    {
            // Leap century - not special case!
            n4Years = (long)(nDaysAbsolute / 1461L);
            n4Day = (long)(nDaysAbsolute % 1461L);
    }

    if (bLeap4)
    {
            // -1 because first year has 366 days
            n4Yr = (n4Day - 1) / 365;

            if (n4Yr != 0)
                    n4Day = (n4Day - 1) % 365;
    }
    else
    {
            n4Yr = n4Day / 365;
            n4Day %= 365;
    }

    // n4Day is now 0-based day of year. Save 1-based day of year, year number
    wYear = (unsigned short)(n400Years * 400 + n400Century * 100 + n4Years * 4 + n4Yr);

    // Handle leap year: before, on, and after Feb. 29.
    if (n4Yr == 0 && bLeap4)
    {
            // Leap Year
            if (n4Day == 59)
            {
                    /* Feb. 29 */
                    wMonth = 2;
                    wDay = 29;
                    goto DoTime;
            }

            // Pretend it's not a leap year for month/day comp.
            if (n4Day >= 60)
                    --n4Day;
    }

    // Make n4DaY a 1-based day of non-leap year and compute
    //  month/day for everything but Feb. 29.
    ++n4Day;

    // Month number always >= n/32, so save some loop time */
    for (wMonth = (unsigned short)(n4Day >> 5) + 1; n4Day > monthDays[wMonth]; wMonth++);

    wDay = (int)(n4Day - monthDays[wMonth - 1]);

DoTime:
    if (nMSecsInDay == 0)
        wHour = wMinute = wSecond = wMSecond = 0;
    else
    {

        wMSecond = (unsigned short)(nMSecsInDay % 1000L);
        if (wMSecond > 0)
            wMSecond--;
        long nSecsInDay = (long)nMSecsInDay / 1000;
        wSecond = (unsigned short)(nSecsInDay % 60L);
        nMinutesInDay = nSecsInDay / 60L;
        wMinute = (int)nMinutesInDay % 60;
        wHour = (int)nMinutesInDay / 60;
    }

    return ;
}


void EventListener::printToLogTrade(IO2GTradeRow *trade)
{
    char szBuffer[1024];
    std::string time = dateToString(trade->getOpenTime());
    sprintf_s(szBuffer, sizeof(szBuffer), "TradeID =%s OfferID=%s AccountID=%s AccountName=%s AccountKind=%s BuySell=%s Commission=%f RollInt=%f Amount=%d Open=%f OpenOrderID=%s OpenOrderReqID=%s RequestTXT=%s QuoteID=%s Time=%s UsedMargin=%d TradeIDOrigin=%s Parties=%s",
                trade->getTradeID(),
                trade->getOfferID(),
                trade->getAccountID(),
                trade->getAccountName(),
                trade->getAccountKind(),
                trade->getBuySell(),
                trade->getCommission(),
                trade->getRolloverInterest(),
                trade->getAmount(),
                trade->getOpenRate(),
                trade->getOpenOrderID(),
                trade->getOpenOrderReqID(),
                trade->getOpenOrderRequestTXT(),
                trade->getOpenQuoteID(),
                time.c_str(),
                trade->getUsedMargin(),
                trade->getTradeIDOrigin(),
                trade->getParties());
    mLog->logString(szBuffer);
}

void EventListener::printToLogClosedTrade(IO2GClosedTradeRow *closedTrade)
{
    char szBuffer[1024];
    std::string closeTimeString = dateToString(closedTrade->getCloseTime());
    sprintf_s(szBuffer, sizeof(szBuffer),
"TradeID=%s OfferID=%s AccountID=%s AccountName=%s AccountKind=%s \
BuySell=%s CloseRate=%f CloseOrderID=%s CloseOrderReqID=%s CloseOrderRequestTXT=%s CloseQuoteID=%s CloseOrderParties=%s Time=%s Commission=%f \
GrossPL=%f RolloverInterest=%f Amount=%d OpenRate=%f OpenOrderID=%s \
OpenOrderReqID=%s OpenOrderRequestTXT=%s OpenQuoteID=%s OpenOrderParties=%s TradeIDOrigin=%s TradeIDRemain=%s ValueDate=%s",
                closedTrade->getTradeID(),
                closedTrade->getOfferID(),
                closedTrade->getAccountID(),
                closedTrade->getAccountName(),
                closedTrade->getAccountKind(),
                closedTrade->getBuySell(),
                closedTrade->getCloseRate(),
                closedTrade->getCloseOrderID(),
                closedTrade->getCloseOrderReqID(),
                closedTrade->getCloseOrderRequestTXT(),
                closedTrade->getCloseQuoteID(),
                closedTrade->getCloseOrderParties(),
                closeTimeString.c_str(),
                closedTrade->getCommission(),

                closedTrade->getGrossPL(),
                closedTrade->getRolloverInterest(),
                closedTrade->getAmount(),
                closedTrade->getOpenRate(),
                closedTrade->getOpenOrderID(),
                closedTrade->getOpenOrderReqID(),
                closedTrade->getOpenOrderRequestTXT(),
                closedTrade->getOpenQuoteID(),
                closedTrade->getOpenOrderParties(),
                closedTrade->getTradeIDOrigin(),
                closedTrade->getTradeIDRemain(),
                closedTrade->getValueDate());
    mLog->logString(szBuffer);
}

void EventListener::printToLogMessage(IO2GMessageRow *message)
{
    char szBuffer[1024];
    std::string timeString = dateToString(message->getTime());
    sprintf_s(szBuffer, sizeof(szBuffer), "MsgID=%s Feature=%s From=%s Subject=%s Type=%s Time=%s Text=%s",
                message->getMsgID(),
                message->getFeature(),
                message->getFrom(),
                message->getSubject(),
                message->getType(),
                timeString.c_str(),
                message->getText());
    mLog->logString(szBuffer);
}

void EventListener::onSystemPropertiesReceived(IO2GResponse *response)
{
    IO2GResponseReaderFactory *factory = mSession->getResponseReaderFactory();
    if (factory == NULL)
        return;
    IO2GSystemPropertiesReader *systemResponseReader = factory->createSystemPropertiesReader(response);
    if (systemResponseReader == NULL)
    {
        factory->release();
        return;
    }
    char buffer[512];
    sprintf_s(buffer, sizeof(buffer), "System Properties:");
    mLog->logString(buffer);
    for (int i = 0 ; i < systemResponseReader->size(); i++)
    {
        const char *value;
        const char *sysProperty = systemResponseReader->getProperty(i, value);
        sprintf_s(buffer, sizeof(buffer), "%s %s", sysProperty, value);
        mLog->logString(buffer);
    }
    systemResponseReader->release();
    factory->release();
}



void EventListener::onOrderResponse(IO2GResponse *response)
{
    mOrderSample->onOrderResponse(response);
}

/** Disable price updates*/
void EventListener::disablePriceUpdates()
{
    mSession->setPriceUpdateMode(NoPrice);
}

