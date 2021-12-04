package closeposition;

import com.fxcore2.IO2GResponseListener;
import com.fxcore2.O2GResponse;
import com.fxcore2.O2GResponseType;

public class ResponseListener implements IO2GResponseListener {

    private boolean mError = false;
    private O2GResponse mResponse;
    
    public O2GResponse getResponse() {
        return mResponse;
    }
    
    // Shows if there was an error during the request processing
    public final boolean hasError() {
        return mError;
    }
    
    // Implementation of IO2GResponseListener interface public method onRequestCompleted
    public void onRequestCompleted(String requestID, O2GResponse response) {
        if (response.getType() == O2GResponseType.GET_TRADES) {
            mResponse = response;
        } else {
            System.out.println("Request completed.\nrequestID= " + requestID);
        }
        mError = false;
    }

    // Implementation of IO2GResponseListener interface public method onRequestFailed
    public void onRequestFailed(String requestID, String error) {
        System.out.println("Request failed.\nrequestID= " + requestID + "; error= " + error);
        mError = true;
    }

    // Implementation of IO2GResponseListener interface public method onTablesUpdates
    public void onTablesUpdates(O2GResponse response) {
        
    }   
    
}
