/*
/* In this example we are trying to place buy stop entry order below current market price.
/* System will not allow placement of such order without a pre-condition. 
/* We are assuming that market will go down and this condition will be met.
/* We are creating an OTO group where primary order is an IF-THEN order (Sell, Amount = 0, 30 pips below current market price).
/* We are also creating a secondary order (stop entry buy order 15 pips below current market price). 
/* We are joining these orders together in the OTO group.
/* If condition of first order will be met, the second order will be activated.
*/

package createoto;

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
    static double mRateIf = 0.0d;
    static double mRate = 0.0d;
    
    
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
        
        // Create a session, subscribe to session listener, login, create OTO orders, logout
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
                    // Main OTO ValueMap
                    O2GValueMap mainValueMap = requestFactory.createValueMap();
                    mainValueMap.setString(O2GRequestParamsEnum.COMMAND, "CreateOTO");

                    // ValueMap for IF-THEN order
                    O2GValueMap valueMapIf = requestFactory.createValueMap();
                    valueMapIf.setString(O2GRequestParamsEnum.COMMAND, "CreateOrder");
                    valueMapIf.setString(O2GRequestParamsEnum.ORDER_TYPE, "SE");
                    valueMapIf.setString(O2GRequestParamsEnum.ACCOUNT_ID, mAccountID); 
                    valueMapIf.setString(O2GRequestParamsEnum.OFFER_ID, mOfferID); 
                    valueMapIf.setString(O2GRequestParamsEnum.BUY_SELL, "S");
                    valueMapIf.setDouble(O2GRequestParamsEnum.RATE, mRateIf);                       
                    valueMapIf.setInt(O2GRequestParamsEnum.AMOUNT, 0); 
                    mainValueMap.appendChild(valueMapIf);

                    
                    // ValueMap for stop entry order
                    O2GValueMap valueMapDown = requestFactory.createValueMap();
                    valueMapDown.setString(O2GRequestParamsEnum.COMMAND, "CreateOrder");
                    valueMapDown.setString(O2GRequestParamsEnum.ORDER_TYPE, "SE");
                    valueMapDown.setString(O2GRequestParamsEnum.ACCOUNT_ID, mAccountID); 
                    valueMapDown.setString(O2GRequestParamsEnum.OFFER_ID, mOfferID); 
                    valueMapDown.setString(O2GRequestParamsEnum.BUY_SELL, "B");
                    valueMapDown.setDouble(O2GRequestParamsEnum.RATE, mRate);                       
                    valueMapDown.setInt(O2GRequestParamsEnum.AMOUNT, mAmount);
                    mainValueMap.appendChild(valueMapDown);                 
                    
                    // Send request using Main OCO ValueMap
                    O2GRequest request = requestFactory.createOrderRequest(mainValueMap);
                    mSession.sendRequest(request);
                    Thread.sleep(1000);
                    if (!responseListener.hasError()) {
                        System.out.println("You have successfully created OTO orders for " + mInstrument + " instrument.\n");
                    } else {
                        System.out.println("Your request to create OTO orders for " + mInstrument + " instrument has failed.\n");
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
                        
                        // For the purpose of this example we will place IF-THEN order 30 pips below the current market price
                        // and our stop entry order 15 pips below the current market price
                        mRate = ask - 15 * pointSize;
                        mRateIf = ask - 30 * pointSize;
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