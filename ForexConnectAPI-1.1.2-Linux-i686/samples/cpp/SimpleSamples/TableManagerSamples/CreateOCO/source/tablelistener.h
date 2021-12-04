#pragma once

#include "stdafx.h"

class TableListener :
    public IO2GTableListener
{
    HANDLE mEvent;
    long mRef;
    std::vector<std::string> mVect;
    int mReqsComplete;
public:
    long addRef(void);
    long release();
    void onStatusChanged(O2GTableStatus);
    void onAdded(const char *,IO2GRow *);
    void onChanged(const char *,IO2GRow *);
    void onDeleted(const char *,IO2GRow *);

    void setRequest(const std::vector<std::string> &vect)
    {
        mReqsComplete = 0;
        mVect = vect;
    };
    TableListener(void);
    HANDLE getEvent();
    ~TableListener(void);
};

