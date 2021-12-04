package getReport;

import java.util.Calendar;
import java.net.URL;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import com.fxcore2.O2GSession;
import com.fxcore2.O2GTransport;
import com.fxcore2.O2GLoginRules;
import com.fxcore2.O2GResponseReaderFactory;
import com.fxcore2.O2GResponse;
import com.fxcore2.O2GAccountsTableResponseReader;
import com.fxcore2.O2GTableType;
import com.fxcore2.O2GAccountRow;

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

        // Create a session, subscribe to session listener, login, get accounts, logout
        try {
            mSession = O2GTransport.createSession();
            SessionStatusListener statusListener = new SessionStatusListener(mSession, mDBName, mPin);
            mSession.subscribeSessionStatus(statusListener);
            mSession.login(mUserID, mPassword, mURL, mConnection);
            while (!statusListener.isConnected() && !statusListener.hasError()) {
                    Thread.sleep(50);
            }
            if (!statusListener.hasError()) {
                getAccounts(mSession);
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

    // Get accounts information
    public static void getAccounts(O2GSession session) {
        try {
            O2GLoginRules loginRules = session.getLoginRules();
            if (loginRules != null && loginRules.isTableLoadedByDefault(O2GTableType.ACCOUNTS)) {
                O2GResponse accountsResponse = loginRules.getTableRefreshResponse(O2GTableType.ACCOUNTS);
                O2GResponseReaderFactory responseFactory = session.getResponseReaderFactory();
                O2GAccountsTableResponseReader accountsReader = responseFactory.createAccountsTableReader(accountsResponse);
                for (int i = 0; i < accountsReader.size(); i++) {
                    O2GAccountRow account = accountsReader.getRow(i);
                    String urlString = session.getReportURL(account, null, null, "html", "", "", 0);
                    System.out.println("AccountID = " + account.getAccountID() +
                                       " Balance = " +  account.getBalance() +
                                       " UsedMargin = " + account.getUsedMargin() +
                                       " Report URL = " + urlString);
                    URL url = new URL(urlString);
                    InputStream stream = null;
                    try{
                        stream = url.openStream();
                        FileOutputStream writer = null;
                        try
                        {
                            writer = new FileOutputStream(account.getAccountID() + ".html");
                            byte[] buffer = new byte[262144];
                            int bytesRead = 0;

                            while ((bytesRead = stream.read(buffer, 0, 262144)) > 0)
                            {  
                                writer.write(buffer, 0, bytesRead);
                               }
                        } finally {
                            if (writer != null)
                                writer.close();
                        }
                    } catch (IOException ioe) {
                        System.out.println("Unable to download an error.");
                    } finally {
                        try {
                            if (stream != null)
                                stream.close();
                        } catch (IOException ioe) {
                        }
                     }
                }
            }
        } catch (Exception e) {
            System.out.println("Exception in getAccounts():\n\t " + e.getMessage());
        }
    }
}
