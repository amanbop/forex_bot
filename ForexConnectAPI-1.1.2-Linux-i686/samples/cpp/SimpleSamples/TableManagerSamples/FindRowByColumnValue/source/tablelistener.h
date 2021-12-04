#pragma once

//#include "c:\program files\candleworks\forexconnectapi\include\io2gtable.h"
#include "stdafx.h"

class TableListener :
    public IO2GTableListener
{
    HANDLE mEvent;
    long mRef;
    int mOrdersCreated;
    std::string mRequestID;
    std::string mOrderID;
public:
    long addRef(void);
    long release();
    void onStatusChanged(O2GTableStatus);
    void onAdded(const char *,IO2GRow *);
    void onChanged(const char *,IO2GRow *);
    void onDeleted(const char *,IO2GRow *);

    void setRequest(const char *sRequestID);
    TableListener(void);
    HANDLE getEvent();
    ~TableListener(void);
};

