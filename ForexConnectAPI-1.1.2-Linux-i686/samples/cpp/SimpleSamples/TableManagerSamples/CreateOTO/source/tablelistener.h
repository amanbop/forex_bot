#pragma once

//#include "c:\program files\candleworks\forexconnectapi\include\io2gtable.h"
#include "stdafx.h"

class TableListener :
    public IO2GTableListener
{
    HANDLE mEvent;
    long mRef;
    std::list<std::string> mliRequestID;
    ::threading::Mutex mMutex;
public:
    long addRef(void);
    long release();
    void onStatusChanged(O2GTableStatus);
    void onAdded(const char *,IO2GRow *);
    void onChanged(const char *,IO2GRow *);
    void onDeleted(const char *,IO2GRow *);

    void appendRequest(const char *requestID )
    {
        ::threading::Mutex::Lock l(mMutex);
        mliRequestID.push_back(requestID);
    }
    TableListener(void);
    HANDLE getEvent();
    ~TableListener(void);
};

