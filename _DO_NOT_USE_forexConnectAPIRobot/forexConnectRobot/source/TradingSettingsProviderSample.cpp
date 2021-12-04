#include "stdafx.h"
#include "SimpleLog.h"
#include "TradingSettingsProviderSample.h"

/** Constructor.*/
TradingSettingsProviderSample::TradingSettingsProviderSample(SimpleLog *log, IO2GSession *session)
{
    mLog = log;
    mSession = session;
    mSession->addRef();
}

/** Destructor. */
TradingSettingsProviderSample::~TradingSettingsProviderSample()
{
}

/** Prints settings. */
void TradingSettingsProviderSample::printSettings()
{
    O2G2Ptr<IO2GLoginRules> loginRules = mSession->getLoginRules();
    O2G2Ptr<IO2GResponse> accountsResponse = loginRules->getTableRefreshResponse(Accounts);
    if (!accountsResponse)
        return;

    O2G2Ptr<IO2GResponse> offersResponse = loginRules->getTableRefreshResponse(Offers);
    if (!offersResponse)
        return;

    O2G2Ptr<IO2GTradingSettingsProvider> tradingSettingsProvider = loginRules->getTradingSettingsProvider();
    O2G2Ptr<IO2GResponseReaderFactory> factory = mSession->getResponseReaderFactory();
    if (!factory)
        return;
    O2G2Ptr<IO2GAccountsTableResponseReader> accountsReader = factory->createAccountsTableReader(accountsResponse);
    if (!accountsReader)
        return;

    O2G2Ptr<IO2GOffersTableResponseReader> instrumentsReader = factory->createOffersTableReader(offersResponse);
    if (!instrumentsReader)
        return;

    O2G2Ptr<IO2GAccountRow> account = accountsReader->getRow(0);
    if (!account)
        return;
    for (int i = 0; i < instrumentsReader->size(); ++i)
    {
        O2G2Ptr<IO2GOfferRow> instrumentRow = instrumentsReader->getRow(i);
        const char* instrument = instrumentRow->getInstrument();
        int condDistStopForTrade = tradingSettingsProvider->getCondDistStopForTrade(instrument);
        int condDistLimitForTrade = tradingSettingsProvider->getCondDistLimitForTrade(instrument);
        int condDistStopForEntryOrder = tradingSettingsProvider->getCondDistStopForEntryOrder(instrument);
        int condDistLimitForEntryOrder = tradingSettingsProvider->getCondDistLimitForEntryOrder(instrument);
        int condDistEntryStop = tradingSettingsProvider->getCondDistEntryStop(instrument);
        int condDistEntryLimit = tradingSettingsProvider->getCondDistEntryLimit(instrument);
        int minQuantity = tradingSettingsProvider->getMinQuantity(instrument, account);
        int maxQuantity = tradingSettingsProvider->getMaxQuantity(instrument, account);
        int baseUnitSize = tradingSettingsProvider->getBaseUnitSize(instrument, account);
        O2GMarketStatus marketStatus = tradingSettingsProvider->getMarketStatus(instrument);
        int minTrailingStepForStop = tradingSettingsProvider->getMinTrailingStepForStop();
        int minTrailingStepForLimit = tradingSettingsProvider->getMinTrailingStepForLimit();
        int minTrailingStepForEntryStop = tradingSettingsProvider->getMinTrailingStepForEntryStop();
        int minTrailingStepForEntryLimit = tradingSettingsProvider->getMinTrailingStepForEntryLimit();
        int maxTrailingStepForStop = tradingSettingsProvider->getMaxTrailingStepForStop();
        int maxTrailingStepForLimit = tradingSettingsProvider->getMaxTrailingStepForLimit();
        int maxTrailingStepForEntryStop = tradingSettingsProvider->getMaxTrailingStepForEntryStop();
        int maxTrailingStepForEntryLimit = tradingSettingsProvider->getMaxTrailingStepForEntryLimit();
        double mmr = tradingSettingsProvider->getMMR(instrument, account);
        double mmr2, emr, lmr;
        bool threeLevelMargin = tradingSettingsProvider->getMargins(instrument, account, mmr2, emr, lmr);

        char buffer[256];
        const char* marketStatusStr = "unknown";
        switch (marketStatus)
        {
        case MarketStatusOpen:
            marketStatusStr = "Market Open";
            break;
        case MarketStatusClosed:
            marketStatusStr = "Market Close";
            break;
        }
        sprintf_s(buffer, sizeof(buffer), "  [%s (%s)] Cond.Dist: ST=%d; LT=%d; SE=%d; LE=%d", instrument, marketStatusStr,
            condDistStopForTrade, condDistLimitForTrade, condDistStopForEntryOrder, condDistLimitForEntryOrder);
        mLog->logString(buffer);
        sprintf_s(buffer, sizeof(buffer), "            Cond.Dist entry stop=%d; entry limit=%d", condDistEntryStop,
            condDistEntryLimit);
        mLog->logString(buffer);
        sprintf_s(buffer, sizeof(buffer), "            Quantity: Min=%d; Max=%d. Base unit size=%d; MMR=%f", minQuantity,
            maxQuantity, baseUnitSize, mmr);
        mLog->logString(buffer);
        if (threeLevelMargin)
            sprintf_s(buffer, sizeof(buffer), "            Three level margin: MMR=%f; EMR=%f; LMR=%f", mmr2, emr, lmr);
        else
            sprintf_s(buffer, sizeof(buffer), "            Single level margin: MMR=%f; EMR=%f; LMR=%f", mmr2, emr, lmr);
        mLog->logString(buffer);
        sprintf_s(buffer, sizeof(buffer), "            Trailing stop: Stop=%d-%d; Limit=%d-%d", minTrailingStepForStop,
            maxTrailingStepForStop, minTrailingStepForLimit, maxTrailingStepForLimit);
        mLog->logString(buffer);
        sprintf_s(buffer, sizeof(buffer), "            Trailing stop: Entry Stop=%d-%d; Entry Limit=%d-%d",
            minTrailingStepForEntryStop, maxTrailingStepForEntryStop, minTrailingStepForEntryLimit, maxTrailingStepForEntryLimit);
        mLog->logString(buffer);
    }

    mLog->dumpLog();
}

