/*
/* In this example we are joining an entry order  
/* into existing OCO group.
*/

package joinexistinggroup;

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
    static String mOrderID = "";
    static String mContingencyID = "";
    static boolean mHasAccount = false;
    static boolean mHasOrder = false;
      
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
            System.out.println("USAGE: [contingency] [order] [account] [user ID] [password] [URL] [connection] [session ID (if needed)] [pin (if needed)]");
            System.exit(1);
        }
        
        // Get command line arguments
        mContingencyID = args[0];
        mOrderID = args[1];
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
                   findOrder(mSession, responseListener);
                }
                if (mHasOrder) {              
                    O2GRequestFactory requestFactory = mSession.getRequestFactory();
                    
                    // Main JoinToExistingContingencyGroup ValueMap
                    O2GValueMap mainValueMap = requestFactory.createValueMap();
                    mainValueMap.setString(O2GRequestParamsEnum.COMMAND, "JoinToExistingContingencyGroup");
                    mainValueMap.setString(O2GRequestParamsEnum.CONTINGENCY_ID, mContingencyID);
                    mainValueMap.setInt(O2GRequestParamsEnum.CONTINGENCY_GROUP_TYPE, 1);

                    // ValueMap for an order to join the group
                    O2GValueMap valueMap = requestFactory.createValueMap();
                    valueMap.setString(O2GRequestParamsEnum.ORDER_ID, mOrderID);
                    valueMap.setString(O2GRequestParamsEnum.ACCOUNT_ID, mAccountID);
                    mainValueMap.appendChild(valueMap);         
                    
                    // Send request using Main ValueMap
                    O2GRequest request = requestFactory.createOrderRequest(mainValueMap);
                    mSession.sendRequest(request);
                    Thread.sleep(1000);
                    if (!responseListener.hasError()) {
                        System.out.println("You have successfully  joined order : " + 
                                           mOrderID +  " into existing group.\n");
                    } else {
                        System.out.println("Your request to join order " + mOrderID +  
                                           " into existing group has failed.\n");
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
    // Check if order is valid
    public static void findOrder(O2GSession session, ResponseListener listener) {     
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
                if (mOrderID.equals(order.getOrderID())) {
                    mHasOrder = true;
                    break;                    
                }                                                         
            }                   
            if (!mHasOrder) {
                System.out.println("\nYou specified an order which does not exist. No action will be taken.");
            }   
        } catch (Exception e) {
            System.out.println("Exception in findOrders():\n\t " + e.getMessage());
        }
    }       
}