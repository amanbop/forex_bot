/*
/* This example consists of three steps.
/* In the first step, we are trying to place buy stop entry order below current market price.
/* System will not allow placement of such order without a pre-condition. 
/* We are assuming that market will go down and this condition will be met.
/* We are creating an OTO group where primary order is an IF-THEN order (Sell, Amount = 0, 30 pips below current market price).
/* We are also creating a secondary order (stop entry buy order 15 pips below current market price). 
/* We are joining these orders together in the OTO group.
/* If condition of first order will be met, the second order will be activated.
/* In the second step, we are checking if an entry order exists which is
/* not part of the OTO group. If such order is found, we join it with existing
/* OTO group. 
/* Finally, in the third step, we are removing an order from the OTO group.
*/

package createoto;

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
import com.fxcore2.O2GOrderRow;
import com.fxcore2.O2GTableManagerStatus;

public class Main {
    
    //Entry order variables    
    static String mInstrument = "";
    static int mAmount = 0;
    static String mOfferID = "";
    static String mAccountID = "";
    static boolean mHasAccount = false;
    static boolean mHasOffer = false;
    static String mOrderID = "";
    static boolean mHasOrder = false;
    static double mRateIf = 0.0d;
    static double mRate = 0.0d;
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
        String mContingencyID = null;
        boolean bRemove = false;
        
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
        
        // Create a session, subscribe to session listener, login, create OTO orders, add an order to OTO, remove an order from OTO, logout
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

                        // Send request using Main OTO ValueMap
                        O2GRequest request = requestFactory.createOrderRequest(mainValueMap);
                        O2GOrdersTable ordersTable = (O2GOrdersTable)tableManager.getTable(O2GTableType.ORDERS);                        
                        TableListener tableListener = new TableListener();
                        ordersTable.subscribeUpdate(O2GTableUpdateType.INSERT, tableListener);
                        mSession.sendRequest(request);
                        Thread.sleep(1000);
                        mContingencyID = tableListener.getContingency();
                        ordersTable.unsubscribeUpdate(O2GTableUpdateType.INSERT, tableListener);
                        if (!responseListener.hasError()) {
                            System.out.println("You have successfully created OTO orders for " + mInstrument + " instrument.\n");
                        } else {
                            System.out.println("Your request to create OTO orders for " + mInstrument + " instrument has failed.\n");
                        }

                        // Check if entry order exists and add it to OTO group
                        for (int i = 0; i < ordersTable.size(); i++) {
                            O2GOrderRow order = ordersTable.getRow(i);
                            if ((order.getContingencyType() == 0) && 
                                (order.getAccountID().equals(mAccountID)) && 
                                (order.getAmount() != 0)) {
                                mHasOrder = true;
                                mOrderID = order.getOrderID();
                                break;
                            }
                        }
                        if (!mHasOrder) {
                            System.out.println("You have no entry orders to add to OTO group. No action will be taken.");
                        } else {                        

                            // JoinToExistingContingencyGroup ValueMap
                            O2GValueMap addValueMap = requestFactory.createValueMap();
                            addValueMap.setString(O2GRequestParamsEnum.COMMAND, "JoinToExistingContingencyGroup");
                            addValueMap.setString(O2GRequestParamsEnum.CONTINGENCY_ID, mContingencyID);
                            addValueMap.setInt(O2GRequestParamsEnum.CONTINGENCY_GROUP_TYPE, 2);

                            // ValueMap for an order to join the group
                            O2GValueMap valueMap = requestFactory.createValueMap();
                            valueMap.setString(O2GRequestParamsEnum.ORDER_ID, mOrderID);
                            valueMap.setString(O2GRequestParamsEnum.ACCOUNT_ID, mAccountID);
                            addValueMap.appendChild(valueMap);         

                            // Send request using Add ValueMap
                            O2GRequest addRequest = requestFactory.createOrderRequest(addValueMap);
                            ordersTable.subscribeUpdate(O2GTableUpdateType.UPDATE, tableListener);
                            mSession.sendRequest(addRequest);
                            Thread.sleep(1000);
                            ordersTable.unsubscribeUpdate(O2GTableUpdateType.UPDATE, tableListener);
                            if (!responseListener.hasError()) {
                                System.out.println("You have successfully  joined order " + 
                                                    mOrderID +  " into existing group.\n");
                                bRemove = true;
                            } else {
                                System.out.println("Your request to join order " + mOrderID +  
                                                    " into existing group has failed.\n");
                                bRemove = false;                       
                            }   
                        }

                        // Check if order exists and remove it from OTO group
                        if (bRemove) {   

                            //  RemoveFromContingencyGroup ValueMap
                            O2GValueMap removeValueMap = requestFactory.createValueMap();
                            removeValueMap.setString(O2GRequestParamsEnum.COMMAND, "RemoveFromContingencyGroup");

                            // ValueMap for an order to be removed
                            O2GValueMap removeMap = requestFactory.createValueMap();
                            removeMap.setString(O2GRequestParamsEnum.ORDER_ID, mOrderID);
                            removeMap.setString(O2GRequestParamsEnum.ACCOUNT_ID, mAccountID);
                            removeMap.setString(O2GRequestParamsEnum.COMMAND, "RemoveFromContingencyGroup");
                            removeValueMap.appendChild(removeMap);

                            // Send request using Remove ValueMap
                            O2GRequest removeRequest = requestFactory.createOrderRequest(removeValueMap);
                            ordersTable.subscribeUpdate(O2GTableUpdateType.UPDATE, tableListener);
                            mSession.sendRequest(removeRequest);
                            Thread.sleep(1000);
                            ordersTable.unsubscribeUpdate(O2GTableUpdateType.UPDATE, tableListener);
                            if (!responseListener.hasError()) {
                                System.out.println("You have successfully  removed order  " + 
                                                   mOrderID + " from OTO group.\n");
                            } else {
                                System.out.println("Your request to remove order " + mOrderID +  
                                                   " from OTO group has failed.\n");
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
                
                // For the purpose of this example we will place IF-THEN order 30 pips below the current market price
                // and our stop entry order 15 pips below the current market price
                mRate = ask - 15 * mPointSize;
                mRateIf = ask - 30 * mPointSize;
                mHasOffer = true;
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