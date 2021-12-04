// ForexConnectSample.cpp : Defines the entry point for the console application.
//
 
#include "stdafx.h"
#include <iostream> 
#include <pthread.h>
#include <math.h>
 
class CMyApp : IO2GSessionStatus
{ 
 
private:
   //structure which is used for synchronize the main thread with the 'event handler' thread when necessary to wait the response event after request 
   struct cont 
   {
        pthread_mutex_t mutex;
        pthread_cond_t condition;
   };
 
   struct cont mSyncSessionEvent; //to sync with the callback of the IO2GSessionStatus's methods from ForexConnect lib
 
   IO2GSession* mSession; //keep the Session reference   
   long dwRef; //refernce counter to implement IAddRef
   bool bHasSessionListener;
 
   O2GSessionStatus mSessionStatus; //current connection status   
   pthread_mutex_t csSessionStatus; //mutex to sync access to mSessionStatus    
 
   //helper function to make a signal from a 'handlers' thread for waited code in the main thread    
   void setEvent(cont* cont)
   {
        pthread_mutex_lock(&cont->mutex);
        pthread_cond_signal(&cont->condition);  
        pthread_mutex_unlock(&cont->mutex); 
   }
 
   //helper function to wait a signal from the 'handlers' thread in the main thread
   bool waitEvent(cont* cont)
   {
       timespec to; 
       to.tv_sec = time(NULL) + 5; //wait 5 second
       to.tv_nsec = 0; 
 
       pthread_mutex_lock(&cont->mutex);
       int res = pthread_cond_timedwait(&cont->condition, &cont->mutex, &to);
       pthread_mutex_unlock(&cont->mutex);  
       return (0 == res);
   }  
 
public:
 
   //ctor
   CMyApp()
   {  
       pthread_mutex_init(&mSyncSessionEvent.mutex, 0);       
       pthread_mutex_init(&csSessionStatus, 0);              
       dwRef = 1;           
       bHasSessionListener = false;
   }    
   //descructor
   ~CMyApp()
   {   
       pthread_mutex_destroy(&mSyncSessionEvent.mutex);
       pthread_mutex_destroy(&csSessionStatus);        
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
   //create the session, login and other preparation
   bool run()
   {
        mSession = CO2GTransport::createSession(); //create the IO2GSession object
        mSession->addRef();
        mSession->subscribeSessionStatus(this); //this is implementation of IO2GSessionStatus listener
        bHasSessionListener =true; //to unsubscribe it on stop
 
        mSession->login("user", "password", "http://www.fxcorporate.com/Hosts.jsp", "Demo");
 
        //Waiting the async login result
        if(!waitEvent(&mSyncSessionEvent) && this->getStatus() != IO2GSessionStatus::Connected)
            return false;
 
        mSession->subscribeResponse(this);
        bHasResponseListener = true; // to unsubscribe it on stop
 
        //TODO
 
        return true;        
   }
   //logout and stop
   void stop()
   {
       if(mSession != NULL) 
       {
           mSession->logout();           
           waitEvent(&mSyncSessionEvent);                      
           if(bHasSessionListener)
           {
               mSession->unsubscribeSessionStatus(this);            
               bHasSessionListener = false;
           }
 
           pthread_mutex_lock(&csSessionStatus);                                    
           mSessionStatus = Disconnected;
           pthread_mutex_unlock(&csSessionStatus);                                                          
 
           mSession->release();           
           mSession = NULL; //to avoid the second time stop           
        }   
   } 
};

