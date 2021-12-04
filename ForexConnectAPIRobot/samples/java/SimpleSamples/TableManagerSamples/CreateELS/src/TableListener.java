package createels;

import com.fxcore2.IO2GTableListener;
import com.fxcore2.O2GRow;
import com.fxcore2.O2GTableStatus;
import com.fxcore2.O2GOrderRow;

public class TableListener implements IO2GTableListener {
    
    String mChangeOrderID = null;
    public String getOrderID() {
        return mChangeOrderID;
    }
    
    // Implementation of IO2GTableListener interface public method onAdded
     public void onAdded(String rowID, O2GRow rowData) { 
        O2GOrderRow orderRow = (O2GOrderRow)(rowData);
        System.out.println("Order created. Order ID is " + rowID);
        if ((orderRow.getContingencyType() == 3) && (orderRow.getType().trim().equals("LE"))) {
            mChangeOrderID = orderRow.getOrderID();
        }
    }

    // Implementation of IO2GTableListener interface public method onChanged
    public void onChanged(String rowID, O2GRow rowData) {
        System.out.println("Order changed. Order ID is " + rowID);
    }
    
    // Implementation of IO2GTableListener interface public method onDeleted
    public void onDeleted(String rowID, O2GRow rowData) {
        System.out.println("Order deleted. Order ID is " + rowID);
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
