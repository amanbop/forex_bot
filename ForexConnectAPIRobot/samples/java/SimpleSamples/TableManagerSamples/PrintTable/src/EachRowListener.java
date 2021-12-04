package printtable;

import com.fxcore2.O2GRow;
import com.fxcore2.IO2GEachRowListener;


public class EachRowListener implements IO2GEachRowListener {
    // Implementation if IO2GEachRowListener interface public method onEachRow
    public void onEachRow(String rowID, O2GRow rowData) {
        int columnsCount = rowData.getColumns().size();
        for (int i = 0; i < columnsCount; i++)
           System.out.print(rowData.getColumns().get(i).getId() + "=" + rowData.getCell(i) + "; ");
       System.out.println();
    }
}
