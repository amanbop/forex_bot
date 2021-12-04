package createels;

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
import com.fxcore2.O2GTableManagerStatus;
import com.fxcore2.O2GTradesTable;

public class Main {
    
    //ELS order variables    
    static String mInstrument = "";
    static int mAmount = 0;
    static String mOfferID = "";
    static String mAccountID = "";
    static String mBuySell = "";
    static String mOrderType = "";
    static boolean mHasAccount = false;
    static boolean mHasOffer = false;
    static double mRate = 0.0d;
    static double mRateLimit = 0.0d;
    static double mRateStop = 0.0d;
    static double mPointSize = 0.0d;
    
    public static void main(String[] args) {
    
        // Connection and session variables
        String mUserID = "";
        String mPassword = "";
        String mURL = "";
        String mConnection = "";
        String mDBName = "";
        String mPin = "";
        O2GSession mSession = null;
        String mLimitOrderID = "";
        
        // Check for correct number of arguments
        if (args.length < 8) {
            System.out.println("Not Enough Parameters!");
            System.out.println("USAGE: [instrument] [amount] [buysell] [order type] [user ID] [password] [URL] [connection] [session ID (if needed)] [pin (if needed)]");
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
        
        // Create a session, subscribe to session listener, login, create ELS order, logout
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
                    O2GAccountsTable accountsTable = (O2GAccountsTable)tableManager.getTable(O2GTableType.ACCOUNTS);                    
                    getAccount(accountsTable);
                    if (mHasAccount) {
                        O2GOffersTable offersTable = (O2GOffersTable)tableManager.getTable(O2GTableType.OFFERS);                        
                        getOfferRate(offersTable);
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
                            valueMap.setString(O2GRequestParamsEnum.CUSTOM_ID, "EntryOrderWithStopLimit"); 
                            valueMap.setDouble(O2GRequestParamsEnum.RATE_LIMIT, mRateLimit); 
                            valueMap.setDouble(O2GRequestParamsEnum.RATE_STOP, mRateStop); 
                            O2GRequest request = requestFactory.createOrderRequest(valueMap);
                            O2GOrdersTable ordersTable = (O2GOrdersTable)tableManager.getTable(O2GTableType.ORDERS);                        
                            TableListener tableListener = new TableListener();
                            ordersTable.subscribeUpdate(O2GTableUpdateType.INSERT, tableListener);
                            mSession.sendRequest(request);
                            Thread.sleep(1000);
                            ordersTable.unsubscribeUpdate(O2GTableUpdateType.INSERT, tableListener);
                            if (!responseListener.hasError()) {
                                System.out.println("You have successfully created an entry order with attached stop and limit orders for " + mInstrument + " instrument.\n");
                            } else {
                                System.out.println("Your request to create an entry order with attached stop and limit orders for " + mInstrument + " instrument has failed.\n");
                            }

                            // Edit secondary limit order (we will increase price by 50 pips)
                            mLimitOrderID = tableListener.getOrderID();
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
    
    // Get current prices and calculate order price
    public static void getOfferRate(O2GOffersTable table) { 
        double bid = 0.0d;
        double ask = 0.0d;
        
        for (int i = 0; i < table.size(); i++) {
            O2GOfferRow offer = table.getRow(i);
            if (mInstrument.equals(offer.getInstrument())) {
                mOfferID = offer.getOfferID();
                mHasOffer = true;
                bid = offer.getBid();
                ask = offer.getAsk();
                mPointSize = offer.getPointSize();
                
                // For the purpose of this example we will place entry order 8 pips from the current market price
                // and attach stop and limit orders 10 pips from an entry order price
                if (mOrderType.equals("LE")) {
                    if (mBuySell.equals("B")) {
                        mRate = ask - 8 * mPointSize;
                        mRateLimit = mRate + 10 * mPointSize;
                        mRateStop = mRate - 10 * mPointSize;
                    } else {
                        mRate = bid + 8 * mPointSize;
                        mRateLimit = mRate - 10 * mPointSize;
                        mRateStop = mRate + 10 * mPointSize;
                    }
                } else {
                    if (mBuySell.equals("B")) {
                        mRate = ask + 8 * mPointSize;
                        mRateLimit = mRate + 10 * mPointSize;
                        mRateStop = mRate - 10 * mPointSize;
                    } else {
                        mRate = bid - 8 * mPointSize;
                        mRateLimit = mRate - 10 * mPointSize;
                        mRateStop = mRate + 10 * mPointSize;
                    }
                }
                break;
            }
        } 
        if (!mHasOffer) {
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