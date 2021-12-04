package printtablelistener;

import com.fxcore2.IO2GTableManagerListener;
import com.fxcore2.O2GTableManager;
import com.fxcore2.O2GTableManagerStatus;

/**
 *
 * @author ssakhno
 */
public class TableManagerListener implements IO2GTableManagerListener 
{
    private Object obj = new Object();
    private O2GTableManagerStatus mLastStatus = O2GTableManagerStatus.TABLES_LOADING;

    // Implementation of IO2GTableManagerListener interface public method onStatusChanged
    public void onStatusChanged(O2GTableManagerStatus status, O2GTableManager manager) {
        //O2GTableManager is the main table manager when login or "Account" table manager when call getTableManagerByAccount()
        mLastStatus = status;
        switch(status) {
            case TABLES_LOADED:
                System.out.println("\nAll tables are loaded");   
                synchronized (obj)
                {
                    obj.notify();
                }
                break;
            case TABLES_LOAD_FAILED:
                System.out.println("\nTables loading failed");
                synchronized (obj)
                {
                    obj.notify();
                }
                break;
            case TABLES_LOADING:                
                break;
        }
    }
    
    public boolean WaitForTablesLoad()
    {   
        if(mLastStatus == O2GTableManagerStatus.TABLES_LOADING) 
        {
            synchronized (obj)
            {
                try 
                {
                    obj.wait(5000);
                }
                catch(InterruptedException e) 
                {
                    System.out.println("InterruptedException caught");
                } 
            }
        }
        return (mLastStatus == O2GTableManagerStatus.TABLES_LOADED);        
    }           
}
