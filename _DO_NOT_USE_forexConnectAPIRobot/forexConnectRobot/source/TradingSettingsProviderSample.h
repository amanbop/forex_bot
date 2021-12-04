#pragma once
class SimpleLog;
class TradingSettingsProviderSample
{
 public:
    /** Constructor.*/
    TradingSettingsProviderSample(SimpleLog *log, IO2GSession *session);

    /** Destructor. */
    ~TradingSettingsProviderSample();

    /** Prints settings. */
    void printSettings();
 private:

    SimpleLog *mLog;
    O2G2Ptr<IO2GSession> mSession;
};

