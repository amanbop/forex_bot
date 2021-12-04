package createoto;

import com.fxcore2.IO2GTableListener;
import com.fxcore2.O2GRow;
import com.fxcore2.O2GTableStatus;
import com.fxcore2.O2GOrderRow;

public class TableListener implements IO2GTableListener {
  
    String mRequestID = null;
    String mContingencyID = null;
    
    public void setRequest(String requestID) {
        mRequestID = requestID;
    }
    
    public String getContingency() {
        return mContingencyID;
    }
    
    // Implementation of IO2GTableListener interface public method onAdded
     public void onAdded(String rowID, O2GRow rowData) { 
            O2GOrderRow orderRow = (O2GOrderRow)(rowData);
            mContingencyID = orderRow.getContingentOrderID();
            System.out.println("Order created. Order ID is " + rowID + 
                               " ContingencyID is = " + mContingencyID);
                
    }

    // Implementation of IO2GTableListener interface public method onChanged
    public void onChanged(String rowID, O2GRow rowData) {
        O2GOrderRow orderRow = (O2GOrderRow)(rowData);
        String contingency = orderRow.getContingentOrderID();
        System.out.println("Order changed. Order ID is " + rowID + 
                           " ContingencyID is = " + contingency);
    }
    
    // Implementation of IO2GTableListener interface public method onDeleted
    public void onDeleted(String rowID, O2GRow rowData) {
    }

    // Implementation of IO2GTableListener interface public method onStatus
    public void onStatusChanged(O2GTableStatus status) {
        switch(status) {
            case REFRESHED:
                System.out.println("\nYour refresh request was successfull");
                break;
            case REFRESHING:
            case INITIAL:
            case FAILED:
               break;
        }
    }
}
