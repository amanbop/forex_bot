package gethistprices;

import com.fxcore2.IO2GResponseListener;
import com.fxcore2.O2GResponse;

public class ResponseListener implements IO2GResponseListener {
   
    //Response , log and error variables
    private SimpleLog mSimpleLog;
    private boolean mError = false;
    private O2GResponse mResponse = null;
    
    // Gets Response
    public O2GResponse getResponse() {
        return mResponse;
    }
    
    // Shows if there was an error during the request processing
    public final boolean hasError() {
        return mError;
    }
    
    // Constructor
    public ResponseListener(SimpleLog simpleLog) {
        mSimpleLog = simpleLog;
    }
    
    // Implementation of IO2GResponseListener interface public method onRequestCompleted
    public void onRequestCompleted(String requestID, O2GResponse response) {
        mResponse = response;
        mSimpleLog.log("Request {0} completed", requestID);
        mError = false;
    }

    // Implementation of IO2GResponseListener interface public method onRequestFailed
    public void onRequestFailed(String requestID, String error) {
        mSimpleLog.log("Request {0} failed. Error = {1}", requestID, error);
        mError = true;
    }

    // Implementation of IO2GResponseListener interface public method onTablesUpdates
    public void onTablesUpdates(O2GResponse response) {
    }   
    
}