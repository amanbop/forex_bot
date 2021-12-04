

#include "../include/header.h"
#include "../include/Example.h"

#include "../include/Session.h"
#include "../include/ResponseListener.h"

#define autoStepSleepTime 500

CExample::CExample(IO2GSession* session):
CBaseExample(session)
{
}

// Helper method to create valueMap for order
IO2GValueMap* CExample::createOrder(const char* offerID, const char* accountID, double rate, int amount, const char* buySell, const char* orderType)
{
    using namespace O2G2;
    O2G2Ptr<IO2GRequestFactory> factory = getRequestFactory();

    IO2GValueMap* valuemap = factory->createValueMap();

    valuemap->setString(Command, Commands::CreateOrder);    // for OTO orders it's not mandatory field
    valuemap->setString(OrderType, orderType);
    valuemap->setString(AccountID, accountID);
    valuemap->setString(OfferID, offerID);
    valuemap->setString(BuySell, buySell);
    valuemap->setDouble(Rate, rate);
    valuemap->setInt(Amount, amount);

    return valuemap;
}

// Helper method to create valueMap for exist order
IO2GValueMap* CExample::existOrder(const char* orderID, const char* accountID, const char* command/* = NULL*/)
{
    using namespace O2G2;
    O2G2Ptr<IO2GRequestFactory> factory = getRequestFactory();

    IO2GValueMap* valuemap = factory->createValueMap();

    valuemap->setString(OrderID, orderID);
    valuemap->setString(AccountID, accountID);
    if (command != NULL)
        valuemap->setString(Command, command);

    return valuemap;
}



void CExample::run(bool autoStep)
{
    printf("Creating OTO orders(press any key to done)...\n");
    sendRequest(createOTO());
    getOrdersIDFromResponses();
    if (autoStep)
        mySleep(autoStepSleepTime);
    else
        getchar();

    printf("Removing all created orders(press any key to done)...\n");
    sendRequest(removeOrders());
    getResponseListener()->clear();
    mOrdersID.clear();
    if (autoStep)
        mySleep(autoStepSleepTime);
    else
        getchar();

    printf("Creating five simple orders(press any key to done)...\n");
    sendRequest(createOrders());
    getOrdersIDFromResponses();
    if (autoStep)
        mySleep(autoStepSleepTime);
    else
        getchar();

    printf("Creating new OTO group from with orders(press any key to done)...\n");
    sendRequest(joinToNewContingencyGroup());
    if (autoStep)
        mySleep(autoStepSleepTime);
    else
        getchar();

    printf("Attaching two orders to existing OTO group(press any key to done)...\n");
    sendRequest(joinToExistingContingencyGroup());
    if (autoStep)
        mySleep(autoStepSleepTime);
    else
        getchar();

    printf("Removing all created orders(press any key to done)...\n");
    sendRequest(removeOrders());
    if (autoStep)
        mySleep(autoStepSleepTime);
    else
        getchar();
}

IO2GRequest* CExample::removeOrders()
{
    using namespace O2G2;
    O2G2Ptr<IO2GRequestFactory> factory = getRequestFactory();

    std::string accountID = getAccountID(0);
    if (accountID.empty())
        return NULL;

    ///////// creating DeleteOrder command
    ///////// this is command cannot creating as tree
    O2G2Ptr<IO2GValueMap> valuemap = factory->createValueMap();
    valuemap->setString(Command, Commands::DeleteOrder);
    //set primary orderID or existing groupID

    size_t nCount = mOrdersID.size();
    for (size_t i = 0; i < nCount; i++)
        valuemap->appendChild(existOrder(mOrdersID[i].c_str(), accountID.c_str(), Commands::DeleteOrder));

    // create request from the valuemap
    return factory->createOrderRequest(valuemap);
}


IO2GRequest* CExample::createOrders()
{
    using namespace O2G2;
    O2G2Ptr<IO2GRequestFactory> factory = getRequestFactory();

    ///////// filling data
    std::string accountID = getAccountID(0);
    if (accountID.empty())
        return NULL;
    int iBaseUnitSize = getBaseUnitSize(0);

    O2G2Ptr<IO2GOfferRow> offer = getOffer("1");
    std::string offerID = offer->getOfferID();
    if (offerID.empty())
        return NULL;

    double rateOffer = offer->getAsk();
    double ratePrimary = rateOffer + offer->getPointSize() * 100;
    double rateSecondary1 = ratePrimary + offer->getPointSize() * 75;
    double rateSecondary2 = ratePrimary - offer->getPointSize() * 75;
    double rateSecondary3 = ratePrimary + offer->getPointSize() * 50;
    double rateSecondary4 = ratePrimary - offer->getPointSize() * 50;



    ///////// creating OTO command
    O2G2Ptr<IO2GValueMap> valuemap = factory->createValueMap();
    valuemap->setString(Command, Commands::CreateOrder);

    valuemap->appendChild(createOrder(offerID.c_str(), accountID.c_str(), ratePrimary,
                                      1 * iBaseUnitSize, O2G2::Buy, Orders::StopEntry));

    valuemap->appendChild(createOrder(offerID.c_str(), accountID.c_str(), rateSecondary1,
                                      2 * iBaseUnitSize, O2G2::Buy, Orders::StopEntry));

    valuemap->appendChild(createOrder(offerID.c_str(), accountID.c_str(), rateSecondary2,
                                      3 * iBaseUnitSize, O2G2::Sell, Orders::LimitEntry));

    valuemap->appendChild(createOrder(offerID.c_str(), accountID.c_str(), rateSecondary3,
                                      4 * iBaseUnitSize, O2G2::Sell, Orders::LimitEntry));

    valuemap->appendChild(createOrder(offerID.c_str(), accountID.c_str(), rateSecondary4,
                                      5 * iBaseUnitSize, O2G2::Sell, Orders::LimitEntry));

    // create request from the valuemap
    return factory->createOrderRequest(valuemap);

}

IO2GRequest* CExample::createOTO()
{
    using namespace O2G2;
    O2G2Ptr<IO2GRequestFactory> factory = getRequestFactory();

    ///////// filling data
    std::string accountID = getAccountID(0);
    if (accountID.empty())
        return NULL;

    O2G2Ptr<IO2GOfferRow> offer = getOffer("1");
    std::string offerID = offer->getOfferID();
    if (offerID.empty())
        return NULL;

    int iBaseUnitSize = getBaseUnitSize(0);

    double rateOffer = offer->getAsk();
    double ratePrimary = rateOffer + offer->getPointSize() * 25;
    double rateSecondary1 = rateOffer;
    double rateSecondary2 = rateOffer + offer->getPointSize() * 50;


    ///////// creating OTO command
    O2G2Ptr<IO2GValueMap> valuemap = factory->createValueMap();
    valuemap->setString(Command, Commands::CreateOTO);

    ///////// creating primary order
    valuemap->appendChild(createOrder(offerID.c_str(), accountID.c_str(), ratePrimary,
                                      1 * iBaseUnitSize, O2G2::Buy, Orders::StopEntry));

        ///////// creating two secondary order
        valuemap->appendChild(createOrder(offerID.c_str(), accountID.c_str(), rateSecondary1,
                                          2 * iBaseUnitSize, O2G2::Sell, Orders::StopEntry));

        valuemap->appendChild(createOrder(offerID.c_str(), accountID.c_str(), rateSecondary2,
                                          3 * iBaseUnitSize, O2G2::Buy, Orders::StopEntry));


        ///////// creating secondary OTO group
        O2G2Ptr<IO2GValueMap> valuemapSecondary = factory->createValueMap();
/*!*/   valuemapSecondary->setString(Command, Commands::CreateOTO);
        ///////// creating primary "IF" order
        valuemapSecondary->appendChild(createOrder(offerID.c_str(), accountID.c_str(), rateSecondary1,
/*!*/                                              0, O2G2::Sell, Orders::StopEntry)); // An IF order can be created only on the bid side.

            ///////// creating three secondary order
            valuemapSecondary->appendChild(createOrder(offerID.c_str(), accountID.c_str(), ratePrimary,
                                              7 * iBaseUnitSize, O2G2::Sell, Orders::LimitEntry));

            valuemapSecondary->appendChild(createOrder(offerID.c_str(), accountID.c_str(), rateSecondary2,
                                              6 * iBaseUnitSize, O2G2::Buy, Orders::StopEntry));

            valuemapSecondary->appendChild(createOrder(offerID.c_str(), accountID.c_str(), rateSecondary2,
                                              5 * iBaseUnitSize, O2G2::Buy, Orders::StopEntry));

        valuemap->appendChild(valuemapSecondary);


    // create request from the valuemap
    return factory->createOrderRequest(valuemap);
}

IO2GRequest* CExample::joinToNewContingencyGroup()
{
    if (mOrdersID.size() < 3)
    {
        printf("Not enough orders to continue the example");
        return NULL;
    }
    using namespace O2G2;
    O2G2Ptr<IO2GRequestFactory> factory = getRequestFactory();

    ///////// filling data
    std::string accountID = getAccountID(0);
    if (accountID.empty())
        return NULL;

    O2G2Ptr<IO2GOfferRow> offer = getOffer("1");
    std::string offerID = offer->getOfferID();
    if (offerID.empty())
        return NULL;

    std::string primaryID = mOrdersID[0];
    std::string secID1 = mOrdersID[1];
    std::string secID2 = mOrdersID[2];
    if (primaryID.empty())
        return NULL;

    ///////// creating JoinToNewContingencyGroup command
    ///////// this is command cannot creating as tree
    O2G2Ptr<IO2GValueMap> valuemap = factory->createValueMap();
    valuemap->setString(Command, Commands::JoinToNewContingencyGroup);
    //set contingencyType of creating group
/*!*/   valuemap->setInt(ContingencyGroupType, 2); // OTO

    ///////// primary order
    valuemap->appendChild(existOrder(primaryID.c_str(), accountID.c_str()));
    ///////// secondary order
    valuemap->appendChild(existOrder(secID1.c_str(), accountID.c_str()));
    ///////// secondary order
    valuemap->appendChild(existOrder(secID2.c_str(), accountID.c_str()));

    // create request from the valuemap
    return factory->createOrderRequest(valuemap);
}

IO2GRequest* CExample::joinToExistingContingencyGroup()
{
    if (mOrdersID.size() < 5)
    {
        printf("Not enough orders to continue the example");
        return NULL;
    }

    using namespace O2G2;
    O2G2Ptr<IO2GRequestFactory> factory = getRequestFactory();

    ///////// filling data
    std::string accountID = getAccountID(0);
    if (accountID.empty())
        return NULL;

    O2G2Ptr<IO2GOfferRow> offer = getOffer("1");
    std::string offerID = offer->getOfferID();
    if (offerID.empty())
        return NULL;

    std::string primaryID = mOrdersID[0];
    std::string secID1 = mOrdersID[3];
    std::string secID2 = mOrdersID[4];
    if (primaryID.empty())
        return NULL;

    ///////// creating JoinToExistingContingencyGroup command
    ///////// this is command cannot creating as tree
    O2G2Ptr<IO2GValueMap> valuemap = factory->createValueMap();
    valuemap->setString(Command, Commands::JoinToExistingContingencyGroup);
    //set primary orderID or existing groupID
/*!*/   valuemap->setString(ContingencyID, primaryID.c_str());
   valuemap->setInt(ContingencyGroupType, 2); // OTO

    ///////// secondary order
    valuemap->appendChild(existOrder(secID1.c_str(), accountID.c_str()));

    ///////// secondary order
    valuemap->appendChild(existOrder(secID2.c_str(), accountID.c_str()));

    // create request from the valuemap
    return factory->createOrderRequest(valuemap);
}

IO2GRequest* CExample::removeFromGroup()
{
    if (mOrdersID.size() < 3)
    {
        printf("Not enough orders to continue the example");
        return NULL;
    }

    using namespace O2G2;
    O2G2Ptr<IO2GRequestFactory> factory = getRequestFactory();

    ///////// filling data
    std::string accountID = getAccountID(0);
    if (accountID.empty())
        return NULL;

    std::string secID1 = mOrdersID[1];
    std::string secID2 = mOrdersID[2];

    ///////// creating JoinToExistingContingencyGroup command
    ///////// this is command cannot creating as tree
    O2G2Ptr<IO2GValueMap> valuemap = factory->createValueMap();
    valuemap->setString(Command, Commands::RemoveFromContingencyGroup);


    ///////// secondary order
    IO2GValueMap* valuemapRemove = existOrder(secID1.c_str(), accountID.c_str());
    valuemapRemove->setString(Command, Commands::RemoveFromContingencyGroup);
    valuemap->appendChild(valuemapRemove);

    ///////// secondary order
    valuemapRemove = existOrder(secID2.c_str(), accountID.c_str());
    valuemapRemove->setString(Command, Commands::RemoveFromContingencyGroup);
    valuemap->appendChild(valuemapRemove);


    // create request from the valuemap
    return factory->createOrderRequest(valuemap);
}

