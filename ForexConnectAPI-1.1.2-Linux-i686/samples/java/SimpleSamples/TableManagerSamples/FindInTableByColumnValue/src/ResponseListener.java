package findintablebycolumnvalue;

import com.fxcore2.IO2GResponseListener;
import com.fxcore2.O2GResponse;
import com.fxcore2.O2GResponseType;

public class ResponseListener implements IO2GResponseListener {

    private boolean mError = false;
    private String mRequestID;
    private Object failSyncObj = new Object();
        
    // Shows if there was an error during the request processing
    public final boolean hasError() {
        return mError;
    }
    
    // Implementation of IO2GResponseListener interface public method onRequestCompleted
    public void onRequestCompleted(String requestID, O2GResponse response) {
        if (requestID.equals(mRequestID)) {
            System.out.println("Request completed.\nrequestID= " + requestID);
            mError = false;            
        }
    }   

    // Implementation of IO2GResponseListener interface public method onRequestFailed
    public void onRequestFailed(String requestID, String error) {            
        if (requestID.equals(mRequestID)) {
            System.out.println("Request failed.\nError= " + error);
            mError = true;  
            synchronized(failSyncObj)
            {                
                failSyncObj.notify();
            }
        }
    }

    // Implementation of IO2GResponseListener interface public method onTablesUpdates
    public void onTablesUpdates(O2GResponse response) {
        
    }

    public void setRequest(String sRequestID) {
        mRequestID = sRequestID;
        mError = false;        
    }

    public synchronized Object getFailSyncObj(){
        return failSyncObj;
    }
    
}
