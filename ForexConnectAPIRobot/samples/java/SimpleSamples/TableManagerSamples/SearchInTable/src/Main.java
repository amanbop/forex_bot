package searchintable;

import com.fxcore2.O2GSession;
import com.fxcore2.O2GTransport;
import com.fxcore2.O2GTableManagerMode;
import com.fxcore2.O2GTableManager;
import com.fxcore2.O2GTableType;
import com.fxcore2.O2GOrdersTable; 
import com.fxcore2.O2GOrderTableRow;
import com.fxcore2.O2GTableManagerStatus;

public class Main {
      
    public static void main(String[] args) {
    
        // Connection and session variables
        String mOrderID = "";
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
            System.out.println("USAGE: [order] [user ID] [password] [URL] [connection] [session ID (if needed)] [pin (if needed)]");
            System.exit(1);
        }
        
       // Get command line arguments
        mOrderID = args[0];
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
             
        // Create a session, subscribe to session listener, login, find order and print order info, logout
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
                    O2GOrdersTable ordersTable = (O2GOrdersTable)tableManager.getTable(O2GTableType.ORDERS);                
                    TableListener tableListener = new TableListener();
                    ordersTable.subscribeStatus(tableListener);                    
                    O2GOrderTableRow order = ordersTable.findRow(mOrderID);
                    if (order != null) {
                        System.out.println("Information for OrderID = " + mOrderID + ":\n\t" +
                                        "Account = " + order.getAccountID() + "\n\t" +
                                        "Amount = " +  order.getAmount() + "\n\t" +
                                        "Rate = " + order.getRate() + "\n\t" +
                                        "Type = " + order.getType() + "\n\t" +
                                        "Buy/Sell = " + order.getBuySell() + "\n\t" +
                                        "Stage = " + order.getStage());     
                        ordersTable.unsubscribeStatus(tableListener);           
                    } else {
                        System.out.println("\nYou specified an order which does not exist.");
                    }
                }
                else
                {
                    System.out.println("\nTables refreshing filed.");
                }                    
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
}