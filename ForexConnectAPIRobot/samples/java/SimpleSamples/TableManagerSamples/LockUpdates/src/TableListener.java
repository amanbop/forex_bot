package lockupdates;

import com.fxcore2.IO2GTableListener;
import com.fxcore2.O2GRow;
import com.fxcore2.O2GTableStatus;
import com.fxcore2.O2GOrderRow;

public class TableListener implements IO2GTableListener {
  
    String mRequestID = null;
    Object obj = new Object();
    boolean mOrderAdded = false;
    
    public void setRequest(String requestID ) {
        mRequestID = requestID;
        mOrderAdded = false;        
    }
    
    // Implementation of IO2GTableListener interface public method onAdded
     public void onAdded(String rowID, O2GRow rowData) {    
        O2GOrderRow orderRow = (O2GOrderRow)(rowData);
        if (mRequestID.equals(orderRow.getRequestID())) 
        {
            System.out.println("Order created. Order ID =" + orderRow.getOrderID() + "\t Status=" + orderRow.getStatus());
            synchronized(obj)
            {
                mOrderAdded = true;
                obj.notify();                
            }
            try
            {
                Thread.sleep(100); //time to allow lock updates in main thread
            }
            catch(InterruptedException e)
            {
                System.out.println("\nThread interrupted exception");
            }
        } 
    }

    // Implementation of IO2GTableListener interface public method onChanged
    public void onChanged(String rowID, O2GRow rowData) 
    {
        O2GOrderRow orderRow = (O2GOrderRow)(rowData);
        System.out.println("Order changed. Order ID =" + orderRow.getOrderID() + "\t Status=" + orderRow.getStatus());
    }
    
    // Implementation of IO2GTableListener interface public method onDeleted
    public void onDeleted(String rowID, O2GRow rowData) {
        O2GOrderRow orderRow = (O2GOrderRow)(rowData);
        System.out.println("Order deleted. Order ID =" + orderRow.getOrderID() + "\t Status=" + orderRow.getStatus());

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

    void WaitForOrderCreation() {
        synchronized(obj)
        {
            if(!mOrderAdded)
            {
                try
                {
                    obj.wait(5000);
                }
                catch(InterruptedException e)
                {
                    System.out.println("\nThread interrupted exception");
                }
            }
        }
    }
}
