package org.kinslayermud.userlog;

import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.LinkedList;
import java.util.List;

import org.kinslayermud.exception.NonExistentObjectException;

public class UserLogUtil {

  public static List<UserLogRecord> getUserLogRecords(Statement statement, int userId) throws SQLException {
    
    List<UserLogRecord> userLogRecords = new LinkedList<UserLogRecord>();
    String sql = " SELECT"
               + "   `id`,"
               + "   `record_date`"
               + " FROM userLog"
               + " WHERE user_id = " + userId
               + " ORDER BY record_date DESC";
    
    ResultSet resultSet = statement.executeQuery(sql);
    
    while(resultSet.next()) {
      
      userLogRecords.add(getUserLogRecord(resultSet, userId));
    }
    
    return userLogRecords;
  }
  
  public static UserLogRecord getUserLogRecord(ResultSet resultSet, Integer userId) throws SQLException {
    
    UserLogRecord userLogRecord = new UserLogRecord();
    
    userLogRecord.setId(resultSet.getInt("id"));
    userLogRecord.setRecordDate(resultSet.getTimestamp("record_date"));
    
    if(userId == null) {
      userLogRecord.setUserId(resultSet.getInt("user_id"));
    }
    else {
      userLogRecord.setUserId(userId);
    }
    
    return userLogRecord;
  }
  
  public static UserLog getUserLog(Statement statement, int userLogId, boolean throwIfNotFound) throws SQLException, NonExistentObjectException {
    
    String sql = " SELECT *"
               + " FROM userLog"
               + " WHERE id = " + userLogId;
    
    ResultSet resultSet = statement.executeQuery(sql);
    
    if(resultSet.next()) {
      
      return getUserLog(resultSet);
    }
    
    if(throwIfNotFound) {
      
      throw new NonExistentObjectException("No user log found. ID = " + userLogId);
    }
    
    return null;
  }
  
  public static UserLog getUserLog(ResultSet resultSet) throws SQLException {
    
    UserLog userLog = new UserLog();
    
    userLog.setId(resultSet.getInt("id"));
    userLog.setUserId(resultSet.getInt("user_id"));
    userLog.setConsoleOutput(resultSet.getString("console_output"));
    userLog.setRecordDate(resultSet.getTimestamp("record_date"));
    
    return userLog;
  }
}
