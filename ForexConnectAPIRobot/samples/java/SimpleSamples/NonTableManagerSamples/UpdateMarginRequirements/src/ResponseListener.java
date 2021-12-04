package updatemarginrequirements;

import com.fxcore2.IO2GResponseListener;
import com.fxcore2.O2GResponse;
import com.fxcore2.O2GResponseReaderFactory;
import com.fxcore2.O2GResponseType;
import com.fxcore2.O2GSession;

public class ResponseListener implements IO2GResponseListener {
    private O2GSession mSession;
    public ResponseListener(O2GSession session)
    {
        mSession = session;
    }
    private boolean mError = false;
    
    // Shows if there was an error during the request processing
    public final boolean hasError() {
        return mError;
    }
    
    // Implementation of IO2GResponseListener interface public method onRequestCompleted
    public void onRequestCompleted(String requestID, O2GResponse response) {
        System.out.println("Request completed.\nrequestID= " + requestID);
        mError = false;
        O2GResponseType responseType = response.getType();
        if (responseType == O2GResponseType.MARGIN_REQUIREMENTS_RESPONSE){
              O2GResponseReaderFactory responseFactory = mSession.getResponseReaderFactory();
              responseFactory.processMarginRequirementsResponse(response);
        }
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
