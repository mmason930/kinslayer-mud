package org.kinslayermud.user.login;

import java.sql.SQLException;
import java.sql.Statement;
import java.util.List;

import org.kinslayermud.util.QueryUtil;

public class UserLoginUtil {
  
  public static List<UserLogin> getUserLoginsByUserId(Statement statement, int userId) throws SQLException {
    
    String criteria = " user_id = " + userId;
    return QueryUtil.retrieveDataObjectList(statement, "userLogin", criteria, UserLogin.class);
  }
}
