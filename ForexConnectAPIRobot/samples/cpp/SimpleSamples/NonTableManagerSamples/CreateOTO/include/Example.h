
#pragma once

#include "ExampleBase.h"

class CExample : public CBaseExample
{
    IO2GValueMap* createOrder(const char* offerID, const char* accountID, double rate,
                              int amount, const char* buySell, const char* orderType);
    IO2GValueMap* existOrder(const char* orderID, const char* accountID, const char* command = NULL);

protected:
    IO2GRequest* removeOrder(int index);
    IO2GRequest* removeOrders();
    IO2GRequest* createOrders();
    IO2GRequest* createOTO();
    IO2GRequest* joinToNewContingencyGroup();
    IO2GRequest* joinToExistingContingencyGroup();
    IO2GRequest* removeFromGroup();

public:

    CExample(IO2GSession* session);

    virtual void run(bool autoStep);
};

