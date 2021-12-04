package createentryorder;

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
    static String mBuySell = "";
    static String mOrderType = "";
    static boolean mHasAccount = false;
    static boolean mHasOffer = false;
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
        if (args.length < 8) {
            System.out.println("Not Enough Parameters!");
            System.out.println("USAGE: [instrument] [amount] [buysell] [order_type] [user ID] [password] [URL] [connection] [session ID (if needed)] [pin (if needed)]");
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
        mOrderType = args[3];
        if (!mOrderType.equals("LE") && !mOrderType.equals("SE")) {
            System.out.println("Please specify a valid order type. LE or SE");
            System.exit(1);
        }
        mUserID = args[4];
        mPassword = args[5];
        mURL = args[6];
        mConnection = args[7];
        if (args.length > 8) {
            mDBName = args[8];
        }
        if (args.length > 9) {
            mPin = args[9];
        }
        
        // Create a session, subscribe to session listener, login, create entry limit order, logout
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
                    if (requestFactory != null) {
                        O2GValueMap valueMap = requestFactory.createValueMap();
                        valueMap.setString(O2GRequestParamsEnum.COMMAND, "CreateOrder");
                        valueMap.setString(O2GRequestParamsEnum.ORDER_TYPE, mOrderType);
                        valueMap.setString(O2GRequestParamsEnum.ACCOUNT_ID, mAccountID); 
                        valueMap.setString(O2GRequestParamsEnum.OFFER_ID, mOfferID); 
                        valueMap.setString(O2GRequestParamsEnum.BUY_SELL, mBuySell);
                        valueMap.setDouble(O2GRequestParamsEnum.RATE, mRate);                       
                        valueMap.setInt(O2GRequestParamsEnum.AMOUNT, mAmount); 
                        valueMap.setString(O2GRequestParamsEnum.CUSTOM_ID, "LimitEntryOrder"); 
                        O2GRequest request = requestFactory.createOrderRequest(valueMap);
                        mSession.sendRequest(request);
                        Thread.sleep(1000);
                        if (!responseListener.hasError()) {
                            System.out.println("You have successfully created an entry order for " + mInstrument + " instrument.\n");
                        } else {
                            System.out.println("Your request to create an entry order for " + mInstrument + " instrument has failed.\n");
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
                        
                        // For the purpose of this example we will place entry order 10 pips from the current market price.
                        if (mOrderType.equals("LE")) {
                            if (mBuySell.equals("B")) {
                                mRate = ask - 10 * pointSize;
                            } else {
                                mRate = bid + 10 * pointSize;
                            }
                        } else {
                            if (mBuySell.equals("B")) {
                                mRate = ask + 10 * pointSize;
                            } else {
                                mRate = bid - 10 * pointSize;
                            }
                        }
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