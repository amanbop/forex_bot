package findintablebycolumnvalue;

import com.fxcore2.Constants;
import com.fxcore2.O2GAccountRow;
import com.fxcore2.O2GAccountTableRow;
import com.fxcore2.O2GAccountsTable;
import com.fxcore2.O2GLoginRules;
import com.fxcore2.O2GOfferRow;
import com.fxcore2.O2GOfferTableRow;
import com.fxcore2.O2GOffersTable;
import com.fxcore2.O2GSession;
import com.fxcore2.O2GTransport;
import com.fxcore2.O2GTableManagerMode;
import com.fxcore2.O2GTableManager;
import com.fxcore2.O2GTableType;
import com.fxcore2.O2GOrdersTable; 
import com.fxcore2.O2GOrderTableRow;
import com.fxcore2.O2GRequest;
import com.fxcore2.O2GRequestFactory;
import com.fxcore2.O2GRequestParamsEnum;
import com.fxcore2.O2GTableIterator;
import com.fxcore2.O2GTableManagerStatus;
import com.fxcore2.O2GTableUpdateType;
import com.fxcore2.O2GTradeTableRow;
import com.fxcore2.O2GTradesTable;
import com.fxcore2.O2GTradingSettingsProvider;
import com.fxcore2.O2GValueMap;
import java.util.Calendar;
import java.util.Formatter;

public class Main {
    //Entry order variables    
    static String mInstrument = "";
    static int mAmount = 0;
    static String mOfferID = "";
    static String mAccountID = "";
    static String mBuySell = "";
    static String mOrderType = Constants.Orders.Entry;
    static boolean mHasAccount = false;
    static boolean mHasOffer = false;
    static boolean isPriceInvalid = false;
    static double mRate = 0.0d; 
    static double mRateLimit = 0.0;
    static double mRateStop = 0.0;

    static O2GAccountRow account = null;
    static O2GSession mSession = null;
    
    public static void main(String[] args) {
    
        // Connection variables
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
                
       // Get command line arguments        
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
             
        // Create a session, subscribe to session listener, login, find order and print order info, logout
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
                if (tableManager.getStatus() == O2GTableManagerStatus.TABLES_LOADED){   
                    O2GAccountsTable accountsTable = (O2GAccountsTable)tableManager.getTable(O2GTableType.ACCOUNTS);                    
                    getAccount(accountsTable); 
                    if (mHasAccount) {
                        O2GOffersTable offersTable = (O2GOffersTable)tableManager.getTable(O2GTableType.OFFERS);                        
                        getOffer(offersTable);
                    }
                    if (!(isPriceInvalid) && (mHasOffer)) {
                        O2GRequestFactory requestFactory = mSession.getRequestFactory();
                        if (requestFactory != null) {
                            O2GValueMap valueMap = requestFactory.createValueMap();
                            valueMap.setString(O2GRequestParamsEnum.COMMAND, "CreateOrder");
                            valueMap.setString(O2GRequestParamsEnum.ORDER_TYPE, mOrderType);
                            valueMap.setString(O2GRequestParamsEnum.ACCOUNT_ID, mAccountID); 
                            valueMap.setString(O2GRequestParamsEnum.OFFER_ID, mOfferID); 
                            valueMap.setString(O2GRequestParamsEnum.BUY_SELL, mBuySell);
                            valueMap.setDouble(O2GRequestParamsEnum.RATE, mRate);                       
                            valueMap.setDouble(O2GRequestParamsEnum.RATE_STOP, mRateStop);
                            valueMap.setDouble(O2GRequestParamsEnum.RATE_LIMIT, mRateLimit);
                            valueMap.setInt(O2GRequestParamsEnum.AMOUNT, mAmount); 
                            valueMap.setString(O2GRequestParamsEnum.CUSTOM_ID, "EntryOrder"); 
                            O2GRequest request = requestFactory.createOrderRequest(valueMap);
                            
                            String sRequestID = request.getRequestId();
                            responseListener.setRequest(sRequestID);                            
                            
                            O2GOrdersTable ordersTable = (O2GOrdersTable)tableManager.getTable(O2GTableType.ORDERS);                                                        
                            TableListener ordersListener = new TableListener(responseListener.getFailSyncObj());                             
                            ordersTable.subscribeUpdate(O2GTableUpdateType.INSERT, ordersListener);
                            ordersListener.setRequestID(sRequestID);
                            
                            mSession.sendRequest(request);                           
                            
                            ordersListener.waitInsertion();
                            
                            if (!responseListener.hasError()) {                                                                
                                O2GTableIterator ordersIterator = new O2GTableIterator();
                                O2GOrderTableRow order = ordersTable.getNextRowByColumnValue("RequestID", sRequestID, ordersIterator);                                                                                                
                                while(order!= null){
                                    System.out.println("Order: " +
                                        "OrderID = " + order.getOrderID() + "\t" +
                                        "OfferID = " +  order.getOfferID() + "\t" +
                                        "Type = " + order.getType() + "\t" +
                                        "Rate = " + order.getRate() + "\t" +
                                        "Buy/Sell = " + order.getBuySell() + "\t" +
                                        "Status = " + order.getStatus() + "\t" +
                                        "Limit = " + order.getLimit() + "\t" +  
                                        "Stop = " + order.getStop() + "\t" +
                                        "RequestID = " + order.getRequestID());     

                                    order = ordersTable.getNextRowByColumnValue("RequestID", sRequestID, ordersIterator);                                
                                }                                
                            } else {
                                System.out.println("Your request to create an entry order for " + mInstrument + " instrument has failed.\n");
                            }
                            
                            ordersTable.unsubscribeUpdate(O2GTableUpdateType.INSERT, ordersListener);
                        }
                    }                    
                }
                else
                {
                    System.out.println("\nTables refreshing failed.");
                }                    
                mSession.logout();
                while (!statusListener.isDisconnected()) {
                    Thread.sleep(50);
                }
            }
            mSession.unsubscribeResponse(responseListener);
            mSession.unsubscribeSessionStatus(statusListener);
            mSession.dispose();
            System.exit(1);
        } catch (Exception e) {
            System.out.println ("Exception: " + e.getMessage());
            System.exit(1);
        }
    }
       
    // Get current prices and find order type
    public static void getOffer(O2GOffersTable table) {
        double bid = 0.0d;
        double ask = 0.0d;
        double pointSize =0.0d;        
        O2GTableIterator offersIterator = new O2GTableIterator();
        O2GOfferTableRow offer = table.getNextRowByColumnValue("Instrument", mInstrument, offersIterator);
        if(offer!= null) {                        
            mOfferID = offer.getOfferID();
            mHasOffer = true;
            bid = offer.getBid();
            ask = offer.getAsk();
            pointSize = offer.getPointSize();

            if (mBuySell.equals("B")){
               mRate = ask - (20 * pointSize);
               mRateLimit = ask + (20 * pointSize);
               mRateStop = ask - (40 * pointSize);
            }
            else{
               mRate = bid + (20 * pointSize);
               mRateLimit = bid - (20 * pointSize);
               mRateStop = bid + (40 * pointSize);
            }            
        }
        else{                    
            mHasOffer = false;
            System.out.println("You specified invalid instrument. No action will be taken.");
        }                   
    }
    
    // Get account for trade
    public static void getAccount(O2GAccountsTable table) {
        String accountID = "";
        String accountKind = "";
        for (int i = 0; i < table.size(); i++) {
            O2GAccountTableRow account = table.getRow(i);
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
    
}