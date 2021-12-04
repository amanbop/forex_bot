/*
/* In this example we are trying to catch market direction.
/* The market is currently flat and we don't know if it will go up or down. 
/* We are placing a buy order above the current market price channel,
/* hoping to benefit from the future market upswing. 
/* We are placing a sell order below current market price channel, 
/* hoping to benefit from the future market downswing. 
/* We are joining these orders together in the OCO group.
*/

package createoco;

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

public class Main {
    
    //Entry order variables    
    static String mInstrument = "";
    static int mAmount = 0;
    static String mOfferID = "";
    static String mAccountID = "";
    static boolean mHasAccount = false;
    static boolean mHasOffer = false;
    static double mRateUp = 0.0d;
    static double mRateDown = 0.0d;
    
    
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
            System.out.println("USAGE: [instrument] [amount] [user ID] [password] [URL] [connection] [session ID (if needed)] [pin (if needed)]");
            System.exit(1);
        }
        
        // Get command line arguments
        mInstrument = args[0];
        if (mInstrument.equals("")||mInstrument.equals("{INSTRUMENT}")) {
            System.out.println("You must specify a valid instrument.");
            System.exit(1);
        }           
        try {
            mAmount = Integer.parseInt(args[1].trim());
        } catch (NumberFormatException e) {
            System.out.println("Amount must be a number: " + e.getMessage());
            System.exit(1);
        }
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
        
        // Create a session, subscribe to session listener, login, create OCO orders, logout
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
                getAccount(mSession);
                if (mHasAccount) {
                    getOfferRate(mSession, mInstrument);
                }
                if ((mHasAccount) && (mHasOffer)) {
                    O2GRequestFactory requestFactory = mSession.getRequestFactory();
                    // Main OCO ValueMap
                    O2GValueMap mainValueMap = requestFactory.createValueMap();
                    mainValueMap.setString(O2GRequestParamsEnum.COMMAND, "CreateOCO");

                    // ValueMap for upswing
                    O2GValueMap valueMapUp = requestFactory.createValueMap();
                    valueMapUp.setString(O2GRequestParamsEnum.COMMAND, "CreateOrder");
                    valueMapUp.setString(O2GRequestParamsEnum.ORDER_TYPE, "SE");
                    valueMapUp.setString(O2GRequestParamsEnum.ACCOUNT_ID, mAccountID); 
                    valueMapUp.setString(O2GRequestParamsEnum.OFFER_ID, mOfferID); 
                    valueMapUp.setString(O2GRequestParamsEnum.BUY_SELL, "B");
                    valueMapUp.setDouble(O2GRequestParamsEnum.RATE, mRateUp);                       
                    valueMapUp.setInt(O2GRequestParamsEnum.AMOUNT, mAmount); 
                    mainValueMap.appendChild(valueMapUp);

                    
                    // ValueMap for downswing
                    O2GValueMap valueMapDown = requestFactory.createValueMap();
                    valueMapDown.setString(O2GRequestParamsEnum.COMMAND, "CreateOrder");
                    valueMapDown.setString(O2GRequestParamsEnum.ORDER_TYPE, "SE");
                    valueMapDown.setString(O2GRequestParamsEnum.ACCOUNT_ID, mAccountID); 
                    valueMapDown.setString(O2GRequestParamsEnum.OFFER_ID, mOfferID); 
                    valueMapDown.setString(O2GRequestParamsEnum.BUY_SELL, "S");
                    valueMapDown.setDouble(O2GRequestParamsEnum.RATE, mRateDown);                       
                    valueMapDown.setInt(O2GRequestParamsEnum.AMOUNT, mAmount);
                    mainValueMap.appendChild(valueMapDown);                 
                    
                    // Send request using Main OCO ValueMap
                    O2GRequest request = requestFactory.createOrderRequest(mainValueMap);
                    mSession.sendRequest(request);
                    Thread.sleep(1000);
                    if (!responseListener.hasError()) {
                        System.out.println("You have successfully created OCO orders for " + mInstrument + " instrument.\n");
                    } else {
                        System.out.println("Your request to create OCO orders for " + mInstrument + " instrument has failed.\n");
                    }
                    
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
    
    // Get current prices and calculate order price
    public static void getOfferRate(O2GSession session, String instrument) { 
        double bid = 0.0d;
        double ask = 0.0d;
        double pointSize =0.0d;
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
                        bid = offer.getBid();
                        ask = offer.getAsk();
                        pointSize = offer.getPointSize();
                        
                        // For the purpose of this example we will place entry orders 30 pips from the current market price
                        mRateUp = ask + 30 * pointSize;
                        mRateDown = bid - 30 * pointSize;
                        mHasOffer = true;
                        break;
                    }
                } 
                if (!mHasOffer) {
                    System.out.println("You specified invalid instrument. No action will be taken.");
                }               
            }
        } catch (Exception e) {
            System.out.println("Exception in getOffer().\n\t " + e.getMessage());
        }   
    }
    // Get account for trade
    public static void getAccount(O2GSession session) { 
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
                        mAccountID = accountID; 
                        mHasAccount = true;
                        break;
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
}