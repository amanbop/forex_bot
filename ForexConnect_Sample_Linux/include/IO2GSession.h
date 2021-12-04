#pragma once

class Order2Go2 IO2GSessionDescriptor : public IAddRef
{
 protected:
    IO2GSessionDescriptor();
 public:
    /** Gets the unique identifier of the descriptor. */
    virtual const char * getID() = 0;
    /** Gets the readable name of the descriptor. */
    virtual const char *getName() = 0;
    /** Gets the description of the descriptor. */
    virtual const char *getDescription() = 0;

    virtual bool requiresPin() = 0;
};

class Order2Go2 IO2GSessionDescriptorCollection : public IAddRef
{
 protected:
    IO2GSessionDescriptorCollection();
 public:
    /** Gets number of session descriptors. */
    virtual int size() = 0;
    /** Gets the session descriptor by index.*/
    virtual IO2GSessionDescriptor *get(int index) = 0;
};

class Order2Go2 IO2GSessionStatus : public IAddRef
{
 protected:
    IO2GSessionStatus();
 public:
    typedef enum
    {
        Disconnected = 0,
        Connecting = 1,
        TradingSessionRequested = 2,
        Connected = 3,
        Reconnecting = 4,
        Disconnecting = 5,
        SessionLost = 6,
        PriceSessionReconnecting = 7
    } O2GSessionStatus;

    virtual void onSessionStatusChanged(O2GSessionStatus status) = 0;
    virtual void onLoginFailed(const char *error) = 0;
};

class Order2Go2 IO2GSession : public IAddRef
{
 protected:
    IO2GSession();
 public:
    virtual IO2GLoginRules *getLoginRules() = 0;
    /** Establishes connection with the trade server.*/
    virtual void login(const char *user, const char *pwd, const char *url, const char *connection) = 0;
    /** Closes connection with the trade server.*/
    virtual void logout() = 0;
    /* Subscribes the session status listener.*/
    virtual void subscribeSessionStatus(IO2GSessionStatus *listener) = 0;
    /* Unsubscribes the session status listener.*/
    virtual void unsubscribeSessionStatus(IO2GSessionStatus *listener) = 0;
    /** Gets the session descriptors collection.*/
    virtual IO2GSessionDescriptorCollection *getTradingSessionDescriptors() = 0;
    /** Sets the trading session identifier and pin.*/
    virtual void setTradingSession(const char *sessionId, const char *pin) = 0;
    /** Subscribes response listener.*/
    virtual void subscribeResponse(IO2GResponseListener *listener) = 0;
    /** Unsubscribes response listener.*/
    virtual void unsubscribeResponse(IO2GResponseListener *listener) = 0;
    /** Get the request factory.*/
    virtual IO2GRequestFactory * getRequestFactory() = 0 ;
    /** Gets the response factory reader.*/
    virtual IO2GResponseReaderFactory *getResponseReaderFactory() = 0;    
    /** Send the request to the trade server.*/
    virtual void sendRequest(IO2GRequest *request) = 0;    
    /** Gets time converter for converting request and markes snapshot date.*/
    virtual IO2GTimeConverter *getTimeConverter() = 0;
};
