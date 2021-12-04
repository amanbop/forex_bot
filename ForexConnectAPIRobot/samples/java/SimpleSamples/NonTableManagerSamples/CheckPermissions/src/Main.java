package checkpermissions;

import com.fxcore2.O2GSession;
import com.fxcore2.O2GTransport;
import com.fxcore2.O2GPermissionChecker;
import com.fxcore2.O2GLoginRules;

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
        String mInstrument = ""; 
        
        // Check for correct number of arguments
        if (args.length < 5) {
            System.out.println("Not Enough Parameters!");
            System.out.println("USAGE: [instrument] [user ID] [password] [URL] [connection] [session ID (if needed)] [pin (if needed)]");
            System.exit(1);
        }
        
        // Get command line arguments
        mInstrument = args[0];
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
        
        // Create a session, subscribe to session listener, login, get permissions, logout
        try {
            mSession = O2GTransport.createSession();
            SessionStatusListener statusListener = new SessionStatusListener(mSession, mDBName, mPin);
            mSession.subscribeSessionStatus(statusListener);
            mSession.login(mUserID, mPassword, mURL, mConnection);
            while (!statusListener.isConnected() && !statusListener.hasError()) {
                    Thread.sleep(50);
            }
            if (!statusListener.hasError()) {
                checkPermissions(mSession, mInstrument);
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
    
    // Check permissions for specified trading instrument
    public static void checkPermissions(O2GSession session, String sInstrument) {
        O2GLoginRules loginRules = session.getLoginRules();
        O2GPermissionChecker permissionChecker = loginRules.getPermissionChecker();
        System.out.println("canCreateMarketOpenOrder = "  + permissionChecker.canCreateMarketOpenOrder(sInstrument));
        System.out.println("canChangeMarketOpenOrder = "  + permissionChecker.canChangeMarketOpenOrder(sInstrument));
        System.out.println("canDeleteMarketOpenOrder = "  + permissionChecker.canDeleteMarketOpenOrder(sInstrument));            
        System.out.println("canCreateMarketCloseOrder = "  + permissionChecker.canCreateMarketCloseOrder(sInstrument));
        System.out.println("canChangeMarketCloseOrder = "  + permissionChecker.canChangeMarketCloseOrder(sInstrument));
        System.out.println("canDeleteMarketCloseOrder = "  + permissionChecker.canDeleteMarketCloseOrder(sInstrument));
        System.out.println("canCreateEntryOrder = "  + permissionChecker.canCreateEntryOrder(sInstrument));
        System.out.println("canChangeEntryOrder = "  + permissionChecker.canChangeEntryOrder(sInstrument));
        System.out.println("canDeleteEntryOrder = "  + permissionChecker.canDeleteEntryOrder(sInstrument));
        System.out.println("canCreateStopLimitOrder = "  + permissionChecker.canCreateStopLimitOrder(sInstrument));
        System.out.println("canChangeStopLimitOrder = "  + permissionChecker.canChangeStopLimitOrder(sInstrument));
        System.out.println("canDeleteStopLimitOrder = "  + permissionChecker.canDeleteStopLimitOrder(sInstrument));
        System.out.println("canRequestQuote = "  + permissionChecker.canRequestQuote(sInstrument));
        System.out.println("canAcceptQuote = "  + permissionChecker.canAcceptQuote(sInstrument));
        System.out.println("canDeleteQuote = "  + permissionChecker.canDeleteQuote(sInstrument));
        System.out.println("canJoinToNewContingencyGroup = "  + permissionChecker.canJoinToNewContingencyGroup(sInstrument));
        System.out.println("canJoinToExistingContingencyGroup = "  + permissionChecker.canJoinToExistingContingencyGroup(sInstrument));
        System.out.println("canRemoveFromContingencyGroup = "  + permissionChecker.canRemoveFromContingencyGroup(sInstrument));
        System.out.println("canChangeOfferSubscription = "  + permissionChecker.canChangeOfferSubscription(sInstrument));
        System.out.println("canCreateNetCloseOrder = "  + permissionChecker.canCreateNetCloseOrder(sInstrument));
        System.out.println("canChangeNetCloseOrder = "  + permissionChecker.canChangeNetCloseOrder(sInstrument));
        System.out.println("canDeleteNetCloseOrder = "  + permissionChecker.canDeleteNetCloseOrder(sInstrument));
        System.out.println("canCreateNetStopLimitOrder = "  + permissionChecker.canCreateNetStopLimitOrder(sInstrument));
        System.out.println("canChangeNetStopLimitOrder = "  + permissionChecker.canChangeNetStopLimitOrder(sInstrument));
        System.out.println("canDeleteNetStopLimitOrder = "  + permissionChecker.canDeleteNetStopLimitOrder(sInstrument));
        System.out.println("canUseDynamicTrailingForStop = "  + permissionChecker.canUseDynamicTrailingForStop());
        System.out.println("canUseDynamicTrailingForLimit = "  + permissionChecker.canUseDynamicTrailingForLimit());
        System.out.println("canUseDynamicTrailingForEntryStop = "  + permissionChecker.canUseDynamicTrailingForEntryStop());
        System.out.println("canUseDynamicTrailingForEntryLimit = "  + permissionChecker.canUseDynamicTrailingForEntryLimit());
        System.out.println("canUseFluctuateTrailingForStop = "  + permissionChecker.canUseFluctuateTrailingForStop());
        System.out.println("canUseFluctuateTrailingForLimit = "  + permissionChecker.canUseFluctuateTrailingForLimit());            
        System.out.println("canUseFluctuateTrailingForEntryStop = "  + permissionChecker.canUseFluctuateTrailingForEntryStop());
        System.out.println("canUseFluctuateTrailingForEntryLimit = "  + permissionChecker.canUseFluctuateTrailingForEntryLimit());
    }

}