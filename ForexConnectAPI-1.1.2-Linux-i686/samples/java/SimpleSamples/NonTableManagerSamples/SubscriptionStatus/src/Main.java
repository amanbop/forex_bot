package subscriptionstatus;

import com.fxcore2.O2GSession;
import com.fxcore2.O2GTransport;
import com.fxcore2.O2GLoginRules;
import com.fxcore2.O2GResponseReaderFactory;
import com.fxcore2.O2GResponse;
import com.fxcore2.O2GOffersTableResponseReader;
import com.fxcore2.O2GTableType;
import com.fxcore2.O2GOfferRow;
import com.fxcore2.O2GRequestFactory;
import com.fxcore2.O2GValueMap;
import com.fxcore2.O2GRequest;
import com.fxcore2.O2GRequestParamsEnum;

public class Main {
    
    //Subscription variables    
    static String mSubscribe = "";
    static String mUnsubscribe = "";
    static String mOfferSubscribe = "";
    static String mOfferUnsubscribe = "";
    static boolean mHasSubscribe = false;
    static boolean mHasUnsubscribe = false;
    
    public static void main(String[] args) {
    
        // Connection and session variables
        String mUserID = "";
        String mPassword = "";
        String mURL = "";
        String mConnection = "";
        String mDBName = "";
        String mPin = "";
        O2GSession mSession = null;
        
        
        // Check for correct number of arguments
        if (args.length < 6) {
            System.out.println("Not Enough Parameters!");
            System.out.println("USAGE: [subscribe] [unsubscribe] [user ID] [password] [URL] [connection] [session ID (if needed)] [pin (if needed)]");
            System.exit(1);
        }
        
        // Get command line arguments
        mSubscribe = args[0];
        mUnsubscribe = args[1];
        mUserID = args[2];
        mPassword = args[3];
        mURL = args[4];
        mConnection = args[5];
        if (args.length > 6) {
            mDBName = args[6];
        }
        if (args.length > 7) {
            mPin = args[7];
        }
        
        // Create a session, subscribe to session listener, login, get instrument subscription status, request instrument subsribtion/unsubsription logout
        try {
            mSession = O2GTransport.createSession();
            SessionStatusListener statusListener = new SessionStatusListener(mSession, mDBName, mPin);
            mSession.subscribeSessionStatus(statusListener);
            ResponseListener responseListener = new ResponseListener();
            mSession.subscribeResponse(responseListener);
            mSession.login(mUserID, mPassword, mURL, mConnection);
            while (!statusListener.isConnected() && !statusListener.hasError()) {
                    Thread.sleep(50);
            }
            if (!statusListener.hasError()) {
                getSubscription(mSession, true);
                if (!mHasSubscribe) {
                    O2GRequestFactory requestFactory = mSession.getRequestFactory();
                    if (requestFactory != null) {
                        O2GValueMap valueMap = requestFactory.createValueMap();
                        valueMap.setString(O2GRequestParamsEnum.COMMAND, "SetSubscriptionStatus");
                        valueMap.setString(O2GRequestParamsEnum.SUBSCRIPTION_STATUS, "T"); 
                        valueMap.setString(O2GRequestParamsEnum.OFFER_ID, mOfferSubscribe);
                        O2GRequest request = requestFactory.createOrderRequest(valueMap);
                        mSession.sendRequest(request);
                        Thread.sleep(1000);
                        if (!responseListener.hasError()) {
                            System.out.println("You have successfully subscribed to " + mSubscribe + " instrument.\n");
                        } else {
                            System.out.println("Subscription to " + mSubscribe + " instrument has failed.\n");
                        }
                    }
                }
                if (!mHasUnsubscribe) {
                    O2GRequestFactory requestFactory = mSession.getRequestFactory();
                    if (requestFactory != null) {
                        O2GValueMap valueMap = requestFactory.createValueMap();
                        valueMap.setString(O2GRequestParamsEnum.COMMAND, "SetSubscriptionStatus");
                        valueMap.setString(O2GRequestParamsEnum.SUBSCRIPTION_STATUS, "D"); 
                        valueMap.setString(O2GRequestParamsEnum.OFFER_ID, mOfferUnsubscribe);
                        O2GRequest request = requestFactory.createOrderRequest(valueMap);
                        mSession.sendRequest(request);
                        Thread.sleep(1000);
                        if (!responseListener.hasError()) {
                            System.out.println("You have successfully unsubscribed from " + mUnsubscribe + " instrument.\n");
                        } else {
                            System.out.println("Unsubscription to " + mUnsubscribe + " instrument has failed.\n");
                        }
                    }
                }
                getSubscription(mSession, false);               
                mSession.logout();
                while (!statusListener.isDisconnected()) {
                    Thread.sleep(50);
                }
            }
            mSession.unsubscribeSessionStatus(statusListener);
            mSession.unsubscribeResponse(responseListener);
            mSession.dispose();
            System.exit(1);
        } catch (Exception e) {
            System.out.println ("Exception: " + e.getMessage());
            System.exit(1);
        }
    }
    
    // Get subsription information
    public static void getSubscription(O2GSession session, boolean checkstatus) { 
        try {
            O2GLoginRules loginRules = session.getLoginRules();
            if (loginRules != null && loginRules.isTableLoadedByDefault(O2GTableType.OFFERS)) {
                String status = "";
                String statusCode = "";
                String instrument = "";
                O2GResponse offersResponse = loginRules.getTableRefreshResponse(O2GTableType.OFFERS);
                O2GResponseReaderFactory responseFactory = session.getResponseReaderFactory();
                O2GOffersTableResponseReader offersReader = responseFactory.createOffersTableReader(offersResponse);
                System.out.println("\nHere is you current subsription list:\n");
                for (int i = 0; i < offersReader.size(); i++) {
                    O2GOfferRow offer = offersReader.getRow(i);
                    instrument = offer.getInstrument();
                    statusCode = offer.getSubscriptionStatus();
                    if (statusCode.equals("T")) {
                        status = "Available for [T]rade";
                    } else if (statusCode.equals("D")) {
                        status = "[D]isabled";
                    } else if (statusCode.equals("V")) {
                        status = "[V]iew only";
                    } else {
                        status = statusCode;
                    }
                    if (checkstatus) {
                        if (instrument.equals(mSubscribe)) {
                            if (!statusCode.equals("T")) {
                                mOfferSubscribe = offer.getOfferID();
                            } else {
                                mHasSubscribe = true;
                            }   
                        }
                        if (instrument.equals(mUnsubscribe)) {
                            if (!statusCode.equals("D")) {
                                mOfferUnsubscribe   = offer.getOfferID();
                            } else {
                                mHasUnsubscribe = true;
                            }   
                        }
                    }
                    System.out.println("Instrument = " + instrument +
                                       " Subscription Status = " + status);           
                }
                if (checkstatus) {
                    if (mHasSubscribe) {
                        System.out.println("\nYou requested subscription to " + mSubscribe + " instrument.\n" +
                                           "You are already subscribed to this instrument.\nNo action will be taken.\n" );
                    } else {
                        System.out.println("\nYou requested subscription to " + mSubscribe + " instrument.\n" +
                                           "Please wait while system executes your request.\n");
                    }
                    if (mHasUnsubscribe) {
                        System.out.println("\nYou requested to unsubsribe from " + mUnsubscribe + " instrument.\n" +
                                           "You are currently not subscribed to this instrument.\nNo action will be taken.\n" );
                    } else {
                        System.out.println("\nYou requested to unsubsribe from " + mUnsubscribe + " instrument.\n" +
                                           "Please wait while system executes your request.\n");
                    }
                }               
            }
        } catch (Exception e) {
            System.out.println("Exception in getSubscription().\n\t " + e.getMessage());
        }   
    }
}