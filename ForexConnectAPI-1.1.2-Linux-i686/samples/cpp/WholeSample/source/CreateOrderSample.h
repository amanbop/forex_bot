#pragma once
class SimpleLog;
class CreateOrderSample
{
    struct OrderCreationParam
    {
        std::string mOfferID;
        std::string mAccountID;
        int mBaseAmount;
        double mAsk;
        double mBid;
        double mPointSize;
    };
    struct sOrderParams
    {
        std::string offerID;
        int amount;
        double rate;
        std::string buySell;
        std::string orderType;
    };
    int orderForTradeNum;
 public:
    /** Constructor.*/
    CreateOrderSample(SimpleLog *log, IO2GSession *session);
    /** Destructor.*/
    ~CreateOrderSample();
    /** When the trade is created, either create one of the close orders (Market Close, True Market Close, Range Close) or add Stop/Limit to the trade */
    void CreateOrderForTrade(IO2GTradeRow *trade);
    /** Prepare parameters for creation order from login rules.*/
    void prepareParamsFromLoginRules(IO2GLoginRules *loginRules);
    /** Prepare parameters and call True Market Close order */
    void prepareParamsAndCallTrueMarketCloseOrder(IO2GTradeRow *trade);
    /** Prepare parameters and call Market Close order */
    void prepareParamsAndCallMarketCloseOrder(IO2GTradeRow *trade);
    /** Prepare parameters and call Range Close order */
    void prepareParamsAndCallRangeCloseOrder(IO2GTradeRow * trade, int atMarket);
    /** Prepare parameters and call Stop order */
    void prepareParamsAndCallStopOrder(IO2GTradeRow * trade, int atMarket);
    /** Prepare parameters and call Limit order */
    void prepareParamsAndCallLimitOrder(IO2GTradeRow * trade, int atMarket);
    /** Prepare parameters and call Stop Entry order.*/
    void prepareParamsAndCallEntryStopOrder(const char *offerID, const char *accountID, int amount, double pointSize, int numPoints, const char * buySell);
    /** Prepare parameters and call Limit Entry order.*/
    void prepareParamsAndCallEntryLimitOrder(const char *offerID, const char *accountID, int amount, double pointSize, int numPoints, const char * buySell);
    /** Prepare parameters and call RangeOrder */
    void prepareParamsAndCallRangeOrder(const char *offerID, const char *accountID, int amount, double pointSize, int atMarket, const char * buySell);
    /** Preapate entry order and call entry limit order with stop/limit.*/
    void prepareParamsAndCallEntryLimitOrderWithStopLimit(const char *offerID, const char *accountID, int amount, double pointSize, int numPoints, const char * buySell);
    /** Prepare parameters and call OCO orders */
    void prepareParamsAndCallOCOOrders(const char *offerID, const char *accountID, int amount, double pointSize, const char * buySell);
    /** Prepare parameters and call OTO orders */
    void prepareAndCreateOTO(const char *accountID, const char *offerID, int baseAmount, double pointSize);
    void prepareToManageGroup(const char* accountID, const char* offerID,int baseAmount, double pointSize);

    IO2GValueMap* createOrder(const char* accountID, const char* offerID, double rate, int amount, const char* buySell, const char* orderType);
    IO2GValueMap* createOrder(const char* accountID, sOrderParams& params);

    void createOTO(IO2GValueMap* primary, IO2GValueMap* secondary1, IO2GValueMap* secondary2);


    IO2GValueMap* existOrder(const char* orderID, const char* accountID);
    void joinToNewContingencyGroup(const char* accountID, int contingecnyType, const char* primaryID, const char* secID1, const char* secID2);
    void joinToExistingContingencyGroup(const char* accountID, int contingecnyType, const char* primaryID, const char* secID1, const char* secID2);
    void removeFromGroup(const char* accountID, const char* secID1, const char* secID2);

    /** Create orders.*/
    void createOrders();
    /** Create OCO orders.*/
    void createOCO(const char *offerID, const char *accountID, int amount, double rate, const char * buySell, int iOrdersCount);
    /** Create true market order.*/
    void createTrueMarketOrder(const char *offerID, const char *accountID, int amount, const char * buySell);
    /** Create market order.*/
    void createMarketOrder(const char *offerID, const char *accountID, int amount, double rate, const char * buySell);
    /** Create range market order.*/
    void createRangeOrder(const char *offerID, const char *accountID, int amount, double rateMin, double rateMax, const char * buySell);
    /** Prepare parameters and call range market order.*/
    void createEntryStopOrder(const char *offerID, const char *accountID, int amount, double rate, const char * buySell);
    /** Create Limit entry order */
    void createEntryLimitOrder(const char *offerID, const char *accountID, int amount, double rate, const char * buySell);
    /** Create true market close order.*/
    void createTrueMarketCloseOrder(const char *offerID, const char *accountID, const char * tradeID, int amount, const char * buySell);
    /** Create market close order. */
    void createMarketCloseOrder(const char *offerID, const char *accountID, const char * tradeID, int amount, double rate, const char * buySell);
    /** Create range market close order. */
    void createRangeCloseOrder(const char *offerID, const char *accountID, const char * tradeID, int amount, double rateMin, double rateMax, const char * buySell);
    /** create Stop order. */
    void createStopOrder(const char *offerID, const char *accountID, const char * tradeID, int amount, double rate, const char * buySell);
    /** create Limit order. */
    void createLimitOrder(const char *offerID, const char *accountID, const char * tradeID, int amount, double rate, const char * buySell);
    /** Create enrly limit order with attached stop limit.*/
    void createEntryLimitOrderWithStopLimit(const char *offerID, const char *accountID, int amount, double rate, const char * buySell, double stopOffset, double limitOffset);
    /** Edit order.*/
    void editOrder(const char *orderID, const char* accountID, int amount, double rate);
    /** Delete order command.*/
    void deleteOrder(const char* orderID);
    /** */
    void onOrderResponse(IO2GResponse *response);
    /** */
    void onCommandResponse(IO2GResponse *response);
 private:
    typedef enum
    {
        deleteOrderAction,
        setStopLimit,
        editOrderAction,
        manageOTOAction,
    } actions;
    /** array of OrderID*/
    std::vector <std::string> mOrders;
     /** Order data.*/
    OrderCreationParam *mParams;
    SimpleLog *mLog;
    /** Map of request id with action. After order request has been completed
        the action is applied for order.*/
    std::map<std::string, actions> mActions;

    O2G2Ptr<IO2GSession> mSession;
};

