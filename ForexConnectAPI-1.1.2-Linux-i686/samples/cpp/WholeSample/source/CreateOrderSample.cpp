#include "stdafx.h"
#include "SimpleLog.h"
#include "CreateOrderSample.h"

/** Constructor.*/
CreateOrderSample::CreateOrderSample(SimpleLog *log, IO2GSession *session)
{
    mParams = NULL;
    mLog = log;
    mSession = session;
    mSession->addRef();
    orderForTradeNum = 0;
}
/** Destructor.*/
CreateOrderSample::~CreateOrderSample()
{
    if (mParams)
        delete mParams;
}
/** Create orders.*/
void CreateOrderSample::createOrders()
{
    int numberOfLots = 2;
    createTrueMarketOrder(mParams->mOfferID.c_str(),
                          mParams->mAccountID.c_str(),
                          mParams->mBaseAmount * numberOfLots, O2G2::Buy);

    prepareParamsAndCallEntryStopOrder(mParams->mOfferID.c_str(),
                                    mParams->mAccountID.c_str(),
                                    mParams->mBaseAmount * numberOfLots,
                                    mParams->mPointSize, 100, O2G2::Buy);

    prepareParamsAndCallEntryLimitOrder(mParams->mOfferID.c_str(),
                                     mParams->mAccountID.c_str(),
                                     mParams->mBaseAmount * numberOfLots,
                                     mParams->mPointSize, 100, O2G2::Sell);

    prepareParamsAndCallEntryLimitOrderWithStopLimit(mParams->mOfferID.c_str(),
                                        mParams->mAccountID.c_str(),
                                        mParams->mBaseAmount * numberOfLots,
                                        mParams->mPointSize, 500,
                                        O2G2::Buy);

    createMarketOrder(mParams->mOfferID.c_str(),
                      mParams->mAccountID.c_str(),
                      mParams->mBaseAmount * numberOfLots,
                      mParams->mAsk, O2G2::Buy);

    prepareParamsAndCallRangeOrder(mParams->mOfferID.c_str(),
                                mParams->mAccountID.c_str(),
                                mParams->mBaseAmount * numberOfLots,
                                mParams->mPointSize, 10, O2G2::Buy);

    prepareParamsAndCallOCOOrders(mParams->mOfferID.c_str(),
                               mParams->mAccountID.c_str(),
                               mParams->mBaseAmount * numberOfLots,
                               mParams->mPointSize, O2G2::Buy);

    prepareAndCreateOTO(mParams->mAccountID.c_str(), mParams->mOfferID.c_str(),
                        mParams->mBaseAmount, mParams->mPointSize);

    prepareToManageGroup(mParams->mAccountID.c_str(), mParams->mOfferID.c_str(),
                        mParams->mBaseAmount, mParams->mPointSize);
}

void CreateOrderSample::prepareAndCreateOTO(const char* accountID, const char* offerID,
                                            int baseAmount, double pointSize)
{
    double rate = mParams->mAsk;

    sOrderParams orders[3];
    orders[0].amount = orders[1].amount = orders[2].amount = 10 * baseAmount;
    orders[0].offerID = orders[1].offerID = orders[2].offerID = offerID;

    orders[0].buySell = O2G2::Buy;
    orders[0].orderType = O2G2::Orders::LimitEntry;
    orders[0].rate = rate + 50 * pointSize;
    orders[1].buySell = O2G2::Sell;
    orders[1].orderType = O2G2::Orders::StopEntry;
    orders[1].rate = orders[0].rate - 10 * pointSize;
    orders[2].buySell = O2G2::Buy;
    orders[2].orderType = O2G2::Orders::StopEntry;
    orders[2].rate = orders[0].rate + 20 * pointSize;

    IO2GValueMap* primary = createOrder(accountID, orders[0]);
    IO2GValueMap* secondary1 = createOrder(accountID, orders[1]);
    IO2GValueMap* secondary2 = createOrder(accountID, orders[2]);
    createOTO(primary, secondary1, secondary2);
}


// Helper method for creating valueMap for one order
IO2GValueMap* CreateOrderSample::createOrder(const char* accountID, const char* offerID, double rate, int amount, const char* buySell, const char* orderType)
{
    O2G2Ptr<IO2GRequestFactory> factory = mSession->getRequestFactory();

    IO2GValueMap* valuemap = factory->createValueMap();

    valuemap->setString(Command, O2G2::Commands::CreateOrder);    // for OTO orders this field is not mandatory
    valuemap->setString(OrderType, orderType);
    valuemap->setString(AccountID, accountID);
    valuemap->setString(OfferID, offerID);
    valuemap->setString(BuySell, buySell);
    valuemap->setDouble(Rate, rate);
    valuemap->setInt(Amount, amount);

    return valuemap;
}
// Helper method for creating valueMap for one order
IO2GValueMap* CreateOrderSample::createOrder(const char* accountID, sOrderParams& params)
{
    return createOrder(accountID, params.offerID.c_str(), params.rate, params.amount, params.buySell.c_str(), params.orderType.c_str());
}

void CreateOrderSample::createOTO(IO2GValueMap* primary, IO2GValueMap* secondary1, IO2GValueMap* secondary2)
{
    O2G2Ptr<IO2GRequestFactory> factory = mSession->getRequestFactory();

    // create OTO command
    O2G2Ptr<IO2GValueMap> valuemap = factory->createValueMap();
    valuemap->setString(Command, O2G2::Commands::CreateOTO);

    // create primary order
    valuemap->appendChild(primary);

    // create two secondary orders
    valuemap->appendChild(secondary1);
    valuemap->appendChild(secondary2);

    // create request from valueMap
    O2G2Ptr<IO2GRequest> request = factory->createOrderRequest(valuemap);
    for (int i = 0; i < request->getChildrenCount(); i++)
    {
        O2G2Ptr<IO2GRequest> childRequest = request->getChildRequest(i);
        mActions[childRequest->getRequestID()] = deleteOrderAction;
    }
    mSession->sendRequest(request);
}




void CreateOrderSample::prepareToManageGroup(const char* accountID, const char* offerID,
                                            int baseAmount, double pointSize)
{
    double rate = mParams->mAsk;

    sOrderParams orders[5];
    orders[0].amount = orders[1].amount = orders[2].amount = orders[3].amount = orders[4].amount = 10 * baseAmount;
    orders[0].offerID = orders[1].offerID = orders[2].offerID = orders[3].offerID = orders[4].offerID = offerID;

    orders[0].buySell = O2G2::Buy;
    orders[0].orderType = O2G2::Orders::LimitEntry;
    orders[0].rate = rate + 40 * pointSize;
    orders[1].buySell = O2G2::Sell;
    orders[1].orderType = O2G2::Orders::StopEntry;
    orders[1].rate = orders[0].rate - 100 * pointSize;
    orders[2].buySell = O2G2::Buy;
    orders[2].orderType = O2G2::Orders::LimitEntry;
    orders[2].rate = orders[0].rate + 20 * pointSize;
    orders[3].buySell = O2G2::Buy;
    orders[3].orderType = O2G2::Orders::LimitEntry;
    orders[3].rate = orders[0].rate + 30 * pointSize;
    orders[4].buySell = O2G2::Buy;
    orders[4].orderType = O2G2::Orders::LimitEntry;
    orders[4].rate = orders[0].rate + 25 * pointSize;

    O2G2Ptr<IO2GRequestFactory> factory = mSession->getRequestFactory();

    // create OTO command
    O2G2Ptr<IO2GValueMap> valuemap = factory->createValueMap();
    valuemap->setString(Command, O2G2::Commands::CreateOrder);
    valuemap->appendChild(createOrder(accountID, orders[0]));
    valuemap->appendChild(createOrder(accountID, orders[1]));
    valuemap->appendChild(createOrder(accountID, orders[2]));
    valuemap->appendChild(createOrder(accountID, orders[3]));
    valuemap->appendChild(createOrder(accountID, orders[4]));

    O2G2Ptr<IO2GRequest> request = factory->createOrderRequest(valuemap);
    for (int i = 0; i < request->getChildrenCount(); i++)
    {
        O2G2Ptr<IO2GRequest> childRequest = request->getChildRequest(i);
        mActions[childRequest->getRequestID()] = manageOTOAction;
    }
    mSession->sendRequest(request);
}

// Helper method for creating valueMap for existing order
IO2GValueMap* CreateOrderSample::existOrder(const char* orderID, const char* accountID)
{
    O2G2Ptr<IO2GRequestFactory> factory = mSession->getRequestFactory();

    IO2GValueMap* valuemap = factory->createValueMap();

    valuemap->setString(OrderID, orderID);
    valuemap->setString(AccountID, accountID);

    return valuemap;
}

void CreateOrderSample::joinToNewContingencyGroup(const char* accountID, int contingecnyType, const char* primaryID, const char* secID1, const char* secID2)
{
    O2G2Ptr<IO2GRequestFactory> factory = mSession->getRequestFactory();

    O2G2Ptr<IO2GValueMap> valuemap = factory->createValueMap();
    valuemap->setString(Command, O2G2::Commands::JoinToNewContingencyGroup);
    //set contingency type for created group
    valuemap->setInt(ContingencyGroupType, contingecnyType);

    // primary order
    valuemap->appendChild(existOrder(primaryID, accountID));
    // secondary order
    valuemap->appendChild(existOrder(secID1, accountID));
    // secondary order
    valuemap->appendChild(existOrder(secID2, accountID));

    // create request from valueMap
    O2G2Ptr<IO2GRequest> request = factory->createOrderRequest(valuemap);
    for (int i = 0; i < request->getChildrenCount(); i++)
    {
        O2G2Ptr<IO2GRequest> childRequest = request->getChildRequest(i);
        mActions[childRequest->getRequestID()] = manageOTOAction;
    }
    mSession->sendRequest(request);
}

void CreateOrderSample::joinToExistingContingencyGroup(const char* accountID, int contingecnyType, const char* groupID, const char* secID1, const char* secID2)
{
    O2G2Ptr<IO2GRequestFactory> factory = mSession->getRequestFactory();

    O2G2Ptr<IO2GValueMap> valuemap = factory->createValueMap();
    valuemap->setString(Command, O2G2::Commands::JoinToExistingContingencyGroup);
    valuemap->setString(ContingencyID, groupID);
    //set contingencyType of existing group
    valuemap->setInt(ContingencyGroupType, contingecnyType);

    // secondary order
    valuemap->appendChild(existOrder(secID1, accountID));
    // secondary order
    valuemap->appendChild(existOrder(secID2, accountID));

    // create request from valueMap
    O2G2Ptr<IO2GRequest> request = factory->createOrderRequest(valuemap);
    for (int i = 0; i < request->getChildrenCount(); i++)
    {
        O2G2Ptr<IO2GRequest> childRequest = request->getChildRequest(i);
        mActions[childRequest->getRequestID()] = manageOTOAction;
    }
    mSession->sendRequest(request);
}

void CreateOrderSample::removeFromGroup(const char* accountID, const char* secID1, const char* secID2)
{
    O2G2Ptr<IO2GRequestFactory> factory = mSession->getRequestFactory();

    // create JoinToExistingContingencyGroup command
    // this command cannot be created as tree
    O2G2Ptr<IO2GValueMap> valuemap = factory->createValueMap();
    valuemap->setString(Command, O2G2::Commands::RemoveFromContingencyGroup);

    // secondary order
    IO2GValueMap* valuemapRemove = existOrder(secID1, accountID);
    valuemapRemove->setString(Command, O2G2::Commands::RemoveFromContingencyGroup);
    valuemap->appendChild(valuemapRemove);

    // secondary order
    valuemapRemove = existOrder(secID2, accountID);
    valuemapRemove->setString(Command, O2G2::Commands::RemoveFromContingencyGroup);
    valuemap->appendChild(valuemapRemove);

    // create request from valueMap
    O2G2Ptr<IO2GRequest> request = factory->createOrderRequest(valuemap);
    for (int i = 0; i < request->getChildrenCount(); i++)
    {
        O2G2Ptr<IO2GRequest> childRequest = request->getChildRequest(i);
        mActions[childRequest->getRequestID()] = manageOTOAction;
    }
    mSession->sendRequest(request);
}




/** Create OCO orders. */
void CreateOrderSample::createOCO(const char *offerID, const char *accountID, int amount, double rate, const char * buySell, int iOrdersCount)
{
    O2G2Ptr<IO2GRequestFactory> factory = mSession->getRequestFactory();

    O2G2Ptr<IO2GValueMap> mainValueMap = factory->createValueMap();
    mainValueMap->setString(Command, O2G2::Commands::CreateOCO);
    for (int i = 0; i < iOrdersCount; i++)
    {
        O2G2Ptr<IO2GValueMap> valuemap = factory->createValueMap();
        valuemap->setString(Command, O2G2::Commands::CreateOrder);
        valuemap->setString(OrderType, O2G2::Orders::LimitEntry);
        valuemap->setString(AccountID, accountID);          // The identifier of the account the order should be placed for.
        valuemap->setString(OfferID, offerID);              // The identifier of the instrument the order should be placed for.
        valuemap->setString(BuySell, buySell);              // The order direction (O2G2::Buy for buy, O2G2::Sell for sell)
        valuemap->setDouble(Rate, rate);                    // The rate at which the order must be filled (below current rate for Buy, above current rate for Sell)
        valuemap->setInt(Amount, amount);                   // The quantity of the instrument to be bought or sold.

        char customID[32];
        sprintf_s(customID, 32, "OCO_LimitEntry_N%d", i + 1);

        valuemap->setString(CustomID, customID); // The custom identifier of the order.
        mainValueMap->appendChild(valuemap);
    }

    O2G2Ptr<IO2GRequest> request = factory->createOrderRequest(mainValueMap);
    for (int i = 0; i < request->getChildrenCount(); i++)
    {
        O2G2Ptr<IO2GRequest> childRequest = request->getChildRequest(i);
        mActions[childRequest->getRequestID()] = deleteOrderAction;
    }
    mSession->sendRequest(request);
}

/** Create True Market order */
void CreateOrderSample::createTrueMarketOrder(const char *offerID, const char *accountID, int amount, const char * buySell)
{
    O2G2Ptr<IO2GRequestFactory> factory = mSession->getRequestFactory();

    O2G2Ptr<IO2GValueMap> valuemap = factory->createValueMap();
    valuemap->setString(Command, O2G2::Commands::CreateOrder);
    valuemap->setString(OrderType, O2G2::Orders::TrueMarketOpen);
    valuemap->setString(AccountID, accountID);            // The identifier of the account the order should be placed for.
    valuemap->setString(OfferID, offerID);                // The identifier of the instrument the order should be placed for.
    valuemap->setString(BuySell, buySell);                // The order direction: O2G2::Sell for "Sell", O2G2::Buy for "Buy".
    valuemap->setInt(Amount, amount);                    // The quantity of the instrument to be bought or sold.
    valuemap->setString(CustomID, "TrueMarketOrder");    // The custom identifier of the order.

    O2G2Ptr<IO2GRequest> request = factory->createOrderRequest(valuemap);
    mSession->sendRequest(request);
}

/** Create Stop Entry order */
void CreateOrderSample::createEntryStopOrder(const char * offerID, const char * accountID, int amount, double rate, const char * buySell)
{
    O2G2Ptr<IO2GRequestFactory> factory = mSession->getRequestFactory();

    O2G2Ptr<IO2GValueMap> valuemap = factory->createValueMap();
    valuemap->setString(Command, O2G2::Commands::CreateOrder);
    valuemap->setString(OrderType, O2G2::Orders::StopEntry);
    valuemap->setString(AccountID, accountID);         // The identifier of the account the order should be placed for.
    valuemap->setString(OfferID, offerID);             // The identifier of the instrument the order should be placed for.
    valuemap->setString(BuySell, buySell);             // The order direction (O2G2::Buy for buy, O2G2::Sell for sell)
    valuemap->setDouble(Rate, rate);                 // The rate at which the order must be filled (above current rate for Buy, bellow current rate for Sell)
    valuemap->setInt(Amount, amount);                 // The quantity of the instrument to be bought or sold.
    valuemap->setString(CustomID, "StopEntryOrder"); // The custom identifier of the order.

    O2G2Ptr<IO2GRequest> request = factory->createOrderRequest(valuemap);
    mSession->sendRequest(request);
    // Store request and set delete actions. After order has been created
    // then delete its.
    mActions[request->getRequestID()] = deleteOrderAction;
}

/** Create Limit Entry order */
void CreateOrderSample::createEntryLimitOrder(const char * offerID, const char * accountID, int amount, double rate, const char * buySell)
{
    O2G2Ptr<IO2GRequestFactory> factory = mSession->getRequestFactory();

    O2G2Ptr<IO2GValueMap> valuemap = factory->createValueMap();
    valuemap->setString(Command, O2G2::Commands::CreateOrder);
    valuemap->setString(OrderType, O2G2::Orders::LimitEntry);
    valuemap->setString(AccountID, accountID);          // The identifier of the account the order should be placed for.
    valuemap->setString(OfferID, offerID);              // The identifier of the instrument the order should be placed for.
    valuemap->setString(BuySell, buySell);              // The order direction (O2G2::Buy for buy, O2G2::Sell for sell)
    valuemap->setDouble(Rate, rate);                  // The rate at which the order must be filled (below current rate for Buy, above current rate for Sell)
    valuemap->setInt(Amount, amount);                  // The quantity of the instrument to be bought or sold.
    valuemap->setString(CustomID, "LimitEntryOrder"); // The custom identifier of the order.

    O2G2Ptr<IO2GRequest> request = factory->createOrderRequest(valuemap);
    mSession->sendRequest(request);
}

/** Create True Market Close order */
void CreateOrderSample::createTrueMarketCloseOrder(const char *offerID, const char *accountID, const char * tradeID, int amount, const char * buySell)
{
    O2G2Ptr<IO2GRequestFactory> factory = mSession->getRequestFactory();

    O2G2Ptr<IO2GValueMap> valuemap = factory->createValueMap();
    valuemap->setString(Command, O2G2::Commands::CreateOrder);
    valuemap->setString(OrderType, O2G2::Orders::TrueMarketClose);
    valuemap->setString(AccountID, accountID);                // The identifier of the account the order should be placed for.
    valuemap->setString(OfferID, offerID);                    // The identifier of the instrument the order should be placed for.
    valuemap->setString(TradeID, tradeID);                    // The identifier of the trade to be closed.
    valuemap->setInt(Amount, amount);                         // The quantity of the instrument to be bought or sold. Must be <= to the size of the position ("Trades" table, Lot column).

    valuemap->setString(BuySell, buySell);                    // The order direction: O2G2::Buy for Buy, O2G2::Sell for Sell. Must be opposite to the direction of the trade.
    valuemap->setString(CustomID, "CloseTrueMarketOrder");  // The custom identifier of the order.

    O2G2Ptr<IO2GRequest> request = factory->createOrderRequest(valuemap);
    mSession->sendRequest(request);
}

/** Create Market order.*/
void CreateOrderSample::createMarketOrder(const char *offerID, const char *accountID, int amount, double rate, const char * buySell)
{
    O2G2Ptr<IO2GRequestFactory> factory = mSession->getRequestFactory();

    O2G2Ptr<IO2GValueMap> valuemap = factory->createValueMap();
    valuemap->setString(Command, O2G2::Commands::CreateOrder);
    valuemap->setString(OrderType, O2G2::Orders::MarketOpen);
    valuemap->setString(AccountID, accountID);                // The identifier of the account the order should be placed for.
    valuemap->setString(OfferID, offerID);                    // The identifier of the instrument the order should be placed for.
    valuemap->setString(BuySell, buySell);                    // The order direction (use O2G2::Buy for Buy, O2G2::Sell for Sell)
    valuemap->setDouble(Rate, rate);                          // The rate at which the order must be filled.
    valuemap->setInt(Amount, amount);                         // The quantity of the instrument to be bought or sold.
    valuemap->setString(CustomID, "OpenMarketOrder");         // The custom identifier of the order.

    O2G2Ptr<IO2GRequest> request = factory->createOrderRequest(valuemap);
    mSession->sendRequest(request);
}

/** Create Market order */
void CreateOrderSample::createMarketCloseOrder(const char *offerID, const char *accountID, const char * tradeID, int amount, double rate, const char * buySell)
{
    O2G2Ptr<IO2GRequestFactory> factory = mSession->getRequestFactory();

    O2G2Ptr<IO2GValueMap> valuemap = factory->createValueMap();
    valuemap->setString(Command, O2G2::Commands::CreateOrder);
    valuemap->setString(OrderType, O2G2::Orders::MarketClose);
    valuemap->setString(AccountID, accountID);                // The identifier of the account the order should be placed for.
    valuemap->setString(OfferID, offerID);                    // The identifier of the instrument the order should be placed for.
    valuemap->setString(TradeID, tradeID);                    // The identifier of the trade to be closed.
    valuemap->setString(BuySell, buySell);                    // The order direction (O2G2::Buy - for Buy, O2G2::Sell - for Sell). Must be opposite to the direction of the trade.
    valuemap->setInt(Amount, amount);                         // The quantity of the instrument to be bought or sold.  Must <= to the size of the position (Lot column of the trade).
    valuemap->setDouble(Rate, rate);                          // The rate at which the order must be filled.
    valuemap->setString(CustomID, "CloseMarketOrder");        // The custom identifier of the order.
    O2G2Ptr<IO2GRequest> request = factory->createOrderRequest(valuemap);
    mSession->sendRequest(request);
}

/** Create Range order.*/
void CreateOrderSample::createRangeOrder(const char *offerID, const char *accountID, int amount, double rateMin, double rateMax, const char * buySell)
{
    O2G2Ptr<IO2GRequestFactory> factory = mSession->getRequestFactory();

    O2G2Ptr<IO2GValueMap> valuemap = factory->createValueMap();
    valuemap->setString(Command, O2G2::Commands::CreateOrder);
    valuemap->setString(OrderType, O2G2::Orders::MarketOpenRange);
    valuemap->setString(AccountID, accountID);       // The identifier of the account the order should be placed for.
    valuemap->setString(OfferID, offerID);           // The identifier of the instrument the order should be placed for.
    valuemap->setString(BuySell, buySell);           // The order direction (O2G2::Buy for buy, O2G2::Sell for sell).
    valuemap->setDouble(RateMin, rateMin);           // The minimum rate at which the order can be filled.
    valuemap->setDouble(RateMax, rateMax);           // The maximum rate at which the order can be filled.
    valuemap->setString(CustomID, "OpenRangeOrder"); // The custom identifier of the order.
    valuemap->setInt(Amount, amount);

    O2G2Ptr<IO2GRequest> request = factory->createOrderRequest(valuemap);
    mSession->sendRequest(request);
}
/** Create Range Close order */
void CreateOrderSample::createRangeCloseOrder(const char *offerID, const char *accountID, const char *tradeID, int amount, double rateMin, double rateMax, const char * buySell)
{
    O2G2Ptr<IO2GRequestFactory> factory = mSession->getRequestFactory();

    O2G2Ptr<IO2GValueMap> valuemap = factory->createValueMap();
    valuemap->setString(Command, O2G2::Commands::CreateOrder);
    valuemap->setString(OrderType, O2G2::Orders::MarketCloseRange);
    valuemap->setString(AccountID, accountID);
    valuemap->setString(OfferID, offerID);                // The identifier of the instrument the order should be placed for.
    valuemap->setString(TradeID, tradeID);                // The identifier of the trade to be closed.
    valuemap->setString(BuySell, buySell);                // The order direction (O2G2::Buy for Buy, O2G2::Sell for Sell). Must be opposite to the direction of the trade.
    valuemap->setInt(Amount, amount);                    // The quantity of the instrument to be bought or sold. Must be <= size of the position (Lot of the trade). Must be divisible by baseUnitSize.
    valuemap->setDouble(RateMin, rateMin);                // The minimum rate at which the order can be filled.
    valuemap->setDouble(RateMax, rateMax);                // The maximum rate at which the order can be filled.
    valuemap->setString(CustomID, "CloseRangeOrder");    // The custom identifier of the order.
    O2G2Ptr<IO2GRequest> request = factory->createOrderRequest(valuemap);
    mSession->sendRequest(request);
}

void CreateOrderSample::onOrderResponse(IO2GResponse *response)
{
    O2G2Ptr<IO2GResponseReaderFactory> factory = mSession->getResponseReaderFactory();

    // Log order response
    O2G2Ptr<IO2GOrderResponseReader> orderResponseReader = factory->createOrderResponseReader(response);
    if (!orderResponseReader)
        return;

    char buffer[512];
    sprintf_s(buffer, sizeof(buffer), "Create order: OrderID=%s",
                           orderResponseReader->getOrderID());
    mLog->logString(buffer);


    // Try fine the request in the stored request id
    std::string requestID = response->getRequestID();
    std::map<std::string, actions>::iterator iter;
    iter = mActions.find(requestID);
    // In case the order request doesn't assosiated with actions then skip
    // response
    if (iter == mActions.end())
        return;
    switch (iter->second)
    {
    case deleteOrderAction:
        deleteOrder(orderResponseReader->getOrderID());
    break;
    case editOrderAction:
        editOrder(orderResponseReader->getOrderID(),
                  mParams->mAccountID.c_str(),
                  mParams->mBaseAmount*5,
                  // The original rate have 500 point(50 pips),
                  // increase rate to 5 pips.
                  mParams->mAsk - mParams->mPointSize * 550);
    break;
    case manageOTOAction:
        mOrders.push_back(orderResponseReader->getOrderID());
        if (mOrders.size() == 5)
        {
            joinToNewContingencyGroup(mParams->mAccountID.c_str(), 2/*OTO_CONTINGENCY_GROUP*/,
                mOrders[0].c_str(), mOrders[1].c_str(), mOrders[2].c_str());
        }
        break;
    }
    mActions.erase(iter);
}

void CreateOrderSample::onCommandResponse(IO2GResponse *response)
{
    O2G2Ptr<IO2GResponseReaderFactory> factory = mSession->getResponseReaderFactory();

    // Log order response
    O2G2Ptr<IO2GOrderResponseReader> orderResponseReader = factory->createOrderResponseReader(response);
    if (!orderResponseReader)
        return;

    char buffer[512];
    sprintf_s(buffer, sizeof(buffer), "Create order: OrderID=%s",
                           orderResponseReader->getOrderID());
    mLog->logString(buffer);


    // Try fine the request in the stored request id
    std::string requestID = response->getRequestID();
    std::map<std::string, actions>::iterator iter;
    iter = mActions.find(requestID);
    // In case the order request doesn't assosiated with actions then skip
    // response
    if (iter == mActions.end())
        return;

    actions act = iter->second;
    mActions.erase(iter);
    if (act != manageOTOAction)
        return;

    static int step = 0;
    static int requestProcessed = 0;
    requestProcessed++;
    if (step == 0 && requestProcessed == 2)
    {
        joinToExistingContingencyGroup(mParams->mAccountID.c_str(), 2/*OTO_CONTINGENCY_GROUP*/,
            mOrders[0].c_str(), mOrders[3].c_str(), mOrders[4].c_str());
        step++;
        requestProcessed = 0;
    }
    else if (step == 1 && requestProcessed == 2)
    {
        removeFromGroup(mParams->mAccountID.c_str(), mOrders[1].c_str(), mOrders[2].c_str());
        removeFromGroup(mParams->mAccountID.c_str(), mOrders[3].c_str(), mOrders[4].c_str());
        step++;
        requestProcessed = 0;
    }
    else if (step == 2 && requestProcessed == 4)
    {
        deleteOrder(mOrders[0].c_str());
        deleteOrder(mOrders[1].c_str());
        deleteOrder(mOrders[2].c_str());
        deleteOrder(mOrders[3].c_str());
        deleteOrder(mOrders[4].c_str());
    }
}


/** Edit order.*/
void CreateOrderSample::editOrder(const char *orderID, const char* accountID, int amount, double rate)
{
    O2G2Ptr<IO2GRequestFactory> factory = mSession->getRequestFactory();

    O2G2Ptr<IO2GValueMap> valuemapChangeOrder = factory->createValueMap();
    valuemapChangeOrder->setString(Command, "EditOrder");
    valuemapChangeOrder->setString(OrderID, orderID);
    valuemapChangeOrder->setString(AccountID, accountID);
    valuemapChangeOrder->setDouble(Rate, rate);
    valuemapChangeOrder->setInt(Amount, amount);
    valuemapChangeOrder->setString(CustomID, "EditOrder");

    O2G2Ptr<IO2GRequest> requestChangeOrder = factory->createOrderRequest(valuemapChangeOrder);
    mSession->sendRequest(requestChangeOrder);

}

void CreateOrderSample::prepareParamsFromLoginRules(IO2GLoginRules *loginRules)
{
    mParams = new OrderCreationParam();

    O2G2Ptr<IO2GResponseReaderFactory> factory = mSession->getResponseReaderFactory();
    // Gets first account from login.
    O2G2Ptr<IO2GResponse> accountsResponse = loginRules->getTableRefreshResponse(Accounts);
    O2G2Ptr<IO2GAccountsTableResponseReader> accountsReader = factory->createAccountsTableReader(accountsResponse);
    O2G2Ptr<IO2GAccountRow> account = accountsReader->getRow(0);
    // Store account id
    mParams->mAccountID = account->getAccountID();
    // Store base amount
    mParams->mBaseAmount = account->getBaseUnitSize();
    // Get offers for eur/usd
    O2G2Ptr<IO2GResponse> offerResponse = loginRules->getTableRefreshResponse(Offers);
    O2G2Ptr<IO2GOffersTableResponseReader> offersReader = factory->createOffersTableReader(offerResponse);
    for (int i = 0; i < offersReader->size(); i++)
    {
        O2G2Ptr<IO2GOfferRow> offer = offersReader->getRow(i);
        if (_stricmp(offer->getInstrument(), "EUR/USD") == 0)
        {
            mParams->mOfferID = offer->getOfferID();
            mParams->mAsk = offer->getAsk();
            mParams->mBid = offer->getBid();
            mParams->mPointSize = offer->getPointSize();
            break;
        }
    }

}

/** Prepare parameters for Limit Entry order, than create an order */
void CreateOrderSample::prepareParamsAndCallEntryLimitOrderWithStopLimit(const char *offerID, const char *accountID, int amount, double pointSize, int numPoints, const char * buySell)
{
    double distance = 10.0; // Distance in pips
    double pegStop = 0;
    double pegLimit = 0;
    double orderRate = 0;
    double curRate = 0;
    if (!strcmp(buySell, O2G2::Buy))
    {
        curRate = mParams->mAsk;
        orderRate = curRate - pointSize * numPoints;  // The buy order rate must be below the market.
        pegStop = -distance;
        pegLimit = distance;
    }
    else                        // BuySell = O2G2::Sell
    {
        curRate = mParams->mBid;
        orderRate = curRate + pointSize * (numPoints + distance);  // The sell order rate must be above the market.
        pegStop = distance;
        pegLimit = -distance;
    }

    createEntryLimitOrderWithStopLimit(offerID, accountID, amount, orderRate, buySell, pegStop, pegLimit);
}


/** Create entry limit order with attached stop limit.*/
void CreateOrderSample::createEntryLimitOrderWithStopLimit(const char *offerID, const char *accountID, int amount, double rate, const char * buySell, double stopOffset, double limitOffset)
{
    O2G2Ptr<IO2GRequestFactory> factory = mSession->getRequestFactory();

    O2G2Ptr<IO2GValueMap> valuemap = factory->createValueMap();
    valuemap->setString(Command, O2G2::Commands::CreateOrder);
    valuemap->setString(OrderType, O2G2::Orders::LimitEntry);
    valuemap->setString(AccountID, accountID);          // The identifier of the account the order should be placed for.
    valuemap->setString(OfferID, offerID);              // The identifier of the instrument the order should be placed for.
    valuemap->setString(BuySell, buySell);              // The order direction (O2G2::Buy for buy, O2G2::Sell for sell)
    valuemap->setDouble(Rate, rate);                    // The rate at which the order must be filled (below current rate for Buy, above current rate for Sell)
    valuemap->setInt(Amount, amount);                   // The quantity of the instrument to be bought or sold.
    valuemap->setString(CustomID, "LimitEntryOrderWithStopLimit"); // The custom identifier of the order.
    valuemap->setString(PegTypeStop, O2G2::Peg::FromClose);   // The peg offset type
    valuemap->setDouble(PegOffsetStop, stopOffset);
    valuemap->setString(PegTypeLimit, O2G2::Peg::FromOpen);     // The peg limit type
    valuemap->setDouble(PegOffsetLimit, limitOffset);


    O2G2Ptr<IO2GRequest> request = factory->createOrderRequest(valuemap);
    mSession->sendRequest(request);

    // Store request ID and set the order for further editing. After the order has been created,
    // the order rate and amount will be changed.
    mActions[request->getRequestID()] = editOrderAction;
}

void CreateOrderSample::createStopOrder(const char *offerID, const char *accountID, const char *tradeID, int amount, double rate, const char * buySell)
{
    O2G2Ptr<IO2GRequestFactory> factory = mSession->getRequestFactory();

    O2G2Ptr<IO2GValueMap> valuemap = factory->createValueMap();
    valuemap->setString(Command, O2G2::Commands::CreateOrder);
    valuemap->setString(OrderType, O2G2::Orders::Stop);
    valuemap->setString(AccountID, accountID);        // The identifier of the account the order should be placed for
    valuemap->setString(OfferID, offerID);            // The identifier of the instrument the order should be placed for
    valuemap->setString(TradeID, tradeID);            // The identifier of the trade to be closed
    valuemap->setString(BuySell, buySell);            // The order direction (O2G2::Buy for Buy, O2G2::Sell for Sell). Must be opposite to the direction of the trade
    valuemap->setInt(Amount, amount);                // The quantity of the instrument to be bought or sold. Must be = size of the position (Lot of the trade). Must be divisible by baseUnitSize
    valuemap->setDouble(Rate, rate);                // The rate at which the order must be filled (< market for "Sell" order, > market for "Buy" order)
    valuemap->setString(CustomID, "StopOrder");        // The custom identifier of the order
    O2G2Ptr<IO2GRequest> request = factory->createOrderRequest(valuemap);
    mSession->sendRequest(request);
}

void CreateOrderSample::createLimitOrder(const char *offerID, const char *accountID, const char *tradeID, int amount, double rate, const char * buySell)
{
    O2G2Ptr<IO2GRequestFactory> factory = mSession->getRequestFactory();

    O2G2Ptr<IO2GValueMap> valuemap = factory->createValueMap();

    valuemap->setString(Command, O2G2::Commands::CreateOrder);
    valuemap->setString(OrderType, O2G2::Orders::Limit);
    valuemap->setString(AccountID, accountID);
    valuemap->setString(OfferID, offerID);            // The identifier of the instrument the order should be placed for
    valuemap->setString(TradeID, tradeID);            // The identifier of the trade to be closed
    valuemap->setString(BuySell, buySell);            // The order direction(O2G2::Buy for Buy, O2G2::Sell for Sell). Must be opposite to the direction of the trade
    valuemap->setInt(Amount, amount);                 // The quantity of the instrument to be bought or sold. Must == the size of the position (Lot of the trade).
    valuemap->setDouble(Rate, rate);                  // The rate at which the order must be filled (> market for Sell, < market for Buy)
    valuemap->setString(CustomID, "LimitOrder");      // The custom identifier of the order
    O2G2Ptr<IO2GRequest> request = factory->createOrderRequest(valuemap);
    mSession->sendRequest(request);
}

/** Delete order command.*/
void CreateOrderSample::deleteOrder(const char* orderID)
{
    O2G2Ptr<IO2GRequestFactory> factory = mSession->getRequestFactory();
    O2G2Ptr<IO2GValueMap> valuemap = factory->createValueMap();
    valuemap->setString(Command, O2G2::Commands::DeleteOrder);
    valuemap->setString(OrderID, orderID);
    O2G2Ptr<IO2GRequest> request = factory->createOrderRequest(valuemap);
    mSession->sendRequest(request);
}

/** Prepare parameters for True Market Close order, then create an order */
void CreateOrderSample::prepareParamsAndCallTrueMarketCloseOrder(IO2GTradeRow *trade)
{
    //The order direcion of the close order must be opposite to the direction of the trade.
    const char * tradeBuySell = trade->getBuySell();
    if (!strcmp(tradeBuySell, O2G2::Buy))                    // trade BuySell=O2G2::Buy => order BuySell = O2G2::Sell
        createTrueMarketCloseOrder(trade->getOfferID(), trade->getAccountID(), trade->getTradeID(), trade->getAmount(), O2G2::Sell);
    else                                            // trade BuySell=O2G2::Sell => order BuySell = O2G2::Buy
        createTrueMarketCloseOrder(trade->getOfferID(), trade->getAccountID(), trade->getTradeID(), trade->getAmount(), O2G2::Buy);
}

/** Prepare parameters for Market close order, then create and order */
void CreateOrderSample::prepareParamsAndCallMarketCloseOrder(IO2GTradeRow *trade)
{
    const char * tradeBuySell = trade->getBuySell();
    // Close order is opposite to the trade
    bool buyOrder = false;
    if (!strcmp (tradeBuySell, O2G2::Sell))
        buyOrder = true;

    const char *tradeOfferID = trade->getOfferID();
    double rate = 0;
    // Ask price for Buy and Bis price for Sell
    rate = buyOrder ?  mParams->mAsk : mParams->mBid;

    createMarketCloseOrder(tradeOfferID,
                           trade->getAccountID(),
                           trade->getTradeID(),
                           trade->getAmount(),
                           rate,
                           buyOrder ? O2G2::Buy : O2G2::Sell);
}

/** Prepare parameters for Range Close order, then create an order */
void CreateOrderSample::prepareParamsAndCallRangeCloseOrder(IO2GTradeRow * trade, int atMarket)
{
    const char * tradeBuySell = trade->getBuySell();
    // Close order is opposite to the trade
    bool buyOrder = true;
    if (strcmp(tradeBuySell, O2G2::Buy) == 0)
        buyOrder = false;

    // Get rate, then calculate rateMin and rateMax
    const char * tradeOfferID = trade->getOfferID();
    double rate = 0;
    rate = buyOrder ? mParams->mAsk : mParams->mBid;

    double pointSize = mParams->mPointSize;
    double rateMin = rate - atMarket * pointSize;
    double rateMax = rate + atMarket * pointSize;

    createRangeCloseOrder(tradeOfferID,
                          trade->getAccountID(),
                          trade->getTradeID(),
                          trade->getAmount(),
                          rateMin, rateMax,
                          buyOrder ? O2G2::Buy : O2G2::Sell);
}

/** Prepare parameters for Stop order, then create an order */
void CreateOrderSample::prepareParamsAndCallStopOrder(IO2GTradeRow * trade, int atMarket)
{
    const char * tradeBuySell = trade->getBuySell();
    // Close order is opposite to the trade
    bool buyOrder = false;
    if (!strcmp (tradeBuySell, O2G2::Sell))
        buyOrder = true;

    const char * tradeOfferID = trade->getOfferID();
    double curRate = 0;
    double stopRate = 0;
    double pointSize = mParams->mPointSize;
    if (buyOrder)
    {
        curRate = mParams->mAsk;
        stopRate = curRate + atMarket * pointSize;

    }
    else
    {
        curRate = mParams->mBid;
        stopRate = curRate - atMarket * pointSize;
    }

    createStopOrder(tradeOfferID,
                    trade->getAccountID(),
                    trade->getTradeID(),
                    trade->getAmount(),
                    stopRate,
                    buyOrder ? O2G2::Buy : O2G2::Sell);
}

/** Prepare parameters for Limit order, then create an order */
void CreateOrderSample::prepareParamsAndCallLimitOrder(IO2GTradeRow * trade, int atMarket)
{
    const char * tradeBuySell = trade->getBuySell();
    // Close order is opposite to the trade
    bool buyOrder = false;
    if (!strcmp (tradeBuySell, O2G2::Sell))
        buyOrder = true;

    const char * tradeOfferID = trade->getOfferID();
    double curRate = 0;
    double limitRate = 0;
    double pointSize = mParams->mPointSize;
    if (buyOrder)
    {
        curRate = mParams->mAsk;
        limitRate = curRate - atMarket * pointSize;
    }
    else
    {
        curRate = mParams->mBid;
        limitRate = curRate + atMarket * pointSize;
    }

    createLimitOrder(tradeOfferID,
                     trade->getAccountID(),
                     trade->getTradeID(),
                     trade->getAmount(),
                     limitRate, buyOrder ? O2G2::Buy : O2G2::Sell);
}
/** Prepare parameters for Stop Entry order, then create an order */
void CreateOrderSample::prepareParamsAndCallEntryStopOrder(const char *offerID, const char *accountID, int amount, double pointSize, int numPoints, const char * buySell)
{
    double curRate = 0;
    double orderRate = 0;
    if (!strcmp(buySell, O2G2::Buy))
    {
        curRate = mParams->mAsk;
        orderRate = curRate + pointSize * numPoints;  // The buy order rate must be above the market.
    }
    else    // BuySell = O2G2::Sell
    {
        curRate = mParams->mBid;
        orderRate = curRate - pointSize * numPoints;  // The sell order rate must be below the market.
    }
    createEntryStopOrder(offerID, accountID, amount, orderRate, buySell);
}

/** Prepare parameters for Limit Entry order, than create an order */
void CreateOrderSample::prepareParamsAndCallEntryLimitOrder(const char *offerID, const char *accountID, int amount, double pointSize, int numPoints, const char * buySell)
{
    double orderRate = 0;
    double curRate = 0;
    if (!strcmp(buySell, O2G2::Buy))
    {
        curRate = mParams->mAsk;
        orderRate = curRate - pointSize * numPoints;  // The buy order rate must be below the market.
    }
    else                        // BuySell = O2G2::Sell
    {
        curRate = mParams->mBid;
        orderRate = curRate + pointSize * numPoints;  // The sell order rate must be above the market.
    }
    createEntryLimitOrder(offerID, accountID, amount, orderRate, buySell);
}

/** Prepare parameters for Range order, then create an order */
void CreateOrderSample::prepareParamsAndCallRangeOrder(const char *offerID, const char *accountID, int amount, double pointSize, int atMarket, const char * buySell)
{
    bool bBuy = strcmp(buySell, O2G2::Buy) == 0;
    double curRate =  bBuy ? mParams->mAsk : mParams->mBid;
    double rateMin = curRate - pointSize * atMarket;
    double rateMax = curRate + pointSize * atMarket;
    createRangeOrder(offerID, accountID, amount, rateMin, rateMax, buySell);
}

/** Prepare parameters and call OCO orders */
void CreateOrderSample::prepareParamsAndCallOCOOrders(const char *offerID, const char *accountID, int amount, double pointSize, const char * buySell)
{
    bool bBuy = (strcmp(buySell, O2G2::Buy) == 0);
    double rate = bBuy ? mParams->mAsk : mParams->mBid;
    double amendment = 5 * pointSize;
    if (strcmp(buySell, O2G2::Buy) == 0)
        rate -= amendment;
    else
        rate += amendment;
    createOCO(offerID, accountID, amount, rate, buySell, 3);
}

/** When the trade is created, either create one of the close orders (Market Close, True Market Close, Range Close) or add Stop/Limit to the trade */
void CreateOrderSample::CreateOrderForTrade(IO2GTradeRow *trade)
{
    switch (orderForTradeNum)
    {
    case 0:
        prepareParamsAndCallMarketCloseOrder(trade);
        orderForTradeNum++;
        break;
    case 1:
        prepareParamsAndCallTrueMarketCloseOrder(trade);
        orderForTradeNum++;
        break;
    case 2:
        prepareParamsAndCallRangeCloseOrder(trade, 10);
        orderForTradeNum++;
        break;
    case 3:
        prepareParamsAndCallStopOrder(trade, 100);
        prepareParamsAndCallLimitOrder(trade, 100);
        orderForTradeNum++;
        break;
    default:
        break;
    }
}

