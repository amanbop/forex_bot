package findintablebycolumnvalue;

import com.fxcore2.IO2GTableListener;
import com.fxcore2.O2GOrderTableRow;
import com.fxcore2.O2GRow;
import com.fxcore2.O2GTableStatus;
import com.fxcore2.O2GTableType;

public class TableListener implements IO2GTableListener {
    
    private Object insertSyncObj;
    private boolean mDone = false;
    private String mRequestID;
    private int mOrdersCount = 0;
    
    // Constructor
    public TableListener(Object syncObj)
    {
        insertSyncObj = syncObj;
    }
    
    public void setRequestID(String sRequestID)
    {
        mDone = false;
        mRequestID = sRequestID;
        mOrdersCount = 0;
    }
    
    
    // Implementation of IO2GTableListener interface public method onAdded
    public void onAdded(String rowID, O2GRow rowData) {        
        if(rowData.getTableType()==O2GTableType.ORDERS){
            O2GOrderTableRow order = (O2GOrderTableRow)rowData;
            if(order.getRequestID().equals(mRequestID)){            
                mOrdersCount++;
                if(mOrdersCount == 3){ //entry + stop + limit                
                    synchronized(insertSyncObj){
                        mDone = true;
                        insertSyncObj.notify();
                    }
                }
            }
        }
    }

    // Implementation of IO2GTableListener interface public method onChanged
    public void onChanged(String rowID, O2GRow rowData) {
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

    void waitInsertion() {
        try{
            synchronized(insertSyncObj)
            {                
                if(!mDone){
                    insertSyncObj.wait();
                }
            }            
        }
        catch(InterruptedException e)
        {
            System.out.println("Interrrupted exception occurs");
        }
    }
}
