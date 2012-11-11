package org.kinslayermud.util;

import java.sql.Connection;
import java.sql.Statement;
import java.util.List;

import org.kinslayermud.character.User;
import org.kinslayermud.exception.DataInterfaceException;
import org.kinslayermud.misc.Provider;
import org.kinslayermud.mob.MobPrototype;
import org.kinslayermud.mob.MobUtil;
import org.kinslayermud.userlog.UserLog;
import org.kinslayermud.userlog.UserLogRecord;
import org.kinslayermud.userlog.UserLogUtil;
import org.kinslayermud.zone.Zone;
import org.kinslayermud.zone.ZoneUtil;


public class WebSupportImp implements WebSupport {

  protected Provider provider;
  public WebSupportImp() {
    
    provider = new Provider();
  }
  
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
  
  public void performPlayerPortalSignOut(String sessionId, int userId) throws DataInterfaceException {
    
    Connection connection = null;
    Statement statement = null;
    try {
      connection = DatabaseUtil.getConnection();
      statement = connection.createStatement();
      
      UserUtil.performPlayerPortalSignOut(statement, sessionId, userId);
      
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
  

  public List<UserLogRecord> getUserLogRecords(int userId) throws DataInterfaceException {
    
    Connection connection = null;
    Statement statement = null;
    try {
      connection = DatabaseUtil.getConnection();
      statement = connection.createStatement();
      
      List<UserLogRecord> userLogRecords = UserLogUtil.getUserLogRecords(statement, userId);
      
      statement.close();
      connection.commit();
      connection.close();
      
      return userLogRecords;
    }
    catch(Throwable throwable) {
      
      throw new DataInterfaceException(throwable);
    }
    finally {
      
      QueryUtil.closeNoThrow(statement);
      QueryUtil.closeNoThrow(connection);
    }
  }
  
  public UserLog getUserLog(int userLogId) throws DataInterfaceException {
    
    Connection connection = null;
    Statement statement = null;
    try {
      connection = DatabaseUtil.getConnection();
      statement = connection.createStatement();
      
      UserLog userLog = UserLogUtil.getUserLog(statement, userLogId, false);
      
      statement.close();
      connection.commit();
      connection.close();
      
      return userLog;
    }
    catch(Throwable throwable) {
      
      throw new DataInterfaceException(throwable);
    }
    finally {
      
      QueryUtil.closeNoThrow(statement);
      QueryUtil.closeNoThrow(connection);
    }
  }
  
  public String getInstanceDomain() {
    
    return provider.getInstanceDomain();
  }

  public MobPrototype getMobPrototype(int mobPrototypeId) throws DataInterfaceException {
    
    Connection connection = null;
    Statement statement = null;
    try {
      connection = DatabaseUtil.getConnection();
      statement = connection.createStatement();
      
      MobPrototype mobPrototype = MobUtil.getMobPrototype(statement, mobPrototypeId, false);
      
      statement.close();
      connection.commit();
      connection.close();
      
      return mobPrototype;
    }
    catch(Throwable throwable) {
      
      throw new DataInterfaceException(throwable);
    }
    finally {
      
      QueryUtil.closeNoThrow(statement);
      QueryUtil.closeNoThrow(connection);
    }
  }
  
  public List<MobPrototype> getSuperMobPrototypes() throws DataInterfaceException {
    
    Connection connection = null;
    Statement statement = null;
    try {
      connection = DatabaseUtil.getConnection();
      statement = connection.createStatement();
      
      List<MobPrototype> mobPrototypes = MobUtil.getSuperMobPrototypes(statement);
      
      statement.close();
      connection.commit();
      connection.close();
      
      return mobPrototypes;
    }
    catch(Throwable throwable) {
      
      throw new DataInterfaceException(throwable);
    }
    finally {
      
      QueryUtil.closeNoThrow(statement);
      QueryUtil.closeNoThrow(connection);
    }
  }
  
  public Zone getZoneLoadingSuperMobPrototype(int mobPrototypeId) throws DataInterfaceException {
    
    Connection connection = null;
    Statement statement = null;
    try {
      connection = DatabaseUtil.getConnection();
      statement = connection.createStatement();
      
      Zone zone = ZoneUtil.getZoneLoadingSuperMobPrototype(statement, mobPrototypeId, false);
      
      statement.close();
      connection.commit();
      connection.close();
      
      return zone;
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
