package printtable;

import com.fxcore2.O2GSession;
import com.fxcore2.O2GTransport;
import com.fxcore2.O2GTableManagerMode;
import com.fxcore2.O2GTableManager;
import com.fxcore2.O2GTableType;
import com.fxcore2.O2GTable;
import com.fxcore2.O2GOffersTable;
import com.fxcore2.O2GOfferRow;
import com.fxcore2.O2GTableManagerStatus;
import com.fxcore2.O2GTradesTable;
import com.fxcore2.O2GTradeRow;

public class Main {
    
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
        if (args.length < 4) {
            System.out.println("Not Enough Parameters!");
            System.out.println("USAGE: [user ID] [password] [URL] [connection] [session ID (if needed)] [pin (if needed)]");
            System.exit(1);
        }
        
        // Get command line arguments
        mUserID = args[0];
        mPassword = args[1];
        mURL = args[2];
        mConnection = args[3];
        if (args.length > 4) {
            mDBName = args[4];
        }
        if (args.length > 5) {
            mPin = args[5];
        }
        
        // Create a session, subscribe to session listener, login, prin Offers and Trades tables, logout
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
                    O2GOffersTable offersTable = (O2GOffersTable)tableManager.getTable(O2GTableType.OFFERS);
                    getOffers(offersTable);
                    O2GTradesTable tradesTable = (O2GTradesTable)tableManager.getTable(O2GTableType.TRADES);
                    getTrades(tradesTable);

                    printTable(tableManager.getTable(O2GTableType.SUMMARY));
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

    // Print table using IO2GEachRowListener
    public static void printTable(O2GTable table) {
        if (table.size() == 0)
            System.out.println("Table is empty!");
        else
            table.forEachRow(new EachRowListener());
        }
    
    // Print Offers Table
    public static void getOffers(O2GOffersTable table) {        
        TableListener tableListener = new TableListener();
        table.subscribeStatus(tableListener);
        System.out.println("\nPrinting Offers Table\n");
        for (int i = 0; i < table.size(); i++) {
                    O2GOfferRow offer = table.getRow(i);
                    System.out.println("Instrument = " + offer.getInstrument() +
                                       " Bid Price = " +  offer.getBid() +
                                       " Ask Price = " + offer.getAsk());              
        }        
        table.unsubscribeStatus(tableListener);
    }
    
    // Print Trades Table
    public static void getTrades(O2GTradesTable table) {
        try {         
            TableListener tableListener = new TableListener();
            table.subscribeStatus(tableListener);
            System.out.println("\nPrinting Trades Table\n");
            for (int i = 0; i < table.size(); i++) {
                O2GTradeRow trade = table.getRow(i);
                System.out.println("TradeID = " + trade.getTradeID() +
                                   " BuySell = " +  trade.getBuySell() +
                                   " Amount = " + trade.getAmount());              
            }            
            table.unsubscribeStatus(tableListener);
        } catch (Exception e) {
            System.out.println("Exception in getTrades().\n\t " + e.getMessage());
        }                 
    }
    
}
