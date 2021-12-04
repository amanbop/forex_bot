package searchintable;

import com.fxcore2.O2GSession;
import com.fxcore2.O2GTransport;
import com.fxcore2.O2GResponseReaderFactory;
import com.fxcore2.O2GResponse;
import com.fxcore2.O2GTableType;
import com.fxcore2.O2GRequestFactory;
import com.fxcore2.O2GRequest;
import com.fxcore2.O2GRequestParamsEnum;
import com.fxcore2.O2GOrdersTableResponseReader;
import com.fxcore2.O2GOrderRow;
import com.fxcore2.O2GAccountsTableResponseReader;
import com.fxcore2.O2GAccountRow;
import com.fxcore2.O2GLoginRules;

public class Main {
    
    //Order variables    
    static String mOrderID = "";
    static String mAccountID = "";
    static boolean mHasOrder = false;
    static boolean mHasAccount = false;
       
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
        if (args.length < 6) {
            System.out.println("Not Enough Parameters!");
            System.out.println("USAGE: [order] [account] [user ID] [password] [URL] [connection] [session ID (if needed)] [pin (if needed)]");
            System.exit(1);
        }
        
       // Get command line arguments
        mOrderID = args[0];
        mAccountID = args[1];
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
             
        // Create a session, subscribe to session listener, login, find order and print order info, logout
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
                    O2GRequestFactory factory = mSession.getRequestFactory();
                    if (factory != null) {
                        mSession.sendRequest(factory.createRefreshTableRequestByAccount(O2GTableType.ORDERS, mAccountID));
                        Thread.sleep(1000);
                    }
                    O2GResponse ordersResponse = responseListener.getResponse();
                    O2GResponseReaderFactory responseFactory = mSession.getResponseReaderFactory();
                    O2GOrdersTableResponseReader ordersReader = responseFactory.createOrdersTableReader(ordersResponse);
                    for (int i = 0; i < ordersReader.size(); i++) {
                        O2GOrderRow order = ordersReader.getRow(i);
                        if (mOrderID.equals(order.getOrderID())) {
                            System.out.println("Information for OrderID = " + mOrderID + ":\n\t" +
                                               "Account = " + order.getAccountID() + "\n\t" +
                                               "Amount = " +  order.getAmount() + "\n\t" +
                                               "Rate = " + order.getRate() + "\n\t" +
                                               "Type = " + order.getType() + "\n\t" +
                                               "Buy/Sell = " + order.getBuySell() + "\n\t" +
                                               "Stage = " + order.getStage());     
                            mHasOrder = true;
                        } 
                    }
                    if (!mHasOrder) {
                     System.out.println("\nYou specified an order which does not exist.");
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
    // Gets accounts and checks if specified account exists 
    public static void getAccount(O2GSession session) { 
        try {
            O2GLoginRules loginRules = session.getLoginRules();
            if (loginRules != null && loginRules.isTableLoadedByDefault(O2GTableType.ACCOUNTS)) {
                String accountID = "";
                String accountKind = "";
                O2GResponse accountsResponse = loginRules.getTableRefreshResponse(O2GTableType.ACCOUNTS);
                O2GResponseReaderFactory responseFactory = session.getResponseReaderFactory();
                O2GAccountsTableResponseReader accountsReader = responseFactory.createAccountsTableReader(accountsResponse);
                System.out.println("Here is a list of accounts available for trading:\n");
                for (int i = 0; i < accountsReader.size(); i++) {
                    O2GAccountRow account = accountsReader.getRow(i);
                    accountID = account.getAccountID();
                    accountKind = account.getAccountKind();
                    if (accountKind.equals("32")||accountKind.equals("36")) {
                       System.out.println("\tAccountID = " + accountID);
                        if (mAccountID.equals(accountID)) {
                            mHasAccount = true;
                        }
                    }
                }    
                if (!mHasAccount) {
                    System.out.println("You specified wrong account. No action will be taken.");
               }                        
            }
        } catch (Exception e) {
            System.out.println("Exception in getAccount():\n\t " + e.getMessage());
        }   
    }   
}