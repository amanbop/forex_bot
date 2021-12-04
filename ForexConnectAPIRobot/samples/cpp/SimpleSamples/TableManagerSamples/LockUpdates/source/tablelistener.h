#pragma once

#include "stdafx.h"

class TableListener :
    public IO2GTableListener
{
    HANDLE mEvent;
    long mRef;
    std::string mRequestID;
    IO2GTableManager *mTableManager;
public:
    long addRef(void);
    long release();
    void onStatusChanged(O2GTableStatus);
    void onAdded(const char *,IO2GRow *);
    void onChanged(const char *,IO2GRow *);
    void onDeleted(const char *,IO2GRow *);

    void setRequest(const char *requestID);
    TableListener(IO2GTableManager *);
    HANDLE getEvent();
    ~TableListener(void);
};

