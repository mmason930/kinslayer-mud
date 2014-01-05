package org.kinslayermud.misc;

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.SQLException;
import java.util.Properties;

import org.kinslayermud.util.MiscUtil;

public class Provider {

  protected String instanceDomain;
  protected String mysqlUrl;
  protected String mysqlUsername;
  protected String mysqlPassword;
  protected int playerPortalServerPort;
  
  protected Connection connection;
  
  public int getPlayerPortalServerPort() {
    
    return playerPortalServerPort;
  }
  
  public String getInstanceDomain() {
    
    return instanceDomain;
  }
  
  public Connection getConnection() throws InstantiationException, IllegalAccessException, ClassNotFoundException, SQLException {
    
    Class.forName("com.mysql.jdbc.Driver").newInstance();
    Connection connection = DriverManager.getConnection
    (
      mysqlUrl,
      mysqlUsername,
      mysqlPassword
    );
    
    connection.setAutoCommit(false);
    return connection;
  }
  
  public void loadConfiguration(String configurationFilePath) throws Exception {
    
    Properties properties = MiscUtil.loadPropertiesResource(configurationFilePath);
    
    instanceDomain = properties.getProperty("InstanceDomain");
    mysqlUrl = properties.getProperty("Mysql.Main.Url");
    mysqlUsername = properties.getProperty("Mysql.Main.Username");
    mysqlPassword = properties.getProperty("Mysql.Main.Password");
    playerPortalServerPort = Integer.valueOf(properties.getProperty("PlayerPortalServerPort"));
  }
}