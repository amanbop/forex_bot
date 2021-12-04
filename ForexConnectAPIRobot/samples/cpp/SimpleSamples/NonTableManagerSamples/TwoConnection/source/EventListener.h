#pragma once
class CreateOrderSample;
class PermissionCheckerSample;
class TradingSettingsProviderSample;
class CSessionStatusListener;
class ResponseListener;
class CLoginDataProvider;

class EventListener : public threading::AThread
{
 public:
    /** */
    EventListener(ResponseQueue *responses,
                  SimpleLog *log,
                  IO2GSession *session,
                  HANDLE status,
                  HANDLE response,
                  HANDLE connected,
                  HANDLE stop);
    EventListener(std::string sFile, HANDLE stopEvent);
    ~EventListener();
    /** Event listener*/
    void waitNextEvent();

    void login(const char *user, const char *pwd, const char *url, const char *connection);

    /** Disable price updates*/
    void disablePriceUpdates();

    // Implementation from gstool::date
    static bool VariantTimeToSystemTime(double dt, SYSTEMTIME *st);

private:

    virtual int run(void *);

    void onResponse();
    void onConnected();
    void onStatusChanged();
    void logTimeFrames();
    void sendMarketDataSnapshot();
    void sendGetOffers();
    void getTrades();
    void getClosedTrades();
    void getOrders(IO2GAccountsTableResponseReader *accountsReader);
    void onSystemPropertiesReceived(IO2GResponse *response);
    void onMarketDataSnaphsotRecieved(IO2GResponse *response);
    void onOffersRecieved(IO2GResponse *response);
    void onAccountsReceived(IO2GResponse *response);
    void onOrderReceived(IO2GResponse *response);
    void onTradeReceived(IO2GResponse *response);
    void onClosedTradeReceived(IO2GResponse *response);
    void onUpdateOffers(IO2GOffersTableResponseReader *offersReader);
    void onOffersTableResponse(IO2GOffersTableResponseReader *offersReader);
    void useGenericTableInterface(IO2GGenericTableResponseReader *offersReader);
    void useOffersTableIntefaces(IO2GOffersTableResponseReader *offersReader);
    void useAccountsTableIntefaces(IO2GAccountsTableResponseReader *accountsReader);
    void useOrdersTableIntefaces(IO2GOrdersTableResponseReader *ordersReader);
    void useTradesTableInterfaces(IO2GTradesTableResponseReader *tradesReader);
    void useClosedTradesTableInterfaces(IO2GClosedTradesTableResponseReader *closedTradesReader);
    void onTableUpdateReceive(IO2GResponse *response);
    void dumpLog();

    void onOrderResponse(IO2GResponse *response);

    void printToLogOffer(IO2GOfferRow *offer);
    void printToLogAccount(IO2GAccountRow *account);
    void printToLogOrder(IO2GOrderRow *order);
    void printToLogTrade(IO2GTradeRow *trade);
    void printToLogClosedTrade(IO2GClosedTradeRow *closedTrade);
    void printToLogMessage(IO2GMessageRow *message);
    // Implementation from gstool::date
    static void split(double dtSrc, unsigned short& wYear, unsigned short& wMonth, unsigned short& wDay,
                unsigned short& wHour, unsigned short& wMinute, unsigned short& wSecond,
                unsigned short& wMSecond);


    HANDLE mStatusHandle;
    HANDLE mResponseHandle;
    HANDLE mConnectedHandle;
    HANDLE mStopHandle;
    HANDLE mStopAllHandle;
    SimpleLog *mLog;
    ResponseQueue *mResponses;
    IO2GSession *mSession;
    CreateOrderSample *mOrderSample;
    PermissionCheckerSample* mPermissionSample;
    TradingSettingsProviderSample* mTradingSettings;
    CSessionStatusListener *mSessionStatusListener;
    ResponseListener *mResponseListener;
    CLoginDataProvider *mLoginData;
};

