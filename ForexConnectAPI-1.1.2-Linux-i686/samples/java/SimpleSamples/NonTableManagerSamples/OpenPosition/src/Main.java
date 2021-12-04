package openposition;

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
    
    //Open position variables    
    static String mInstrument = "";
    static int mAmount = 0;
    static String mOfferID = "";
    static String mAccountID = "";
    static String mBuySell = "";
    
    static String mOfferUnsubscribe = "";
    static boolean mHasAccount = false;
    static boolean mHasOffer = false;
    
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
            System.out.println("USAGE: [instrument] [amount] [buysell] [user ID] [password] [URL] [connection] [session ID (if needed)] [pin (if needed)]");
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
        if (args.length > 7) {
            mDBName = args[7];
        }
        if (args.length > 8) {
            mPin = args[8];
        }
        
        // Create a session, subscribe to session listener, login, open position, logout
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
                    getOffer(mSession, mInstrument);
                }
                if ((mHasAccount) && (mHasOffer)) {
                    O2GRequestFactory requestFactory = mSession.getRequestFactory();
                    if (requestFactory != null) {
                        O2GValueMap valueMap = requestFactory.createValueMap();
                        valueMap.setString(O2GRequestParamsEnum.COMMAND, "CreateOrder");
                        valueMap.setString(O2GRequestParamsEnum.ORDER_TYPE, "OM");
                        valueMap.setString(O2GRequestParamsEnum.ACCOUNT_ID, mAccountID); 
                        valueMap.setString(O2GRequestParamsEnum.OFFER_ID, mOfferID); 
                        valueMap.setString(O2GRequestParamsEnum.BUY_SELL, mBuySell); 
                        valueMap.setInt(O2GRequestParamsEnum.AMOUNT, mAmount); 
                        valueMap.setString(O2GRequestParamsEnum.CUSTOM_ID, "TrueMarketOrder"); 
                        O2GRequest request = requestFactory.createOrderRequest(valueMap);
                        mSession.sendRequest(request);
                        Thread.sleep(1000);
                        if (!responseListener.hasError()) {
                            System.out.println("You have successfully opened a position for " + mInstrument + " instrument.\n");
                        } else {
                            System.out.println("Your request to open a position for " + mInstrument + " instrument has failed.\n");
                        }
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
                    System.out.println("You don't have any accounts available for trading. No action will be taken");
               }                        
            }
        } catch (Exception e) {
            System.out.println("Exception in getAccounts():\n\t " + e.getMessage());
        }   
    }
}