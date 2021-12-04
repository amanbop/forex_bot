/*
/* In this example we are joining two existing orders 
/* in the new OCO group.
*/

package joinnewgroup;

import com.fxcore2.O2GSession;
import com.fxcore2.O2GTransport;
import com.fxcore2.O2GLoginRules;
import com.fxcore2.O2GResponseReaderFactory;
import com.fxcore2.O2GResponse;
import com.fxcore2.O2GTableType;
import com.fxcore2.O2GRequestFactory;
import com.fxcore2.O2GValueMap;
import com.fxcore2.O2GRequest;
import com.fxcore2.O2GRequestParamsEnum;
import com.fxcore2.O2GOrderRow;
import com.fxcore2.O2GOrdersTableResponseReader;
import com.fxcore2.O2GAccountRow;
import com.fxcore2.O2GAccountsTableResponseReader;

public class Main {
    
    //Joining group variables    
    static String mAccountID = "";
    static String mOrderID1 = "";
    static String mOrderID2 = "";
    static boolean mHasAccount = false;
    static boolean mHaveOrders = false;
      
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
            System.out.println("USAGE: [order1] [order2] [account] [user ID] [password] [URL] [connection] [session ID (if needed)] [pin (if needed)]");
            System.exit(1);
        }
        
        // Get command line arguments
        mOrderID1 = args[0];
        mOrderID2 = args[1];
        mAccountID = args[2];
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
        
        // Create a session, subscribe to session listener, login, join orders into OCO group, logout
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
                checkAccount(mSession);
                if (mHasAccount) {
                   findOrders(mSession, responseListener);
                }
                if (mHaveOrders) {              
                    O2GRequestFactory requestFactory = mSession.getRequestFactory();
                    
                    // Main JoinToNewContingencyGroup ValueMap
                    O2GValueMap mainValueMap = requestFactory.createValueMap();
                    mainValueMap.setString(O2GRequestParamsEnum.COMMAND, "JoinToNewContingencyGroup");
                    mainValueMap.setInt(O2GRequestParamsEnum.CONTINGENCY_GROUP_TYPE, 1);

                    // ValueMap for first order
                    O2GValueMap valueMap1 = requestFactory.createValueMap();
                    valueMap1.setString(O2GRequestParamsEnum.ORDER_ID, mOrderID1);
                    valueMap1.setString(O2GRequestParamsEnum.ACCOUNT_ID, mAccountID);
                    mainValueMap.appendChild(valueMap1);
                   
                    // ValueMap for second order
                    O2GValueMap valueMap2 = requestFactory.createValueMap();
                    valueMap2.setString(O2GRequestParamsEnum.ORDER_ID, mOrderID2);
                    valueMap2.setString(O2GRequestParamsEnum.ACCOUNT_ID, mAccountID);
                    mainValueMap.appendChild(valueMap2);          
                    
                    // Send request using Main ValueMap
                    O2GRequest request = requestFactory.createOrderRequest(mainValueMap);
                    mSession.sendRequest(request);
                    Thread.sleep(1000);
                    if (!responseListener.hasError()) {
                        System.out.println("You have successfully  joined orders : " + 
                                           mOrderID1 + " and " + mOrderID2 + " into OCO group.\n");
                    } else {
                        System.out.println("Your request to join orders " + mOrderID1 +  
                                           " and " + mOrderID2 + " into OCO group has failed.\n");
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
    
    // Check if account is valid
    public static void checkAccount(O2GSession session) { 
        try {
            O2GLoginRules loginRules = session.getLoginRules();
            if (loginRules != null && loginRules.isTableLoadedByDefault(O2GTableType.ACCOUNTS)) {
                O2GResponse accountsResponse = loginRules.getTableRefreshResponse(O2GTableType.ACCOUNTS);
                O2GResponseReaderFactory responseFactory = session.getResponseReaderFactory();
                O2GAccountsTableResponseReader accountsReader = responseFactory.createAccountsTableReader(accountsResponse);
                for (int i = 0; i < accountsReader.size(); i++) {
                    O2GAccountRow account = accountsReader.getRow(i);
                    if (mAccountID.equals(account.getAccountID())) {
                        mHasAccount = true;
                        break;
                    }                   
                }
                if (!mHasAccount) {
                    System.out.println("You have specified an invalid account. No action will be taken.");
                }
            }
        } catch (Exception e) {
            System.out.println("Exception in getAccounts():\n\t " + e.getMessage());
        }   
    }
    // Check if orders are valid
    public static void findOrders(O2GSession session, ResponseListener listener) {
        boolean order1Found = false;
        boolean order2Found = false;        
        try {
            O2GRequestFactory factory = session.getRequestFactory();
            if (factory != null) {
                session.sendRequest(factory.createRefreshTableRequestByAccount(O2GTableType.ORDERS, mAccountID));
                Thread.sleep(1000);
            }
            O2GResponse ordersResponse = listener.getResponse();
            O2GResponseReaderFactory responseFactory = session.getResponseReaderFactory();
            O2GOrdersTableResponseReader ordersReader = responseFactory.createOrdersTableReader(ordersResponse);
            for (int i = 0; i < ordersReader.size(); i++) {
                O2GOrderRow order = ordersReader.getRow(i);
                if (mOrderID1.equals(order.getOrderID())) {
                    order1Found = true;                     
                }        
                if (mOrderID2.equals(order.getOrderID())) {
                    order2Found = true;                     
                }  
                if ((order1Found) && (order2Found)) {
                    mHaveOrders = true;
                    break;
                }                                                   
            }
                        
            if (!mHaveOrders) {
                System.out.println("\nYou specified an order which does not exist. No action will be taken.");
            }   
        } catch (Exception e) {
            System.out.println("Exception in findOrders():\n\t " + e.getMessage());
        }
    }       
}