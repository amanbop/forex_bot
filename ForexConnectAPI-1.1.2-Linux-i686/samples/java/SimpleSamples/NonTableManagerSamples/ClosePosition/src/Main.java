package closeposition;

import com.fxcore2.O2GSession;
import com.fxcore2.O2GTransport;
import com.fxcore2.O2GResponseReaderFactory;
import com.fxcore2.O2GResponse;
import com.fxcore2.O2GTableType;
import com.fxcore2.O2GRequestFactory;
import com.fxcore2.O2GValueMap;
import com.fxcore2.O2GRequest;
import com.fxcore2.O2GRequestParamsEnum;
import com.fxcore2.O2GTradesTableResponseReader;
import com.fxcore2.O2GTradeRow;
import com.fxcore2.O2GLoginRules;
import com.fxcore2.O2GPermissionChecker;
import com.fxcore2.O2GPermissionStatus;

public class Main {
    
    //Position variables    
    static String mTradeID = "";
    static int mAmount = 0;
    static String mOfferID = "";
    static String mAccountID = "";
    static String mBuySell = "";
    static boolean mHasTrade = false;
    
    
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
        if (args.length < 5) {
            System.out.println("Not Enough Parameters!");
            System.out.println("USAGE: [trade] [user ID] [password] [URL] [connection] [session ID (if needed)] [pin (if needed)]");
            System.exit(1);
        }
        
       // Get command line arguments
        mTradeID = args[0];
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
        
        
        // Create a session, subscribe to session listener, login, check if position exists, close position, logout
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
                //getTradeInfo(mSession, mTradeID);
                O2GRequestFactory factory = mSession.getRequestFactory();
                if (factory != null) {
                    mSession.sendRequest(factory.createRefreshTableRequest(O2GTableType.TRADES));
                    Thread.sleep(1000);
                }
                O2GResponse tradesResponse = responseListener.getResponse();
                O2GResponseReaderFactory responseFactory = mSession.getResponseReaderFactory();
                O2GTradesTableResponseReader tradesReader = responseFactory.createTradesTableReader(tradesResponse);
                O2GLoginRules loginRules = mSession.getLoginRules();
                O2GPermissionChecker permissionChecker = loginRules.getPermissionChecker();
                System.out.println("\nThis is a list of your open positions: \n");
                for (int i = 0; i < tradesReader.size(); i++) {
                    O2GTradeRow trade = tradesReader.getRow(i);
                    System.out.println("TradeID = " + trade.getTradeID());
                    if (mTradeID.equals(trade.getTradeID())) {
                        mOfferID = trade.getOfferID();
                        mAmount = trade.getAmount();
                        mAccountID = trade.getAccountID();
                        if (("B").equals(trade.getBuySell())) {
                            mBuySell = "S";
                        } else {
                            mBuySell = "B";
                        }   
                        mHasTrade = true;
                    } 
                } 
                if (mHasTrade) {
                    O2GRequestFactory requestFactory = mSession.getRequestFactory();
                    if (requestFactory != null) {
                        if(permissionChecker.canCreateMarketCloseOrder(mOfferID) == O2GPermissionStatus.PERMISSION_ENABLED)
                        {
                            O2GValueMap valueMap = requestFactory.createValueMap();
                            valueMap.setString(O2GRequestParamsEnum.COMMAND, "CreateOrder");
                            valueMap.setString(O2GRequestParamsEnum.ORDER_TYPE, "CM"); 
                            valueMap.setString(O2GRequestParamsEnum.TRADE_ID, mTradeID);
                            valueMap.setString(O2GRequestParamsEnum.ACCOUNT_ID, mAccountID); 
                            valueMap.setString(O2GRequestParamsEnum.OFFER_ID, mOfferID); 
                            valueMap.setString(O2GRequestParamsEnum.BUY_SELL, mBuySell); 
                            valueMap.setInt(O2GRequestParamsEnum.AMOUNT, mAmount); 
                            valueMap.setString(O2GRequestParamsEnum.CUSTOM_ID, "CloseMarketOrder"); 
                            O2GRequest request = requestFactory.createOrderRequest(valueMap);
                            if(request == null)
                            {
                                System.out.println(requestFactory.getLastError());
                            }
                            else
                            {
                                mSession.sendRequest(request);
                                Thread.sleep(1000);
                                if (!responseListener.hasError()) {
                                    System.out.println("You have successfully closed a position " + mTradeID);
                                } else {
                                    System.out.println("Your request to close a position " + mTradeID + " has failed.\n");
                                }
                            }
                        }
                        else
                        {
                            O2GValueMap valueMap = requestFactory.createValueMap();
                            valueMap.setString(O2GRequestParamsEnum.COMMAND, "CreateOrder");
                            valueMap.setString(O2GRequestParamsEnum.ORDER_TYPE, "OM"); // in USA you need to use "OM" to close a position.
                            valueMap.setString(O2GRequestParamsEnum.ACCOUNT_ID, mAccountID); 
                            valueMap.setString(O2GRequestParamsEnum.OFFER_ID, mOfferID); 
                            valueMap.setString(O2GRequestParamsEnum.BUY_SELL, mBuySell); 
                            valueMap.setInt(O2GRequestParamsEnum.AMOUNT, mAmount); 
                            valueMap.setString(O2GRequestParamsEnum.CUSTOM_ID, "TrueMarketOrder"); 
                            O2GRequest request = requestFactory.createOrderRequest(valueMap);
                            mSession.sendRequest(request);
                            Thread.sleep(1000);
                            if (!responseListener.hasError()) {
                                System.out.println("You have successfully closed a position " + mTradeID);
                            } else {
                                System.out.println("Your request to close a position " + mTradeID + " has failed.\n");
                            }
                        }
                    }
                }
                else {
                    System.out.println("\nYou specified an invalid position. No action will be taken.");
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
}