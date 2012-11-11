package org.kinslayermud.util;

import java.util.List;

import org.kinslayermud.character.User;
import org.kinslayermud.exception.DataInterfaceException;
import org.kinslayermud.kit.KitWithItemsAndObjectPrototypes;
import org.kinslayermud.mob.MobPrototype;
import org.kinslayermud.userlog.UserLog;
import org.kinslayermud.userlog.UserLogRecord;
import org.kinslayermud.zone.Zone;


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
  public Zone getZoneLoadingSuperMobPrototype(int mobPrototypeId) throws DataInterfaceException;
  public KitWithItemsAndObjectPrototypes getKitWithItemsAndObjectPrototypes(int kitId) throws DataInterfaceException;
}
