package org.kinslayermud.user;

import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;

import org.kinslayermud.exception.NonExistentObjectException;
import org.kinslayermud.util.QueryUtil;
import org.kinslayermud.util.SQLUtil;

public class UserUtil {

  public static User getUserMeetingCriteria(Statement statement, String criteria, boolean throwIfNotFound) throws SQLException, NonExistentObjectException {
    
    String sql = " SELECT *"
               + " FROM user"
               + " WHERE " + criteria;
    
    ResultSet resultSet = statement.executeQuery(sql);
    
    if(resultSet.next()) {
      
      User user = new User();
      
      fillInBaseFields(resultSet, user);
      
      return user;
    }
    
    if(!throwIfNotFound) {
      
      throw new NonExistentObjectException("User not found. Criteria: " + criteria);
    }
    
    return null;
  }
  
  public static void fillInBaseFields(ResultSet resultSet, User user) throws SQLException {
    
    user.setId(resultSet.getInt("id"));
    user.setPassword(resultSet.getString("password"));
    user.setEmailAddress(resultSet.getString("email_address"));
    user.setCreatedDatetime(resultSet.getTimestamp("created_datetime"));
  }
  
  public static void putUser(Statement statement, User user) throws SQLException {
    
    String sql;
    
    if(user.isNew()) {
      
      sql = " INSERT INTO user("
          + "   `password`,"
          + "   `email_address`,"
          + "   `created_datetime`"
          + " ) VALUES("
          + SQLUtil.escapeQuoteString(user.getPassword()) + ","
          + SQLUtil.escapeQuoteString(user.getEmailAddress()) + ","
          + SQLUtil.encodeQuoteDate(user.getCreatedDatetime()) + ")";
      
      statement.executeUpdate(sql);
      
      user.setId(QueryUtil.getLastInsertedID(statement));
    }
    else {
      
      sql = " UPDATE user SET"
          + "   password = " + SQLUtil.escapeQuoteString(user.getPassword()) + ","
          + "   email_address = " + SQLUtil.escapeQuoteString(user.getEmailAddress()) + ","
          + "   created_datetime = " + SQLUtil.encodeQuoteDate(user.getCreatedDatetime())
          + " WHERE id = " + user.getId();
      
      statement.executeUpdate(sql);
    }
  }
}
