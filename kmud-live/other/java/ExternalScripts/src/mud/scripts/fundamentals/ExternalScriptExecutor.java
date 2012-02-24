package mud.scripts.fundamentals;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.SQLException;
import java.util.Date;

public class ExternalScriptExecutor {

  public static Connection establishConnection() throws InstantiationException, IllegalAccessException, ClassNotFoundException, IOException, SQLException {

    String sqlFilePath = "/kinslayer/kmud-live/lib/misc/sql";
    
    BufferedReader reader = new BufferedReader(new FileReader(sqlFilePath));

    String hostname = "";
    String database = "";
    String username = "";
    String password = "";

    username = reader.readLine();
    password = reader.readLine();
    database = reader.readLine();

    Class.forName("com.mysql.jdbc.Driver").newInstance();
    return DriverManager.getConnection("jdbc:mysql://" + hostname + "/"
        + database, username, password);

  }
  public static void main(String []args) {
    
    if( args.length < 1 ) {
      
      System.out.println("Error: No script specified!");
      return;
    }
    
    String scriptClassName = args[ 0 ];
    Class scriptClass = null;
    Object scriptInstance = null;
    ExternalScriptProcess scriptProcessInstance = null;
    Connection connection = null;
    
    try { 
      scriptClass = Class.forName(scriptClassName);
    }
    catch( ClassNotFoundException exception ) {
      
      System.out.println("The specified script class was not found : " + exception);
      return;
    }
    
    try {
      scriptInstance = scriptClass.newInstance();
    }
    catch( Exception e ) {
      
      System.out.println("Could not instantiate the script : " + e);
      return;
    }
    
    if( !(scriptInstance instanceof ExternalScriptProcess) ) {
      
      System.out.println("The specified script is not an instance of ExternalScriptProcess!");
      return;
    }
    
    scriptProcessInstance = (ExternalScriptProcess)(scriptInstance);
   
    try { 
      connection = establishConnection();
    } catch( Exception e ) {
      
      System.out.println("Could not establish database connection : " + e);
      return;
    }
    
    System.out.println("Starting script at : " + new Date());
    
    scriptProcessInstance.run(connection);
    
    System.out.println("Shutting down at : " + new Date());
    
  }
}
