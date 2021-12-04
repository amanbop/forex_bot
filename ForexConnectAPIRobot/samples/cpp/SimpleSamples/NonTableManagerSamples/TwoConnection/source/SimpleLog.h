#pragma once

static int nCurInstance=0;
static threading::Mutex *mOutLock = NULL;

class SimpleLog
{
 public:
    /** Constructor.*/
    SimpleLog(LPCSTR  szLog);

    /** Destructor.*/
    ~SimpleLog();

    /** Add string to log.*/
    void addStringToLog(LPCSTR string);

    /** Log string to file.*/
    void logString(LPCSTR logmessage);

    /** Dump stirng to log.*/
    void dumpLog();

 private:
    std::list<std::string> mLogStrings;
    std::ofstream mLogFile;
    threading::Mutex m_cs;
    int nInstance;
};



