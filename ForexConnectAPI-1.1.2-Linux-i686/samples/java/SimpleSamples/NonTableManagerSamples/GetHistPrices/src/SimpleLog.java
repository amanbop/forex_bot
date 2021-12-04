package gethistprices;

import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.io.UnsupportedEncodingException;
import java.io.Writer;
import java.text.MessageFormat;

public class SimpleLog {   
    private Writer mWriter;
    private String mLineSeparator;

    // Constructor
    public SimpleLog(String sFilePath) throws FileNotFoundException {
        mLineSeparator = System.getProperty("line.separator");
        try {
            mWriter = new OutputStreamWriter(new FileOutputStream(sFilePath), "ASCII");
        } catch (UnsupportedEncodingException e) {
            e.printStackTrace();
        }
    }
    
    // Writes output to a log file
    public synchronized void log(String sLogMessage, Object ... args) {
        String sString = MessageFormat.format(sLogMessage, args);       
        System.out.println(sString);
        try {
            mWriter.write(sString);
            mWriter.write(mLineSeparator);
            mWriter.flush();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}