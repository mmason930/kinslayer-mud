package org.kinslayermud.scripts.cleanup;

import java.sql.Connection;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.LinkedList;
import java.util.List;

import org.kinslayermud.misc.Provider;
import org.kinslayermud.scripts.KinslayerProcess;
import org.kinslayermud.userlog.UserLog;
import org.kinslayermud.userlog.UserLogUtil;
import org.kinslayermud.util.QueryUtil;

public class CleanupDirtyLogsProcess implements KinslayerProcess {

  protected static final int FETCH_SIZE = 100;
  
  public void process(Provider provider) {
    
    Connection connection = null;
    Statement statement = null;
    
    try {
      connection = provider.getConnection();
      statement = connection.createStatement();

      System.out.println("CLEANUP PROCESS!");
      int minimumId = 1;
      int maxTableId = getMaxTableId(statement);
      
      System.out.println("Max Table ID: " + maxTableId);
      
      /***
      while(minimumId <= maxTableId) {
        
        List<UserLog> userLogs = getUserLogs(statement, minimumId);
        
        for(UserLog userLog : userLogs) {
          
          System.out.println("Processing User Log #" + userLog.getId() + "...");
          boolean isChanged = false;
          StringBuffer stringBuffer = new StringBuffer(userLog.getConsoleOutput());
          int readIndex = 0;
          while(true) {
            
            int jsonStartIndex = stringBuffer.indexOf("{\"", readIndex);
            int jsonEndIndex;
            if(jsonStartIndex == -1) {
              
              break;
            }
            
            //Attempt to find the closing bracket.
            int closingBracketsNeeded = 1;
            for(jsonEndIndex = jsonStartIndex + 2;jsonEndIndex < stringBuffer.length();++jsonEndIndex) {
              
              if(stringBuffer.charAt(jsonEndIndex) == '{') {
                ++closingBracketsNeeded;
              }
              else if(stringBuffer.charAt(jsonEndIndex) == '}') {
                --closingBracketsNeeded;
                
                if(closingBracketsNeeded == 0) {
                  break;
                }
              }
            }
//            if(jsonEndIndex + 1 < stringBuffer.length() && )
            
//          System.out.println("Deleting From " + jsonStartIndex + " to " + jsonEndIndex + ". Total Buffer Size: " + stringBuffer.length());
            readIndex = jsonStartIndex;
            
            if(closingBracketsNeeded == 0) {
              
              if(charAt(stringBuffer, jsonEndIndex + 1, '\n'))
                jsonEndIndex += 1;
              
              isChanged = true;
              stringBuffer.delete(jsonStartIndex, jsonEndIndex + 1);
            }
            else {
              
              System.out.println(" - Not enough closing brackets found. Remaining: " + closingBracketsNeeded);
            }
          }
          
          if(isChanged) {
            
            userLog.setConsoleOutput(stringBuffer.toString());
            UserLogUtil.putUserLog(statement, userLog);
          }
        }
        
        minimumId += FETCH_SIZE;
      }
      ***/
      
      connection.commit();
      statement.close();
      connection.close();
    }
    catch(Throwable throwable) {
      
      throwable.printStackTrace();
      QueryUtil.closeNoThrow(statement);
      QueryUtil.closeNoThrow(connection);
    }
  }
  
  public static boolean charAt(StringBuffer stringBuffer, int index, char ch) {
    
    if(index >= 0 && index < stringBuffer.length())
      return ch == stringBuffer.charAt(index);
    
    return false;
  }
  
  public static int getMaxTableId(Statement statement) throws SQLException {
    
    String sql = " SELECT MAX(id) FROM userLog";
    return QueryUtil.getSingleIntValueResult(statement, sql);
  }
  
  public static List<UserLog> getUserLogs(Statement statement, int minimumId) throws SQLException {
    
    List<UserLog> userLogs = new LinkedList<UserLog>();
    String sql = " SELECT *"
               + " FROM userLog"
               + " WHERE id = 1382";
//               + " WHERE id >= " + minimumId
//               + " AND id < " + (minimumId + FETCH_SIZE);
    
    ResultSet resultSet = statement.executeQuery(sql);
    
    while(resultSet.next()) {
      
      userLogs.add(UserLogUtil.getUserLog(resultSet));
    }
    
    return userLogs;
  }
}
