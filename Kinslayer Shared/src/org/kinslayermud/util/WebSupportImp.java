package org.kinslayermud.util;

import java.sql.Connection;
import java.sql.Statement;

import org.kinslayermud.character.User;
import org.kinslayermud.exception.DataInterfaceException;


public class WebSupportImp implements WebSupport {

  public User performLogin(String username, String password) throws DataInterfaceException {
    
    Connection connection = null;
    Statement statement = null;
    try {
      connection = DatabaseUtil.getConnection();
      statement = connection.createStatement();
      
      User user = UserUtil.performLogin(statement, username, password);
      
      statement.close();
      connection.commit();
      connection.close();
      
      return user;
    }
    catch(Throwable throwable) {
      
      throw new DataInterfaceException(throwable);
    }
    finally {
      
      QueryUtil.closeNoThrow(statement);
      QueryUtil.closeNoThrow(connection);
    }
  }
  
  public void createUserSession(int userId, String sessionId) throws DataInterfaceException {
    
    Connection connection = null;
    Statement statement = null;
    try {
      connection = DatabaseUtil.getConnection();
      statement = connection.createStatement();
      
      UserUtil.createUserSession(statement, userId, sessionId);
      
      statement.close();
      connection.commit();
      connection.close();
    }
    catch(Throwable throwable) {
      
      throw new DataInterfaceException(throwable);
    }
    finally {
      
      QueryUtil.closeNoThrow(statement);
      QueryUtil.closeNoThrow(connection);
    }
  }
  
  public User getUserFromSession(String sessionId) throws DataInterfaceException {
    
    Connection connection = null;
    Statement statement = null;
    try {
      connection = DatabaseUtil.getConnection();
      statement = connection.createStatement();
      
      User user = UserUtil.getUserFromSession(statement, sessionId);
      
      statement.close();
      connection.commit();
      connection.close();
      
      return user;
    }
    catch(Throwable throwable) {
      
      throw new DataInterfaceException(throwable);
    }
    finally {
      
      QueryUtil.closeNoThrow(statement);
      QueryUtil.closeNoThrow(connection);
    }
  }
}
