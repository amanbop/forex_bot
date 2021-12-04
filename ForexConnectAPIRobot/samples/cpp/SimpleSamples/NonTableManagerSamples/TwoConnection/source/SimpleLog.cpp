#include "stdafx.h"
#include "SimpleLog.h"

using threading::Mutex;

/** Constructor.*/
SimpleLog::SimpleLog(LPCSTR szLog)
{
//    m_cs.lock();
    // InitializeCriticalSection(&m_cs);
    mLogFile.open(szLog);
    nInstance = ++nCurInstance;
    if (!mOutLock)
        mOutLock = new threading::Mutex;
}

/** Destructor.*/
SimpleLog::~SimpleLog()
{
    dumpLog();
    mLogFile.close();
//    m_cs.unlock();
    //DeleteCriticalSection(&m_cs);
}


/** Log string to file.*/
void SimpleLog::logString(LPCSTR logmessage)
{
    Mutex::Lock l(mOutLock);
    std::cout << nInstance << ": " << logmessage << std::endl;
    mLogFile <<  nInstance << ": " << logmessage << std::endl;
}

void SimpleLog::addStringToLog(LPCSTR string)
{
    Mutex::Lock l(m_cs);
    //EnterCriticalSection(&m_cs);
    mLogStrings.push_back(string);
    //LeaveCriticalSection(&m_cs);

}

/** Dump stirng to log.*/
void SimpleLog::dumpLog()
{
    Mutex::Lock l(m_cs);
    //EnterCriticalSection(&m_cs);
    while (mLogStrings.size() > 0)
    {
        std::string sLog = mLogStrings.front();
        mLogStrings.pop_front();
        logString(sLog.c_str());
    }
    //LeaveCriticalSection(&m_cs);
}

