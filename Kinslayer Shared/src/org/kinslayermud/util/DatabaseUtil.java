package org.kinslayermud.util;

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.SQLException;

public class DatabaseUtil {

  public static Connection getConnection() throws InstantiationException, IllegalAccessException, ClassNotFoundException, SQLException {
  
    String hostname = "localhost";
    String database = "livemud";
    String username = "root";
    //String password = "Car Keys Suck!";
    String password = "";
    
    
    Class.forName("com.mysql.jdbc.Driver").newInstance();
    Connection connection = DriverManager.getConnection(
      "jdbc:mysql://" + hostname + "/" + database, username, password
    );
    
    connection.setAutoCommit(false);
    return connection;
  }
  
}
