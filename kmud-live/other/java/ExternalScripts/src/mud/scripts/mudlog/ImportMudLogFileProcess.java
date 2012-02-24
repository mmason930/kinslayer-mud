package mud.scripts.mudlog;

import java.io.BufferedReader;
import java.io.FileReader;
import java.sql.Connection;
import java.sql.Statement;
import java.util.Date;
import java.util.regex.Pattern;

import mud.scripts.fundamentals.ExternalScriptProcess;
import mud.scripts.utils.BatchInsertStatement;
import mud.scripts.utils.MiscUtil;

public class ImportMudLogFileProcess implements ExternalScriptProcess {

  Connection connection;
  Statement statement;
  
  private static String FILE_PATH = "/kinslayer/MudLogToImport";
  
  public static Date parseDateNewFormat( String timeStamp ) {
   
    Date date = null;
    
    return date;
    
  }
  public static Date parseDateOldFormat( String timeStamp ) {

    Date date = null;
    
    return date; 
    
  }
  public static Date parseDateFromMudLogMessage( String mudLogMessage ) {
    
    Pattern oldFormatPattern = Pattern.compile("Mon Dec 26 00:3 :: (.*?)");
    Pattern newFormatPattern = Pattern.compile("");
    
    return null;
    
  }
  
  public void run(Connection connection) {
    
    try {
     
      this.connection = connection;
      this.statement = connection.createStatement();
      
      BatchInsertStatement batchInsertStatement = new BatchInsertStatement(connection, "mudlog", 10000);

      batchInsertStatement.addField("messageDateTime");
      batchInsertStatement.addField("messageText");
      
      batchInsertStatement.start();
      
      BufferedReader bufferedReader = new BufferedReader(new FileReader(FILE_PATH));
      
      while( bufferedReader.ready() ) {
        
        String mudLogMessage = bufferedReader.readLine();
        
        Date messageTimeStamp = parseDateFromMudLogMessage( mudLogMessage );
        
        batchInsertStatement.beginEntry();
        
        batchInsertStatement.putDate( new java.sql.Date(messageTimeStamp.getTime()) );
        batchInsertStatement.putString(mudLogMessage);
        
        batchInsertStatement.endEntry();
        
      }
      
      batchInsertStatement.flush();
      
      batchInsertStatement.finish();
      
    }
    catch( Throwable t ) {
      
      t.printStackTrace();
    }
    finally {
      
      MiscUtil.closeNoThrow(statement);
      MiscUtil.closeNoThrow(connection);
    }
  }
}
