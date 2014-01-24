package org.kinslayermud.user.login;

import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.LinkedList;
import java.util.List;

public class UserLoginUtil {

  public static UserLogin getUserLogin(ResultSet resultSet) throws SQLException {
  
    UserLogin userLogin = new UserLogin();

    userLogin.setId(resultSet.getInt("id"));
    userLogin.setUserId(resultSet.getInt("user_id"));
    userLogin.setHost(resultSet.getString("host"));
    userLogin.setLoginDatetime(resultSet.getTimestamp("login_datetime"));
    userLogin.setGatewayDescriptorType(GatewayDescriptorType.getEnum(resultSet.getInt("gateway_descriptor_type")));

    return userLogin;
  }
  
  public static List<UserLogin> getUserLoginsMeetingCriteria(Statement statement, String criteria) throws SQLException {
    
    List<UserLogin> userLogin = new LinkedList<UserLogin>();
    String sql = " SELECT *"
               + " FROM userLogin"
               + " WHERE " + criteria;
    
    ResultSet resultSet = statement.executeQuery(sql);
    
    while(resultSet.next()) {
      
      userLogin.add(getUserLogin(resultSet));
    }
    
    return userLogin;
  }
  
  public static List<UserLogin> getUserLoginsByUserId(Statement statement, int userId) throws SQLException {
    
    String criteria = " user_id = " + userId;
    return getUserLoginsMeetingCriteria(statement, criteria);
  }
}
