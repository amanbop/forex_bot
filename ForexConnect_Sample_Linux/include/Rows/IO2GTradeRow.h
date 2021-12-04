
/*
    !!! Caution:
    Do not change anything in this source code because it was automatically generated
    from XML class description
*/
#pragma once

class IO2GTradeRow : public IAddRef
{
 protected:
  IO2GTradeRow();
 public:
    
    virtual const char* getTradeID() = 0;
    virtual const char* getAccountID() = 0;
    virtual const char* getAccountName() = 0;
    virtual const char* getAccountKind() = 0;
    virtual const char* getOfferID() = 0;
    virtual int getAmount() = 0;
    virtual const char* getBuySell() = 0;
    virtual double getOpenRate() = 0;
    virtual DATE getOpenTime() = 0;
    virtual const char* getOpenQuoteID() = 0;
    virtual const char* getOpenOrderID() = 0;
    virtual const char* getOpenOrderReqID() = 0;
    virtual const char* getOpenOrderRequestTXT() = 0;
    virtual double getCommission() = 0;
    virtual double getRolloverInterest() = 0;
    virtual const char* getTradeIDOrigin() = 0;
    virtual int getUsedMargin() = 0;
    virtual const char* getValueDate() = 0;
    virtual const char* getParties() = 0;
};
