package gethistprices;

import com.fxcore2.O2GSession;
import com.fxcore2.O2GTransport;
import com.fxcore2.O2GRequest;
import com.fxcore2.O2GResponseReaderFactory;
import com.fxcore2.O2GResponse;
import com.fxcore2.O2GTableType;
import com.fxcore2.O2GRequestFactory;
import com.fxcore2.O2GTimeframeCollection;
import com.fxcore2.O2GTimeframe;
import com.fxcore2.O2GMarketDataSnapshotResponseReader;
import com.fxcore2.O2GTimeConverter;
import com.fxcore2.O2GTimeConverterTimeZone;
import java.io.FileNotFoundException;
import java.util.Date;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.Calendar;

public class Main {

    // Historic prices variables
    static Calendar calFrom = null;
    static Calendar calTo = null;
    static Calendar calFirst = null;
    static Calendar calDate = null;
    static String mInstrument = "";
    static int mMaxBars = 300;
    
    public static void main(String[] args) {
    
        // Connection, session and log variables
        String mUserID = "";
        String mPassword = "";
        String mURL = "";
        String mConnection = "";
        String mDBName = "";
        String mPin = "";
        O2GSession mSession = null;
        SimpleLog mSimpleLog = null;
        
        // Market Data request variables
        O2GRequest request = null;
        String mTimeFrame = "";
        String mDateFrom = "";
        String mDateTo = "";
        boolean mContinue = true;
        int mReaderSize = 0;
        int mCounter = 0;
        int mBorder = 0;
        
        
        // Check for correct number of arguments
        if (args.length < 8) {
            System.out.println("Not Enough Parameters!");
            System.out.println("USAGE: [instrument] [timeframe] [datefrom] [dateto] [user ID] [password] [URL] [connection] [session ID (if needed)] [pin (if needed)]");
            System.exit(1);
        }
        
        // Establish date format
        DateFormat df = new SimpleDateFormat("yyyy-MM-dd hh:mm:ss");
        
        // Get command line arguments
        mInstrument = args[0];
        mTimeFrame = args[1];
        mDateFrom = args[2].trim();
        if ((!mDateFrom.equals("")) && (!mDateFrom.equals("{DATEFROM}"))) {
            try {
                Date dtFrom = df.parse(mDateFrom);
                calFrom = Calendar.getInstance();
                calFrom.setTime(dtFrom);
            } catch (Exception e) {
                System.out.println(" Date From format invalid.");
                System.exit(1);
            }
        }                  
        mDateTo = args[3].trim();
        if ((!mDateTo.equals("")) && (!mDateTo.equals("{DATETO}"))) {
            try {
                Date dtTo = df.parse(mDateTo);
                calTo = Calendar.getInstance();
                calTo.setTime(dtTo);
            } catch (Exception e) {
                System.out.println(" Date To format invalid.");
                System.exit(1);
            }
        }                       
        mUserID = args[4];
        mPassword = args[5];
        mURL = args[6];
        mConnection = args[7];
        if (args.length > 8) {
            mDBName = args[8];
        }
        if (args.length > 9) {
            mPin = args[9];
        }
        
        // Open log for writing info
        try {
            mSimpleLog = new SimpleLog(String.format("%s.log", mUserID));
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        }
        
        // Create a session, subscribe to session listener, login, get historic prices, logout
        try {
            mSession = O2GTransport.createSession();
            SessionStatusListener statusListener = new SessionStatusListener(mSession, mSimpleLog, mDBName, mPin);
            mSession.subscribeSessionStatus(statusListener);
            ResponseListener responseListener = new ResponseListener(mSimpleLog);
            mSession.subscribeResponse(responseListener);
            mSession.login(mUserID, mPassword, mURL, mConnection);
            while (!statusListener.isConnected() && !statusListener.hasError()) {
                    Thread.sleep(50);
            }
            if (!statusListener.hasError()) {
                O2GRequestFactory requestFactory = mSession.getRequestFactory();
                O2GTimeframeCollection timeFrames = requestFactory.getTimeFrameCollection();
                O2GTimeframe timeFrame = timeFrames.get(mTimeFrame);
                if (timeFrame == null) {
                   System.out.println("You specified an invalid time frame.");
                   mContinue = false;
                }
                if (mContinue) {
                    request = requestFactory.createMarketDataSnapshotRequestInstrument(mInstrument, timeFrame, mMaxBars);
                    if (request == null) {
                       System.out.println("Cannot create request for market data snapshot.");
                       mContinue = false;
                    }
                }
                if (mContinue) {
                    do {
                        requestFactory.fillMarketDataSnapshotRequestTime(request, calFrom, calTo, false);
                        mSession.sendRequest(request);
                        Thread.sleep(1000);
                        O2GResponse response = responseListener.getResponse();
                        if (response != null){
                            O2GResponseReaderFactory responseFactory = mSession.getResponseReaderFactory();
                            O2GMarketDataSnapshotResponseReader reader = responseFactory.createMarketDataSnapshotReader(response);
                            mReaderSize = reader.size();
                            // check if we need additional request
                            if ((calFrom == null) || (calTo == null) || (mReaderSize < mMaxBars)) {
                                mBorder = 0;
                            } else {
                                if (mCounter > 0) {
                                   if (mTimeFrame.equals("m1")) {
                                        mBorder = 1;
                                    }
                                }                               
                            }
                            O2GTimeConverter converter = mSession.getTimeConverter();                           
                            for (int i = mReaderSize - 1; i >= 0; i--) {
                                calDate = reader.getDate(i);    
                                calDate = converter.convert(calDate, O2GTimeConverterTimeZone.LOCAL);
                                if (i <= mReaderSize - 1 - mBorder) {
                                    mSimpleLog.log("Date = {0}, BidOpen = {1}, BidHigh = {2}, BidLow = {3}, BidClose = {4}, " + 
                                                   "AskOpen = {5}, AskHigh = {6}, AskLow = {7}, AskClose = {8}", calDate.getTime(), 
                                                   reader.getBidOpen(i), reader.getBidHigh(i), reader.getBidLow(i), reader.getBidClose(i),
                                                   reader.getAskOpen(i), reader.getAskHigh(i), reader.getAskLow(i), reader.getAskClose(i));
                                }
                                calFirst = reader.getDate(0);                                                                     
                            }
                            mCounter++;
                        }
                        if (calFrom == null){
                            break;
                        }
                        if (mReaderSize < mMaxBars) {
                           break;
                        }
                        if (calFrom.before(calFirst)) {
                           calTo = calFirst;
                        }
                    } while (calFrom.before(calFirst));                     
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