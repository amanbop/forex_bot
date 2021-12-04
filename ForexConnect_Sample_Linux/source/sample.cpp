// ForexConnectSample.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream> 
#include <pthread.h>
#include <math.h>

class CMyApp : IO2GSessionStatus, IO2GResponseListener 
{ 

private:

   struct cont 
   {
        pthread_mutex_t mutex;
        pthread_cond_t condition;
   };

   struct cont mSyncSessionEvent; //to sync with the callback of the IO2GSessionStatus's methods from ForexConnect lib
   struct cont mSyncResponseEvent; //to sync with the callback of the IO2GResponseListener's methods from ForexConnect lib
   
   IO2GSession* mSession; //keep the Session reference   
   long dwRef; //refernce counter to implement IAddRef
   bool bHasSessionListener;
   bool bHasResponseListener;    
   char mAccount[50];    
   
   O2GSessionStatus mSessionStatus; //current connection status   
   pthread_mutex_t csSessionStatus; //mutex to sync access to mSessionStatus    
   
   double mEURUSDBid;
   double mEURUSDAsk;
   pthread_mutex_t csOffers; //mutex to sync offers acccess with changes
   
   void setEvent(cont* cont)
   {
        pthread_mutex_lock(&cont->mutex);
        pthread_cond_signal(&cont->condition);  
        pthread_mutex_unlock(&cont->mutex); 
   }
   
   bool waitEvent(cont* cont)
   {
       timespec to; 
       to.tv_sec = time(NULL) + 5; //wait 5 sec
       to.tv_nsec = 0; 
       
       pthread_mutex_lock(&cont->mutex);
       int res = pthread_cond_timedwait(&cont->condition, &cont->mutex, &to);
       pthread_mutex_unlock(&cont->mutex);  
       return (0 == res);
   }
   
   
   void splitVariantTime(double dtSrc, unsigned short& wYear, unsigned short& wMonth, unsigned short& wDay,
                unsigned short& wHour, unsigned short& wMinute, unsigned short& wSecond, unsigned short& wMSecond)
    {
        const long minDate = -657434L;                      //!< minimal date value
        const long maxDate = 2958465L;                      //!< maximal date value
        const long monthDays[13] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};

        wMonth = wYear = wDay = wHour = wMinute = wSecond = wMSecond = 0;
        dtSrc += 1e-10;
        //The legal range does not actually span year 0 to 9999.
        if (dtSrc > maxDate || dtSrc < minDate) // about year 100 to about 9999
                    return ;

        long nDays;             // Number of days since Dec. 30, 1899
        long nDaysAbsolute;     // Number of days since 1/1/0
        //    long nSecsInDay;        // Time in seconds since midnight
        long nMinutesInDay;     // Minutes in day

        long n400Years;         // Number of 400 year increments since 1/1/0
        long n400Century;       // Century within 400 year block (0,1,2 or 3)
        long n4Years;           // Number of 4 year increments since 1/1/0
        long n4Day;             // Day within 4 year block
                                //  (0 is 1/1/yr1, 1460 is 12/31/yr4)
        long n4Yr;              // Year within 4 year block (0,1,2 or 3)
        bool bLeap4 = true;     // TRUE if 4 year block includes leap year

        double dblDate = dtSrc; // tempory serial date

        // If a valid date, then this conversion should not overflow
        nDays = (long)dblDate;

        // Round to the second
        //    dblDate += ((dtSrc > 0.0) ? halfSecond : -halfSecond);

        nDaysAbsolute = (long)dblDate + 693959L; // Add days from 1/1/0 to 12/30/1899

        dblDate = fabs(dblDate);
        long nMSecsInDay = (long)((dblDate - floor(dblDate)) * 86400000) + 1;

        //Calculate the day of week (sun=1, mon=2...)
        //  -1 because 1/1/0 is Sat.  +1 because we want 1-based

        // Leap years every 4 yrs except centuries not multiples of 400.
        n400Years = (long)(nDaysAbsolute / 146097L);

        // Set nDaysAbsolute to day within 400-year block
        nDaysAbsolute %= 146097L;

        // -1 because first century has extra day
        n400Century = (long)((nDaysAbsolute - 1) / 36524L);

        // Non-leap century
        if (n400Century != 0)
        {
                // Set nDaysAbsolute to day within century
                nDaysAbsolute = (nDaysAbsolute - 1) % 36524L;

                // +1 because 1st 4 year increment has 1460 days
                n4Years = (long)((nDaysAbsolute + 1) / 1461L);

                if (n4Years != 0)
                        n4Day = (long)((nDaysAbsolute + 1) % 1461L);
                else
                {
                        bLeap4 = false;
                        n4Day = (long)nDaysAbsolute;
                }
        }
        else
        {
                // Leap century - not special case!
                n4Years = (long)(nDaysAbsolute / 1461L);
                n4Day = (long)(nDaysAbsolute % 1461L);
        }

        if (bLeap4)
        {
                // -1 because first year has 366 days
                n4Yr = (n4Day - 1) / 365;

                if (n4Yr != 0)
                        n4Day = (n4Day - 1) % 365;
        }
        else
        {
                n4Yr = n4Day / 365;
                n4Day %= 365;
        }

        // n4Day is now 0-based day of year. Save 1-based day of year, year number
        wYear = (unsigned short)(n400Years * 400 + n400Century * 100 + n4Years * 4 + n4Yr);

        // Handle leap year: before, on, and after Feb. 29.
        if (n4Yr == 0 && bLeap4)
        {
                // Leap Year
                if (n4Day == 59)
                {
                        /* Feb. 29 */
                        wMonth = 2;
                        wDay = 29;
                        goto DoTime;
                }

                // Pretend it's not a leap year for month/day comp.
                if (n4Day >= 60)
                        --n4Day;
        }

        // Make n4DaY a 1-based day of non-leap year and compute
        //  month/day for everything but Feb. 29.
        ++n4Day;

        // Month number always >= n/32, so save some loop time */
        for (wMonth = (unsigned short)(n4Day >> 5) + 1; n4Day > monthDays[wMonth]; wMonth++);

        wDay = (int)(n4Day - monthDays[wMonth - 1]);

DoTime:
        if (nMSecsInDay == 0)
            wHour = wMinute = wSecond = wMSecond = 0;
        else
        {

            wMSecond = (unsigned short)(nMSecsInDay % 1000L);
            if (wMSecond > 0)
                wMSecond--;
            long nSecsInDay = (long)nMSecsInDay / 1000;
            wSecond = (unsigned short)(nSecsInDay % 60L);
            nMinutesInDay = nSecsInDay / 60L;
            wMinute = (int)nMinutesInDay % 60;
            wHour = (int)nMinutesInDay / 60;
        }

        return ;
    }


public:

   //ctor
   CMyApp()
   {  
       pthread_mutex_init(&mSyncSessionEvent.mutex, 0);
       pthread_mutex_init(&mSyncResponseEvent.mutex, 0);       
       
       pthread_mutex_init(&csSessionStatus, 0);       
       pthread_mutex_init(&csOffers, 0);
       
       dwRef = 1;    
       mEURUSDBid = 0;
       mEURUSDAsk = 0;       
       
       bHasSessionListener = false;
       bHasResponseListener = false;        
   }    
   //descructor
   ~CMyApp()
   {   
       pthread_mutex_destroy(&mSyncSessionEvent.mutex);
       pthread_mutex_destroy(&mSyncResponseEvent.mutex);       
       pthread_mutex_destroy(&csSessionStatus);        
       pthread_mutex_destroy(&csOffers);
   }
   //IO2GSessionStatus implementation
   void onSessionStatusChanged(O2GSessionStatus status)
   {    
        pthread_mutex_lock(&csSessionStatus);        
        mSessionStatus = status;
        pthread_mutex_unlock(&csSessionStatus);       
        switch(status)
        {            
           case Connected:
           case Disconnected:
                setEvent(&mSyncSessionEvent);               
                break;
        }   
   }    
   void onLoginFailed(const char *error)
   {
       std:: cout << "\n login error \n";
       pthread_mutex_lock(&csSessionStatus);        
       mSessionStatus = Disconnected;
       pthread_mutex_unlock(&csSessionStatus);             
       
       setEvent(&mSyncSessionEvent);
   }
   //IO2GResponseListener
   double getEURUSDAsk(){
       double v;
       pthread_mutex_lock(&csOffers);                             
       v = mEURUSDAsk;                           
       pthread_mutex_unlock(&csOffers);             
       return v;
   }
   void setEURUSDAsk(double v){       
       pthread_mutex_lock(&csOffers);                             
       mEURUSDAsk = v;                    
       pthread_mutex_unlock(&csOffers);             
   }
   double getEURUSDBid(){
       double v;
       pthread_mutex_lock(&csOffers);                             
       v = mEURUSDBid;                    
       pthread_mutex_unlock(&csOffers);                             
       return v;
   }
   void setEURUSDBid(double v){        
       pthread_mutex_lock(&csOffers);                             
       mEURUSDBid = v;                    
       pthread_mutex_unlock(&csOffers);                             
   }
   void onRequestCompleted(const char * requestId, IO2GResponse  *response = 0){   
       O2G2Ptr<IO2GResponseReaderFactory> pFactory =  mSession->getResponseReaderFactory();        
       if(response->getType() == GetOffers)
       {           
           O2G2Ptr<IO2GOffersTableResponseReader> pOffersReader = pFactory->createOffersTableReader(response);
           for(int i=0; i<pOffersReader->size();i++)
           {
               O2G2Ptr<IO2GOfferRow> offer = pOffersReader->getRow(i);
               if(strcmp(offer->getInstrument(), "EUR/USD")==0)
               {                
                   setEURUSDAsk(offer->getAsk());
                   setEURUSDBid(offer->getBid());                
                   char szBuffer[255];
                   sprintf(szBuffer, "EUR/USD: Bid=%f Ask=%f\n", offer->getBid(), offer->getAsk());
                   std::cout << szBuffer;                
               }                               
           }
           
           setEvent(&mSyncResponseEvent); //signal for waiter of response
       }
       else if (response->getType() == GetOrders)
       {        
           O2G2Ptr<IO2GOrdersTableResponseReader> pOrdersReader = pFactory->createOrdersTableReader(response);
           for(int i=0; i< pOrdersReader->size(); i++)
           {
               O2G2Ptr<IO2GOrderRow> order =  pOrdersReader->getRow(i);
               char szBuffer[255];
               unsigned short wYear, wMonth, wDay, wHour, wMinute, wSecond, wMSecond;
               splitVariantTime(order->getStatusTime(), wYear, wMonth, wDay, wHour, wMinute, wSecond, wMSecond);
               sprintf(szBuffer, "Order: ID=%s Rate=%f BuySell=%s Status=%s StatusTime=%d-%d-%d %d:%d:%d",  
                                                   order->getOrderID(),
                                                   order->getRate(),
                                                   order->getBuySell(),
                                                   order->getStatus(),
                                                   wYear,
                                                   wMonth,
                                                   wDay,
                                                   wHour,
                                                   wMinute,
                                                   wSecond
                                                   );
               std::cout << szBuffer;                                     
           }
           
           setEvent(&mSyncResponseEvent); //signal for waiter of response
       }
       else if (response->getType() == GetAccounts)
       {   
            O2G2Ptr<IO2GAccountsTableResponseReader> pAccountsReader = pFactory->createAccountsTableReader(response);
            //get the only one account for this sample
            O2G2Ptr<IO2GAccountRow> account =  pAccountsReader->getRow(0);
            strcpy(mAccount, account->getAccountID());            
            setEvent(&mSyncResponseEvent); //signal for waiter of response
        }
   }

   void onRequestFailed(const char *requestId , const char *error)
   {
        std::cout << error;
        stop();
   }
   void onTablesUpdates(IO2GResponse *tablesUpdates)
   {  
        O2G2Ptr<IO2GResponseReaderFactory> pFactory =  mSession->getResponseReaderFactory();        
        O2G2Ptr<IO2GTablesUpdatesReader> pReader = pFactory->createTablesUpdatesReader(tablesUpdates);
        for(int i=0; i<pReader->size();i++)
        {
            if(pReader->getUpdateTable(i) == Offers)
            {
                if(pReader->getUpdateType(i) == Update)
                {
                    O2G2Ptr<IO2GOfferRow> offer = pReader->getOfferRow(i);                    
                    if(strcmp(offer->getInstrument(), "EUR/USD")==0)
                    {
                        if(getEURUSDAsk() != 0 && getEURUSDBid() !=0)  //Process only after mEURUSDBid and mEURUSDAsk are initialized with values in onRequestComplete
                        {
                            setEURUSDAsk(offer->getAsk());
                            setEURUSDBid(offer->getBid());                            
                            char szBuffer[255];
                            sprintf(szBuffer, "EUR/USD: Bid=%f Ask=%f\n", offer->getBid(), offer->getAsk());
                            std::cout << szBuffer;                    
                        }
                    }                                    
                }
            }
            else if (pReader->getUpdateTable(i) == Orders)
            {
                O2G2Ptr<IO2GOrderRow> order =  pReader->getOrderRow(i);
                char szBuffer[255];
                unsigned short wYear, wMonth, wDay, wHour, wMinute, wSecond, wMSecond;
                splitVariantTime(order->getStatusTime(), wYear, wMonth, wDay, wHour, wMinute, wSecond, wMSecond);
                sprintf(szBuffer, "Order updated: ID=%s Rate=%f BuySell=%s Status=%s StatusTime=%d-%d-%d %d:%d:%d \n",  
                                                    order->getOrderID(),
                                                    order->getRate(),
                                                    order->getBuySell(),
                                                    order->getStatus(),
                                                    wYear,
                                                    wMonth,
                                                    wDay,
                                                    wHour,
                                                    wMinute,
                                                    wSecond
                                                    );
                std::cout << szBuffer;                                   
            }
       }    
   }
   //IAddRef implementation
   long addRef() 
   {
       return __sync_add_and_fetch(&dwRef, 1);
   }
   long release() 
   {
       long dwRes = __sync_add_and_fetch(&dwRef, -1);
       if (dwRes == 0)
           delete this;
       return dwRes;
   }
   O2GSessionStatus getStatus()
   {
       O2GSessionStatus status;
       pthread_mutex_lock(&csSessionStatus);                                    
       status = mSessionStatus;
       pthread_mutex_unlock(&csSessionStatus);                                    
       return status;
   }
   //create the session,  login and other preparation prepare
   bool run()
   {
        mSession = CO2GTransport::createSession(); //create the IO2GSession object
        mSession->addRef();
        mSession->subscribeSessionStatus(this); //this is implementation of IO2GSessionStatus listener
        bHasSessionListener =true; //to unsubscribe it on stop

        //please change the username and password to known demo account values
        mSession->login("D172806770001", "5201", "http://www.fxcorporate.com/Hosts.jsp", "Demo");

        //Waiting the async login result
        if(!waitEvent(&mSyncSessionEvent) && this->getStatus() != IO2GSessionStatus::Connected)
            return false;

        mSession->subscribeResponse(this);
        bHasResponseListener = true; // to unsubscribe it on stop

        O2G2Ptr<IO2GLoginRules> loginRules = mSession->getLoginRules();
        bool bLoaded = loginRules->isTableLoadedByDefault(Offers);
        if(bLoaded)
        {
            O2G2Ptr<IO2GResponse> response = loginRules->getTableRefeshResponse(Offers);
            onRequestCompleted(NULL, response); //process the offers                       
        }
        else //explicit request the Offers
        {
            O2G2Ptr<IO2GRequestFactory> factory = mSession->getRequestFactory(); //create the request factory
            O2G2Ptr<IO2GRequest> refreshOffers = factory->createRefreshTableRequest(Offers); //create the respons eobject 
            mSession->sendRequest(refreshOffers);                       
            if(!waitEvent(&mSyncResponseEvent)) //waiting the response            
                return false;
            
        }

        bLoaded = loginRules->isTableLoadedByDefault(Accounts); 
        if(bLoaded) 
        {
            O2G2Ptr<IO2GResponse> response = loginRules->getTableRefeshResponse(Accounts);            
            onRequestCompleted(NULL, response); //process the accounts
        } 
        else //explicit request the Accounts 
        {
            O2G2Ptr<IO2GRequestFactory> factory = mSession->getRequestFactory(); //create the request factory
            O2G2Ptr<IO2GRequest> refreshAccounts = factory->createRefreshTableRequest(Accounts); //create the response object 
            mSession->sendRequest(refreshAccounts); 
            if(!waitEvent(&mSyncResponseEvent)) //waiting the response            
                return false;
            
            
        }

        bLoaded = loginRules->isTableLoadedByDefault(Orders);
        if(bLoaded)
        {
            O2G2Ptr<IO2GResponse> response = loginRules->getTableRefeshResponse(Orders);            
            onRequestCompleted(NULL, response); //process the orders
        }
        else //explicit request the Accounts
        {
            O2G2Ptr<IO2GRequestFactory> factory = mSession->getRequestFactory(); //create the request factory
            O2G2Ptr<IO2GRequest> refreshOrders = factory->createRefreshTableRequestByAccount(Orders, mAccount); //create the request object 
            mSession->sendRequest(refreshOrders); 
            if(!waitEvent(&mSyncResponseEvent)) //waiting the response            
                return false;            
        }
        
       return true;        
   }
   //logout and stop
   void stop()
   {
       if(mSession != NULL) 
       {
           mSession->logout();
           std::cout << "\n logout.. \n";
           
           
           waitEvent(&mSyncSessionEvent);
                      
           if(bHasResponseListener)
           {
               mSession->unsubscribeResponse(this);
               bHasResponseListener = false;
           }
           if(bHasSessionListener)
           {
               mSession->unsubscribeSessionStatus(this);            
               bHasSessionListener = false;
           }

           pthread_mutex_lock(&csSessionStatus);                                    
           mSessionStatus = Disconnected;
           pthread_mutex_unlock(&csSessionStatus);                                               
           
                      
           mSession->release();
           
           mSession = NULL; //to avoid second time stop
           
        }   
   } 

   void createOrder(bool bIsBuy)
   {        
       O2G2Ptr<IO2GRequestFactory> factory = mSession->getRequestFactory();

       O2G2Ptr<IO2GValueMap> valuemap = factory->createValueMap();
       valuemap->setString(Command, O2G2::Commands::CreateOrder);
       valuemap->setString(OrderType, O2G2::Order::TrueMarketOpen);
       valuemap->setString(AccountID, mAccount);            // The identifier of the account the order should be placed for.
       valuemap->setString(OfferID, "1");                // The identifier of the instrument the order should be placed for.
       valuemap->setString(BuySell, bIsBuy ? O2G2::Buy : O2G2::Sell);                
       valuemap->setInt(Amount, 100000);                    // The quantity of the instrument to be bought or sold.
       valuemap->setString(CustomID, "TrueMarketOrder");    // The custom identifier of the order.

       O2G2Ptr<IO2GRequest> request = factory->createOrderRequest(valuemap);        
       if(!request)
           std::cout << "\n" << factory->getLastError();
       else
           mSession->sendRequest(request);
   }

   void createOCOOrder()
   {
       O2G2Ptr<IO2GRequestFactory> factory = mSession->getRequestFactory();
       O2G2Ptr<IO2GValueMap> mainValueMap = factory->createValueMap();
       mainValueMap->setString(Command, "CreateOCO");

       double dRate = getEURUSDAsk() + 0.0001;
       O2G2Ptr<IO2GValueMap> childValueMap1 = factory->createValueMap();
       childValueMap1->setString(Command, "CreateOrder");
       childValueMap1->setString(OrderType, "SE");
       childValueMap1->setString(OfferID, "1"); //EUR/USD
       childValueMap1->setDouble(Rate, dRate);
       childValueMap1->setString(AccountID, mAccount);
       childValueMap1->setString(BuySell, "B");
       childValueMap1->setInt(Amount, 100000);        
       mainValueMap->appendChild(childValueMap1);
       
       dRate = getEURUSDBid() - 0.0001;
       O2G2Ptr<IO2GValueMap> childValueMap2 = childValueMap1->clone(); //clone the data of the first order to reduce typing
       childValueMap2->setDouble(Rate, dRate);
       childValueMap2->setString(BuySell, "S"); 
       
       mainValueMap->appendChild(childValueMap2);
                   
       O2G2Ptr<IO2GRequest> orderRequest = factory->createOrderRequest(mainValueMap);
       if (!orderRequest)            
           std::cout << factory->getLastError();                            
       else
           mSession->sendRequest(orderRequest);
       
   }

}; 

int main(int argc, char* argv[])
{
    CMyApp* app = new CMyApp();
    if(app->run()) //waiting the end if login and init subscribtion
    {
        char c='\0';
        while(true)
        {
            std::cin >> c; 
            if(app->getStatus() == IO2GSessionStatus::Disconnected) //in case of error after async call createOrder.
                break;
           
            if(c == 'q')
            {
                app->stop();
                break;
            }
            else if(c == 'b')
                app->createOrder(true); 
            else if(c == 's')
                app->createOrder(false);
            else if (c== 'o')
                app->createOCOOrder();
        }        
    }
    std::cout << "stopped\n";
    
    app->release();

    return 0;
}

