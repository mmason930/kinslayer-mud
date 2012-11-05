package org.kinslayermud.util;

import java.io.UnsupportedEncodingException;
import java.security.NoSuchAlgorithmException;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.Date;

import org.kinslayermud.character.User;
import org.kinslayermud.exception.NonExistentObjectException;

public class UserUtil {

  public static User performLogin(Statement statement, String username, String password) throws SQLException, NoSuchAlgorithmException, UnsupportedEncodingException {
    
    String sql = " SELECT *"
               + " FROM users"
               + " WHERE username = " + SQLUtil.escapeQuoteString(username)
               + " AND user_password = " + SQLUtil.escapeQuoteString(MiscUtil.md5(password));
    
    ResultSet resultSet = statement.executeQuery(sql);
    
    if(resultSet.next()) {
      
      return getUser(resultSet);
    }
    
    return null;
  }
  
  public static User getUser(Statement statement, int userId, boolean throwIfNotFound) throws SQLException, NonExistentObjectException {
    
    String sql = " SELECT *"
               + " FROM users"
               + " WHERE user_id = " + userId;
    
    ResultSet resultSet = statement.executeQuery(sql);
    
    if(resultSet.next()) {
      
      return getUser(resultSet);
    }
    
    if(throwIfNotFound) {
      
      throw new NonExistentObjectException("No user found. ID = " + userId);
    }
    
    return null;
  }
  
  public static User getUser(ResultSet resultSet) throws SQLException {
    
    User user = new User();
    
    user.setUserId(resultSet.getInt("user_id"));
    user.setUserName(resultSet.getString("username"));
    
    return user;
  }
  
  public static void createUserSession(Statement statement, int userId, String sessionId) throws SQLException {
    
    String sql = " INSERT INTO websiteSession("
               + "   `id`,"
               + "   `user_id`,"
               + "   `created_datetime`"
               + " ) VALUES ("
               + SQLUtil.escapeQuoteString(sessionId) + ","
               + userId + ","
               + SQLUtil.encodeQuoteDate(new Date())
               + ")";
    
    statement.executeUpdate(sql);
  }
  
  public static User getUserFromSession(Statement statement, String sessionId) throws SQLException {
    
    String sql = " SELECT"
               + "   users.*"
               + " FROM users, websiteSession"
               + " WHERE users.user_id = websiteSession.user_id"
               + " AND websiteSession.id = " + SQLUtil.escapeQuoteString(sessionId);
    
    ResultSet resultSet = statement.executeQuery(sql);
    
    if(resultSet.next()) {
      
      return getUser(resultSet);
    }
    
    return null;
  }
  
  public static void performPlayerPortalSignOut(Statement statement, String sessionId, int userId) throws SQLException {
    
    String sql = " DELETE FROM websiteSession "
               + " WHERE user_id = " + userId
               + " AND id = " + SQLUtil.escapeQuoteString(sessionId);
    
    statement.executeUpdate(sql);
  }
}
