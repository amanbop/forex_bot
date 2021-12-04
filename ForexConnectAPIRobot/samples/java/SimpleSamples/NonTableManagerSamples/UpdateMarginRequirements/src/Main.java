package updatemarginrequirements;

import com.fxcore2.O2GAccountRow;
import com.fxcore2.O2GAccountsTableResponseReader;
import com.fxcore2.O2GSession;
import com.fxcore2.O2GTransport;
import com.fxcore2.O2GLoginRules;
import com.fxcore2.O2GMargin;
import com.fxcore2.O2GResponseReaderFactory;
import com.fxcore2.O2GResponse;
import com.fxcore2.O2GOffersTableResponseReader;
import com.fxcore2.O2GTableType;
import com.fxcore2.O2GOfferRow;
import com.fxcore2.O2GRequestFactory;
import com.fxcore2.O2GValueMap;
import com.fxcore2.O2GRequest;
import com.fxcore2.O2GRequestParamsEnum;
import com.fxcore2.O2GTradingSettingsProvider;
import java.util.Scanner;

public class Main {
    static O2GSession mSession = null;

    public static void main(String[] args) {
        // Connection and session variables
        String mUserID = "";
        String mPassword = "";
        String mURL = "";
        String mConnection = "";
        String mDBName = "";
        String mPin = "";
        String mInstrument = "";
        String offerInstrument = "";
        Scanner scan = new Scanner (System.in);

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

        // Create a session, subscribe to session listener, login, get instrument subscription status, request instrument subsribtion/unsubsription logout
        try {
            System.out.println("Please unsubscribe from " + mInstrument);
            System.out.println("Press \"enter\" when ready");
        System.out.println("(please note that if you are using ant version 1.8.2 to run this program you may need to press \"enter\", then \"Ctrl-Z\", then \"enter\")");
            scan.nextLine();

            mSession = O2GTransport.createSession();
            SessionStatusListener statusListener = new SessionStatusListener(mSession, mDBName, mPin);
            mSession.subscribeSessionStatus(statusListener);
            ResponseListener responseListener = new ResponseListener(mSession);
            mSession.subscribeResponse(responseListener);
            mSession.login(mUserID, mPassword, mURL, mConnection);
            while (!statusListener.isConnected() && !statusListener.hasError()) {
                    Thread.sleep(50);
            }
            if (!statusListener.hasError()) {
                offerInstrument = getOfferID(mInstrument);
                if (!offerInstrument.isEmpty()) {
                    subscribeInstrument(offerInstrument, mInstrument, responseListener);
                    showMarginRequirements(mInstrument, "After subscription:");
                    updateMarginRequirements(responseListener);
                    showMarginRequirements(mInstrument, "After updating margin requirements:");
                    mSession.logout();
                    while (!statusListener.isDisconnected()) {
                        Thread.sleep(50);
                    }
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

    // Get OfferID
    public static String getOfferID(String instrument) {
       String offerID = "";
       try {
            O2GLoginRules loginRules = mSession.getLoginRules();
            if (loginRules != null && loginRules.isTableLoadedByDefault(O2GTableType.OFFERS)) {
                String instrumentFromOfferRow = "";
                O2GResponse offersResponse = loginRules.getTableRefreshResponse(O2GTableType.OFFERS);
                O2GResponseReaderFactory responseFactory = mSession.getResponseReaderFactory();
                O2GOffersTableResponseReader offersReader = responseFactory.createOffersTableReader(offersResponse);
                for (int i = 0; i < offersReader.size(); i++) {
                    O2GOfferRow offer = offersReader.getRow(i);
                    instrumentFromOfferRow = offer.getInstrument();
                    if (instrumentFromOfferRow.equals(instrument)) {
                            offerID = offer.getOfferID();
                            break;
                    }
                }

            }
       } catch (Exception e) {
            System.out.println("Exception in getOfferID().\n\t " + e.getMessage());
        }
        finally {
            return offerID;
        }
    }

    public static void subscribeInstrument (String offerInstrument, String instrument, ResponseListener responseListener) {
       try {
                O2GRequestFactory requestFactory = mSession.getRequestFactory();
                if (requestFactory != null) {
                    O2GValueMap valueMap = requestFactory.createValueMap();
                    valueMap.setString(O2GRequestParamsEnum.COMMAND, com.fxcore2.Constants.Commands.SetSubscriptionStatus);
                    valueMap.setString(O2GRequestParamsEnum.SUBSCRIPTION_STATUS, "T");
                    valueMap.setString(O2GRequestParamsEnum.OFFER_ID, offerInstrument);
                    O2GRequest request = requestFactory.createOrderRequest(valueMap);
                    mSession.sendRequest(request);
                    Thread.sleep(1000);
                    if (!responseListener.hasError()) {
                        System.out.println("You have successfully subscribed to " + instrument + " instrument.\n");
                    } else {
                        System.out.println("Subscription to " + instrument + " instrument has failed.\n");
                    }
                }
        }
       catch (Exception e) {
       }
    }

    public static void updateMarginRequirements(ResponseListener responseListener) {
       try {
                O2GRequestFactory requestFactory = mSession.getRequestFactory();
                if (requestFactory != null) {
                    O2GValueMap valueMap = requestFactory.createValueMap();
                    valueMap.setString(O2GRequestParamsEnum.COMMAND, com.fxcore2.Constants.Commands.UpdateMarginRequirements);
                    O2GRequest request = requestFactory.createOrderRequest(valueMap);
                    mSession.sendRequest(request);
                    Thread.sleep(2000);
                    if (!responseListener.hasError()) {
                        System.out.println("You have successfully updated margin requirements.\n");
                    } else {
                        System.out.println("Updating margin requirements has failed.\n");
                    }
                }
        }
       catch (Exception e) {
       }
    }

    public static void showMarginRequirements(String instrument, String text) {
        O2GLoginRules loginRules = mSession.getLoginRules();
        O2GTradingSettingsProvider tradingSetting = loginRules.getTradingSettingsProvider();
        O2GResponse accountsResponse = loginRules.getTableRefreshResponse(O2GTableType.ACCOUNTS);
        O2GResponseReaderFactory responseReaderFactory = mSession.getResponseReaderFactory();
        O2GAccountsTableResponseReader accounts = responseReaderFactory.createAccountsTableReader(accountsResponse);
        O2GAccountRow accountRow = accounts.getRow(0);
        O2GMargin margin = tradingSetting.getMargins(instrument, accountRow);
        System.out.println(text + " mmr=" + margin.getMMR() + "; emr=" + margin.getEMR() + "; lmr=" + margin.getLMR());

    }
}
