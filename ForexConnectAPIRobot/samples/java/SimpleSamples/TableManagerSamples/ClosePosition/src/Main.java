package closeposition;

import com.fxcore2.O2GSession;
import com.fxcore2.O2GTransport;
import com.fxcore2.O2GTableManagerMode;
import com.fxcore2.O2GTableManager;
import com.fxcore2.O2GTableType;
import com.fxcore2.O2GRequestFactory;
import com.fxcore2.O2GValueMap;
import com.fxcore2.O2GRequest;
import com.fxcore2.O2GRequestParamsEnum;
import com.fxcore2.O2GTableUpdateType;
import com.fxcore2.O2GOrdersTable;
import com.fxcore2.O2GTableManagerStatus;
import com.fxcore2.O2GTradeRow;
import com.fxcore2.O2GTradesTable;
import com.fxcore2.O2GTableStatus;

public class Main {
    
    //Position variables    
    static String mTradeID = "";
    static int mAmount = 0;
    static String mOfferID = "";
    static String mAccountID = "";
    static String mBuySell = "";
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
        if (args.length < 5) {
            System.out.println("Not Enough Parameters!");
            System.out.println("USAGE: [trade] [user ID] [password] [URL] [connection] [session ID (if needed)] [pin (if needed)]");
            System.exit(1);
        }
        
       // Get command line arguments
        mTradeID = args[0];
        mUserID = args[1];
        mPassword = args[2];
        mURL = args[3];
        mConnection = args[4];
        if (args.length > 5) {
            mDBName = args[5];
        }
        if (args.length > 6) {
            mPin = args[6];
        }
        
        
        // Create a session, subscribe to session listener, login, check if position exists, close position, logout
        try {
            mSession = O2GTransport.createSession();
            mSession.useTableManager(O2GTableManagerMode.YES, null);
            SessionStatusListener statusListener = new SessionStatusListener(mSession, mDBName, mPin);
            mSession.subscribeSessionStatus(statusListener);
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
                    getTradeInfo(tradesTable);  
                    if (mHasTrade) {
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
                            TableListener tableListener = new TableListener();
                            O2GOrdersTable ordersTable = (O2GOrdersTable )tableManager.getTable(O2GTableType.ORDERS);
                            tableListener.setRequest(request.getRequestId());
                            ordersTable.subscribeUpdate(O2GTableUpdateType.INSERT, tableListener);
                            tradesTable.subscribeUpdate(O2GTableUpdateType.DELETE, tableListener);
                            mSession.sendRequest(request);
                            Thread.sleep(1000);
                            ordersTable.unsubscribeUpdate(O2GTableUpdateType.INSERT, tableListener);
                            tradesTable.unsubscribeUpdate(O2GTableUpdateType.DELETE, tableListener);
                        }
                    } else {
                        System.out.println("\nYou specified an invalid position. No action will be taken.");
                    }
                }
                else
                    System.out.println("Tables refreshing failed");
                mSession.logout();
                while (!statusListener.isDisconnected()) {
                    Thread.sleep(50);
                }
            }
            mSession.unsubscribeSessionStatus(statusListener);
            mSession.dispose();
            System.exit(1);
        } catch (Exception e) {
            System.out.println ("Exception: " + e.getMessage());
            System.exit(1);
        }
    }
    // Get Trade information
    public static void getTradeInfo(O2GTradesTable table) {
        try {
            TableListener tableListener = new TableListener();
            table.subscribeStatus(tableListener);            
            System.out.println("\nThis is a list of your open positions: \n");
            for (int i = 0; i < table.size(); i++) {
                O2GTradeRow trade = table.getRow(i);
                System.out.println("TradeID = " + trade.getTradeID());
                if (mTradeID.equals(trade.getTradeID())) {
                    mOfferID = trade.getOfferID();
                    mAmount = trade.getAmount();
                    mAccountID = trade.getAccountID();
                    if (("B").equals(trade.getBuySell())) {
                        mBuySell = "S";
                    } else {
                        mBuySell = "B";
                    }   
                    mHasTrade = true;
                } 
            }   
            table.unsubscribeStatus(tableListener);
        } catch (Exception e) {
            System.out.println("Exception in getTradeInfo().\n\t " + e.getMessage());
        }                 
    }   
}