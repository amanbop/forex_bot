package netstoplimit;

import com.fxcore2.O2GSession;
import com.fxcore2.O2GTransport;
import com.fxcore2.O2GLoginRules;
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
import com.fxcore2.O2GTableManagerStatus;
import com.fxcore2.O2GTradesTable;
import com.fxcore2.O2GTradeTableRow;

public class Main {
    
    //Net Stop Limit variables    
    static String mInstrument = "";
    static double mRateStop = 0.0d;
    static double mRateLimit = 0.0d;
    static String mOfferID = "";
    static String mAccountID = "";
    static String mBuySell = "";
    static String mLimitOrderID = "";   
    static boolean mHasAccount = false;
    static boolean mHasOffer = false;
    static double mPointSize = 0.0d;
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
        
        // Create a session, login, create net stop/limit, edit net limit, remove net limit, logout
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
                    O2GOffersTable offersTable = (O2GOffersTable)tableManager.getTable(O2GTableType.OFFERS);                    
                    getOffer(offersTable);              
                    if (mHasOffer) {   
                        O2GAccountsTable accountsTable = (O2GAccountsTable)tableManager.getTable(O2GTableType.ACCOUNTS);                    
                        getAccount(accountsTable, tableManager);
                    }
                    if (mHasAccount) {  
                        placeOrder(tableManager, "LE", mRateLimit, responseListener);
                        placeOrder(tableManager, "SE", mRateStop, responseListener);

                        //Edit and then remove net limit order
                        O2GRequestFactory requestFactory = mSession.getRequestFactory();
                        if (requestFactory != null) {
                            O2GOrdersTable ordersTable = (O2GOrdersTable)tableManager.getTable(O2GTableType.ORDERS);                        
                            TableListener tableListener = new TableListener();
                            // Edit net limit order (we will increase price by 50 pips)
                            mRateLimit = mRateLimit + 50 * mPointSize;
                            O2GValueMap changeMap = requestFactory.createValueMap();
                            changeMap.setString(O2GRequestParamsEnum.COMMAND, "EditOrder");
                            changeMap.setString(O2GRequestParamsEnum.ORDER_ID, mLimitOrderID);
                            changeMap.setString(O2GRequestParamsEnum.ACCOUNT_ID, mAccountID);
                            changeMap.setDouble(O2GRequestParamsEnum.RATE, mRateLimit);                      

                            // Send request using change ValueMap
                            O2GRequest changeRequest = requestFactory.createOrderRequest(changeMap);
                            ordersTable.subscribeUpdate(O2GTableUpdateType.UPDATE, tableListener);
                            mSession.sendRequest(changeRequest);
                            Thread.sleep(1000);
                            ordersTable.unsubscribeUpdate(O2GTableUpdateType.UPDATE, tableListener);
                            if (!responseListener.hasError()) {
                                System.out.println("You have successfully changed rate for order " + mLimitOrderID);
                            } else {
                                System.out.println("Your request to change rate for order " + mLimitOrderID +  
                                                   " has failed.\n");
                            }

                            // Remove secondary limit order
                            O2GValueMap removeMap = requestFactory.createValueMap();
                            removeMap.setString(O2GRequestParamsEnum.COMMAND, "DeleteOrder");
                            removeMap.setString(O2GRequestParamsEnum.ORDER_ID, mLimitOrderID);

                            // Send request using remove ValueMap
                            O2GRequest removeRequest = requestFactory.createOrderRequest(removeMap);
                            ordersTable.subscribeUpdate(O2GTableUpdateType.DELETE, tableListener);
                            mSession.sendRequest(removeRequest);
                            Thread.sleep(1000);
                            ordersTable.unsubscribeUpdate(O2GTableUpdateType.DELETE, tableListener);
                            if (!responseListener.hasError()) {
                                System.out.println("You have successfully deleted order " + mLimitOrderID);
                            } else {
                                System.out.println("Your request to delete order " + mLimitOrderID +  
                                                   " has failed.\n");
                           }
                        }
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
            mSession.unsubscribeResponse(responseListener);
            mSession.dispose();
            System.exit(1);
        } catch (Exception e) {
            System.out.println ("Exception: " + e.getMessage());
            System.exit(1);
        }
    }

    // Get offer information
    public static void getOffer(O2GOffersTable table) {        
        for (int i = 0; i < table.size(); i++) {
            O2GOfferRow offer = table.getRow(i);
            if (mInstrument.equals(offer.getInstrument())) {
                mOfferID = offer.getOfferID();
                mPointSize = offer.getPointSize();
                mHasOffer = true;
                break;
            }
        } 
        if (!mHasOffer) {
            System.out.println("You specified invalid instrument. No action will be taken.");
        }                   
    }
    
    // Get account for trade
    public static void getAccount(O2GAccountsTable table, O2GTableManager tableManager ) {
        String accountID = "";
        String accountKind = "";
        try {
            O2GTradesTable tradesTable = (O2GTradesTable)tableManager.getTable(O2GTableType.TRADES);
            while (tradesTable.getStatus() != O2GTableStatus.REFRESHED) {
                Thread.sleep(50);
            }   
            for (int i = 0; i < table.size(); i++) {
                if (mHasAccount) {
                    break;
                }
                O2GAccountRow account = table.getRow(i);
                accountID = account.getAccountID();
                accountKind = account.getAccountKind();
                if (accountKind.equals("32")||accountKind.equals("36")) {
                    for (int j = 0; j < tradesTable.size(); j++) {
                        O2GTradeTableRow trade = tradesTable.getRow(j);
                        if ((accountID.equals(trade.getAccountID())) && (mOfferID.equals(trade.getOfferID()))) {
                            System.out.println("Stop = " + trade.getStop() + "Limit = " + trade.getLimit());
                            mAccountID = accountID; 
                            mHasAccount = true;
                            if (("B").equals(trade.getBuySell())) {
                                mBuySell = "S";
                            } else {
                                mBuySell = "B";
                            } 
                            break;
                        }                        
                    }              
                }        
            }
            if (!mHasAccount) {
                System.out.println("You don't have any positions opened in the specified instrument. No action will be taken.");
            } 
        } catch (Exception e) { 
            System.out.println("Exception in getAccount():\n\t " + e.getMessage());
        }                   
    }
    
    // Place an order
    public static void placeOrder(O2GTableManager tableManager, String orderType, double rate, ResponseListener listener) {
        try {
            O2GRequestFactory requestFactory = mSession.getRequestFactory();
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
                O2GOrdersTable ordersTable = (O2GOrdersTable)tableManager.getTable(O2GTableType.ORDERS);
                while (ordersTable.getStatus() != O2GTableStatus.REFRESHED) {
                    Thread.sleep(50);
                }
                TableListener tableListener = new TableListener();
                tableListener.setRequest(request.getRequestId());
                ordersTable.subscribeUpdate(O2GTableUpdateType.INSERT, tableListener);
                mSession.sendRequest(request);
                Thread.sleep(1000);
                ordersTable.unsubscribeUpdate(O2GTableUpdateType.INSERT, tableListener);
                if (!listener.hasError()) {
                    System.out.println("You have successfully placed net " +  orderType + " order for " + mInstrument + " instrument.\n");
                    if (orderType.equals("LE")) {
                        mLimitOrderID = tableListener.getOrderID();
                    }
                } else {
                    System.out.println("Your request to place net " +  orderType + " order for " + mInstrument + " instrument has failed.\n");                  
                }
            }
        } catch (Exception e) { 
            System.out.println("Exception in placeOrder():\n\t " + e.getMessage());
        }       
    }                                 
}