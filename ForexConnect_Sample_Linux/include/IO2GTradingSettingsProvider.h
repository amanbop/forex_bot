#pragma once

/** Trading settings provider.*/
class IO2GTradingSettingsProvider : public IAddRef
{
 protected:
    IO2GTradingSettingsProvider(){};
 public:

    /** Gets condition distance for the stop. */
    virtual int getCondDistStopForTrade(const char* instrument) = 0;

    /** Gets condition distance for the limit. */
    virtual int getCondDistLimitForTrade(const char* instrument) = 0;

    /** Gets condition distance for the stop. */
    virtual int getCondDistStopForEntryOrder(const char* instrument) = 0;

    /** Gets condition distance for the limit. */
    virtual int getCondDistLimitForEntryOrder(const char* instrument) = 0;

    /** Gets condition distance for the entry stop. */
    virtual int getCondDistEntryStop(const char* instrument) = 0;

    /** Gets condition distance for the entry limit. */
    virtual int getCondDistEntryLimit(const char* instrument) = 0;

    /** Gets minimum quantity (in lots) for an order for specified instrument and account. */
    virtual int getMinQuantity(const char* instrument, IO2GAccountRow* account) = 0;

    /** Gets maximum quantity (in lots) for an order for specified instrument and account. */
    virtual int getMaxQuantity(const char* instrument, IO2GAccountRow* account) = 0;

    /** Gets base unit size by the account identifier and instrument identifier. */
    virtual int getBaseUnitSize(const char* instrument, IO2GAccountRow* account) = 0;

    /** Gets marker status. */
    virtual O2GMarketStatus getMarketStatus(const char* instrument) = 0;

    /** Gets minimal trailing stop step. */
    virtual int getMinTrailingStepForStop() = 0;

    /** Gets minimal trailing limit step. */
    virtual int getMinTrailingStepForLimit() = 0;

    /** Gets minimal trailing entry stop step. */
    virtual int getMinTrailingStepForEntryStop() = 0;

    /** Gets minimal trailing entry limit step. */
    virtual int getMinTrailingStepForEntryLimit() = 0;

    /** Gets maximal trailing stop step. */
    virtual int getMaxTrailingStepForStop() = 0;

    /** Gets maximal trailing limit step. */
    virtual int getMaxTrailingStepForLimit() = 0;

    /** Gets maximal trailing entry stop step. */
    virtual int getMaxTrailingStepForEntryStop() = 0;

    /** Gets maximal trailing entry limit step. */
    virtual int getMaxTrailingStepForEntryLimit() = 0;

    /** Gets the MMR for the specified instrument by account. */
    virtual double getMMR(const char* instrument, IO2GAccountRow* account) = 0;

    /** Gets the MMR/LMR/EMR for the specified instrument by account.

        @param  instrument      Instrument.
        @param  account         Account row.
        @param  mmr             [out] MMR.
        @param  emr             [out] EMR.
        @param  lmr             [out] LMR.

        @return Whether three level margin is used.
    */
    virtual bool getMargins(const char* instrument, IO2GAccountRow* account, double& mmr, double& emr, double& lmr) = 0;

};
