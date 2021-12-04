#include "stdafx.h"
#include "SimpleLog.h"
#include "PermissionCheckerSample.h"

/** Constructor.*/
PermissionCheckerSample::PermissionCheckerSample(SimpleLog *log, IO2GSession *session)
{
    mLog = log;
    mSession = session;
    mSession->addRef();
}

/** Destructor. */
PermissionCheckerSample::~PermissionCheckerSample()
{
}

/** Prints permissions. */
void PermissionCheckerSample::printPermissions()
{
    const char* instrument = "EUR/USD";
    O2G2Ptr<IO2GLoginRules> loginRules = mSession->getLoginRules();
    O2G2Ptr<IO2GPermissionChecker> permissionChecker = loginRules->getPermissionChecker();
    print("canCreateMarketOpenOrder", permissionChecker->canCreateMarketOpenOrder(instrument));
    print("canChangeMarketOpenOrder", permissionChecker->canChangeMarketOpenOrder(instrument));
    print("canDeleteMarketOpenOrder", permissionChecker->canDeleteMarketOpenOrder(instrument));
    print("canCreateMarketCloseOrder", permissionChecker->canCreateMarketCloseOrder(instrument));
    print("canChangeMarketCloseOrder", permissionChecker->canChangeMarketCloseOrder(instrument));
    print("canDeleteMarketCloseOrder", permissionChecker->canDeleteMarketCloseOrder(instrument));
    print("canCreateEntryOrder", permissionChecker->canCreateEntryOrder(instrument));
    print("canChangeEntryOrder", permissionChecker->canChangeEntryOrder(instrument));
    print("canDeleteEntryOrder", permissionChecker->canDeleteEntryOrder(instrument));
    print("canCreateStopLimitOrder", permissionChecker->canCreateStopLimitOrder(instrument));
    print("canChangeStopLimitOrder", permissionChecker->canChangeStopLimitOrder(instrument));
    print("canDeleteStopLimitOrder", permissionChecker->canDeleteStopLimitOrder(instrument));
    print("canRequestQuote", permissionChecker->canRequestQuote(instrument));
    print("canAcceptQuote", permissionChecker->canAcceptQuote(instrument));
    print("canDeleteQuote", permissionChecker->canDeleteQuote(instrument));
    print("canCreateOCO", permissionChecker->canCreateOCO(instrument));
    print("canCreateOTO", permissionChecker->canCreateOTO(instrument));
    print("canJoinToNewContingencyGroup", permissionChecker->canJoinToNewContingencyGroup(instrument));
    print("canJoinToExistingContingencyGroup", permissionChecker->canJoinToExistingContingencyGroup(instrument));
    print("canRemoveFromContingencyGroup", permissionChecker->canRemoveFromContingencyGroup(instrument));
    print("canChangeOfferSubscription", permissionChecker->canChangeOfferSubscription(instrument));
    print("canCreateNetCloseOrder", permissionChecker->canCreateNetCloseOrder(instrument));
    print("canChangeNetCloseOrder", permissionChecker->canChangeNetCloseOrder(instrument));
    print("canDeleteNetCloseOrder", permissionChecker->canDeleteNetCloseOrder(instrument));
    print("canCreateNetStopLimitOrder", permissionChecker->canCreateNetStopLimitOrder(instrument));
    print("canChangeNetStopLimitOrder", permissionChecker->canChangeNetStopLimitOrder(instrument));
    print("canDeleteNetStopLimitOrder", permissionChecker->canDeleteNetStopLimitOrder(instrument));
    print("canUseDynamicTrailingForStop", permissionChecker->canUseDynamicTrailingForStop());
    print("canUseDynamicTrailingForLimit", permissionChecker->canUseDynamicTrailingForLimit());
    print("canUseDynamicTrailingForEntryStop", permissionChecker->canUseDynamicTrailingForEntryStop());
    print("canUseDynamicTrailingForEntryLimit", permissionChecker->canUseDynamicTrailingForEntryLimit());
    print("canUseFluctuateTrailingForStop", permissionChecker->canUseFluctuateTrailingForStop());
    print("canUseFluctuateTrailingForLimit", permissionChecker->canUseFluctuateTrailingForLimit());
    print("canUseFluctuateTrailingForEntryStop", permissionChecker->canUseFluctuateTrailingForEntryStop());
    print("canUseFluctuateTrailingForEntryLimit", permissionChecker->canUseFluctuateTrailingForEntryLimit());
    mLog->dumpLog();
}

/** Prints permission name and status. */
void PermissionCheckerSample::print(const char* permissionName, O2GPermissionStatus status)
{
    char buffer[256];
    const char* statusStr = "unknown";
    switch (status)
    {
    case PermissionDisabled:
        statusStr = "Disabled";
        break;
    case PermissionEnabled:
        statusStr = "Enabled";
        break;
    case PermissionHidden:
        statusStr = "Hidden";
        break;
    }
    sprintf_s(buffer, sizeof(buffer), "  %s = %s", permissionName, statusStr);
    mLog->logString(buffer);
}

