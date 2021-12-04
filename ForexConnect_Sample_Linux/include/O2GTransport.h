#pragma once 
class Order2Go2 CO2GTransport
{
 public: 
    /** Create transport session.*/
    static IO2GSession* createSession();
    
    /** Set proxy.
        @param proxyHost            Proxy host. 
        @param iPort                Proxy port.
        @param user                 User name.
        @param password             User password.    
    */
    static void setProxy(const char* proxyHost, int iPort, const char* user, const char* password);
};

