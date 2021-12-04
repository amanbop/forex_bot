package twoconnections;

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
import com.fxcore2.O2GPriceUpdateMode;

public class Main {
    
    //Open position variables    
    static String mInstrument = "";
    static int mAmount = 0;
    static String mBuySell = "";
    static String mOfferID = "";
    static String mAccountID = "";
    static boolean mHasOffer = false;
    
    public static void main(String[] args) {
    
        // Connection and session variables
        String mUserID = "";
        String mPassword = "";
        String mUserID2 = "";
        String mPassword2 = "";
        String mURL = "";
        String mConnection = "";
        String mDBName = "";
        String mPin = "";
        O2GSession mSession = null;
        O2GSession mSession2 = null;
        
        
        // Check for correct number of arguments
        if (args.length < 9) {
            System.out.println("Not Enough Parameters!");
            System.out.println("USAGE: [instrument] [amount] [buysell] [user ID] [password] [URL] [connection] [user ID2] [password2]");
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
        mBuySell = args[2];
        if (!mBuySell.equals("B") && !mBuySell.equals("S")) {
            System.out.println("Please specify a valid operation. [B]uy or [S]ell");
            System.exit(1);
        }
        mUserID = args[3];
        mPassword = args[4];
        mURL = args[5];
        mConnection = args[6];
        mUserID2 = args[7];
        mPassword2 = args[8];
        
        // Create sessions and calls for action
        mSession = O2GTransport.createSession();
        mSession2 = O2GTransport.createSession();
        doAction(mSession, mUserID, mPassword, mURL, mConnection, true);
        doAction(mSession2, mUserID2, mPassword2, mURL, mConnection, false);
        System.exit(1);
    }
    
    // Login, open position, logout 
    public static void doAction(O2GSession session, String userID, String password, 
                                String URL, String connection, boolean hasPriceUpdate) {
        try {    
            SessionStatusListener statusListener = new SessionStatusListener(session);
            session.subscribeSessionStatus(statusListener);
            ResponseListener responseListener = new ResponseListener();
            session.subscribeResponse(responseListener);
            session.login(userID, password, URL, connection);
            while (!statusListener.isConnected() && !statusListener.hasError()) {
                Thread.sleep(50);
            }
            if (!hasPriceUpdate) {
                session.setPriceUpdateMode(O2GPriceUpdateMode.NO_PRICE);
            }
            if (!statusListener.hasError()) {
                mAccountID = getAccount(session);
                if (!mAccountID.equals("")) {
                    mOfferID = getOffer(session, mInstrument);
                }
                if (!mOfferID.equals("")) {
                    openPosition(session, mAccountID, mOfferID, mBuySell, mAmount, responseListener);
                }            
                session.logout();
                while (!statusListener.isDisconnected()) {
                    Thread.sleep(50);
                }
            }
            session.unsubscribeSessionStatus(statusListener);
            session.unsubscribeResponse(responseListener);
            session.dispose();
        } catch (Exception e) {
            System.out.println ("Exception: " + e.getMessage());
        }
    }
    
    // Gets OfferID
    public static String getOffer(O2GSession session, String instrument) {
        boolean hasOffer = false;
        String offerFound = ""; 
        try {
            O2GLoginRules loginRules = session.getLoginRules();
            if (loginRules != null && loginRules.isTableLoadedByDefault(O2GTableType.OFFERS)) {
                O2GResponse offersResponse = loginRules.getTableRefreshResponse(O2GTableType.OFFERS);
                O2GResponseReaderFactory responseFactory = session.getResponseReaderFactory();
                O2GOffersTableResponseReader offersReader = responseFactory.createOffersTableReader(offersResponse);
                for (int i = 0; i < offersReader.size(); i++) {
                    O2GOfferRow offer = offersReader.getRow(i);
                    if (instrument.equals(offer.getInstrument())) {
                        hasOffer = true;
                        offerFound = offer.getOfferID();
                        break;
                    }
                } 
                if (!hasOffer) {
                    System.out.println("You specified invalid instrument. No action will be taken");
                }               
            }
        } catch (Exception e) {
            System.out.println("Exception in getOffer().\n\t " + e.getMessage());
        }
        return offerFound;      
    }
    // Gets account for trade
    public static String getAccount(O2GSession session) { 
        String accountID = "";
        String accountKind = "";
        boolean hasAccount = false;
        String accountFound = "";
        try {
            O2GLoginRules loginRules = session.getLoginRules();
            if (loginRules != null && loginRules.isTableLoadedByDefault(O2GTableType.ACCOUNTS)) {               
                O2GResponse accountsResponse = loginRules.getTableRefreshResponse(O2GTableType.ACCOUNTS);
                O2GResponseReaderFactory responseFactory = session.getResponseReaderFactory();
                O2GAccountsTableResponseReader accountsReader = responseFactory.createAccountsTableReader(accountsResponse);
                for (int i = 0; i < accountsReader.size(); i++) {
                    O2GAccountRow account = accountsReader.getRow(i);
                    accountID = account.getAccountID();
                    accountKind = account.getAccountKind();
                    if (accountKind.equals("32")||accountKind.equals("36")) {
                        hasAccount = true;
                        accountFound = accountID;
                        break;                      
                    }
                }    
                if (!hasAccount) {
                    System.out.println("You don't have any accounts available for trading. No action will be taken");
               }                        
            }
        } catch (Exception e) {
            System.out.println("Exception in getAccounts():\n\t " + e.getMessage());
        } 
        return  accountFound;   
    }
    
    // Opens a position
    public static void openPosition(O2GSession session, String accountID, String offerID, 
                                    String buySell, int amount, ResponseListener listener) {
        try {
            O2GRequestFactory requestFactory = session.getRequestFactory();
            if (requestFactory != null) {
                O2GValueMap valueMap = requestFactory.createValueMap();
                valueMap.setString(O2GRequestParamsEnum.COMMAND, "CreateOrder");
                valueMap.setString(O2GRequestParamsEnum.ORDER_TYPE, "OM");
                valueMap.setString(O2GRequestParamsEnum.ACCOUNT_ID, accountID); 
                valueMap.setString(O2GRequestParamsEnum.OFFER_ID, offerID); 
                valueMap.setString(O2GRequestParamsEnum.BUY_SELL, buySell); 
                valueMap.setInt(O2GRequestParamsEnum.AMOUNT, amount); 
                valueMap.setString(O2GRequestParamsEnum.CUSTOM_ID, "TrueMarketOrder"); 
                O2GRequest request = requestFactory.createOrderRequest(valueMap);
                session.sendRequest(request);
                Thread.sleep(1000);
                if (!listener.hasError()) {
                    System.out.println("You have successfully opened a position for " + mInstrument + " instrument.\n");
                } else {
                    System.out.println("Your request to open a position for " + mInstrument + " instrument has failed.\n");
                }
            }
        } catch (Exception e) {
            System.out.println("Exception in openPosition():\n\t " + e.getMessage());
        }
    }   
}