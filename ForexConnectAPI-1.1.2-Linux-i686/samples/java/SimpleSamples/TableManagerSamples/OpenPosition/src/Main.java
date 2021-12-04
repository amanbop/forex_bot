package openposition;

import com.fxcore2.O2GSession;
import com.fxcore2.O2GTransport;
import com.fxcore2.O2GTableManagerMode;
import com.fxcore2.O2GTableManager;
import com.fxcore2.O2GTableType;
import com.fxcore2.O2GTableStatus;
import com.fxcore2.O2GOfferRow;
import com.fxcore2.O2GRequestFactory;
import com.fxcore2.O2GValueMap;
import com.fxcore2.O2GRequest;
import com.fxcore2.O2GRequestParamsEnum;
import com.fxcore2.O2GOffersTable;
import com.fxcore2.O2GOfferRow;
import com.fxcore2.O2GAccountRow;
import com.fxcore2.O2GAccountsTable;
import com.fxcore2.O2GAccountTableRow;
import com.fxcore2.O2GTableUpdateType;
import com.fxcore2.O2GOrdersTable;
import com.fxcore2.O2GTradesTable;
import com.fxcore2.O2GLoginRules;
import com.fxcore2.O2GTableManagerStatus;
import com.fxcore2.O2GTradingSettingsProvider;

public class Main {
    
    //Open position variables    
    static String mInstrument = "";
    static int mAmount = 0;
    static String mOfferID = "";
    static String mAccountID = "";
    static String mBuySell = "";
    static boolean mHasAccount = false;
    static boolean mHasOffer = false;
    static O2GSession mSession = null;
    
    public static void main(String[] args) {
    
        // Connection and session variables
        String mUserID = "";
        String mPassword = "";
        String mURL = "";
        String mConnection = "";
        String mDBName = "";
        String mPin = "";
        
              
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
            mSession.useTableManager(O2GTableManagerMode.YES, null);
            SessionStatusListener statusListener = new SessionStatusListener(mSession, mDBName, mPin);
            mSession.subscribeSessionStatus(statusListener);
            ResponseListener responseListener = new ResponseListener();
            mSession.subscribeResponse(responseListener);
            mSession.login(mUserID, mPassword, mURL, mConnection);
            while (!statusListener.isConnected() && !statusListener.hasError()) {
                    Thread.sleep(50);
            }
            if (!statusListener.hasError()) {
                O2GTableManager tableManager = mSession.getTableManager();                
                while (tableManager.getStatus() == O2GTableManagerStatus.TABLES_LOADING)
                    Thread.sleep(50);
                if (tableManager.getStatus() == O2GTableManagerStatus.TABLES_LOADED)
                {
                    O2GTradesTable tradesTable = (O2GTradesTable)tableManager.getTable(O2GTableType.TRADES);
                
                    O2GAccountsTable accountsTable = (O2GAccountsTable)tableManager.getTable(O2GTableType.ACCOUNTS);                
                    getAccount(accountsTable);               
                    if (mHasAccount) {                    
                        O2GOffersTable offersTable = (O2GOffersTable)tableManager.getTable(O2GTableType.OFFERS);                        
                        getOffer(offersTable);
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
                            O2GOrdersTable ordersTable = (O2GOrdersTable)tableManager.getTable(O2GTableType.ORDERS);
                            
                            TableListener tableListener = new TableListener();
                            tableListener.setRequest(request.getRequestId());
                            ordersTable.subscribeUpdate(O2GTableUpdateType.INSERT, tableListener);
                            mSession.sendRequest(request);
                            Thread.sleep(1000);
                            ordersTable.unsubscribeUpdate(O2GTableUpdateType.INSERT, tableListener);
                            if (!responseListener.hasError()) {
                                System.out.println("You have successfully opened a position for " + mInstrument + " instrument.\n");
                            } else {
                                System.out.println("Your request to open a position for " + mInstrument + " instrument has failed.\n");
                            }
                        }
                    }
                    else
                        System.out.println("Tables refreshing failed");
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
    
    // Get Offer ID
    public static void getOffer(O2GOffersTable table) {
        for (int i = 0; i < table.size(); i++) {
            O2GOfferRow offer = table.getRow(i);
            if (mInstrument.equals(offer.getInstrument())) {
                mOfferID = offer.getOfferID();
                mHasOffer = true;
                break;
            }
        } 
        if (!mHasOffer) {
            System.out.println("You specified invalid instrument. No action will be taken.");
        }                   
    }
    
    // Get Account for trade
    public static void getAccount(O2GAccountsTable table) {
        String accountID = "";
        String accountKind = "";
        double usableMargin = 0.0d;
        double MMR = 0.0d;
        int baseUnitSize = 0;
        for (int i = 0; i < table.size(); i++) {
            O2GAccountTableRow account = table.getRow(i);
            accountID = account.getAccountID();
            accountKind = account.getAccountKind();
            if (accountKind.equals("32")||accountKind.equals("36")) {
                mAccountID = accountID;
                usableMargin = account.getUsableMargin();
                O2GLoginRules loginRules = mSession.getLoginRules();
                O2GTradingSettingsProvider tradingSettingsProvider = loginRules.getTradingSettingsProvider();
                MMR = tradingSettingsProvider.getMMR(mInstrument, account);
                baseUnitSize = tradingSettingsProvider.getBaseUnitSize(mInstrument, account);
                if (usableMargin > MMR * mAmount / baseUnitSize) {          
                    mHasAccount = true;
                    break;
                }
            }        
        }
        if (!mHasAccount) {
            System.out.println("You don't have any accounts available for trading. No action will be taken.");
        }                  
    }
}