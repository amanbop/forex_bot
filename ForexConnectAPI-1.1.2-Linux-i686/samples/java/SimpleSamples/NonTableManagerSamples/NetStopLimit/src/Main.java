package netstoplimit;

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
import com.fxcore2.O2GAccountsTableResponseReader;
import com.fxcore2.O2GAccountRow;
import com.fxcore2.O2GTradesTableResponseReader;
import com.fxcore2.O2GTradeRow;

public class Main {
    
    //Net Stop Limit variables    
    static String mInstrument = "";
    static double mRateStop = 0.0d;
    static double mRateLimit = 0.0d;
    static String mOfferID = "";
    static String mAccountID = "";
    static String mBuySell = "";   
    static boolean mHasAccount = false;
    static boolean mHasOffer = false;
    static boolean mHasTrade = false;
    
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
        if (args.length < 7) {
            System.out.println("Not Enough Parameters!");
            System.out.println("USAGE: [instrument] [rateS] [rateL] [user ID] [password] [URL] [connection] [session ID (if needed)] [pin (if needed)]");
            System.exit(1);
        }
        
        // Get command line arguments
        mInstrument = args[0];
        try {
            mRateStop = Double.parseDouble(args[1].trim());
        } catch (NumberFormatException e) {
            System.out.println("Stop Rate must be a number: " + e.getMessage());
            System.exit(1);
        }
        try {
            mRateLimit = Double.parseDouble(args[2].trim());
        } catch (NumberFormatException e) {
            System.out.println("Limit Rate must be a number: " + e.getMessage());
            System.exit(1);
        }
        mUserID = args[3];
        mPassword = args[4];
        mURL = args[5];
        mConnection = args[6];
        if (args.length > 7) {
            mDBName = args[7];
        }
        if (args.length > 8) {
            mPin = args[8];
        }
        
        // Create a session, subscribe to session listener, login, create net stop/limit for an instrument, logout
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
                getOffer(mSession, mInstrument);              
                if (mHasOffer) {
                    getAccount(mSession, responseListener);
                }
                if (mHasAccount) {  
                    placeOrder(mSession, "LE", mRateLimit, responseListener);
                    placeOrder(mSession, "SE", mRateStop, responseListener);
                }            
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

    
    // Get mOfferID
    public static void getOffer(O2GSession session, String instrument) { 
        try {
            O2GLoginRules loginRules = session.getLoginRules();
            if (loginRules != null && loginRules.isTableLoadedByDefault(O2GTableType.OFFERS)) {
                O2GResponse offersResponse = loginRules.getTableRefreshResponse(O2GTableType.OFFERS);
                O2GResponseReaderFactory responseFactory = session.getResponseReaderFactory();
                O2GOffersTableResponseReader offersReader = responseFactory.createOffersTableReader(offersResponse);
                for (int i = 0; i < offersReader.size(); i++) {
                    O2GOfferRow offer = offersReader.getRow(i);
                    if (instrument.equals(offer.getInstrument())) {
                        mOfferID = offer.getOfferID();
                        mHasOffer = true;
                        break;
                    }
                } 
                if (!mHasOffer) {
                    System.out.println("You specified invalid instrument. No action will be taken");
                }               
            }
        } catch (Exception e) {
            System.out.println("Exception in getOffer().\n\t " + e.getMessage());
        }   
    } 
   // Get a valid account
    public static void getAccount(O2GSession session, ResponseListener listener) { 
        try {
            O2GLoginRules loginRules = session.getLoginRules();
            if (loginRules != null && loginRules.isTableLoadedByDefault(O2GTableType.ACCOUNTS)) {
                String accountID = "";
                String accountKind = "";
                O2GResponse accountsResponse = loginRules.getTableRefreshResponse(O2GTableType.ACCOUNTS);
                O2GResponseReaderFactory responseFactory = session.getResponseReaderFactory();
                O2GAccountsTableResponseReader accountsReader = responseFactory.createAccountsTableReader(accountsResponse);
                for (int i = 0; i < accountsReader.size(); i++) {
                    O2GAccountRow account = accountsReader.getRow(i);
                    accountID = account.getAccountID();
                    accountKind = account.getAccountKind();
                    if (accountKind.equals("32")||accountKind.equals("36")) {
                        mHasTrade = getTrade(session, accountID, mOfferID, listener);
                        if (mHasTrade) {
                            mAccountID = accountID; 
                            mHasAccount = true;
                            break;
                        }                      
                    }
                }    
                if (!mHasAccount) {
                    System.out.println("You don't have any accounts available for trading. No action will be taken.");
                }                        
            }
        } catch (Exception e) {
            System.out.println("Exception in getAccounts():\n\t " + e.getMessage());
        }   
    } 
    
    // Check if a position in specified instrument exist for an account
    public static boolean getTrade (O2GSession session, String account, String offer, ResponseListener listener) {
        boolean hasTrade = false;
        try {
            O2GRequestFactory factory = session.getRequestFactory();
            if (factory != null) {
                session.sendRequest(factory.createRefreshTableRequestByAccount(O2GTableType.TRADES, account));
                Thread.sleep(1000);
            }
            O2GResponse tradesResponse = listener.getResponse();
            if (tradesResponse != null){
                O2GResponseReaderFactory responseFactory = session.getResponseReaderFactory();
                O2GTradesTableResponseReader tradesReader = responseFactory.createTradesTableReader(tradesResponse);
                for (int i = 0; i < tradesReader.size(); i++) {
                    O2GTradeRow trade = tradesReader.getRow(i);
                    if (offer.equals(trade.getOfferID())) {
                        hasTrade = true;
                        if (("B").equals(trade.getBuySell())) {
                            mBuySell = "S";
                        } else {
                            mBuySell = "B";
                        } 
                        break;                      
                    }
                }                                       
            }                       
            
        } catch (Exception e) {
            System.out.println("Exception in getTrade():\n\t " + e.getMessage());
        }
        return hasTrade;
    }

    // Place an order
    public static void placeOrder(O2GSession session, String orderType, double rate, ResponseListener listener) {
        try {
            O2GRequestFactory requestFactory = session.getRequestFactory();
            if (requestFactory != null) {
                O2GValueMap valueMap = requestFactory.createValueMap();
                valueMap.setString(O2GRequestParamsEnum.COMMAND, "CreateOrder");
                valueMap.setString(O2GRequestParamsEnum.ORDER_TYPE, orderType);
                valueMap.setString(O2GRequestParamsEnum.ACCOUNT_ID, mAccountID); 
                valueMap.setString(O2GRequestParamsEnum.OFFER_ID, mOfferID); 
                valueMap.setString(O2GRequestParamsEnum.BUY_SELL, mBuySell); 
                valueMap.setDouble(O2GRequestParamsEnum.RATE, rate); 
                valueMap.setString(O2GRequestParamsEnum.NET_QUANTITY, "Y");
                O2GRequest request = requestFactory.createOrderRequest(valueMap);
                session.sendRequest(request);
                Thread.sleep(1000);
                if (!listener.hasError()) {
                    System.out.println("You have successfully placed net " +  orderType + " order for " + mInstrument + " instrument.\n");
                } else {
                    System.out.println("Your request to place net " +  orderType + " order for " + mInstrument + " instrument has failed.\n");
                }
            }
        } catch (Exception e) { 
            System.out.println("Exception in placeOrder():\n\t " + e.getMessage());
        }       
    }                                 
}