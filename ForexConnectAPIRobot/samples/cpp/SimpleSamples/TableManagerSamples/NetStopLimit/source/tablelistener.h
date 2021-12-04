#pragma once

//#include "c:\program files\candleworks\forexconnectapi\include\io2gtable.h"
#include "stdafx.h"

class TableListener :
    public IO2GTableListener
{
    struct __OrdersState {
        int stop : 1;
        int limit : 1;
    } ordersState;
    HANDLE mEvent;
    long mRef;
    std::string mStopOrderID;
    std::string mLimitOrderID;
public:
    long addRef(void);
    long release();
    void onStatusChanged(O2GTableStatus);
    void onAdded(const char *,IO2GRow *);
    void onChanged(const char *,IO2GRow *);
    void onDeleted(const char *,IO2GRow *);

    std::string getOrderID( const char *orderType ) const;
    TableListener(void);
    HANDLE getEvent();
    ~TableListener(void);
};

