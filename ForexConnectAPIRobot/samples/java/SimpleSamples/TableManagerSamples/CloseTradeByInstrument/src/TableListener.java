package closetradebyinstrument;

import com.fxcore2.IO2GTableListener;
import com.fxcore2.O2GRow;
import com.fxcore2.O2GTableStatus;
import com.fxcore2.O2GOrderRow;

public class TableListener implements IO2GTableListener {
  
    String mRequestID = null;
    O2GTableStatus mStatus = null;
    
    public void setRequest(String requestID ) {
        mRequestID = requestID;
    }
    
    public O2GTableStatus getStatus() {
        return mStatus;
    }
    
    // Implementation of IO2GTableListener interface public method onAdded
     public void onAdded(String rowID, O2GRow rowData) {    
        O2GOrderRow orderRow = (O2GOrderRow)(rowData);
        if (mRequestID.equals(orderRow.getRequestID())) {
            System.out.println("Order created. Order ID is " +orderRow.getOrderID());
       } 
    }

    // Implementation of IO2GTableListener interface public method onChanged
    public void onChanged(String rowID, O2GRow rowData) {
    }
    
    // Implementation of IO2GTableListener interface public method onDeleted
    public void onDeleted(String rowID, O2GRow rowData) {
        System.out.println("You have successfully closed position " + rowID);
    }

    // Implementation of IO2GTableListener interface public method onStatus
    public void onStatusChanged(O2GTableStatus status) {
        mStatus = status;
        switch(status) {
            case REFRESHED:
                System.out.println("\nYour refresh request was successful");
                break;
            case REFRESHING:
            case INITIAL:
            case FAILED:
               break;
        }
    }
}
