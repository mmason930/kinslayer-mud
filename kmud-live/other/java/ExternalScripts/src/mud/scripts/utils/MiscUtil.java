package mud.scripts.utils;

import java.sql.Connection;
import java.sql.Statement;

public class MiscUtil {
  
  public static void closeNoThrow(Connection connection) {
    
    try {
      connection.close();
    }
    catch( Exception e ) {
      
    }
  }
  public static void closeNoThrow(Statement statement) {
    try {
      statement.close();
    }
    catch(Exception e) {
      
    }
  }

}
