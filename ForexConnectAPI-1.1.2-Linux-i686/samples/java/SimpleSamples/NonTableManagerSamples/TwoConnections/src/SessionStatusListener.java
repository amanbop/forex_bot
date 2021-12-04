package twoconnections;

import com.fxcore2.IO2GSessionStatus;
import com.fxcore2.O2GSession;
import com.fxcore2.O2GSessionStatusCode;
import com.fxcore2.O2GSessionDescriptorCollection;
import com.fxcore2.O2GSessionDescriptor;

public class SessionStatusListener implements IO2GSessionStatus {

    // Connection , session and status variables
    private boolean mConnected = false;
    private boolean mDisconnected = false;
    private boolean mError = false;
    private String mDBName = "";
    private String mPin = "";
    private O2GSession mSession = null;
    private O2GSessionStatusCode mStatus = null;

    // Constructor
    public SessionStatusListener(O2GSession session) {
        mSession = session;
    }
    
    //Shows if session is connected
    public final boolean isConnected() {
        return mConnected;
    }
    
    //Shows if session is disconnected
    public final boolean isDisconnected() {
        return mDisconnected;
    }
    
    // Shows if there was an error during the login process
    public final boolean hasError() {
        return mError;
    }
    
    // Returns current session status
    public final O2GSessionStatusCode getStatus() {
        return mStatus;
    }
    
    // Implementation of IO2GSessionStatus interface public method onSessionStatusChanged
    public final void onSessionStatusChanged(O2GSessionStatusCode status) {
        mStatus = status;
        System.out.println("(For Session " + mSession + ") Status: " + mStatus.toString());
        if (mStatus == O2GSessionStatusCode.CONNECTED) {
            mConnected = true;
        }
        else {
            mConnected = false;
        }
        if (status == O2GSessionStatusCode.DISCONNECTED) {
            mDisconnected = true;
        }
        else {
            mDisconnected = false;
        }       
    }
    
    // Implementation of IO2GSessionStatus interface public method onLoginFailed
    public final void onLoginFailed(String error) {
        System.out.println("Login error: " + error);
        mError = true;
    }
}
