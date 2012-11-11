package org.kinslayermud.util;

import java.util.List;

import org.kinslayermud.character.User;
import org.kinslayermud.exception.DataInterfaceException;
import org.kinslayermud.mob.MobPrototype;
import org.kinslayermud.userlog.UserLog;
import org.kinslayermud.userlog.UserLogRecord;


public interface WebSupport {
  
  public User performLogin(String username, String password) throws DataInterfaceException;
  public void createUserSession(int userId, String sessionId) throws DataInterfaceException;
  public User getUserFromSession(String sessionId) throws DataInterfaceException;
  public void performPlayerPortalSignOut(String sessionId, int userId) throws DataInterfaceException;
  public List<UserLogRecord> getUserLogRecords(int userId) throws DataInterfaceException;
  public UserLog getUserLog(int userLogId) throws DataInterfaceException;
  public String getInstanceDomain();
  public MobPrototype getMobPrototype(int mobPrototypeId) throws DataInterfaceException;
  public List<MobPrototype> getSuperMobPrototypes() throws DataInterfaceException;
}
