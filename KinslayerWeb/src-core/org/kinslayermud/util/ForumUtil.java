package org.kinslayermud.util;

import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;

import org.kinslayermud.exception.NonExistentObjectException;
import org.kinslayermud.forum.ForumUser;

public class ForumUtil {

  public static ForumUser getForumUser(ResultSet resultSet) throws SQLException {
    
    ForumUser forumUser = new ForumUser();
    
    forumUser.setId(resultSet.getInt("user_id"));
    forumUser.setUsername(resultSet.getString("username"));
    forumUser.setAvatarUrl(resultSet.getString("user_avatar"));
    
    return forumUser;
  }
  
  public static ForumUser getForumUserMeetingCriteria(Statement statement, String criteria, boolean throwIfNotFound) throws SQLException, NonExistentObjectException {
    
    ForumUser forumUser = null;
    
    String sql = " SELECT *"
               + " FROM phpbb_users"
               + " WHERE " + criteria;
    
    ResultSet resultSet = statement.executeQuery(sql);
    
    if(resultSet.next()) {
      
      forumUser = getForumUser(resultSet);
    }
    
    resultSet.close();
    
    if(throwIfNotFound && forumUser == null) {
      
      throw new NonExistentObjectException("No forum user found.");
    }
    
    return forumUser;
  }
  
  public static ForumUser getForumUser(Statement statement, int userId, boolean throwIfNotFound) throws SQLException, NonExistentObjectException {
    
    String criteria = "user_id = " + userId;
    return getForumUserMeetingCriteria(statement, criteria, throwIfNotFound);
  }
}
