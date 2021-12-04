#include "stdafx.h"
#include "sessionstatus.h"

std::string ParseO2GPermissionStatus(O2GPermissionStatus status);

int main(int argc, char* argv[])
{
    std::string instrument = "";
    std::string user = "";
    std::string password = "";
    std::string url = "";
    std::string connection = "";
    std::string subsession = "";
    std::string pin = "";

    if (argc < 4)
    {
        std::cout << "usage: CheckPermissions.exe instrument user password url connection [subsession] [pin]" << std::endl;
        return -1;
    }

    instrument = argv[1];
    user = argv[2];
    password = argv[3];
    if (argc >= 5)
        url = argv[4];
    else
        url = "http://www.fxcorporate.com/Hosts.jsp";
    if (argc >= 6)
        connection = argv[5];
    else
        connection = "Demo";
    if (argc >= 7)
        subsession = argv[6];
    if (argc >= 8)
        pin = argv[7];

    IO2GSession *session = CO2GTransport::createSession();
    SessionStatusListener *listener = new SessionStatusListener(session, true, subsession.c_str(), pin.c_str());
    session->subscribeSessionStatus(listener);
    listener->resetError();
    std::cout << "connecting..." << std::endl;
    session->login(user.c_str(), password.c_str(), url.c_str(), connection.c_str());

    bool connected = false;

    while (true)
    {
        uni::WaitForSingleObject(listener->getEvent(), 250);
        if (listener->getStatusCode() == IO2GSessionStatus::Connected)
        {
            connected = true;
            break;
        }
        if (listener->wasError())
        {
            std::cout << "Connection failed:" << listener->getError() << std::endl;
            break;
        }
    }

    if (connected)
    {
        std::cout << "connected" << std::endl;

        std::cout << "Instrument: " << instrument << std::endl;

        IO2GLoginRules *loginRules = session->getLoginRules();
        bool bInstrument = false;
        IO2GResponse *response = loginRules->getTableRefreshResponse(Offers);
        if (response)
        {
            IO2GResponseReaderFactory *readerFactory = session->getResponseReaderFactory();
            IO2GOffersTableResponseReader *reader = readerFactory->createOffersTableReader(response);
            response->release();
            if (reader)
            {
                for (int i = 0; i < reader->size(); ++i)
                {
                    IO2GOfferRow *row = reader->getRow(i);
                    if (instrument == row->getInstrument())
                        bInstrument = true;
                    row->release();
                    if (bInstrument)
                        break;
                }
                reader->release();
            }
            readerFactory->release();
        }

        if (bInstrument)
        {
            IO2GPermissionChecker *permissionChecker = loginRules->getPermissionChecker();

            std::cout << "canCreateMarketOpenOrder = "  << ParseO2GPermissionStatus(permissionChecker->canCreateMarketOpenOrder(instrument.c_str())) << std::endl;
            std::cout << "canChangeMarketOpenOrder = "  << ParseO2GPermissionStatus(permissionChecker->canChangeMarketOpenOrder(instrument.c_str())) << std::endl;
            std::cout << "canDeleteMarketOpenOrder = "  << ParseO2GPermissionStatus(permissionChecker->canDeleteMarketOpenOrder(instrument.c_str())) << std::endl;
            std::cout << "canCreateMarketCloseOrder = "  << ParseO2GPermissionStatus(permissionChecker->canCreateMarketCloseOrder(instrument.c_str())) << std::endl;
            std::cout << "canChangeMarketCloseOrder = "  << ParseO2GPermissionStatus(permissionChecker->canChangeMarketCloseOrder(instrument.c_str())) << std::endl;
            std::cout << "canDeleteMarketCloseOrder = "  << ParseO2GPermissionStatus(permissionChecker->canDeleteMarketCloseOrder(instrument.c_str())) << std::endl;
            std::cout << "canCreateEntryOrder = "  << ParseO2GPermissionStatus(permissionChecker->canCreateEntryOrder(instrument.c_str())) << std::endl;
            std::cout << "canChangeEntryOrder = "  << ParseO2GPermissionStatus(permissionChecker->canChangeEntryOrder(instrument.c_str())) << std::endl;
            std::cout << "canDeleteEntryOrder = "  << ParseO2GPermissionStatus(permissionChecker->canDeleteEntryOrder(instrument.c_str())) << std::endl;
            std::cout << "canCreateStopLimitOrder = "  << ParseO2GPermissionStatus(permissionChecker->canCreateStopLimitOrder(instrument.c_str())) << std::endl;
            std::cout << "canChangeStopLimitOrder = "  << ParseO2GPermissionStatus(permissionChecker->canChangeStopLimitOrder(instrument.c_str())) << std::endl;
            std::cout << "canDeleteStopLimitOrder = "  << ParseO2GPermissionStatus(permissionChecker->canDeleteStopLimitOrder(instrument.c_str())) << std::endl;
            std::cout << "canRequestQuote = "  << ParseO2GPermissionStatus(permissionChecker->canRequestQuote(instrument.c_str())) << std::endl;
            std::cout << "canAcceptQuote = "  << ParseO2GPermissionStatus(permissionChecker->canAcceptQuote(instrument.c_str())) << std::endl;
            std::cout << "canDeleteQuote = "  << ParseO2GPermissionStatus(permissionChecker->canDeleteQuote(instrument.c_str())) << std::endl;
            std::cout << "canJoinToNewContingencyGroup = "  << ParseO2GPermissionStatus(permissionChecker->canJoinToNewContingencyGroup(instrument.c_str())) << std::endl;
            std::cout << "canJoinToExistingContingencyGroup = "  << ParseO2GPermissionStatus(permissionChecker->canJoinToExistingContingencyGroup(instrument.c_str())) << std::endl;
            std::cout << "canRemoveFromContingencyGroup = "  << ParseO2GPermissionStatus(permissionChecker->canRemoveFromContingencyGroup(instrument.c_str())) << std::endl;
            std::cout << "canChangeOfferSubscription = "  << ParseO2GPermissionStatus(permissionChecker->canChangeOfferSubscription(instrument.c_str())) << std::endl;
            std::cout << "canCreateNetCloseOrder = "  << ParseO2GPermissionStatus(permissionChecker->canCreateNetCloseOrder(instrument.c_str())) << std::endl;
            std::cout << "canChangeNetCloseOrder = "  << ParseO2GPermissionStatus(permissionChecker->canChangeNetCloseOrder(instrument.c_str())) << std::endl;

            permissionChecker->release();
        }
        else
            std::cout << "Instrument " << instrument << " not found" << std::endl;

        loginRules->release();

        session->logout();
        std::cout << "disconnecting..." << std::endl;
        while (true)
        {
            uni::WaitForSingleObject(listener->getEvent(), 250);
            if (listener->getStatusCode() == IO2GSessionStatus::Disconnected)
                break;
        }
        std::cout << "disconnected" << std::endl;
    }

    session->unsubscribeSessionStatus(listener);
    listener->release();
    session->release();
    if(!connected)
        return -1;
    return 0;
}

std::string ParseO2GPermissionStatus(O2GPermissionStatus status)
{
    switch(status)
    {
    case PermissionDisabled:
        return std::string("Permission Disabled");
    case PermissionEnabled:
        return std::string("Permission Enabled");
    case PermissionHidden:
        return std::string("Permission Hidden");
    default:
        return std::string("Unknown Permission Status");
    }
    return std::string("");
}

